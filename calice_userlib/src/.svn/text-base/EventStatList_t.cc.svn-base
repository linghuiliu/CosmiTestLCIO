#include "EventStatList_t.hh"
#include <iostream>
#include <iomanip>

void EventStat_t::printTableHeader(std::ostream &out) const 
{
  out << "Errors: " << std::setw(10) << "Same Ev." << std::setw(13) << "Trg.Mismatch" 
      << std::setw(14) << "between slots"
      << std::setw(17) << "Corrupt Record" 
      << std::setw(12) << "No Fe Data" 
      << std::setw(17) << "  Bad VLink Header  "
      << std::setw(17) << " Destr. CrcVlink Records  " 
      << std::endl;
}

void EventStat_t::print(std::ostream &out) 
{
  out << std::setw(10+8) << refreshErrors() << std::setw(13) << triggerCounterErrors() 
      << std::setw(14) << slotTriggerCounterErrors()  
      << std::setw(17) << recordErrors()  
      << std::setw(12) << eventsWithoutFeData()
      << std::setw(12) << badVLinkHeaders()
      << std::setw(17) << destroyedCrcVLinkRecords()
      << std::endl;
}
