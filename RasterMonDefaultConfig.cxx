//
// Created by Maurik Holtrop on 5/28/22.
//

#include "RasterMonDefaultConfig.h"

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

   r->fHists.emplace_back(59, 19, 1, "Raster_R", "Raster Radius;r[mm]", 400, 0., 15.);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(3); // Show on pad 3.
   // Special case - 2 inputs but a 1-D histogrsm
   r->fHists.back().special_fill = 2;
   r->fHists.back().bank_tag2 = 59;
   r->fHists.back().slot2 = 19;
   r->fHists.back().adc_chan2 = 3;
   r->fHists.back().scale_x = 0.004;
   r->fHists.back().offset_x = -8.0;
   r->fHists.back().scale_y = 0.004;
   r->fHists.back().offset_y = -8.0;
   r->fHists.back().hist->SetLineColor(kRed);


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
