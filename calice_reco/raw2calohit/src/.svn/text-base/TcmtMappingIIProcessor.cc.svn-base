#include "TcmtMappingIIProcessor.hh"
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "marlin/ConditionsProcessor.h"
#include "TcmtHit.hh"
#include "FastDecoder.hh"

using namespace std;

namespace CALICE {

  TcmtMappingIIProcessor aTcmtMappingIIProcessor;


  /*****************************************************************************************************/
  /*                                                                                                   */
  /*                                                                                                   */
  /*                                                                                                   */
  /*****************************************************************************************************/
  TcmtMappingIIProcessor::TcmtMappingIIProcessor()
    : BaseMappingIIProcessor("TcmtMappingIIProcessor")
  {
    streamlog_out(DEBUG0)<<"*** TcmtMappingII: constructor called"<< endl;
    _description = "This processor reads calibrated TcmtHits and applies part II of the mapping.";
    
    registerProcessorParameter("EnergyThreshold",
			       "Minimum energy required to save the hit",
			       _energyThreshold,
			       (float)0.5);
    
    registerProcessorParameter("OutputCollectionType",
			       "Format of output hits: (0) CalorimeterHits (default) or (1) TcmtHits",
			       _outputCollectionType, (int)0);
  }
  

  /*****************************************************************************************************/
  /*                                                                                                   */
  /*                                                                                                   */
  /*                                                                                                   */
  /*****************************************************************************************************/
  void TcmtMappingIIProcessor::init() 
  {
    streamlog_out(DEBUG0)<<"*** TcmtMappingII: init() called"<< endl;
    printParameters();
    
    streamlog_out(DEBUG0)<<"*** TcmtMappingII: calling BaseMappingII.init()..."<< endl;
    BaseMappingIIProcessor::init();
    streamlog_out(DEBUG0)<<"*** TcmtMappingII: init() done!"<< endl;
  }
  
  
  /*****************************************************************************************************/
  /*                                                                                                   */
  /*                                                                                                   */
  /*                                                                                                   */
  /*****************************************************************************************************/
  void TcmtMappingIIProcessor::processEvent(LCEvent* evt) 
  {
    streamlog_out(DEBUG0)<<"\n*** TcmtMappingII: processEvent() <<"<<evt->getEventNumber()<<" called"<< endl;
    LCCollection* inVector = NULL;
    try 
      {
	inVector = evt->getCollection(_inputColName);
      }
    catch(DataNotAvailableException &e) 
      {
	streamlog_out(DEBUG0)<<"TcmtMapII: data not available: " << _inputColName <<" in event "<<evt->getEventNumber()<< endl;
      }
    
    if(inVector == NULL) return;
  
    LCCollectionVec* _outputCol;
    
    if(_outputCollectionType != 1) 
      {
	_outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);

	// write 3d coordinates
	LCFlagImpl hitFlag(_outputCol->getFlag());
	hitFlag.setBit(LCIO::RCHBIT_LONG);
	_outputCol->setFlag(hitFlag.getFlag());
	
	EVENT::LCParameters & theParam = _outputCol->parameters();
	//Set the cell decoder which might be useful in event displays
	//suggested by Allister
	//hardcoded, no good, should be created somehow automatically
	//theParam.setValue(LCIO::CellIDEncoding,"M:3,S-1:3,I:9,J:9,K-1:6");

	/*Angela Lucaci: change the encoding to get TCMT layers from 1 to 16, not from 0 to 15*/
 	theParam.setValue(LCIO::CellIDEncoding,"M:3,S-1:3,I:9,J:9,K:6");
      }
    else 
      {
	_outputCol = new LCCollectionVec(LCIO::RAWCALORIMETERHIT);
	lcio::LCFlagImpl hitFlag(_outputCol->getFlag());
	hitFlag.setBit(LCIO::RCHBIT_TIME);
	//     hitFlag.setBit(LCIO::CHBIT_ID1);
	_outputCol->setFlag(hitFlag.getFlag());
      }
    
    assert( _outputCol );
    
    /*------------------------------------------------------------------------------
      loop over input hits
    */
    
    std::string encodingString = "M:3,S-1:3,I:9,J:9,K-1:6";
    FastDecoder* Kcoder = FastDecoder::generateDecoder(encodingString,"K");

