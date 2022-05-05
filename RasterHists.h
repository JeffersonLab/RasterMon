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


class RasterHists : public TQObject, public TObject{

public:
   struct ScopeGraphs_t {   // Object to hold the information for each scope channel for FADC readout.
      unsigned char bank_tag;
      unsigned char slot;
      unsigned char adc_chan;  // Channel for the slot of the FADC module.
      unsigned char tab_number; // Number of the tab where graphs are to be shown.
      unsigned char pad_number; // Number of the pad in the canvas (tab). 0 is top. Max is 3, for 4 pads.
      string  name;        // Name for the TGraph.
      string  title;       // Title. Set to "" to not show title on graph.
      string  legend;      // Legend entry.
      unsigned int color;  // Line color, as in kRed = 632.
      unsigned char width; // Line width
      bool show;      // Set false to not draw the TGraph, but still accumulate the data.
      int data_index; // Index into the data buffers. Filled when EVIO is setup.
      TGraph *graph = nullptr; // The TGraph object that actually contains the data.
      ScopeGraphs_t(unsigned char tab_number, unsigned char pad_number,
                    unsigned int bank_tag, unsigned char slot, unsigned char adc_chan,
                    string name, string title, string legend, unsigned int color, unsigned char width, bool show):
            bank_tag(bank_tag), slot(slot), adc_chan(adc_chan), tab_number(tab_number), pad_number(pad_number),
            name(name), title(title), legend(legend), color(color), width(width), show(show){
         data_index = -1;
         init_graph();
      };
     void init_graph(){
         graph = new TGraph();
         graph->SetTitle(title.c_str());
         graph->SetName(name.c_str());
         graph->SetLineColor(color);
         graph->SetLineWidth(width);
      }
      ~ScopeGraphs_t(){delete graph;}
   };

   struct Histogram_t{  // Object to hold the information for each histogram channel.
      unsigned char bank_tag;
      unsigned char slot;
      unsigned char adc_chan;  // Channel for the slot of the FADC module.
      int data_index = -1;
      unsigned char bank_tag2 = 0; // For 2D histos, second channel info.
      unsigned char slot2 = 0;
      unsigned char adc_chan2 = 0;  // Channel for the slot of the FADC module.
      int data_index2 = -1;
      unsigned char tab_number; // Number of the tab where graphs are to be shown.
      unsigned char pad_number; // Number of the pad in the canvas (tab). 0 is top. Max is 3, for 4 pads.
      bool show;      // Set false to not draw the TGraph, but still accumulate the data.
      double scale_x = 1.;  // Conversion from ADC integer to real
      double offset_x = 0.; // Offset for conversion to real.
      double scale_y = 1.;
      double offset_y = 0.;
      int special_fill = 0;  // Special calculation for filling this histogram, i.e. exceptions.
      TH1D *hist = nullptr;  // Histogram.
      Histogram_t(unsigned char tab_number, unsigned char pad_number,
                  unsigned int bank_tag, unsigned char slot, unsigned char adc_chan,
                  string name, string title, int nx, double x_min, double x_max):
                  bank_tag(bank_tag),slot(slot), adc_chan(adc_chan), tab_number(tab_number), pad_number(pad_number){
         hist = new TH1D(name.c_str(), title.c_str(), nx, x_min, x_max );
         show = true;
      };
      Histogram_t(unsigned char tab_number, unsigned char pad_number,
                  unsigned int bank_tag_x, unsigned char slot_x, unsigned char adc_chan_x,
                  unsigned int bank_tag_y, unsigned char slot_y, unsigned char adc_chan_y,
                  string name, string title, int nx, double x_min, double x_max, int ny, double y_min, double y_max):
            bank_tag(bank_tag_x),slot(slot_x), adc_chan(adc_chan_x),
            bank_tag2(bank_tag_y),slot2(slot_y), adc_chan2(adc_chan_y),
            tab_number(tab_number), pad_number(pad_number) {

         auto hist2d = new TH2D(name.c_str(), title.c_str(), nx, x_min, x_max, ny, y_min, y_max);
         hist = dynamic_cast<TH1D *>(hist2d);
         show = true;
      }

