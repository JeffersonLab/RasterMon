//
// Created by Maurik Holtrop on 3/29/22.
//
#include "RasterHists.h"

RasterHists::~RasterHists(){
// Cleanup.
}

void RasterHists::InitChannels() {
   // Setup the channels for the scope and histograms with a default layout.
   //
   // Init with:  (bank_tag, slot, adc_chan, tab_number, pad_number, name, title, legend, color, width, show)
   // Or with:    (adc_chan, tab_number, pad_number, name, title, legend, color, width)

   // TAB 0 == Histograms that are NOT raw.
   int tab = 0;
   fHists.emplace_back(tab, 3, 59, 19, 1, "Raster_Ix", "Raster Pos x", 400, -10., 10.);
   fHists.emplace_back(tab, 0, 59, 19, 1, "Raster_Iy", "Raster Pos y", 400, -10., 10.);
   fHists.emplace_back(tab, 1, 59, 19, 1,  59, 19, 1,
                       "Raster_Ixy", "Raster Pos y vs x", 400, -10., 10.,  400, -10., 10.);

   // TAB 1
   tab++;
   fHists.emplace_back(tab, 3, 59, 19, 1, "RawIx", "Raw ADC 3, I_x", 4096, -0.5, 4095.5);
   fHists.emplace_back(tab, 0, 59, 19, 3, "RawIy", "Raw ADC 1, I_y", 4096, -0.5, 4095.5);
   fHists.emplace_back(tab, 1, 59, 19, 1, 59, 19, 3,
                       "RawIxy", "Raw ADC 3-2, I_y vs I_x", 409, -0.5, 4095.5, 409, -0.5, 4095.5);
   fHists.emplace_back(tab, 2, 59, 19, 5, 59, 19, 1,
                       "RawIxy", "Raw ADC 3-2, G(x) vs I_x", 409, -0.5, 4095.5, 409, -0.5, 4095.5);

   // TAB 2
   tab++;
   fHists.emplace_back(tab,3,59, 19, 5, "RawGx", "Raw ADC 5, G(x)", 4096, -0.5, 4095.5);
   fHists.emplace_back(tab, 0, 59, 19, 7, "RawGy", "Raw ADC 7, G(y)", 4096, -0.5, 4095.5);
   fHists.emplace_back(tab,1,59, 19, 5, 59, 19, 7,
                       "RawGxy", "Raw ADC 7-5, G(y) vs G(x)", 409, -0.5, 4095.5, 409, -0.5, 4095.5);
   fHists.emplace_back(tab,2,59, 19, 3, 59, 19, 7,
                       "RawGxy", "Raw ADC 7-5, G(y) vs I_y", 409, -0.5, 4095.5, 409, -0.5, 4095.5);


   // TAB 3 Scope
   tab++;
   fScope.emplace_back(tab, 0, 59, 19, 1, "X0", "", "I_x", kRed+1, 2, true);
   fScope.emplace_back(tab, 0, 59, 19, 3, "Y0", "", "I_y", kGreen+1, 2, true);
   fScope.emplace_back(tab, 1,59, 19, 5, "X1", "", "G(x)", kBlue+1, 2, true);
   fScope.emplace_back(tab, 1, 59, 19, 6, "Y1", "", "G(y)", kMagenta+1, 2, true);

   // Tab4 Helicity
   tab++;
   fHists.emplace_back(tab, 0, 19, 19, 1,
                       "Helicity1", "Helicity raw", 256, -2, 4097.);
   fHists.emplace_back(tab, 2, 19, 19, 1,
                       "Helicity1", "Helicity raw", 256, -2, 4097.);
   fHists.emplace_back(tab, 3, 19, 19, 1,
                       "Helicity1", "Helicity raw", 256, -2, 4097.);
   fHists.emplace_back(tab, 1, 0, 0, 0,              // Special - do not fill with raw info.
                       "Helicity", "Helicity", 3, -1.5, 1.5);
   fHists.back().hist->SetFillColor(kRed);
   fHists.emplace_back(tab, 1, 0, 0, 0,              // Special - do not fill with raw info.
                       "Hel_Sync", "Sync", 3, -1.5, 1.5);
   fHists.back().hist->SetFillColor(kGreen);
   fHists.emplace_back(tab, 1, 0, 0, 0,              // Special - do not fill with raw info.
                       "Hel_Quartet", "Quartet", 3, -1.5, 1.5);
   fHists.back().hist->SetFillColor(kBlue);

}

