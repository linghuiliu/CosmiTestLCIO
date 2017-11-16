#include <cmath>
#include <cassert>

#include "TcmtOverlayProcessor.hh"
#include "lcio.h"
#include "lccd.h"
#include "marlin/Processor.h"
#include "TcmtHit.hh"
#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCFlagImpl.h"

using namespace std;

#define TCMTDIGI_DEBUG

namespace CALICE 
{
  
  TcmtOverlayProcessor aTcmtOverlayProcessor;

  /*******************************************************************************************/
  /*                                                                                         */
  /*                                                                                         */
  /*                                                                                         */
  /*******************************************************************************************/
  TcmtOverlayProcessor::TcmtOverlayProcessor()
    : BaseMappingIIProcessor("TcmtOverlayProcessor")
    , _noiseColName("TcmtNoiseHits")
    , _noiseColType(0)
    , _mipGev(1)
    , _energyThreshold(0.5)
    , _maxDiff(0)
    , _maxDiffEvt(-1)
  {
    _description = "Merge processor for the TCMT";
    
    registerProcessorParameter("NoiseCollectionName", "Name of the noise collection", 
			       _noiseColName, _noiseColName);
    
    registerProcessorParameter("NoiseCollectionType", "Type of the noise collection", 
			       _noiseColType, _noiseColType);
    
    registerProcessorParameter("MipPerGeV", "MPV of raw energy deposited by a minimum ionizing particle",
			       _mipGev, _mipGev);
    
    registerProcessorParameter("EnergyThreshold", "energy threshold in MIP for zero suppresion",
			       _energyThreshold, _energyThreshold);
  };
  
