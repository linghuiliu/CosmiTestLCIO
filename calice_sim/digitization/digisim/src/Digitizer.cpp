/**
 * A subdetector digitizer.  This class manages the digitization
 * simulation process for a specific subdetector.
 *
 * @author Guilherme Lima, C.DeCaro
 * @version $Id: Digitizer.cpp,v 1.7 2008-02-01 16:04:11 lima Exp $
 */

#include "CalHitModifier.hpp"
#include "CalHitMapMgr.hpp"
#include "Digitizer.hpp"
#include "TempCalHit.hpp"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IMPL/LCRelationImpl.h"
#include "marlin/StringParameters.h"
#include "EVENT/CalorimeterHit.h"
#include <cassert>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cstdlib>
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

namespace digisim {

  //Basic Constructor
  Digitizer::Digitizer(string name) {
    _name = name;
  }

  Digitizer::~Digitizer() {
    for(vector<CalHitModifier*>::const_iterator iter=_modifs.begin(); iter!=_modifs.end(); ++iter ) {
      if(*iter!=NULL) delete *iter;
    }
    _modifs.clear();
  }

  std::map<long long, TempCalHit> Digitizer::createTempHits(const CalHitMap& simHits) {

    std::map<long long, TempCalHit> tmpHits;

    // creating a temp hit for each sim hit
//     cout<< _name <<": #simHits = "<< simHits.size() << endl;
    for(CalHitMap::const_iterator i=simHits.begin(); i != simHits.end(); i++) {
      // FIXME: DigiSim should also take CaloHits as input.  Currently only SimCalHits are possible
//       const SimCalorimeterHit *psim = dynamic_cast<const SimCalorimeterHit*>(i->second);
//       const CalorimeterHit *pcal = dynamic_cast<const CalorimeterHit*>(i->second);
// //       cout<<"psim="<< psim <<", pcal="<< pcal << endl;

      const SimCalorimeterHit& simHit = *(i->second);

      //use the same input cellID for output cellID
      long long id = static_cast<long long>(simHit.getCellID1()) << 32;
      id = id + static_cast<long long>(simHit.getCellID0());
      TempCalHit tmphit(id, simHit.getEnergy(), simHit.getTimeCont(0) );

      tmpHits[i->first] = tmphit;
    }
    return tmpHits;
  }

  void Digitizer::createOutputCollections(
	  const std::map<long long, TempCalHit>   tmpHits,
	  LCCollection*& rawVec, LCCollection*& relVec)
  {
    // Create collection for writing raw hits into LCIO file
    rawVec = new LCCollectionVec( LCIO::RAWCALORIMETERHIT );
    LCFlagImpl rawFlag(0) ;
    //   rawFlag.setBit( LCIO::RCHBIT_NO_PTR );
    rawFlag.setBit( LCIO::RCHBIT_TIME );
    rawFlag.setBit( LCIO::CHBIT_ID1);
//     rawFlag.setBit( LCIO::CHBIT_LONG);
    rawVec->setFlag( rawFlag.getFlag() );

    // and another collection to save the LCRelations between raw and sim hits
    relVec = new LCCollectionVec( LCIO::LCRELATION );
    relVec->parameters().setValue( "FromType", LCIO::RAWCALORIMETERHIT );
    relVec->parameters().setValue( "ToType", LCIO::SIMCALORIMETERHIT );
    LCFlagImpl relFlag(0);
    relFlag.setBit( LCIO::LCREL_WEIGHTED );
    relVec->setFlag( relFlag.getFlag()  );

    //Loop over all temp hits to create raw hits
    const CalHitMap& simHits
      = CalHitMapMgr::getInstance()->getCollHitMap( _inputColl );

    for(std::map<long long, TempCalHit>::const_iterator i=tmpHits.begin(); i != tmpHits.end(); i++) {
      const TempCalHit& tmphit = i->second;
      //conversions to integers
      int adc = static_cast<int>(tmphit.getTotalEnergy()+0.5);
      int time = static_cast<int>(tmphit.getPrimaryTime()+0.5);
      //create a raw hit for each temp hit
      //long long cellid = tmphit.getContributingIDs()[0];
      long long cellid = tmphit.getCellID();
      //cout << "the cell ID is finally: " << cellid << endl;
      RawCalorimeterHitImpl* rawhit = new RawCalorimeterHitImpl();
      rawhit->setCellID0( cellid );
      rawhit->setCellID1( cellid >> 32);
      rawhit->setAmplitude( adc );
      rawhit->setTimeStamp( time );

      rawVec->addElement( rawhit );

      const std::vector<double> energies = tmphit.getEnergyContributions();
      const std::vector<long long> ids = tmphit.getContributingIDs();
      assert(energies.size() == ids.size() );

      //checking weights
      double etot = 0;
      for(unsigned int i=0; i<ids.size(); ++i) {
	etot +=energies[ i ];
      }
      for(unsigned int i=0; i<ids.size(); ++i) {
	//fetch each sim hit
	long long simid = ids[ i ];
	SimCalorimeterHit* simhit = simHits.find(simid)->second;

	// calculating weight
	double weight = energies[ i ] / etot;
	// create LCRelation object
	LCRelation* prel = new LCRelationImpl( rawhit, simhit, weight );
	// save it
	relVec->addElement( prel );
      }
    }
  }


