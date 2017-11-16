/**
 * An interface between DigiSim classes and the MARLIN framework.
 * It should instantiate the Digitizer objects and transmit to them
 * both parameters and events from the framework.
 *
 * @author Guilherme Lima, C. DeCaro
 * @version $Id: DigiSimProcessor.cpp,v 1.5 2007-03-28 18:25:02 magnan Exp $
 */
#include "CalHitModifier.hpp"
#include "Digitizer.hpp"
#include "DigiSimProcessor.hpp"
#include "CalHitMapMgr.hpp"
#include "TempCalHit.hpp"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IMPL/LCRelationImpl.h"
#include "EVENT/LCParameters.h"
#include "marlin/StringParameters.h"
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>

using std::string;
using std::cout;
using std::endl;
using std::list;
using std::vector;
using marlin::StringParameters;
using namespace IMPL;
using EVENT::FloatVec;
using std::hex;
using std::dec;

using namespace lcio;
using namespace marlin;
using namespace std;

namespace digisim {

  // Global instance needed to register with the framework
  DigiSimProcessor aDigiSimProcessor;

  //.. constructor
  DigiSimProcessor::DigiSimProcessor() : Processor("DigiSimProcessor") {
    _description = " Digitization processor (see http://nicadd.niu.edu/digisim/ for details.)";


    registerProcessorParameter( "Debug",
				"Debug mode",
				_debug,
				static_cast<int>(0) );

    registerProcessorParameter( "InputCollection",
				"Name of the input SimCalorimeterHit collection",
				_inputColl,
				std::string("ProtoDesy0506_ProtoSD03") );

    registerProcessorParameter( "OutputCollection",
				"Name of the output raw hits collection",
				_outputColl,
				std::string("HcalBarrRawHits") );

    registerProcessorParameter( "Raw2SimLinksCollection",
				"Name of the collection with raw to sim links",
				_linksColl,
				std::string("HcalBarrRaw2Sim") );

    vector<string> empty;
    registerProcessorParameter( "ModifierNames",
				"Names of the modifiers to be run",
				_modifNames,
				empty );
  }

  //.. destructor
  DigiSimProcessor::~DigiSimProcessor() {

    list<CalHitModifier*>::iterator imod = _modifs.begin();
    while( imod != _modifs.end() ) {
      if(*imod) delete *imod;
      ++imod;
    }
    _modifs.clear();
  }

  void DigiSimProcessor::init() {
    // initialization from steering file
    StringParameters* pars = parameters();
    Digitizer digitizer(name());
    _digitizers.push_back(digitizer);
    _digitizers[0].init(pars);

    _nEvt = 0;
    _nRun = 0;
  }

  void DigiSimProcessor::processRunHeader( LCRunHeader* run) {
    cout << "DigiSimProcessor::processRun()  " << name()
	 << " in run " << run->getRunNumber()
	 << endl ;

    _nRun++ ;
  }


  void DigiSimProcessor::processEvent( LCEvent * evt) {
    ++_nEvt;
    _digitizers[0].process(evt);
  }

  void DigiSimProcessor::end() {
    cout << "DigiSimProcessor::end()  " << name()
	 << " processed " << _nEvt << " events in " << _nRun << " runs "
	 << endl ;
  }

  void DigiSimProcessor::printRelation( const EVENT::LCCollection* col ) const
  {
    if( col->getTypeName() != LCIO::LCRELATION ) {
      cout << " collection not of type " << LCIO::LCRELATION << endl ;
      return ;
    }

    cout<< endl
	<<"-------- "<<"print out of " << LCIO::LCRELATION << " collection "
	<<"--------------- " << endl ;

    cout << endl
	 << "  flag:  0x" << hex  << col->getFlag() << dec << endl ;

    int nRel =  col->getNumberOfElements() ;

    cout<<" fromType : "<< col->getParameters().getStringVal("FromType")<<endl;
    cout<<" toType : "  << col->getParameters().getStringVal("ToType")  <<endl;

    cout << endl <<  " |  [from_id]  |  [to_id]   | weight "  << endl ;

    for( int i=0; i < nRel ; i++ ){

      LCRelation* rel = dynamic_cast<LCRelation*>( col->getElementAt(i) ) ;
      printf(" | [%8.8x] |  [%8.8x]   | %5.3e \n"
	     , rel->getFrom() == NULL ? 0 : ((RawCalorimeterHit*)rel->getFrom())->getCellID0()
	     , ((SimCalorimeterHit*)rel->getTo())->getCellID0()
	     , rel->getWeight()
	     );
    }
  }

}// namespace digisim
