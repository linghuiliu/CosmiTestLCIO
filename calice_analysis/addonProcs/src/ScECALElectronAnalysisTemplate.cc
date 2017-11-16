#include <sstream>
#include "ScECALElectronAnalysisTemplate.hh"

#define b111124_1 0

//Interface classes

//#include "AdcBlock.hh"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <math.h>

#include <CellParameter.hh>
#include <CellParameterAccess.hh>
#include <histmgr/HistMgr.hh>
#include <histmgr/HistPar.hh>
#include <histmgr/FloatHistogram1D.hh>
#include <collection_names.hh>

#include <IMPL/CalorimeterHitImpl.h>

#include <Average_t.hh>

#include <TriggerBits.hh>

using namespace std;
using namespace lcio;
using namespace marlin;
using namespace CALICE;
//using namespace lccd;

namespace marlin { // the largest loop 

  ScECALElectronAnalysisTemplate aScECALElectronAnalysisTemplate;

  ScECALElectronAnalysisTemplate::ScECALElectronAnalysisTemplate() : Processor("ScECALElectronAnalysisTemplate")

  {

    registerProcessorParameter("isMonteCarloEvents",
			       "This is MC event file ",
			       _isMonteCarloEvents,
			       (bool) false );
    
  }


  ScECALElectronAnalysisTemplate::~ScECALElectronAnalysisTemplate() {}

  void ScECALElectronAnalysisTemplate::init() {

#if b111124_1
  cout << "START_INIT" << endl;
#endif	  
      _outfile = new TFile("hist.root","RECREATE");
     //_outfile = new TFile("hist.root","NEW");
    _Esumhist = new TH1F("Esum","Esum",10000,0,10000);

//9809.1421
    _fNEvents=new TTree("nte", "Event based ntuple in TTree");
		//    ,"nevt:esum:esumh:esumhem:ecentx:ecenty:ecentz:ehmax_z:z_ehmax:emax_z:z_emax:ecentxh:ecentyh:ecentzh:ehlastlem");

             //     ,edep_zH05max,z_edepH05max
	      //9825.2040     ,EHlastLmax
	  
	_fNEvents->Branch("nevt",      &n_Evt,       "n_Evt/I");
	_fNEvents->Branch("esum",      &Esum,        "Esum/D");
	_fNEvents->Branch("esumh",     &EsumH,       "EsumH/D");
	_fNEvents->Branch("esumhem",   &EsumHinEM,   "EsumHinEM/D");
	_fNEvents->Branch("ecentx",    &EcenterX,    "EcenterX/D");
	_fNEvents->Branch("ecenty",    &EcenterY,    "EcenterY/D");
	_fNEvents->Branch("ecentz",    &EcenterZ,    "EcenterZ/D");
	_fNEvents->Branch("ehmax_z",   &edep_zHmax,  "edep_zHmax/F");
	_fNEvents->Branch("z_ehmax",   &z_edepHmax,  "z_edepHmax/I");
	_fNEvents->Branch("emax_z",    &edep_zmax,   "edep_zmax/F");
	_fNEvents->Branch("z_emax",    &z_edepmax,   "z_edepmax/I");
	_fNEvents->Branch("ecentxh",   &EcenterHX,   "EcenterHX/D");
	_fNEvents->Branch("ecentyh",   &EcenterHY,   "EcenterHY/D");
	_fNEvents->Branch("ecentzh",   &EcenterHZ,   "EcenterHZ/D");
	_fNEvents->Branch("ehlastl",   &EHlastLmax,  "EHlastLmax/F");
         // EHlastLmaxEM does not mean that Lastrayer in ECAL. it means that 
	 // it occurred in the area which ECAL makes the project on HCAL.
	_fNEvents->Branch("ehlastlem", &EHlastLmaxEM,"EHlastLmaxEM/F");
			
    _fNHits=new TNtuple("nth", "Hit based ntuple","nevt:e:x:y:z:layer:strip:oddeven");
    _fNHitsH=new TNtuple("nthh", "Hit based ntuple in Hcal","nevt:e:x:y:z:layer:tyle");

//tmp      streamlog_out(MESSAGE) << "myanal.root will be created" << std::endl;
 
//9816.1825 replaced from "processEvent"
//
//    nlayerH = 38;
    layerHD = 0;
    zposHD = 0;
    for ( int i = 0; i< nlayerH; i++ ) {
    zposH[i] = 0;
    }
    
    ifstream file_hcalzpos(
//111124.1237	"/scratch/coterra/frmAzusa/scecal_codes/calib_data/hcalzpos.dat");
	"/home/coterra/tempo_data/hcalzpos.dat");
     if (file_hcalzpos==0){
 	cout<<"!!!ERROR HCAL Z POSITION DATA FILE DOES NOT EXIST!!!"<<endl;
     } 
     do {
	file_hcalzpos>>layerHD>>zposHD;
	zposH[layerHD] = zposHD;
     }  while (!file_hcalzpos.eof());


  }


