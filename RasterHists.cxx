//
// Created by Maurik Holtrop on 3/29/22.
//
#include "RasterHists.h"

RasterHists::~RasterHists(){
// Cleanup.
}

void Default_Setup_Raster_Tab(RasterHists *r){
   r->fTabs.emplace_back("Raster", 2, 2);

   r->fHists.emplace_back(59, 19, 1,
                          "Raster_x", "Raster Pos x;x[mm]", 400, -10., 10.);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.back().scale_x = 0.004;
   r->fHists.back().offset_x = -8.0;
   r->fHists.emplace_back(59, 19, 3,
                          "Raster_y", "Raster Pos y;y[mm]", 400, -10., 10.);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(1); // Show on pad 1.
   r->fHists.back().scale_x = 0.004;
   r->fHists.back().offset_x = -8.0;
   r->fHists.emplace_back(59, 19, 1,  59, 19, 3,
                          "Raster_xy", "Raster Pos y vs x;x[mm];y[xx]", 400, -10., 10.,  400, -10., 10.);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(2); // Show on pad 2.
   r->fHists.back().scale_x = 0.004;
   r->fHists.back().offset_x = -8.0;
   r->fHists.back().scale_y = 0.004;
   r->fHists.back().offset_y = -8.0;
   r->fHists.back().draw_opt = "colz";
   r->fHists.back().hist->SetStats(false);
}
void Default_Setup_Raw_Raster_Tab(RasterHists *r){
   r->fTabs.emplace_back("Raw1", 2, 2);
   r->fHists.emplace_back(59, 19, 1,
                          "RawIx", "Raw ADC 3, I_x;ADC(1) channel", 4096, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.emplace_back(59, 19, 3,
                          "RawIy", "Raw ADC 1, I_{y};ADC(3) channel", 4096, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(1); // Show on pad 1.
   r->fHists.emplace_back(59, 19, 1, 59, 19, 3,
                          "RawIxy", "Raw ADC 3-2, I_{y} vs I_{x};ADC(1) channel;ADC(3) channel", 409, -0.5, 4095.5, 409, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(2); // Show on pad 4.
   r->fHists.back().draw_opt = "colz";
   r->fHists.back().hist->SetStats(false);
   r->fHists.emplace_back(59, 19, 5, 59, 19, 1,
                          "RawIGx", "Raw ADC 3-2, G(x) vs I_{x};ADC(1) channel; ADC(5) channel", 409, -0.5, 4095.5, 409, -0.5, 4095.5);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(3); // Show on pad 3.
   r->fHists.back().draw_opt = "colz";
   r->fHists.back().hist->SetStats(false);
}

void Default_Setup_Raw_Raster_Tab2(RasterHists *r){
   r->fTabs.emplace_back("Raw2", 2, 2);
   r->fHists.emplace_back(59, 19, 5,
                          "RawGx", "Raw ADC 5, G(x); ADC(5) channel", 4096, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.emplace_back(59, 19, 7,
                          "RawGy", "Raw ADC 7, G(y); ADC(7) channel", 4096, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(1); // Show on pad 1.
   r->fHists.emplace_back(59, 19, 5, 59, 19, 7,
                          "RawGxy", "Raw ADC 7-5, G(y) vs G(x)", 409, -0.5, 4095.5, 409, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(2); // Show on pad 2.
   r->fHists.back().draw_opt = "colz";
   r->fHists.back().hist->SetStats(false);
   r->fHists.emplace_back(59, 19, 3, 59, 19, 7,
                          "RawIGy", "Raw ADC 7-5, G(y) vs I_{y}", 409, -0.5, 4095.5, 409, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(3); // Show on pad 3.
   r->fHists.back().draw_opt = "colz";
   r->fHists.back().hist->SetStats(false);

}
void Default_Setup_Raw_Raster_Tab3(RasterHists *r){
   r->fTabs.emplace_back("Raw3", 2, 2);
   r->fHists.emplace_back(59, 19, 9,
                          "RawVx", "Raw ADC 9, G(x); ADC(5) channel", 4096, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.emplace_back(59, 19, 11,
                          "RawVy", "Raw ADC 11, G(y); ADC(7) channel", 4096, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(1); // Show on pad 1.
   r->fHists.emplace_back(59, 19, 9, 59, 19, 11,
                          "RawVxy", "Raw ADC 11-9, G(y) vs G(x)", 409, -0.5, 4095.5, 409, -0.5, 4095.5);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(2); // Show on pad 2.
   r->fHists.back().draw_opt = "colz";
   r->fHists.back().hist->SetStats(false);
}

void Default_Setup_1_Channel_Scope(RasterHists *r){
   r->fTabs.emplace_back("Scope", 1, 1, 0, 0);
   r->fTabs.back().grid = {1};
   r->fTabs.back().calc = {0}; // Calc 1 - modify the bottom margin.
   r->fGraphs.emplace_back(59, 19, 1, "IX0", ";t (s)", "I_{x}", kRed+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(0); // Show on pad 0.
   r->fGraphs.emplace_back(59, 19, 3,"IY0", ";t(s)", "I_{y}", kGreen+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(0); // Show on pad 0.
   r->fTabs.back().pad_link = {1};
}


void Default_Setup_2_Channel_Scope(RasterHists *r){
   r->fTabs.emplace_back("Scope", 1, 2, 0, 0);
   r->fTabs.back().grid = {1, 1};
   r->fTabs.back().calc = {1, 0}; // Calc 1 - modify the bottom margin.
   r->fGraphs.emplace_back(59, 19, 1, "IX0", ";t (s)", "I_{x}", kRed+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(1); // Show on pad 1.
   r->fGraphs.emplace_back(59, 19, 3, "IY0", ";t(s)", "I_{y}", kGreen+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(1); // Show on pad 1.

   r->fGraphs.emplace_back(59, 19, 5,
                           "GX1", ";t(s)", "G(x)", kBlue+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(2); // Show on pad 2.
   r->fGraphs.emplace_back(59, 19, 7,
                           "GY1", ";t(s)", "G(y)", kMagenta+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(2); // Show on pad 2.
   r->fTabs.back().pad_link = {2, 1};
}

void Default_Setup_3_Channel_Scope(RasterHists *r){
   r->fTabs.emplace_back("Scope", 1, 3, 0, 0);
   r->fTabs.back().grid = {1, 1, 1};
   r->fTabs.back().calc = {1, 1, 0}; // Calc 1 - modify the bottom margin.
   r->fGraphs.emplace_back(59, 19, 1, ""
                                              "IX0", ";t (s)", "I_{x}", kRed+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(1); // Show on pad 1.
   r->fGraphs.emplace_back(59, 19, 3,
                           "IY0", ";t(s)", "I_{y}", kGreen+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(1); // Show on pad 1.

   r->fGraphs.emplace_back(59, 19, 5,
                           "GX1", ";t(s)", "G(x)", kBlue+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(2); // Show on pad 2.
   r->fGraphs.emplace_back(59, 19, 7,
                           "GY1", ";t(s)", "G(y)", kMagenta+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(2); // Show on pad 2.

   r->fGraphs.emplace_back(59, 19, 9,
                           "VX1", ";t(s)", "V_{x}", kAzure+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(3); // Show on pad 3.
   r->fGraphs.emplace_back(59, 19, 11,
                           "VY1", ";t(s)", "V_{y}", kViolet+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(3); // Show on pad 3.
   r->fTabs.back().pad_link = {2, 3, 1};
}

void Default_Setup_Helicity(RasterHists *r){
   r->fTabs.emplace_back("Helicity", 2, 2);
   r->fTabs.back().logy = {true, true, true, true};

   r->fHelicity_stack = new THStack("Helicity_stack","Helicity Histograms");
   r->fHelicity_legend = new TLegend(0.9,0.8,1.0 ,1.0);

   r->fHists.emplace_back(19, 19, 0,
                          "HelicityRaw", "Helicity raw", 256, -2, 4097.);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(1); // Show on pad 1.

   r->fHists.emplace_back(19, 19, 2,
                          "SyncRaw", "Sync raw", 256, -2, 4097.);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(2); // Show on pad 2.

   r->fHists.emplace_back(19, 19, 4,
                          "QuartedRaw", "Quarted raw", 256, -2, 4097.);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(3); // Show on pad 3.

   r->fHists.emplace_back(19, 19, 0,              // Special - do not fill with raw info.
                          "Helicity", "Helicity", 3, -1.5, 1.5);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.back().hist->SetFillColor(kRed);
   r->fHists.back().special_fill = 1;
   r->fHists.back().special_draw = -1;
   r->fHelicity_stack->Add(r->fHists.back().GetHist());
   r->fHelicity_legend->AddEntry(r->fHists.back().GetHist(), "Helicity");

   r->fHists.emplace_back(19, 19, 2,              // Special - do not fill with raw info.
                          "Sync", "Sync", 3, -1.5, 1.5);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.back().hist->SetFillColor(kGreen);
   r->fHists.back().special_fill = 1;
   r->fHists.back().special_draw = -1;
   r->fHelicity_stack->Add(r->fHists.back().GetHist());
   r->fHelicity_legend->AddEntry(r->fHists.back().GetHist(), "Sync");

   r->fHists.emplace_back(19, 19, 4,              // Special - do not fill with raw info.
                          "Quartet", "Quartet", 3, -1.5, 1.5);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.back().hist->SetFillColor(kBlue);
   r->fHists.back().special_fill = 1;
   r->fHists.back().special_draw = 1;
   r->fHelicity_stack->Add(r->fHists.back().GetHist());
   r->fHelicity_legend->AddEntry(r->fHists.back().GetHist(), "Quartet");
}

void Default_Initialize_Histograms(RasterHists *r){
   // This function is called at the start to initialize the histograms.
   // This will setup the structures for the tabs and the channels for the scope and histograms with a default layout.
   //
   // Init with:  (bank_tag, slot, adc_chan, tab_number, pad_number, name, title, legend, color, width, show)
   // Or with:    (adc_chan, tab_number, pad_number, name, title, legend, color, width)

   // Some general styling of the app.
   gStyle->SetPaperSize(TStyle::kUSLetter);   // Output for PDF is US-letter format.
   if(r->fEvio != nullptr) r->fEvio->UpdateBufferSize(10000);   // Set default Scope buffer to 50k.
   // Note: PAD numbering starts at 1, with 0 being the Canvas (i.e. only one object on the screen)
   // TAB 0 == Histograms that are NOT raw.
   Default_Setup_Raster_Tab(r);

   // TAB 1
   Default_Setup_Raw_Raster_Tab(r);

   // TAB 2
   Default_Setup_Raw_Raster_Tab2(r);
   // TAB 3
   Default_Setup_Raw_Raster_Tab3(r);

   // TAB 4 Scope
   Default_Setup_3_Channel_Scope(r);

   // Tab 5 Helicity
   Default_Setup_Helicity(r);
}

TGTab * RasterHists::AddTabArea(TGWindow *frame, int w, int h) {
   // Add a Tab Area to the main window frame.
   fTabAreaTabs = new TGTab(frame, 1, 1);

   for( auto &tab_info: fTabs) {
      TGCompositeFrame *tab = fTabAreaTabs->AddTab(tab_info.name.c_str());
      // Note: Cannot use "emplace_back", because the copy constructor for TEmbeddedCanvas has been explicitly deleted.
      // This forces us to create the canvas with new.
      auto embedded_canvas = new TRootEmbeddedCanvas(tab_info.name.c_str(), tab, w, h);
      tab->AddFrame(embedded_canvas, new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                                       kLHintsExpandY, 5, 5, 5, 1));

      tab_info.canvas = embedded_canvas;
      SetupCanvas(tab_info);
   }
   DoDraw();
   return fTabAreaTabs;
}

void RasterHists::SetupCanvas(TabSpace_t &tab, TCanvas *canvas){
   // Process the canvasses.
   if( tab.canvas == nullptr && canvas == nullptr){
      cout << "ERROR - Tab has no canvas. ";
      return;
   }

   TCanvas *canv;
   if( canvas == nullptr) canv = dynamic_cast<TCanvas *>(tab.canvas->GetCanvas());
   else canv = canvas;

   if( tab.nx>1 || tab.ny >1) canv->Divide( (Int_t)tab.nx, (Int_t)tab.ny, tab.x_margin, tab.y_margin);
   if( tab.pad_link.size()>0 ) {  // Setup signals for pad resizing.
      for (int i=1; i <= std::min(tab.pad_link.size(), (size_t) 6); i++) {  // Up to 6 pads can be linked.
         auto pad = dynamic_cast<TPad *>(canv->cd(i));
         char slot_name[20];
         sprintf(slot_name, "SubPad%dResized()", i);
         if(fDebug>2) std::cout << "Connecing pad " << i << " RangeChanged to slot " << slot_name << std::endl;
         pad->Connect("RangeChanged()", "RasterHists", this, slot_name);
         pad->Connect("UnZoomed()", "RasterHists", this,  slot_name);
      }
   }
   for(int i=0; i< tab.logy.size(); ++i){    // Set the logy for pads that want this. Can be set/unset by clicking on pad.
      auto pad = canv->cd(i+1);
      pad->SetLogy(tab.logy[i]);
   }

   for(int i=0; i< tab.grid.size(); ++i){    // Set the grid for pads that want this.
      auto pad = canv->cd(i+1);
      pad->SetGrid(tab.grid[i], tab.grid[i]);
   }

   for(int i=0; i< tab.calc.size(); ++i){
      if( tab.calc[i] == 1){
         auto pad = canv->cd(i+1);
         pad->SetBottomMargin(0.04);
      }
   }

   // TODO: Setup the legends for each Pad that wants one.

   //   auto legend = new TLegend(0.9,0.85,1.0,1.0);
   //   legend->AddEntry(fGRaw_x.get(), "I_x");
   //   legend->AddEntry(fGRaw_y.get(),"I_y");
   //   legend->AddEntry(fGRaw2_x.get(),"G(x)");
   //   legend->AddEntry(fGRaw2_y.get(), "G(y)");
   //   legend->Draw();
}

void RasterHists::SetupData() {
   // For each histogram or graph, fill in the data fetch information so HistFillWorker's can do their job.
   // Link the EVIO data for each histogram to the data store.
   // This also tells (RasterEvioTool *)fEvio which channels to store.
   // We can only run SetupData() *after* the histograms and graphs have been setup, either from a config or with
   // the default function.

   for(auto &h_t: fHists){
      int his_index = fEvio->AddChannel(h_t.bank_tag, h_t.slot, h_t.adc_chan);
      if(his_index>=0) h_t.data_index = his_index;
      if(h_t.Is2D()){
         his_index = fEvio->AddChannel(h_t.bank_tag2, h_t.slot2, h_t.adc_chan2);
         if(his_index>=0) h_t.data_index2 = his_index;
      }
   }

   // Same for the graphs.
   for(auto &g_t: fGraphs){
      int his_index = fEvio->AddChannel(g_t.bank_tag, g_t.slot, g_t.adc_chan);
      if(his_index>=0) g_t.data_index = his_index;
   }
}

TAxis * RasterHists::GetTopAxisFromPad(TPad *pad){
   // Collect the axes from a pad and return them in a vector.
   // Note that the returned object is
   TAxis * axes;
   auto prims1 = pad->GetListOfPrimitives();
   for(int i=0; i< prims1->GetEntries(); ++i){
      if(strncmp(prims1->At(i)->ClassName(),"TGr",3) == 0){
         auto graph = (TGraph *)prims1->At(i);
         axes = graph->GetXaxis();
         break;
      }else if(strncmp(prims1->At(i)->ClassName(),"TH1",3) == 0){
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
   if (fPadSizeIsUpdating){
      // std::cout << "Pad is already updating.\n";
      return;  // This is to make sure we don't call this twice too quickly.
   }
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
      // std::cout << "Axes already the same.\n";
      fPadSizeIsUpdating = false;
   } else {
      // This zooms and un-zooms okay. BUT, if one graph has no data, the labels do not update the same.
      // Note: The mouse based zooming occurs in "TPad::ExecuteEventAxis". The final mouse up action does axis->SetRange()
      axis2->SetRange(axis1->GetFirst(), axis1->GetLast());
      fPadSizeIsUpdating = false;
      // std::cout << "Signal two modified.\n";
      two->Modified();
   }
}

void RasterHists::SubPadResized(int i) {
   // We got a signal from Pad i that it has resized.
   // We know you can only resize a pad that is on the current tab. So find the current tab and then
   // see what pad this pad is linked to, then resize the other pad.
   int current_tab = fTabAreaTabs->GetCurrent();
   auto tab = fTabs[current_tab];
   if(i > tab.pad_link.size()){
      // cout << "Wrong tab? " << i << "\n";
      return;  // Wrong tab showing. This can happen during global canvas update.
   }
   auto canvas = tab.canvas->GetCanvas();
   auto pad1 = dynamic_cast<TPad *>(canvas->GetPad(i));
   auto pad2_number = tab.pad_link[i-1];
   if(fDebug>2) std::cout << "SubPadResize -- copying axis range from pad " << i << " to pad " << pad2_number << std::endl;
   if(pad2_number>0 && pad2_number != i) {
      auto pad2 = dynamic_cast<TPad *>(canvas->GetPad(pad2_number));
      SubPadCopyRange(pad1, pad2);
   }
}

void RasterHists::ResizeScopeGraphs(unsigned long size){
   if(fDebug>1) std::cout << "Resizing the oscilloscope graphs to: " << size << std::endl;
//   fGRaw_x->Expand(size);
////   fGRaw_x->Set(size);
//   fGRaw_y->Expand(size);
////   fGRaw_y->Set(size);
//   fGRaw2_x->Expand(size);
////   fGRaw2_x->Set(size);
//   fGRaw2_y->Expand(size);
//   fGRaw2_y->Set(size);
}

void RasterHists::DrawCanvas(int tab_no, TCanvas *canvas, vector<Histogram_t> &histograms, vector<Graph_t> &graphs,
                             bool batch) {
   auto &tab = fTabs.at(tab_no);
   TCanvas *canv;
   if( canvas == nullptr ) canv = tab.canvas->GetCanvas();
   else canv = canvas;

   unsigned short max_pads=0;
   for(auto i_h: tab.hist_pads) max_pads = std::max(max_pads, i_h); // get the highest pad number.
   std::vector<int> pad_count(max_pads+1); // for counting pad occurrence. Initialized to zero.

   for(int i=0; i < tab.hists.size(); ++i) {
      int i_h = tab.hists[i];
      unsigned short pad_number = tab.hist_pads[i];
      pad_count[pad_number]++;
      auto &h_t = histograms.at(i_h);
      auto pad = canv->cd(pad_number);
      if(h_t.special_draw == 0){
         string draw_opt = h_t.draw_opt;
         if(pad_count[pad_number]>1) draw_opt += "same";
         fDrawLock.lock();
         gROOT->SetBatch(batch);
         h_t.hist->Draw(draw_opt.c_str());
         gROOT->SetBatch(false);
         fDrawLock.unlock();
         pad->Modified();
      }else if(h_t.special_draw == 1){   // The helicity stack has a separate draw.
         fDrawLock.lock();
         gROOT->SetBatch(batch);
         fHelicity_stack->Draw("nostackb");
         fHelicity_legend->Draw();
         gROOT->SetBatch(false);
         fDrawLock.unlock();
      }
   }

   //
   // To fill the graphs, we need to copy the data from the fEvio->fTimeBuf and fEvio->fAdcAverageBuf to th graph.
   // Because we *link* the x-axis of the graph pads, we want the same number of events in each graph, otherwise
   // such linking becomes a second order ROOT hell.
   // If an fEvio->fTimeBuf is empty, then just fill it with the correct data from another channel and set the
   // fEvio->fAdcAverageBuf to zero.
   // TODO: A problem occurs here when the data on linked pads is of different size. This is probably rare or not
   // TODO: occurring, so we can not worry about it for now.
   max_pads = 0;
   for(auto i_h: tab.graph_pads) max_pads = std::max(max_pads, i_h); // get the highest pad number.
   pad_count.resize(max_pads+1);
   pad_count.assign(max_pads+1, 0);
   for(int i=0; i < tab.graphs.size(); ++i){
      auto i_h = tab.graphs[i];
      unsigned short pad_number = tab.graph_pads[i];
      auto &g_t = graphs.at(i_h);
      pad_count[pad_number]++;
      auto pad = canv->cd(pad_number);
      auto graph = g_t.graph;
      string draw_option = g_t.draw_opt;
      if(pad_count[pad_number]>1) draw_option += "same";
      fDrawLock.lock();
      gROOT->SetBatch(batch);
      graph->Draw(draw_option.c_str());
      gROOT->SetBatch(false);
      fDrawLock.unlock();
      pad->Modified();
   }
   fDrawLock.lock();
   gROOT->SetBatch(batch);
   canv->Update();
   gROOT->SetBatch(false);
   fDrawLock.unlock();
}

void RasterHists::FillGraphs(int tab_no, vector<Graph_t> &graphs) {
   // Fill the graphs with the information from the fEvio buffers.
   // We don't need to do this every event, only when we update the tab that displays the graphs.
   // This is a relatively expensive operation, and gets pretty slow for big buffers.
   auto &tab = fTabs.at(tab_no);
   for (int i_h: tab.graphs) {
      auto &g_t = graphs.at(i_h);
      auto graph = g_t.graph;
      if (fEvio) {
         auto data_idx = g_t.data_index;
         if (fEvio->fTimeBuf[data_idx].empty()) {
            int not_empty = 0;
            for (; not_empty < tab.graphs.size(); ++not_empty) if (!fEvio->fTimeBuf[not_empty].empty()) break;
            if (not_empty < tab.graphs.size()) { // We found the first not empty graph.
               for (int i = 0; i < fEvio->fTimeBuf[not_empty].size(); ++i) {
                  graph->SetPoint(i, fEvio->fTimeBuf[not_empty].at(i), 0.);
               }
            } else { // All the graphs are empty!
               graph->SetPoint(0, 0., 0.);
               graph->SetPoint(1, 1., 0.);
            }
         } else {
            for (int i = 0; i < fEvio->fTimeBuf[data_idx].size(); ++i) {
               graph->SetPoint(i, fEvio->fTimeBuf[data_idx].at(i), fEvio->fAdcAverageBuf[data_idx].at(i));
            }
         }
      }
   }
}

void RasterHists::Stop(){
   fKeepWorking = false;
   for(auto &&worker : fWorkers) {
      if(fIsTryingToRead){
         // The worker is actively reading. If it is hung (no event from ET) then worker.join() will hang until
         // an event appears on the ET, which may take forever.
         std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for a whole 1 second.
         for(int i=10; i>=0 && fIsTryingToRead; --i ){
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep for another 1/2 second.
            std::cout << "The worker thread is still waiting for an event from ET. \n";
            std::cout << "We cannot stop the thread until an event is received. Waiting for " << i << " more seconds.\n";
         }
      }
      if(fIsTryingToRead) {
         std::cout
               << "Reader thread has hung. We are aborting stop(). This condition will clear with the next event on the ET.\n";
         return;  // <-- We abandon the thread? :-(.
      }
      worker.join();
   }
   fWorkers.clear();
}

void RasterHists::Go(){
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
      for(int i_tab=0; i_tab< fTabs.size(); ++i_tab) {
         FillGraphs(i_tab, fGraphs);
         DrawCanvas(i_tab, nullptr, fHists, fGraphs);
      }
   }else{
      FillGraphs(active_tab, fGraphs);
      DrawCanvas(active_tab, nullptr, fHists, fGraphs);
   }
}

void RasterHists::Clear(int active_tab){
   // Clear the histograms
   if(active_tab<0){ // Clear everything
      if(fDebug) std::cout << "RasterHists::clear() \n";
      for(auto &h_t: fHists){
         auto &h = h_t.hist;
         h->Reset();
      }
      for(auto &g_t: fGraphs){
         auto &g = g_t.graph;
         g->Set(1);  // Only one point effectively empties the graph.
         for(auto &buf: fEvio->fTimeBuf) buf.clear();  // Empty the buffers too.
         for(auto &buf: fEvio->fAdcAverageBuf) buf.clear();
      }
   }else{
      if(fDebug) std::cout << "RasterHists::clear() tab " << active_tab << "\n";
      auto tab = fTabs.at(active_tab);
      for(int i_h: tab.hists){
         fHists.at(i_h).hist->Reset();
      }
      for(int i_g: tab.graphs){
         fGraphs.at(i_g).graph->Set(1);
         fEvio->fTimeBuf.at(i_g).clear();
         fEvio->fAdcAverageBuf.at(i_g).clear();
      }
   }
   DoDraw();    // Draw so that even during a Pause the display is updated.
}

void RasterHists::HistFillWorker(int thread_num){
   TRandom3 rndm(thread_num);

   std::vector<double> local_data;
   local_data.reserve(fHists.size() + fGraphs.size());   // Probably more than needed, but okay.

   if(fDebug>0) std::cout << "RasterHists::HistFillWorker - Start thread "<< thread_num << "\n";

   while(fKeepWorking){
      if(!fPause) {
         fEvioReadLock.lock();
         fIsTryingToRead = true;
         int stat = fEvio->Next();
         fIsTryingToRead = false;
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

         // *Copy* the data. Then is is safe to release the lock.
         // If we don't copy, then the lock cannot be opened, and so there is no point in having multiple workers.
         // This copy is pretty fast, so okay even for a single thread.
         //

         local_data.clear();
         // memcpy is perhaps twice as fast ~ 1,879 ns per copy if size = 10;
         // memcpy(&local_data[0], &fEvio->fChannelAverage[0], fEvio->fChannelAverage.size() * sizeof(double));
         // Safer way to do this is assign or insert. ~ 3.7953 ns per copy is size = 10;
         local_data.assign(fEvio->fChannelAverage.begin(), fEvio->fChannelAverage.end());

         fEvioReadLock.unlock();

         ///////////////////////////////////////////////////////////////////////////////////////////////////
         // Sort and fill the histograms here...
         ///////////////////////////////////////////////////////////////////////////////////////////////////
         // The part below would benefit from multiple threads *if* the Fill() ends up being too slow.
         //
         for(auto &h: fHists) {
            if (h.special_fill == 0) {
               int indx = h.data_index;
               double x = fEvio->GetData(h.data_index)*h.scale_x + h.offset_x;
               if (h.Is2D()) {
                  double y = fEvio->GetData(h.data_index2) * h.scale_y + h.offset_y;
                  h.GetHist2D()->Fill(x, y);
               }
               else {
                  h.GetHist()->Fill(x);
               }
            }else if(h.special_fill == 1){
               // The helicity histgrams are -1 or 1
               // TODO: determine hi and low level?
               int indx = h.data_index;
               if(fEvio->GetData(indx) > 1500 )
                  h.GetHist()->Fill(1);
               else
                  h.GetHist()->Fill(-1);
            }
         }
         // Nothing to do for Scope, the buffer is filled in
         // fEvio->Next(); We just need to draw it.

      }else{
         std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
   }
   if(fDebug>0) std::cout << "RasterHists::HistFillWorker - Exit thread "<< thread_num << "\n";
}

void RasterHists::SavePDF(const string &file, bool overwrite){
   // Save the canvasses as PDF file
   DoDraw();  // Make sure they are all updated.

   for(int count =0; count < fTabs.size(); ++count){
      auto tab = fTabs.at(count);
      TCanvas *canv = tab.canvas->GetCanvas();
      string out;
      if(count == 0 && fTabs.size() > 1) out = file + "(";  // First one of set.
      else if(count == fTabs.size() -1) out = file + ")";   // Last one of set.
      else out = file;                                      // Middle page.
      canv->Print(out.c_str());
   }
};

void RasterHists::SaveCanvasesToPDF(const string &filename, std::vector<TCanvas *> *canvasses) {
   // Save the canvasses to PDF file.
   for (int i = 0; i < canvasses->size(); ++i) {
      auto canv = canvasses->at(i);
      string out = filename + to_string(i) + ".pdf";
      fDrawLock.lock();
      gROOT->SetBatch(true);
      canv->Draw();
      canv->Print(out.c_str());
      gROOT->SetBatch(false);
      fDrawLock.unlock();
      canv->Destructor();
      delete canv;
   }
   canvasses->clear();
}

std::vector<std::string> RasterHists::SaveCanvasesToImageFiles(const string &filename, const string &ending, std::vector<TCanvas *> *canvasses){
   // Save the canvasses  as set of PNG, GIF or JPG files, depending on the "ending" provided.
   // The tabs will be stored as filename_#.ending  where # is the tab number.
   // Make sure they are all updated.
   std::vector<std::string> out_filenames;
   for(int i =0; i < canvasses->size(); ++i) {
      gSystem->ProcessEvents();
      auto canv = canvasses->at(i);
      string out = filename + to_string(i) + "." + ending;
      fDrawLock.lock();
      gROOT->SetBatch(true);
      canv->Draw();
      canv->Print(out.c_str());
      gSystem->Sleep(250);
      gROOT->SetBatch(false);
      fDrawLock.unlock();
      out_filenames.push_back(out);
      canv->Close();
      canv->Destructor();
      delete canv;
   }
   canvasses->clear();

   return out_filenames;
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

      for(auto &hist: fHists) hist.GetTH1()->Write();
      for(auto &graph: fGraphs) graph.graph->Write();
      for(auto &tab: fTabs){
         TCanvas *canv = tab.canvas->GetCanvas();
         canv->Write();
      }
      out_file->Write();
      out_file->Close();

   } catch(exception e){
      std::cout << "ERROR Opening ROOT file to save histograms \n";
      std::cout << e.what() << std::endl;
   }

};
