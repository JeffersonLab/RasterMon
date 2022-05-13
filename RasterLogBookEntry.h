//
// Created by Maurik Holtrop on 5/12/22.
//
//
// Documentation for the logbook API:  https://logbooks.jlab.org/content/logbook-api.  I
// "logentry" commandline code: (e.g. https://github.com/JeffersonLab/clas12-epics/blob/9f362753cb429c5cd5476663ffafa8e735501a29/apps/mollerSetupApp/src/makeLogEntry.c#L62-L84).


#ifndef RASTERMON_RASTERLOGBOOKENTRY_H
#define RASTERMON_RASTERLOGBOOKENTRY_H

#include "TGFrame.h"

class RasterLogBookEntry: public TGTransientFrame{
public:
   RasterLogBookEntry(const TGWindow *parent_window);

   void MakeEntry(int i);
   void EntryDialog(int i);

private:
   std::vector<std::thread> fThreadPool;

};


#endif //RASTERMON_RASTERLOGBOOKENTRY_H
