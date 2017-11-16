#include "AHCPmtEntry.hh"
#include <sstream>

AHCPmtEntry::AHCPmtEntry( const std::string& line ){
  init( line );
}


AHCPmtEntry::~AHCPmtEntry(){
}



void AHCPmtEntry::init( const std::string& line ){
  std::stringstream ln( line );
  ln >> _slot >> _fe >> _type
     >> _hold;
}