    for (int i = 0; i < inVector->getNumberOfElements(); i++) 
      {
	TcmtHit* oldHit = static_cast<TcmtHit*>(inVector->getElementAt(i));
	
	// apply energy cut here
	float energy( oldHit->getEnergyValue() );
	if(energy<_energyThreshold) continue;
	
	// for Tcmt modID is stored as chip and map.getModuleID() returns 0
	unsigned moduleID = oldHit->getChip();
	unsigned location = _inverseModuleMap[ moduleID ];
	unsigned strip = oldHit->getChannel();
	ThreeVector_t _myPos = _mapping.getPosition(location, strip);

	streamlog_out(DEBUG0)<<"   location="<<location<<" strip="<<strip<<endl;
	
	if(_outputCollectionType !=1 ) 
	  {
	    CalorimeterHitImpl* aCalorimeterHit = new CalorimeterHitImpl();
	    aCalorimeterHit->setCellID0(_mapping.getGeometricalCellIndex(location,strip));
	    aCalorimeterHit->setEnergy(oldHit->getEnergyValue());
	    aCalorimeterHit->setTime(oldHit->getTimeStamp());
	    aCalorimeterHit->setPosition(_myPos.data());
	    if(aCalorimeterHit) _outputCol->addElement(aCalorimeterHit);   
	    
	    
	    streamlog_out(DEBUG0)<<"TcmtMapII hit: modID="<< oldHit->getChip()
			 <<" layer="<< ((aCalorimeterHit->getCellID0() >> 24) & 0x3f)
			 <<" strip="<< oldHit->getChannel()
			 <<", cellID=<"<< hex << aCalorimeterHit->getCellID0() << dec <<">"
			 <<", E="<< aCalorimeterHit->getEnergy();
	    
	    streamlog_out(DEBUG0) <<", pos=("<< aCalorimeterHit->getPosition()[0]
			  << "; " << aCalorimeterHit->getPosition()[1]
			  << "; " << aCalorimeterHit->getPosition()[2] <<")"<<endl;

	    unsigned int K = Kcoder->decodeU(aCalorimeterHit->getCellID0());
	    streamlog_out(DEBUG0)<<"----> Kcoder: K="<<K<<endl;
	    
	  }
	else 
	  {
	    TcmtHit* aTcmtHit = new TcmtHit(_mapping.getGeometricalCellIndex(location,strip),
					    oldHit->getEnergyValue(), 0, oldHit->getTimeStamp());
	    if(aTcmtHit) _outputCol->addElement(aTcmtHit);
	    
	    
	    streamlog_out(DEBUG0)<<"TcmtMapII hit: modID="<< moduleID
			 <<" layer="<< ((aTcmtHit->getCellID() >> 24) & 0x3f)
			 <<" strip="<< oldHit->getChannel()
			 <<", cellID=<"<< hex << aTcmtHit->getCellID() << dec <<">"
			 <<", E="<< aTcmtHit->getEnergyValue()
			 <<", position=("<<_myPos.data()[0]
			 <<","<<_myPos.data()[1]
			 <<","<<_myPos.data()[2]<<")"
			 <<endl;
	    
	  }
	
	
      }

    if(_outputCol->getNumberOfElements() > 0) 
      {
	evt->addCollection(_outputCol, _outputColName);
      }
    streamlog_out(DEBUG0)<<"*** TcmtMappingII::processEvent(): done!"<< endl;
  }
  

  /*****************************************************************************************************/
  /*                                                                                                   */
  /*                                                                                                   */
  /*                                                                                                   */
  /*****************************************************************************************************/
//   void TcmtMappingIIProcessor::check(LCEvent* evt) 
//   {
//     streamlog_out(DEBUG0)<<"*** TcmtMappingII::check() called"<< endl;
//   }
  
  
  /*****************************************************************************************************/
  /*                                                                                                   */
  /*                                                                                                   */
  /*                                                                                                   */
  /*****************************************************************************************************/
//   void TcmtMappingIIProcessor::end() 
//   {
//     streamlog_out(DEBUG0)<<"*** TcmtMappingII::end() called"<< endl;
//   }
  


}
