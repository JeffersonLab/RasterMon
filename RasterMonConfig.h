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
#include "RasterMonConfigInfo.h"

class RasterMonGui;

class RasterMonConfig : public TGTransientFrame {

public:
   // const TGWindow *fParentWindow;
   RasterEvioTool *fEvio = nullptr;
   RasterHists *fHists = nullptr;
   const RasterMonGui *fRasGui = nullptr;

   TGListBox *fDebugLevelChooser;
   TGNumberEntry *fNumberEntryRate;
   TGNumberEntry *fNumberEntryAutoClear;
   TGNumberEntry *fNumberEntryScopeBufDepth;
   TGCheckButton *fAutoClearOnOff;
   TGCheckButton *fEvioDebugInfo1;
   TGCheckButton *fEvioDebugInfo2;
   TGCheckButton *fEvioDebug1;
   TGCheckButton *fEvioDebug2;
   TGNumberEntry *fScale_x;
   TGNumberEntry *fOffset_x;
   TGNumberEntry *fScale_y;
   TGNumberEntry *fOffset_y;


   RasterMonConfigInfo fInfo;

public:
   RasterMonConfig(const RasterMonGui *parent, RasterEvioTool *evio, RasterHists *hists,
                   unsigned int update_rate):
         fRasGui(parent), fEvio(evio), fHists(hists),
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

      SetInfoValues(update_rate);

      SetWindowName("RasterMon Configure Dialog");
      Connect("CloseWindow()", "RasterMonConfig", this, "CloseWindow()");
      DontCallClose(); // to avoid double deletions.
      // use hierarchical cleaning
      SetCleanup(kDeepCleanup);
      // Bottom buttons:  Load, Save, Close
      auto Frame1 = new TGHorizontalFrame(this, frame_width, 20, kFixedWidth);

      // ------------- Bottom button row -------------

