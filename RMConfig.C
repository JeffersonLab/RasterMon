//
// Created by Maurik Holtrop on 5/24/22.
//
// The first few lines of the config file should not be changed.
#include "TROOT.h"
R__ADD_INCLUDE_PATH(/data/CLAS12/RasterMon)   // Path to the code for locating the header files.
R__ADD_LIBRARY_PATH(/data/CLAS12/RasterMon/cmake-build-debug)
R__LOAD_LIBRARY(libRasterLib)

#include "RasterHists.h"

void RMConfig(long r_h_pointer_long) {  // Function name *must* be the same as the filename. Argument must be long.
   RasterHists *r = (RasterHists *)r_h_pointer_long;  // This is like, super dangerous and makes me really unhappy.

   printf("We are configuring RasterHists from pointer 0x%lx \n",r_h_pointer_long);
   cout << "Setting up default.\n";
   // Default_Initialize_Histograms(r);
   int tab = 0;
   Default_Setup_Raster_Tab(r, tab);
   tab++;
   Default_Setup_Raw_Raster_Tab(r, tab);
   tab++;
   Default_Setup_1_Channel_Scope(r, tab);
//   Default_Setup_3_Channel_Scope(r, tab);

   // Overrides
   cout << "Changing the Scope buffer size.\n";
   if (r->fEvio != nullptr) r->fEvio->UpdateBufferSize(1000);
}