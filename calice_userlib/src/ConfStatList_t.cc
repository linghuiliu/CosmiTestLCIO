#include "ConfStatList_t.hh"
#include <iostream>
#include <iomanip>

void ConfStat_t::printTableHeader(std::ostream &out) const 
{
  out << std::setw(9) << "Conf.Ch." << std::setw(9) << "bad CRC" << std::setw(9) << "no read"
      << std::endl;
}

void ConfStat_t::print(std::ostream &out) 
{
  out << std::setw(9) << nTotal() << std::setw(9) << badTotal() << std::setw(9) << consecutiveWritesTotal()
      << std::endl;
}
