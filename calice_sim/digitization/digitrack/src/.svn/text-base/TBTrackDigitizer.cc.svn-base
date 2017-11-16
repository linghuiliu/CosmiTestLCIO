#include <vector>
#include <iostream>
#include <iomanip>
#include <cassert>

#include "TRandom.h"

#include "EVENT/LCEvent.h"
#include "EVENT/LCIntVec.h"
#include "EVENT/SimTrackerHit.h"

#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimTrackerHitImpl.h"

#include "marlin/Exceptions.h"

#include "TBTrackDigitizer.hh"

using namespace lcio;

//namespace CALICE {

TBTrackDigitizer a_TBTrackDigitizer_instance;

// The digitization of MC Drift Chamber Hits

TBTrackDigitizer::TBTrackDigitizer() :
  TBTrackBaseProcessor("TBTrackDigitizer") {


  registerProcessorParameter( "SeparateHitsXY",
			      "Set to != 0 if simulation used different chambers for x and y tracking",
			      _separateHitsXY, int( 1 ) );
  /*
    _dc1ColName="TBdch02_dchSD1";
    //_dc1ColName="TBdchX02_dchSDx1";
    registerProcessorParameter( "DC1_MC_CollectionName" , 
				"The name of the DC1 MC input collection  (drift chambers)." ,
				_dc1ColName ,
				_dc1ColName);

    _dc2ColName="TBdch02_dchSD2";
    //_dc2ColName="TBdchX02_dchSDx2";
    registerProcessorParameter( "DC2_MC_CollectionName" , 
				"The name of the DC2 MC input collection  (drift chambers)." ,
				_dc2ColName ,
				_dc2ColName);

    _dc3ColName="TBdch02_dchSD3";
    //_dc3ColName="TBdchX02_dchSDx3";
    registerProcessorParameter( "DC3_MC_CollectionName" , 
				"The name of the DC3 MC input collection  (drift chambers)." ,
				_dc3ColName ,
				_dc3ColName);

    _dc4ColName="TBdch02_dchSD4";
    //_dc4ColName="TBdchX02_dchSDx4";
    registerProcessorParameter( "DC4_MC_CollectionName" , 
				"The name of the DC4 MC input collection  (drift chambers)." ,
				_dc4ColName ,
				_dc4ColName);

    //_trackerHitColName=COL_DCHITS;
    _trackerHitColName="TBTrackTdcHits";
    registerProcessorParameter( "TBTrackHitCollectionName" , 
				"The name of the output collection which contains smeared tracker hits." ,
				_trackerHitColName ,
				_trackerHitColName);
  */
    /*
    registerProcessorParameter( "TBTrackTRUEHitCollectionName" , 
				"The name of the output collection which contains TRUE tracker hits." ,
				_TRUETrackerHitColName ,
				_TRUETrackerHitColName);

    _cut_de_dx=1e-06;
    registerProcessorParameter( "Cut_De_Dx" , 
				"Cut_De_Dx for the DC signal." ,
				_cut_de_dx ,
				_cut_de_dx);

    registerProcessorParameter( "Database" , 
				"Set to non-zero to use local (not event) constants",
				_dataBase,0);
    
    registerProcessorParameter( "TBTrackParameterCollectionName" , 
    				"Name of the drift chamber parameter conditions data. (folder /TsukubaTBTrack/parameters)"  ,
				_driftChamberParColName ,
    				std::string("drift_chamber_parameters") ) ;

    _sigma=50;
    registerProcessorParameter( "Sigma" , 
				"Sigma from the DC resolution." ,
				_sigma ,
				_sigma);

    _drift_velocity=0.03;
    registerProcessorParameter( "DriftVelocity" , 
				"Drift velocity for the DC gas mixture." ,
				_drift_velocity ,
				_drift_velocity);

     registerProcessorParameter( "Debug",
				"Debug, if true all output are shown." ,
				_debug,
				0);
    */
  }

