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

   TGListBox *fDebugLevelChooser;
   TGNumberEntry *fNumberEntryRate;
   TGNumberEntry *fNumberEntryScopeBufDepth;

   unsigned int fRefreshRate = 500;
   unsigned int fScopeBufDepth = 10000;

   double fScale_x = 0.004;
   double fOffset_x = -8;
   double fScale_y = 0.004;
   double fOffset_y = -8;

public:
   RasterMonConfig(const TGWindow *parent_window, RasterEvioTool *evio, RasterHists *hists): fParentWindow(parent_window), fEvio(evio), fHists(hists) {
      // Handle the user input for configurable settings of the program.
   };
   void Run();
   void OK(){
      cout << "OK - debug set to : " << fDebugLevelChooser->GetSelected() << "\n";
      fConfigDialog->CloseWindow();
      fRefreshRate = fNumberEntryRate->GetIntNumber();
   };
   void Cancel(){
      cout << "Cancel\n";
      fConfigDialog->CloseWindow();
   };
   void CloseWindow(){ Cancel(); };

   void DebugSelection(int level){
      std::cout << "Debug changed to level " << level << std::endl;
      fHists->fDebug = level;

   }

   void GetScaleOffset(){
      // Get the X and Y scale offsets for the raster hists.
      // Find histograms Raster_xy and get the constants from them. Raster_x, Raster_y should have same constants!
      for(auto &h_t: fHists->fHists){
         if( strncmp(h_t.hist->GetName(),"Raster_xy", 8) == 0){
            fScale_x = h_t.scale_x;
            fOffset_x = h_t.offset_x;
            fScale_y = h_t.scale_y;
            fOffset_y = h_t.offset_y;
         }
      }
   }

   void SetScaleOffset(){
      // Get the X and Y scale offsets for the raster hists.
      // Find histograms Raster_x, Raster_y, Raster_xy and get the constants on them.
      for(auto &h_t: fHists->fHists){
         if( strncmp(h_t.hist->GetName(),"Raster_xy", 8) == 0){
            h_t.scale_x = fScale_x;
            h_t.offset_x = fOffset_x;
            h_t.scale_y = fScale_y;
            h_t.offset_y = fOffset_y;
         }
         if( strncmp(h_t.hist->GetName(),"Raster_x", 8) == 0){
            h_t.scale_x = fScale_x;
            h_t.offset_x = fOffset_x;
         }
         if( strncmp(h_t.hist->GetName(),"Raster_y", 8) == 0){
            h_t.scale_y = fScale_y;
            h_t.offset_y = fOffset_y;
         }
      }
   }


   void ValueSet(){
      // Handle value changed in the Raster conversion section
      TGNumberEntry *nmbr = (TGNumberEntry*) gTQSender;
      int id = nmbr->WidgetId();
      if( id == 3 ) fScale_x = nmbr->GetNumber();
      if( id == 4 ) fOffset_x = nmbr->GetNumber();
      if( id == 5 ) fScale_y = nmbr->GetNumber();
      if( id == 6 ) fOffset_y = nmbr->GetNumber();
      if(3 <= id && id <= 6) SetScaleOffset();
   }

   void HandleButtons() {
      // Handle different buttons.

      TGButton *btn = (TGButton *) gTQSender;
      int id = btn->WidgetId();
      if( id>0 && id < 5) {    // EVIO Debug buttons.
         int bits = fEvio->fDebug;
         if (btn->GetState()) {
            bits = bits | (1 << (id - 1));
         } else {
            bits = bits & ~(1 << (id - 1));
         }
//      std::bitset<8> b(bits);
//      printf("DoButton: id = %d IsSelected: %d -> ", id, btn->GetState());
//      cout << b << endl;
         fEvio->fDebug = bits;
      }
   }

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
   int frame_width = 360;
   int frame_height = 400;
   int label_width = 100;
   int field_width = 200;
   fConfigDialog = new TGTransientFrame(gClient->GetRoot(), fParentWindow, 400, 200);
   fConfigDialog->SetWindowName("RasterMon Configure Dialog");
   fConfigDialog->Connect("CloseWindow()", "RasterMonConfig", this, "CloseWindow()");
   fConfigDialog->DontCallClose(); // to avoid double deletions.
   // use hierarchical cleaning
   fConfigDialog->SetCleanup(kDeepCleanup);
   // Bottom buttons:  Cancel, OK.
   auto Frame1 = new TGHorizontalFrame(fConfigDialog, frame_width, 20, kFixedWidth);

   // ------------- Bottom button row -------------
   auto OkButton = new TGTextButton(Frame1, "&Close", 1);
   OkButton->Connect("Clicked()", "RasterMonConfig", this, "OK()");
