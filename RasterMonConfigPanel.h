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

class RasterMonConfigPanel : public TGTransientFrame {

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

   RasterMonConfigInfo *fInfo;

public:
   RasterMonConfigPanel(const RasterMonGui *parent, RasterEvioTool *evio, RasterHists *hists,
                                    unsigned int update_rate);

   void SetInfoValues(unsigned int update_rate);
   void PutInfoValues();

   void UpdateDisplay();

   void OK() {
      CloseWindow();
   };

   void DebugSelection(int level) {
      std::cout << "Debug changed to level " << level << std::endl;
      fInfo->fDebugLevel = level;
      fHists->fDebug = level;
   }

   void GetScaleOffset();
   void SetScaleOffset();
   void ValueSet();
   void HandleButtons();
   void UpdateADCBufDepth();

};

#endif //RASTERMON_RASTERMONCONFIG_H
