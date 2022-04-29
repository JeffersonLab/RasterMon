//
// Created by Maurik Holtrop on 3/29/22.
//

#ifndef RASTERMON_RASTERHISTS_H
#define RASTERMON_RASTERHISTS_H

#include <TROOT.h>
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
#include <TH1D.h>
#include <TH2D.h>
#include <THStack.h>
#include <TLegend.h>
#include <TTimer.h>
#include <TFile.h>

#include "RasterEvioTool.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>


class RasterHists : public TQObject, public TObject{

private:
   RasterEvioTool *fEvio = nullptr;

   // Tab 0
   std::unique_ptr<TH1D> fHRaster_X = nullptr;
   std::unique_ptr<TH1D> fHRaster_Y = nullptr;
   std::unique_ptr<TH1D> fHRaster_R = nullptr;
   std::unique_ptr<TH2D> fHRaster_XY = nullptr;
   // Tab 1
   std::unique_ptr<TH1D> fHRaw_X = nullptr;
   std::unique_ptr<TH1D> fHRaw_Y = nullptr;
   std::unique_ptr<TH2D> fHRaw_XY = nullptr;
   // Tab 2
   std::unique_ptr<TH1D> fHRaw2_X = nullptr;
   std::unique_ptr<TH1D> fHRaw2_Y = nullptr;
   std::unique_ptr<TH2D> fHRaw2_XY = nullptr;

   std::unique_ptr<TH2D> fHRaw2_vs_Raw1_x = nullptr;
   std::unique_ptr<TH2D> fHRaw2_vs_Raw1_y = nullptr;

   // Tab 3  -- 3 channels for helicity.
   std::vector<TH1I> fHelicity;
   std::vector<TH1D> fHelicity_raw;
   std::unique_ptr<THStack> fHelicity_stack;
   std::unique_ptr<TLegend> fHelicity_legend;

   double fRasterScale[2] = {0.004, 0.004};
   double fRasterOffset[2] = {-8., -8.};

   // For the worker fill threads.
   int  fDebug = 0;
   bool fKeepWorking = false;  // Set to true in Go();
   bool fPause = true;

   int fNWorkers = 1;
   std::vector<std::thread> fWorkers;
   std::mutex fEvioReadLock;

   std::vector<TCanvas *> fCanvases;

public:
   RasterHists(){};
   RasterHists(RasterEvioTool *evio): fEvio(evio) {};
   virtual ~RasterHists();

   void Setup_Histograms(TCanvas *canvas);
   void DrawCanvas(int hist_no);
   void HistFillWorker(int seed=0);
   RasterEvioTool *GetEvioPtr(){return fEvio;}
   void pause(){ fPause = true;}
   void unpause(){ fPause = false;}
   void go();
   void stop();
   void clear();
   void DoDraw();
   bool isworking(){return(fKeepWorking);}

   void SetDebug(int level){ fDebug = level;}
   int GetDebug(){ return(fDebug);}
   void SavePDF(const string &file, bool overwrite=true);
   void SaveRoot(const string &file, bool overwrite=true);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(RasterHists, 0)
#pragma clang diagnostic pop

};


#endif //RASTERMON_RASTERHISTS_H
