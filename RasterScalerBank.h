//
// Created by Maurik Holtrop on 8/31/22.
//

#ifndef RASTERMON_RASTERSCALERBANK_H
#define RASTERMON_RASTERSCALERBANK_H

#include <ostream>
#include <iomanip>
#include "EvioTool.h"
#include "Bank.h"
#include "Leaf.h"

// Forward reference:
class RasterEvioTool;

struct Scaler_t {
   unsigned int scaler_value : 24;
   unsigned int channel_id   :  5;
   bool         time_interval:  1;
   bool         quarted_signal: 1;
   bool         helicity_signal: 1;
   friend std::ostream& operator<<(std::ostream& os, Scaler_t s);
};

class RasterScalerBank: public Bank {

public:
   EvioTool *fMother;
   int fDebug = 0;
   unsigned int fLastEvioEvent = 0;
   unsigned long fLastEventTimeStamp = 0;
   unsigned long fUnixTimestamp = 0;    // Offset corrected timestamp

   std::map<string, int> fScalerStoreIndex; // Stores the index to the stored scalers.
   std::vector<double> fScalerStore;        // Stores the scaler information.

   Leaf<unsigned int> *fHeader;
   Leaf<string> *fScalers;

public:
   RasterScalerBank(EvioTool *mother);

   virtual void CallBack() override;
   double GetScaler(string name);
   double GetScaler(int idx) const;

   string GetName(int idx) const;
   int GetIndex(const string &name) const;
   unsigned int GetRunNumber() const;
   unsigned int GetScalerSequenceNumber() const;
   unsigned int GetLastEventNumber() const;
   unsigned long GetUnixTimeStamp() const;
   unsigned long GetTimeStamp() const;
   size_t size() const { return fScalers->size();}

   void Print(const Option_t *option="") const override;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterScalerBank, 1);
#pragma clang diagnostic pop
};

struct ScalerInfo_t {
   unsigned long integrated0 = 0;
   unsigned long integrated = 0;
   unsigned long plus = 0;
   unsigned long minus = 0;
   unsigned long plus_integrated = 0;
   unsigned long minus_integrated = 0;
   unsigned long timestamp = 0;
   void Clear(){
      integrated0 = integrated;
      integrated=0;
      plus_integrated=0;
      minus_integrated=0;
   }
   friend std::ostream& operator<<(std::ostream& os, ScalerInfo_t s);
};

enum ScalerInfo_indexes {
   I_FCUP = 0,
   I_CLOCK = 2,
   I_FCUP_SETTLE = 4,
   I_CLOCK_SETTLE = 6,
   I_FCUP_UNGATED = 0,
   I_FCUP_GATED = 1,
   I_CLOCK_UNGATED = 2,
   I_CLOCK_GATED = 3,
   I_FCUP_UNGATED_SETTLE = 4,
   I_FCUP_GATED_SETTLE = 5,
   I_CLOCK_UNGATED_SETTLE = 6,
   I_CLOCK_GATED_SETTLE = 7,
   I_NUM_SCALERINFO = 8
};

class StruckScalerBank : public Bank {

public:
   int fDebug = 0;
   Leaf<unsigned int> *fHeader;
   Leaf<unsigned int> *fIntegrate;
   unsigned long fIntegrate_timestamp=0;
   Leaf<Scaler_t> *fUngatedScalers; // Slot 1
   Leaf<Scaler_t> *fGatedScalers;   // Slot 0 + catchall
   std::vector<Leaf<Scaler_t> *> fScalers;   // Convenience vector contains the two above.
   std::vector<unsigned long> fScalersTimestamps; // Timestamps of last fills.
   bool fTimeMarkerFlipped = false;
   // Computed values.
   double fCupSlope = 29.9046;   // beam_stop_atten*(scalerS2b - fcup_offset)/906.2 = FCup charge, but this is for 1 second.
   double fCupOffset = 4.6;      // These numbers are for 0.033ms, so 906.2*0.033 and 140*0.033.
   double fTimeStampScale = 4.e-6;

   std::vector<ScalerInfo_t> fInfo;

public:
   StruckScalerBank(EvioTool *mother);

   virtual void CallBack() override;
   void Print(const Option_t *option) const override;
   size_t DataSize() const { return fUngatedScalers->size() + fGatedScalers->size();}
   unsigned long GetTimeStamp(){
      if( fHeader->size() > 3){
         return ((unsigned long) fHeader->data[2]) + (((unsigned long)(fHeader->data[3] & 0x0000FFFF)) << 32);
      }else return 0;
   }
   unsigned long GetEventNumber(){
      if( fHeader->size() > 3){
         return ((unsigned long) fHeader->data[1]) + (((unsigned long)(fHeader->data[3] & 0xFFFF0000)) << 16);
      }else return 0;

   }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(StruckScalerBank, 1);
#pragma clang diagnostic pop
};


#endif //RASTERMON_RASTERSCALERBANK_H
