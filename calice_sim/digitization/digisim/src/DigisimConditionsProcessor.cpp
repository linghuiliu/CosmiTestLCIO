#ifdef USE_LCCD
#include "DigisimConditionsProcessor.hpp"

//--- LCIO headers 
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"
#include "EVENT/LCParameters.h"
#include <UTIL/LCTime.h>
//#include <IMPL/LCEventImpl.h> // debug

//--- LCCD headers
#include "lccd.h"
#include "lccd/SimpleFileHandler.hh"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBFileHandler.hh"
#include "lccd/DataFileHandler.hh"

#include "lccd/LCConditionsMgr.hh"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace lcio ;


DigisimConditionsProcessor aDigisimConditionsProcessor ;

bool DigisimConditionsProcessor::registerChangeListener( lccd::IConditionsChangeListener* cl, const std::string&  name) {

  try{ 
    lccd::LCConditionsMgr::instance()->registerChangeListener( cl , name ) ;
    return true ;
  }
  catch(Exception){}
  
  return false ;
}
  
  
DigisimConditionsProcessor::DigisimConditionsProcessor() : Processor("DigisimConditionsProcessor") {
    
  // modify processor description
  _description = "ConditionsProcessor provides access to conditions data " 
    " transparently from LCIO files or a databases, using LCCD" ;

  // register steering parameters: name, description, class-variable, default value
    
#ifdef USE_CONDDB
  registerProcessorParameter( "DBInit" , 
			      "Initialization string for conditions database"  ,
			      _dbInit ,
			      std::string("localhost:lccd_test:calvin:hobbes")) ;
    
    
  StringVec dbcondExample ;
  dbcondExample.push_back("conditionsName");
  dbcondExample.push_back("/lccd/myfolder");
  dbcondExample.push_back("HEAD");
    
    
  registerOptionalParameter( "DBCondHandler" , 
			     "Initialization of a conditions database handler"  ,
			     _dbcondHandlerInit ,
			     dbcondExample ,
			     dbcondExample.size() ) ;
    
#endif
    
  StringVec simpleExample ;
  simpleExample.push_back("conditionsName");
  simpleExample.push_back("conditions.slcio");
  simpleExample.push_back("collectionName");
    
  registerOptionalParameter( "SimpleFileHandler" , 
			     "Initialization of a simple conditions file handler"  ,
			     _simpleHandlerInit ,
			     simpleExample ,
			     simpleExample.size() ) ;
    
  StringVec dbfileExample ;
  dbfileExample.push_back("conditionsName");
  dbfileExample.push_back("conditions.slcio");
  dbfileExample.push_back("collectionName");
    
    
  registerOptionalParameter( "DBFileHandler" , 
			     "Initialization of a conditions db file handler"  ,
			     _dbfileHandlerInit ,
			     dbfileExample ,
			     dbfileExample.size() ) ;
    
    
  StringVec datafileExample ;
  datafileExample.push_back("conditionsName");
    
  registerOptionalParameter( "DataFileHandler" , 
			     "Initialization of a data file handler"  ,
			     _datafileHandlerInit ,
			     datafileExample ,
			     datafileExample.size() ) ;
    
}
  
void DigisimConditionsProcessor::init() { 
    
  // usually a good idea to
  printParameters() ;
    
  _nRun = 0 ;
  _nEvt = 0 ;
    
  // initialize the requested conditions handler 
    
  if( parameterSet( "SimpleFileHandler" ) ) {
      
    unsigned index = 0 ;
    while( index < _simpleHandlerInit.size() ){
	
      std::string condName( _simpleHandlerInit[ index++ ] ) ; 
      std::string fileName( _simpleHandlerInit[ index++ ] ) ; 
      std::string colName ( _simpleHandlerInit[ index++ ] ) ; 
	
      lccd::LCConditionsMgr::instance()->
	registerHandler( condName ,  new lccd::SimpleFileHandler( fileName, condName, colName ) ) ;

      _condHandlerNames.push_back( condName ) ;
    }
  }
    
#ifdef USE_CONDDB
    
  if( parameterSet( "DBCondHandler" ) ) {
      
    unsigned index = 0 ;
    while( index < _dbcondHandlerInit.size() ){
	
      std::string condName( _dbcondHandlerInit[ index++ ] ) ; 
      std::string folder  ( _dbcondHandlerInit[ index++ ] ) ; 
      std::string tag     ( _dbcondHandlerInit[ index++ ] ) ; 
	
      // HEAD corresponds to no tag in database
      if( tag == "HEAD" ) tag = "" ;
	
      lccd::LCConditionsMgr::instance()->
	registerHandler( condName, new lccd::DBCondHandler( _dbInit , folder , condName, tag ) ) ;

      _condHandlerNames.push_back( condName ) ;
    }
  }
    
#endif
    
  if( parameterSet( "DBFileHandler" ) ) {
      
    unsigned index = 0 ;
    while( index < _dbfileHandlerInit.size() ){
	
      std::string condName( _dbfileHandlerInit[ index++ ] ) ; 
      std::string fileName( _dbfileHandlerInit[ index++ ] ) ; 
      std::string colName ( _dbfileHandlerInit[ index++ ] ) ; 
	
      lccd::LCConditionsMgr::instance()->
	registerHandler( condName ,  new lccd::DBFileHandler( fileName, condName, colName ) ) ;

      _condHandlerNames.push_back( condName ) ;
    }
  }
    
    
  if( parameterSet( "DataFileHandler" ) ) {
      
    unsigned index = 0 ;
    while( index < _datafileHandlerInit.size() ){
	
      std::string condName ( _datafileHandlerInit[ index++ ] ) ; 
	
      lccd::LCConditionsMgr::instance()->
	registerHandler( condName ,  new lccd::DataFileHandler( condName ) ) ;

      _condHandlerNames.push_back( condName ) ;
    }
  }
    
}
  
