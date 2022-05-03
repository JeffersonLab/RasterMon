//
// Created by Maurik Holtrop on 5/2/22.
//

#ifndef RASTERMON_ETCONNECTIONCONFIG_H
#define RASTERMON_ETCONNECTIONCONFIG_H
#include "TObject.h"
#include "TGCanvas.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TGNumberEntry.h"
#include "TGComboBox.h"
#include <RQ_OBJECT.h>

#include <iostream>
#include <string>
#include <map>

#include "RasterEvioTool.h"

class ETConnectionConfig {

   RQ_OBJECT("ETConnectionConfig");

public:
   const TGWindow *fParentWindow;
   RasterEvioTool *fEvio;
   TGTransientFrame *fEtDialog;
   TGComboBox *fPredefCombo;
   TGTextEntry *fTextEntryStationName;
   TGTextEntry *fTextEntryHostName;
   TGNumberEntry *fNumberEntryPort;
   TGTextEntry *fTextEntryBufName;

   bool fIsOK=false;

   std::map<std::string, std::string> fETSysKnown = {
         {"clondaq2", "129.57.167.109"}, // 1
         {"clondaq3", "129.57.167.226"}, // 2
         {"clondaq4", "129.57.167.227"}, // 3
         {"clondaq5", "129.57.167.41"},  // 4
         {"clondaq6", "129.57.167.60"},  // 5
         {"clondaq7", "129.57.167.20"},  // 6
         {"localhost", "127.0.0.1"}      // 7
   };

   // clondaq2  - 129.57.167.109
   // clondaq3  - 129.57.167.226
   // clondaq4  - 129.57.167.227
   // clondaq5  - 129.57.167.41
   // clondaq6  - 129.57.167.60
   // clondaq7  - 129.57.167.20


public:
   ETConnectionConfig(const TGWindow *parent_window, RasterEvioTool *evio): fParentWindow(parent_window), fEvio(evio) {
      // Handle the user input for making the ET ring "work".
      // See the tutorial/gui/guitest.C class TestDialog for examples on building dialogs.
   };
   virtual ~ETConnectionConfig(){};

   void Run();

   void Reconnect(){
      std::cout << "ETDialog Reconnect\n";
      fEvio->Close();
      OK();
   }

   void OK(){
      fEtDialog->CloseWindow();
      fIsOK = true;
      fEvio->fETStationName = fTextEntryStationName->GetText();
      fEvio->fETHost = HostNameToIP(fTextEntryHostName->GetText());
      fEvio->fETPort = fNumberEntryPort->GetIntNumber();
      fEvio->fETName = fTextEntryBufName->GetText();
      cout << "Set ET system to: \n";
      cout << "Name: " << fEvio->fETStationName << endl;
      cout << "Host: " << fEvio->fETHost << endl;
      cout << "Port: " << fEvio->fETPort << endl;
      cout << "Buff: " << fEvio->fETName << endl;
      if(fEvio) {
         fEvio->OpenEt(fEvio->fETStationName, fEvio->fETName, fEvio->fETHost, fEvio->fETPort);
      }
   }

   void Cancel(){
      std::cout << "ETDialog Cancel\n";
      fEvio->Close();
      fEtDialog->CloseWindow();
   }

   string HostNameToIP(string name){
      if(fETSysKnown.find(name) != fETSysKnown.end()){
         return fETSysKnown[name];
      }else return name;
   }

   int HostNameToChoice(string name){
      // This is NOT foolproof, since the map could mix the order, but I expect it will just work.
      if(fETSysKnown.find(name) != fETSysKnown.end()){
         int i = 0;
         for(const auto &[key,val]: fETSysKnown){
            if( name == key){
               cout << "Found :" << name << " = " << key << endl;
               return i;
            }else{
               ++i;
            }
         }
      }
      return -1;
   }

   void CloseWindow(){
      std::cout << "ETDialog CloseWindow\n";
      Cancel();
   }

   void HandleETDefault(char *selection){
      // This sets defaults according to the selection.
      if(fETSysKnown.find(selection) != fETSysKnown.end() ){
         fEvio->fETHost = fETSysKnown[selection];
         fTextEntryHostName->SetText(fEvio->fETHost.c_str());
         fEvio->fETPort = ET_DEFAULT_PORT;
         fNumberEntryPort->SetIntNumber(fEvio->fETPort);
         fEvio->fETName = ET_DEFAULT_NAME;
         fTextEntryBufName->SetText(fEvio->fETName.c_str());
      }
   }
};

