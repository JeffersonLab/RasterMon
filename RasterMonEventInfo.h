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
      // Bank 57610 ('0xe10a'), 7 uint32, TI Hardware Data [???, event number, timestamp,
      //                                                    evt num high [31:16] + time stamp high [15:0] , ??, ??, ??
      RHead1 = this->AddLeaf<uint32_t>("EventInfo", 57610, 0, "Event Info");
      // Bank 57615 ('0xe10f'), 5 uint32, version num = 0, run number, event number, unix time = 0, event type = 0, roc pattern = 0 ]
      RHead2 = this->AddLeaf<uint32_t>("EventInfo2", 57615, 0, "Event RHead2");
      vector<unsigned short> tags = this->GetTags();
  };

  Leaf<uint32_t> *GetRHead1(){ return(RHead1); }
  Leaf<uint32_t> *GetRHead2(){ return(RHead2); }

  unsigned int GetVersionNumber() const {
      if(RHead1 && RHead1->size() > 1) return(RHead1->GetData(0));
      else return 0;
  };

   unsigned long GetEventNumberLong() const {
      // Event number from the 57610 bank.
      if(RHead1 && RHead1->size() > 4){
         unsigned long evt_num = RHead1->GetData(1) + ((unsigned long) (RHead1->GetData(3) & 0xffff0000) << 16);
         return(evt_num);
      }
      else return 0;
   };

  unsigned long GetTimeStamp() const {   //
     if(RHead1 && RHead1->size() > 5) {
        unsigned long time = RHead1->GetData(2) + ((unsigned long) (RHead1->GetData(3) & 0x0000ffff) << 32);
        return time;
     } else return 0;
  };

   unsigned long GetTrigger() const {
      // Return the 64 bits of the trigger.
      if(RHead1 && RHead1->size() >= 7) {
         return RHead1->GetData(4) | (((long) RHead1->GetData(5))<<32);
      }else return 0;
   }

  unsigned int GetTrigger_low() const {
     // Return the main 32 bits of the trigger.
     if(RHead1 && RHead1->size() >= 7) {
        return RHead1->GetData(4);
     }else return 0;
  }

   unsigned int GetTrigger_high() const {
      // Return the upper 32 bits of the trigger.
      if(RHead1 && RHead1->size() >= 7) {
         return RHead1->GetData(5);
      }else return 0;
   }

   unsigned int GetRunNumber() const {
     // Run number. This is only available in the 57615 bank.
      if(RHead2 && RHead2->size() >= 5){
         return RHead2->GetData(1);
      }
      else return 0;
  };

   unsigned int GetEventNumber() const {
      // Event number from the 57615 bank.
      if(RHead2 && RHead2->size() >= 5){
         return(RHead2->GetData(2));
      }
      else return 0;
   };

   unsigned int GetUnixTime() const {     // In RGC this seems to return 0.
      if(RHead2 && RHead2->size() >=5 ){
         return RHead2->GetData(3);
      }
      else return 0;
   }

   bool GetHelicity() const {
      if(RHead1 && RHead1->size() >= 7) {
         return (RHead2->GetData(5)>>1) & 0x01;
      }else return false;
   }

   bool GetHelicityValid() const {
      if(RHead1 && RHead1->size() >= 7) {
         return RHead2->GetData(5) & 0x01;
      }else{
         return false;
      }
   }

   int GetHelicity3() const {
      // 3-state version of the helicity.
      if(RHead1 && RHead1->size() >= 7) {
         if(RHead2->GetData(5) & 0x01){
            return ((RHead2->GetData(5)>>1) & 0x01)?1:-1;
         }else
            return 0;
      }else return 0;
   }


ClassDef(RasterMonEventInfo, 1);
};

#endif /* Headbank_h */