  void ScECALElectronAnalysisTemplate::processEvent(LCEvent* evt){

#if b111124_1
	    cout << "START_EVT" << endl;
#endif

//    const int nlayerH = 38;

    Esum=0.;
    EsumH=0.;
    EsumHinEM=0.; //9825.1150 esum in H but in (+-110,+-110) = (x,y)
    double Ecenter[3] = {0.};
    EcenterX=0.;
    EcenterY=0.;
    EcenterZ=0.;
    double Esumx=0.;
    double Esumy=0.;
    double edep_zH[nlayerH]={0};
    double edep_zH05[nlayerH]={0};
    double EcenterH[3] = {0.};
    EcenterHX = 0.;
    EcenterHY = 0.;
    EcenterHZ = 0.;
    EHlastLmax = 0.; // to reject muon in event in 220 mm x 220 mm
    EHlastLmaxEM = 0.;
   //9902.1319
    double  edep_z[nlayer] = {0};
    
#if b111124_1
      cout << "BEFORE_GET_EVT" << endl;
#endif

    TriggerBits triggerConf = evt->getParameters().getIntVal(PAR_TRIGGER_CONF);
    TriggerBits triggerEvent = evt->getParameters().getIntVal(PAR_TRIGGER_EVENT);
    //b408.1905 UInt_t reco_state=static_cast<UInt_t>(evt->getParameters().getIntVal(PAR_RECO_STATE));
    unsigned int reco_state=static_cast<unsigned int>(evt->getParameters().getIntVal(PAR_RECO_STATE));
    streamlog_out(DEBUG) << "reco_state = " << reco_state << endl;
    if (reco_state==kRecoStateBeam || reco_state==kRecoStateCalibration) return;

    //Some Event info
    //9809.1532
     n_Evt = evt->getEventNumber(); 
    //9809.1533  if (evt->getEventNumber()%100==1) {
     if (n_Evt%100==0) {
   //9809.1533 cout << evt->getEventNumber() << " events processed." << endl;   
      cout << n_Evt << " events processed." << endl;   
      LCTime EventTime(evt->getTimeStamp());
      //cout << "Event Time: " << EventTime.getDateString() << endl;   

#if b111124_1
            cout << "BEFORE_TRIGGER" << endl;
#endif

      if(triggerConf.isBeamTrigger()) printf("conf: isBeamTrigger\n");
      triggerConf.print(cout);
      cout << endl;
      triggerEvent.print(cout);
      cout << endl;
      if(triggerEvent.isPureBeamTrigger()) printf("conf: isPureBeamTrigger\n");
      if(triggerEvent.isPedestalTrigger()) printf("conf: isPedestalTrigger\n");
      if(triggerEvent.isPurePedestalTrigger()) printf("conf: isPurePedestalTrigger\n");
      if(triggerEvent.isCalibTrigger()) printf("conf: isCalibTrigger\n");
      if(triggerEvent.isPureCalibTrigger()) printf("conf: isPureCalibTrigger\n");   
    }

    // Reject non-beam events
//     if (!triggerEvent.isPureBeamTrigger()) return;
    if (!triggerEvent.isPureBeamTrigger() && !_isMonteCarloEvents) return;

#if b111124_1
          cout << "AFTER_TRIGGER" << endl;
#endif


    const StringVec* strVec = evt->getCollectionNames() ;
    for( StringVec::const_iterator name = strVec->begin() ; name !=
	   strVec->end() ; name++){ ////////////////////////////////////
	    ////////////////////////////////////////////////////////////
	    ////////////////////////////////////////////////////////////
	    /////     Collection name scan  ////////////////////
	    ////////////////////////////////////////////////////////////
      streamlog_out(DEBUG) << "CollectionName = " << *name << endl;
#if b111124_1
	          cout << "BEFORE_CHECK_COLLECTION" << endl;
#endif

      if(*name == "SceCalorimeter_Hits") { // got  ECAL 
	_inCol = evt->getCollection("SceCalorimeter_Hits") ;
	//cout << "DEBYG  " << _inCol->getNumberOfElements() << endl;

	// loop over ScECAL hits
	for (int i=0;i<_inCol->getNumberOfElements();i++) { //loop in ECAL hits
	  CalorimeterHit* aCalorimeterHit 
	    = dynamic_cast<CalorimeterHit*>(_inCol->getElementAt(i));

	  // layer and strip.
	  int layer = aCalorimeterHit->getCellID0();
	  int strip = aCalorimeterHit->getCellID1();

	//9809.1518
        const float *pos = aCalorimeterHit->getPosition();

	  // calibrated energy, in unit of MIPs.
	  double energy = aCalorimeterHit->getEnergy();
          if ( energy > 0 ) {
            energy = energy;
          } else {
            energy = 0.;
          } 

	  edep_z[layer] += energy;
	  
        int    oddeven=0;
  
	//9815.1713 energy center
	double posWeightE[3] = {0};
	posWeightE[2] = pos[2] * energy; 
	Ecenter[2] += posWeightE[2];
	
	//using only  detailed layers.
	if ( layer%2==1 ) {
	  Esumx += energy;
	  posWeightE[0] = pos[0] * energy; 
	  Ecenter[0] += posWeightE[0];
	  oddeven = 1;
	} else if ( layer%2==0 ) {
	  Esumy += energy;
	  posWeightE[1] = pos[1] * energy; 
	  Ecenter[1] += posWeightE[1];
	  oddeven = 2;
        }

	//120508.2002
	streamlog_out(DEBUG) << "n_Evt,energy,pos[0],pos[1],pos[2],layer,strip,oddeven" << endl 
			     << n_Evt << ", " << energy << ", " << pos[0] << ", " << pos[1]
			     << ", " << pos[2] << ", " << layer << ", " <<  strip << ", "
			     << oddeven << endl;

	//9809.1526  
	  _fNHits->Fill(n_Evt,energy,pos[0],pos[1],pos[2],layer,strip,oddeven);

	  Esum += energy;

	}   /// the end of ECAL hits loop
     }	else if(*name == "AhcCalorimeter_Hits") { // HCAL search
	_inColH = evt->getCollection("AhcCalorimeter_Hits");

	for ( int i = 0; i < _inColH->getNumberOfElements();i++ ) { //HCAL loop
	  CalorimeterHit* aHCalorimeterHit
	    = dynamic_cast<CalorimeterHit*>(_inColH->getElementAt(i));
 
        //9816.1408
	const float *posH = aHCalorimeterHit->getPosition();

	
//9826        if( (-110<posH[0]&&posH[0]<110) && (-110<posH[1]&&posH[1]<110) ) { //
		////////////////  ( -110 < x and y  <  +110 ) loop 
	
        int layer = aHCalorimeterHit->getCellID0();
        int tile = aHCalorimeterHit->getCellID1();
	 
	  double energyH = aHCalorimeterHit->getEnergy();
#if 0
  	cout << "energyH: " << energyH  << endl;	
#endif
	//9821.1451
	double posWeightEH[3] = {0};
	for ( int i = 0; i <3; i++ ) {
	posWeightEH[i] = posH[i] * energyH;
	EcenterH[i] += posWeightEH[i];
	}

// #### i want to separate this function out of this processor. ####
     int layerH = 0;
      for ( int i = 0; i<nlayerH; i++ ) { // start of "what layer did you hit?
       if ( (zposH[i]-5.) < (int)posH[2] && (int)posH[2] < (zposH[i]+5.)) {
	       
	layerH = i;  // +1;
	
	 //9825.1038
	 if ( (i == nlayerH - 1) && (energyH > EHlastLmax) 
	  //   && ( -110<posH[0]&&posH[0]<110 )
	  //   && ( -110<posH[1]&&posH[1]<110 ) 
	     ) {
	   EHlastLmax = energyH ;// deposited energy sum in last layer of HCAL.
	    if (( -110<posH[0]&&posH[0]<110 ) 
	     && ( -110<posH[1]&&posH[1]<110 ) ) {
	       EHlastLmaxEM = energyH ;// 
	    }	    
	 }	 
	}		
      }   // end of " what layer did you hit?"
// ###################################################################
	
#if 0 //i want to make this function instead of above lines.	
        int layerH = z_tolayerH( posH[2] );
#endif

	//9825.1141 i ignore outside of ECAL X Y plane.
	if ((-110<posH[0]&&posH[0]<110 )&&(-110<posH[1]&&posH[1]<110)) {
	  edep_zH[layerH] += energyH;
	  if ( energyH > 0.5 ) {
		  edep_zH05[layerH] += energyH;
	  }
	  EsumHinEM += energyH;
	}
	
	//9816.1415	
	layerH = layerH+1;
        _fNHitsH->Fill(n_Evt,energyH
			,posH[0],posH[1],posH[2],layerH,tile);
	
	  EsumH += energyH;
//9826.1917	} /// the end of ( -110 < x and y  <  +110 ) loop
	} // the end of HCAL hit scan
      } // the end of HCAL search
   }  //////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    ////     Collection name scan                 //////////////////
    ////////////////////////////////////////////////////////////////
#if b111124_1
      cout << "AFTER_CHECK_COLLECTION" << endl;
#endif

//9815.0959 come from inner "if loop"   
//9815.1740
        Ecenter[0] = Ecenter[0]/Esumx;
        Ecenter[1] = Ecenter[1]/Esumy;
        Ecenter[2] = Ecenter[2]/Esum;

//9821.1458
        for ( int i = 0; i < 3; i++ ) {
	EcenterH[i] = EcenterH[i]/EsumH;
	}
	
////////////////////////////////////////////////////////////////////////
	//9816.1657 select deposit energy on Hcal in z position.
	edep_zHmax = 0.;
	z_edepHmax = 0;
	for ( int i = 0; i<nlayerH; i++ ) {            //////
		if ( edep_zH[i] > edep_zHmax )  {      ////////
			edep_zHmax = edep_zH[i];
			z_edepHmax = i;
		}                              /////////////
	}                                      //////////
///////////////////////////////////////////////////////////////////
        z_edepHmax = z_edepHmax + 1;

/////////////////////////////////////////////////////////////////////	
	//9816.1657 select deposit energy on Hcal in z position.
	float edep_zH05max = 0.;
	int   z_edepH05max = 0;
	for ( int i = 0; i<nlayerH; i++ ) {  //////////////
		if ( edep_zH05[i] > edep_zH05max )  { ///////
			edep_zH05max = edep_zH05[i];
			z_edepH05max = i;
		}   ////////////
	}  //////////////////
////////////////////////////////////////////////////////////////////
        z_edepH05max = z_edepH05max + 1;

////////////////////////////////////////////////////////////////////
//        9902.2017 select deposit energy on ECAL in z position.
        edep_zmax = 0.;
	z_edepmax = 0;
	for ( int i = 0; i<nlayer; i++ ) {
		if ( edep_z[i] > edep_zmax ) {
		edep_zmax = edep_z[i];
		z_edepmax = i;
		}
	}
////////////////////////////////////////////////////////////////////
        z_edepmax = z_edepmax + 1;
	
////////////////////////////////////////////////////////////////////

	EcenterX = Ecenter[0];
	EcenterY = Ecenter[1];
	EcenterZ = Ecenter[2];
	EcenterHX = EcenterH[0];
	EcenterHY = EcenterH[1];
	EcenterHZ = EcenterH[2];

//9809.1530
        _fNEvents->Fill(); //n_Evt,Esum,EsumH,EsumHinEM
			  //,Ecenter[0],Ecenter[1],Ecenter[2]
			  //,edep_zHmax,z_edepHmax
//9902.2023			,edep_zH05max,z_edepH05max
			  //,edep_zmax,z_edepmax
			  //,EcenterH[0],EcenterH[1],EcenterH[2]
	//9825.2040	,EHlastLmax
			  //,EHlastLmaxEM);
	//
	//
	
	_Esumhist->Fill(Esum);
#if b111124_1
	      cout << "END_OF_EVT" << endl;
#endif

  }
};
