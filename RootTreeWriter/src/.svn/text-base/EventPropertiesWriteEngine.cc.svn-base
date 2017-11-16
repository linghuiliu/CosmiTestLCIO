#include "EventPropertiesWriteEngine.hh"

#include <cmath>
#include <cfloat>

#include "EVENT/ReconstructedParticle.h"
#include "UTIL/LCTypedVector.h"

using namespace std;
using namespace lcio;

#define DDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  void EventPropertiesWriteEngine::registerParameters()
  {
    // use reconstructed particles as input
    _hostProcessor.relayRegisterInputCollection(LCIO::RECONSTRUCTEDPARTICLE,_engineName+"_InCol",
						"Name of ReconstructedParticle collection",
						_recPartColName, std::string("Unset")  );

  }

  void EventPropertiesWriteEngine::registerBranches( TTree* hostTree )
  {
    hostTree->Branch("eventKin", &_eventKin ,"sumE/D:sumP[3]/D:sqrtS/D" );

  }


  void EventPropertiesWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    
    LCCollection* inCol;
    try
      {
	inCol = evt->getCollection( _recPartColName );

	_eventKin.sumE    = 0;
	_eventKin.sumP[0] = 0;
	_eventKin.sumP[1] = 0;
	_eventKin.sumP[2] = 0;
//	_eventKin.sqrtS   = 0;

	typedef LCTypedVector< ReconstructedParticle > LCTypedVecRP;
	typedef LCTypedVecRP::iterator RPIter;
	LCTypedVecRP parts( inCol );
	for ( RPIter rpIt = parts.begin(); rpIt != parts.end(); ++rpIt )
	  {
	    const double* mom = (*rpIt)->getMomentum();
	    
	    _eventKin.sumP[0] += mom[0];
	    _eventKin.sumP[1] += mom[1];
	    _eventKin.sumP[2] += mom[2];

	    _eventKin.sumE += (*rpIt)->getEnergy();
	  }

	
	double magP2 = _eventKin.sumP[0] * _eventKin.sumP[0] 
	  /*      */ + _eventKin.sumP[1] * _eventKin.sumP[1] 
	  /*      */ + _eventKin.sumP[2] * _eventKin.sumP[2];

	_eventKin.sqrtS = sqrt( _eventKin.sumE *  _eventKin.sumE - magP2 );

      }// try
    catch ( DataNotAvailableException err )
      {
	cout <<  "WARNING: Collection "<< _recPartColName
	     << " not available in event "<< evt->getEventNumber() << endl;

	_eventKin.sumE    = INVALIDF;
	_eventKin.sumP[0] = INVALIDF;
	_eventKin.sumP[1] = INVALIDF;
	_eventKin.sumP[2] = INVALIDF;
	_eventKin.sqrtS   = INVALIDF;
	
      }// catch

    
  }

}//namespace marlin
