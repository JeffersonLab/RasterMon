//
// Created by Maurik Holtrop on 8/31/22.
//

#ifndef RASTERMON_RASTERSCALERBANK_H
#define RASTERMON_RASTERSCALERBANK_H

#include <ostream>
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

class StruckScalerBank : public Bank {

public:
   Leaf<unsigned int> *fHeader;
   Leaf<unsigned int> *fIntegrate;
   Leaf<Scaler_t> *fHelicity1; // Slot 1
   Leaf<Scaler_t> *fHelicity0; // Slot 0 + catchall

public:
   StruckScalerBank(EvioTool *mother): Bank("Bank64", 64, 0, "Scalers bank"){
      mother->AddBank(this);
      fHeader = AddLeaf<unsigned int>("header",57610, 0, "bank info");
      // Bank 57621 ('0xe115') "DSC2 Scalers raw format" aka "Integrating Scalers", Has 72 items:
      // •	block header,
      //•	event header,
      //•	scaler header,
      //•	16 TRG gated scaler values,
      //•	16 TDC gated scaler values,
      //•	16 TRG ungated scaler values,
      //•	16 TDC ungated scaler values,
      //•	gated reference,
      //•	ungated reference,
      //•	DSC2 filler (always 0),
      //•	VME filler,
      //•	bock trailer.
      // For each [TRG gated, TDC gated,  TRG ungated, TDC ungated ] the first 3 are fcup, slm and clock
      fIntegrate = AddLeaf<unsigned int>("integrate", 0xe115, 0, "Integration scalers" );
      // Bank 57637 ('0xe125') "Helicity Scalers", [31]: helicity, [30]: quartet, [29]: time interval, [28-24]: chan id
      //                                           [23:0] value.
      // Slot = 0 for Gated.
      // Slot = 1 for UnGated.
      // Channel 0/1/2 are fcup/slm/clock for
      fHelicity1 = AddLeaf<Scaler_t>("helicity1", 0xe125, 1, "Helicity scalers ungated" );  // Slot 1
      fHelicity0 = AddLeaf<Scaler_t>("helicity0", 0xe125, 0, "Helicity scalers gated" );  // Slot 0 + catch all.
   };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(StruckScalerBank, 1);
#pragma clang diagnostic pop
};


#endif //RASTERMON_RASTERSCALERBANK_H