TAxis * RasterHists::GetTopAxisFromPad(TPad *pad){
   // Collect the axes from a pad and return them in a vector.
   // Note that the returned object is
   TAxis * axes;
   auto prims1 = pad->GetListOfPrimitives();
   std::cout << "Scanning pad: " << pad->GetName() << std::endl;
   for(int i=0; i< prims1->GetEntries(); ++i){
      if(strncmp(prims1->At(i)->ClassName(),"TGr",3) == 0){
         std::cout << "Object at " << i << " is TGraph.\n";
         auto graph = (TGraph *)prims1->At(i);
         axes = graph->GetXaxis();
         break;
      }else if(strncmp(prims1->At(i)->ClassName(),"TH1",3) == 0){
         std::cout << "Object at " << i << " is TH1F\n";
         auto histo = (TH1 *)prims1->At(i);
         axes = histo->GetXaxis();
         break;
      }
   }
   return axes;
}

void RasterHists::SubPadCopyRange(TPad *one, TPad *two){
// Set the X axis of TPad two to be the same as that for TPad one.
// Here we assume that each histogram or TGraph has the same number of bins in one and two.
//
   if (fPadSizeIsUpdating) return;  // This is to make sure we don't call this twice too quickly.
   fPadSizeIsUpdating = true;
   auto axis1 = GetTopAxisFromPad(one);
   auto axis2 = GetTopAxisFromPad(two);
   if(axis1 == nullptr) return;
   if(axis2 == nullptr) return;
   double ax1lo = axis1->GetBinLowEdge(axis1->GetFirst());
   double ax1hi = axis1->GetBinUpEdge(axis1->GetLast());
   double ax2lo = axis2->GetBinLowEdge(axis2->GetFirst());
   double ax2hi = axis2->GetBinUpEdge(axis2->GetLast());

   if(axis1->GetFirst() == axis2->GetFirst() && axis1->GetLast() == axis2->GetLast() &&
         abs(ax1lo - ax2lo ) < 1E5 && abs(ax1hi - ax2hi ) < 1E5 ){
      std::cout << "Axes already the same.\n";
   }
   // These allow for zooming, but not unzooming. The graph is re-written.
   // axis2->SetLimits(ax1lo, ax1hi);
   // axis2->SetRangeUser( ax1lo, ax1hi);

   // This zooms and un-zooms okay. BUT, if one graph has no data, the labels do not update the same.
   // Note: The mouse based zooming occurs in "TPad::ExecuteEventAxis". The final mouse up action does axis->SetRange()
   axis2->SetRange(axis1->GetFirst(), axis1->GetLast());
   two->Modified();

   fPadSizeIsUpdating = false;
}
void RasterHists::SubPadTopResized() {
   // If the lower pad (fPadTop) is zoomed by the user using the cursor, a signal is send calling this method.
   // Here we read the new limits from the bottom x graph, and then set those limits on the top x graph,
   // causing both pads to have the same x scale.
   //
   std::cout << "RasterHists::SubPadTopResized() - \n";
   SubPadCopyRange(fPadTop, fPadBot);

}