inline void ETConnectionConfig::Run(){
   // Pop up the window and process user input.
   int label_width = 100;
   int field_width = 200;
   fEtDialog = new TGTransientFrame(gClient->GetRoot(), fParentWindow, 400, 200);
   fEtDialog->SetWindowName("ET Configure Dialog");
   fEtDialog->Connect("CloseWindow()", "ETConnectionConfig", this, "CloseWindow()");
   fEtDialog->DontCallClose(); // to avoid double deletions.
   // use hierarchical cleaning
   fEtDialog->SetCleanup(kDeepCleanup);

   // Bottom buttons:  Reconnect, Cancal, OK.
   auto Frame1 = new TGHorizontalFrame(fEtDialog, 400, 20, kFixedWidth);
   auto OkButton = new TGTextButton(Frame1, "&Ok", 1);
   OkButton->Connect("Clicked()", "ETConnectionConfig", this, "OK()");
   auto CancelButton = new TGTextButton(Frame1, "&Cancel", 2);
   CancelButton->Connect("Clicked()", "ETConnectionConfig", this, "Cancel()");
   auto ReconnectButton = new TGTextButton(Frame1, "&Reconnect", 3);
   ReconnectButton->Connect("Clicked()", "ETConnectionConfig", this, "Reconnect()");
   auto L1 = new TGLayoutHints(kLHintsTop | kLHintsRight , //| kLHintsExpandX,
                               2, 2, 2, 2);
   auto L2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

   Frame1->AddFrame(OkButton, L1);
   Frame1->AddFrame(CancelButton, L1);
   Frame1->AddFrame(ReconnectButton, L1);
   Frame1->Resize(400, OkButton->GetDefaultHeight());
   fEtDialog->AddFrame(Frame1, L2);

   auto Frame2 = new TGVerticalFrame(fEtDialog, 350, 250, kFixedWidth | kFixedHeight);

   auto *h_frame2 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame2, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   auto label2 = new TGLabel(h_frame2,"Select Default:      ");
   h_frame2->AddFrame(label2, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 35, 5, 5));
   label2->Resize(label_width, 25);
   fPredefCombo = new TGComboBox(h_frame2);
   h_frame2->AddFrame(fPredefCombo, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsCenterY ));
   fPredefCombo->Resize(field_width-100, 25);
   std::map<string, string>::iterator it;
   int i = 0;
   fPredefCombo->AddEntry("          ", i++);
   for(it = fETSysKnown.begin(); it!= fETSysKnown.end(); ++it){
      // cout << "Adding " << it->first << " - " << it->second << endl;
      fPredefCombo->AddEntry(it->first.c_str(), i++);
   }
   int select = HostNameToChoice(fEvio->fETHost) + 1;
   if(select >=0 ) fPredefCombo->Select(select, kFALSE);
   fPredefCombo->Connect("Selected(const char *)","ETConnectionConfig", this, "HandleETDefault(char *)");

   auto *h_frame1 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame1, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 10, 2, 20, 2));
   auto label1 = new TGLabel(h_frame1, "ET Station Name:    ");
   h_frame1->AddFrame(label1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 5, 5));
   label1->Resize(label_width, 25);
   fTextEntryStationName = new TGTextEntry(h_frame1);
   h_frame1->AddFrame(fTextEntryStationName, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsCenterY , 10, 10, 5, 5));
   fTextEntryStationName->Resize(field_width, 25);
   fTextEntryStationName->SetText(fEvio->fETStationName.c_str());
   fTextEntryStationName->SetTextColor(gROOT->GetColor(kGreen + 2));

   auto *h_frame3 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame3, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 10, 2, 20, 2));
   auto label3 = new TGLabel(h_frame3, "ET Host Name or ip: ");
   h_frame3->AddFrame(label3, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 5, 5));
   label3->Resize(label_width, 25);
   fTextEntryHostName = new TGTextEntry(h_frame3);
   h_frame3->AddFrame(fTextEntryHostName, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsCenterY , 10, 10, 5, 5));
   fTextEntryHostName->Resize(field_width, 25);
   fTextEntryHostName->SetText(fEvio->fETHost.c_str());
   fTextEntryHostName->SetTextColor(gROOT->GetColor(kGreen + 2));

   auto *h_frame4 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame4, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 10, 2, 20, 2));
   auto label4 = new TGLabel(h_frame4, "ET Port:            ");
   h_frame4->AddFrame(label4, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 35, 5, 5));
   label4->Resize(label_width, 25);
   fNumberEntryPort = new TGNumberEntry(h_frame4, fEvio->fETPort, 5, 1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
   h_frame4->AddFrame(fNumberEntryPort, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsCenterY , 10, 10, 5, 5));
   fNumberEntryPort->Resize(field_width, 25);
   fNumberEntryPort->SetIntNumber(fEvio->fETPort);
   
   auto *h_frame5 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame5, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 10, 2, 20, 2));
   auto label5 = new TGLabel(h_frame5, "ET Buffer name:     ");
   h_frame5->AddFrame(label5, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 5, 5));
   label5->Resize(label_width, 25);
   fTextEntryBufName = new TGTextEntry(h_frame5);
   h_frame5->AddFrame(fTextEntryBufName, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsCenterY , 10, 10, 5, 5));
   fTextEntryBufName->Resize(field_width, 25);
   fTextEntryBufName->SetText(fEvio->fETName.c_str());
   fTextEntryBufName->SetTextColor(gROOT->GetColor(kGreen + 2));

   fEtDialog->AddFrame(Frame2, new TGLayoutHints(kLHintsTop));
   fEtDialog->MapSubwindows();

   UInt_t width = fEtDialog->GetDefaultWidth();
   UInt_t height = fEtDialog->GetDefaultHeight();
   fEtDialog->Resize(width, height);
   fEtDialog->CenterOnParent();
   // make the message box non-resizable
   fEtDialog->SetWMSize(width, height);
   fEtDialog->SetWMSizeHints(width, height, width, height, 0, 0);
   fEtDialog->MapWindow();

}



#endif //RASTERMON_ETCONNECTIONCONFIG_H
