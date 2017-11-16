#include "FeEvent.hh"
#include <iostream>
#include <iomanip>

namespace CALICE {
  void FeEvent::print(  std::ostream& os) const 
  {
    os << " board: " << BoardID(getBoardID()) << std::endl
       << " record label:   " << std::setw(3) << getRecordLabel() << std::endl 
       << " Trigger Counter:" << std::setw(9) << getTriggerCounter() << std::endl
       << " Spy Register:   " << std::setw(9) << getSpyRegister() << std::endl;
    os << std::endl;
  }
  
}
