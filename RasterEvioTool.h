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
#include "CircularBuffer.h"
#include <mutex>

#ifndef ET_DEFAULT_PORT
#define ET_DEFAULT_PORT 11111
#endif
#ifndef ET_DEFAULT_NAME
#define ET_DEFAULT_NAME "/et/clasprod"
#endif
#ifndef ET_DEFAULT_HOST
#define ET_DEFAULT_HOST "clondaq6"
#endif

struct EvioSlot_t {
   unsigned short slot;
   vector<unsigned short> channels;   // Channels for this slot that we need to parse out.
   vector<unsigned short> data_index; // Index in the data array where this slot is put.
   EvioSlot_t(unsigned int slot, unsigned int channel, unsigned int i_dat): slot(slot) {
      channels.push_back(channel);
      data_index.push_back(i_dat);
   }
   bool AddChannel(unsigned int channel, unsigned int i_dat){
      for(auto &c: channels){
         if(c == channel){ // Channel already exists. Don't add again.
            return false;
         }
      }
      // If we get here, the channel does not yet exist.
      channels.push_back(channel);
      data_index.push_back(i_dat);
      return true;
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
      TI = bank->AddLeaf<unsigned int>(name + "_TI", 57610, 9, name +" Trigger bank.");
      FADC = bank->AddLeaf<FADCdata>(name+"_FADC", 57601, 0, name+" FADC data");
      slots.emplace_back(slot, channel, data_index);
   }
   bool AddSlot(unsigned short slot, unsigned short channel, unsigned short data_index){
      // Check if slot exists.
      for(auto &s: slots){
         if(s.slot == slot){  // Found it.
            bool stat = s.AddChannel(channel, data_index);
            return(stat);
         }
      }
      // Not found, so add it.
      slots.emplace_back(slot, channel, data_index);
      return true;
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

   string fETStationName = "RasterMon";
   int    fETPort = ET_DEFAULT_PORT;
   string fETHost = ET_DEFAULT_HOST;
   string fETName = ET_DEFAULT_NAME;

   unsigned long fNEventsProcessed=0;
   //
   // Setup the data for the EVIO parsing.
   //
   // Note on mem: The memory will be managed by TObjArray. So no delete to be called.
   Leaf<unsigned int>  *fEvioHead = nullptr;  // EvioHead is tag=49152 and is always there.
   unsigned long fMostRecentEventNumber;      // Number of last actual event that was read.
   RasterMonEventInfo *fRasterHead = nullptr; // RasterHead is tag=
   Bank   *fRasterCrate = nullptr;
   Leaf<unsigned int> *fRasterCrateTI = nullptr; // Raster Crate trigger info bank.
   Leaf<FADCdata> *fRasterFADC = nullptr;
   Bank   *fHelicityCrate = nullptr;
   Leaf<FADCdata> *fHelicityFADC = nullptr;

   size_t fN_buf = 5000;
   std::vector<EvioBank_t> fEvioBanks;
   std::vector<double> fChannelAverage;
   std::vector< CircularBuffer<double> > fTimeBuf;
   std::vector< CircularBuffer<double> > fAdcAverageBuf;

   int fHelicityBankTag = 190;
   int fHelicitySlot = 19;
   std::vector<int> fHelicityChannels = {0, 2, 4};
   std::vector<double> fHelicityChannel_data = {-1., -1., -1.}; // reserve 3 slots for the data.

   int fRasterBankTag = 159;
   int fRasterSlot = 19;
   std::vector<int> fRasterChannels = {1, 3, 5, 7};
   std::vector<double> fRasterChannel_data  = {-1., -1., -1., -1.}; // reserve slots for the data.

 //  size_t fN_buf = 5000;
   std::vector< CircularBuffer<double> > fRasterTimeBuf;
   std::vector< CircularBuffer<double> > fRasterAdcBuf;

   std::mutex fFileLock;   // Because the Next() has a next file build in.

public:
   explicit RasterEvioTool(string infile="");
   virtual ~RasterEvioTool(){
      // Nothing to destroy.
   };

   void AddChannel(unsigned short bank_tag, unsigned short slot, unsigned short channel);

   void AddFile(const string &file){
      fInputFiles.push_back(file);
   }
   int Next() override;
   void Clear(Option_t*opt) override;
   unsigned int GetEventNumber() const {
      if( fEvioHead->size()>2 ) return(fEvioHead->GetData(0));
      else return(0);
   }
   unsigned int GetRunNumber() const {return(fRasterHead->GetRunNumber());}
   unsigned int GetTimeStamp() const {return(fRasterHead->GetTimeStamp());}
   unsigned long GetTimeCrate(unsigned short i=0) const {
      if(fEvioBanks.size()>i){ // Assume first crate is raster
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

   void UpdateBufferSize(unsigned long bufsize){
      std::lock_guard<std::mutex> _lck(fFileLock);
      for(int i=0; i< fTimeBuf.size(); ++i) fTimeBuf[i] = CircularBuffer<double>(bufsize);
      for(int i=0; i< fAdcAverageBuf.size(); ++i) fAdcAverageBuf[i] = CircularBuffer<double>(bufsize);
      fN_buf = bufsize;
   }

   void SetETHost(string host) { fETHost = host; }
   string GetETHost() const { return fETHost;}
   void SetETPort(int port){ fETPort = port;}
   int GetETPort() const { return fETPort; }
   void SetETName(string name){ fETName = name;}
   string GetETName() const {return fETName;}


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterEvioTool, 0);
#pragma clang diagnostic pop

};

#endif //RASTERMON_RASTERBANK_H