  /*******************************************************************************************/
  /*                                                                                         */
  /*                                                                                         */
  /*                                                                                         */
  /*******************************************************************************************/
  void TcmtOverlayProcessor::init() 
  {
    streamlog_out(DEBUG) <<"*** TcmtOverlay: init() called"<< endl;
    printParameters();
    
    streamlog_out(DEBUG) <<"*** TcmtOverlay: calling BaseMappingII.init()..."<< endl;
    BaseMappingIIProcessor::init();
    streamlog_out(DEBUG)<<"*** TcmtOverlay: init() done!"<< endl;
  };
  
  
  /*******************************************************************************************/
  /*                                                                                         */
  /*                                                                                         */
  /*                                                                                         */
  /*******************************************************************************************/
  void TcmtOverlayProcessor::processEvent(LCEvent *evt) 
  {
  
    typedef std::map<int,float> AmplitudeMap;
    AmplitudeMap _mcHits, _mcHitError2;
    double esumMC=0;
    double esumNoise=0;
    double esumDigi=0;
    
    LCCollection* noiseVector;
    try 
      {
      noiseVector = evt->getCollection(_noiseColName);      
      streamlog_out(DEBUG) << "noise data found" << std::endl;      
    }
    catch(DataNotAvailableException &e) 
      {      
      streamlog_out(DEBUG) << "TcmtOverlayProcessor::processEvent(): no noise data available" << std::endl;      
      noiseVector = 0;
      }
    
    // process MC hits
    lcio::LCCollection* inVector(NULL);
    try 
      {
	inVector = evt->getCollection(_inputColName);
      }
    catch (DataNotAvailableException &e) 
      {
	streamlog_out(DEBUG) << "TcmtOverlayProcessor::processEvent(): no input MC collection available" << std::endl;
      }
    
    LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    // write 3d coordinates
    LCFlagImpl hitFlag(_outputCol->getFlag());
    hitFlag.setBit(LCIO::CHBIT_LONG);
    _outputCol->setFlag(hitFlag.getFlag());

    /*Angela Lucaci: change TCMT encoding also for digitized hits;
      hardcoded, no good, should be created somehow automatically*/
    EVENT::LCParameters & theParam = _outputCol->parameters();
    theParam.setValue(LCIO::CellIDEncoding,"M:3,S-1:3,I:9,J:9,K:6");
    
    // fill map with MC hits
    if(inVector) 
      {
	for (unsigned i=0; i < static_cast<unsigned>(inVector->getNumberOfElements()); i++) 
	  {
	    TcmtHit* hit = dynamic_cast<TcmtHit*>(inVector->getElementAt(i));
	    if(!hit) 
	      {
		// input hits are CalorimeterHits
		CalorimeterHit* aCalHit = dynamic_cast<CalorimeterHit*>(inVector->getElementAt(i));
		if(aCalHit) 
		  {
		    int cellid = aCalHit->getCellID0();
		    float energy = aCalHit->getEnergy();
		    hit = new TcmtHit(cellid,energy,0,0);
		  }
	      }
	    
	    if(hit) 
	      {
		const unsigned cellid = hit->getCellID();
		float ampl = hit->getEnergyValue();
		float amplError = hit->getEnergyError();
		esumMC += ampl;
		
		streamlog_out(DEBUG) <<"TcmtOverlay: MChit: "<< cellid << " (" << std::hex << cellid << std::dec << ") "
				     << ampl << " " << amplError << std::endl;
		
		ampl /= _mipGev;
		amplError /= _mipGev;
		_mcHits[cellid] += ampl;
		_mcHitError2[cellid] += amplError*amplError;
		
		streamlog_out(DEBUG) << "TcmtOverlayProc: cellid="<< cellid <<" E=" << hit->getEnergyValue() << " -> " << ampl << std::endl; 
		
	      } 
	    else 
	      {
		std::stringstream message;
		message << "Collection " << _inputColName << " doesn't contain valid calorimeter hits" << std::endl;
		throw std::runtime_error(message.str());
	      }	
	  }
      }
    
    
    // add noise
    if (noiseVector) 
      {
	streamlog_out(DEBUG) <<"# noise hits: "<< noiseVector->getNumberOfElements() << endl;

	for( unsigned i=0; i < static_cast<unsigned>(noiseVector->getNumberOfElements()); i++) 
	  {
	    unsigned cellid = 0;
	    float ampl = 0;
	    
	    if(_noiseColType!=1) 
	      {
		// noise in TcmtHit format
		TcmtHit* noise = dynamic_cast<TcmtHit*>(noiseVector->getElementAt(i));
		if(noise) 
		  {
		    cellid = noise->getCellID();
		    ampl = noise->getEnergyValue();
		    // 	  amplError = noise->getEnergyError();
		  }
		else 
		  {
		    // I don't think this will ever be processed...
		    std::stringstream message;
		    message << "Collection " << _noiseColName << " doesn't contain TcmtHits" << std::endl;
		    throw std::runtime_error(message.str());
		  }
	      }
	    else 
	      {
		// noise in CalorimeterHit format
		CalorimeterHit* noise = dynamic_cast<CalorimeterHit*>(noiseVector->getElementAt(i));
		if(noise) 
		  {
		    cellid = noise->getCellID0();
		    ampl = noise->getEnergy();
		    // 	  float amplError = noise->getEnergyError();
		  }
		else 
		  {
		    // I don't think this will ever be processed...
		    std::stringstream message;
		    message << "Collection " << _noiseColName << " doesn't contain CalorimeterHits, as requested in steering file." << std::endl;
		    throw std::runtime_error(message.str());
		  }
	      }
	    
	    esumNoise += ampl;
	    
	    streamlog_out(DEBUG) <<"added noise "<< std::hex << cellid << std::dec
				 <<" E="<< _mcHits[cellid] <<" + "<< ampl
				 <<" = "<< (_mcHits[cellid]+ampl) << std::endl;
	    _mcHits[cellid] += ampl;
	    //_mcHitError2[cellid] += amplError*amplError;
	    _mcHitError2[cellid] += 0.0;
	  }
      }
    
    // store in output collection
    for (AmplitudeMap::const_iterator mapIter=_mcHits.begin(); mapIter!=_mcHits.end(); mapIter++) 
      {    
	const unsigned cellid = mapIter->first;
	const float ampl = mapIter->second;
	//     const float amplError = sqrt( _mcHitError2[cellid] );
	
	esumDigi += ampl;
	
	if(ampl>_energyThreshold) 
	  {
	    streamlog_out(DEBUG) << " new digitized hit: cellid=" << std::hex << cellid << std::dec
				 << " " << ampl << std::endl;
	    
	    //         TcmtHit* newHit = new TcmtHit(cellid, ampl, amplError, 0);
	    CalorimeterHitImpl* newHit = new CalorimeterHitImpl();
	    if(newHit) 
	      {
		newHit->setCellID0(cellid);
		newHit->setEnergy(ampl);
		
		// FIXME: should have a class to manage this bit decoding
		unsigned layer = (cellid >> 24) + 1;
		unsigned stripx = (cellid >> 6) & 0x1ff;
		unsigned stripy = (cellid >> 15) & 0x1ff;
		unsigned strip = stripx>0 ? (stripx-1) : (stripy-1);
		
		// debug output
		streamlog_out(DEBUG0) << "layer: " << layer << "  " << "strip: " << strip << std::endl;
		
		assert(layer>0 && layer<=16);
		assert(strip>=0 && strip<=19);
		
		ThreeVector_t _myPos = _mapping.getPosition(layer-1, strip);
		newHit->setPosition(_myPos.data());
		
		_outputCol->addElement(newHit);
	      }
	  }
      }
    
    streamlog_out(DEBUG) <<"TcmtOverlay: #hits: MC="<< inVector->getNumberOfElements()
			 <<" noise="<< noiseVector->getNumberOfElements()
			 <<" merged="<< _mcHits.size()
			 <<" zeroSuppressed="<< _outputCol->getNumberOfElements() << endl;
    
    
    // some energy accounting, to make sure that noise is being properly added
    double diff = fabs(esumMC+esumNoise-esumDigi);
    if( diff>_maxDiff ) 
      {
	_maxDiff = diff;
	_maxDiffEvt = evt->getEventNumber();
      }
    //   cout<<" esumMC="<< esumMC <<", esumNoise="<< esumNoise <<", esumDigi="<< esumDigi
    //       <<", DIFF="<< diff <<" maxDiff="<< _maxDiff <<" in evt="<< _maxDiffEvt << endl;
    
    // add new collection to the event
    if (_outputCol->getNumberOfElements() > 0)
      evt->addCollection(_outputCol, _outputColName);
  }
  
  /*******************************************************************************************/
  /*                                                                                         */
  /*                                                                                         */
  /*                                                                                         */
  /*******************************************************************************************/
  void TcmtOverlayProcessor::end() 
  {
    // some energy accounting, to make sure that noise is being properly added
    streamlog_out(DEBUG) <<"*** TcmtOverlay ("<< _outputColName <<"): maxDiff(esumMC+esumNoise-esumDigi) = "<< _maxDiff
			 <<" in event "<< _maxDiffEvt << endl;
  }
  
} // end namespace CALICE
