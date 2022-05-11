//
// Created by Maurik Holtrop on 3/29/22.
//

#include "RasterMonGui.h"

RasterMonGui::RasterMonGui(RasterHists *hist, const TGWindow *p, UInt_t w, UInt_t h)
      : fRHists(hist), TGMainFrame(p,w,h) {
   Init(w,h);
}

void RasterMonGui::Init(UInt_t w, UInt_t h){

   setlocale(LC_NUMERIC, "");  // This helps with fancy printing numbers, setting locale => printf("%'d",i) now works.

   static const char *gfFileTypes[] = {
         "EVIO data",   "*.evio.*",
         "EVIO File",   "*.evio",
//      "HIPO file",   "*.hipo",
         0,               0
   };

   static const char *gfFileSaveTypes [] = {"PDF File", "*.pdf", "PNG Image set", "*.png", "ROOT File", "*.root"};

   fFileInfo.SetIniDir("/data");
   fFileInfo.fFileTypes = gfFileTypes;
   fFileInfo.SetMultipleSelection(true);
   fSaveFileInfo.SetIniDir(std::filesystem::current_path().c_str());
   fSaveFileInfo.fFileTypes = gfFileSaveTypes;
   fSaveFileInfo.SetFilename("RasterMonHists");

   SetupGUI(w, h);
   fHistUpdateTimer = std::make_unique<TTimer>(this, fUpdateRate) ;
   fEvio = fRHists->GetEvioPtr();
}

void RasterMonGui::AddMenuBar(){
   // Add the menu bar to the main window.

   fMenuBar = std::make_unique<TGMenuBar>(this, 10, 10, kHorizontalFrame);

   auto fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry(" &Open Evio file...\tCtrl+O", M_FILE_OPEN, 0,
                       gClient->GetPicture("bld_open.png"));
   fMenuFile->AddEntry(" &ET Connect...\tCtrl+E", M_ET_CONNECT);
   fMenuFile->AddEntry(" Co&nfigure\tCtrl+N", M_CONFIGURE);

   fMenuFile->AddEntry(" &Save Histogram file...\tCtrl+O", M_HISTOGRAM_SAVE, 0,
                       gClient->GetPicture("bld_open.png"));

   fMenuFile->AddSeparator();
   fMenuFile->AddEntry(" E&xit\tCtrl+Q", M_FILE_EXIT, 0,
                       gClient->GetPicture("bld_exit.png"));
   fMenuFile->Connect("Activated(Int_t)", "RasterMonGui", this,
                      "HandleMenu(Int_t)");

   fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop|kLHintsLeft,
                                                            0, 4, 0, 0));
   auto fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->AddEntry(" &About...", M_HELP_ABOUT, 0,
                       gClient->GetPicture("about.xpm"));
   fMenuHelp->Connect("Activated(Int_t)", "RasterMonGui", this,
                      "HandleMenu(Int_t)");

   fMenuBar->AddPopup("&Help", fMenuHelp, new TGLayoutHints(kLHintsTop|kLHintsRight));

   AddFrame(fMenuBar.operator->(), new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 5));

}

void RasterMonGui::AddControlBar(){
   // Bottom bar with control buttons.

   auto *hframe=new TGHorizontalFrame(this, 10 ,10);

   auto *go = new TGTextButton(hframe,"&Go");
   go->Connect("Clicked()","RasterMonGui",this,"Go()");
   hframe->AddFrame(go, new TGLayoutHints(kLHintsCenterX,
                                          5,5,3,4));

   fPauseButton = new TGTextButton(hframe," &Pause ");
   fPauseButton->Connect("Clicked()","RasterMonGui",this,"Pause()");
   hframe->AddFrame(fPauseButton, new TGLayoutHints(kLHintsCenterX,
                                             5,5,3,4));

   TGButton *stop;
   auto stop_pic =  gClient->GetPicture("ed_interrupt.png");
   if(stop_pic){
      stop = new TGPictureButton(hframe, stop_pic);
   }else{
      stop = new TGTextButton(hframe, "&Stop");
   }
   stop->Connect("Clicked()","RasterMonGui",this,"Stop()");
   hframe->AddFrame(stop, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));

   auto *cleartab = new TGTextButton(hframe,"&Clear");
   cleartab->Connect("Clicked()","RasterMonGui",this,"ClearTab()");
   hframe->AddFrame(cleartab, new TGLayoutHints(kLHintsCenterX,
                                             5,5,3,4));

   auto *clear = new TGTextButton(hframe,"&Clear All");
   clear->Connect("Clicked()","RasterMonGui",this,"ClearAll()");
   hframe->AddFrame(clear, new TGLayoutHints(kLHintsCenterX,
                                             5,5,3,4));

   auto exit_pic =  gClient->GetPicture("ed_quit.png");
   auto *exit = new TGPictureButton(hframe, exit_pic);
