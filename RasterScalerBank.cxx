//
// Created by Maurik Holtrop on 8/31/22.
//

#include "RasterScalerBank.h"
#include "RasterEvioTool.h"

ClassImp(RasterScalerBank);
ClassImp(StruckScalerBank);

RasterScalerBank::RasterScalerBank(EvioTool *mother): Bank("RasterScalerBank",129 ,0 , "Scaler text bank."),
                                                      fMother(mother){
   fMother->AddBank(this);
   if( !fMother->CheckTag(31)){
      std::cout << "WARNING!!! -- Scaler bank is initiated, but tag=31 events are not being processed.\n";
   }
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
   if( (fDebug & 0x10) == 0x10){
      std::cout << "Scaler bank at scaler seq: " << GetScalerSequenceNumber() << "  timestamp: " << GetTimeStamp() << endl;
      if( (fDebug & 0x30) == 0x30){
         printf("%12s = %18.5f\n",GetName(8).c_str(), GetScaler(8));
      }
      if( (fDebug & 0x50) == 0x50){
         Print("");
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

////////////////////////////////////////////////////////////////////////////////////////////////////////

StruckScalerBank::StruckScalerBank(EvioTool *mother) : Bank("Bank64", 64, 0, "Scalers bank"){
   mother->AddBank(this);
   // Bank 57610 ('0xe10a') -- TI Hardware Data [???, event number, timestamp, evt num high [31:16] + time stamp high [15:0]
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
   fUngatedScalers = AddLeaf<Scaler_t>("helicity1", 0xe125, 1, "Helicity scalers ungated" );  // Slot 1
   fScalers.push_back(fUngatedScalers);
   fGatedScalers = AddLeaf<Scaler_t>("helicity0", 0xe125, 0, "Helicity scalers gated" );  // Slot 0 + catch all.
   fScalers.push_back(fGatedScalers);

   fInfo.reserve(I_NUM_SCALERINFO);
   ScalerInfo_t zero;                 // Initialized to zero.
   fInfo.assign(I_NUM_SCALERINFO, zero); // Initialize all elements in vector.

};

void StruckScalerBank::CallBack(){
   if(fIntegrate->size() >= 38) { // Should be 0 or 72
      fInfo[I_FCUP_GATED].integrated = fIntegrate->At(3);
      fInfo[I_FCUP_GATED].timestamp = GetTimeStamp();
      fInfo[I_CLOCK_GATED].integrated = fIntegrate->At(5);
      fInfo[I_CLOCK_GATED].timestamp = GetTimeStamp();
      fInfo[I_FCUP_UNGATED].integrated = fIntegrate->At(35);
      fInfo[I_FCUP_UNGATED].timestamp = GetTimeStamp();
      fInfo[I_CLOCK_UNGATED].integrated = fIntegrate->At(37);
      fInfo[I_CLOCK_UNGATED].timestamp = GetTimeStamp();
      if(fInfo[I_FCUP_GATED].integrated0 == 0 ){
         for(int i=0; i<4; ++i){
            fInfo[i].integrated0 = fInfo[i].integrated;
         }
      }
   }

   for( int i=0; i<2; ++i) {
      auto ptr = fScalers[i];
      if (ptr->size() > 0) {
         if (((ptr->At(2).time_interval ^ fTimeMarkerFlipped) == 0 &&  // Should be the 33ms period.
              (ptr->At(2).scaler_value < 2000)) ||                   // But it isn't so it is flipped.
             ((ptr->At(2).time_interval ^ fTimeMarkerFlipped) == 1 && // Should be the 500µs period.
              (ptr->At(2).scaler_value > 2000))) {                    // But it isn't so it is flipped.
            // Seems that the definition of time period flipped.
            std::cout << "fGatedStruckScalers -- Time period marker flipped sign, i= " << i;
            std::cout << " event = " << GetEventNumber() << " Timestamp = " << GetTimeStamp() << endl;
            fTimeMarkerFlipped = !fTimeMarkerFlipped;
         }

         for (int signal_offset=0; signal_offset < ptr->size(); signal_offset += 32) {
            int storage_offset = 0;
            if( (ptr->At(signal_offset).time_interval ^ fTimeMarkerFlipped) ) { // Is the 500µs period?
               storage_offset = I_FCUP_SETTLE;
            } else {
               storage_offset = I_FCUP;
            }
            if (ptr->At(signal_offset).helicity_signal) {   // PLUS polarity helicity.
               fInfo[I_FCUP + i + storage_offset].plus = ptr->At(signal_offset).scaler_value;
               fInfo[I_FCUP + i + storage_offset].minus = 0;
               fInfo[I_FCUP + i + storage_offset].plus_integrated += ptr->At(signal_offset).scaler_value;
               fInfo[I_CLOCK + i + storage_offset].plus = ptr->At(signal_offset + 2).scaler_value;
               fInfo[I_CLOCK + i + storage_offset].minus = 0;
               fInfo[I_CLOCK + i + storage_offset].plus_integrated += ptr->At(signal_offset + 2).scaler_value;
            } else {
               fInfo[I_FCUP + i + storage_offset].plus = 0;
               fInfo[I_FCUP + i + storage_offset].minus = ptr->At(signal_offset).scaler_value;
               fInfo[I_FCUP + i + storage_offset].minus_integrated += ptr->At(signal_offset).scaler_value;
               fInfo[I_CLOCK + i + storage_offset].plus = 0;
               fInfo[I_CLOCK + i + storage_offset].minus = ptr->At(signal_offset + 2).scaler_value;
               fInfo[I_CLOCK + i + storage_offset].minus_integrated += ptr->At(signal_offset + 2).scaler_value;
            }
            fInfo[I_CLOCK + i + storage_offset].timestamp = GetTimeStamp();
         }
      }
   }
   if( (fDebug & 0x10) == 0x10 && DataSize() > 0){
      std::cout << "Struck scaler bank at evt: " << GetEventNumber() << "  timestamp: " << GetTimeStamp() << endl;
      if( (fDebug & 0x30) == 0x30){
         Print("");
      }
   }
};

void StruckScalerBank::Print(const Option_t *option) const {
   // Print the bank contents.
   if(option[0] == 'r'|| option[0] == 'R') {
      std::cout << "UN-GATED STRUCK SCALERS \n";
      for (int i = 0; i < fUngatedScalers->size(); ++i) {
         if (fUngatedScalers->At(i).scaler_value > 0)
            std::cout << "(" << std::setw(3) << i << ") " << fUngatedScalers->At(i) << std::endl;
      }

      std::cout << "GATED STRUCK SCALERS \n";
      for (int i = 0; i < fGatedScalers->size(); ++i) {
         if (fGatedScalers->At(i).scaler_value > 0)
            std::cout << "(" << std::setw(3) << i << ") " << fGatedScalers->At(i) << std::endl;
      }
   }else{
      for(int i=0; i< I_NUM_SCALERINFO; ++i) std::cout << "(" << i << ") " << fInfo.at(i) << endl;
   }
}


std::ostream& operator<<(std::ostream& os, Scaler_t s){
   // Because this is a friend function, we have access to the object's private data
   os << "id: " << std::setw(2) << s.channel_id;
   if(s.time_interval) os << " s";
   else                os << " i";
   os << ": " << std::setw(9) << s.scaler_value << ", Q:" << s.quarted_signal << ", H:" << s.helicity_signal;
   return os;
}

std::ostream& operator<<(std::ostream& os, ScalerInfo_t s){
   os << "I0:" << std::setw(12) << s.integrated0 << " I:" << std::setw(12) << s.integrated;
   os << " H+:" << std::setw(6)  << s.plus  << " H-:" << std::setw(6) << s.minus;
   os << " ΣH+:" << std::setw(10) << s.plus_integrated << " ΣH-:" << std::setw(10) << s.minus_integrated;
   os << " T:" << std::setw(14) << s.timestamp;
   return os;
}

template<> int Leaf<Scaler_t>::Type(void){ return(Leaf_FADC + 1); };
ClassImp(Leaf<Scaler_t>);