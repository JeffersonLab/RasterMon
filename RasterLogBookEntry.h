//
// Created by Maurik Holtrop on 5/12/22.
//
//
// Documentation for the logbook API:  https://logbooks.jlab.org/content/logbook-api.  I
// "logentry" commandline code: (e.g. https://github.com/JeffersonLab/clas12-epics/blob/9f362753cb429c5cd5476663ffafa8e735501a29/apps/mollerSetupApp/src/makeLogEntry.c#L62-L84).
//
// Library location at JLab:
// /cs/certified/apps/...
// /apps/acctools/cs/certified/apps/eloglib/2.5/inc  - contains: adminExtension.h  attachment.h  base64.h  body.h
//      comment.h  elog.h  logentry.h  logException.h  logItem.h  logObj.h  mimeReader.h  pathSet.h  problemReport.h
//      timeHandler.h  xmlutl.h
// /apps/acctools/cs/certified/apps/eloglib/2.5/lib/rhel-7-x86_64  - contains: libelog.a libelog.so
//
// The cli program is at:
//  /cs/certified/apps/logentrycli/PRO/bin/logentry
// ONLY that location works.


#ifndef RASTERMON_RASTERLOGBOOKENTRY_H
#define RASTERMON_RASTERLOGBOOKENTRY_H

#include <filesystem>
#include "RasterHists.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGTextEdit.h"
#include "TGLabel.h"
#include "TGComboBox.h"
#include "RQ_OBJECT.h"

#ifndef DEFAULT_HISTOGRAM_PATH
#define DEFAULT_HISTOGRAM_PATH  "/home/clasrun/raster"     // This *should* have been overwritten in CMakeLists.txt
#endif

#ifndef CLI_LOGENTRY_PROGRAM
#define CLI_LOGENTRY_PROGRAM "/home/clasrun/RasterMon/logentry"
#endif

#define RED "\033[91m"
#define BLUE "\033[94m"
#define CYAN  "\033[96m"
#define GREEN  "\033[92m"
#define YELLOW "\033[93m"
#define RED "\033[91m"
#define ENDC "\033[0m"

class RasterLogBookEntry {

   RQ_OBJECT("RasterLogBookEntry")

public:
   RasterLogBookEntry(const TGWindow *parent_window, RasterHists *rhists);

   TGTextEntry* AddTextLine(string label_text, string init_text, string tooltip);
   void MakeEntry();
   void SubmitToLogBook();
   void SaveCanvassesToFile();
   void ScrubString(string &instr);
   void CloseWindow(){
      if(fEntryThread.joinable()) fEntryThread.join();
      fMain->CloseWindow();
      Emit("CloseWindow()");
   };
   void Cancel(){
      CloseWindow();
   }
   void OK(){
      SubmitToLogBook();
      CloseWindow();
   }

public:

   bool fLogEntryOK = false;
   // std::filesystem::path fHistogramPath{DEFAULT_HISTOGRAM_PATH}; //! CLING doesn't know what to do with this.
   std::string fHistogramPath{DEFAULT_HISTOGRAM_PATH};
   const TGWindow *fParentWindow;
   TGTransientFrame *fMain;
   RasterHists *fRHists;  // Pointer to the histograms and graphs.
   bool fAlreadyWritingImages=false;  // Right now, only one thread to write images.
   std::thread fEntryThread;

   std::string fTitle;
   TGTextEntry* fTitleEntry;
   std::string fLogBooks{"HBRUN"};
   std::vector<std::string> fLogBookChoices = {"HBRUN", "HBLOG", "HBRUN,HBLOG", "ELOG", "HBLOG,ELOG", "TLOG"};
   TGComboBox* fLogBookCBox;
   std::string fEntryMakers{""};
   TGTextEntry* fEntryMakersEntry;
   std::string fTags{"Beamline"};
   std::vector<std::string> fTagChoices = {"","Beamline"};
   TGComboBox* fTagsCBox;
   std::string fReference{""};
   TGTextEntry* fReferenceEntry;
   std::string fEmailNotify{""};
   TGTextEntry* fEmailNotifyEntry;
   std::string fBody;
   TGTextEdit *fBodyEdit;
   std::vector<std::string> fAttachments;
   std::vector<std::string> fAttachmentCaptions;
};


#endif //RASTERMON_RASTERLOGBOOKENTRY_H
