#include "ROCThresholdProcessor.h"
#include <iostream>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/MCParticle.h>
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include <IMPL/LCFlagImpl.h>
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"


// ----- ROOT --------
#include "TMath.h"
#include "TROOT.h"





#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
//#include <AIDA/IHistogram1D.h>
#endif // MARLIN_USE_AIDA


using namespace lcio ;
using namespace marlin ;


ROCThresholdProcessor aROCThresholdProcessor ;


ROCThresholdProcessor::ROCThresholdProcessor() : Processor("ROCThresholdProcessor") {

  // register steering parameters: name, description, class-variable, default value

  registerInputCollection(LCIO::SIMCALORIMETERHIT,
			  "ROCThresholdProcessor_simHitInputCollection",
			  "Name of SimCalorimeterHit input collections",
			  _calorimInpCollection, 
			  std::string("hcalSD") );


  registerOutputCollection( LCIO::CALORIMETERHIT,
			    "ROCThresholdProcessor_calHitOutputCollection" , 
			    "Name of the Calorimeter Hit output collection"  ,
			    _calorimOutCollection,
			    std::string("ahcal_ROCThr") ) ;

   registerProcessorParameter("ROCThresholdProcessor_tileEdge",
			     "tile edge dimension in mm",
			     _tileEdge,
			     float(30.0));

   registerProcessorParameter("ROCThresholdProcessor_deadSpace",
			     "dead space between tiles in mm",
			     _deadSpace,
			     float(0.1));

  registerProcessorParameter("ROCThresholdProcessor_MIP",
			     "MIP value in GeV",
			     _MIPvalue,
			     float(0.000477));

  registerProcessorParameter("ROCThresholdProcessor_Threshold",
			     "MIP threshold fraction",
			     _MIPThr,
			     float(0.5));

  registerProcessorParameter("ROCThresholdProcessor_tfast",
			     "fast shaper time in ns",
			     _tfast,
			     float(15));

  registerProcessorParameter("ROCThresholdProcessor_tslow",
			     "slow shaper time in ns",
			     _tslow,
			     float(50));



}

/************************************************************************************/

void ROCThresholdProcessor::init() { 

  streamlog_out(DEBUG) << "   init called  " << std::endl ;
  energyThreshold = _MIPvalue*_MIPThr;
  halfdS = 0.5*_deadSpace;
  
  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;

  



}

/************************************************************************************/
void ROCThresholdProcessor::processRunHeader( LCRunHeader* run) { 

  _nRun++ ;
} 

/************************************************************************************/

