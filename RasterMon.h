#ifndef RasterMon_h
#define RasterMon_h

#include <TROOT.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>
#include "TH1D.h"
#include "TH2D.h"
#include "TTimer.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "RasterMonGui.h"
#include "RasterEvioTool.h"
#include "RasterMonEventInfo.h"
#include "RasterMonDefaultConfig.h"
#include "cxxopts.hpp"

#define RASTERMON_VERSION "1.0.5"

#endif