      ~Histogram_t(){ delete hist;}
   };

   struct TabSpace_t{  // Object for each of the tabs.
      string name;
      TCanvas *canvas;  // Canvas to hold the histograms and graphs. Comes from the TRootEmbeddedCanvas
      unsigned nx;      // Divisions in x.
      unsigned ny;      // Divisions in y.
      vector<bool> logy; // true of false for each pad.
      vector<int> calc;  // Number for any special calculation that may be needed for a pad or canvas.
      std::vector<ScopeGraphs_t *> graphs;  // Pointers the Scope graphs.
      std::vector<Histogram_t *> hists;     // Pointers to histograms.

   };


public:
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
   // Tab 3  -- Scope
   std::unique_ptr<TGraph> fGRaw_x = nullptr;
   std::unique_ptr<TGraph> fGRaw_y = nullptr;
   std::unique_ptr<TGraph> fGRaw2_x = nullptr;
   std::unique_ptr<TGraph> fGRaw2_y = nullptr;

   TPad *fPadTop= nullptr;
   TPad *fPadBot= nullptr;
   bool fPadSizeIsUpdating = false;  // To lock for asynchronous resizing.
   bool fPadTopResizing = false;     // To avoid pingpong resizing
   bool fPadBotResizing = false;

   std::vector<TCanvas *> fCanvases;


   // Tab 4  -- 3 channels for helicity.
   std::vector<TH1I> fHelicity;
   std::vector<TH1D> fHelicity_raw;
   std::unique_ptr<THStack> fHelicity_stack;
   std::unique_ptr<TLegend> fHelicity_legend;

   double fRasterScale[2] = {0.004, 0.004};  // ToDo: Fill these with a fetch from the CCDB.
   double fRasterOffset[2] = {-8., -8.};

   // For the scope channels.
   std::vector<ScopeGraphs_t> fScope;
   // For the histograms.
   std::vector<Histogram_t> fHists;

   // For the worker fill threads.
   int  fDebug = 0;
   bool fKeepWorking = false;  // Set to true in Go();
   bool fPause = true;

   int fNWorkers = 1;
   std::vector<std::thread> fWorkers;
   std::mutex fEvioReadLock;

public:
   RasterHists(){};
   RasterHists(RasterEvioTool *evio): fEvio(evio) {};
   virtual ~RasterHists();

   void InitChannels();
   void ReserveCanvasSpace(int n) { fCanvases.reserve(n);}
   void Setup_Histograms(TCanvas *canvas, int tab_num);
   void CreateScopeGraphs(TCanvas *canvas, int nbins);
   TCanvas *GetCanvas(int i){return fCanvases[i]->GetCanvas();}
   void ResizeScopeGraphs(unsigned long size);
   void DrawCanvas(int hist_no);
   void HistFillWorker(int seed=0);
   RasterEvioTool *GetEvioPtr(){return fEvio;}
   void pause(){ fPause = true;}
   void unpause(){ fPause = false;}
   void go();
   void stop();
   void clear();
   void DoDraw(int active_tab=-1);
   bool isworking(){return(fKeepWorking);}

   TAxis * GetTopAxisFromPad(TPad *pad);
   void SubPadCopyRange(TPad *one, TPad *two);
   void SubPadTopResized();
   void SubPadBotResized();
   void SignalTest1(){ cout << "Signal test 1. \n";}
   void SignalTest2(){ cout << "Signal test 2. \n";}
   void SignalTest3(){ cout << "Signal test 3. \n";}
   void SignalTest4(){ cout << "Signal test 4. \n";}

   void TopUpBuffer(CircularBuffer<double> &buf);

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