//   auto CancelButton = new TGTextButton(Frame1, "&Cancel", 2);
//   CancelButton->Connect("Clicked()", "RasterMonConfig", this, "Cancel()");
   auto L1 = new TGLayoutHints(kLHintsTop | kLHintsRight , //| kLHintsExpandX,
                               2, 10, 2, 4);
   auto L2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);
   Frame1->AddFrame(OkButton, L1);
 //  Frame1->AddFrame(CancelButton, L1);
   Frame1->Resize(400, OkButton->GetDefaultHeight());

   fConfigDialog->AddFrame(Frame1, L2);

   // -------------- Main Dialog Frame --------------

   // Vertically stacked items, i.e. followed by TGHorizontalFrames for filling the rows.
   auto Frame2 = new TGVerticalFrame(fConfigDialog, frame_width, frame_height, kFixedWidth | kFixedHeight);

   auto *h_frame0 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame0, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   auto label0 = new TGLabel(h_frame0,"For numeric entry, use shift key for x10, crtl key for x100");
   h_frame0->AddFrame(label0,new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   label0->Resize(label_width + field_width, 25);

   // ********** Debug Level Set  RasterMon ***********
   auto *h_frame_debug_level = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame_debug_level, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
   auto label_debug_level = new TGLabel(h_frame_debug_level,"RasterMon Gui Debug Level:");
   h_frame_debug_level->AddFrame(label_debug_level, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 5, 5));
   label_debug_level->Resize(100, 50);

   fDebugLevelChooser = new TGListBox(h_frame_debug_level);
   fDebugLevelChooser->AddEntry("Quiet, no debug - 0", 0);
   fDebugLevelChooser->AddEntry("Level 1 debug - 1", 1);
   fDebugLevelChooser->AddEntry("Level 2 debug - 2", 2);
   fDebugLevelChooser->AddEntry("Level 3 debug - 3", 3);
   fDebugLevelChooser->AddEntry("Level 4 debug - 4", 4);

   fDebugLevelChooser->Resize(150, 50);
   fDebugLevelChooser->Select(fHists->fDebug);
   h_frame_debug_level->AddFrame(fDebugLevelChooser, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

   fDebugLevelChooser->Connect("Selected(Int_t)", "RasterMonConfig", this, "DebugSelection(int)");
   fDebugLevelChooser->Connect("Selected(Int_t)","RasterMonGui",fRasGui,"SetDebug(int)");

   // ********** Debug Level Set  EVIO ***********

   auto *h_frame_debug_level2 = new TGGroupFrame(Frame2, new TGString("Evio System Debug:"),kHorizontalFrame|kRaisedFrame);
   //auto *h_frame_debug_level2 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame_debug_level2, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

   auto info1 = new TGCheckButton(h_frame_debug_level2, "Info L1",1);
   h_frame_debug_level2->AddFrame(info1, new TGLayoutHints(kLHintsLeft| kLHintsBottom, 1, 1, 5, 0));

   auto info2 = new TGCheckButton(h_frame_debug_level2, "Info L2",2);
   h_frame_debug_level2->AddFrame(info2, new TGLayoutHints(kLHintsLeft| kLHintsBottom, 1, 1, 5, 0));

   auto debugl1 = new TGCheckButton(h_frame_debug_level2, "Debug L1",3);
   h_frame_debug_level2->AddFrame(debugl1, new TGLayoutHints(kLHintsLeft| kLHintsBottom, 1, 1, 5, 0));

   auto debugl2 = new TGCheckButton(h_frame_debug_level2, "Debug L2",4);
   h_frame_debug_level2->AddFrame(debugl2, new TGLayoutHints(kLHintsLeft| kLHintsBottom, 1, 1, 5, 0));

   info1->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
   info2->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
   debugl1->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
   debugl2->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");

   // ********** Refresh Rate Set Box **********

   auto *h_frame1 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame1, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));
   auto label1 = new TGLabel(h_frame1,"Window Refresh rate:");
   h_frame1->AddFrame(label1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 55, 5, 5));
   label1->Resize(label_width, 20);

   fNumberEntryRate = new TGNumberEntry(h_frame1, fRefreshRate, 10, 1, TGNumberFormat::kNESInteger,
                                        TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMin, 50, 10000);
   h_frame1->AddFrame(fNumberEntryRate, new TGLayoutHints(kLHintsLeft | kLHintsCenterY ,
                                                          1, 1, 5, 5));

   auto label1tail = new TGLabel(h_frame1," ms/frame");
   label1tail->Resize(9, 20);
   h_frame1->AddFrame(label1tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));

   // fNumberEntryRate->Resize(field_width, 20);
   if(fRasGui) fNumberEntryRate->Connect("ValueSet(Long_t)","RasterMonGui",fRasGui,"SetUpdateRate()");

   // ********** Buffer Depth Set Box **********

   auto *h_frame2 = new TGHorizontalFrame(Frame2);
   Frame2->AddFrame(h_frame2, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));
   auto label2 = new TGLabel(h_frame2,"Scope event buffer depth:");
   label2->Resize(label_width, 20);
   h_frame2->AddFrame(label2, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 20, 5, 5));

   fNumberEntryScopeBufDepth = new TGNumberEntry(h_frame2, (double)(fScopeBufDepth/1000.), 10, 2,
                                                 TGNumberFormat::kNESRealOne,TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMinMax , 0.2, 1000.0);

   // fNumberEntryScopeBufDepth->Resize( 60, 20);
   h_frame2->AddFrame(fNumberEntryScopeBufDepth, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY ,
                                                          1, 1, 5, 5));
   auto label2tail = new TGLabel(h_frame2,"x1k evt.");
   label2tail->Resize(20, 20);
   h_frame2->AddFrame(label2tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));

   fNumberEntryScopeBufDepth->Connect("ValueSet(Long_t)","RasterMonConfig",this,"UpdateADCBufDepth()");


   // ********************** Raster Conversion Numbers.
   GetScaleOffset();

   auto *h_frame_raster_conv = new TGGroupFrame(Frame2, new TGString("Raster Conversion:"), kVerticalFrame | kRaisedFrame);
   Frame2->AddFrame(h_frame_raster_conv, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

   auto *h_frame_raster_row = new TGHorizontalFrame(h_frame_raster_conv);
   h_frame_raster_conv->AddFrame(h_frame_raster_row, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

   auto label_x = new TGLabel(h_frame_raster_row, "f(x) = ");
   h_frame_raster_row->AddFrame(label_x, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
   auto x_scale_enter = new TGNumberEntry(h_frame_raster_row, fScale_x, 12, 3,
                                          TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits);
   x_scale_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
   h_frame_raster_row->AddFrame(x_scale_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
   auto label_x2 = new TGLabel(h_frame_raster_row, "*x + ");
   h_frame_raster_row->AddFrame(label_x2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
   auto x_offset_enter = new TGNumberEntry(h_frame_raster_row, fOffset_x, 12, 4,
                                           TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits);
   x_offset_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
   h_frame_raster_row->AddFrame(x_offset_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));

   h_frame_raster_row = new TGHorizontalFrame(h_frame_raster_conv);
   h_frame_raster_conv->AddFrame(h_frame_raster_row, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

   auto label_y = new TGLabel(h_frame_raster_row, "f(y) = ");
   h_frame_raster_row->AddFrame(label_y, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
   auto y_scale_enter = new TGNumberEntry(h_frame_raster_row, fScale_y, 12, 5,
                                          TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits);
   y_scale_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
   h_frame_raster_row->AddFrame(y_scale_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
   auto label_y2 = new TGLabel(h_frame_raster_row, "*y + ");
   h_frame_raster_row->AddFrame(label_y2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
   auto y_offset_enter = new TGNumberEntry(h_frame_raster_row, fOffset_y, 12, 6,
                                           TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits);
   y_offset_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
   h_frame_raster_row->AddFrame(y_offset_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));


   // ********************** Finish up the Dialog ******************************

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
