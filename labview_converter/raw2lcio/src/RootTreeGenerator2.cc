#include "RootTreeGenerator2.hh"

#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include <UTIL/CellIDDecoder.h>

namespace CALICE {
  
  RootTreeGenerator2 aRootTreeGenerator2;
  
  RootTreeGenerator2::RootTreeGenerator2() : Processor("RootTreeGenerator2") {
    _description = "Processor to generate root tree for T0 currently";

    registerProcessorParameter("InputCollectionName",
                               "Name of the input collection of Labview raw data",
                               _inputColName,
                               std::string("AHcalHits"));
    
    registerProcessorParameter("OutputRootFileName",
                               "Name of the output root file",
                               _rootFileName,
                               std::string("LabviewDataCERN.root"));
    
  }


  RootTreeGenerator2::~RootTreeGenerator2() {}
  
  void RootTreeGenerator2::init() {

    _rootFile = new TFile( _rootFileName.c_str(), "recreate" );

    _tree = new TTree( "bigtree", "collect information from DD4hep and slic" );
    _tree->Branch( "ESum_Layer", &_Esum_Layer, "ESum_Layer[500]/D" );
    _tree->Branch( "ESum", &_Esum, "ESum/D" );
    _tree->Branch( "Event_number", &_event_num, "Event_number/I" );
    _tree->Branch( "nHits", &nHits, "nhits/I" );
    _tree->Branch( "nHits_Layer", &_nHits_Layer, "nhits_Layer[500]/I" );
    _tree->Branch( "K", &K, "K[400000]/I" );
    _tree->Branch( "I", &I, "I[400000]/I" );
    _tree->Branch( "J", &J, "J[400000]/I" );
    _tree->Branch( "hitEnergy", &hitEnergy, "hitEnergy[400000]/F" );
    _tree->Branch( "hitTimestamp", &hitTimestamp, "hitTimestamp[400000]/F" );
    _tree->Branch( "hitPos", &hitPos, "hitPos[400000][3]/F" );

  }

  void RootTreeGenerator2::processEvent(LCEvent* evt){

    try {
      //fetch Labview data raw collection
      LCCollection* inCol = evt->getCollection( _inputColName ) ;

      _Esum = 0;
      for (int i = 0; i<500;i++){
	_Esum_Layer[i]=0;
	_nHits_Layer[i]=0;
      }

      _event_num = evt->getEventNumber();

      int _nHits = inCol->getNumberOfElements();

      CellIDDecoder<SimCalorimeterHit> idDecoder( inCol );

      nHits = 0;

      for (int ielm=0; ielm < _nHits; ielm++) {

	
	SimCalorimeterHit *thisHit = dynamic_cast<SimCalorimeterHit*>(inCol->getElementAt(ielm));


	int L = idDecoder(thisHit)["layer"];

	_Esum_Layer[L] += thisHit->getEnergy();

	_Esum += thisHit->getEnergy();
	_nHits_Layer[L]++;

	const float* thisHitPos =  thisHit->getPosition();
	hitEnergy[nHits]    = thisHit->getEnergy();
	hitTimestamp[nHits] = thisHit->getTimeCont(0);
	hitPos[nHits][0]    = thisHitPos[0];
	hitPos[nHits][1]    = thisHitPos[1];
	hitPos[nHits][2]    = thisHitPos[2];
	K[nHits]            = idDecoder(thisHit)["layer"];
	I[nHits]            = idDecoder(thisHit)["x"];
	J[nHits]            = idDecoder(thisHit)["y"];

	nHits++;

      }

    } catch (  lcio::DataNotAvailableException &err ) {
      err.what();
      return;
    }
    
      _tree->Fill();

   }
  
  void RootTreeGenerator2::end()
  {

    _tree->Write();
    _rootFile->Close();
  }
  
}
