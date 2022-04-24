//
// Created by Maurik Holtrop on 3/29/22.
//

#ifndef RASTERMON_RASTERMONGUI_H
#define RASTERMON_RASTERMONGUI_H

#include <TROOT.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>
#include <TGMenu.h>
#include <TGTab.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTimer.h>
#include <TObjString.h>
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

class RasterMonGui : public TGMainFrame {

private:

   enum EMyMessageTypes {
      M_FILE_OPEN,
      M_HISTOGRAM_SAVE,
      M_ET_CONNECT,
      M_CONFIGURE,
      M_FILE_EXIT,
      M_HELP_ABOUT
   };

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

   void Go(){
      if(fDebug>1) std::cout << "Go \n";
      fPause = false;
      fHistUpdateTimer->TurnOn();
      fRHists->go();
   };

   void Pause(){
      if(fPause){
         fPauseButton->SetText("&Pause");
         if(fDebug>1) std::cout << "Un-Pause \n";
         fHistUpdateTimer->TurnOn();
         fPause = false;
         fRHists->unpause();
      }else{
         fPauseButton->SetText("&UnPause");
         if(fDebug>1) std::cout << "Pause \n";
         fHistUpdateTimer->TurnOff();
         fPause = true;
         fRHists->pause();
      }
   };

   void Stop(){
      if(fDebug>1) std::cout << "Stop \n";
      fHistUpdateTimer->TurnOff();
      fPause = true;
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
