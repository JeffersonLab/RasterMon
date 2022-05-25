//
// Created by Maurik Holtrop on 5/24/22.
//
// This is a RasterMon Configuration file.
// The code is essentially a ROOT C++ macro that will be executed inside RasterMon so that it can be configured fully.
// Inside RasterMon, the code is executed as:  .x RMConfig.C(0x123456789)  - where 0x123456789 is a long integer that
// contains the location of the RHists class.
// This means several things:
// 1) You have to write valid C++ statements.
// 2) Just about anything can be modified in the config file. Too much to make a manual for it.
// 3) If you make a mistake, the code will most likely crash.
//
// Layout of this file:
// The first section is needed so that the library and header files can be found.
// Next is the name of the function, which *must* be the same as the name of the file (except the .C).
// The first line of the function is needed to convert the long integer to a pointer. The pointer then allows
// you to manipulate all configurable aspects of the program.
//
#include "TROOT.h"
R__ADD_INCLUDE_PATH(/data/CLAS12/RasterMon)   // Path to the code for locating the header files.
R__ADD_LIBRARY_PATH(/data/CLAS12/RasterMon/cmake-build-debug)
R__LOAD_LIBRARY(libRasterLib)

#include "RasterHists.h"

void RMConfig(long r_h_pointer_long) {  // Function name *must* be the same as the filename. Argument must be long.
   RasterHists *r = (RasterHists *)r_h_pointer_long;  // This is like, super dangerous and makes me really unhappy.

   if(r->fDebug) printf("We are configuring RasterHists from pointer 0x%lx \n",r_h_pointer_long);

   // Default_Initialize_Histograms(r);  // Calling this function will setup the code as if no config file was provided.

   // Tab 0:
   // Add a tab called "Main", with a canvas that is not divided: (nx=1, ny=1).
   r->fTabs.emplace_back("Main", 1, 1);
   // Add a 2-D histogram. (tab_number, pad_number, bank_tag_x, slot_x, adc_chan_x, bank_tag_y, slot_y, adc_chan_y,
   //                       name, title, nx, x_min, x_max, ny, y_min, y_max)
   r->fHists.emplace_back(59, 19, 1,  59, 19, 3,
                          "Raster_xy", "Raster Pos y vs x;x[mm];y[xx]", 400, -10., 10.,  400, -10., 10.);
   int i_r_xy = r->fHists.size()-1;
   r->fTabs.back().hists.push_back( i_r_xy); // Link this histogram to the tab just created.
   r->fTabs.back().hist_pads.push_back(0);

   // If you want to put this histogram also on another tab, remember the number. That way you do not fill it twice.
   r->fHists.back().scale_x = 0.004;  // Set the x scale factor for conversion from ADC to position.
   r->fHists.back().offset_x = -8.0;  // Set the x offset
   r->fHists.back().scale_y = 0.004;  // Set the y scale
   r->fHists.back().offset_y = -8.0;  // Set the y offset.
   r->fHists.back().draw_opt = "colz"; // Plot with colz.
   r->fHists.back().hist->SetStats(false); // No statistics box.

   // Next tab - 1
   // Add a canvas that is divided into 4 pads, in a 2x2 grid.
   // In this tab we repeat the histogram from tab 0.
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
   r->fTabs.back().hists.push_back( i_r_xy); // Add the histogram from tab 0.
   r->fTabs.back().hist_pads.push_back(2); // Show on pad 2

   Default_Setup_Raw_Raster_Tab(r);
   // Next Tab
   Default_Setup_1_Channel_Scope(r);
//   Default_Setup_3_Channel_Scope(r);

   // Overrides
   cout << "Changing the Scope buffer size.\n";
   if (r->fEvio != nullptr) r->fEvio->UpdateBufferSize(10000);
}