//
// Created by Maurik Holtrop on 3/28/22.
//

#ifndef RASTERMON_RASTERBANK_H
#define RASTERMON_RASTERBANK_H

#include <TObject.h>
#include <EvioTool.h>
#include <FADCdata.h>
#include <Leaf.h>
#include "RasterMonEventInfo.h"
#include "RasterScalerBank.h"
#include "RasterHelicity.h"
#include "CircularBuffer.h"
#include <mutex>

#ifndef FADC_TIME_CONVERSION
#define FADC_TIME_CONVERSION 4.0e-9   // The FADCs run at 250 MHz, so one tick is 4ns.
#endif

struct EvioSlot_t {
   unsigned short slot;
   vector<unsigned short> channels;   // Channels for this slot that we need to parse out.
   vector<unsigned short> data_index; // Index in the data array where this slot is put.
   EvioSlot_t(unsigned int slot, unsigned int channel, unsigned int i_dat): slot(slot) {
      channels.push_back(channel);
      data_index.push_back(i_dat);
   }
   int GetChannelIndex(unsigned int channel){
      auto p = std::find(channels.begin(), channels.end(), channel);
      if( p == channels.end() ) return(-1);
      return std::distance(channels.begin(), p);
   }

   int GetChannelDataIndex(unsigned int channel){
      int idx = GetChannelIndex(channel);
      if( idx>=0 ) return data_index[idx];
      else return -1;
   }

   bool HasChannel(unsigned int channel){
      auto p = std::find(channels.begin(), channels.end(), channel);
      return p != channels.end();
   }
   bool AddChannel(unsigned int channel, unsigned int i_dat){
      if( HasChannel(channel)) return false;
      // If we get here, the channel does not yet exist.
      channels.push_back(channel);
      data_index.push_back(i_dat);
      return true;
   }

   inline bool operator==(const EvioSlot_t& rhs){
      return slot == rhs.slot;
   }
   inline bool operator!=(const EvioSlot_t& rhs){
      return slot != rhs.slot;
   }
   inline bool operator==(const unsigned short rhs){
      return slot == rhs;
   }
   inline bool operator!=(const unsigned short rhs){
      return slot != rhs;
   }

};

struct EvioBank_t {
   Bank *bank = nullptr;
   Leaf<unsigned int> *TI = nullptr;
   Leaf<FADCdata> *FADC = nullptr;
   std::vector<EvioSlot_t> slots; // Slots of the FADC sub-bank we are interested in.
   EvioBank_t(Bank *parent, unsigned short tag, unsigned short slot, unsigned short channel, unsigned short data_index){
      string name = "Bank_" + to_string(tag);
      bank = parent->AddBank(name, tag, 0, "FADC Crate");
      TI = bank->AddLeaf<unsigned int>(name + "_TI", 57610, 0, name +" Trigger bank.");
      FADC = bank->AddLeaf<FADCdata>(name+"_FADC", 57601, 0, name+" FADC data");
      slots.emplace_back(slot, channel, data_index);
   }
   int GetSlotIndex(unsigned short slot){
      auto p = std::find(slots.begin(), slots.end(), slot);
      if( p != slots.end()) return std::distance(slots.begin(), p);
      else return -1;
   }

   bool HasSlot(unsigned short slot){
      return std::find(slots.begin(), slots.end(), slot) != slots.end();
   }

   int GetDataIndex(unsigned short slot, unsigned short channel){
      int idx = GetSlotIndex(slot);
      if( idx < 0) return -1;
      return slots[idx].GetChannelDataIndex(channel);
   }

   bool AddSlot(unsigned short slot, unsigned short channel, unsigned short data_index){
      // Check if slot exists by getting the index.

      int idx = GetSlotIndex(slot);
      if( idx >= 0 ){
         return slots[idx].AddChannel(channel, data_index);
      }else {
//      for(auto &s: slots){
//         if(s.slot == slot){  // Found it.
//            bool stat = s.AddChannel(channel, data_index);
//            return(stat);
//         }
//      }
//      // Not found, so add it.
         slots.emplace_back(slot, channel, data_index);
         return true;
      }
   }
   unsigned long GetRefTime() const{
      if(TI && TI->size()>3)
         return (unsigned long)TI->GetData(2) +  ((unsigned long) (TI->GetData(3) & 0x0000ffff) << 32);
      else return 0;
   }
   unsigned long GetEventNumber() const{
      if(TI && TI->size()>3)
         return TI->GetData(1);
         // This seems not activated (labelled optional) in clonbanks.xml : +  ((unsigned long) (TI->GetData(3) & 0xffff0000) << 16);
      else
         return 0;
   }

   inline bool operator==(const EvioBank_t &rhs){
      return bank->tags == rhs.bank->tags;
   }
   inline bool operator!=(const EvioBank_t &rhs){
      return bank->tags != rhs.bank->tags;
   }

