//
// Created by Maurik Holtrop on 3/28/22.
//

#include "RasterEvioTool.h"
#include <thread>
#include <chrono>

RasterEvioTool::RasterEvioTool(string infile) : EvioTool(infile){
   // Basic setup of the EvioTool Class

   fAutoAdd = false;  // Do not add every bank in the file automatically.
   fChop_level = 1;   // Top level bank is chopped.
   tags = {0x80};      // Only get the physics event banks.  The 31 tag is EPICS.
   tag_masks = {0x80}; // Any event with bit 6 set  2<<6
   fEvioHead = AddLeaf<unsigned int>("fEvioHead", 49152, 0, "Evio Event Header Info");
   fRasterHead =  new RasterMonEventInfo(this);  // Calls AddBank internally.
   fRasterCrate = AddBank("Raster", fRasterBankTag, 0, "Raster fRasterFADC banks");
   fRasterCrateTI = fRasterCrate->AddLeaf<unsigned int>("RasterCrateTI", 57610, 0, "Raster Crate TI info");
   fRasterFADC = fRasterCrate->AddLeaf<FADCdata>("fRasterFADC", 57601, 0, "Raster fRasterFADC");
   fHelicityCrate = AddBank("Helicity", fHelicityBankTag, 0, "Raster fRasterFADC banks");
   fHelicityFADC = fHelicityCrate->AddLeaf<FADCdata>("fHelicityFADC", 57601, 0, "Raster fRasterFADC");

   for(int i=0; i<fRasterChannels.size(); ++i){
      fRasterTimeBuf.emplace_back(fN_buf);
      fRasterAdcBuf.emplace_back(fN_buf);
   }

}

void RasterEvioTool::Clear(Option_t *opt) {
   // Clears the entire data structures.
   // Here we clear the *local* data, then call the parent Clear to clear the rest.
   for(int i=0; i < fHelicityChannel_data.size(); ++i) fHelicityChannel_data[i] = 0;
   EvioTool::Clear(opt);
}

int RasterEvioTool::Next() {
   // Get the next event from EVIO and process it.
   // If the file is not opened, or the read causes an EOF, open the next file.
   Clear("");

   fFileLock.lock();
   if( !IsReadingFromEt() && !IsOpen()){
      // Open the next file from this file list.
      fiInputFile++;
      if(fiInputFile >= fInputFiles.size()){
         if( fDebug & EvioTool_Debug_Info ) std::cout << "End of EVIO file data and ET not connected. \n";
         fFileLock.unlock();
         return(EVIO_END_OF_DATA);
      }
      if( fDebug & EvioTool_Debug_Info ) std::cout << "Open EVIO file " << fInputFiles[fiInputFile] << "\n";
      Open(fInputFiles[fiInputFile].c_str());
   }
   fFileLock.unlock();

   int stat = EvioTool::Next();
   if( stat == EvioTool_Status_EOF){
      if( IsOpen() ) Close();
      return(Next());  // Recursively call next, which wil open the next file and read the next event.
   }
   fNEventsProcessed++;
   // Here we parse the expected fRasterFADC data to make for easy access.
   for(int i=0; i < fHelicityFADC->size(); ++i){
      if(fHelicityFADC->GetData(i).GetCrate() == fHelicityBankTag && fHelicityFADC->GetData(i).GetSlot() == fHelicitySlot){
         for(int j=0; j < fHelicityChannels.size(); ++j){
            if(fHelicityFADC->GetData(i).GetChan() == fHelicityChannels[j]){
               double sum = 0.;
               for(int k=0; k < fHelicityFADC->GetData(i).GetSampleSize(); ++k){
                  sum += (double) fHelicityFADC->GetData(i).GetSample(k);
               }
               fHelicityChannel_data[j] = sum / (double)fHelicityFADC->GetData(i).GetSampleSize();
            }
         }
      }
   }

   for(int i=0; i < fRasterFADC->size(); ++i){
      if(fRasterFADC->GetData(i).GetCrate() == fRasterBankTag && fRasterFADC->GetData(i).GetSlot() == fRasterSlot){
         for(int j=0; j < fRasterChannels.size(); ++j){
            if(fRasterFADC->GetData(i).GetChan() == fRasterChannels[j]){
               double sum = 0.;
               size_t N = fRasterFADC->GetData(i).GetSampleSize();
               for(size_t k=0; k < N; ++k){
                  sum += (double) fRasterFADC->GetData(i).GetSample(k);
               }
               sum = sum / (double) N;
               fRasterChannel_data[j] = sum;
               double time = 5.e-9*(double)fRasterFADC->GetData(i).GetRefTime();
               if(time>0) { // If time == 0, then this is not data so skip it.
                  fRasterTimeBuf[j].push_back(time);   // Convert to second
                  fRasterAdcBuf[j].push_back(sum);
               }
            }
         }
      }
   }
   // std::this_thread::sleep_for(std::chrono::microseconds(500));
   return(EvioTool_Status_OK);
}

ClassImp(RasterEvioTool);