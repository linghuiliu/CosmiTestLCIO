//2012.03.14 written by coterra Katsu
#include "ScECALDigitizer.hh"
#include <iostream>
#include <cmath>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <EVENT/SimCalorimeterHit.h>

//120314 added coterra
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/LCCollectionVec.h>
//#include <IMPL/LCFlagImpl.h>
#include <IMPL/LCRelationImpl.h>
//#include "CalorimeterHitType.h"
//#include "CHT_helper.h"
//#include <UTIL/CellIDDecoder.h>
//

#include <UTIL/LCTOOLS.h>

#include "TH1D.h"
#include "TFile.h"
#include "TNtuple.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"
#include <marlin/Global.h>

#define b120313 0


using namespace lcio ;
using namespace marlin ;


ScECALDigitizer ScECALDigitizer ;


//===================================================================================
ScECALDigitizer::ScECALDigitizer() : Processor("ScECALDigitizer") {
  
  // modify processor description
  _description = "ScECALDigitizer does whatever it does ..." ;
  

  // register steering parameters: name, description, class-variable, default value

  registerInputCollection( LCIO::MCPARTICLE,
			   "CollectionName" , 
			   "Name of the MCParticle collection"  ,
			   _colName ,
			   std::string("MCParticle") ) ;

  std::vector<std::string> ecalCollections;
  ecalCollections.push_back(std::string("ecalSD"));
//  ecalCollections.push_back(std::string("EcalEndcapCollection"));
//  ecalCollections.push_back(std::string("EcalEndcapRingCollection"));

  registerInputCollections( LCIO::SIMCALORIMETERHIT,
			    "ECALCollections",
			    "ECAL Collection Names",
			    _ecalCollections,
			    ecalCollections);

  _outputEcalCollections.push_back(std::string("SceCalorimeter_Hits"));
  registerOutputCollection( LCIO::CALORIMETERHIT,
                           "ECALOutputCollection0" ,
                           "ECAL Collection of real Hits" ,
                            _outputEcalCollections[0],
                            std::string("SceCalorimeter_Hits") );

  float thresholdEcal = 0.;
  registerProcessorParameter("EnergyThreshold"
		             ,"ScECAL energy threshold for a channel"
                             ,_thresholdEcal
                             ,thresholdEcal 
			     ); 

  float calibrEcal = 1.;
  registerProcessorParameter("CalibrECAL" , 
                             "Calibration coefficients for ECAL" ,
                              _calibrCoeffEcal,
                             calibrEcal);

  
  registerProcessorParameter("RootFileName",
			   "Root file name to output Ntuples, etc",
			   _rootFileName,
			   std::string("myanal.root"));

}


//===================================================================================
void ScECALDigitizer::init() { 

  streamlog_out(DEBUG) << "   init called  " 
		       << std::endl ;
  
  
  // usually a good idea to
  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;


  fFile=new TFile(_rootFileName.c_str(),"RECREATE");
  fHist=new TH1D("hist", "Sample", 100,0.,1000.0);
  fNEvents=new TNtuple("nte", "Event based ntuple","nevt:nmc:ne:esum");
  fNHits=new TNtuple("nth", "Hit based ntuple","nevt:e:r:x:y:z");
  
  streamlog_out(MESSAGE) << "myanal.root will be created" << std::endl;
}

//===================================================================================
void ScECALDigitizer::processRunHeader( LCRunHeader* run) { 

  _nRun++ ;
} 