   inline bool operator==(unsigned short rhs){
      // return std::find(bank->tags.begin(), bank->tags.end(), rhs) != bank->tags.end();
      return bank->CheckTag(rhs);
   }
   inline bool operator!=(unsigned short rhs){
      return !bank->CheckTag(rhs);
   }
};

class RasterEvioTool: public EvioTool{

public:
   enum EVIO_CODES{
      EVIO_SUCCESS,
      EVIO_END_OF_DATA,
      EVIO_READ_ERROR
   };

   std::vector<string> fInputFiles;
   int fiInputFile = -1;

   unsigned long fNEventsProcessed=0;
   //
   // Setup the data for the EVIO parsing.
   //
   // Note on mem: The memory will be managed by TObjArray. So no delete to be called. No unique_ptr etc.
   Leaf<unsigned int>  *fEvioHead = nullptr;  // EvioHead is tag=49152 and is always there for physics events, but not scaler.
   unsigned long fMostRecentEventNumber=0;      // Number of last actual event that was read.
   RasterMonEventInfo *fEventInfo = nullptr; // EventInfo bank.
   StruckScalerBank *fStruckScaler = nullptr;
   RasterScalerBank *fScaler = nullptr;
   RasterHelicity *fHelicity;

   size_t fAdcBufferSize = 5000;
   std::vector<EvioBank_t> fEvioBanks;
   std::vector<double> fChannelAverage;
   std::vector<double> fChannelTime;
   std::vector< CircularBuffer<double> > fTimeBuf;
   std::vector< CircularBuffer<double> > fAdcAverageBuf;
   unsigned int fLastEventNumber = 0;
   unsigned int fLastRunNumber = 0;
   unsigned long fLastTimeStamp = 0;

   std::mutex fFileLock;    // Because the Next() has a next file build in.
   std::mutex fBufferLock;  // Guard against buffer size changes.

   std::vector<std::function<void(void)>> fNotifyCalls;  // Vector of references to void functions.

public:
   explicit RasterEvioTool(string infile="");
   ~RasterEvioTool() override{
      // Nothing to destroy.
   };

   int AddChannel(unsigned short bank_tag, unsigned short slot, unsigned short channel);
   int GetBankIndex(unsigned short bank_tag);
   int GetDataIndex(unsigned short bank_tag, unsigned short slot, unsigned short channel);

   void AddFile(const string &file){
      fInputFiles.push_back(file);
   }

   void AddNotify(std::function<void(void)> func){ fNotifyCalls.push_back(func);};
   void ProcessChannels();

   int Next() override;
   void Clear(Option_t*opt) override;
   unsigned int GetEventNumber() const {
      if( fEvioHead->size()>2 ) return(fEvioHead->GetData(0));
      else return(0);
   }
   unsigned int GetLastEventNumber(){return fLastEventNumber;}
   unsigned int GetRunNumber() const {return fEventInfo->GetRunNumber();}
   unsigned int GetLastRunNumber() const {return fLastRunNumber;}
   unsigned long GetTimeStamp() const {return fEventInfo->GetTimeStamp();}
   unsigned long GetLastTimeStamp() const {return fLastTimeStamp; }
   unsigned long GetTrigger() const {return fEventInfo->GetTrigger();}
   unsigned long GetTrigger_low() const {return fEventInfo->GetTrigger_low();}
   unsigned long GetTrigger_high() const {return fEventInfo->GetTrigger_high();}
   bool GetHelicity() const {return fEventInfo->GetHelicity(); }
   bool GetHelicityValid() const {return fEventInfo->GetHelicityValid(); }
   int  GetHelicity3() const {return fEventInfo->GetHelicity3();}
   unsigned long GetTimeCrate(unsigned short i=0) const {
      if(i < fEvioBanks.size()){ // Assume first crate is raster
         return fEvioBanks[i].GetRefTime();
      }
      else return 0;
   }
   unsigned long GetEventNumberCrate(unsigned short i=0) const {
      if(fEvioBanks.size()>i){ // Assume first crate is raster
         return fEvioBanks[i].GetEventNumber();
      }
      else return 0;

   }
   unsigned int GetDataSize(){ return fChannelAverage.size(); }
   double GetData(int i){ if(i >=0 && i<fChannelAverage.size()){ return fChannelAverage[i];} else return 0;}

   size_t GetAdcBufferSize() const{ return fAdcBufferSize;}
   void UpdateBufferSize(unsigned long bufsize){
      std::lock_guard<std::mutex> _lck(fBufferLock);
      for(int i=0; i< fTimeBuf.size(); ++i) fTimeBuf[i] = CircularBuffer<double>(bufsize);
      for(int i=0; i< fAdcAverageBuf.size(); ++i) fAdcAverageBuf[i] = CircularBuffer<double>(bufsize);
      fAdcBufferSize = bufsize;
   }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterEvioTool, 0);
#pragma clang diagnostic pop

};

#endif //RASTERMON_RASTERBANK_H
