//
// Created by Maurik Holtrop on 5/19/22.
//

#ifndef RASTERMON_ELOGBOOKSHIM_H
#define RASTERMON_ELOGBOOKSHIM_H

#include <iostream>
#include <string>
#include <vector>

class ELogBookShim {

public:
   std::string fTitle;
   std::string fLogBooks;
   std::string fEntryMakers;
   std::string fTags;
   std::string fReference;
   std::string fEmailNotify;
   std::string fBody;
   std::vector<std::string> fAttachments;
   std::vector<std::string> fAttachmentCaptions;

   void MakeEntry();
};


#endif //RASTERMON_ELOGBOOKSHIM_H