void RasterHists::SubPadBotResized() {
   // If the lower pad (fPadBot) is zoomed by the user using the cursor, a signal is send calling this method.
   // Here we read the new limits from the bottom x graph, and then set those limits on the top x graph,
   // causing both pads to have the same x scale.
   //
   std::cout << "RasterHists::SubPadBotResized() - \n";
   SubPadCopyRange(fPadBot, fPadTop);

}

void RasterHists::ResizeScopeGraphs(unsigned long size){
   if(fDebug>1) std::cout << "Resizing the oscilloscope graphs to: " << size << std::endl;
   fGRaw_x->Expand(size);
//   fGRaw_x->Set(size);
   fGRaw_y->Expand(size);
//   fGRaw_y->Set(size);
   fGRaw2_x->Expand(size);
//   fGRaw2_x->Set(size);
   fGRaw2_y->Expand(size);
//   fGRaw2_y->Set(size);
}

void RasterHists::CreateScopeGraphs(TCanvas *canvas, int nbins) {
   // Setup the scope graphs.
   fGRaw_x = std::make_unique<TGraph>();
   fGRaw_x->Expand(nbins);
   fGRaw_x->AddPoint(0,0);
   fGRaw_x->SetLineWidth(2);
   fGRaw_x->SetLineColor(kRed);
   fGRaw_x->SetTitle("");
   fGRaw_y = std::make_unique<TGraph>();
   fGRaw_y->Expand(nbins);
   fGRaw_y->AddPoint(0,0);
   fGRaw_y->SetLineWidth(2);
   fGRaw_y->SetLineColor(kGreen);
   fGRaw_y->SetTitle("");
   fGRaw2_x = std::make_unique<TGraph>();
   fGRaw2_x->Expand(nbins);
   fGRaw2_x->AddPoint(0,0);
   fGRaw2_x->SetLineWidth(2);
   fGRaw2_x->SetLineColor(kOrange);
   fGRaw2_x->SetTitle("");
   fGRaw2_y = std::make_unique<TGraph>();
   fGRaw2_y->Expand(nbins);
   fGRaw2_y->AddPoint(0,0);
   fGRaw2_y->SetLineWidth(2);
   fGRaw2_y->SetLineColor(kBlue);
   fGRaw2_y->SetTitle("");
   fPadTop->cd();
   fGRaw_x->Draw();
   fGRaw_y->Draw("same");
   fPadBot->cd();
   fGRaw2_x->Draw();
   fGRaw2_y->Draw("same");
   fPadTop->Modified();
   fPadBot->Modified();
   canvas->cd();
   auto legend = new TLegend(0.9,0.85,1.0,1.0);
   legend->AddEntry(fGRaw_x.get(), "I_x");
   legend->AddEntry(fGRaw_y.get(),"I_y");
   legend->AddEntry(fGRaw2_x.get(),"G(x)");
   legend->AddEntry(fGRaw2_y.get(), "G(y)");
   legend->Draw();

}

