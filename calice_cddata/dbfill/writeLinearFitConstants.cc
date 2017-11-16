#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

#include "LinearFitConstant.hh"
#include "HcalTileIndex.hh"

#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "lccd.h"
#include "lccd/DBInterface.hh"

#include "collectionParameterHelper.hh"

#include "writeLCGenericObject.hh"

class writeLinearFitConstants : public writeLCGenericObject 
{
  
public:
  
  writeLinearFitConstants(std::string description) : writeLCGenericObject(description) {}

  /****************************************************************************************/
  /*                                                                                      */
  /*                                                                                      */
  /*                                                                                      */
  /****************************************************************************************/
  lcio::LCCollectionVec* readFlatFile( const char* fname )
  {
    std::ifstream fin( fname );

    if (! fin.is_open() )
      {
	std::stringstream msg;
	msg << "Cannot open file " << fname;
	throw std::runtime_error( msg.str() );
      }
    
    IMPL::LCCollectionVec* col = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );

    std::string line;

    int module, chip, channel;

    double constant, constant_error;
    double constant_reference_point, constant_reference_point_error;

    while ( fin.good() )
      {
	getline( fin, line );
	if ( line == "" || line.at(0) == '#' ) continue;
	std::stringstream ln( line );
	
	ln >> module >> chip >> channel 
	   >> constant >> constant_error 
	   >> constant_reference_point >> constant_reference_point_error;
	
	CALICE::HcalTileIndex hti( module, chip, channel);
	CALICE::LinearFitConstant* sv = 
	  new CALICE::LinearFitConstant( hti.getIndex(), 
					 constant,
					 constant_error,
					 constant_reference_point,
					 constant_reference_point_error);
	
	col->addElement( sv );
      }
    
    return col;
  }



  /****************************************************************************************/
  /*                                                                                      */
  /*                                                                                      */
  /*                                                                                      */
  /****************************************************************************************/
  int printHelp( int ret )
  {
    std::cout << "Program writeLinearFitConstants" << std::endl
              << "--inFile <file>        input flat file" << std::endl
              << "--outFile <file>       output LCIO file" << std::endl
              << "--colName <file>       name of LCCollection in file" << std::endl
              << "--dbinit <string>      database init string" << std::endl
              << "--folder <string>      database folder" << std::endl
              << "--description <string> description stored in DB" << std::endl
              << "--from <string>        start of validity time" << std::endl
              << "--until <string>       end of validity time" << std::endl
              << "--timeIsTimeStamp <string> true or false   " << std::endl
              << "--write                dry test run otherwise" << std::endl
              << "--parameterFile <file> file with collection parameters" << std::endl
              << "--help                 print this text" << std::endl;
    return ret;
  }


  /****************************************************************************************/
  /*                                                                                      */
  /*                                                                                      */
  /*                                                                                      */
  /****************************************************************************************/
  void screenDump( lcio::LCCollection* col )
  {
    for ( int i = 0; i != col->getNumberOfElements(); ++i )
      {
	CALICE::LinearFitConstant* val = static_cast<CALICE::LinearFitConstant*>( col->getElementAt( i ) );
	CALICE::HcalTileIndex hti( val->getID() );
	std::cout << hti << '\n'
		  << *val << std::endl;
      }
  }
  
  
};


/****************************************************************************************/
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
int main(int argc, char** argv) {
  
  writeLinearFitConstants wlfc("LinearFitConstants");
  
  wlfc.mymain(argc, argv);
  
}
