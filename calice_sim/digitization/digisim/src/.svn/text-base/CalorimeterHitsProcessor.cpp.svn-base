#include "CalorimeterHitsProcessor.hpp"
#include "EVENT/LCIO.h"
#include "EVENT/RawCalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "marlin/StringParameters.h"
#include "CalHitMapMgr.hpp"
#include <fstream>
#include <cassert>

using namespace IMPL;
using namespace marlin;
using namespace std;

namespace digisim {

  // Global instance needed to register with the framework
  CalorimeterHitsProcessor aCalorimeterHitsProcessor;

  /***********************************************************************************/
  /*                                                                                 */
  /* constructor                                                                     */
  /*                                                                                 */
  /***********************************************************************************/
  CalorimeterHitsProcessor::CalorimeterHitsProcessor()
    : Processor("CalorimeterHitsProcessor")
    , _rawName("invalid")
    , _outputName("invalid")
    , _posRefName("invalid")
    , _deferPositionAssignments(true)
  {
    _description = " RawCalHit -> CalHit conversion processor (see http://nicadd.niu.edu/digisim/ for details.)";

    vector<string> empty;
    registerProcessorParameter( "InputCollection",
				"Name of input collection to be converted",
				_rawName,
				_rawName );

    registerProcessorParameter( "PositionReference",
			        "Name of either a file or an input LCIO collection used as reference for hit positions",
			        _posRefName,
			        _posRefName);

    registerProcessorParameter( "OutputCollection",
				"Name of output LCIO collection",
				_outputName,
				_outputName);

    registerProcessorParameter( "EnergyFactor",
				"Energy conversion factor",
				_eneFactor,
				1.0 );

    registerProcessorParameter( "TimeFactor",
				"Time conversion factor",
				_timeFactor,
				1.0 );
  }

  /***********************************************************************************/
  /*                                                                                 */
  /* destructor                                                                      */
  /*                                                                                 */
  /***********************************************************************************/
  CalorimeterHitsProcessor::~CalorimeterHitsProcessor() 
  {
    if(_converter) delete _converter;
  }

  /***********************************************************************************/
  /*                                                                                 */
  /*                                                                                 */
  /*                                                                                 */
  /***********************************************************************************/
  void CalorimeterHitsProcessor::processEvent( LCEvent * evt) 
  {
    ++_nEvt;
    _evt = evt;
    
    string& rawname = _rawName;
    string& refname = _posRefName;
    string& outname = _outputName;
    streamlog_out(DEBUG)<<" rawName="<< rawname <<", refName="<< refname << endl;

    LCCollection* rawHits = _evt->getCollection(rawname);

    /* convert raw hits to calibrated hits*/
    _converter->process( rawHits );

    /* prepare new CalorimeterHits collection*/
    vector<const CalorimeterHitImpl*>& newHits = _converter->getCalorimeterHits();
    const map<long long, SimCalorimeterHit*>* simHits = NULL;
    if(!_positionsFromFile) simHits = &(CalHitMapMgr::getInstance()->getCollHitMap(refname));

//     cout<<"CalHitsProcessor: #hits raw="<< rawHits->getNumberOfElements() <<" sim="<< newHits.size() << endl;
//     // loop over raw hits
//     for(unsigned i=0; i<newHits.size(); ++i) {
//       RawCalorimeterHit* ihit = (RawCalorimeterHit*)rawHits->getElementAt(i);
//       cout<<" raw hit "<< i <<" "<< hex << ihit->getCellID0() << dec <<", E="<< ihit->getAmplitude() << endl;
//     }

    //*** set hit positions!  Temporary?!
    // loop over newHits
    for(unsigned ihit = 0; ihit < newHits.size(); ++ihit) 
      {
	CalorimeterHitImpl* calhit = const_cast<CalorimeterHitImpl*>(newHits[ihit]);
	long long cellid = calhit->getCellID0();
	streamlog_out(DEBUG)<<" new simhit "<< ihit <<" "<< hex << calhit->getCellID0() << dec <<", E="<< calhit->getEnergy() << endl;
	
	if( _deferPositionAssignments == false ) 
	  {
	    /* figure out position of new hit*/
	    float pos[3];
	    if(_positionsFromFile) 
	      {
		const vector<double>& tmppos = _positionsMap.find(cellid)->second;

		if( tmppos.size()==3 ) 
		  {
		    pos[0] = tmppos[0];
		    pos[1] = tmppos[1];
		    pos[2] = tmppos[2];
		  } 
		else 
		  {
		    streamlog_out(DEBUG)<< name() << " problem? Evt#"<< _evt->getEventNumber() <<" cellid=0x"<< hex << cellid <<dec<<", size="<< tmppos.size() << endl;
		    assert ( tmppos.size()==3 );
		  }		
	      }
	    else 
	      {
		/* use positions from a reference LCIO collection*/
		SimCalorimeterHit* simhit = simHits->find( cellid )->second;
		long long cellid1 = simhit->getCellID0();
		assert(cellid==cellid1);
		const float* tmppos = simhit->getPosition();
		pos[0] = tmppos[0];
		pos[1] = tmppos[1];
		pos[2] = tmppos[2];	
	      }
	    
	calhit->setPosition( pos );
      }
    }
    //*** end temporary

    LCCollection* calhitColl = createOutputCollection( newHits );

    /* append collection to event*/

    if (calhitColl->getNumberOfElements()>0)
      evt->addCollection( calhitColl, outname );

  }

