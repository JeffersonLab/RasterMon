//
// Created by Maurik Holtrop on 5/12/22.
//
#include <iostream>
#include <sstream>
#include <thread>
#include <ctime>
#include <cstdlib>
#include "RasterLogBookEntry.h"

#ifdef HAS_LOGBOOK
#include "ELogBookShim.h"
#endif

RasterLogBookEntry::RasterLogBookEntry(const TGWindow *parent_window, RasterHists *rhists): fRHists(rhists) {
   fParentWindow = parent_window;
};

TGTextEntry* RasterLogBookEntry::AddTextLine(string label_text, string init_text, string tooltip){
   // Add a single entry line with label to the dialog.
   int label_width = 90;
   auto Frame = new TGHorizontalFrame(fMain, 0, 50, kLHintsExpandX);
   auto LabelFrame = new TGHeaderFrame( Frame, label_width, 50, kFixedWidth);
   Frame->AddFrame(LabelFrame, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));
   auto label = new TGLabel(LabelFrame, label_text.c_str());
   LabelFrame->AddFrame(label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 5, 2, 2));
   auto TextEntry = new TGTextEntry(Frame, init_text.c_str(), 0);
   TextEntry->SetToolTipText(tooltip.c_str());
   Frame->AddFrame(TextEntry, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 2, 2, 2, 2));
   fMain->AddFrame(Frame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 15, 5, 2));
   return TextEntry;
}

void RasterLogBookEntry::MakeEntry() {

   fMain = new TGTransientFrame(gClient->GetRoot(), fParentWindow, 400, 400);
   fMain->SetWindowName("Logbook Entry Dialog");
   fMain->DontCallClose(); // to avoid double deletions.
   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);
   // We spin off a thread to save the canvasses to file, otherwise the entire app blocks while these are being
   // written, and writing them can be slow. We use a lock mechanism (see fRHists->fDrawLock) to allow the thread to
   // write the files in batch mode.

   if(!fAlreadyWritingImages) {
      //fEntryThread = std::thread(&RasterLogBookEntry::SaveCanvassesToFile, this);
      //SaveCanvassesToFile(0);
      // fAlreadyWritingImages = true;
   }

//#ifdef HAS_LOGBOOK
   // Next we open the dialog window for the user to interact with.
   fMain->Connect("CloseWindow()", "RasterLogBookEntry", this, "CloseWindow()");

   auto Frame1 = new TGHorizontalFrame(fMain, 800, 50, kFixedWidth | kFitHeight);

   auto OkButton = new TGTextButton(Frame1, "&Ok", 991);
   OkButton->Connect("Clicked()", "RasterLogBookEntry", this, "OK()");
   Frame1->AddFrame(OkButton, new TGLayoutHints(kLHintsTop | kLHintsRight , 2, 2, 2, 2));

   auto CancelButton = new TGTextButton(Frame1, "&Cancel", 990);
   CancelButton->Connect("Clicked()", "RasterLogBookEntry", this, "Cancel()");
   Frame1->AddFrame(CancelButton, new TGLayoutHints(kLHintsTop | kLHintsRight , 2, 2, 2, 2));

   fTitle = "RasterMon for run " + to_string(fRHists->fEvio->GetRunNumber());
   fTitleEntry = AddTextLine("Title:", fTitle, "Update the title for the logbook entery, or leave as is.");
   fLogBooksEntry = AddTextLine("Logbook(s):", fLogBooks, "A list of logbooks (separate by a comma) where this entry should be logged.");
   fTagsEntry = AddTextLine("Tags:", fTags, "Optional tags can be used to classify entries.");
   fEntryMakersEntry = AddTextLine("Entry makers:", fEntryMakers, "A list of usernames (separated by commas) of individuals who should be associated with authorship of the entry.");
   fEmailNotifyEntry = AddTextLine("Email notify:", fEmailNotify,"A list of email addresses (separated by commas) to receive an email copy of the entry after it is posted.");

   fBodyEdit = new TGTextEdit(fMain, 0, 500);
   fMain->AddFrame(fBodyEdit, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 10));

   fBodyEdit->AddLine("RasterMon auto generated logbook entry.");
   fBodyEdit->AddLine("");
   int i_tab =1;
   for(auto tab: fRHists->fTabs){
      string line = "Tab: " + tab.name;
      fBodyEdit->AddLine(line.c_str());
      line = "[Figure:" + to_string(i_tab) + "]";
      fBodyEdit->AddLine(line.c_str());
      i_tab++;
   }

   fMain->AddFrame(Frame1, new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1));
   fMain->MapSubwindows();

   UInt_t width = fMain->GetDefaultWidth();
   UInt_t height = fMain->GetDefaultHeight();
   fMain->Resize(width, height);
   fMain->CenterOnParent();
   fMain->MapWindow();
