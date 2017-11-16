#include "Mixture.hh"
#include <iostream>
#include <cstring>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include "IO/LCReader.h"
#include "UTIL/LCTOOLS.h"
#include "marlin/Exceptions.h"
#include <marlin/Global.h>
#include <IMPL/TrackImpl.h>
#include <IMPL/LCFlagImpl.h>
#include <gear/GEAR.h>
#include <gear/TPCParameters.h>
#include <gearimpl/TPCParametersImpl.h>
#include <gearimpl/FixedPadSizeDiskLayout.h>
#include <gear/CalorimeterParameters.h>
#include <gear/LayerLayout.h>
#include <gear/BField.h>

#include "EVENT/CalorimeterHit.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCGenericObjectImpl.h"


using namespace lcio;
using namespace marlin;
using namespace std;

Mixture aMixture ;


//============================================================================
static int _ecal_last_layer(const gear::CalorimeterParameters& pCAL){
//============================================================================
  int i;
  const gear::LayerLayout &lb = pCAL.getLayerLayout() ;
  int nLayerb = lb.getNLayers() ;
  double tlb  = lb.getThickness(0);
  double dlb  = lb.getAbsorberThickness(0);
  for( i=1 ; i < nLayerb ; i++ )
    if(fabs(tlb-lb.getThickness(i))>0.0000001 ||
       fabs(dlb-lb.getAbsorberThickness(i))>0.0000001)
      return i;
  cerr<<"ERROR: Can't get boundary of CAL"<<endl;
  return i;
}

