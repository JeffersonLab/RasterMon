//
//  RasterMonEventInfo.h
//  HPSEvioReader
//
//  Created by Maurik Holtrop on 4/11/22.
//  Copyright © 2022 UNH. All rights reserved.
//
// For HPS data the RasterMonEventInfo is a Leaf from the tag=46 bank.
//
// RasterMonEventInfo   tag = 0xe10f (57615)   unsigned int
//  0(31:0)  "version number"
//  1(31:0)  "run number"
//  2(31:0)  "event number"
//  3(31:0)  0
//  4(31:0)  0
//  5(31:0)  0
//
//
#ifndef Headbank_h
#define Headbank_h

#include "EvioTool.h"
#include "Leaf.h"

class RasterMonEventInfo: public Bank{

public:
    Leaf<uint32_t> *RHead1 = nullptr;
    Leaf<uint32_t> *RHead2 = nullptr;

public:
  RasterMonEventInfo(){};
  RasterMonEventInfo(Bank *b, unsigned short itag=37, unsigned short inum=0): Bank("RasterMonEventInfo", itag, inum, "RasterMonEventInfo data"){
      b->AddBank((Bank *)this);
      // Bank 57610, 7 uint32, ??, event number, timestamp, ??, ??, ??, ??
      RHead1 = this->AddLeaf<uint32_t>("EventInfo", 57610, 0, "Event Info");
      // Bank 57615, 5 uint32, 0, run number, event number, 0, 0, 0
      RHead2 = this->AddLeaf<uint32_t>("EventInfo2", 57615, 0, "Event RHead2");
      vector<unsigned short> tags = this->GetTags();
  };

  Leaf<uint32_t> *GetRHead1(){ return(RHead1); }
  Leaf<uint32_t> *GetRHead2(){ return(RHead2); }
  unsigned int GetVersionNumber() const {
      if(RHead1 && RHead1->size() > 5) return(RHead1->GetData(0));
      else return(0);
  };
  unsigned int GetTimeStamp() const {
      if(RHead1 && RHead1->size() > 5) return(RHead1->GetData(2));
      else return(0);
  };

  unsigned int GetRunNumber() const {
      if(RHead2 && RHead2->size() > 4) return(RHead2->GetData(1));
      else return(0);
  };
  unsigned int GetEventNumber() const {
      if(RHead2 && RHead2->size() > 4) return(RHead2->GetData(2));
      else return(0);
  };

  ClassDef(RasterMonEventInfo, 1);
};

#endif /* Headbank_h */