      auto CloseButton = new TGTextButton(Frame1, "&Close", 3);
      CloseButton->Connect("Clicked()", "RasterMonGui", (TGWindow *) fRasGui, "CloseConfigure()");
      Frame1->AddFrame(CloseButton, new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 10, 2, 4));

      auto SaveButton = new TGTextButton(Frame1, "&Save", 2);
      SaveButton->Connect("Clicked()", "RasterMonConfigInfo", &fInfo, "SaveToJSON()");
      Frame1->AddFrame(SaveButton, new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 10, 2, 4));

      auto LoadButton = new TGTextButton(Frame1, "&Load", 1);
      LoadButton->Connect("Clicked()", "RasterMonConfigInfo", &fInfo, "LoadFromJSON()");
      LoadButton->Connect("Clicked()", "RasterMonConfig", this, "UpdateDisplay()");
      Frame1->AddFrame(LoadButton, new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 10, 2, 4));

      //  Frame1->AddFrame(CancelButton, L1);
      Frame1->Resize(400, CloseButton->GetDefaultHeight());

      AddFrame(Frame1, new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1));

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
      h_frame_debug_level->AddFrame(fDebugLevelChooser,
                                    new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 5, 5,
                                                      5, 5));

      fDebugLevelChooser->Connect("Selected(Int_t)", "RasterMonConfig", this, "DebugSelection(int)");
      fDebugLevelChooser->Connect("Selected(Int_t)", "RasterMonGui", (TGWindow *) fRasGui, "SetDebug(int)");

      // ********** Debug Level Set  EVIO ***********

      auto *h_frame_debug_level2 = new TGGroupFrame(Frame2, new TGString("Evio System Debug:"),
                                                    kHorizontalFrame | kRaisedFrame);
      //auto *h_frame_debug_level2 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame_debug_level2, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      fEvioDebugInfo1 = new TGCheckButton(h_frame_debug_level2, "Info L1", 1);
      h_frame_debug_level2->AddFrame(fEvioDebugInfo1, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      fEvioDebugInfo2 = new TGCheckButton(h_frame_debug_level2, "Info L2", 2);
      h_frame_debug_level2->AddFrame(fEvioDebugInfo2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      fEvioDebug1 = new TGCheckButton(h_frame_debug_level2, "Debug L1", 3);
      h_frame_debug_level2->AddFrame(fEvioDebug1, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      fEvioDebug2 = new TGCheckButton(h_frame_debug_level2, "Debug L2", 4);
      h_frame_debug_level2->AddFrame(fEvioDebug2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 1, 1, 5, 0));

      fEvioDebugInfo1->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
      fEvioDebugInfo2->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
      fEvioDebug1->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");
      fEvioDebug2->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");

      // ********** Refresh Rate Set Box **********

      auto *h_frame1 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame1, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));

      auto *h_frame1_1 = new TGHorizontalFrame(h_frame1, label_width, 20, kFixedWidth);
      h_frame1->AddFrame(h_frame1_1);
      auto label1 = new TGLabel(h_frame1_1, "Window Refresh rate:     ");
      h_frame1_1->AddFrame(label1, new TGLayoutHints(kLHintsLeft | kLHintsCenterY | kFixedWidth, 10, 55, 5, 5));

      fNumberEntryRate = new TGNumberEntry(h_frame1, fInfo.fUpdateRate, 10, 1, TGNumberFormat::kNESInteger,
                                           TGNumberFormat::kNEAPositive, TGNumberFormat::kNELLimitMin, 50, 10000);
      h_frame1->AddFrame(fNumberEntryRate, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                                                             1, 1, 5, 5));

      auto label1tail = new TGLabel(h_frame1, " ms/frame");
      h_frame1->AddFrame(label1tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));

      fNumberEntryRate->Connect("ValueSet(Long_t)", "RasterMonGui", (TGWindow *) fRasGui, "SetUpdateRate()");
      fNumberEntryRate->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");

      // ********** Auto-Clear Set Box ************

      auto *h_frame3 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame3, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));
      auto *h_frame3_1 = new TGHorizontalFrame(h_frame3, label_width, 20, kFixedWidth);
      h_frame3->AddFrame(h_frame3_1);

      auto label3 = new TGLabel(h_frame3_1, "Auto Clear Rate: ");
      h_frame3_1->AddFrame(label3, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 5, 5));

      fAutoClearOnOff = new TGCheckButton(h_frame3_1, "on/off", 5);
      h_frame3_1->AddFrame(fAutoClearOnOff, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));

      fNumberEntryAutoClear = new TGNumberEntry(h_frame3, fHists->fHistClearTimerRate, 10, 2, TGNumberFormat::kNESRealOne,
                                           TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELNoLimits);
      h_frame3->AddFrame(fNumberEntryAutoClear, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                                                             1, 1, 5, 5));

      auto label3tail = new TGLabel(h_frame3, " s/clear");
      h_frame3->AddFrame(label3tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));
      fNumberEntryAutoClear->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      fAutoClearOnOff->Connect("Clicked()", "RasterMonConfig", this, "HandleButtons()");

      // ********** Buffer Depth Set Box **********

      auto *h_frame2 = new TGHorizontalFrame(Frame2);
      Frame2->AddFrame(h_frame2, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 1));
      auto *h_frame2_1 = new TGHorizontalFrame(h_frame2, label_width, 20, kFixedWidth);
      h_frame2->AddFrame(h_frame2_1);

      auto label2 = new TGLabel(h_frame2_1, "Scope event buffer depth:");
      h_frame2_1->AddFrame(label2, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 20, 5, 5));

      fNumberEntryScopeBufDepth = new TGNumberEntry(h_frame2, double(fInfo.fScopeBufDepth) / 1000., 10, 2,
                                                    TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAPositive,
                                                    TGNumberFormat::kNELLimitMinMax, 0.2, 1000.0);

      // fNumberEntryScopeBufDepth->Resize( 60, 20);
      h_frame2->AddFrame(fNumberEntryScopeBufDepth, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                                                                      1, 1, 5, 5));
      auto label2tail = new TGLabel(h_frame2, "* 1k evt.");
      h_frame2->AddFrame(label2tail, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 5, 5));

      fNumberEntryScopeBufDepth->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "UpdateADCBufDepth()");


      // ********************** Raster Conversion Numbers.

      auto *h_frame_raster_conv = new TGGroupFrame(Frame2, new TGString("Raster Conversion:"),
                                                   kVerticalFrame | kRaisedFrame);
      Frame2->AddFrame(h_frame_raster_conv, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      auto *h_frame_raster_row = new TGHorizontalFrame(h_frame_raster_conv);
      h_frame_raster_conv->AddFrame(h_frame_raster_row, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      auto label_x = new TGLabel(h_frame_raster_row, "f(x) = ");
      h_frame_raster_row->AddFrame(label_x, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      fScale_x = new TGNumberEntry(h_frame_raster_row, fInfo.fScale_x, 12, 3,
                                        TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                        TGNumberFormat::kNELNoLimits);
      fScale_x->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(fScale_x, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto label_x2 = new TGLabel(h_frame_raster_row, "*x + ");
      h_frame_raster_row->AddFrame(label_x2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));

      fOffset_x = new TGNumberEntry(h_frame_raster_row, fInfo.fOffset_x, 12, 4,
                                         TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                         TGNumberFormat::kNELNoLimits);
      fOffset_x->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(fOffset_x, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));

      h_frame_raster_row = new TGHorizontalFrame(h_frame_raster_conv);
      h_frame_raster_conv->AddFrame(h_frame_raster_row, new TGLayoutHints(kLHintsExpandX, 10, 2, 5, 2));

      auto label_y = new TGLabel(h_frame_raster_row, "f(y) = ");
      h_frame_raster_row->AddFrame(label_y, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      fScale_y = new TGNumberEntry(h_frame_raster_row, fInfo.fScale_y, 12, 5,
                                        TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                        TGNumberFormat::kNELNoLimits);
      fScale_y->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(fScale_y, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      auto label_y2 = new TGLabel(h_frame_raster_row, "*y + ");
      h_frame_raster_row->AddFrame(label_y2, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));
      fOffset_y = new TGNumberEntry(h_frame_raster_row, fInfo.fOffset_y, 12, 6,
                                         TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber,
                                         TGNumberFormat::kNELNoLimits);
      fOffset_y->Connect("ValueSet(Long_t)", "RasterMonConfig", this, "ValueSet()");
      h_frame_raster_row->AddFrame(fOffset_y, new TGLayoutHints(kLHintsLeft | kLHintsBottom, 5, 2, 5, 5));

      UpdateDisplay();

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

   void SetInfoValues(unsigned int update_rate){
      // Load the Info structure with the values from other places in the code.
      fInfo.fScopeBufDepth = fEvio->GetAdcBufferSize();
      fInfo.fUpdateRate = update_rate;
      fInfo.fAutoClearRate = fHists->fHistClearTimerRate;
      fInfo.fAutoClearOn = fHists->fHistClearTimerIsOn;
      fInfo.fDebugLevel = fHists->fDebug;
      fInfo.fEvioDebugLevel = fEvio->fDebug;
      GetScaleOffset();
   }


   void PutInfoValues(){
      // Put the updated values in the correct place, which we do after a "Load".
      fEvio->fDebug = fInfo.fEvioDebugLevel;
      fHists->fDebug = fInfo.fDebugLevel;
      fNumberEntryRate->Emit("ValueSet(Long_t)", fInfo.fUpdateRate);   // The only way we can tell the "parent".
      UpdateADCBufDepth();
      fHists->fHistClearTimerRate = fInfo.fAutoClearRate;
      fHists->SetAutoClearRate(fInfo.fAutoClearRate);
      fHists->fHistClearTimerIsOn = fInfo.fAutoClearOn;
      SetScaleOffset();
   }

   void UpdateDisplay(){
      // Update the information on the display from the items in memory.
      // This is called when loading the data from a file, so also set the actual corresponding value.
      // Because the "emit" does not emit what we think, and so actual values are not updated otherwise.
      fDebugLevelChooser->Select(fInfo.fDebugLevel);
      fEvioDebugInfo1->SetState((EButtonState)((fInfo.fEvioDebugLevel & 1) > 0));
      fEvioDebugInfo2->SetState((EButtonState)((fInfo.fEvioDebugLevel & 2) > 0));
      fEvioDebug1->SetState((EButtonState)((fInfo.fEvioDebugLevel & 4) > 0));
      fEvioDebug2->SetState((EButtonState)((fInfo.fEvioDebugLevel & 8) > 0));
      fNumberEntryRate->SetIntNumber(fInfo.fUpdateRate);
      fAutoClearOnOff->SetState((EButtonState)fInfo.fAutoClearOn);
      fNumberEntryAutoClear->SetNumber(fInfo.fAutoClearRate);
      fNumberEntryScopeBufDepth->SetNumber(double(fInfo.fScopeBufDepth) / 1000.);
      fScale_x->SetNumber(fInfo.fScale_x);
      fOffset_x->SetNumber(fInfo.fOffset_x);
      fScale_y->SetNumber(fInfo.fScale_y);
      fOffset_y->SetNumber(fInfo.fOffset_y);
      PutInfoValues();
   }

   void OK() {
      CloseWindow();
   };

   void DebugSelection(int level) {
      std::cout << "Debug changed to level " << level << std::endl;
      fInfo.fDebugLevel = level;
      fHists->fDebug = level;
   }

   void GetScaleOffset() {
      // Get the X and Y scale offsets for the raster hists.
      // Find histograms Raster_xy and get the constants from them. Raster_x, Raster_y should have same constants!
      for (auto &h_t: fHists->fHists) {
         if (strncmp(h_t.hist->GetName(), "Raster_xy", 8) == 0) {
            fInfo.fScale_x = h_t.scale_x;
            fInfo.fOffset_x = h_t.offset_x;
            fInfo.fScale_y = h_t.scale_y;
            fInfo.fOffset_y = h_t.offset_y;
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
               h_t.scale_x = fInfo.fScale_y;     // Looks confusing, BUT, this is for the x-axis of this histogram!
               h_t.offset_x = fInfo.fOffset_y;
            } else {
               h_t.scale_x = fInfo.fScale_x;
               h_t.offset_x = fInfo.fOffset_x;
               h_t.scale_y = fInfo.fScale_y;
               h_t.offset_y = fInfo.fOffset_y;
            }
         }
      }
   }


   void ValueSet() {
      // Handle value changed in the Raster conversion section
      TGNumberEntry *nmbr = (TGNumberEntry *) gTQSender;
      int id = nmbr->WidgetId();
      // std::cout << "Changing the value for " << id << " to " << nmbr->GetNumber() << std::endl;
      if (id == 3) fInfo.fScale_x = nmbr->GetNumber();
      if (id == 4) fInfo.fOffset_x = nmbr->GetNumber();
      if (id == 5) fInfo.fScale_y = nmbr->GetNumber();
      if (id == 6) fInfo.fOffset_y = nmbr->GetNumber();
      if (3 <= id && id <= 6) SetScaleOffset();
      if (id == 1) {
         fInfo.fUpdateRate = nmbr->GetNumber();
      }
      if (id == 2){
         fInfo.fAutoClearRate = fNumberEntryAutoClear->GetNumber();
         fHists->SetAutoClearRate(fInfo.fAutoClearRate);
         fInfo.fAutoClearOn = fHists->fHistClearTimerIsOn;
         fAutoClearOnOff->SetState((EButtonState)fHists->fHistClearTimerIsOn);
      }
   }

   void HandleButtons() {
      // Handle different buttons.

      TGButton *btn = (TGButton *) gTQSender;
      int id = btn->WidgetId();
      // std::cout << "Changing button " << id << " to " << btn->GetState() << std::endl;
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
         fInfo.fEvioDebugLevel = bits;
      }else if(id == 5){  // Auto clear on/off
         fInfo.fAutoClearOn = btn->GetState();
         fHists->fHistClearTimerIsOn = fInfo.fAutoClearOn;
         if(fHists->fHistClearTimerIsOn) fHists->SetAutoClearRateOn();
         else fHists->SetAutoClearRateOff();
      }
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