void RasterHists::Setup_Histograms(TCanvas *canvas, int tab_num) {
   // Create the histograms and the corresponding tab.

   fCanvases[tab_num] = canvas;
   canvas->cd();
   string name = "Helicity";
   string name_raw = "Helicity_raw";
   std::vector<int> colors = {kRed, kGreen, kBlue};
   std::vector<string> helicity_names = {"Helicity","Sync","Quartet"};
   TVirtualPad *pad;
   TLegend *legend;

   switch(tab_num) {
      case 0:  // These are the calibrated x-y plots for the raster readback current.
         canvas->Divide(2, 2);
         fHRaster_X = std::make_unique<TH1D>("Raster_X", "Raster x;x (mm);counts", 400, -10., 10.);
         fHRaster_XY = std::make_unique<TH2D>("Raster_XY", "Raster (x,y); x(mm); y(mm)", 200, -15., 15.,
                                200, -15., 15.);
         fHRaster_XY->SetStats(false);
         fHRaster_Y = std::make_unique<TH1D>("Raster_Y", "Raster y;y (mm);counts", 400, -10., 10.);
         fHRaster_R = std::make_unique<TH1D>("Raster_R", "Raster r;r (mm);counts", 400, 0., 15.);
         break;
      case 1: // These are the uncalibrated raw ADC values for the raster readback current.
         canvas->Divide(2, 2);
         fHRaw_X = std::make_unique<TH1D>("Raster_Raw_X", "Raster Current Readback x;x (ADC);counts", 4100, -0.5, 4099.5);
         fHRaw_XY = std::make_unique<TH2D>("Raster_Raw_XY", "Raster Current Readback (x,y); x(ADC); y(ADC)", 200, 0., 4095.,
                                200, 0., 4095.);
         fHRaw_XY->SetStats(false);
         fHRaw_Y = std::make_unique<TH1D>("Raster_Raw_Y", "Raster Current Readback y;y (ADC);counts", 4100, -0.5, 4099.5);

         fHRaw2_vs_Raw1_x = std::make_unique<TH2D>("Raw2_vs_Raw1_x", "Raw2 vs Raw1 (x)", 2048, -0.5, 4095.5,
                                                   2048, -0.5, 4095.5);
         fHRaw2_vs_Raw1_x->SetStats(false);

         break;
      case 2: // These are the uncalibrated raw ADC values for the raster powersupply input.
         canvas->Divide(2, 2);
         fHRaw2_X = std::make_unique<TH1D>("Raster2_Raw_X", "Raster Generator x;x (ADC);counts", 4100, -0.5, 4099.5);
         fHRaw2_XY = std::make_unique<TH2D>("Raster2_Raw_XY", "Raster Generator (x,y); x(ADC); y(ADC)", 200, 0., 4095.,
                                           200, 0., 4095.);
         fHRaw2_XY->SetStats(false);
         fHRaw2_Y = std::make_unique<TH1D>("Raster2_Raw_Y", "Raster Generator y;y (ADC);counts", 4100, -0.5, 4099.5);

         fHRaw2_vs_Raw1_y = std::make_unique<TH2D>("Raw2_vs_Raw1_y", "Raw2 vs Raw1 (y)", 2048, -0.5, 4095.5,
                                                   2048, -0.5, 4095.5);
         fHRaw2_vs_Raw1_y->SetStats(false);
         break;
      case 3: // Here we setup the TGraphs for the "scope" function, ADC versus time.
         canvas->Divide(1,2);
         fPadTop = dynamic_cast<TPad *>(canvas->cd(1));
         fPadTop->Connect("RangeChanged()", "RasterHists", this, "SubPadTopResized()");
         // fPadTop->SetName("ADC, current read back.");
         fPadTop->SetGrid();
         fPadTop->SetBit(TPad::kCannotMove);
         fPadBot = dynamic_cast<TPad *>(canvas->cd(2));
         // fPadBot->SetName("ADC2, readout from driver.");
         fPadBot->Connect("RangeChanged()", "RasterHists", this, "SubPadBotResized()");
         // fPadBot->Connect("Resized()", "RasterHists", this, "SignalTest()"); // Signal that is called when pad *size* changes.
         fPadBot->SetGrid();
         fPadBot->SetBit(TPad::kCannotMove);

         if(fEvio) CreateScopeGraphs(canvas, fEvio->fN_buf);
         else CreateScopeGraphs(canvas, 10000);
         break;

      case 4:
         canvas->Divide(2, 2);
         pad = canvas->cd(1);
         pad->SetLogy(1);
         pad = canvas->cd(3);
         pad->SetLogy(1);
         pad = canvas->cd(4);
         pad->SetLogy(1);

         fHelicity_stack = std::make_unique<THStack>("Helicity_stack","Helicity Histograms");
         for(int i=0; i<3; ++i){
            fHelicity.emplace_back(name + i,helicity_names[i].c_str(), 3, -1.5,1.5);
            fHelicity[i].SetFillColor(colors[i]);
            fHelicity_raw.emplace_back(name_raw + i, (helicity_names[i]+" raw").c_str(), 256, -2.,4097.);
            fHelicity_raw[i].SetFillColor(colors[i]);
        }

         fHelicity_legend = std::make_unique<TLegend>(0.1, 0.7, 0.4, 0.9);
         for(int i=0; i<fHelicity.size(); ++i){   // NOTE: Some really obscure ROOT error does not permit these to be added in the loop above.
            fHelicity_stack->Add( &(fHelicity[i]));
            fHelicity_legend->AddEntry(&(fHelicity[i]));
         }
         break;

      default:
         cout << "ERROR - setting up a tab too many.\n";
   }

   canvas->Draw();
   DrawCanvas(tab_num);

}

