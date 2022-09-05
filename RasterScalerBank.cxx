//
// Created by Maurik Holtrop on 8/31/22.
//

#include "RasterScalerBank.h"
#include "RasterEvioTool.h"

ClassImp(RasterScalerBank);
ClassImp(StruckScalerBank);

RasterScalerBank::RasterScalerBank(EvioTool *mother): Bank("Bank129",129,0, "Scaler text bank."), fMother(mother){
   fMother->AddBank(this);
   fHeader = AddLeaf<unsigned int>("header",57618, 0, "bank info");
   fScalers = this->AddLeaf<string>("scalers", 57620, 0, "Scaler strings");
}

void RasterScalerBank::CallBack(){
   // cout << "Scaler Bank Call-Back. \n";
   RasterEvioTool *evio = (RasterEvioTool *)fMother;
   fLastEvioEvent = evio->GetLastEventNumber();
   fLastEventTimeStamp = evio->GetLastTimeStamp();
   fUnixTimestamp =  GetTimeStamp();

   for(int i=0; i< size(); ++i){
      unsigned long loc_space = fScalers->At(i).find(" ", 18);
      string name = fScalers->At(i).substr(18,loc_space - 18);
      int idx = GetIndex(name);
      if( idx < 0 ) {
         fScalerStore.push_back(std::stod(fScalers->At(i).substr(0, 18)));
         fScalerStoreIndex[name] = fScalerStore.size() -1;
      }else{
         fScalerStore[idx] = std::stod(fScalers->At(i).substr(0, 18));
      }
   }

}

double RasterScalerBank::GetScaler(string name){
   // Get scaler by name.
   int idx = GetIndex(name);
   return GetScaler(idx);
}

double RasterScalerBank::GetScaler(int idx) const{
   // Get scaler by name.
   if(idx < fScalerStore.size() && idx >= 0) {
      return fScalerStore[idx];
   }else {
      return 0.0;
   }
}

string RasterScalerBank::GetName(int idx) const{
   // This is a reverse map lookup, so slower. Try to avoid using this.
   for( auto const& x : fScalerStoreIndex){
      if( x.second == idx ) return x.first;
   }
   return string("Invalid");
}

int RasterScalerBank::GetIndex(const string &name) const{
   // Get the scaler index from the name.
   auto found = fScalerStoreIndex.find(name);
   if( found != fScalerStoreIndex.end() ){
      return found->second;
   }else{
      return(-1);
   }
}

unsigned int RasterScalerBank::GetRunNumber() const{
   if(fHeader->size() >= 5){
      return fHeader->GetData(1);
   }else{
      return(0);
   }
}

unsigned int RasterScalerBank::GetScalerSequenceNumber() const{
   if(fHeader->size() >= 5){
      return fHeader->GetData(2);
   }else{
      return(0);
   }
}

unsigned int RasterScalerBank::GetLastEventNumber() const{
   return fLastEvioEvent;
}

unsigned long RasterScalerBank::GetUnixTimeStamp() const {
   return fUnixTimestamp;
}

unsigned long RasterScalerBank::GetTimeStamp() const{
   if(fHeader->size() >= 5){
      return fHeader->GetData(3);
   }else{
      return(fUnixTimestamp);
   }
}

void RasterScalerBank::Print(const Option_t *option) const{
   // Print the scalers in the bank in a table.
   for(int i=0; i< size(); ++i){
      printf("%27s = %18.5f\n",GetName(i).c_str(), GetScaler(i));
   }
}

std::ostream& operator<<(std::ostream& os, Scaler_t s){
   // Because this is a friend function, we have access to the object's private data
   os << "id: " << s.channel_id;
   if(s.time_interval) os << " s";
   else                os << " i";
   os << ": " << s.scaler_value << ", " << s.quarted_signal << ", " << s.helicity_signal;
   return os;
}

template<> int Leaf<Scaler_t>::Type(void){ return(Leaf_FADC + 1); };
ClassImp(Leaf<Scaler_t>);