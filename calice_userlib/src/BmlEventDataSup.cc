#include "BmlEventDataSup.hh"


namespace CALICE{

   std::ostream& BmlEventDataSup::print(std::ostream &ostrm)  
   {
     ostrm << "In Sup: " << std::endl;
     ostrm <<   getTypeName() << std::endl;
     ostrm << " TDC Type: " << getTDCType() << std::endl;
     ostrm << " Word Count: " << getWordCount() << std::endl;
     ostrm << " Event Count: " << getEventCount() << std::endl;
     ostrm << " BunchID: " << getBunchID() << std::endl;
     ostrm << " EventID TDC trailer: " << getEventIDTrailer() << std::dec << std::endl;
     ostrm << " TDC errors: " << getTDCErrors() << std::dec << std::endl;
     ostrm << " Buffer overflow: " << getBufferOverflow() << std::endl;
     ostrm << " Trigger lost: " << getTriggerLost() << std::endl;
     ostrm << " Number of trailer words: " << getTDCWordCountTrailer() << std::endl; 
     return ostrm;
   }
}



 

