#include "BifWriteEngine.hh"

#include <cfloat>
#include <iostream>
#include <string>
#include <cstdlib>

/*lcio*/
#include "EVENT/LCCollection.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/CellIDDecoder.h"

/*calice*/
#include "BIFBlock.hh"

using namespace lcio;
using namespace std;
using namespace CALICE;

#define DDEBUG(name) ;std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) ;std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void BifWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter("BifWriteEngine_prefix",
						   "BifWriteEngine prefix to tree variables",
						   _prefix,
						   std::string("Bif"));

    _hostProcessor.relayRegisterInputCollection(LCIO::LCGENERICOBJECT,_engineName+"_InCol",
						"Name of input collection",
						_bifColName, std::string("BIFData")  );
  }


  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/

  void BifWriteEngine::registerBranches( TTree* hostTree )
  {
    if ( _prefix.size() > 0 )
      if ( _prefix[ _prefix.length()-1 ] != '_' )
	_prefix += "_";

    hostTree->Branch(string(_prefix+"StartAcq").c_str(),     &_bifFill.StartAcq, 
		     string(_prefix+"StartAcq/L").c_str());
    hostTree->Branch(string(_prefix+"StopAcq").c_str(),     &_bifFill.StopAcq, 
		     string(_prefix+"StopAcq/L").c_str());
    hostTree->Branch(string(_prefix+"nTrigger").c_str(),     &_bifFill.nTrigger, 
		     string(_prefix+"nTrigger/I").c_str());
    hostTree->Branch(string(_prefix+"source").c_str(),     &_bifFill.source, 
		     string(_prefix+"source["+_prefix+"nTrigger]/I").c_str());
    hostTree->Branch(string(_prefix+"BXID").c_str(),     &_bifFill.BXID, 
		     string(_prefix+"BXID["+_prefix+"nTrigger]/I").c_str());
    hostTree->Branch(string(_prefix+"Time").c_str(),     &_bifFill.Time, 
		     string(_prefix+"Time["+_prefix+"nTrigger]/F").c_str());

  }

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void BifWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    LCCollection* inCol;
    
    try {
      inCol = evt->getCollection( _bifColName );

      streamlog_out(DEBUG) <<"\n Event "<<evt->getEventNumber()<<", loop over collection "<<_bifColName
			   <<" with "<<inCol->getNumberOfElements()<<" elements "<<endl;

      unsigned long long int start = 0;
      unsigned long long int stop = 0;
      int ntrigger = 0;

      //Get Start and Stop acquisition
      std::string start_str = inCol->getParameters().getStringVal("Start_Acquisition");
      std::string stop_str = inCol->getParameters().getStringVal("Stop_Acquisition");

      if(start_str != "")
	start = std::stoll(start_str);
      if(stop_str != "")
	stop = std::stoll(stop_str);

      for ( int Counter = 0; Counter < inCol->getNumberOfElements(); Counter++ ) 
	{
	  BIFBlock *bif = dynamic_cast<BIFBlock*>(inCol->getElementAt(Counter));

	  int _source = bif->getTriggerSource();
	  int _BXID = bif->getBXID();
	  float _Time = bif->getTime();

	  //write vector to ROOT file
	  _bifFill.source[ntrigger] = _source;
	  _bifFill.BXID[ntrigger] = _BXID;
	  _bifFill.Time[ntrigger] = _Time;

	  ntrigger++;
	}

      _bifFill.StartAcq = start;
      _bifFill.StopAcq = stop;
      _bifFill.nTrigger = ntrigger;

    }/*try*/
    
    catch ( DataNotAvailableException err ) 
      {
	resetBifFill();
	streamlog_out(DEBUG) <<  "BifWriteEngine WARNING: Collection "<< _bifColName
			     << " not available in event "<< evt->getEventNumber() << endl;
      }/*catch*/
    
  }/*fillVariables*/

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/

  void BifWriteEngine::resetBifFill() 
  {
    _bifFill.StartAcq = 0;
    _bifFill.StopAcq = 0;
    _bifFill.nTrigger = 0;

    for (unsigned int i = 0; i<MAXTRIGGERS; i++)
      {
	_bifFill.source[i] = 0;
	_bifFill.BXID[i] = 0;
	_bifFill.Time[i] = 0;
      }
  }

}//namespace
