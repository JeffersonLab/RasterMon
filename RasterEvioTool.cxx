//
// Created by Maurik Holtrop on 3/28/22.
//

#include "RasterEvioTool.h"

RasterEvioTool::RasterEvioTool(string infile) : EvioTool(infile) {
   // Basic setup of the EvioTool Class

   fAutoAdd = false;  // Do not add every bank in the file automatically.
   fChop_level = 1;   // Top level bank is chopped.
   tags = {0x80};      // Only get the physics event banks.  The 31 tag is EPICS.
   tag_masks = {0x80}; // Any event with bit 6 set  2<<6
   fEvioHead = AddLeaf<unsigned int>("fEvioHead", 49152, 0, "Evio Event Header Info");
   fRasterHead =  new RasterMonEventInfo(this);  // Calls AddBank internally.
   fRasterCrate = AddBank("Raster", fRasterCrateNum, 0, "Raster fRasterFADC banks");
   fRasterFADC = fRasterCrate->AddLeaf<FADCdata>("fRasterFADC", 57601, 0, "Raster fRasterFADC");
   fHelicityCrate = AddBank("Helicity", fHelicityCrateNum, 0, "Raster fRasterFADC banks");
   fHelicityFADC = fHelicityCrate->AddLeaf<FADCdata>("fHelicityFADC", 57601, 0, "Raster fRasterFADC");
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
      if(fHelicityFADC->GetData(i).GetCrate() == fHelicityCrateNum && fHelicityFADC->GetData(i).GetSlot() == fHelicitySlot){
         for(int j=0; j < fHelicityChannels.size(); ++j){
            if(fHelicityFADC->GetData(i).GetChan() == fHelicityChannels[j]){
               double sum = 0.;
               for(int k=0; k < fHelicityFADC->GetData(i).GetSampleSize(); ++k){
                  sum += (double) fHelicityFADC->GetData(i).GetSample(k);
               }
               fHelicityChannel_data[j] = sum / fHelicityFADC->GetData(i).GetSampleSize();
            }
         }
      }
   }

   for(int i=0; i < fRasterFADC->size(); ++i){
      if(fRasterFADC->GetData(i).GetCrate() == fRasterCrateNum && fRasterFADC->GetData(i).GetSlot() == fRasterSlot){
         for(int j=0; j < fRasterChannels.size(); ++j){
            if(fRasterFADC->GetData(i).GetChan() == fRasterChannels[j]){
               double sum = 0.;
               for(int k=0; k < fRasterFADC->GetData(i).GetSampleSize(); ++k){
                  sum += (double) fRasterFADC->GetData(i).GetSample(k);
               }
               fRasterChannel_data[j] = sum / fRasterFADC->GetData(i).GetSampleSize();
            }
         }
      }
   }
   return(EvioTool_Status_OK);
}

ClassImp(RasterEvioTool);