//
// Created by Maurik Holtrop on 3/29/22.
//

#ifndef RASTERMON_RASTERHISTS_H
#define RASTERMON_RASTERHISTS_H

#include <TROOT.h>
#include <TStyle.h>
#include <TObject.h>
#include <TQObject.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>
#include <TGTab.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>
#include <THStack.h>
#include <TLegend.h>
#include <TTimer.h>
#include <TFile.h>

#include "RasterEvioTool.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "Graph.h"
#include "Histogram.h"
#include "Tab.h"

class RasterHists : public TQObject, public TObject{

private:
   bool fPause = true;

public:
   RasterEvioTool *fEvio = nullptr;
   TGTab *fTabAreaTabs = nullptr;    // Pointer to the tab area

   bool fPadSizeIsUpdating = false;  // To lock for asynchronous resizing.

   std::vector<TabSpace_t> fTabs;
   // For the scope channels.
   std::vector<Graph_t> fGraphs;
   size_t fBufferDepth = 10000;
   // For the histograms.
   std::vector<Histogram_t> fHists;

   THStack *fHelicity_stack= nullptr;
   TLegend *fHelicity_legend = nullptr;

   // For the worker fill threads.
   int  fDebug = 0;
   bool fKeepWorking = false;    // Set to true in Go();
   bool fIsTryingToRead = false; // To detect if a read is hung.


   int fNWorkers = 1;
   std::vector<std::thread> fWorkers;
   std::mutex fEvioReadLock;
   // The fDrawLock is for locking ROOT Draw type processes. The issue here is that ROOT cannot (as of yet v 6.27)
   // execute a "Draw()" from multiple threads. The only way to do a "Draw()" from a thread is to do so in batch mode.
   // (I have not verified this with the ROOT team, so I may have missed something.)
   // So the fDrawLock allows the thread that saves histograms to file to lock drawing, then switch ROOT to batch mode,
   // and then switch back and unlock. Clumsy, yes, but it does seem to sort of work. And gROOT->SetBatch() is a global
   // rather deeply embedded in the ROOT system.
   std::mutex fDrawLock;

public:
   RasterHists()= default;
   explicit RasterHists(RasterEvioTool *evio): fEvio(evio) {};
   ~RasterHists() override;

   void InitTabs();
   TGTab *AddTabArea(TGWindow *frame, int w, int h);
   void SetupCanvas(TabSpace_t &tab, TCanvas *canvas = nullptr);
   void SetupData();
   void ResizeScopeGraphs(unsigned long size);
   void DrawCanvas(int tab_no, TCanvas *canvas, vector<Histogram_t> &histograms, vector<Graph_t> &graphs, bool batch=false);
   void FillGraphs(int tab_no, vector<Graph_t> &graphs);
   void HistFillWorker(int seed=0);
   RasterEvioTool *GetEvioPtr() const{return fEvio;}
   bool Pause(){ bool prevstate = fPause; fPause = true; return prevstate;}
   bool UnPause(){ bool prevstate = fPause; fPause = false; return prevstate;}
   bool IsPaused(){return fPause;}
   void Go();
   void Stop();
   void Clear(int active_tab=-1);
   void DoDraw(int active_tab=-1);
   bool IsWorking(){return(fKeepWorking);}

   static TAxis * GetTopAxisFromPad(TPad *pad);
   void SubPadCopyRange(TPad *one, TPad *two);
   // Unfortunately the Signal for a virtual void TPad::RangeChanged() { Emit("RangeChanged()"); } // *SIGNAL*
   // does not include the fNumber identifier. We could derive from TPad, but then TCanvas->Divide() would malfunction.
   // We could *also* derive our own TCanvas, but that would be more of a mess than multiple SubPad#Resize() functions.
   void SubPadResized(int i);
   void SubPad1Resized(){ SubPadResized(1);};  // TODO: Reimplement using gTQSender ???
   void SubPad2Resized(){ SubPadResized(2);};
   void SubPad3Resized(){ SubPadResized(3);};
   void SubPad4Resized(){ SubPadResized(4);};
   void SubPad5Resized(){ SubPadResized(5);};
   void SubPad6Resized(){ SubPadResized(6);};
//   void TestSignal(){ cout << "Test signal\n";};

   void SetDebug(int level){ fDebug = level;}
   int GetDebug(){ return(fDebug);}
   void SavePDF(const string &file, bool overwrite=true);
   void SaveCanvasesToPDF(const string &filename, std::vector<TCanvas *> *canvasses);
   std::vector<std::string> SaveCanvasesToImageFiles(const string &filename, const string &ending,
                                                     std::vector<TCanvas *> *canvasses = nullptr);
   void SaveRoot(const string &file, bool overwrite=true);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterHists, 0)
#pragma clang diagnostic pop

};

#endif //RASTERMON_RASTERHISTS_H
