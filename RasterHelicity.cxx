//
// Created by Maurik Holtrop on 9/6/22.
//
// Some code to work with the event helicity.
//
// Documentation for the decoder board is here: https://clasweb.jlab.org/wiki/images/3/3a/HelicityDecoderBoard.pdf
// From conversation with Nathan Baltzell: "The decoder board has the inverse of the helicity bits as defined in the FADCs".
// Helicity from the FADC board is documented here: https://logbooks.jlab.org/entry/3531353
// Nathan: "The injector has been in 'quad' encoding mode forever. In theory, it can also do toggle and octet" -> Stick with quad.
//
// The Struck Scaler documentation comes from: https://clasweb.jlab.org/wiki/index.php/CLAS12_DSTs#RAW::scaler
// Which points to a word document:
// https://github.com/JeffersonLab/clas12-offline-software/raw/development/common-tools/clas-detector/doc/Scaler%20information%20in%20CLAS12%202018%20data.docx
//
#include "RasterHelicity.h"
#include "RasterEvioTool.h"

ClassImp(HelicityDecoder);
ClassImp(HelicityRegister);

RasterHelicity::RasterHelicity(RasterEvioTool *mother): TNamed("RasterHelicity","Raster Helicity decoder class."), fMother(mother) {
   fBank38 = mother->AddBank("Bank38", 38, 0, "Bank containing helicity data.");
   fDecoder = new HelicityDecoder(fBank38);
   fHel_index  = fMother->AddChannel(19, 19, 0);  // Helicity
   fSync_index = fMother->AddChannel(19, 19, 2);  // Sync
   fQuad_index = fMother->AddChannel(19, 19, 4);  // Quarted

   ((RasterEvioTool *)fMother)->AddNotify( [this]() -> void{ return this->Process(); });

}

void RasterHelicity::Process() {
   int datsize = fMother->GetDataSize();
   if(datsize < 3) {
      cout << "RasterHelicity::Process() -- Cannot process, datasize is not 3! datasize = " << datsize << " \n";
   }
   if(fMother->GetData(fHel_index) < 1000){
      fHel_signal = -1;
   }else if(fMother->GetData(fHel_index) > 2500){
      fHel_signal = 1;
   }else{
      fHel_signal = 0;
   }
   fSync_signal = (fMother->GetData(fSync_index) < 2000 ? 1 : 0);
   fQuad_signal = (fMother->GetData(fQuad_index) < 2000 ? 1 : 0);

   if( fSync_signal != fLast_Sync ){
      ++fFlip_Count;
      fSync_TimeStamp = fMother->GetTimeStamp();
      if(fQuad_signal == 0){
         fHel_phase = 0;
         fHel_at_pattern_start = fHel_signal;
      }else{
         ++fHel_phase;
      }
   }
}

HelicityDecoder::HelicityDecoder(Bank *b, unsigned short itag, unsigned short inum):
                                       Leaf<unsigned int>("Helicity Data", itag, inum, "Data from helicity decoder."){
   b->AddThisLeaf(this);
}


void HelicityDecoder::CallBack() {
   // std::cout << "RasterHelicity Call Back \n";

   // This clear is probably not needed, because event gets cleared at start of data.
   fTriggerNumber.clear();
   fTriggerTime.clear();
   fDecodedData.clear();

   //
   // Parse out the data in the bank.
   //
   bool has_trig_time = false;
   // Careful here, the size() is overridden, we we need the data.size() here, that gets the size of the raw leaf.
   for(int i=0; i< data.size(); ++i){
      if(data[i] & 0x80000000){   // Control words.
         unsigned int control_word = (data[i] & 0x78000000)>>27;
         switch(control_word){
            case 0:   // header
               break;
            case 1:   // footer
               break;
            case 2:   // Event Header
               // Check the size. This should not be needed?
               fTriggerNumber.push_back( data[i] & 0x0FFF); // Low 12 bits (0 - 11)
               break;
            case 3:  // Trigger time
               fTriggerTime.push_back(long(data[i]&0x00FFFFFF) + (long(data[i+1]&0x00FFFFFF)<< 24));
               i = i+1;
               break;
            case 8:  // Data header.
               if( (data[i] & 0x001F) != 14){
                  std::cout << "ERROR - Number of data words is not 14! \n";
               }
               fDecodedData.push_back(reinterpret_cast<Helicity_Decoder_t *>(&data[i + 1]));
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

void HelicityDecoder::Clear(Option_t *opt){
   fTriggerNumber.clear();
   fTriggerTime.clear();
   fDecodedData.clear();
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
   os << "Decoder stat : " << std::setw(8) << s.status << std::endl;
   os << "pattern_sync : 0x" << std::setw(8) << s.pattern_sync << " = 0b" << std::bitset<32>(s.pattern_sync) << std::endl;
   os << "pair_sync    : 0x" << std::setw(8) << s.pair_sync << " = 0b" << std::bitset<32>(s.pair_sync) << std::endl;
   os << std::dec;  // RESET.
   return os;
}