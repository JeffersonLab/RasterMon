//
// Created by Maurik Holtrop on 3/28/22.
//

#ifndef RASTERMON_RASTERBANK_H
#define RASTERMON_RASTERBANK_H

#include <TObject.h>
#include <EvioTool.h>
#include <FADCdata.h>
#include <Leaf.h>
#include <RasterMonEventInfo.h>
#include <mutex>

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
   // Sub banks.
   // Note on mem: The memory will be managed by TObjArray. So no delete to be called.
   Leaf<unsigned int>  *fEvioHead = nullptr;
   unsigned long fMostRecentEventNumber;  // Number of last actual event that was read.
   RasterMonEventInfo *fRasterHead = nullptr;
   Bank   *fRasterCrate = nullptr;
   Leaf<FADCdata> *fRasterFADC = nullptr;
   Bank   *fHelicityCrate = nullptr;
   Leaf<FADCdata> *fHelicityFADC = nullptr;

   int fHelicityCrateNum = 19;
   int fHelicitySlot = 19;
   std::vector<int> fHelicityChannels = {0, 2, 4};
   std::vector<double> fHelicityChannel_data = {-1., -1., -1.}; // reserve 3 slots for the data.

   int fRasterCrateNum = 59;
   int fRasterSlot = 19;
   std::vector<int> fRasterChannels = {1, 3};
   std::vector<double> fRasterChannel_data = {-1., -1.}; // reserve 2 slots for the data.



   std::mutex fFileLock;   // Because the Next() has a next file build in.

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
   double GetHelicity(int i){ if(i >=0 && i<3){ return fHelicityChannel_data[i];} else return 0;}
   double GetRaster(int i){ if(i >=0 && i<2){ return fRasterChannel_data[i];} else return 0;}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
ClassDef(RasterEvioTool, 0);
#pragma clang diagnostic pop

};

#endif //RASTERMON_RASTERBANK_H