void RasterHists::TopUpBuffer(CircularBuffer<double> &buf){
   // This function is used to fill the buffer with the same points until the end.
   // Doing so avoids the graphs from having zeros that distort the picture.
   // ToDo: Check if it is better to resize the TGraph each time.

   if(!buf.full() && !buf.empty()){
      for(int i=buf.size(); i< buf.capacity(); ++i){
         buf.push_back(buf.back());
      }
   }
}

void RasterHists::DrawCanvas(int hist_no) {
   {
      // Draw the histograms on the selected canvas.
      auto canvas = fCanvases[hist_no];
      switch(hist_no){
         case 0:
            canvas->cd(1);
            fHRaster_Y->Draw("E");
            canvas->cd(2);
            fHRaster_XY->Draw("colz");
            canvas->cd(3);
            fHRaster_R->Draw("E");
            canvas->cd(4);
            fHRaster_X->Draw("E");
            break;
         case 1:
            canvas->cd(1);
            fHRaw_Y->Draw("");
            canvas->cd(2);
            fHRaw_XY->Draw("colz");
            canvas->cd(3);
            fHRaw2_vs_Raw1_x->Draw("colz");
            canvas->cd(4);
            fHRaw_X->Draw("");
            break;
         case 2:
            canvas->cd(1);
            fHRaw2_Y->Draw("");
            canvas->cd(2);
            fHRaw2_XY->Draw("colz");
            canvas->cd(3);
            fHRaw2_vs_Raw1_y->Draw("colz");
            canvas->cd(4);
            fHRaw2_X->Draw("");
            break;
         case 3:
            if(fEvio){
               for (int i = 0; i < fEvio->fRasterTimeBuf[0].size(); ++i){
                  fGRaw_x->SetPoint(i, fEvio->fRasterTimeBuf[0].at(i), fEvio->fRasterAdcBuf[0].at(i));
                  if(fEvio->fRasterTimeBuf[1].empty()) fGRaw_y->SetPoint(i, fEvio->fRasterTimeBuf[0].at(i), 0.);
                  if(fEvio->fRasterTimeBuf[2].empty()) fGRaw2_x->SetPoint(i, fEvio->fRasterTimeBuf[0].at(i), 0.);
                  if(fEvio->fRasterTimeBuf[3].empty()) fGRaw2_y->SetPoint(i, fEvio->fRasterTimeBuf[0].at(i), 0.);
               }
               for (int i = 0; i < fEvio->fRasterTimeBuf[1].size(); ++i)
                  fGRaw_y->SetPoint(i, fEvio->fRasterTimeBuf[1].at(i), fEvio->fRasterAdcBuf[1].at(i));
               for (int i = 0; i < fEvio->fRasterTimeBuf[2].size(); ++i)
                  fGRaw2_x->SetPoint(i, fEvio->fRasterTimeBuf[2].at(i), fEvio->fRasterAdcBuf[2].at(i));
               for (int i = 0; i < fEvio->fRasterTimeBuf[3].size(); ++i)
                  fGRaw2_y->SetPoint(i, fEvio->fRasterTimeBuf[3].at(i), fEvio->fRasterAdcBuf[3].at(i));
            }
            fGRaw_x->SetMinimum(0);
            fGRaw_x->SetMaximum(4096);
            fGRaw2_x->SetMinimum(0);
            fGRaw2_x->SetMaximum(4096);

            fPadTop->cd();
            fGRaw_x->Draw();
            if(fGRaw_y->GetN()) fGRaw_y->Draw("same");

            fPadBot->cd();
            fGRaw2_x->Draw();
            if(fGRaw2_y->GetN()) fGRaw2_y->Draw("same");

            break;
         case 4:
            canvas->cd(1);
            fHelicity_raw[0].Draw();
            canvas->cd(2);
            fHelicity_stack->Draw("nostackb");
            fHelicity_legend->Draw();
            canvas->cd(3);
            fHelicity_raw[1].Draw();
            canvas->cd(4);
            fHelicity_raw[2].Draw();
            break;
      }
      canvas->Update();
   }
}

