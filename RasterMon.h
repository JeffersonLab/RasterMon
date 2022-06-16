#ifndef RasterMon_h
#define RasterMon_h

#define RASTERMON_VERSION "1.2"
#define ET_DEFAULT_STATION "RasterMon"

#ifndef RASTERMON_DEFAULT_JSON_CONFIG
#define RASTERMON_DEFAULT_JSON_CONFIG "RasterMonConfig.json"
#endif

#ifndef RASTERMON_DEFAULT_JSON_CONFIG_PATH
#define RASTERMON_DEFAULT_JSON_CONFIG_PATH "/home/clasrun"
#endif


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

#include "RasterMonConfigInfo.h"
#include "RasterMonGui.h"
#include "RasterEvioTool.h"
#include "RasterMonEventInfo.h"
#include "RasterMonDefaultConfig.h"
#include "cxxopts.hpp"

#endif