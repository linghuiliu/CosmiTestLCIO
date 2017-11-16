#include "AHCReader.hh"
#include "AHCHcalEntry.hh"
#include "AHCTcmtEntry.hh"
#include "AHCPinEntry.hh"
#include "AHCPmtEntry.hh"

#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>

AHCReader::AHCReader(){
  reset();
}



AHCReader::AHCReader( const char* fname ){
  reset();
  read( fname );
}



AHCReader::~AHCReader(){
}



AHCReader& AHCReader::read( const char* fname ){
  reset();
  std::ifstream fin( fname );
  std::string line;
  while ( fin.good() ){
    getline( fin, line );
    if ( line == "" || line.at( 0 ) == '#' ) continue;
    if ( line.find( " AHCAL ", 0 ) != std::string::npos )
      insert( new AHCHcalEntry( line ) );
    else if ( line.find( " AHCAL8 ", 0 ) != std::string::npos )
      insert( new AHCHcalEntry( line ) );
    else if ( line.find( " TCMT ", 0 ) != std::string::npos )
      insert( new AHCTcmtEntry( line ) );
    else if ( line.find( " PIN ", 0 ) != std::string::npos )
      insert( new AHCPinEntry( line ) );
    else if ( line.find( " PMT ", 0 ) != std::string::npos )
      insert( new AHCPmtEntry( line ) );
    else
      throw std::runtime_error( "Unknown line tag in AHC.cfg - update code!" );
  }
#ifdef DEBUG
  std::cout << "File " << fname << " contains " << size()
	    << " meaningful lines" << std::endl;
#endif
  return (*this);
}



AHCReader& AHCReader::reset(){
  for ( iterator iter = begin(); iter != end(); ++iter )
    delete (*iter);
  clear();
  return (*this);
}