//===================================================================================
void ScECALDigitizer::processEvent( LCEvent * evt ) { 

//120313.2031 coterra
  // create the output collections
    LCCollectionVec *relcol  = new LCCollectionVec(LCIO::LCRELATION);
    
  // this gets called for every event 
  // usually the working horse ...

  static bool mydebug=true;

  if ( _nEvt > 10 ) { mydebug=false; }
  if ( _nEvt%100 == 1 ) { mydebug = true ; }

  if ( mydebug ) {
    std::cout << "AMyProcess...." << std::endl;

  //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

    streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
			 << "   in run:  " << evt->getRunNumber() 
			 << std::endl ;
    LCTOOLS::dumpEvent(evt);
  }
  
  LCCollection *col=evt->getCollection( _colName );
  int nMCP=0;
  if( col != 0 ) {
     nMCP=col->getNumberOfElements();
     fHist->Fill((Double_t)nMCP);
     if( mydebug ) { streamlog_out(DEBUG) << "nMCP=" << nMCP << std::endl; }
   }

//120314.1922   CellIDDecoder<SimCalorimeterHit> idDecoder( col );

   // Reading Collections of ECAL Simulated Hits *
  int nesum=0;
  double esum=0;
  for (unsigned int i=0;i< _ecalCollections.size(); ++i) {
    try {
      std::string colName=_ecalCollections[i];
      if ( mydebug ) { streamlog_out(DEBUG) << "Collection Name=" << colName << std::endl; }
      LCCollection *col=evt->getCollection( colName.c_str() );
      if ( mydebug ) { 
	streamlog_out(DEBUG) << " Number of elements=" << col->getNumberOfElements() << std::endl; }

      // create new collection
      LCCollectionVec *ecalcol = new LCCollectionVec(LCIO::CALORIMETERHIT);
       //TODO  ecalcol->setFlag(flag.getFlag());

      nesum+=col->getNumberOfElements();
      for(int j=0;j<col->getNumberOfElements();++j) { // start of loop of col elements
	SimCalorimeterHit *hit=dynamic_cast<SimCalorimeterHit*> ( col->getElementAt(j) );
	float energy=hit->getEnergy();
	esum+=energy;
	const float *pos=hit->getPosition();
	float rad=sqrt( pos[0]*pos[0] + pos[1]*pos[1]);
	fNHits->Fill((Float_t)_nEvt,energy,rad,pos[0],pos[1],pos[2]);

        if (energy > _thresholdEcal) {
          CalorimeterHitImpl * calhit = new CalorimeterHitImpl();
          int cellid = hit->getCellID0();
          int cellid1 = hit->getCellID1();

        int n_layer =  (cellid & 0xff);
//         int n_strip = ((cellid >> 8) & 0xff );
        int n_strip = ((cellid >> 8) & 0xfff );
  //
//          float calibr_coeff(1.);

#if b120313
	  std::cout << "HELLO_b130313 cellid = " << cellid << " cellid1 = " << cellid1 << std::endl;
	  std::cout << "n_layer =  " << n_layer << ", n_strip = " << n_strip << std::endl;
//          layer = idDecoder(hit)["K"];
//         layer = idDecoder(hit)["K-1"];
#endif

//             if(_ecalGapCorrection!=0){
//                  _calHitsByStaveLayer[stave][layer].push_back(calhit);
//                  _calHitsByStaveLayerModule[stave][layer].push_back(module);
//             }


          calhit->setPosition(hit->getPosition());

//TODO 120314          calhit->setType( CHT( CHT::em, CHT::ecal, caloLayout ,  layer ) );

          calhit->setRawHit(hit);
          calhit->setCellID0(n_layer);
          calhit->setCellID1(n_strip);
#if b120313
	  std::cout << "HELLO_b130313 energy = " << energy << std::endl;
#endif

	  calhit->setEnergy( energy * _calibrCoeffEcal );
          ecalcol->addElement(calhit);
							              // make relation between hit and sim hit
          LCRelationImpl *rel = new LCRelationImpl(calhit,hit,1.);
          relcol->addElement( rel );
								      //
	} // end of condition energy > threshold.
      } // end of loop of col elements 

      evt->addCollection(ecalcol,_outputEcalCollections[i].c_str());
      
    }
    catch(DataNotAvailableException &e) { // start of -----
      streamlog_out(DEBUG) << " Data is not available exception detected." << std::endl;
    } // end of -----
  } //end of ecalcollections loop with [i]

  fNEvents->Fill((Float_t)_nEvt,(Float_t)nMCP, nesum, (Float_t)esum);

  _nEvt ++ ;
} //end of processEvent


//===================================================================================
void ScECALDigitizer::check( LCEvent * evt ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


//===================================================================================
void ScECALDigitizer::end(){ 
  
//   std::cout << "MyProcessor::end()  " << name() 
// 	    << " processed " << _nEvt << " events in " << _nRun << " runs "
// 	    << std::endl ;
   fFile->Write();

}