  /***********************************************************************************/
  /*                                                                                 */
  /*                                                                                 */
  /*                                                                                 */
  /***********************************************************************************/
  LCCollection* CalorimeterHitsProcessor::createOutputCollection( const vector<const CalorimeterHitImpl*>& newhits )
  {
    /* Create output collection*/
    LCCollection* newColl = new LCCollectionVec( LCIO::CALORIMETERHIT );
    LCFlagImpl newFlag(0);
    if(!_deferPositionAssignments) newFlag.setBit( LCIO::RCHBIT_LONG );
    newFlag.setBit( LCIO::RCHBIT_TIME );
    newFlag.setBit( LCIO::RCHBIT_ID1);
    newColl->setFlag( newFlag.getFlag() );

    /* loop over new hits*/
    for(vector<const CalorimeterHitImpl*>::const_iterator iter=newhits.begin(); iter!=newhits.end(); ++iter) {
      CalorimeterHitImpl* newhit = const_cast<CalorimeterHitImpl*>(*iter);
      // add this hit to new collection
      newColl->addElement( newhit );
    }

    return newColl;
  }

  /***********************************************************************************/
  /*                                                                                 */
  /*                                                                                 */
  /*                                                                                 */
  /***********************************************************************************/
  void CalorimeterHitsProcessor::init() 
  {
    //*initialization from steering file*/
    cout << "CalHitsProcessor.init(): parameters=<"<< *parameters() << ">" << endl;
    _nEvt = 0;
    _nRun = 0;

    /* setup RawHits -> CalHits converter*/
    _converter = new RawHitConverter(_eneFactor, _timeFactor);

    /* assume that LCIO collection name does not contain a dot*/
    string::size_type loc = _posRefName.find( ".", 0 );

    if( loc != string::npos ) 
      {
	streamlog_out(DEBUG) << "DigiSim::CaloHitsProcessor: Using positions from an external file " << _posRefName << endl;
	_positionsFromFile = true;
	_deferPositionAssignments = false;
      } 
    else 
      {
	_positionsFromFile = false;

	if( _posRefName=="invalid" || _posRefName=="none" ) 
	  {
	    streamlog_out(DEBUG) << "DigiSim::CaloHitsProcessor: Deferring hit position assignments to downstream processors (position reference: " << _posRefName <<")"<< endl;
	    _deferPositionAssignments = true;
	  }
      else 
	{
	  streamlog_out(DEBUG) << "DigiSim::CaloHitsProcessor: Using positions from reference LCIO collection " << _posRefName << endl;
	  _deferPositionAssignments = false;
	}
      }

    if(_positionsFromFile) 
      {
	ifstream file(_posRefName.c_str());
	
	long long cellid;
	double x,y,z;

	while(!file.eof()) 
	  {	
	    file >> hex >> cellid >> dec >> x >> y >> z ;
	    if(!file.eof()) 
	      {
		streamlog_out(DEBUG)<<"from file: "<< hex << cellid << dec << " "<< x <<" "<< y <<" "<< z << endl;
		_positionsMap.insert(pair<long long, vector<double> >(cellid,vector<double>()));
		vector<double>& pos = _positionsMap.find( cellid )->second;
		pos.push_back( x );
		pos.push_back( y );
		pos.push_back( z );
		
		streamlog_out(DEBUG)<<"Position map: "<< hex << cellid << dec
			    << " "<< pos[0] <<" "<< pos[1] <<" "<< pos[2] <<" size="<< pos.size() <<" "<< _positionsMap.size() << endl;
		assert( pos.size()==3 || !(cout<<"\nERROR: Please check parameter PositionReference in your steering file.\n"<< endl) );
	      }
	  }
      }
  }
  
  /***********************************************************************************/
  /*                                                                                 */
  /*                                                                                 */
  /*                                                                                 */
  /***********************************************************************************/
  void CalorimeterHitsProcessor::end() 
  {
    cout << "CalorimeterHitsProcessor::end()  " << name()
	 << " processed " << _nEvt << " events in " << _nRun << " runs "
	 << endl ;
    
  }
}// namespace digisim
