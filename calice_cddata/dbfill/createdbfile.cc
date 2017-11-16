#ifdef USE_CONDDB

// -- LCCD headers
#include "lccd.h"
#include "lccd/DBInterface.hh"

// -- C++ headers 
#include <iostream>
#include <cstdlib>

using namespace std ;
using namespace lcio;
//using namespace lccd ;


/** Test program that creates an LCIO fole with all conditions data in a database folder 
 *  for a given tag.
 * 
 * @author F.Gaede, DESY
 * @version $Id: createdbfile.cc,v 1.2 2008-06-13 15:07:35 poeschl Exp $
 */

int main(int argc, char** argv ) {
  
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;
  
  // read file name and collection name from command line 
  if( argc < 2) {
    cout << " usage: createdbfile <db collection name> [<tag>]" << endl ;
    exit(1) ;
  }
  
  string folder( argv[1] ) ;
  //string folder( "/lccd_calice/" + colName ) ;
  string tag("") ;
  
  if( argc > 2 )
    tag = argv[2] ;

  string dbinit("flccaldb02.desy.de:calice:caliceon:Delice.1:3306") ;
  if( argc > 3 ) dbinit = argv[3]; 

  lccd::DBInterface db( dbinit, folder , false ) ;

  db.createDBFile( tag ) ; 


}
#endif
