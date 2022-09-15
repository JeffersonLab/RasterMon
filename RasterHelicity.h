//
// Created by Maurik Holtrop on 9/6/22.
//

#ifndef RASTERMON_RASTERHELICITY_H
#define RASTERMON_RASTERHELICITY_H

#include <ostream>
#include <iomanip>
#include <bitset>
#include "EvioTool.h"
#include "Bank.h"
#include "Leaf.h"

struct Helicity_Decoder_status_t {   // Bitwise structure to unpack the status word. Low bit to high bit.
   bool t_settle: 1;
   bool pattern_sync: 1;
   bool pair_sync: 1;
   bool helicity: 1;
   bool helicity_at_pattern_start: 1;
   bool event_polarity: 1;   // XOR[Helicity, Helicity at pattern start]
   unsigned int zero2: 2;
   unsigned int pattern_phase_count: 4;
   unsigned int zero1: 20;
   friend std::ostream& operator<<(std::ostream& os, Helicity_Decoder_status_t s);
};

struct Helicity_Decoder_t {
   // 14 unsigned int representing the state of the helicity decoder.
   // See the document: https://clasweb.jlab.org/wiki/images/3/3a/HelicityDecoderBoard.pdf
   unsigned int helicity_seed;
   unsigned int n_rising_edge;
   unsigned int n_falling_edge;
   unsigned int n_pattern_sync;
   unsigned int n_pair_sync;
   unsigned int time_since_start_of_stable;  // Unit = 8 ns
   unsigned int time_since_end_of_stable;    // Unit = 8 ns
   unsigned int delta_t_last_settle;
   unsigned int delta_t_last_stable;
   Helicity_Decoder_status_t status; // Status at trigger time.
   unsigned int pattern_sync;
   unsigned int pair_sync;
   unsigned int helicity_state;
   unsigned int helicity_state_at_start_of_pattern;
   friend std::ostream& operator<<(std::ostream& os, Helicity_Decoder_t s);
};

class HelicityRegister {

public:
   unsigned int fRegister = 0;
   unsigned long fNpair = 0;

public:
   HelicityRegister(){};
   HelicityRegister(unsigned int seed, unsigned int npair=0): fRegister(seed), fNpair(npair) {};

   void Set(unsigned int seed, unsigned int npair=0){
      fRegister = seed;
      fNpair = npair;
   }

   bool GetHelicty(){
      return fRegister & 0x01;
   }

   HelicityRegister &operator=(unsigned int seed){
      fRegister = seed;
      fNpair=0;
      return *this;
   }

   inline bool PredictNextBit(){
      bool bit6  = (fRegister & 1<<6) !=0;
      bool bit27 = (fRegister & 1<<27) !=0;
      bool bit28 = (fRegister & 1<<28) !=0;
      bool bit29 = (fRegister & 1<<29) !=0;
      bool nextBit = (bit29^bit28^bit27^bit6);
      return nextBit;
   }

   HelicityRegister &operator++(){
      // Prefix operator: ++hel.
      // Repeats in 1073741823 cycles.
      ++fNpair;
      bool bit6  = (fRegister & 1<<6) !=0;
      bool bit27 = (fRegister & 1<<27) !=0;
      bool bit28 = (fRegister & 1<<28) !=0;
      bool bit29 = (fRegister & 1<<29) !=0;
      unsigned int next_bit = (bit29^bit28^bit27^bit6) & 0x1;
      fRegister = ((fRegister << 1) | next_bit) & 0x3FFFFFFF;
      return *this;
   };

   HelicityRegister &operator--(){
      // Prefix operator: --hel. Shift backwards.
      --fNpair;
      bool bit1 = ((fRegister) & 0x1);
      fRegister = (fRegister >> 1) & 0x1FFFFFFF;
      bool bit6  =(fRegister & 1<<6) !=0;
      bool bit27 =(fRegister & 1<<27) !=0;
      bool bit28 =(fRegister & 1<<28) !=0;

      unsigned int bit30 = (bit1^bit28^bit27^bit6) & 0x1;
      fRegister = fRegister | (bit30 << 29);
      return *this;
   }

   HelicityRegister &operator<<=(int n){
      // Shift left and assign operator, which is the same as a predict forward.
      for(int i=0; i< n; ++i){
         operator++();
      }
      return *this;
   }

   HelicityRegister &operator>>=(int n){
      // Shift left and assign operator, which is the same as a predict forward.
      for(int i=0; i< n; ++i){
         operator--();
      }
      return *this;
   }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(HelicityRegister, 1);
#pragma clang diagnostic pop
};

class RasterHelicity: public Leaf<unsigned int> {

public:

   double fFrequency = 29.5596;     // Values could come from CCDB /runcontrol/helicity
   int fDelay = 8;
   double fTSettle = 500;
   double fTStable = 33330;
   std::vector<unsigned int> fTriggerNumber;
   std::vector<unsigned long> fTriggerTime;
   std::vector<Helicity_Decoder_t *> fDecoder;


public:
   RasterHelicity(){};
   RasterHelicity(Bank *b,unsigned short itag=57651,unsigned short inum=0);

   void CallBack() override;
   void Clear(Option_t *opt="") override;
   bool PredictHelicity();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterHelicity, 1);
#pragma clang diagnostic pop
};


#endif //RASTERMON_RASTERHELICITY_H
