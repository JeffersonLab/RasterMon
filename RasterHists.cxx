//
// Created by Maurik Holtrop on 3/29/22.
//
#include "RasterHists.h"

RasterHists::~RasterHists(){
// Cleanup.
}

void RasterHists::Setup_Histograms(TCanvas *canvas) {
   // Create the histograms and the corresponding tab.

   fCanvases.push_back(canvas);
   canvas->cd();
   int tab_num = fCanvases.size()-1;
   string name = "Helicity";
   string name_raw = "Helicity_raw";
   std::vector<int> colors = {kRed, kGreen, kBlue};
   std::vector<string> helicity_names = {"Helicity","Sync","Quartet"};
   TVirtualPad *pad;
   switch(tab_num) {
      case 0:
         canvas->Divide(2, 2);
         fHRaster_X = std::make_unique<TH1D>("Raster_X", "Raster x;x (mm);counts", 400, -10., 10.);
         fHRaster_XY = std::make_unique<TH2D>("Raster_XY", "Raster (x,y); x(mm); y(mm)", 200, -15., 15.,
                                200, -15., 15.);
         fHRaster_Y = std::make_unique<TH1D>("Raster_Y", "Raster y;y (mm);counts", 400, -10., 10.);
         fHRaster_R = std::make_unique<TH1D>("Raster_R", "Raster r;r (mm);counts", 400, 0., 15.);
         break;
      case 1:
         canvas->Divide(2, 2);
         fHRaw_X = std::make_unique<TH1D>("Raster_Raw_X", "Raster x;x (ADC);counts", 400, 0., 4095.);
         fHRaw_XY = std::make_unique<TH2D>("Raster_Raw_XY", "Raster (x,y); x(ADC); y(ADC)", 200, 0., 4095.,
                                200, 0., 4095.);
         fHRaw_Y = std::make_unique<TH1D>("Raster_Raw_Y", "Raster y;y (ADC);counts", 400, 0., 4095.);

         break;
      case 2:
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

void RasterHists::DrawCanvas(int hist_no) {
   {
      // Draws function graphics in randomly chosen interval
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
            fHRaw_XY->Draw("");
            canvas->cd(3);
            canvas->cd(4);
            fHRaw_X->Draw("");
            break;
         case 2:
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

void RasterHists::DoDraw(){
   for( int i=0; i< fCanvases.size(); ++i) {
      DrawCanvas(i);
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
   for(auto &h: fHelicity ) h.Reset();
   for(auto &h: fHelicity_raw ) h.Reset();
   DoDraw();    // Draw so that even during a pause the display is updated.
}

void RasterHists::HistFillWorker(int thread_num){
   TRandom3 rndm(thread_num);

   double time = 0;
   double r = 0.1;
   double theta = 0;

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
         if(fEvio->GetEventNumber() == 0){
            fEvioReadLock.unlock();
            continue;
         }

         // Helicity Histograms
         for(int i=0; i<fHelicity_raw.size(); ++i){
            fHelicity_raw[i].Fill(fEvio->GetHelicity(i));
            if( fEvio->GetHelicity(i) > 1500.) fHelicity[i].Fill(1);
            else fHelicity[i].Fill(-1);
         }

         fEvioReadLock.unlock();
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
         //std::this_thread::sleep_for(std::chrono::microseconds(10));
      }else{
         std::this_thread::sleep_for(std::chrono::milliseconds(250));
       }
   }
   if(fDebug>0) std::cout << "RasterHists::HistFillWorker - Exit thread "<< thread_num << "\n";
}

void RasterHists::SavePDF(const string &file, bool overwrite){
   // Save the canvesses as PDF file
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