//   TGTextButton *exit = new TGTextButton(hframe,"&Exit");
   exit->Connect("Clicked()","RasterMonGui",this,"Exit()");
   hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));
   AddFrame(hframe,new TGLayoutHints(kLHintsCenterX,2,2,2,2));

   auto *config = new TGTextButton( hframe, "Config");
   config->Connect("Clicked()","RasterMonGui", this, "DoConfigure()");
   hframe->AddFrame(config, new TGLayoutHints(kLHintsRight, 50,5,3,4));
}

void RasterMonGui::AddStatusBar() {
   // status bar
   Int_t parts[] = {10, 20, 20, 35, 15};
   fStatusBar = std::make_unique<TGStatusBar>(this, 50, 10, kVerticalFrame);
   fStatusBar->SetParts(parts, 5);
   fStatusBar->Draw3DCorner(kFALSE);
   AddFrame(fStatusBar.get(), new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));

}

void RasterMonGui::StatusBarUpdate(){
   // Update the information in the status bar.
   static int n_updates = 0;
   static unsigned long last_event_count=0;
   static auto time0 = std::chrono::system_clock::now();
   static auto time1 = std::chrono::system_clock::now();

   n_updates++;
   char text0[10];
   sprintf(text0, "%'8d", n_updates);
   fStatusBar->SetText( text0, 0);
   char text1[100];
   sprintf(text1,"Current event: %'10ld",fEvio->fMostRecentEventNumber);
   fStatusBar->SetText( text1, 1);
   sprintf(text1,"Nevt: %'12ld",fEvio->fNEventsProcessed);
   fStatusBar->SetText( text1, 2);

   auto delta_evt = fEvio->fNEventsProcessed - last_event_count;
   last_event_count = fEvio->fNEventsProcessed;
   auto time2 = std::chrono::system_clock::now();
   auto delta_t = std::chrono::duration_cast<std::chrono::microseconds>(time2-time1);
   auto total_t = std::chrono::duration_cast<std::chrono::microseconds>(time2-time0);
   sprintf(text1,"<rate> = %8.3f kHz i: %8.3f kHz",
          1000.*fEvio->fNEventsProcessed/total_t.count() ,1000.*delta_evt/delta_t.count());
   fStatusBar->SetText( text1, 3);

   time1 = time2;
   if(fDebug>1){
      printf("Events processed:   %'10ld    delta: %5ld     Update #%5d\n", fEvio->fNEventsProcessed, delta_evt, n_updates);
      printf("Time elapsed total: %'10ld   delta t: %5ld\n", (long)total_t.count(), (long)delta_t.count());
      printf("Average rate:       %8.3f kHz  Current rate: %8.3f kHz\n",
             1000.*fEvio->fNEventsProcessed/total_t.count() ,1000.*delta_evt/delta_t.count());
   }
}

void RasterMonGui::AddTabArea(UInt_t w, UInt_t h) {
   //--------- create the Tab widget
   fRHists->InitTabs();
   fTabAreaTabs = fRHists->AddTabArea(this, w, h);
   AddFrame(fTabAreaTabs, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,
                                            2, 2, 5, 1));
}

void RasterMonGui::SetupGUI(UInt_t w, UInt_t h){
   // Menu bar
   AddMenuBar();
   //
   AddTabArea(w,h);
   //
   AddControlBar();
   //
   AddStatusBar();
   // Sets window name and shows the main frame
   SetWindowName("RasterMon");
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
}

