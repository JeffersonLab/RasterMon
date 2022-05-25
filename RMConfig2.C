//
// Created by Maurik Holtrop on 5/24/22.
//
// The first few lines of the config file should not be changed.
#include "TROOT.h"
R__ADD_INCLUDE_PATH(/data/CLAS12/RasterMon)   // Path to the code for locating the header files.
R__ADD_LIBRARY_PATH(/data/CLAS12/RasterMon/cmake-build-debug)
R__LOAD_LIBRARY(libRasterLib)

#include "RasterHists.h"

void RMConfig2(long r_h_pointer_long) {  // Function name *must* be the same as the filename. Argument must be long.
   RasterHists *r = (RasterHists *)r_h_pointer_long;  // This is like, super dangerous and makes me really unhappy.

   printf("We are configuring RasterHists from pointer 0x%lx \n",r_h_pointer_long);
   cout << "Setting up default.\n";
   // Default_Initialize_Histograms(r);
   int tab = 0;
   r->fTabs.emplace_back("Raster", 1, 1);
   r->fHists.emplace_back(tab, 0, 59, 19, 1,  59, 19, 3,
                          "Raster_xy", "Raster Pos y vs x;x[mm];y[xx]", 400, -10., 10.,  400, -10., 10.);
   r->fTabs.back().hists.push_back( r->fHists.size()-1);
   r->fHists.back().scale_x = 0.004;
   r->fHists.back().offset_x = -8.0;
   r->fHists.back().scale_y = 0.004;
   r->fHists.back().offset_y = -8.0;
   r->fHists.back().draw_opt = "colz";
   r->fHists.back().hist->SetStats(false);

   tab++;
   r->fTabs.emplace_back("Scope", 1, 1, 0, 0);
   r->fTabs.back().grid = {1, 1};
   r->fTabs.back().calc = {1, 0}; // Calc 1 - modify the bottom margin.
   r->fGraphs.emplace_back(tab, 0, 59, 19, 1, ""
                                              "IX0", ";t (s)", "I_{x}", kRed+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fGraphs.emplace_back(tab, 0, 59, 19, 3,
                           "IY0", ";t(s)", "I_{y}", kGreen+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);

   r->fGraphs.emplace_back(tab, 0, 59, 19, 5,
                           "GX1", ";t(s)", "G(x)", kBlue+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   r->fGraphs.emplace_back(tab, 0, 59, 19, 7,
                           "GY1", ";t(s)", "G(y)", kMagenta+1, 2, true);
   r->fTabs.back().graphs.push_back(r->fGraphs.size() - 1);
   // Overrides
   cout << "Changing the Scope buffer size.\n";
   if (r->fEvio != nullptr) r->fEvio->UpdateBufferSize(1000);
}