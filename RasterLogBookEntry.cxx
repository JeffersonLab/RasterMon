//
// Created by Maurik Holtrop on 5/12/22.
//
#include <iostream>
#include <thread>
#include <ctime>
#include "RasterLogBookEntry.h"

RasterLogBookEntry::RasterLogBookEntry(const TGWindow *parent_window, RasterHists *rhists): fRHists(rhists),
   TGTransientFrame(gClient->GetRoot(), parent_window, 400, 400){
   SetWindowName("Logbook Entry Dialog");
   DontCallClose(); // to avoid double deletions.
   // use hierarchical cleaning
   SetCleanup(kDeepCleanup);
};

void RasterLogBookEntry::MakeEntry() {

   // We spin off a thread to save the canvasses to file, otherwise the entire app blocks while these are being
   // written, and writing them can be slow. We use a lock mechanism (see fRHists->fDrawLock) to allow the thread to
   // write the files in batch mode.

   if(!fAlreadyMakingEntry) {
 //     fEntryThread = ::thread(&RasterLogBookEntry::SaveCanvassesToFile, this);
      //SaveCanvassesToFile(0);
      fAlreadyMakingEntry = true;
   }

   // Next we open the dialog window for the user to interact with.
   Connect("CloseWindow()", "RasterLogBookEntry", this, "CloseWindow()");

   auto Frame1 = new TGHorizontalFrame(this, 400, 400, kFixedWidth);

   auto CancelButton = new TGTextButton(Frame1, "&Cancel", 990);
   CancelButton->Connect("Clicked()", "RasterLogBookEntry", this, "Cancel()");
   Frame1->AddFrame(CancelButton, new TGLayoutHints(kLHintsTop | kLHintsRight , 2, 2, 2, 2));

   auto OkButton = new TGTextButton(Frame1, "&Ok", 991);
   OkButton->Connect("Clicked()", "RasterLogBookEntry", this, "OK()");
   Frame1->AddFrame(OkButton, new TGLayoutHints(kLHintsTop | kLHintsRight , 2, 2, 2, 2));



   AddFrame(Frame1, new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1));
   MapSubwindows();

   UInt_t width = GetDefaultWidth();
   UInt_t height = GetDefaultHeight();
   Resize(width, height);
   CenterOnParent();
   MapWindow();

}

void RasterLogBookEntry::SubmitToLogBook() {
   fEntryThread.join();
}

void RasterLogBookEntry::SaveCanvassesToFile(){
   // This is where all the work is done for the logbook entry. Reminder: this runs multi-threaded,
   // so resources must be local to this thread, or global to all log entries open.
   //
   // First step: Snapshot of all the canvases, so that we get an accurate copy of the current state.
//   std::vector<TCanvas *> canvasses;


   // Here we do the work to save the images of the canvasses.
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
   string directory(DEFAULT_HISTOGRAM_PATH);
   fRHists->SaveCanvasesToImageFiles(directory+filename,"png", &canvs);
   //fRHists->SaveCanvasesToPDF(directory+filename, &canvs);
   canvs.clear();
   // Now we bring up a dialog for the user to add information.
   fAlreadyMakingEntry = false;

   // End of thread.

}