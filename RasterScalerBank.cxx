//
// Created by Maurik Holtrop on 8/31/22.
//

#include "RasterScalerBank.h"

ClassImp(RasterScalerBank);

std::ostream& operator<<(std::ostream& os, Scalar_t s){
   // Because this is a friend function, we have access to the object's private data
   os << "id: " << s.channel_id;
   if(s.time_interval) os << " interval";
   else                os << " settle_t";
   os << ": " << s.scaler_value << ", " << s.quarted_signal << ", " << s.helicity_signal;
   return os;
}

template<> int Leaf<Scalar_t>::Type(void){ return(Leaf_FADC + 1); };
ClassImp(Leaf<Scalar_t>);