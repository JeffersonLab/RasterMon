//
// Created by Maurik Holtrop on 3/29/22.
//

#ifndef RASTERMON_RASTERMONGUI_H
#define RASTERMON_RASTERMONGUI_H

#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TRootEmbeddedCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTimer.h>
#include <TObjString.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGMenu.h>
#include <TGTab.h>
#include <TGFileDialog.h>
#include <TGStatusBar.h>
#include <TRootHelpDialog.h>

#include <locale>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "RasterHists.h"
#include "RasterEvioTool.h"
#include "ETConnectionConfig.h"
#include "RasterMonConfig.h"

class RasterMonGui : public TGMainFrame {

public:

   enum EMyMessageTypes {
      M_FILE_OPEN,
      M_HISTOGRAM_SAVE,
      M_ET_CONNECT,
      M_CONFIGURE,
      M_FILE_EXIT,
      M_HELP_ABOUT
   };

   TGTab *fTabAreaTabs = nullptr;
   std::vector<TRootEmbeddedCanvas *> fCanvases;
   std::unique_ptr<TGMenuBar> fMenuBar = nullptr;
   std::unique_ptr<TGStatusBar> fStatusBar = nullptr;
   TGTextButton *fPauseButton = nullptr;
   bool fPause = false;
   std::unique_ptr<TTimer> fHistUpdateTimer = nullptr;

   std::unique_ptr<RasterHists> fRHists = nullptr;
   RasterEvioTool *fEvio = nullptr;  // Not an object we own, just a handy pointer.
   TGFileInfo fFileInfo;             // Contains file(s) chosen by Open dialog or populated from command line.
   TGFileInfo fSaveFileInfo;         // Contains info for Histogram Save dialog.
   unsigned int fUpdateRate=1000;     // Update rate in ms.

   RasterMonConfig *fConfig = nullptr;
   bool fUpdateSelectedTabOnly = true;

public:
   int fDebug = 0;

public:
   RasterMonGui(RasterHists *hist, const TGWindow *p, UInt_t w, UInt_t h);

   virtual ~RasterMonGui() {
      // Clean up used widgets: frames, buttons, layout hints
      Stop();
      // Cleanup specifically allocated memory.
      for(auto c: fCanvases) delete c;
   }

   void Init(UInt_t w, UInt_t h);
   void DoDraw();

//   void AddFile(const string file){
//      std::cout << "Adding file :" << file << std::endl;
//      if(fFileInfo.fFileNamesList == nullptr ){
//         fFileInfo.fFileNamesList = new TList();
//      }
//      auto file_obj = new TObjString(file.c_str());
//      fFileInfo.fFileNamesList->Add(file_obj);
//   };

   void SetupGUI(UInt_t w, UInt_t h);
   void AddMenuBar();
   void AddTabArea(UInt_t w, UInt_t h);
   void AddControlBar();
   void AddStatusBar();
   void StatusBarUpdate();
   void HandleMenu(int choice);
   void DoConfigure();

   void SetUpdateRate(){
      unsigned long rate=1000;
      if(fConfig){
         rate = fConfig->fNumberEntryRate->GetIntNumber();
         if(fDebug) std::cout << "Set update rate to: " << rate << std::endl;
         fUpdateRate = rate;
         fHistUpdateTimer->SetTime(rate);
      }else{
         if(fDebug) std::cout << "Rate not updates.\n";
      }


   }
   unsigned int GetUpdateRate() const {return fUpdateRate;}

   void Go(){
      if(fDebug>1) std::cout << "Go \n";
      Pause(-1);
      fRHists->go();
   };

   void Pause(int set_state=0);  // set_state: -1 = go, 0=toggle, 1 = pause;

   void Stop(){
      if(fDebug>1) std::cout << "Stop \n";
      Pause(1);
      fRHists->stop();
   };

   void Clear(){
      fRHists->clear();
   }

   void Exit(){
      Stop();
      cout << "Exiting RasterMon. Bye now. \n";
      gApplication->Terminate();
   }

   Bool_t HandleTimer(TTimer *timer) override{
      if(timer == fHistUpdateTimer.get()){
         DoDraw();
         StatusBarUpdate();
      }
      return kTRUE;
   }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterMonGui, 0)
#pragma clang diagnostic pop

};


#endif //RASTERMON_RASTERMONGUI_H
