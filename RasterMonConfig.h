//
// Created by Maurik Holtrop on 5/2/22.
//

#ifndef RASTERMON_RASTERMONCONFIG_H
#define RASTERMON_RASTERMONCONFIG_H
#include "RasterEvioTool.h"
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
#include "RasterHists.h"

class RasterMonGui;

class RasterMonConfig {

   RQ_OBJECT("RasterMonConfig");

public:
   const TGWindow *fParentWindow;
   RasterEvioTool *fEvio = nullptr;
   RasterHists *fHists = nullptr;
   RasterMonGui *fRasGui = nullptr;

   TGTransientFrame *fConfigDialog;
   TGNumberEntry *fNumberEntryRate;
   TGNumberEntry *fNumberEntryScopeBufDepth;

   unsigned int fRefreshRate = 500;
   unsigned int fScopeBufDepth = 10000;

public:
   RasterMonConfig(const TGWindow *parent_window, RasterEvioTool *evio, RasterHists *hists): fEvio(evio), fHists(hists) {
      // Handle the user input for configurable settings of the program.
   };
   void Run();
   void OK(){
      cout << "OK\n";
      fConfigDialog->CloseWindow();
      fRefreshRate = fNumberEntryRate->GetIntNumber();
   };
   void Cancel(){
      cout << "Cancel\n";
      fConfigDialog->CloseWindow();
   };
   void CloseWindow(){ Cancel(); };

   void UpdateADCBufDepth(){
      double rnum = fNumberEntryScopeBufDepth->GetNumber();
      unsigned long bufsize = (unsigned long)(rnum*1000.);
      fHists->fPause = true;  // Pause the worker threads.
      if(fEvio){
         fEvio->UpdateBufferSize(bufsize);
      }
      if(fHists){
         fHists->ResizeScopeGraphs(bufsize);
      }
      fHists->fPause = false;
   }

};

inline void RasterMonConfig::Run(){
   // Run the main bit of gui code.
   int label_width = 100;
   int field_width = 200;
   fConfigDialog = new TGTransientFrame(gClient->GetRoot(), fParentWindow, 400, 200);
   fConfigDialog->SetWindowName("RasterMon Configure Dialog");
   fConfigDialog->Connect("CloseWindow()", "RasterMonConfig", this, "CloseWindow()");
   fConfigDialog->DontCallClose(); // to avoid double deletions.
   // use hierarchical cleaning
   fConfigDialog->SetCleanup(kDeepCleanup);
   // Bottom buttons:  Cancel, OK.
   auto Frame1 = new TGHorizontalFrame(fConfigDialog, 400, 20, kFixedWidth);
   auto OkButton = new TGTextButton(Frame1, "&Ok", 1);
   OkButton->Connect("Clicked()", "RasterMonConfig", this, "OK()");
   auto CancelButton = new TGTextButton(Frame1, "&Cancel", 2);
   CancelButton->Connect("Clicked()", "RasterMonConfig", this, "Cancel()");
   auto L1 = new TGLayoutHints(kLHintsTop | kLHintsRight , //| kLHintsExpandX,
                               2, 2, 2, 2);
   auto L2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);
   Frame1->AddFrame(OkButton, L1);
   Frame1->AddFrame(CancelButton, L1);
   Frame1->Resize(400, OkButton->GetDefaultHeight());
   fConfigDialog->AddFrame(Frame1, L2);
   auto Frame2 = new TGVerticalFrame(fConfigDialog, 350, 250, kFixedWidth | kFixedHeight);

   auto *h_frame0 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame0, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   auto label0 = new TGLabel(h_frame0,"For numeric entry, use shift key for x10, crtl key for x100");
   h_frame0->AddFrame(label0,new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   label0->Resize(label_width + field_width, 25);

   auto *h_frame1 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame1, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   auto label1 = new TGLabel(h_frame1,"Window Refresh rate [ms]:");
   h_frame1->AddFrame(label1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 35, 5, 5));
   label1->Resize(label_width, 25);

   fNumberEntryRate = new TGNumberEntry(h_frame1, fRefreshRate, 5, 1, TGNumberFormat::kNESInteger,
                                        TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMin, 50, 10000);
   h_frame1->AddFrame(fNumberEntryRate, new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsCenterY ,
                                                          10, 10, 5, 5));
   fNumberEntryRate->Resize(field_width, 25);
   if(fRasGui){
      fNumberEntryRate->Connect("ValueSet(Long_t)","RasterMonGui",fRasGui,"SetUpdateRate()");
   }

   auto *h_frame2 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame2, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   auto label2 = new TGLabel(h_frame2,"Scope event buffer depth:");\
   label2->Resize(label_width, 20);
   h_frame2->AddFrame(label2, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 20, 5, 5));

   fNumberEntryScopeBufDepth = new TGNumberEntry(h_frame2, (double)(fScopeBufDepth/1000.), 10, 1,
                                                 TGNumberFormat::kNESRealOne,TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMinMax , 0.2, 1000.0);

   // fNumberEntryScopeBufDepth->Resize( 60, 20);
   h_frame2->AddFrame(fNumberEntryScopeBufDepth, new TGLayoutHints(kLHintsCenterX | kLHintsExpandX | kLHintsCenterY ,
                                                          1, 1, 5, 5));
   auto label2tail = new TGLabel(h_frame2,"x1k evt.");
   label2tail->Resize(9, 20);
   h_frame2->AddFrame(label2tail, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 2, 1, 5, 5));

   fNumberEntryScopeBufDepth->Connect("ValueSet(Long_t)","RasterMonConfig",this,"UpdateADCBufDepth()");


   fConfigDialog->AddFrame(Frame2, new TGLayoutHints(kLHintsTop));
   fConfigDialog->MapSubwindows();

   UInt_t width = fConfigDialog->GetDefaultWidth();
   UInt_t height = fConfigDialog->GetDefaultHeight();
   fConfigDialog->Resize(width, height);
   fConfigDialog->CenterOnParent();
   // make the message box non-resizable
   fConfigDialog->SetWMSize(width, height);
   fConfigDialog->SetWMSizeHints(width, height, width, height, 0, 0);
   fConfigDialog->MapWindow();

}

#endif //RASTERMON_RASTERMONCONFIG_H