void DigisimConditionsProcessor::processRunHeader( LCRunHeader* run) { 
  static int lastRun   = -1 ;
    
  //std::cout << " Processing run number : " << run->getRunNumber() << std::endl;
  //const_cast<const LCParameters*>(
  _params = &(run->getParameters());

  std::cout << "DigisimConditionsProcessor::processRun()  " << name()
       << " in run " << _params->getIntVal("RunNumber")
	    << std::endl ;

  if(run->getRunNumber() != lastRun ){
    //lccd::LCCDTimeStamp tstart(getRunStartTime(run->getRunNumber()));
    lccd::LCCDTimeStamp tstart(_params->getIntVal("StartTime"));
    tstart *= 1000000000;
    lccd::LCConditionsMgr::instance()->update( tstart );
    std::cout << " Update has been called " << std::endl;

    for (unsigned int i=0; i< _condHandlerNames.size()  ; ++i){
      //lcio::LCTime t(getRunStartTime(run->getRunNumber()));
      lcio::LCTime t(_params->getIntVal("StartTime"));
      lcio::LCTime t0((lccd::LCConditionsMgr::instance()->getHandler(_condHandlerNames[i]))->validSince());
      lcio::LCTime t1((lccd::LCConditionsMgr::instance()->getHandler(_condHandlerNames[i]))->validTill());

      std::cout << " ------> Collection : " << _condHandlerNames[i] << " has been updated for time : " << t.getDateString() << " and is valid between : " << t0.getDateString() << " and " << t1.getDateString() << std::endl;

    }
    lastRun = _params->getIntVal("RunNumber");
  }

  _nRun++ ;

}
  
void DigisimConditionsProcessor::processEvent( LCEvent * evt ) { 
    
  static bool firstEvent = true;

  if (evt->getTimeStamp() != 0) {
    assert (evt->getTimeStamp() >= _params->getIntVal("StartTime") && 
    	    evt->getTimeStamp() <= _params->getIntVal("EndTime"));
  }

  if (firstEvent){
    //lccd::LCCDTimeStamp tstart(_params->getIntVal("StartTime"));
    //tstart *= 1000000000;
    //tstart += _nEvt+1;
    //lccd::LCConditionsMgr::instance()->update( tstart );
    //std::cout << " Update has been called " << std::endl;

    //std::cout << "----------- ConditionsProcessor : evt #" << evt->getEventNumber() << "------------" << std::endl;

    for (unsigned int i=0; i< _condHandlerNames.size()  ; ++i){
      LCCollection *bla;
      bla = (lccd::LCConditionsMgr::instance()->getHandler(_condHandlerNames[i]))->currentCollection();
      //col[i] = (lccd::LCConditionsMgr::instance()->getHandler(_condHandlerNames[i]))->currentCollection();
      //evt->addCollection(bla,_condHandlerNames[i]);
      //evt->addCollection(col[i],_condHandlerNames[i]);
    }

//   typedef const std::vector<std::string> StringVec ;
//   StringVec* strVec = evt->getCollectionNames() ;
//   for( StringVec::const_iterator name = strVec->begin() ; name != strVec->end() ; name++)
//     {//loop on Input Collection Names

//       std::string sss = name->c_str();
//       std::cout << "---CondProc---> Evt " <<evt->getEventNumber()<< ", EXISTING COLLECTION named : " << sss << std::endl;
//       cole = evt->takeCollection(*name);
//       int nHits =  cole->getNumberOfElements() ;

//       std::cout << "---CondProc---> Evt " <<evt->getEventNumber()<< ", EXISTING COLLECTION, of the class : " << cole->getTypeName().c_str() << " and named : " << sss << " with " << nHits << " elements." << std::endl;
//     }

  //for (unsigned int i=0; i< _condHandlerNames.size()  ; ++i){
  //  delete col[i];
  //}
    firstEvent = false;

  //delete col[];

  }

  _nEvt++;

}


 
void DigisimConditionsProcessor::end(){ 

  // delete all handlers
//   for(unsigned int i=0; i< _condHandlerNames.size()  ; ++i){
//     lccd::LCConditionsMgr::instance()->removeHandler( _condHandlerNames[i] );
//   }
}



#endif // #ifdef USE_LCCD
