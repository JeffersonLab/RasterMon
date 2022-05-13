//
// Created by Maurik Holtrop on 5/12/22.
//
#include <iostream>
#include <thread>
#include <chrono>
#include "RasterLogBookEntry.h"

RasterLogBookEntry::RasterLogBookEntry(const TGWindow *parent_window):
   TGTransientFrame(gClient->GetRoot(), parent_window, 400, 200){

};

void RasterLogBookEntry::MakeEntry(int i) {
   fThreadPool.emplace_back(&RasterLogBookEntry::EntryDialog, this, i);
}

void RasterLogBookEntry::EntryDialog(int i){
   std::cout << "Should be making an entry. \n";
   std::this_thread::sleep_for(std::chrono::seconds(30));
   std::cout << "Done typing and submitted... ? \n";
}