void RasterHists::stop(){
   fKeepWorking = false;
   for(auto &&worker : fWorkers) {
      worker.join();
   }
   fWorkers.clear();
}

void RasterHists::go(){
   fPause = false;
   if(fKeepWorking) return;  // Do not allow multiple go() clicks.
   fKeepWorking = true;
   // Start up the thread workers to fill the histogram.
   if(fDebug) std::cout << "Start " <<  fNWorkers << " workers.\n";
   for(int i=0; i< fNWorkers; ++i) {
      fWorkers.emplace_back(&RasterHists::HistFillWorker, this, i);
   }
}

void RasterHists::DoDraw(int active_tab){
   if(active_tab == -1) {
      for (int i = 0; i < fCanvases.size(); ++i) {
         DrawCanvas(i);
      }
   }else{
      DrawCanvas(active_tab);
   }
}

void RasterHists::clear(){
   // Clear the histograms
   if(fDebug) std::cout << "RasterHists::clear() \n";
   fHRaster_X->Reset();
   fHRaster_Y->Reset();
   fHRaster_XY->Reset();
   fHRaster_R->Reset();
   fHRaw_X->Reset();
   fHRaw_XY->Reset();
   fHRaw_Y->Reset();
   fHRaw2_X->Reset();
   fHRaw2_XY->Reset();
   fHRaw2_Y->Reset();
   fHRaw2_vs_Raw1_x->Reset();
   fHRaw2_vs_Raw1_y->Reset();

   for(auto &h: fHelicity ) h.Reset();
   for(auto &h: fHelicity_raw ) h.Reset();
   DoDraw();    // Draw so that even during a pause the display is updated.
}

