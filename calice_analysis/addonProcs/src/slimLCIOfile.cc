#include <iostream>
#include <sstream>/*stringstream*/
#include <string>
#include <fstream>/*file I/O*/
#include <math.h>

using namespace std;


/*LCIO headers*/
#include "lcio.h"
#include "UTIL/LCSplitWriter.h"
#include "IOIMPL/LCFactory.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCRunHeader.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/LCFlagImpl.h"

using namespace lcio;



int main(int argc, char** argv )
{
  /*read file names from the command line (only arguments)*/
  if( argc < 3) 
    {
      cout << " usage: "<<argv[0]<<" <input-file> <output-file>" << endl ;
      return 1;
    }

  const char *inputFileName = argv[1];
  const char *outputFileName = argv[2];
  const char *noiseColName = "AhcNoise";
  
  /*----------------------------------------------------------------*/
  /*                                                                */
  /* prepare to write the LCIO file                                 */
  /*                                                                */
  /*----------------------------------------------------------------*/
  /*Use the LCSplitWriter in order to automatically split files after 
    the 1.9 GByte file size has been exceeded*/
  LCSplitWriter* lcWrt = new LCSplitWriter( LCFactory::getInstance()->createLCWriter(), 2040109465 ) ;
  lcWrt->open(outputFileName);  

  LCRunHeaderImpl* runHdr = new LCRunHeaderImpl;
  stringstream description;
  description << "AHCAL noise hits";
  runHdr->setDescription( description.str());
  lcWrt->writeRunHeader( runHdr ) ;

  /*----------------------------------------------------------------*/
  /*                                                                */
  /* open the LCIO input file                                       */
  /*                                                                */
  /*----------------------------------------------------------------*/
  LCReader *lcReader = lcio::LCFactory::getInstance()->createLCReader();
  lcReader->open(inputFileName);
 
  int events = 0;
  LCEvent *inputEvent = NULL;
  LCEventImpl* outputEvent = NULL;



  //----------- the event loop -----------
  while( (inputEvent = lcReader->readNextEvent()) != 0 ) 
    {
      std::cout<<" Input event: "<<inputEvent->getEventNumber()<<std::endl;
      try{
	LCCollection *col = inputEvent->getCollection(noiseColName);
	
	outputEvent = new LCEventImpl();
	outputEvent->setEventNumber( events );	
	outputEvent->addCollection(col, noiseColName);
	lcWrt->writeEvent( outputEvent );
	events++;

	std::cout<<"  output event: "<<events<<std::endl;
      }

      catch (EVENT::DataNotAvailableException &e) 
	{
	  std::cout<<" no collection" << std::endl;
	}	
      
      
 
 
    }


//   try{
//     inputEvent = lcReader->readNextEvent();

//     while (inputEvent != NULL)
//       {
// 	LCCollection *col = inputEvent->getCollection(noiseColName);
	
// 	//outputEvent->setRunNumber( runNumber );
// 	outputEvent->setEventNumber( events );

// 	outputEvent->addCollection(col, noiseColName);
// 	lcWrt->writeEvent( inputEvent );
// 	events++;
	
//       }//end while
//   }//end try
//   catch (EVENT::DataNotAvailableException &e) 
//     {
//       std::cout<<"Event  not found" << std::endl;
//     }	
  
  lcReader->close();
  lcWrt->close();
  
  cout<<"\nFinished writing "<<events<<" events to "<<outputFileName<<endl;

  return 0;
}







