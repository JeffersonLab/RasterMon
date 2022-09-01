//
// Created by Maurik Holtrop on 8/31/22.
//

#ifndef RASTERMON_RASTERSCALERBANK_H
#define RASTERMON_RASTERSCALERBANK_H

#include <ostream>
#include "Bank.h"
#include "Leaf.h"

struct Scalar_t {
   unsigned int scaler_value : 24;
   unsigned int channel_id   :  5;
   bool         time_interval:  1;
   bool         quarted_signal: 1;
   bool         helicity_signal: 1;
   friend std::ostream& operator<<(std::ostream& os, Scalar_t s);
};


class RasterScalerBank : public Bank {

public:
   Leaf<Scalar_t> *integrate;
   Leaf<Scalar_t> *helicity;

public:
   RasterScalerBank(Bank *mother,int tag, int num=0){
      auto bank64 = mother->AddBank("Bank64", 64, 0, "Scalers bank");
      integrate = bank64->AddLeaf<Scalar_t>("integrate", 0xe115, 0, "Integration scalers" );
      helicity = bank64->AddLeaf<Scalar_t>("helicity", 0xe125, 0, "Helicity scalers" );
   };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterScalerBank,1);
#pragma clang diagnostic pop
};


#endif //RASTERMON_RASTERSCALERBANK_H
