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
   //fThreadPool.emplace_back(&RasterLogBookEntry::EntryDialog, this, fThreadPool.size());
   EntryDialog(0);
}

void RasterLogBookEntry::EntryDialog(int i){
   // This is where all the work is done for the logbook entry. Reminder: this runs multi-threaded,
   // so resources must be local to this thread, or global to all log entries open.
   //
   // First step: Snapshot of all the canvases, so that we get an accurate copy of the current state.
//   std::vector<TCanvas *> canvasses;
   fRHists->Pause();
   std::vector<TCanvas *> canvs;
   for(int count =0; count < fRHists->fTabs.size(); ++count) {
      auto tab = fRHists->fTabs.at(count);
      TCanvas *canv = (TCanvas *) tab.canvas->GetCanvas()->Clone();
      canvs.push_back(canv);
   }
   fRHists->UnPause();

   // Construct the filename as rastermon_2022_05_15_16:04:23_1.png
   char buf[50];
   time_t t_now = time(NULL);
   strftime(buf, 50, "rastermon_%Y_%m_%d:%H:%M:%S_", localtime(&t_now));
   string filename(buf);
   string directory(DEFAULT_HISTOGRAM_PATH);
   // gROOT->SetBatch(kTRUE);
   fRHists->SaveCanvasesToPDF(directory+filename, &canvs);
   canvs.clear();
   // Now we bring up a dialog for the user to add information.

}