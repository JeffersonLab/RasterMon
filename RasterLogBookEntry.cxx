//
// Created by Maurik Holtrop on 5/12/22.
//
#include <iostream>
#include <thread>
#include <ctime>
#include "RasterLogBookEntry.h"

RasterLogBookEntry::RasterLogBookEntry(const TGWindow *parent_window, RasterHists *rhists): fRHists(rhists),
   TGTransientFrame(gClient->GetRoot(), parent_window, 400, 200){

};

void RasterLogBookEntry::MakeEntry() {
   if(!fAlreadyMakingEntry) {
      fEntryThread = ::thread(&RasterLogBookEntry::EntryDialog, this);
      //EntryDialog(0);
      fEntryThread.detach();
      fAlreadyMakingEntry = true;
   }
}

void RasterLogBookEntry::EntryDialog(){
   // This is where all the work is done for the logbook entry. Reminder: this runs multi-threaded,
   // so resources must be local to this thread, or global to all log entries open.
   //
   // First step: Snapshot of all the canvases, so that we get an accurate copy of the current state.
//   std::vector<TCanvas *> canvasses;
   std::vector<TCanvas *> canvs;

   fRHists->Pause();                                        // Pause, so all histos and graphs are at the same event.
   std::vector<Histogram_t> histo_copy = fRHists->fHists;   // Copy the histograms and graphs. (vector copy assignment.)
   std::vector<Graph_t> graph_copy = fRHists->fGraphs;
   for(int i_tab=0; i_tab < fRHists->fTabs.size(); ++i_tab) {
      fRHists->FillGraphs(i_tab, graph_copy);       // Update the graph contents.
   }
   fRHists->UnPause();

   for(int i_tab=0; i_tab < fRHists->fTabs.size(); ++i_tab) {
      auto tab = fRHists->fTabs.at(i_tab);
      string cname = "print_" + to_string(i_tab);
      fRHists->fDrawLock.lock();
      gROOT->SetBatch(true);
      auto canvas = new TCanvas(cname.c_str(), cname.c_str(), 2000, 2000);
      fRHists->SetupCanvas(tab, canvas);
      gROOT->SetBatch(false);
      fRHists->fDrawLock.unlock();
      // Fill those canvasses with the histograms.
      fRHists->DrawCanvas(i_tab, canvas, histo_copy, graph_copy, true);
      canvs.push_back(canvas);
   }


   // Construct the filename as rastermon_2022_05_15_16_04_23_1.png
   char buf[50];
   time_t t_now = time(NULL);
   strftime(buf, 50, "rastermon_%Y_%m_%d_%H_%M_%S_", localtime(&t_now));
   string filename(buf);
   string directory(DEFAULT_HISTOGRAM_PATH"/");
   fRHists->SaveCanvasesToImageFiles(directory+filename,"png", &canvs);
   //fRHists->SaveCanvasesToPDF(directory+filename, &canvs);
   canvs.clear();
   // Now we bring up a dialog for the user to add information.
   fAlreadyMakingEntry = false;

   // End of thread.

}