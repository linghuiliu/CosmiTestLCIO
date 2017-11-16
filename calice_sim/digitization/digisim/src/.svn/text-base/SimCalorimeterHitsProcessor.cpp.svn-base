#include "SimCalorimeterHitsProcessor.hpp"
#include "EVENT/LCIO.h"
#include "EVENT/RawCalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "marlin/StringParameters.h"
#include <iostream>
#include <cassert>

using namespace IMPL;
using namespace marlin;
using namespace std;

namespace digisim {

  // Global instance needed to register with the framework
  SimCalorimeterHitsProcessor aSimCalorimeterHitsProcessor;

  //.. constructor
  SimCalorimeterHitsProcessor::SimCalorimeterHitsProcessor() : Processor("SimCalorimeterHitsProcessor") {
    _description = " RawCalHit -> SimCalHit conversion processor (see http://nicadd.niu.edu/digisim/ for details.)";

    vector<string> empty;
    registerProcessorParameter( "InputCollections",
				"Names of input collections to be converted",
				_rawNames,
				empty );

    registerProcessorParameter( "Raw2SimCollections",
				"Names of input collections to be converted",
				_linkNames,
				empty );

    registerProcessorParameter( "OutputCollections",
				"Names of input collections to be converted",
				_outputNames,
				empty );

    registerProcessorParameter( "EnergyFactor",
				"Energy conversion factor",
				_eneFactor,
				1.0 );

    registerProcessorParameter( "TimeFactor",
				"Time conversion factor",
				_timeFactor,
				1.0 );
  }

  //.. destructor
  SimCalorimeterHitsProcessor::~SimCalorimeterHitsProcessor() {

//     cout<< "Destructor was called! "<< name() <<" "<<_rawNames.size()<<" "<< _outputNames.size() <<" "<< _linkNames.size() << endl;
    for(vector<string>::iterator icol = _rawNames.begin();
	icol != _rawNames.end() ; ++icol ) {
      _rawNames.erase(icol);
    }
    for(vector<string>::iterator icol = _linkNames.begin();
	icol != _linkNames.end() ; ++icol ) {
      _linkNames.erase(icol);
    }
    for(vector<string>::iterator icol = _outputNames.begin();
	icol != _outputNames.end() ; ++icol ) {
      _outputNames.erase(icol);
    }
    _rawNames.clear();
    _linkNames.clear();
    _outputNames.clear();
    if(_converter) delete _converter;
  }


//   void SimCalorimeterHitsProcessor::processRunHeader( LCRunHeader* run) {
//     cout << "SimCalorimeterHitsProcessor::processRun()  " << name()
// 	 << " in run " << run->getRunNumber()
// 	 << endl ;
//     _nRun++ ;
//   }


  void SimCalorimeterHitsProcessor::processEvent( LCEvent * evt) {
    ++_nEvt;
    _evt = evt;

    // loop over all collections in the event
    assert( _rawNames.size() == _linkNames.size() );
    for(unsigned int i=0; i<_rawNames.size(); ++i) {

      string rawname = _rawNames[i];
      string linkname = _linkNames[i];
      string outname = _outputNames[i];

//       // get raw hits collection
// //     vector<RawCalorimeterHit*>& rawhits = this.getRawHits(rawname);
//       LCCollection* rawHits = _evt->getCollection(rawname);

      // get LCRelations collection
//     vector<RawCalorimeterHit*>& rawhits = this.getLinks(rawname);
      LCCollection* raw2SimLinks = _evt->getCollection(linkname);

      // convert raw hits to sim hits
      _converter->process( raw2SimLinks );

      vector<const SimCalorimeterHitImpl*>& newHits = _converter->getSimCalorimeterHits();

      // prepare new SimCalorimeterHits collection
      LCCollection* calhitColl = createOutputCollection( newHits );

      // append collection to event
      evt->addCollection( calhitColl, outname );
    }
  }

  LCCollection* SimCalorimeterHitsProcessor::createOutputCollection
  (
   const vector<const SimCalorimeterHitImpl*>& newhits
   )
  {
    // Create output collection
    LCCollection* newColl = new LCCollectionVec( LCIO::SIMCALORIMETERHIT );
    LCFlagImpl newFlag(0);
//     newFlag.setBit( LCIO::CHBIT_NO_PTR );
    newFlag.setBit( LCIO::RCHBIT_TIME );
    newFlag.setBit( LCIO::RCHBIT_ID1);
    newColl->setFlag( newFlag.getFlag() );

    // loop over new hits
    for(vector<const SimCalorimeterHitImpl*>::const_iterator iter=newhits.begin(); iter!=newhits.end(); ++iter) {
      SimCalorimeterHitImpl* newhit = const_cast<SimCalorimeterHitImpl*>(*iter);
      // add this hit to new collection
      newColl->addElement( newhit );
    }

    return newColl;
  }


//   map<const RawCalorimeterHit*, vector<const LCRelation*>>& buildLinksMap(const vector<const LCRelation>& links) {
//     // clear links map
//     this.clearLinksMap();

//     // loop over links
//     for( vector<const LCRelation>::const_iterator iter = links->begin(); iter != links->end() ; ++iter ) {
//       const LCRelation* rel = *iter;
//       const RawCalorimeterHit* rawhit = (const RawCalorimeterHit*)rel->getFrom();

//       if( _linksMap->count(rawhit) == 0 ) {
// 	// hit not in map yet, so add it now
// 	vector<LCRelation*> relvec;
// 	relvec.push_back( rel );
// 	_linksMap->insert( pair<const RawCalorimeterHit*,vector<const LCRelation*> >(rawhit, relvec) );
//       }
//       else {
// 	// just add a new relation
// 	vector<LCRelation*>& relvec = _linksMap->find( rawhit )->second;
// 	relvec.push_back( rel );
//       }
//     }

//     return _linksMap;
//   }

  void SimCalorimeterHitsProcessor::init() {
    // initialization from steering file
    StringParameters* pars = parameters();
    cout << "SimCalHitsProcessor.init(): parametes=<"<< *pars << ">" << endl;
    _nEvt = 0;
    _nRun = 0;

    // setup RawColl -> SimHits converter
    _converter = new Raw2SimConverter(_eneFactor, _timeFactor);
  }

  void SimCalorimeterHitsProcessor::end() {
    cout << "SimCalorimeterHitsProcessor::end()  " << name()
	 << " processed " << _nEvt << " events in " << _nRun << " runs "
	 << endl ;
    // self destruction!!!  It seems Marlin does not destruct it...
    delete this;
  }

}// namespace digisim
