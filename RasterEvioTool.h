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
   // Sub banks.
   // Note on mem: The memory will be managed by TObjArray. So no delete to be called.
   Leaf<unsigned int>  *fEvioHead = nullptr;  // EvioHead is tag=49152 and is always there.
   unsigned long fMostRecentEventNumber;      // Number of last actual event that was read.
   RasterMonEventInfo *fRasterHead = nullptr; // RasterHead is tag=
   Bank   *fRasterCrate = nullptr;
   Leaf<unsigned int> *fRasterCrateTI = nullptr; // Raster Crate trigger info bank.
   Leaf<FADCdata> *fRasterFADC = nullptr;
   Bank   *fHelicityCrate = nullptr;
   Leaf<FADCdata> *fHelicityFADC = nullptr;

   int fHelicityCrateNum = 19;
   int fHelicitySlot = 19;
   std::vector<int> fHelicityChannels = {0, 2, 4};
   std::vector<double> fHelicityChannel_data = {-1., -1., -1.}; // reserve 3 slots for the data.

   int fRasterCrateNum = 59;
   int fRasterSlot = 19;
   std::vector<int> fRasterChannels = {1, 3, 1, 3};
   std::vector<double> fRasterChannel_data = {-1., -1., -1., -1.}; // reserve slots for the data.

   std::mutex fFileLock;   // Because the Next() has a next file build in.

   size_t fN_buf = 5000;
   std::vector< CircularBuffer<double> > fRasterTimeBuf;
   std::vector< CircularBuffer<double> > fRasterAdcBuf;

public:
   explicit RasterEvioTool(string infile="");
   virtual ~RasterEvioTool(){
      // Nothing to destroy.
   };

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
   unsigned long GetTimeRasterCrate() const {
      if(fRasterCrateTI && fRasterCrateTI->size()>=5)
         return fRasterCrateTI->GetData(2) +  ((unsigned long) (fRasterCrateTI->GetData(3) & 0x0000ffff) << 32);
      else return 0;
   }
   unsigned long GetEventNumberRasterCrate() const {
      if(fRasterCrateTI && fRasterCrateTI->size()>=5)
         return fRasterCrateTI->GetData(1);
// This seems not activated (labelled optional in clonbanks.xml : +  ((unsigned long) (fRasterCrateTI->GetData(3) & 0xffff0000) << 16);
      else return 0;
   }
   unsigned int GetHelicitySize(){ return fHelicityChannel_data.size(); }
   double GetHelicity(int i){ if(i >=0 && i<fHelicityChannel_data.size()){ return fHelicityChannel_data[i];} else return 0;}
   unsigned int GetRasterSize(){ return fRasterChannel_data.size(); }
   double GetRaster(int i){ if(i >=0 && i<fRasterChannel_data.size()){ return fRasterChannel_data[i];} else return 0;}

   void UpdateRasterBufferSize(unsigned long bufsize){
      std::lock_guard<std::mutex> _lck(fFileLock);
      std::cout << "Resizing buffers.\n";
      for(int i=0; i< fRasterTimeBuf.size(); ++i) fRasterTimeBuf[i] = CircularBuffer<double>(bufsize);
      for(int i=0; i< fRasterAdcBuf.size(); ++i) fRasterAdcBuf[i] = CircularBuffer<double>(bufsize);
      fN_buf = bufsize;
      std::cout << "Done Resizing buffers.\n";
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
