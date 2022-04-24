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
    fRasterHead =  new RasterMonEventInfo(this);  // Calls AddBank internally.
    fRasterCrate = AddBank("Raster", 19, 0, "Raster fFADC banks");
    fFADC = fRasterCrate->AddLeaf<FADCdata>("fFADC", 57601, 0, "Raster fFADC");
}

void RasterEvioTool::Clear(Option_t *opt) {
   // Clears the entire data structures.
   // Here we clear the *local* data, then call the parent Clear to clear the rest.
   for(int i=0; i< fChannel_data.size(); ++i) fChannel_data[i] = 0;
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

    int stat = EvioTool::Next();  // Get the next fEvio file and parse the content.
    if( stat == EvioTool_Status_EOF){
       if( IsOpen() ) Close();
       return(Next());  // Recursively call next, which wil open the next file and read the next event.
    }
   fNEventsProcessed++;
    // Here we parse the expected fFADC data to make for easy access.
   for(int i=0; i < fFADC->size(); ++i){
      if(fFADC->GetData(i).GetCrate() == fCrate && fFADC->GetData(i).GetSlot() == fSlot){
         for(int j=0; j< fChannels.size(); ++j){
            if(fFADC->GetData(i).GetChan() == fChannels[j]){
               double sum = 0.;
               for(int k=0; k < fFADC->GetData(i).GetSampleSize(); ++k){
                  sum += (double) fFADC->GetData(i).GetSample(k);
               }
               fChannel_data[j] = sum / fFADC->GetData(i).GetSampleSize();
            }
         }
      }
   }
    return(EvioTool_Status_OK);
}

ClassImp(RasterEvioTool);