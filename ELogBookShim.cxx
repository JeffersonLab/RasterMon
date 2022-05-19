//
// Created by Maurik Holtrop on 5/19/22.
//

#include "ELogBookShim.h"

#include "logentry.h"
#include "logObj.h"
#include "comment.h"
#include "logException.h"
#include <iostream>
#include <exception>

void ELogBookShim::MakeEntry(){
   try {
      std::cout << "Making logbook entry. \n";
      ELOG::Logentry logentry(fTitle, fLogBooks);
      logentry.setBody(fBody);
      logentry.setTags(fTags);
      logentry.setEntryMakers(fEntryMakers);
      std::cout << "Add the notify\n";
      logentry.setEmailNotify(fEmailNotify);
      for(int i=0; i< fAttachments.size(); ++i) {
         logentry.addAttachment(fAttachments[i], fAttachmentCaptions[i]);
      }
      auto stat = logentry.submit();
      if( stat == 0){
         std::cout << "Log entry was queued because: \n" << logentry.whyQueued() << std::endl;
      }

   } catch(std::exception &exc) {
      std::cout << exc.what() << std::endl;
   }
}