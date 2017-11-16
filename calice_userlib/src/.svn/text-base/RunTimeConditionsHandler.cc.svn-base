#include "RunTimeConditionsHandler.hh"

#include "lcio.h"

#include "IO/LCReader.h"
#include "IMPL/LCCollectionVec.h"

#include <iostream>
#include <sstream>

using namespace lcio ;
using namespace lccd ;

namespace CALICE {

  /**************************************************************************/
  /*                                                                        */
  /*                                                                        */
  /*                                                                        */
  /**************************************************************************/
  RunTimeConditionsHandler::RunTimeConditionsHandler( const std::string& name ) :  
    ConditionsHandlerBase(name) 
  {
    _col = 0;
    _evt = 0;
    _missedAddEvent = false;

    /* create an empty collection for this listener: later this could be a global for all listeners*/
    _defaultCol = NULL;
    _lastValidCollection = NULL;   
  }
  
  /**************************************************************************/
  /*                                                                        */
  /*                                                                        */
  /*                                                                        */
  /**************************************************************************/
  RunTimeConditionsHandler::~RunTimeConditionsHandler() 
  {
    if( _col ) 
      delete _col ;

    if ( _defaultCol )
      delete _defaultCol;

    if ( _lastValidCollection )
      delete _lastValidCollection;
  }
  
  /**************************************************************************/
  /*                                                                        */
  /*                                                                        */
  /*                                                                        */
  /**************************************************************************/
  void RunTimeConditionsHandler::update( LCCDTimeStamp timestamp ) 
  {
  }    

  /**************************************************************************/
  /*                                                                        */
  /*                                                                        */
  /*                                                                        */
  /**************************************************************************/
  void RunTimeConditionsHandler::updateEvent( LCEvent* evt) 
  {
    _evt = evt;
  }    

  /**************************************************************************/
  /*                                                                        */
  /*                                                                        */
  /*                                                                        */
  /**************************************************************************/
  void RunTimeConditionsHandler::update( LCCollection *col ) 
  {

    /* set infinite validity range*/
    _validSince =  LCCDMinusInf ;
    _validTill = LCCDPlusInf ;
    

    std::stringstream mess;
    mess<<"RunTimeConditionsHandler::update: no default collection set for conditions handler: "
	<<this->name()<<std::endl;

    /*don't throw exception, but send the default collection*/
    if ( _defaultCol )
      {
	col = _defaultCol;
      }
    else
      {
	//throw lcio::Exception(mess.str());
	std::cout<<mess.str()<<", do nothing"<<std::endl;
      }

    if ( _col != _defaultCol )
      {
	if (_lastValidCollection)
	  delete _lastValidCollection;

	_lastValidCollection = _col;
      }

   _col = col;

   if (_col != _defaultCol)
     _col->parameters().setValue("CollectionName", this->name());
    
    if (_evt) 
      {
	_evt->addCollection( currentCollection() , name()  ) ;
	
	/* take away ownership of collection*/
	_evt->takeCollection( name() ) ;
	_evt = 0;
      }


    /*
    std::cout <<  "\n RunTimeConditionsHandler: about to notify listeners for collection "
    <<name() << std::endl;
    std::string colName = col->getParameters().getStringVal("CollectionName") ;
    std::cout<<"   with actual name "<<colName<<std::endl;
    */

    notifyListeners() ;
    
  }

  /**************************************************************************/
  /*                                                                        */
  /*                                                                        */
  /*                                                                        */
  /**************************************************************************/
  void RunTimeConditionsHandler::registerDefaultCollection( LCCollection *col ) 
  {
    if (_defaultCol == NULL)
      {
	_defaultCol = col;
      }
    else
      {
	std::stringstream mess;
	mess<<"RunTimeConditionsHandler::registerDefaultCollection"
	    <<" Default collection already set for RunTimeConditionsHandler: "
	    <<this->name()<<"  "<<_defaultCol<<std::endl;
	throw lcio::Exception(mess.str());
      }
    
  }


  
}
