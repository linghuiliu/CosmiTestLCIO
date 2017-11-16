#include "DifTrigger.hh"


std::ostream& DifTrigger::print(std::ostream &ostrm){
    ostrm << " DIF Trigger Counter: " << getTriggerCounter() << std::endl; 
    ostrm << std::endl;
    return ostrm;
}