void RasterHists::HistFillWorker(int thread_num){
   TRandom3 rndm(thread_num);

   double time = 0;
   double r = 0.1;
   double theta = 0;
   double helicity[3];
   double raster_raw[4];

   if(fDebug>0) std::cout << "RasterHists::HistFillWorker - Start thread "<< thread_num << "\n";

   while(fKeepWorking){
      if(!fPause) {
         fEvioReadLock.lock();
         int stat = fEvio->Next();
         if(stat != EvioTool::EvioTool_Status_OK){
            fKeepWorking = false;
            fEvioReadLock.unlock();
            break;
         }
         if(fEvio->GetEventNumber() == 0){  // Event number = 0 does not have useful data for us.
            fEvioReadLock.unlock();
            continue;
         }
         fEvio->fMostRecentEventNumber = fEvio->GetEventNumber(); // For GUI to always show a useful number.

         // Copy Helicity and Raster data.
         for(int i=0; i<fEvio->GetHelicitySize(); ++i){
            helicity[i] = fEvio->GetHelicity(i);
         }
         for(int i=0; i<fEvio->GetRasterSize(); ++i) {
            raster_raw[i] = fEvio->GetRaster(i);
         }

         fEvioReadLock.unlock();

         for(int i=0; i<fHelicity_raw.size(); ++i){
            fHelicity_raw[i].Fill(helicity[i]);
            if( helicity[i] > 1500.) fHelicity[i].Fill(1);
            else fHelicity[i].Fill(-1);
         }
         // std::this_thread::sleep_for(std::chrono::microseconds(30));

#ifndef FAKE_DATA_FOR_TESTING
         fHRaw_X->Fill(raster_raw[0]);
         fHRaw_Y->Fill(raster_raw[1]);
         fHRaw_XY->Fill(raster_raw[0], raster_raw[1]);

         fHRaw2_X->Fill(raster_raw[2]);
         fHRaw2_Y->Fill(raster_raw[3]);
         fHRaw2_XY->Fill(raster_raw[2], raster_raw[3]);

         fHRaw2_vs_Raw1_x->Fill( raster_raw[0], raster_raw[2]);
         fHRaw2_vs_Raw1_y->Fill( raster_raw[1], raster_raw[3]);

         double x = raster_raw[0]*fRasterScale[0] + fRasterOffset[0];
         double y = raster_raw[1]*fRasterScale[1] + fRasterOffset[1];
         fHRaster_X->Fill(x);
         fHRaster_Y->Fill(y);
         fHRaster_XY->Fill(x,y);
         double r = sqrt(x*x + y*y);
         fHRaster_R->Fill(r);
#else
         // FAKE DATA -- FOR TESTING.
         double x = r*cos(theta);
         double y = r*sin(theta);
         fHRaster_X->Fill(x);
         fHRaster_Y->Fill(y);
         fHRaster_XY->Fill(x, y);
         fHRaster_R->Fill(r);
         double rx = rndm.Gaus(2000,200);
         double ry = rndm.Gaus(2000, 300);
         fHRaw_X->Fill(rx);
         fHRaw_Y->Fill(ry);
         fHRaw_XY->Fill(rx, ry);
         time = time + 0.0002;
         // d theta/ dt = const / R
         // d R / dtheta = const.
         double dtheta = 0.001/r;
         theta = theta + dtheta;
         r = r + 0.1*dtheta;
         if(r > 10.){
            r= 0.1;
            theta =0;
         }
#endif
      }else{
         std::this_thread::sleep_for(std::chrono::milliseconds(250));
       }
   }
   if(fDebug>0) std::cout << "RasterHists::HistFillWorker - Exit thread "<< thread_num << "\n";
}

void RasterHists::SavePDF(const string &file, bool overwrite){
   // Save the canvesses as PDF file
   DoDraw();

   if(fCanvases.size()>1) {
      fCanvases[0]->Print((file+"(").c_str());
      for (int i = 1; i < fCanvases.size() - 1; ++i) {
         fCanvases[i]->Print(file.c_str());
      }
      fCanvases[fCanvases.size() - 1]->Print((file+")").c_str());
   }else{
      fCanvases[0]->Print(file.c_str());
   }
};

void RasterHists::SaveRoot(const string &file, bool overwrite){
   // Save the histograms to a ROOT file.
   std::unique_ptr<TFile> out_file;
   try {
      if (overwrite) {
         out_file = std::make_unique<TFile>(file.c_str(), "RECREATE");
      } else {
         out_file = std::make_unique<TFile>(file.c_str(), "UPDATE");
      }
      fHRaw_X->Write();
      fHRaw_Y->Write();
      fHRaw_XY->Write();
      fHRaster_X->Write();
      fHRaster_Y->Write();
      fHRaster_XY->Write();
      fHRaster_R->Write();
      for( auto h: fHelicity_raw) h.Write();
      for( auto h: fHelicity) h.Write();

      out_file->Write();
      out_file->Close();

   } catch(exception e){
      std::cout << "ERROR Opening ROOT file to save histograms \n";
      std::cout << e.what() << std::endl;
   }

};
