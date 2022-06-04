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

class RasterMonConfig : public TGTransientFrame {

public:
   // const TGWindow *fParentWindow;
   RasterEvioTool *fEvio = nullptr;
   RasterHists *fHists = nullptr;
   // RasterMonGui *fRasGui = nullptr;

   TGListBox *fDebugLevelChooser;
   TGNumberEntry *fNumberEntryRate;
   TGNumberEntry *fNumberEntryAutoClear;
   TGNumberEntry *fNumberEntryScopeBufDepth;
   TGCheckButton *fAutoClearOnOff;

   unsigned int fScopeBufDepth = 10000;

   double fScale_x = 0.004;
   double fOffset_x = -8;
   double fScale_y = 0.004;
   double fOffset_y = -8;

public:
   RasterMonConfig(const RasterMonGui *parent, RasterEvioTool *evio, RasterHists *hists,
                   unsigned int update_rate):
         fEvio(evio), fHists(hists),
         TGTransientFrame(gClient->GetRoot(), (TGWindow *) parent, 400, 200) {
      // Handle the user input for configurable settings of the program.
      // Run the main bit of gui code.
      // update_rate and clear_rate are passed, because the full RaterMonGui class is not known.
      // and we have RasterMonGui include RasterMonConfig.
      //
      int frame_width = 400;
      int frame_height = 400;
      int label_width = 190;
      int field_width = 200;

      fScopeBufDepth = fEvio->GetAdcBufferSize();

      SetWindowName("RasterMon Configure Dialog");
      Connect("CloseWindow()", "RasterMonConfig", this, "CloseWindow()");
      DontCallClose(); // to avoid double deletions.
      // use hierarchical cleaning
      SetCleanup(kDeepCleanup);
      // Bottom buttons:  Cancel, OK.
      auto Frame1 = new TGHorizontalFrame(this, frame_width, 20, kFixedWidth);

      // ------------- Bottom button row -------------
      auto OkButton = new TGTextButton(Frame1, "&Close", 1);
      OkButton->Connect("Clicked()", "RasterMonGui", (TGWindow *) parent, "CloseConfigure()");
//   auto CancelButton = new TGTextButton(Frame1, "&Cancel", 2);
//   CancelButton->Connect("Clicked()", "RasterMonConfig", this, "Cancel()");
      auto L1 = new TGLayoutHints(kLHintsTop | kLHintsRight, //| kLHintsExpandX,
                                  2, 10, 2, 4);
      auto L2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);
      Frame1->AddFrame(OkButton, L1);
      //  Frame1->AddFrame(CancelButton, L1);
      Frame1->Resize(400, OkButton->GetDefaultHeight());

      AddFrame(Frame1, L2);

      // -------------- Main Dialog Frame --------------

      // Vertically stacked items, i.e. followed by TGHorizontalFrames for filling the rows.
      auto Frame2 = new TGVerticalFrame(this, frame_width, frame_height, kFixedWidth | kFixedHeight);

