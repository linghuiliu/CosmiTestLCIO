#include "AHCTcmtEntry.hh"
#include <sstream>

AHCTcmtEntry::AHCTcmtEntry( const std::string& line ){
  init( line );
}


AHCTcmtEntry::~AHCTcmtEntry(){
}


void AHCTcmtEntry::init( const std::string& line ){
  std::stringstream ln( line );
  ln >> _slot >> _fe >> _type 
     >> _dacFileName
     >> _holdExt >> _holdLedCm >> _holdLedPm
     >> _vcalibCm >> _vcalibPm;
}