  void Digitizer::init(StringParameters* pars) {

    std::vector<ModifierParameters> modifParams;
    std::vector<double> dfloats;

    _inputColl = pars->getStringVal("InputCollection");
    _outputColl = pars->getStringVal("OutputCollection");
    _linksColl = pars->getStringVal("Raw2SimLinksCollection");
    _debug = pars->getIntVal("Debug");

    if (_debug) {
      std::cout << " Param chosen:"
		<<" InputCol " << _inputColl
		<<", OutputCol " << _outputColl
		<<", LinksCol " << _linksColl
		<< std::endl;
    }

    StringVec modifNames;
    pars->getStringVals("ModifierNames", modifNames );
    if (_debug) {
      std::cout << " Modifiers chosen : " ;
      for (std::vector<string>::iterator imod = modifNames.begin(); imod != modifNames.end(); imod++){
	std::cout << (*imod) << " " ;
      }
      std::cout << std::endl;
    }

    // Initialize the modifiers
    std::vector<string>::iterator imod = modifNames.begin();
    int ipar = 0;
    StringVec tempPars;
    while( imod != modifNames.end() ) {
      dfloats.clear();
      tempPars.clear();

      string key(modifNames[ipar]);
      tempPars = pars->getStringVals( key, tempPars );

      // remove the first element (type) name string
      string type = pars->getStringVal( modifNames[ipar] );
      tempPars.erase( tempPars.begin() );

      // After removing the first name string, 
      // convert the rest string to dboule.
      for(unsigned int i = 0; i < tempPars.size(); i++) {
	const char * s = tempPars[i].c_str();
	char * pEnd;
	double dfloat = strtod( s, &pEnd);
	dfloats.push_back(dfloat);	
      }

      if (_debug) {
	cout << "\n Initializing modif# "<< ipar
	     << ": # float parameters = "<< dfloats.size() << endl;
      }

      ModifierParameters parms(modifNames[ipar], type, dfloats);

      modifParams.push_back(parms);
      ++imod;
      ++ipar;
    }

    initialize(modifParams);
  }//init method


  /**
   * Instantiate modifiers, assuming the collection names have been set
   * @param modifiers a vector with ModifierParameters objects.
   * Each ModifierParameters object will be associated to its own
   * modifier, creating a chain of modifiers for this digitizer.
   */
  void Digitizer::initialize(std::vector<ModifierParameters> modifiers) {
    assert (modifiers.size() > 0);
    std::vector<float> ffloats;

    _nRun = 0;
    _nEvt = 0;

    //Create all modifiers needed.  For each one, choose the appropriate modifier among the available.

    int counter = 0;
    for(std::vector<ModifierParameters>::iterator i=modifiers.begin(); i != modifiers.end(); i++) {
      cout << "\n*** "<< (i)->getName() <<"::init()  " << endl;
      string type = (i)->getType();
      cout << "modifier " << counter << ", type: "<<type << endl;
      // make sure there is a global instance for this type of modifier (see Globals.cpp)
      CalHitModifier* pglobal = CalHitModifier::_modifiersAvailable[ type ];
      assert(pglobal);

      CalHitModifier* newmod = pglobal->newInstance( (i)->getName() );

      ffloats.clear();
      for(unsigned int count = 0; count < i->getParams().size(); count++) {
	float ffloat = i->getParams()[count];
	ffloats.push_back(ffloat);
      }


      newmod->init( ffloats );
      _modifs.push_back( newmod );
      ++counter;

      newmod->setDebug( 0 );
      newmod->print();
    }

  }


  /** Called for every event - the event loop */

  void Digitizer::process( LCEvent * evt ) {

    string detName = evt->getDetectorName();

    // Create map storage for transient hits to be smeared
    const CalHitMap& hits = CalHitMapMgr::getInstance()->getCollHitMap(_inputColl);

    if (_debug) std::cout << " Number of hits in the collection : " << hits.size() << std::endl;

    std::map<long long, TempCalHit> tmpHits = createTempHits( hits );

    //*** Call processHits method for each modifier
    std::vector<CalHitModifier*>::iterator i;
    for(i=_modifs.begin(); i != _modifs.end(); i++) {
      (*i)->processHits(tmpHits);
    }

    //create output collections and append them to the LCEvent
    //could contain 0 hits !!!
    LCCollection* rawColl = NULL;
    LCCollection* relColl = NULL;
    createOutputCollections(tmpHits, rawColl, relColl);

    evt->addCollection( rawColl, _outputColl );
    evt->addCollection( relColl, _linksColl );

    tmpHits.clear();

    ++_nEvt;
  }//end of process()

}