      auto *h_frame0 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame0, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
      auto label0 = new TGLabel(h_frame0, "For numeric entry, use shift key for x10, crtl key for x100");
      h_frame0->AddFrame(label0, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));

      // ********** Debug Level Set  RasterMon ***********
      auto *h_frame_debug_level = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame_debug_level, new TGLayoutHints(kLHintsExpandX, 10, 2, 10, 2));
      auto label_debug_level = new TGLabel(h_frame_debug_level, "RasterMon Gui Debug Level:");
      h_frame_debug_level->AddFrame(label_debug_level, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 5, 5));

      fDebugLevelChooser = new TGListBox(h_frame_debug_level);
      fDebugLevelChooser->AddEntry("Quiet, no debug - 0", 0);
      fDebugLevelChooser->AddEntry("Level 1 debug - 1", 1);
      fDebugLevelChooser->AddEntry("Level 2 debug - 2", 2);
      fDebugLevelChooser->AddEntry("Level 3 debug - 3", 3);
      fDebugLevelChooser->AddEntry("Level 4 debug - 4", 4);

      fDebugLevelChooser->Resize(150, 50);
      fDebugLevelChooser->Select(fHists->fDebug);
      h_frame_debug_level->AddFrame(fDebugLevelChooser,
                                    new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 5, 5,
                                                      5, 5));

      fDebugLevelChooser->Connect("Selected(Int_t)", "RasterMonConfig", this, "DebugSelection(int)");
      fDebugLevelChooser->Connect("Selected(Int_t)", "RasterMonGui", (TGWindow *) parent, "SetDebug(int)");

      // ********** Debug Level Set  EVIO ***********

      auto *h_frame_debug_level2 = new TGGroupFrame(Frame2, new TGString("Evio System Debug:"),
                                                    kHorizontalFrame | kRaisedFrame);
      //auto *h_frame_debug_level2 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame_debug_level2, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      auto info1 = new TGCheckButton(h_frame_debug_level2, "Info L1", 1);
      h_frame_debug_level2->AddFrame(info1, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      auto info2 = new TGCheckButton(h_frame_debug_level2, "Info L2", 2);
      h_frame_debug_level2->AddFrame(info2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      auto debugl1 = new TGCheckButton(h_frame_debug_level2, "Debug L1", 3);
      h_frame_debug_level2->AddFrame(debugl1, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      auto debugl2 = new TGCheckButton(h_frame_debug_level2, "Debug L2", 4);
      h_frame_debug_level2->AddFrame(debugl2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      info1->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
      info2->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
      debugl1->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
      debugl2->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");

      // ********** Refresh Rate Set Box **********

      auto *h_frame1 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame1, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));

      auto *h_frame1_1 = new TGHorizontalFrame(h_frame1, label_width, 20, kFixedWidth);
      h_frame1->AddFrame(h_frame1_1);
      auto label1 = new TGLabel(h_frame1_1, "Window Refresh rate:     ");
      h_frame1_1->AddFrame(label1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY | kFixedWidth, 10, 55, 5, 5));

      fNumberEntryRate = new TGNumberEntry(h_frame1, update_rate, 10, 1, TGNumberFormat::kNESInteger,
                                           TGNumberFormat::kNEAPositive, TGNumberFormat::kNELLimitMin, 50, 10000);
      h_frame1->AddFrame(fNumberEntryRate, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                                                             1, 1, 5, 5));

      auto label1tail = new TGLabel(h_frame1, " ms/frame");
      h_frame1->AddFrame(label1tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));

      fNumberEntryRate->Connect("ValueSet(Long_t)", "RasterMonGui", (TGWindow *) parent, "SetUpdateRate()");

      // ********** Auto-Clear Set Box ************

      auto *h_frame3 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame3, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));
      auto *h_frame3_1 = new TGHorizontalFrame(h_frame3, label_width, 20, kFixedWidth);
      h_frame3->AddFrame(h_frame3_1);

      auto label3 = new TGLabel(h_frame3_1, "Auto Clear Rate: ");
      h_frame3_1->AddFrame(label3, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 5, 5));

      fAutoClearOnOff = new TGCheckButton(h_frame3_1, "on/off", 5);
      h_frame3_1->AddFrame(fAutoClearOnOff, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));
      fAutoClearOnOff->SetState((EButtonState)fHists->fHistClearTimerIsOn);

      fNumberEntryAutoClear = new TGNumberEntry(h_frame3, fHists->fHistClearTimerRate, 10, 2, TGNumberFormat::kNESRealOne,
                                           TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELNoLimits);
      h_frame3->AddFrame(fNumberEntryAutoClear, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                                                             1, 1, 5, 5));

      auto label3tail = new TGLabel(h_frame3, " s/clear");
      h_frame3->AddFrame(label3tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));
      fNumberEntryAutoClear->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "SetAutoClearRate()");
      fAutoClearOnOff->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");

      // ********** Buffer Depth Set Box **********

      auto *h_frame2 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame2, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));
      auto *h_frame2_1 = new TGHorizontalFrame(h_frame2, label_width, 20, kFixedWidth);
      h_frame2->AddFrame(h_frame2_1);

      auto label2 = new TGLabel(h_frame2_1, "Scope event buffer depth:");
      h_frame2_1->AddFrame(label2, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 20, 5, 5));

      fNumberEntryScopeBufDepth = new TGNumberEntry(h_frame2, double(fScopeBufDepth) / 1000., 10, 2,
                                                    TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAPositive,
                                                    TGNumberFormat::kNELLimitMinMax, 0.2, 1000.0);

      // fNumberEntryScopeBufDepth->Resize( 60, 20);
      h_frame2->AddFrame(fNumberEntryScopeBufDepth, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                                                                      1, 1, 5, 5));
      auto label2tail = new TGLabel(h_frame2, "* 1k evt.");
      h_frame2->AddFrame(label2tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));

      fNumberEntryScopeBufDepth->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "UpdateADCBufDepth()");


      // ********************** Raster Conversion Numbers.
      GetScaleOffset();

      auto *h_frame_raster_conv = new TGGroupFrame(Frame2, new TGString("Raster Conversion:"),
                                                   kVerticalFrame | kRaisedFrame);
      Frame2->AddFrame(h_frame_raster_conv, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      auto *h_frame_raster_row = new TGHorizontalFrame(h_frame_raster_conv);
      h_frame_raster_conv->AddFrame(h_frame_raster_row, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      auto label_x = new TGLabel(h_frame_raster_row, "f(x) = ");
      h_frame_raster_row->AddFrame(label_x, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto x_scale_enter = new TGNumberEntry(h_frame_raster_row, fScale_x, 12, 3,
                                             TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                             TGNumberFormat::kNELNoLimits);
      x_scale_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(x_scale_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto label_x2 = new TGLabel(h_frame_raster_row, "*x + ");
      h_frame_raster_row->AddFrame(label_x2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto x_offset_enter = new TGNumberEntry(h_frame_raster_row, fOffset_x, 12, 4,
                                              TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                              TGNumberFormat::kNELNoLimits);
      x_offset_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(x_offset_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));

      h_frame_raster_row = new TGHorizontalFrame(h_frame_raster_conv);
      h_frame_raster_conv->AddFrame(h_frame_raster_row, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      auto label_y = new TGLabel(h_frame_raster_row, "f(y) = ");
      h_frame_raster_row->AddFrame(label_y, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto y_scale_enter = new TGNumberEntry(h_frame_raster_row, fScale_y, 12, 5,
                                             TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                             TGNumberFormat::kNELNoLimits);
      y_scale_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(y_scale_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto label_y2 = new TGLabel(h_frame_raster_row, "*y + ");
      h_frame_raster_row->AddFrame(label_y2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto y_offset_enter = new TGNumberEntry(h_frame_raster_row, fOffset_y, 12, 6,
                                              TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                              TGNumberFormat::kNELNoLimits);
      y_offset_enter->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(y_offset_enter, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));


      // ********************** Finish up the Dialog ******************************

      AddFrame(Frame2, new TGLayoutHints(kLHintsTop));
      MapSubwindows();
      UInt_t width = GetDefaultWidth();
      UInt_t height = GetDefaultHeight();
      Resize(width, height);
      CenterOnParent();
      // make the message box non-resizable
      SetWMSize(width, height);
      SetWMSizeHints(width, height, width, height, 0, 0);
      MapWindow();
   };

   void OK() {
      CloseWindow();
   };

   void DebugSelection(int level) {
      std::cout << "Debug changed to level " << level << std::endl;
      fHists->fDebug = level;
   }

   void GetScaleOffset() {
      // Get the X and Y scale offsets for the raster hists.
      // Find histograms Raster_xy and get the constants from them. Raster_x, Raster_y should have same constants!
      for (auto &h_t: fHists->fHists) {
         if (strncmp(h_t.hist->GetName(), "Raster_xy", 8) == 0) {
            fScale_x = h_t.scale_x;
            fOffset_x = h_t.offset_x;
            fScale_y = h_t.scale_y;
            fOffset_y = h_t.offset_y;
         }
      }
   }

   void SetScaleOffset() {
      // Get the X and Y scale offsets for the raster hists.
      // Find histograms Raster_x, Raster_y, Raster_xy and get the constants on them.
      for (auto &h_t: fHists->fHists) {
         string name = h_t.hist->GetName();   // Copy name to string and make all lower case.
         std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c);});
         if (name.compare(0, 6 ,"raster") == 0 ) {         // It is a "raster" type histogram.
            if(name.compare(name.size()-2,2,"_y") == 0){   // For the _y histograms, the X axis scaling must change!
               h_t.scale_x = fScale_y;     // Looks confusing, BUT, this is for the x-axis of this histogram!
               h_t.offset_x = fOffset_y;
            } else {
               h_t.scale_x = fScale_x;
               h_t.offset_x = fOffset_x;
               h_t.scale_y = fScale_y;
               h_t.offset_y = fOffset_y;
            }
         }
      }
   }


   void ValueSet() {
      // Handle value changed in the Raster conversion section
      TGNumberEntry *nmbr = (TGNumberEntry *) gTQSender;
      int id = nmbr->WidgetId();
      if (id == 3) fScale_x = nmbr->GetNumber();
      if (id == 4) fOffset_x = nmbr->GetNumber();
      if (id == 5) fScale_y = nmbr->GetNumber();
      if (id == 6) fOffset_y = nmbr->GetNumber();
      if (3 <= id && id <= 6) SetScaleOffset();
   }

   void HandleButtons() {
      // Handle different buttons.

      TGButton *btn = (TGButton *) gTQSender;
      int id = btn->WidgetId();
      if (id > 0 && id < 5) {    // EVIO Debug buttons.
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
      }else if(id == 5){  // Auto clear on/off
         fHists->fHistClearTimerIsOn = btn->GetState();
         if(fHists->fHistClearTimerIsOn) fHists->SetAutoClearRateOn();
         else fHists->SetAutoClearRateOff();
      }
   }

   void SetAutoClearRate(){
      double new_rate = fHists->fHistClearTimerRate = fNumberEntryAutoClear->GetNumber();
      fHists->SetAutoClearRate(new_rate);
      fAutoClearOnOff->SetState((EButtonState)fHists->fHistClearTimerIsOn);
   }

   void UpdateADCBufDepth() {
      double rnum = fNumberEntryScopeBufDepth->GetNumber();
      unsigned long bufsize = (unsigned long) (rnum * 1000.);
      bool prevstate = fHists->Pause();   // Pause the worker threads.
      if (fEvio) {
         fEvio->UpdateBufferSize(bufsize);
      }
      if (fHists) {
         fHists->ResizeScopeGraphs(bufsize);
      }
      if (!prevstate) fHists->UnPause();
   }

};

#endif //RASTERMON_RASTERMONCONFIG_H