// #endif
}

void RasterLogBookEntry::SubmitToLogBook() {
   fTitle = fTitleEntry->GetText();
   fLogBooks = fLogBooksEntry->GetText();
   fTags = fTagsEntry->GetText();
   fEntryMakers = fEntryMakersEntry->GetText();
   fEmailNotify = fEmailNotifyEntry->GetText();
   auto body_tgtext = fBodyEdit->GetText();
   fBody = body_tgtext->AsString();
   while( fAlreadyWritingImages ){
      gSystem->ProcessEvents();
      gSystem->Sleep(50);
   }
   if(fEntryThread.joinable()) fEntryThread.join();
   // At this point the attachments should be ready.
   for(int i=0; i< fRHists->fTabs.size(); ++i){
      auto tab = fRHists->fTabs.at(i);
      fAttachmentCaptions.push_back("Histograms for tab " + tab.name);
   }

#ifdef HAS_LOGBOOK
   ELogBookShim lb;
   lb.fTitle = fTitle;
   lb.fLogBooks = fLogBooks;
   lb.fTags = fTitle;
   lb.fEntryMakers = fEntryMakers;
   lb.fEmailNotify = fEmailNotify;
   lb.fBody = fBody;
   lb.fAttachments = fAttachments;
   lb.fAttachmentCaptions = fAttachmentCaptions;
#else
   string cmd = "echo '" + fBody + "' | logentry -t '" + fTitle + "' ";
   if(!fLogBooks.empty()) {
      stringstream ss(fLogBooks);
      while(ss.good()) {
         string substr;
         getline(ss, substr, ',');
         cmd += " -l " + substr + " ";
      }
   }else {
      cmd += " -l HBLOG ";
   }
   if(!fTags.empty()) {
      stringstream ss(fTags);
      while (ss.good()) {
         string substr;
         getline(ss, substr, ',');
         cmd += " --tag " + substr + " ";
      }
   }
   if(!fEntryMakers.empty()) cmd += "--entrymaker '" + fEntryMakers +"' ";
   if(!fEmailNotify.empty()) {
      stringstream ss(fEmailNotify);
      while (ss.good()) {
         string substr;
         getline(ss, substr, ',');
         cmd += " --notify '" + substr + "' ";
      }
   }
   for(int i=0; i< fAttachments.size(); ++i) {
      cmd += " --attach '" + fAttachments[i] + "' --caption '" + fAttachmentCaptions[i] + "' ";
   }
   if(fRHists->fDebug){
      std::cout << "Logbook command: \n";
      std::cout << cmd << std::endl;
   }
   std::system(cmd.c_str());

#endif
}

void RasterLogBookEntry::SaveCanvassesToFile(){
   // This is where all the work is done for the logbook entry. Reminder: this runs multi-threaded,
   // so resources must be local to this thread, or global to all log entries open.
   //
   // First step: Snapshot of all the canvases, so that we get an accurate copy of the current state.
   // Here we do the work to save the images of the canvasses.
   std::vector<TCanvas *> canvs;
   fRHists->Pause();                                        // Pause, so all histos and graphs are at the same event.
   std::vector<Histogram_t> histo_copy{fRHists->fHists};   // Copy the histograms and graphs. (vector copy assignment.)
   // std::vector<Graph_t> graph_copy{fRHists->fGraphs};   // Barfs on gcc 9.3.0 ????  TODO:: Figure out what is wrong here!!!
   std::vector<Graph_t> graph_copy;                        // So create the vector and fill it separately.
   graph_copy.insert(graph_copy.begin(),fRHists->fGraphs.begin(),fRHists->fGraphs.end());
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
   fAttachments = fRHists->SaveCanvasesToImageFiles(directory+filename,"png", &canvs);
   //fRHists->SaveCanvasesToPDF(directory+filename, &canvs);
   canvs.clear();
   fAlreadyWritingImages = false;
   // End of thread.
}