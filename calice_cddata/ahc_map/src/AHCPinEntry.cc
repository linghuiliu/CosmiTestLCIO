#include "AHCPinEntry.hh"
#include <sstream>

AHCPinEntry::AHCPinEntry( const std::string& line ){
  init( line );
}


AHCPinEntry::~AHCPinEntry(){
}



void AHCPinEntry::init( const std::string& line ){
  std::stringstream ln( line );
  ln >> _slot >> _fe >> _type
     >> _hold >> _hbab;
}