//=====================================
Mixture::Mixture() : Processor("Mixture") {
//=====================================
  _description = "Mixture creates overlapped events from several especially prepared CALICE files";

  StringVec files ;
  files.push_back("some_file.slcio");
  registerProcessorParameter( "InputFileNames", 
			      "Name of additional lcio input file(s)",
			      _inputFileNames,files);

  registerProcessorParameter( "InputFileNames", 
			      "Name of additional lcio input file(s)",
			      _inputFileNames,files);

//   Number of input pairs of shift coordinates should be equal to the number 
//   of given input additional files/particles
  std::vector<float>  XY_shifts;
  XY_shifts.push_back(0.0);
  XY_shifts.push_back(300.0);
  registerProcessorParameter( "Coordinate shift for additional " ,
                              "Coordinate shift for additional " ,
                              _XY_shifts,XY_shifts);

  registerOutputCollection(LCIO::CALORIMETERHIT,
      "SumOfECALHits", "Name of the hit collection for PANDORA",
      _ECALHitsCol,std::string("MixtureECAL"));

  registerOutputCollection(LCIO::CALORIMETERHIT,
      "SumOfHCALHits", "Name of the hit collection for PANDORA",
      _HCALHitsCol,std::string("MixtureHCAL"));

   registerOutputCollection(LCIO::LCGENERICOBJECT,
      "ParticleProperties", "Name of particle properties collection for PANDORA",
      _propCol,std::string("ParticleProperties"));

}
//=================================================================================
void Mixture::init() { 
//=================================================================================

//  Opening all additional files to make a mixture 
  _lcReaders.resize(_inputFileNames.size());
  _numMixture = _inputFileNames.size();
  for(int i=0; i < _numMixture; ++i) {
    _lcReaders[i] = LCFactory::getInstance()->createLCReader();
    _lcReaders[i]->open(_inputFileNames[i]); 
    if(_lcReaders[i]){
      std::cout<< "        File: "<< _inputFileNames[i]  
               <<" was open successfully"<<std::endl;
    } else {
      cout<< "             File: "<< _inputFileNames[i]
	  <<"  was not open"<<std::endl;
    }
  }
  _numShifts = _XY_shifts.size();
  if(_numMixture*2 - _numShifts){
    cout<<"   Number of Shifts is not equal a number of Mixture files \n"
	<<"   Program will be stopped"<<endl;
    throw StopProcessingException(this);
  }

  cout<<"        Asked  number of Files = "<<_numMixture
      <<";  Asked  number of Shifts = "<<_numShifts<<endl;

  for(int k = 0; k < _numMixture; k++){
    cout<<"        X shift = "<< _XY_shifts[unsigned(2*k)]
	<<"; Y shift = "<< _XY_shifts[unsigned(2*k+1)]<<endl;
  }
//...................................................................................
//    read gear 
//...................................................................................
  const gear::CalorimeterParameters &pECAL_B = Global::GEAR->getEcalBarrelParameters();
  const gear::LayerLayout &lb = pECAL_B.getLayerLayout() ;
  last_layer = _ecal_last_layer(Global::GEAR->getEcalBarrelParameters());
  er_inner   = lb.getDistance(0);
  er_outer   = lb.getDistance(last_layer);
  en_sampl   = last_layer;
  esampling_1  = lb.getThickness(0);
  esampling_2  = lb.getThickness(10);
  esampling_3  = lb.getThickness(20);
  emin_lay   = 0;
  emax_lay   = last_layer-1;
  eabsorber  = lb.getAbsorberThickness(0);
//  ecell_size = lb.getCellSize0(0); // should be 10 mm in GEAR

  const gear::CalorimeterParameters &pHCAL = Global::GEAR->getHcalBarrelParameters();
  const gear::LayerLayout &lhb = pHCAL.getLayerLayout() ;
  hr_inner   = pHCAL.getExtent()[0];
  hr_outer   = pHCAL.getExtent()[1];
  hz_inner   = pHCAL.getExtent()[2];
  hz_outer   = pHCAL.getExtent()[3];
  hn_sampl   = lhb.getNLayers() ;
  hsampling  = lhb.getThickness(0);
  hmin_lay   = 0;
  hmax_lay   = int(hn_sampl-1);
  habsorber  = lhb.getAbsorberThickness(0);
//  hcell_size = lhb.getCellSize0(0); // should be 30 mm

  hmip_vis    = 816.0e-6;
  ecal_coeff1 = 0.00376;
  ecal_coeff2 = 0.00752;
  ecal_coeff3 = 0.01128;
  hcal_coeff1 = 28.97*hmip_vis;

  cout<<"        ECAL sampling =  "<<esampling_1<<" ; "<<esampling_2<<" ; "<<esampling_3
      <<"; HCAL sampling =  "<<hsampling<< "HCAL MIP/GeV = "<<hcal_coeff1<<endl;

  cout<<"          End of of Mixture::Init"<<endl;

  _nRun = 0 ;
  _nEvt = 0 ;
}
//=======================================================================
void Mixture::processRunHeader( LCRunHeader* run) { _nRun++ ;} 
//=======================================================================
void Mixture::modifyEvent( LCEvent * evt ) {
//=======================================================================

  cout<<"\n*******************  Begin event # "<<_nEvt + 1 <<"  Main collection **********************"<<endl;
//    LCTOOLS::dumpEvent(evt);

//  event selection
  float showerRadius=0.;
  float showerEnergy=0.;
  int selPar[3];
  EVENT::LCCollection* firstSelectColl = 0;
  firstSelectColl = evt->takeCollection("ADDS");
  if(firstSelectColl){

   std::cout << "first ADDS col. has parameters ";

  LCGenericObjectImpl* pSelPar = dynamic_cast<LCGenericObjectImpl*>(firstSelectColl->getElementAt(0));
  for(int i=0; i<2;++i){  
  selPar[i] = pSelPar -> getIntVal(i);
  cout << selPar[i]<<" ";
    }
  showerEnergy = pSelPar -> getFloatVal(3);
  showerRadius = pSelPar -> getFloatVal(4);
   cout <<"shower energy ="<<showerEnergy<<" shower radius =" << showerRadius<<std::endl;
  
  }
  if(showerRadius==0){
  
  const std::vector<std::string>* colNames = evt->getCollectionNames();

  for( StringVec::const_iterator name = colNames->begin();
  name != colNames->end() ; name++ ){

  LCCollectionVec* col = dynamic_cast<LCCollectionVec*>( evt->getCollection( *name ));

   if ( !std::strncmp((*name).data(),"ECAL", 4)
   || !std::strncmp((*name).data(),"HCAL", 4)
   || !std::strncmp((*name).data(),"TCAL", 4)
   || !std::strncmp((*name).data(),"PTRK", 4)
   || !std::strncmp((*name).data(),"ADDS", 4) ) {
   col->setTransient( true );
//cout << "Collection: " << (*name).c_str() << " dropped" << endl;
      }
    } 
  return;
  }
  //  unsigned l = 0;
  float x   = 0.0;
  float y   = 0.0;
  float z   = 0.0;
  float e   = 0.0;

  int nHitsE = 0;
  int nHitsH = 0;
  //  int nHitsT = 0;
  int ix;
  int jy;
  int kz;
  double stp_ecal = 10.0;
  double shift_int_ecal = 1000.0;
  double stp_hcal = 30.0;
  double shift_int_hcal = 1000.0;
  float ecoef;
  float sumFullEnergy = 0.;
  float fullEnergy ;
  float ECALEnergy = 0.;
  float HCALEnergy = 0.;
//  Initialyze amplitudes
  for(ix=0;ix<200;ix++)
    for(jy=0;jy<200;jy++)
      for(kz=0;kz<100;kz++)
	ampl[ix][jy][kz] = 0.0;

  EVENT::LCCollection* ecal_coll = 0;
  EVENT::LCCollection* hcal_coll = 0;
  EVENT::LCCollection* tcal_coll = 0;
//  EVENT::LCCollection* ptrk_coll = 0;
  ecal_coll = evt->takeCollection("ECAL");
  hcal_coll = evt->takeCollection("HCAL");
  tcal_coll = evt->takeCollection("TCAL");
//  ptrk_coll = evt->takeCollection("PTRK");

 LCCollectionVec *properties = new LCCollectionVec(LCIO::LCGENERICOBJECT);
 LCGenericObjectImpl* elProperties = new LCGenericObjectImpl;


  CalorimeterHit* hit; // pointer of class CalorimeterHit
  if(ecal_coll){ // Read initial particle ECAL collection into Mixture
  cout <<"the first ";
//  cout<<"=================  ECAL  ====================================="<<endl;
    nHitsE =  ecal_coll->getNumberOfElements();// Get number of hits in ECAL
    for( int i = 0 ; i < nHitsE ; i++ ){      // then fill it
      hit = dynamic_cast<CalorimeterHit*>(ecal_coll->getElementAt(i));
      e = (hit->getEnergy());     // [MIPs]
   if(e<0.5)continue;
      x = (hit->getPosition()[0]);
      y = (hit->getPosition()[1]);
      z = (hit->getPosition()[2]);

      kz = (hit->getType());
  ecoef = ecal_coeff1;
	if(kz >  9)  ecoef = ecal_coeff2;
	if(kz > 19)  ecoef = ecal_coeff3;
      ix = int((x+shift_int_ecal)/stp_ecal) + 1;
      jy = int((y+shift_int_ecal)/stp_ecal) + 1;
      if(ix >= 0 && jy >= 0  && ix < 200 && jy < 200){
//   cout<<"first part. ECAL Hit has "<<e<< " MIP and pos. "<<x<<"  "<<y<<"  "<<z<<"  :  "<<ix<<" "<<jy<<"  "<<kz<<endl;
	ampl[ix][jy][kz] +=  e;
  if(kz<30){ECALEnergy += e*ecoef;

        }
      }
    }
  cout <<" ECALEnergy ="<< ECALEnergy <<" GeV";
  }

  //  cout<<"=================  HCAL  ====================================="<<endl;

  if(hcal_coll){ // Read initial particle HCAL collection into Mixture
    nHitsH =  hcal_coll->getNumberOfElements();// Get number of hits in ECAL

  ecoef = hcal_coeff1; // all coeffs here are the same
    for( int i = 0 ; i < nHitsH ; i++ ){      // then fill it
      hit = dynamic_cast<CalorimeterHit*>(hcal_coll->getElementAt(i));
      e = (hit->getEnergy());     // [MIPs]
  if(e<0.5)continue;
      x = (hit->getPosition()[0]);
      y = (hit->getPosition()[1]);
      z = (hit->getPosition()[2]);
      kz = (hit->getType());
      ix = int((x+shift_int_hcal)/stp_hcal) + 1;
      jy = int((y+shift_int_hcal)/stp_hcal) + 1;
      if(ix >= 0 && jy >= 0 && ix < 200 && jy < 200){
  //cout<<" HCAL Hit "<<x<<"  "<<y<<"  "<<z<<"  :  "<<ix<<" "<<jy<<"  "<<kz<<endl;
	ampl[ix][jy][kz] +=  e;
  HCALEnergy += e*ecoef;
      }
    }

  fullEnergy = ECALEnergy + HCALEnergy;
  sumFullEnergy=fullEnergy;
  cout <<" HCALEnergy ="<< HCALEnergy <<" GeV"<<" fullEnergy ="<< fullEnergy<<" GeV"<< endl;

  elProperties -> setIntVal(0,selPar[0]);
  elProperties -> setIntVal(1,selPar[1]);
  elProperties -> setFloatVal(2,showerEnergy);
  elProperties -> setFloatVal(3,showerRadius);
  elProperties -> setFloatVal(4,fullEnergy);
  properties -> addElement(elProperties);

  }
  LCEvent * mevt = 0;

//222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

//  Read all additional files with particles to join in one event

//222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

  for(int imix=0; imix < _numMixture; ++imix){
    mevt = _lcReaders[imix]->readNextEvent(LCIO::UPDATE);
    float xsh = _XY_shifts[unsigned(2*imix)];
    float ysh = _XY_shifts[unsigned(2*imix+1)];
    if(mevt){
      //      cout<<"=============================================================="<<endl;
      //      cout<<" Collection # "<<imix<<" of additional at event"<<_nEvt<<endl;
      //      cout<<"=============================================================="<<endl;
//             LCTOOLS::dumpEvent(mevt);

//  event selection

    EVENT::LCCollection* addSelectColl = 0;
  addSelectColl = mevt->takeCollection("ADDS");
  if(addSelectColl){

  std::cout << " add. ADDS col. has parameters ";

  LCGenericObjectImpl* pSelPar = dynamic_cast<LCGenericObjectImpl*>(addSelectColl->getElementAt(0));
  for(int i=0; i<2;++i){  
  selPar[i] = pSelPar -> getIntVal(i);
  cout  << selPar[i]<<" ";
    }

  showerEnergy = pSelPar -> getFloatVal(3);
  showerRadius = pSelPar -> getFloatVal(4);
  cout <<"shower energy ="<< showerEnergy<<" shower radius =" << showerRadius<<std::endl;

  }
  if(showerRadius==0){
  
  const std::vector<std::string>* colNames = evt->getCollectionNames();

  for( StringVec::const_iterator name = colNames->begin();
  name != colNames->end() ; name++ ){

  LCCollectionVec* col = dynamic_cast<LCCollectionVec*> (evt->getCollection( *name ) );

   if ( !std::strncmp((*name).data(),"ECAL", 4)
   || !std::strncmp((*name).data(),"HCAL", 4)
   || !std::strncmp((*name).data(),"TCAL", 4)
   || !std::strncmp((*name).data(),"PTRK", 4)
   || !std::strncmp((*name).data(),"ADDS", 4) ) {
   col->setTransient( true );
//cout << "Collection: " << (*name).c_str() << " dropped" << endl;
      }
    } 
  return;
  }

      ecal_coll = mevt->takeCollection("ECAL");
      hcal_coll = mevt->takeCollection("HCAL");
      tcal_coll = mevt->takeCollection("TCAL");
      //      ptrk_coll = mevt->takeCollection("PTRK");
      ECALEnergy = 0.;
      HCALEnergy = 0.;
  LCGenericObjectImpl* elProperties = new LCGenericObjectImpl;

      if(ecal_coll){ // Read secondary particle ECAL collection into Mixture
  cout <<"additional";
//cout<<"=================  ECAL  ====================================="<<endl;
	nHitsE =  ecal_coll->getNumberOfElements();// Get number of hits in ECAL
	for( int i = 0 ; i < nHitsE ; i++ ){      // then fill it
	  hit = dynamic_cast<CalorimeterHit*>(ecal_coll->getElementAt(i));
          kz = (hit->getType());
  if(kz<(selPar[0]*30+selPar[1]))continue;
          e = (hit->getEnergy());     // [MIPs]
  if (e<0.5)continue;
	  x = (hit->getPosition()[0]) + xsh;
	  y = (hit->getPosition()[1]) + ysh;
	  z = (hit->getPosition()[2]);
  ecoef = ecal_coeff1;
	if(kz >  9)  ecoef = ecal_coeff2;
	if(kz > 19)  ecoef = ecal_coeff3;
	  ix = int((x+shift_int_ecal)/stp_ecal) + 1;
	  jy = int((y+shift_int_ecal)/stp_ecal) + 1;

	  if(ix >= 0 && jy >= 0 && ix < 200 && jy < 200){
//   cout<<"second part. ECAL Hit has  "<<e<< " MIP and pos. "<<x<<"  "<<y<<"  "<<z<<"  :  "<<ix<<" "<<jy<<"  "<<kz<<endl;
	    ampl[ix][jy][kz] +=  e;
  if(kz<30)ECALEnergy += e*ecoef;
	 }
	
      }
	cout <<" ECALEnergy ="<<ECALEnergy<<" Gev";
    } 
//cout<<"=================  HCAL  ====================================="<<endl;
      if(hcal_coll){// Read secondary particle HCAL collection into Mixture
	nHitsH =  hcal_coll->getNumberOfElements();// Get number of hits in HCAL

  ecoef = hcal_coeff1; // all coeffs here are the same
	for( int i = 0 ; i < nHitsH ; i++ ){      // then fill it
	  hit = dynamic_cast<CalorimeterHit*>(hcal_coll->getElementAt(i));
  kz = (hit->getType());
  if(kz<(selPar[0]*30+selPar[1]))continue;
          e = (hit->getEnergy());     // [MIPs]
  if(e<0.5)continue;
	  x = (hit->getPosition()[0]) + xsh;
	  y = (hit->getPosition()[1]) + ysh;
	  z = (hit->getPosition()[2]);

	  ix = int((x+shift_int_hcal)/stp_hcal) + 1;
	  jy = int((y+shift_int_hcal)/stp_hcal) + 1;

	  if(ix >= 0 && jy >= 0 && ix < 200 && jy < 200){
	    //cout<<" HCAL Hit "<<x<<"  "<<y<<"  "<<z<<"  :  "<<ix<<" "<<jy<<"  "<<kz<<endl;
	    ampl[ix][jy][kz] +=  e;
  HCALEnergy += e*ecoef;
	  }
	}
  fullEnergy = ECALEnergy + HCALEnergy;
  sumFullEnergy+=fullEnergy;
  cout <<" HCALEnergy ="<< HCALEnergy <<" GeV"<<" fullEnergy ="<< fullEnergy<<" GeV"<< endl;


  elProperties -> setIntVal(0,selPar[0]);
  elProperties -> setIntVal(1,selPar[1]);
  elProperties -> setFloatVal(2,showerEnergy);
  elProperties -> setFloatVal(3,showerRadius);
  elProperties -> setFloatVal(4,fullEnergy);
  properties -> addElement(elProperties);

      }
/*
//       Make copy of all collections from additional files
      char coll_name[128];
      sprintf(coll_name,"ECAL%1d",imix);
      evt->addCollection(ecal_coll,coll_name);
      sprintf(coll_name,"HCAL%1d",imix);
      evt->addCollection(hcal_coll,coll_name);
      sprintf(coll_name,"TCAL%1d",imix);
      evt->addCollection(tcal_coll,coll_name);
      //      sprintf(coll_name,"PTRK%1d",imix);
      //      evt->addCollection(ptrk_coll,coll_name);
*/
    } else {
      cout<<"+++++++++++   LCIO Reader did nor find next event in file "
	  <<_inputFileNames[imix]<<" ++++++++++++++++++++++++++++\n"
	  <<" +++++++++++++++++  Run should be stopped  ++++++++++++++++++++\n"
	  <<" +++++++++++  StopProcessingException is called  ++++++++++++++"
	  <<endl;
// ???  How to say to MARLIN to stop all events with correct output
//        and call for all processors end of run ????
      throw StopProcessingException(this);
    }
  }
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    cout<<"======  Create    Collections ================================"<<endl;
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  cout <<" mixed     ";
//-----------------------------------------------------
//  At this stage we have array of summed amplitudes
//  Let us create new set of calorimeter hits
//-----------------------------------------------------

  evt -> addCollection(properties,_propCol);

  LCCollectionVec *ecalohitVec = new LCCollectionVec(LCIO::CALORIMETERHIT);
  LCCollectionVec *hcalohitVec = new LCCollectionVec(LCIO::CALORIMETERHIT);

  ECALEnergy = 0.;
  HCALEnergy = 0.;
//   fullEnergy = 0.;
  float pos[3];
  for(kz=0;kz<30;kz++){ // ECAL only
    for(ix=0;ix<200;ix++){
      for(jy=0;jy<200;jy++){
	if(ampl[ix][jy][kz] < 0.5)  
	  continue;
	ecoef = ecal_coeff1;
	if(kz >  9)  ecoef = ecal_coeff2;
	if(kz > 19)  ecoef = ecal_coeff3;
	CalorimeterHitImpl * hit = new CalorimeterHitImpl();
	float xx = (ix-0.5)*stp_ecal - shift_int_ecal;
	float yy = (jy-0.5)*stp_ecal - shift_int_ecal;
	pos[0] = -xx;
	pos[1] = er_inner + kz*esampling_1+0.5*esampling_1;  // correct step + shift to radius
       	if(kz > 9)pos[1] = er_inner+10*esampling_1+(kz-10)*esampling_2+ 0.5*esampling_2;
       	if(kz > 19)pos[1] = er_inner+10*esampling_1+10*esampling_2 + (kz-20)*esampling_3 + 0.5*esampling_3;
	pos[2] = yy;

// 	cout<< " new X = "<<pos[0]<<" new Y = "<<pos[1]<<" new Z = "<<pos[2]<<" layer number = "<<kz<<endl;

  ECALEnergy += ecoef*ampl[ix][jy][kz];

	
	hit->setPosition(pos);
	hit->setEnergy(ecoef*float(ampl[ix][jy][kz]));
	ecalohitVec ->addElement(hit);
      }
    }
  }

  cout <<" ECALEnergy ="<<ECALEnergy<<" Gev";

//------------------------------------- HCAL -------------------------------------------

  ecoef = hcal_coeff1; // all coeffs here are the same
  for(kz=30;kz<68;kz++){ // HCAL only
    for(ix=0;ix<200;ix++){
      for(jy=0;jy<200;jy++){
	if(ampl[ix][jy][kz] < 0.5)  
	  continue;
	CalorimeterHitImpl * hit = new CalorimeterHitImpl();
	float xx = (ix-0.5)*stp_hcal - shift_int_hcal;
	float yy = (jy-0.5)*stp_hcal - shift_int_hcal;
	float zz = (kz-30)*hsampling;
	pos[0] = -xx;
	pos[1] = zz + hr_inner + 0.5*hsampling;  // correct step + shift to radius
	pos[2] = yy;
  HCALEnergy += ampl[ix][jy][kz]*ecoef;
	hit->setPosition(pos);
	hit->setEnergy(ecoef*ampl[ix][jy][kz]);
	hcalohitVec ->addElement(hit);       

      }
    }
  }

  fullEnergy = ECALEnergy + HCALEnergy;
  cout <<" HCALEnergy ="<< HCALEnergy <<" GeV"<<" fullEnergy ="<< fullEnergy<<"("<<sumFullEnergy<<") GeV"<< endl;

// If we want to point back to the hits we need to set the flag
    LCFlagImpl cFlag(0) ;
    cFlag.setBit(LCIO::CHBIT_LONG);
    ecalohitVec->setFlag( cFlag.getFlag());
    hcalohitVec->setFlag( cFlag.getFlag());


  evt->addCollection(ecalohitVec,_ECALHitsCol);
  evt->addCollection(hcalohitVec,_HCALHitsCol);

  _nEvt++;

  }
//===============================================================================================
void Mixture::check( LCEvent *evt) {;}
//===============================================================================================
void Mixture::end(){;}
//===============================================================================================