void ROCThresholdProcessor::processEvent( LCEvent * evt ) { 
  LCCollectionVec* calOutVec = new LCCollectionVec( LCIO::CALORIMETERHIT) ;


  LCFlagImpl hitFlag(calOutVec->getFlag());
  hitFlag.setBit(LCIO::RCHBIT_TIME);
  hitFlag.setBit(LCIO::CHBIT_LONG);
  calOutVec->setFlag(hitFlag.getFlag());




  int evtNumber = evt->getEventNumber();
  if ((evtNumber % 1000) == 0) 
    std::cout<<" \n ---------> Event: "<<evtNumber<<"!!! <-------------\n"<<std::endl;

  bool calorimCollectionFound = false;

  try {//check if the input collection exists
    std::vector< std::string >::const_iterator iter;
    const std::vector< std::string >* colNames = evt->getCollectionNames();
      
    for( iter = colNames->begin() ; iter != colNames->end() ; iter++) {
      if ( *iter == _calorimInpCollection ) calorimCollectionFound = true;
    }
  }
  catch(DataNotAvailableException &e){
    std::cout <<  "WARNING: List of collection names not available in event "<< evt->getEventNumber() << std::endl;
    return;
  };

  if (calorimCollectionFound){
    LCCollection *inputCalorimCollection = evt->getCollection(_calorimInpCollection);
    int noHits = inputCalorimCollection->getNumberOfElements();
    std::string encoding = inputCalorimCollection->getParameters().getStringVal( "CellIDEncoding");
   
    CellIDDecoder<SimCalorimeterHit> decoder(inputCalorimCollection);
    CellIDEncoder<CalorimeterHit> encoder(encoding.c_str(),calOutVec);
	  
   
    for (int i = 0; i < noHits; i++){
      SimCalorimeterHit *aSimCalorimHit = dynamic_cast<SimCalorimeterHit*>(inputCalorimCollection->getElementAt(i));
	
      int noSubHits = aSimCalorimHit->getNMCContributions();
 

      vector<float> subhitsTime;
      vector<float> subhitsEnergy;
      subhitsTime.resize(noSubHits);
      subhitsEnergy.resize(noSubHits);
	  
      for(int j = 0; j < noSubHits; j++){//fill hit time and deposited energy per sub-hit
	   
	const float *_hitStep = aSimCalorimHit->getStepPosition(j); 
	Float_t _hitOntileX =(Float_t)_hitStep[0]-(Float_t) std::floor(_hitStep[0]/_tileEdge)*_tileEdge;
	Float_t _hitOntileY =(Float_t)_hitStep[1]-(Float_t) std::floor(_hitStep[1]/_tileEdge)*_tileEdge;

	float _edepstep = (float)aSimCalorimHit->getEnergyCont(j);

	if(_hitOntileX < halfdS || _hitOntileX > (_tileEdge-halfdS) || _hitOntileY < halfdS || _hitOntileY > (_tileEdge-halfdS)) _edepstep = 0.0;
	      
	subhitsTime[j] = (float)aSimCalorimHit->getTimeCont(j);
	subhitsEnergy[j] = _edepstep;

      }//end loop on subhits

      vector<int> lookup;
      lookup.resize(noSubHits);

      TMath::Sort(noSubHits,&subhitsTime[0],&lookup[0],kFALSE);//creating look-up table for time ordered subhits

      float energy(0.);//energy sum
      int ThrIndex(0);//index of the first hit in the sliding window
      float tH(0.);//time of passing threshold
  
      float Epar(0.);
      bool passThr(false);

      /*checking for the time of signal passing threshold:
	amplitude are added only until they are in a 50 ns window*/


      for(int k=0; k<noSubHits; k++){//loop on all subhits
	ThrIndex = k;
	Epar = 0.;
	for(int isub=k; isub< noSubHits; isub++){//loop on the remaining hits
	      
	  if((subhitsTime[lookup[isub]]-subhitsTime[lookup[ThrIndex]]) <_tfast){//check if the following hit is within tfast
	    Epar += subhitsEnergy[lookup[isub]];
	  }else{break;}//endif //if time is > tfast exit loop on remaining hits

	  if(Epar>energyThreshold){//check of energy sum vs threshold
	    tH = subhitsTime[lookup[isub]];//ste time hit = last hit time
	    passThr = true;		
	    break;//exit loop on remaining hits
	  } //endif 
     
	}//end loop on remaining hits
	if(passThr==true) break; //if threshold had passed exit loop on all subhits
      }//end loop

	  
      /*checking if the subhits happen within the risetime of the slow shaper*/
      if(passThr==true){
	for(int j=ThrIndex; j<noSubHits; j++){
	  if(subhitsTime[lookup[j]] < (subhitsTime[lookup[ThrIndex]]+_tslow)){
	    energy += subhitsEnergy[lookup[j]];
	  }
	}
	CalorimeterHitImpl * aCalorimHit =  new CalorimeterHitImpl();

	const float *hitpos = aSimCalorimHit->getPosition();

	aCalorimHit->setTime(tH);
	aCalorimHit->setEnergy(energy);
	aCalorimHit->setPosition(hitpos);
	aCalorimHit->setCellID0(aSimCalorimHit->getCellID0());


	calOutVec->addElement(dynamic_cast<CalorimeterHit*>(aCalorimHit));

      }
    }//end loop over SimCalorimeterHits

    //=======================================================================

  }//end if collection found

  evt->addCollection( calOutVec, _calorimOutCollection ) ;


  //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
		       << "   in run:  " << evt->getRunNumber() << std::endl ;



  _nEvt ++ ;
}

/************************************************************************************/

void ROCThresholdProcessor::check( LCEvent * evt ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}

/************************************************************************************/
void ROCThresholdProcessor::end(){ 

  std::cout << "ROCThresholdProcessor::end()  " << name() 
  	    << " processed " << _nEvt << " events in " << _nRun << " runs "
  	    << std::endl ;

}


/************************************************************************************/
void ROCThresholdProcessor::printParameters(){
  std::cerr<<"============= ROC Threshold Processor ================="<<std::endl;
  std::cerr<<" Simulating Spiroc2 timestamping and energy measurement"<<std::endl;
  std::cerr<<" Tile Edge: "<<_tileEdge<<" mm"<<std::endl;
  std::cerr<<" Dead space bet. tiles: "<<_deadSpace<<" mm"<<std::endl;
  std::cerr<<" MIP: "<<_MIPvalue<<" GeV"<<std::endl;
  std::cerr<<" disctiminator: "<<_MIPThr<<" MIP"<<std::endl;
  std::cerr<<" fast shaper time: "<<_tfast<<" ns"<<std::endl;
  std::cerr<<" slow shaper time: "<<_tslow<<" ns"<<std::endl;
  std::cerr<<"======================================================="<<std::endl;
  return;

}
