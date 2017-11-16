#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/LCIntVec.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/LCFlagImpl.h"

#include "TBTrackMokkaCheck.hh"


using namespace lcio;
using namespace marlin;
using namespace TBTrack;


TBTrackMokkaCheck aTBTrackMokkaCheck;


TBTrackMokkaCheck::TBTrackMokkaCheck() :
  TBTrackBaseProcessor("TBTrackMokkaCheck") {
  _description = "TBTrackMokkaCheck";
}

void TBTrackMokkaCheck::initHists() {

  std::string sxy[2]={"X","Y"};
  std::string sly[4]={"Layer0","Layer1","Layer2","Layer3"};
  
  hNCol=new TH1F("NCol","Number of hit collections",9,0.0,9.0);
  hNEnt=new TH1F("NEnt","Number of events with hit object 4*XY+Layer",8,0.0,8.0);

  for(unsigned xy(0);xy<2;xy++) {
    for(unsigned layer(0);layer<4;layer++) {
      hNumb[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Numb").c_str(),
				(sxy[xy]+" "+sly[layer]+": Number of hits").c_str(),
				100,0.0,100.0);
      hCell[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Cell").c_str(),
				(sxy[xy]+" "+sly[layer]+": Hit distribution").c_str(),
				8,0.0,8.0);
      hPosn[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Posn").c_str(),
				(sxy[xy]+" "+sly[layer]+": Hit position (mm)").c_str(),
				100,-50.0,50.0);
      hPosz[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Posz").c_str(),
				(sxy[xy]+" "+sly[layer]+": Hit z position (mm)").c_str(),
				1000,-10000.0,0.0);
      hEner[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Ener").c_str(),
				(sxy[xy]+" "+sly[layer]+": Hit energy (MeV)").c_str(),
				100,0.0,0.0005);
      hTime[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Time").c_str(),
				(sxy[xy]+" "+sly[layer]+": Hit time (ns)").c_str(),
				1500,0.0,150.0/10000.0);
      hTana[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Tana").c_str(),
				(sxy[xy]+" "+sly[layer]+": Hit tan(angle)").c_str(),
				100,-0.01,0.01);
    }
  }
}

void TBTrackMokkaCheck::Init() {
}

void TBTrackMokkaCheck::ProcessRunHeader( LCRunHeader* run) {
  closeHFile();
  openHFile(run);
  initHists();
}

void TBTrackMokkaCheck::ProcessEvent( LCEvent * evt ) {

  //the following access the names of the collection.
  // having the name, we can access the collection itself by calling 
  // LCCollection *mycol = evt->getCollection(*name);

  typedef const std::vector<std::string> StringVec;
  StringVec* strVec = evt->getCollectionNames();
  for( StringVec::const_iterator name = strVec->begin(); name != strVec->end(); name++) {
      
    std::string sss = name->c_str();
    LCCollection* col = evt->getCollection(*name);
    int nHits =  col->getNumberOfElements();
    
    if (printLevel(3)) std::cout << "EXISTING COLLECTION, of the class : "
				 << col->getTypeName().c_str()
				 << " and named : " << sss << " with "
				 << nHits << " elements." << std::endl;
  }
  



  const LCCollection *c(0);

  if((c=getCollection(evt,"MCParticle",LCIO::MCPARTICLE))!=0) {
    if(c->getNumberOfElements()<1) {
      if(printLevel(-2)) std::cout << "Number of MCParticles = "
				   << c->getNumberOfElements() << std::endl;
    } else {
      if(printLevel(2)) std::cout << "Number of MCParticles = "
				<< c->getNumberOfElements() << std::endl;
      for(int i(0);i<c->getNumberOfElements();i++) {
	if(printLevel(3)) std::cout << "MCParticles " << i << std::endl;
      }
    }
  }
  


    unsigned nCol(0);
    LCCollection *col;				
  for(int l(0);l<4;l++) {
    col=0;
    try{
      if(l==0) col = evt->getCollection("TBdch02_dchSD2"); //DC1 and DC2 are swapped
      if(l==1) col = evt->getCollection("TBdch02_dchSD1"); //DC1 and DC2 are swapped
      if(l==2) col = evt->getCollection("TBdch02_dchSD3");
      if(l==3) col = evt->getCollection("TBdch02_dchSD4");
      
    } catch ( DataNotAvailableException &e) {
      
      try{
	if(l==0) col = evt->getCollection("TBdch04_dchSD3");
	if(l==1) col = evt->getCollection("TBdch04_dchSD2");
	if(l==2) col = evt->getCollection("TBdch04_dchSD1");
	
      } catch ( DataNotAvailableException &e) {
	std::cout << "Problem in TDC Collection, incorrect name?" << std::endl;
      }
    }
    
    double energy(0.0);
    double xe(0.0),ye(0.0);
    
    if(col!=0) {
      assert(col->getTypeName()==LCIO::SIMTRACKERHIT);
      
      nCol++;
      hNEnt->Fill(2*l);

      //    std::cout << "Number of elements = "
      //      << col->getNumberOfElements() << std::endl;
      
      for(int i(0);i<col->getNumberOfElements();i++) {
	const SimTrackerHit *p(dynamic_cast<const SimTrackerHit*>(col->getElementAt(i)));
	assert(p!=0);
	//assert(p->getCellID()==l);

	hCell[0][l]->Fill(p->getCellID());
	hPosn[0][l]->Fill(p->getPosition()[0]);
	hPosz[0][l]->Fill(p->getPosition()[2]);
	hEner[0][l]->Fill(p->getdEdx()*p->getPathLength());
	//hTime[0][l]->Fill(10000.0*p->getTime());
	hTime[0][l]->Fill(p->getTime());
	if(p->getMomentum()[2]!=0.0)
	  hTana[0][l]->Fill(p->getMomentum()[0]/p->getMomentum()[2]);

	/*
	std::cout << l << ", "
		  << p->getCellID() << ", "
		  << p->getPosition()[0] << ", "
		  << p->getPosition()[1] << ", "
		  << p->getPosition()[2] << ", "
		  << p->getdEdx() << ", "
		  << p->getTime() << ", "
		  << p->getMomentum()[0] << ", "
		  << p->getMomentum()[1] << ", "
		  << p->getMomentum()[2] << ", "
		  << p->getPathLength() << std::endl;
	*/
	//double speed((35000.0+p->getPosition()[2])/(10000.0*p->getTime()));
	//std::cout << "speed, E = " << speed << std::endl;

	energy+=p->getdEdx()*p->getPathLength();
	xe+=p->getdEdx()*p->getPathLength()*p->getPosition()[0];
	ye+=p->getdEdx()*p->getPathLength()*p->getPosition()[1];
      }
      //std::cout << "Energy " << energy << ", average x " 
      //	<< xe/energy << ", y " << ye/energy << std::endl;
    }
  }

  hNCol->Fill(nCol);
}

void TBTrackMokkaCheck::End() {
  closeHFile();
}