void RasterMonGui::HandleMenu(int choice) {
   // Handle the menu choices.
   TRootHelpDialog *hd;
   ETConnectionConfig *et_dialog;
   RasterMonConfig    *config_dialog;

   switch (choice) {
      case M_FILE_OPEN:
         // Set the file, or file list, to open by EvioTool.
         new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fFileInfo);
         if(fFileInfo.fFilename){
            if(fDebug) std::cout << "Adding file " << fFileInfo.fFilename << "\n";
            if(fEvio->fiInputFile >= fEvio->fInputFiles.size()) fEvio->fiInputFile--; // The Next() in fEvio will increase this again before opening next file!
            fEvio->AddFile(fFileInfo.fFilename);
         }
         if(fFileInfo.fFileNamesList){
            if(fEvio->fiInputFile >= fEvio->fInputFiles.size()) fEvio->fiInputFile--;
            for(const auto &&ff: *fFileInfo.fFileNamesList){
               if(fDebug) std::cout << "Adding file " << ff->GetName() << "\n";
               fEvio->AddFile(ff->GetName());
            }

         }
         break;

      case M_HISTOGRAM_SAVE:
         new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fSaveFileInfo);
         if(fSaveFileInfo.fFilename){
            if(fDebug) std::cout << "Saving histograms to file: " << fSaveFileInfo.fFilename << "\n";
            string save_file(fSaveFileInfo.fFilename);
            std::string::size_type const p(save_file.find_last_of('.')); // Find extension.
            auto file_ending = save_file.substr(p+1);
            string file_stub = save_file.substr(0, p);

            if( file_ending == "" ) {   // no extension, save as pdf
               file_ending = "pdf";
            }

            if( file_ending == "pdf"){
               fRHists->SavePDF(save_file);
            }else if ( file_ending == "root"){
               fRHists->SaveRoot(save_file, fSaveFileInfo.fOverwrite);
            }else {  // Try the ROOT "SaveAs" for each canvas.
               fRHists->SaveImageFile(file_stub, file_ending);
            }
            break;
            }
         break;

      case M_HELP_ABOUT:
         hd = new TRootHelpDialog(this, "About RasterMon ...", 550, 250);
         hd->SetText(""
                     "===============  RasterMonGui ===============\n"
                     "This is a simple GUI program to monitor the Raster in the RGC run.\n"
                     "It can open an EVIO file or attach to the CLAS12 ET ring to process\n"
                     "the data coming from the experiment.\n");
         hd->Popup();
         break;
      case M_ET_CONNECT:
         if(fRHists->IsWorking()){
            std::cout << "Don't mess with the ET while we are processing data! Called Stop. \n";
            fRHists->Stop();
         }
         et_dialog = new ETConnectionConfig(this, fEvio);
         et_dialog->Run();
         break;

      case M_CONFIGURE:
         // Stop();
         DoConfigure();
         break;

      case M_FILE_EXIT:
         // close the window and quit application
         Exit();
         break;

   }
}

void RasterMonGui::DoConfigure(){
   cout << "Start Configure dialog.";
   fConfig = new RasterMonConfig(this, fEvio, fRHists.get());
   fConfig->fRasGui = this;
   fConfig->fRefreshRate = fUpdateRate;
   fConfig->Run();
   cout << "Config Run() is done.\n";
}

void RasterMonGui::DoDraw() {
   // This is a Draw for the tab selected canvas.
   if(fUpdateSelectedTabOnly){
      int current_tab = fTabAreaTabs->GetCurrent();
      fRHists->DoDraw(current_tab);
   }else {
      fRHists->DoDraw(-1);
   }
   if (!fRHists->IsWorking() && !fPause) Stop();  // Stop if you detect the worker threads ended.
   // But not if Paused, because Pause calls DoDraw to make sure canvasses are updated, and we don't want to loop.
}

void RasterMonGui::Pause(int set_state){
   // Pause or UnPause, there is no try.
   // If set_state == 1  then go to Pause state -- for stop()
   // If set_state == -1 then go to UnPause state -- for go()
   if(set_state == 0){
      if(fRHists->IsWorking()) {
         if (fPause) set_state = -1;
      }
      else{
         if(fDebug) std::cout << "Cannot Pause when I am not working. \n";
         return;
      }
   }

   if(set_state == -1){
      // Unpause
      fPauseButton->SetText("&Pause");
      if(fDebug>1) std::cout << "Un-Pause \n";
      fHistUpdateTimer->TurnOn();
      fPause = false;
      fRHists->UnPause();
   }else{
      // Pause
      if(set_state == 0) fPauseButton->SetText("&UnPause");
      if(fDebug>1) std::cout << "Pause \n";
      fHistUpdateTimer->TurnOff();
      // fRHists->Pause();
      fPause = true;
      fUpdateSelectedTabOnly = false;  // Update all the canvases, so they show something.
      DoDraw();
      fUpdateSelectedTabOnly = true;
   }
}
