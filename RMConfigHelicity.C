//
// Created by Maurik Holtrop on 5/24/22.
//
// The first few lines of the config file should not be changed.
#include "TROOT.h"
R__ADD_INCLUDE_PATH(/data/CLAS12/RasterMon)   // Path to the code for locating the header files.
R__ADD_LIBRARY_PATH(/data/CLAS12/RasterMon/cmake-build-debug)
R__LOAD_LIBRARY(libRasterLib)

#include "RasterHists.h"

void RMConfigHelicity(long r_h_pointer_long) {  // Function name *must* be the same as the filename. Argument must be long.
   RasterHists *r = (RasterHists *)r_h_pointer_long;  // This is like, super dangerous and makes me really unhappy.

   r->fTabs.emplace_back("Helicity", 2, 2);
   r->fTabs.back().logy = {true, true, true, true};

   r->fHelicity_stack = new THStack("Helicity_stack","Helicity Histograms");
  // r->fHelicity_stack->SetMinimum(1);
  // r->fHelicity_stack->SetMaximum(1E8);
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
   r->fHists.back().special_fill = kHist_Special_Fill_Helicity;
   r->fHists.back().special_draw = kHist_Special_Draw_NoDraw;
   r->fHelicity_stack->Add(r->fHists.back().GetHist());
   r->fHelicity_legend->AddEntry(r->fHists.back().GetHist(), "Helicity");

   r->fHists.emplace_back(19, 19, 2,              // Special - do not fill with raw info.
                          "Sync", "Sync", 3, -1.5, 1.5);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.back().hist->SetFillColor(kGreen);
   r->fHists.back().special_fill = kHist_Special_Fill_Helicity;
   r->fHists.back().special_draw = kHist_Special_Draw_NoDraw;
   r->fHelicity_stack->Add(r->fHists.back().GetHist());
   r->fHelicity_legend->AddEntry(r->fHists.back().GetHist(), "Sync");

   r->fHists.emplace_back(19, 19, 4,              // Special - do not fill with raw info.
                          "Quartet", "Quartet", 3, -1.5, 1.5);
   r->fTabs.back().hists.push_back(r->fHists.size()-1);
   r->fTabs.back().hist_pads.push_back(4); // Show on pad 4.
   r->fHists.back().hist->SetFillColor(kBlue);
   r->fHists.back().special_fill = kHist_Special_Fill_Helicity;
   r->fHists.back().special_draw = kHist_Special_Draw_Stack;
   r->fHelicity_stack->Add(r->fHists.back().GetHist());
   r->fHelicity_legend->AddEntry(r->fHists.back().GetHist(), "Quartet");

   /// Scope Tab

   r->fTabs.emplace_back("Scope", 1, 3);
   r->fTabs.back().grid = {1, 1, 1};
   r->fTabs.back().calc = {1, 1, 0}; // Calc 1 - modify the bottom margin.

   r->fGraphs.emplace_back(19, 19, 0, "H", ";t (s)", "H", kRed+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(1); // Show on pad 1.
   r->fGraphs.emplace_back(19, 19, 2, "Q", ";t (s)", "H", kBlue+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(2); // Show on pad 1.
   r->fGraphs.emplace_back(19, 19, 4, "S", ";t (s)", "H", kGreen+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fTabs.back().graph_pads.push_back(3); // Show on pad 1.

   r->fTabs.back().pad_link = {2, 3, 1};

   printf("We are configuring RasterHists from pointer 0x%lx \n",r_h_pointer_long);
   if (r->fEvio != nullptr) r->fEvio->UpdateBufferSize(10000);

   /////////////// Trigger Tab
   r->fTabs.emplace_back("Trigger", 1, 3);
   r->fTabs.back().logy = {true, true, true};
   r->fTabs.back().calc = {0, 0, 0}; // Calc 1 - modify the bottom margin.

   r->fHists.emplace_back(0, 0, 1, "TriggerBits", "Trigger Bits", 32, -0.5, 31.5);
   int hist1 =  r->fHists.size()-1;
   r->fTabs.back().hists.push_back(hist1);
   r->fTabs.back().hist_pads.push_back(1); // Show on pad 1.
   r->fHists.back().hist->SetFillColor(kBlue);
   r->fHists.back().special_fill = kHist_Special_Fill_Trigger;

   r->fHists.emplace_back(0, 0, 3, "TriggerBits25", "Trigger Bit 25", 32, -0.5, 31.5);
   int hist2 =  r->fHists.size()-1;
   r->fTabs.back().hists.push_back(hist2);
   r->fTabs.back().hist_pads.push_back(2);
   r->fHists.back().hist->SetFillColor(kGreen);
   r->fHists.back().special_fill = kHist_Special_Fill_Trigger;
   r->fHists.back().trigger_bits = 1<<25;

   r->fHists.emplace_back(0, 0, 2, "TriggerBits31", "Trigger Bit 31", 32, -0.5, 31.5);
   int hist3 = r->fHists.size()-1;
   r->fTabs.back().hists.push_back(hist3);
   r->fTabs.back().hist_pads.push_back(3);
   r->fHists.back().hist->SetFillColor(kRed);
   r->fHists.back().special_fill = kHist_Special_Fill_Trigger;
   r->fHists.back().trigger_bits = 1<<31;

   r->fTabs.back().hists.push_back(hist2);
   r->fTabs.back().hist_pads.push_back(1);

   r->fTabs.back().hists.push_back(hist3);
   r->fTabs.back().hist_pads.push_back(1);
}