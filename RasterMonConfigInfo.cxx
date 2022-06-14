//
// Created by Maurik Holtrop on 6/14/22.
//

#include "RasterMonConfigInfo.h"
#include "TBufferJSON.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

ClassImp(RasterMonConfigInfo);

void RasterMonConfigInfo::SaveToJSON(){
   // Open the fJSONFile and stream the config class to the file.
   TString json = TBufferJSON::ToJSON(this);
   std::ofstream out(fJSONFile);
   out << json;
   out.close();
   std::cout << "RasterMon -- Config panel information saved to: " << fJSONFile << "\n";
};

void RasterMonConfigInfo::LoadFromJSON(){
   // Open the fJSONFile and stream the config class from the file.
   try {
      std::ifstream in(fJSONFile);
      if(!in.is_open()){
         std::cout << "RasterMonConfigInfo - Could open the configuration file:" << fJSONFile << "\n";
         return;
      }
      std::stringstream in_json;
      in_json << in.rdbuf();
      std::string json{in_json.str()};
      RasterMonConfigInfo *new_info = nullptr;
      TBufferJSON::FromJSON(new_info, json.c_str());
      CopyInfo(new_info);
   } catch (const std::exception& e){
      std::cout << "RasterMonConfigInfo - Could not read the configuration file. \n";
      std::cout << e.what() << std::endl;
   }

};

void RasterMonConfigInfo::CopyInfo(RasterMonConfigInfo *that) {
   // Coppy only the relevant setting data from one RasterMonConfigInfo to self.
   fDebugLevel = that->fDebugLevel;
   fEvioDebugLevel = that->fEvioDebugLevel;
   fUpdateRate = that->fUpdateRate;
   fAutoClearOn = that->fAutoClearOn;
   fAutoClearRate = that->fAutoClearRate;
   fScopeBufDepth = that->fScopeBufDepth;
   fScale_x = that->fScale_x;
   fOffset_x = that->fOffset_x;
   fScale_y =  that->fScale_y;
   fOffset_y = that->fOffset_y;
}