void TBTrackDigitizer::ProcessEvent( LCEvent * evt ) {
  
  /* Skip everything if this is not simulation
  */
  if(!_runInformation.isMC()) return;

  /* Check there are some constants which are valid.
     The simulation constants must be there and check
     the real data mapping constants are not present also.
  */
  if(!_simConstantsValid || _mapConstantsValid) {
    if(!_simConstantsValid && printLevel(-3)) std::cout
      << "No valid SimConstants" << std::endl;
    if(_mapConstantsValid && printLevel(-3)) std::cout
      << "Valid MapConstants" << std::endl;
    assert(false);
  }

  /* Check names of collections
   */  
  if(printLevel(3)) {
    std::cout << "Collections in the event" << std::endl;

    typedef const std::vector<std::string> StringVec ;
    StringVec* strVec = evt->getCollectionNames() ;
    
    for( StringVec::const_iterator name = strVec->begin() ; name != strVec->end() ; name++) {
      std::string sss = name->c_str();
      LCCollection* col = evt->getCollection(*name);
      int nHits =  col->getNumberOfElements() ;
      
      std::cout << "----> Evt " <<evt->getEventNumber()<< ", EXISTING COLLECTION, of the class : "
		<< col->getTypeName().c_str() << " and named : " << sss << " with " << nHits << " elements." << std::endl;
    }
  }
  
  /* Seed the random number generator
     This needs to be repeatable for an
     event to allow debugging. Use the run
     and event number here, but add one so as
     to not get zero on the first event in
     the run, which seeds to the machine
     clock
  */
  //gRandom->SetSeed(evt->getEventNumber()+1);
  gRandom->SetSeed(1000000*(_iRun%1000)+evt->getEventNumber()+1);
  
  if(printLevel(2)) std::cout << "gRandom seed initialised to "
			      << gRandom->GetSeed() << std::endl;

  /* Because of the differences between the 
     various MC versions, unpack to a common
     format.
  */
  if (_obsoleteMokkaCollections) getSimTrackerHits(evt);

  const LCCollection *c(0);
  if((c=getCollection(evt,_simTrackerHitCollection,LCIO::SIMTRACKERHIT))!=0) {
    
    /* Form the output collection and always put it into the
       event to flag this code ran
    */
    LCCollectionVec* outdcCol(new LCCollectionVec(LCIO::LCINTVEC));
    evt->addCollection(outdcCol,_tdcHitCollection);
    
    LCIntVec *elemdc[2][4];
    for(unsigned layer(0);layer<4;layer++) {
      for(unsigned xy(0);xy<2;xy++) {
	elemdc[xy][layer]=new LCIntVec;
	elemdc[xy][layer]->push_back(2*layer+xy);
      }
    }
    
    double energy[2][4]={{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
    double coord[2][4]={{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
    double time[2][4]={{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
    
    for(int i(0);i<c->getNumberOfElements();i++) {
      const SimTrackerHit *p(dynamic_cast<const SimTrackerHitImpl*>(c->getElementAt(i)));
      
      int cellID(p->getCellID0());
      double en=p->getEDep()*p->getPathLength();
      if ( _separateHitsXY ) {
	assert(cellID<8);
	unsigned xy(cellID&1);
	unsigned layer(cellID>>1);
	
	energy[xy][layer]+=en;
	coord[xy][layer]+=en*p->getPosition()[xy];
	time[xy][layer]+=en*p->getTime();

	if(printLevel(3,false)) {
	  std::cout << " SimTrackerHit " << layer 
		    << std::setw(4) << xy << " found " << std::endl;
	}
      } else {
	assert(cellID<4);
	unsigned layer( cellID );
	for ( int xy( 0 ); xy != 2; ++xy ) {
	  energy[xy][layer]+=en;
	  coord[xy][layer]+=en*p->getPosition()[xy];
	  time[xy][layer]+=en*p->getTime();
	}
	if(printLevel(3,false)) {
	  std::cout << " SimTrackerHit " << layer 
		    << " found and used for both dimensions" << std::endl;
	}
      }
      
      if(printLevel(3,false)) {
	std::cout << std::setw(2)           << p->getCellID0()
		  << std::setw(10)          << p->getPosition()[0]
		  << std::setw(10)          << p->getPosition()[1]
		  << std::setw(10)          << p->getPosition()[2]
		  << std::setw(13)          << p->getEDep()
		  << std::setw(10)          << p->getTime()
	          << std::setw(10) << "0x" << std::hex
		  << p->getMCParticle() << std::dec
		  << std::setw(10)          << p->getMomentum()[0]
		  << std::setw(10)          << p->getMomentum()[1]
		  << std::setw(10)          << p->getMomentum()[2]
		  << std::setw(10)          << p->getPathLength() << std::endl;
      }
    }
    
    
    for(unsigned layer(0);layer<4;layer++) {
      for(unsigned xy(0);xy<2;xy++) {
	
	// time[xy][layer]*=12000.0; // SimTrackerHit units are 0.12us!!!
	
	if(energy[xy][layer]>0.0) {
	  if(gRandom->Uniform()<=_simConstants.cEffic(xy,layer)) {
	    double posx=coord[xy][layer]/energy[xy][layer];
	    
	    if (printLevel(3)) std::cout << "Original posn "
					 << posx << std::endl; //debug
	    
	    double newposx=gRandom->Gaus(posx,_simConstants.cSmear(xy,layer));
	    
	    if (printLevel(3)) std::cout << "Smeared  posn "
					 << newposx << std::endl; //debug
	    
	    int newtimex=_simConstants.tdcValue(xy,layer,newposx,/*(time[xy][layer]/energy[xy][layer])*/ -_simConstants.tTzero(xy,layer));
	    
	    if (printLevel(3)) std::cout << "Hit time "
					 << newtimex << std::endl; //debug
	    
	    elemdc[xy][layer]->push_back(newtimex);
	  }
	}
	

	
	double width(100.0); //??????
	
	// Add noise
	unsigned nNoise(gRandom->Poisson(width*_simConstants.cNoise(xy,layer)));
	for(unsigned i(0);i<nNoise;i++) {
	  double pos(width*(gRandom->Uniform()-0.5));
	  if (printLevel(3)) std::cout << "Noise hit posn " << pos << std::endl;
	  int nTime(_simConstants.tdcValue(xy,layer,pos));
	  if (printLevel(3)) std::cout << "Noise hit time " << nTime << std::endl;
	  
	  elemdc[xy][layer]->push_back(nTime);
	}
	
	// Finally put it into the output collection
	if(elemdc[xy][layer]->size()>1) outdcCol->addElement(elemdc[xy][layer]);
      }
    }
  }







    /*
    LCCollection* dcCol[4];
  
    LCCollectionVec* outdcCol = new LCCollectionVec( LCIO::LCINTVEC );
    evt->addCollection(outdcCol, _trackerHitColName.c_str() );
  
    //DESY
    //if( _model.compare("TBDesy0506")==0 || _model.compare("TBDesy0506_01")==0){  
      try{
	dcCol[0] = evt->getCollection( _dc2ColName ); //DC1 and DC2 are swapped
	dcCol[1] = evt->getCollection( _dc1ColName ); //DC1 and DC2 are swapped
	dcCol[2] = evt->getCollection( _dc3ColName );
	dcCol[3] = evt->getCollection( _dc4ColName );
	DC_number = 4;
      }
      catch ( DataNotAvailableException &e){
	std::cout << "Problem in TDC Collection, incorrect name = " << _dc2ColName << " ?" << std::endl;
	_no_all_hit ++;
	
	//throw SkipEventException(this);
	//      }
	//    }
	//    else{
      try{
	dcCol[2] = evt->getCollection( _dc1ColName );
	dcCol[1] = evt->getCollection( _dc2ColName );
	dcCol[0] = evt->getCollection( _dc3ColName );
	DC_number = 3;
      }
      catch ( DataNotAvailableException &e){
	std::cout << "Problem in TDC Collection, incorrect name = " << _dc1ColName << " ?" << std::endl;
	_no_all_hit ++;
	
	//throw SkipEventException(this);
      }
      }
    
    LCIntVec *elemdc[2*DC_number] ;//= new LCIntVec;
    
    for(unsigned j(0);j<static_cast<unsigned>(2*DC_number);j++) 
      {  
	elemdc[j] = new LCIntVec;
      }
    
    for(int ndc = 0; ndc < DC_number; ndc++){
      
      if (printLevel(3)) std::cout<<"-----------DC"<<ndc+1<<"-----------"<<std::endl;
      int n_sim_hits = dcCol[ndc]->getNumberOfElements();
      
      double *pos;
      totx = 0;
      toty = 0;
      ngoodhit =0;
      
      SimTrackerHit* SimDCHit = dynamic_cast<SimTrackerHit*>( dcCol[ndc]->getElementAt( 0 ) ) ;
      
      pos = (double*) SimDCHit->getPosition(); 
      
      if(gRandom->Uniform()<_simConstants.cEffic(0,ndc)) {
	posx = pos[0];
	double newposx=gRandom->Gaus(posx,_simConstants.cError(0,ndc));
	newtimex=_simConstants.tdcValue(0,ndc,newposx);
	
	if (printLevel(3)) std::cout << "New Time x "
			      << newtimex << std::endl; //debug
	
	elemdc[2*ndc]->push_back(0);
	elemdc[2*ndc]->push_back(ndc);
	elemdc[2*ndc]->push_back(newtimex);
	
	outdcCol->addElement(elemdc[2*ndc]);
      }
      
      if(gRandom->Uniform()<_simConstants.cEffic(1,ndc)) {
	posy = pos[1];
	double newposy=gRandom->Gaus(posy,_simConstants.cError(1,ndc));
	newtimey=_simConstants.tdcValue(1,ndc,newposy);
	
	if (printLevel(3)) std::cout << "New Time y " << newtimey << std::endl; //debug
	
	elemdc[2*ndc+1]->push_back(1);
	elemdc[2*ndc+1]->push_back(ndc);
	elemdc[2*ndc+1]->push_back(newtimey);
	
	outdcCol->addElement(elemdc[2*ndc+1]);
      }
      
    }
    */
}

//}
