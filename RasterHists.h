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

struct Graph_t {   // Object to hold the information for each scope channel for FADC readout.
   unsigned char bank_tag;
   unsigned char slot;
   unsigned char adc_chan;  // Channel for the slot of the FADC module.
   unsigned char tab_number; // Number of the tab where graphs are to be shown.
   unsigned char pad_number; // Number of the pad in the canvas (tab). 0 is top. Max is 3, for 4 pads.
   string  name;        // Name for the TGraph.
   string  title;       // Title. Set to "" to not show title on graph.
   string  legend;      // Legend entry.
   string draw_opt = ""; // Draw option.
   unsigned int color;  // Line color, as in kRed = 632.
   unsigned char width; // Line width
   bool show;      // Set false to not draw the TGraph, but still accumulate the data.
   int data_index; // Index into the data buffers. Filled when EVIO is setup.
   TGraph *graph = nullptr; // The TGraph object that actually contains the data.
   Graph_t(unsigned char tab_number, unsigned char pad_number,
           unsigned int bank_tag, unsigned char slot, unsigned char adc_chan,
           string name, string title, string legend, unsigned int color, unsigned char width, bool show):
         bank_tag(bank_tag), slot(slot), adc_chan(adc_chan), tab_number(tab_number), pad_number(pad_number),
         name(name), title(title), legend(legend), color(color), width(width), show(show) {
      data_index = -1;
      init_graph();
   };
//   ~Graph_t(){                            // TODO: No destructor = potential memory leak on graph.
//      if(graph != nullptr) delete graph;  // TODO: Add destructor, copy constructor, etc etc.
//   }
   void init_graph(){
      graph = new TGraph();
      graph->SetTitle(title.c_str());
      graph->SetName(name.c_str());
      graph->SetLineColor(color);
      graph->SetLineWidth(width);
      graph->SetMinimum(-1.);
      graph->SetMaximum(4096.);
   }
};

struct Histogram_t {  // Object to hold the information for each histogram channel.
   unsigned char bank_tag;
   unsigned char slot;
   unsigned char adc_chan;  // Channel for the slot of the FADC module.
   int data_index = -1;
   unsigned char bank_tag2 = 0; // For 2D histos, second channel info.
   unsigned char slot2 = 0;
   unsigned char adc_chan2 = 0;  // Channel for the slot of the FADC module.
   int data_index2 = -1;
   unsigned char tab_number; // Number of the tab where graphs are to be shown.
   unsigned char pad_number; // Number of the pad in the canvas (tab). 0 = canvas, 1 is first pad etc.
   bool show;      // Set false to not draw the THist, but still accumulate the data.
   double scale_x = 1.;  // Conversion from ADC integer to real
   double offset_x = 0.; // Offset for conversion to real.
   double scale_y = 1.;
   double offset_y = 0.;
   int special_fill = 0;  // Special calculation for filling this histogram, i.e. exceptions. -1 = no fill.
   int special_draw = 0;  // Special way of drawing this histogram.
   string draw_opt = "";  // Drawing option.
   string legend="";      // Legend entry. -- Usually blank, so no legend.
   TH1 *hist = nullptr;  // Histogram. -- Note: Must be either a unique_ptr, OR we need to be really careful with copy and move constructors.
   Histogram_t(unsigned char tab_number, unsigned char pad_number,
               unsigned int bank_tag, unsigned char slot, unsigned char adc_chan,
               string name, string title, int nx, double x_min, double x_max) :
         bank_tag(bank_tag), slot(slot), adc_chan(adc_chan), tab_number(tab_number), pad_number(pad_number){
      hist = new TH1D(name.c_str(), title.c_str(), nx, x_min, x_max);
      show = true;
   };

