//
// Created by Maurik Holtrop on 5/12/22.
//
//
// Documentation for the logbook API:  https://logbooks.jlab.org/content/logbook-api.  I
// "logentry" commandline code: (e.g. https://github.com/JeffersonLab/clas12-epics/blob/9f362753cb429c5cd5476663ffafa8e735501a29/apps/mollerSetupApp/src/makeLogEntry.c#L62-L84).
//
// Library location at JLab:
// /apps/acctools/cs/certified/apps/eloglib/2.5/inc  - contains: adminExtension.h  attachment.h  base64.h  body.h
//      comment.h  elog.h  logentry.h  logException.h  logItem.h  logObj.h  mimeReader.h  pathSet.h  problemReport.h
//      timeHandler.h  xmlutl.h
// /apps/acctools/cs/certified/apps/eloglib/2.5/lib/rhel-7-x86_64  - contains: libelog.a libelog.so


#ifndef RASTERMON_RASTERLOGBOOKENTRY_H
#define RASTERMON_RASTERLOGBOOKENTRY_H

#include "RasterHists.h"
#include "TGFrame.h"

#ifndef DEFAULT_HISTOGRAM_PATH
#define DEFAULT_HISTOGRAM_PATH  "/home/clasrun/raster"     // This *should* have been overwritten in CMakeLists.txt
#endif

class RasterLogBookEntry: public TGTransientFrame{
public:
   RasterLogBookEntry(const TGWindow *parent_window, RasterHists *rhists);

   void MakeEntry();
   void SubmitToLogBook();
   void SaveCanvassesToFile();
   void CloseWindow() override{
      TGTransientFrame::CloseWindow();
   };
   void Cancel(){
      CloseWindow();
   }
   void OK(){
      TGTransientFrame::CloseWindow();
      SubmitToLogBook();
   }

private:
   RasterHists *fRHists;  // Pointer to the histograms and graphs.
   bool fAlreadyMakingEntry=false;
   std::thread fEntryThread;

   std::string fTitle;
   std::string fBody;
   std::vector<std::string> fLogBooks = {"HBLOG"};
   std::string fEntryMakers;
   std::string fTags{"BeamLine"};
   std::string fReference;
   std::string fEmailNotify;

};


#endif //RASTERMON_RASTERLOGBOOKENTRY_H
