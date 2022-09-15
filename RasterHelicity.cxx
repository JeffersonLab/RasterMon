//
// Created by Maurik Holtrop on 9/6/22.
//

#include "RasterHelicity.h"

ClassImp(RasterHelicity);
ClassImp(HelicityRegister);

RasterHelicity::RasterHelicity(Bank *b,unsigned short itag,unsigned short inum):
                                       Leaf<unsigned int>("Helicity Data", itag, inum, "Data from helicity decoder."){
   b->AddThisLeaf(this);
}

bool RasterHelicity::PredictHelicity() {
   // Wind forward by fDelay to predict the current helicity bit.
   if( fDecoder.size() == 0) return false;
   return false;
}

void RasterHelicity::CallBack() {
   // std::cout << "RasterHelicity Call Back \n";

   // This clear is probably not needed, because event gets cleared at start of data.
   fTriggerNumber.clear();
   fTriggerTime.clear();
   fDecoder.clear();
   //
   // Parse out the data in the bank.
   //
   bool has_trig_time = false;
   for(int i=0; i< size(); ++i){
      if(At(i) & 0x80000000){   // Control words.
         unsigned int control_word = (At(i) & 0x78000000)>>27;
         switch(control_word){
            case 0:   // header
               break;
            case 1:   // footer
               break;
            case 2:   // Event Header
               // Check the size. This should not be needed?
               fTriggerNumber.push_back( At(i) & 0x0FFF); // Low 12 bits (0 - 11)
               break;
            case 3:  // Trigger time
               fTriggerTime.push_back(long(data[i]&0x00FFFFFF) + (long(data[i+1]&0x00FFFFFF)<< 24));
               i = i+1;
               break;
            case 8:  // Data header.
               if( (data[i] & 0x001F) != 14){
                  std::cout << "ERROR - Number of data words is not 14! \n";
               }
               fDecoder.push_back(reinterpret_cast<Helicity_Decoder_t *>(&data[i+1]));
               i = i+ 14;
               break;
            case 14:
               std::cout << "WARNING -- Data for Helicity Decoder is invalid. \n";
               break;
            case 15:    // Filler
               break;
         }
      }
   }
   if( !has_trig_time) fTriggerTime.push_back(0);
}

void RasterHelicity::Clear(Option_t *opt){
   fTriggerNumber.clear();
   fTriggerTime.clear();
   fDecoder.clear();
   Leaf<unsigned int>::Clear();
}

std::ostream& operator<<(std::ostream& os, Helicity_Decoder_status_t s){
   os << "N_phase: " << s.pattern_phase_count << " Hs: " << s.helicity_at_pattern_start << " H: " << s.helicity;
   os << " ps: " << s.pair_sync << " pat_s: " << s.pattern_sync << " t_settle:" << s.t_settle;
   return os;
}

std::ostream& operator<<(std::ostream& os, Helicity_Decoder_t s){
   os << std::hex;
   os << "Helicity Seed: 0x" << std::setw(8) << s.helicity_seed << " = 0b" << std::bitset<30>(s.helicity_seed) << std::endl;
   os << "Heli State   : 0x" << std::setw(8) << s.helicity_state << " = 0b" << std::bitset<30>(s.helicity_state) << std::endl;
   os << "Heli State@st: 0x" << std::setw(8) << s.helicity_state_at_start_of_pattern <<
                                    " = 0b" << std::bitset<30>(s.helicity_state_at_start_of_pattern) << std::endl;
   os << std::dec;
   os << "N rising edge: " << std::setw(5) << s.n_rising_edge << std::endl;
   os << "N falling edg: " << std::setw(5) << s.n_falling_edge << std::endl;
   os << "N patt sync  : " << std::setw(5) << s.n_pattern_sync << std::endl;
   os << "N pair sync  : " << std::setw(5) << s.n_pair_sync << std::endl;
   os << "T start stabl: " << std::setw(8) << s.time_since_start_of_stable*8 << " ns \n";
   os << "T end stable : " << std::setw(8) << s.time_since_end_of_stable*8 << " ns \n";
   os << "ΔT stable    : " << std::setw(8) << s.delta_t_last_stable*8 << " ns \n";
   os << "ΔT settle    : " << std::setw(8) << s.delta_t_last_settle*8 << " ns \n";
   os << std::hex;
   os << "Decoder stat : 0x" << std::setw(8) << s.status << std::endl;
   os << "pattern_sync : 0x" << std::setw(8) << s.pattern_sync << " = 0b" << std::bitset<32>(s.pattern_sync) << std::endl;
   os << "pair_sync    : 0x" << std::setw(8) << s.pair_sync << " = 0b" << std::bitset<32>(s.pair_sync) << std::endl;
   os << std::dec;  // RESET.
   return os;
}