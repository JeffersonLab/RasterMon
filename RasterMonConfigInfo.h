//
// Created by Maurik Holtrop on 6/14/22.
//

#ifndef RASTERMON_RASTERMONCONFIGINFO_H
#define RASTERMON_RASTERMONCONFIGINFO_H

#include "TObject.h"
#include <string>

// Initial values are from the start of the run.
class RasterMonConfigInfo : public TObject {
public:
   int fDebugLevel = 0;
   int fEvioDebugLevel = 0;
   unsigned int fUpdateRate = 1000;
   bool fAutoClearOn = false;
   double fAutoClearRate = 30.;
   unsigned int fScopeBufDepth = 10000;
   double fScale_x = 0.0205;
   double fOffset_x = -43.8;
   double fScale_y =  -0.0369;
   double fOffset_y = 79.6;

   std::string fJSONFile = "RasterMonConfig.json";  //! Default name for the config file. Not stored.

   void SaveToJSON();
   void LoadFromJSON();
   void CopyInfo(RasterMonConfigInfo *that);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
   ClassDef(RasterMonConfigInfo, 1)
#pragma clang diagnostic pop
};


#endif //RASTERMON_RASTERMONCONFIGINFO_H