   Histogram_t(unsigned char tab_number, unsigned char pad_number,
               unsigned int bank_tag_x, unsigned char slot_x, unsigned char adc_chan_x,
               unsigned int bank_tag_y, unsigned char slot_y, unsigned char adc_chan_y,
               string name, string title, int nx, double x_min, double x_max, int ny, double y_min, double y_max) :
         bank_tag(bank_tag_x), slot(slot_x), adc_chan(adc_chan_x),
         bank_tag2(bank_tag_y), slot2(slot_y), adc_chan2(adc_chan_y),
         tab_number(tab_number), pad_number(pad_number) {
      hist = new TH2D(name.c_str(), title.c_str(), nx, x_min, x_max, ny, y_min, y_max);
      show = true;
   }
   // TODO: No destructor = potential memory leak on graph.
   // TODO: Add the 5 from the Rule of 5.
   // Note on *DESTRUCTOR*  == The Rule of 5.
   // IF you choose to have a non-defaulted destructor (i.e. include a ~Histogram_t()), then you *MUST* also include
   // a copy constructor, a move constructor, a copy assignment and a move assignment, because none of these will be default.
   // I.e. add::
   //   Histogram_t(Histogram_t&&);                  // move constructor
   //   Histogram_t(const Histogram_t&);             // copy constructor
   //   Histogram_t& operator=(Histogram_t&&);       // move assignment
   //   Histogram_t& operator=(const Histogram_t&);  // copy assignment
   //   ~Histogram_t();                              // destructor


   [[nodiscard]] bool Is2D() const{ return bank_tag2>0; }

   [[nodiscard]] TH1 *GetTH1() const{
      // Return a TH1D histogram pointer. No checks are made that the histogram is there.
      return hist;
   }

   [[nodiscard]] TH1D *GetHist() const{
      // Return a TH1D histogram pointer. No checks are made that the histogram is there.
      return dynamic_cast<TH1D *>(hist);
   }
   [[nodiscard]] TH2D *GetHist2D() const{
      // Return a TH2D histogram pointer. Note: No check is made that this is indeed a 2D histogram!
      return dynamic_cast<TH2D *>(hist);
   }

};

struct TabSpace_t{  // Object for each of the tabs.
   string name;
   TRootEmbeddedCanvas *canvas;  // Canvas to hold the histograms and graphs in a tab.
   unsigned int nx;      // Divisions in x.
   unsigned int ny;      // Divisions in y.
   unsigned int npads;   // usually = to nx*ny, unless you do custom pad layout.
   float x_margin;
   float y_margin;
   vector<bool> logy; // true of false for each pad.
   vector<int> grid; // true of false for each pad.
   vector<int> calc;  // Number for any special calculation that may be needed for a pad.
   vector<int> pad_link; // List of how pads are linked for x-axis updates. for 3 pads, set to [2, 3, 1], so pad1 updates pad2, etc.
   std::vector< unsigned short > graphs;  // Index for the Scope graphs into fGraphs. (If you want pointers, then Graph_t needs to be more complicated.)
   std::vector< unsigned short > hists;     // Index for the Histogram into fHists.
   TabSpace_t(string name,unsigned int nx, unsigned int ny, float x_margin=0.01, float y_margin = 0.01):
      name(std::move(name)), nx(nx), ny(ny), npads(nx*ny), x_margin(x_margin), y_margin(y_margin), canvas(nullptr){ };
};

class RasterHists : public TQObject, public TObject{

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
   bool fPause = true;

   int fNWorkers = 1;
   std::vector<std::thread> fWorkers;
   std::mutex fEvioReadLock;

public:
   RasterHists()= default;
   explicit RasterHists(RasterEvioTool *evio): fEvio(evio) {};
   ~RasterHists() override;

   void InitTabs();
   TGTab *AddTabArea(TGWindow *frame, int w, int h);
   void SetupCanvas(TabSpace_t &tab, TCanvas *canvas = nullptr);
   void SetupData();
   void ResizeScopeGraphs(unsigned long size);
   void DrawCanvas(int tab_no, TCanvas *canvas = nullptr);
   void HistFillWorker(int seed=0);
   RasterEvioTool *GetEvioPtr() const{return fEvio;}
   void Pause(){ fPause = true;}
   void UnPause(){ fPause = false;}
   void Go();
   void Stop();
   void Clear(int active_tab=-1);
   void DoDraw(int active_tab=-1);
   bool IsWorking(){return(fKeepWorking);}
   bool IsPaused(){return(fPause);}

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
   void SaveCanvasesToImageFiles(const string &filename, const string &ending, std::vector<TCanvas *> *canvasses = nullptr);
   void SaveRoot(const string &file, bool overwrite=true);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterHists, 0)
#pragma clang diagnostic pop

};

void Default_Initialize_Histograms(RasterHists *r, RasterEvioTool *e);

#endif //RASTERMON_RASTERHISTS_H
