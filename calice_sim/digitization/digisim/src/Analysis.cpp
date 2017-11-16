#include "Analysis.hpp"
#include "EVENT/LCIO.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCCollection.h"
#include "marlin/StringParameters.h"
// #include "TupleMgr.hh"

#include <iostream>
#include <cassert>

#include "TH1F.h"

using namespace IMPL;
using namespace marlin;
using namespace std;

namespace digisim {

  // Global instance needed to register with the framework
  Analysis global;
//   TFile Analysis::_rootFile("basic.root","RECREATE");

  //.. constructor
  Analysis::Analysis() : Processor("Analysis") {
    _description = " CalHit analysis processor";

    string empty("empty");;
    registerProcessorParameter( "EcalCollectionName",
				"Name of input ECAL CalorimeterHits collection",
				_ecalCollName,
				empty );

    registerProcessorParameter( "HcalCollectionName",
				"Name of input HCAL CalorimeterHits collection",
				_hcalCollName,
				empty );

    registerProcessorParameter( "TcmtCollectionName",
				"Name of input TCMT CalorimeterHits collection",
				_tcmtCollName,
				empty );
  }

  //.. destructor
  Analysis::~Analysis() {

//     cout<< "Destructor was called! "<< name() <<" "<<_rawNames.size()<<" "<< _outputNames.size() << endl;
//     for(vector<string>::iterator icol = _collNames.begin();
// 	icol != _collNames.end() ; ++icol ) {
//       _collNames.erase(icol);
//     }
//     _collNames.clear();
//     if(_converter) delete _converter;
  }


//   void Analysis::processRunHeader( LCRunHeader* run) {
//     cout << "Analysis::processRun()  " << name()
// 	 << " in run " << run->getRunNumber()
// 	 << endl ;
//     _nRun++ ;
//   }


  void Analysis::processEvent( LCEvent *evt ) {
//     // loop over all collections in the event
//     for(unsigned int i=0; i<_collNames.size(); ++i) {
//       string collname = _collNames[i];

    ++_nEvt;
    _evt = evt;

//     _rootFile.cd("Ecal");
    this->processEcalData( evt, _ecalCollName );

//     _rootFile.cd("../Hcal");
    this->processHcalData( evt, _hcalCollName );

//     _rootFile.cd("../Tcmt");
    this->processTcmtData( evt, _tcmtCollName );

//     _rootFile.cd("../Hcal");
//     this->processHcalRawData( evt, "TBhcal06_01_hcalSD" );

//     _rootFile.cd("..");

//     // fill tuple
//     TupleMgr::fillTuple(_tuple);
  }

  void Analysis::processTcmtData(LCEvent *evt, string colName) {
    // loop over hits in collection
    LCCollection* hits = 0;
    try {
      hits = _evt->getCollection(colName.c_str());
    }
    catch(Exception x) {
      cout<<"*** Data not available: "<< colName << endl;
    }

    if(!hits) return;

    int nhits = 0;
    int eDigiTcmt=0;
    double sumTcmtE=0.0;
    int nhitsInLayer[NLAYERSTCMT] = {NLAYERSTCMT*0};
    double energyInLayer[NLAYERSTCMT] = {NLAYERSTCMT*0.0};
    for(int i=0; i<hits->getNumberOfElements(); ++i) {
      LCObject *obj = hits->getElementAt(i);
      CalorimeterHit& ihit = *(CalorimeterHit*)obj;

      double energy = ihit.getEnergy();
      int cellid = ihit.getCellID0();

      // Mokka convention (virtual cells)
//       int layer = (cellid & 0xff) - 1;
      // CALICE convention (remapped in TcmtGanging)
      int layer = (cellid >> 24) -1;
//       int strip = (cellid >> 6) & 0x1ff;
//       if(strip==0) strip = (cellid >> 15) & 0x1ff;

//       const float* pos = ihit.getPosition();
//       cout<<"TCMT hit: id="<< hex << cellid << dec <<", layer="<< layer
// 	  <<" str="<< strip <<" E="<< energy
// 	  <<" pos=("<< pos[0] <<"; "<< pos[1] <<"; "<< pos[2] <<")"<< endl;

      ++(nhitsInLayer[layer]);
      energyInLayer[layer] += energy;

      _hTcmtCellEnergy->Fill(energy);
      _hTcmtCellEnergyMIP->Fill(energy);

//       // fill tuple structure
//       _tuple.tcmEne[nhits] = energy;
//       _tuple.tcmId[nhits] = cellid;
      ++nhits;

      if(layer>=0 && layer<8) {
	++eDigiTcmt;
	sumTcmtE += energy;
      }
      else if(layer>=8 && layer<16) {
	eDigiTcmt += 5;
	sumTcmtE += 4.92*energy;
      }
      else {
	cout<<"***** Analysis error: event="<< evt->getEventNumber() <<", TCMT layer="<< layer << endl;
      }
    }
//     _tuple.nhitsTC = nhits;
//     _tuple.eDigiTC = eDigiTcmt;
//     _tuple.eneTC = sumTcmtE;

    _hTcmtHitsPerEvent->Fill(nhits);
    for(int ilay=0; ilay<NLAYERSTCMT; ++ilay) {
      _hTcmtLayerEnergy->Fill( ilay, energyInLayer[ilay] );
      _hTcmtHitsPerLayer->Fill( ilay, nhitsInLayer[ilay] );
    }
  }

  void Analysis::processEcalData(LCEvent *evt, string colName) {
    // loop over hits in collection
    LCCollection* hits = 0;
    try {
      hits = _evt->getCollection(colName.c_str());
    }
    catch(Exception x) {
      cout<<"*** Data not available: "<< colName << endl;
    }

    if(!hits) return;

    int nhits = 0;
    int eDigiEcal=0;
    double sumEcalE=0.0;
    int nhitsInLayer[NLAYERSECAL] = {NLAYERSECAL*0};
    double energyInLayer[NLAYERSECAL] = {NLAYERSECAL*0.0};

    for(int i=0; i<hits->getNumberOfElements(); ++i) {
      LCObject *obj = hits->getElementAt(i);
      CalorimeterHit& ihit = *(CalorimeterHit*)obj;

      double energy = ihit.getEnergy();
      int cellid = ihit.getCellID0();
      int layer = (cellid >> 24) & 0x3f;  // first Ecal layer is 0 in CalHits
//       cout<<"Ecal hit: id="<< hex << cellid << dec <<", layer="<< layer << endl;

      ++(nhitsInLayer[layer]);
      energyInLayer[layer] += energy;

      _hEcalCellEnergy->Fill(energy);
      _hEcalCellEnergyMIP->Fill(energy);

//       // fill tuple structure
//       _tuple.emcEne[nhits] = energy;
//       _tuple.emcId[nhits] = cellid;
      ++nhits;

      if(layer>=0 && layer<10) {
	++eDigiEcal;
	sumEcalE += energy;
      }
      else if(layer>=10 && layer<20) {
	eDigiEcal += 2;
	sumEcalE += 2*energy;
      }
      else if(layer>=20 && layer<30) {
	eDigiEcal += 3;
	sumEcalE += 3*energy;
      }
      else {
	cout<<"***** Analysis error: event="<< evt->getEventNumber() <<", ECAL layer="<< layer << endl;
      }
    }
//     _tuple.nhitsEM = nhits;
//     _tuple.eDigiEM = eDigiEcal;
//     _tuple.eneEM = sumEcalE;

    _hEcalHitsPerEvent->Fill(nhits);
    for(int ilay=0; ilay<NLAYERSECAL; ++ilay) {
      _hEcalLayerEnergy->Fill( ilay, energyInLayer[ilay] );
      _hEcalHitsPerLayer->Fill( ilay, nhitsInLayer[ilay] );
    }
  }

  void Analysis::processHcalData(LCEvent *evt, string colName) {
    // loop over hits in collection
    LCCollection* hits = 0;
    try {
      hits = _evt->getCollection(colName.c_str());
    }
    catch(Exception x) {
      cout<<"*** Data not available: "<< colName << endl;
    }

    if(!hits) return;

    int nhits = 0;
    int eDigiHcal=0;
    double sumHcalE=0.0;
    int nhitsInLayer[NLAYERSHCAL] = {NLAYERSHCAL*0};
    double energyInLayer[NLAYERSHCAL] = {NLAYERSHCAL*0.0};

    for(int i=0; i<hits->getNumberOfElements(); ++i) {
      LCObject *obj = hits->getElementAt(i);
      CalorimeterHit& ihit = *(CalorimeterHit*)obj;

      double energy = ihit.getEnergy();
      int cellid = ihit.getCellID0();

      // CALICE convention
      int layer = ((cellid >> 24) & 0x3f)+1;  // first Hcal layer is 1 in CalHits
      // Mokka HCAL convention
//       int layer = cellid & 0xff;

//       const float* pos = ihit.getPosition();
//       cout<<"HCAL hit: id="<< hex << cellid << dec <<", layer="<< layer
// 	  <<" pos=("<< pos[0] <<"; "<<pos[1] <<"; "<<pos[2] <<")"<< endl;

      ++(nhitsInLayer[layer]);
      energyInLayer[layer] += energy;
    
      _hHcalCellEnergy->Fill(energy);
      _hHcalCellEnergyMIP->Fill(energy);

//       // fill tuple structure
//       _tuple.ahcEne[nhits] = energy;
//       _tuple.ahcId[nhits] = cellid;
      ++nhits;

      if(layer>=1 && layer<=17) {
	++eDigiHcal;
	sumHcalE += energy;
      }
      else if(layer>=19 && layer<=29) {
	eDigiHcal += 2;
	sumHcalE += 2*energy;
      }
      else {
	cout<<"***** Analysis error: event="<< evt->getEventNumber() <<", HCAL layer="<< layer << endl;
      }
    }
//     _tuple.nhitsHD = nhits;
//     _tuple.eDigiHD = eDigiHcal;
//     _tuple.eneHD = sumHcalE;

    _hHcalHitsPerEvent->Fill(nhits);
    for(int ilay=0; ilay<NLAYERSHCAL; ++ilay) {
      _hHcalLayerEnergy->Fill( ilay, energyInLayer[ilay+1] );
      _hHcalHitsPerLayer->Fill( ilay, nhitsInLayer[ilay+1] );
    }
  }

  //============================================================

  void Analysis::processHcalRawData(LCEvent *evt, string colName) {
    // loop over hits in collection
    LCCollection* hits = 0;
    try {
      hits = _evt->getCollection(colName.c_str());
    }
    catch(Exception x) {
      cout<<"*** Data not available: "<< colName << endl;
    }

    if(!hits) return;

    double energyInLayer[NLAYERSHCAL] = {NLAYERSHCAL*0.0};

//     cout<<"### Event "<< evt->getEventNumber() << endl;
    for(int i=0; i<hits->getNumberOfElements(); ++i) {
      LCObject *obj = hits->getElementAt(i);
      SimCalorimeterHit& ihit = *(SimCalorimeterHit*)obj;

      double energy = ihit.getEnergy();
      int cellid = ihit.getCellID0();
      int layer = (cellid & 0xff) - 1;  // first Hcal layer is 1 in CalHits
//       cout<<"HCAL hit: id="<< hex << cellid << dec <<", layer="<< layer
// 	  <<", E="<< energy << endl;

      energyInLayer[layer] += energy;
    }

    for(int ilay=0; ilay<NLAYERSHCAL; ++ilay) {
      if(energyInLayer[ilay]>0) _hHcalLiveEnergyPerLayer->Fill( 1000*energyInLayer[ilay] );
    }
  }

  void Analysis::init() {
    // initialization from steering file
    //StringParameters* pars = parameters();
    cout << "Analysis.init(): parameters=<"<< *parameters() << ">" << endl;
    _nEvt = 0;
    _nRun = 0;

//     // loop over all collections in the event
//     for(unsigned int i=0; i<_collNames.size(); ++i) {

//       string collname = _collNames[i];

//     for(int i)

//     _rootFile.mkdir("Tcmt");
//     _rootFile.cd("Tcmt");

    const char* name = "TcmtCellEnergy";
    _hTcmtCellEnergy = new TH1F(name,name,100,0,100);
    _hTcmtCellEnergy->GetXaxis()->SetTitle("Energy (mips)");
    _hTcmtCellEnergy->GetYaxis()->SetTitle("# entries / bin");

    name = "TcmtCellEnergyMIP";
    _hTcmtCellEnergyMIP = new TH1F(name,name,60,0,3);
    _hTcmtCellEnergyMIP->GetXaxis()->SetTitle("Energy (mips)");
    _hTcmtCellEnergyMIP->GetYaxis()->SetTitle("# entries / bin");

    name = "TcmtHitsPerEvent";
    _hTcmtHitsPerEvent = new TH1F(name,name,100,0,200);
    _hTcmtHitsPerEvent->GetXaxis()->SetTitle("# hits");
    _hTcmtHitsPerEvent->GetYaxis()->SetTitle("# entries / bin");

    name = "TcmtLayerEnergy";
    _hTcmtLayerEnergy = new TProfile(name,name,NLAYERSTCMT,0,NLAYERSTCMT);
    _hTcmtLayerEnergy->GetXaxis()->SetTitle("layer");
    _hTcmtLayerEnergy->GetYaxis()->SetTitle("Energy (mips)");

    name = "TcmtHitsPerLayer";
    _hTcmtHitsPerLayer = new TProfile(name,name,NLAYERSTCMT,0,NLAYERSTCMT);
    _hTcmtHitsPerLayer->GetXaxis()->SetTitle("layer");
    _hTcmtHitsPerLayer->GetYaxis()->SetTitle("# hits");

    // Ecal histos
//     _rootFile.mkdir("Ecal");
//     _rootFile.cd("Ecal");

    name = "EcalCellEnergy";
    _hEcalCellEnergy = new TH1F(name,name,100,0,100);
    _hEcalCellEnergy->GetXaxis()->SetTitle("Energy (mips)");
    _hEcalCellEnergy->GetYaxis()->SetTitle("# entries / bin");

    name = "EcalCellEnergyMIP";
    _hEcalCellEnergyMIP = new TH1F(name,name,60,0,3);
    _hEcalCellEnergyMIP->GetXaxis()->SetTitle("Energy (mips)");
    _hEcalCellEnergyMIP->GetYaxis()->SetTitle("# entries / bin");

    name = "EcalHitsPerEvent";
    _hEcalHitsPerEvent = new TH1F(name,name,100,0,200);
    _hEcalHitsPerEvent->GetXaxis()->SetTitle("# hits");
    _hEcalHitsPerEvent->GetYaxis()->SetTitle("# entries / bin");

    name = "EcalLayerEnergy";
    _hEcalLayerEnergy = new TProfile(name,name,NLAYERSECAL,0,NLAYERSECAL);
    _hEcalLayerEnergy->GetXaxis()->SetTitle("layer");
    _hEcalLayerEnergy->GetYaxis()->SetTitle("Energy (mips)");

    name = "EcalHitsPerLayer";
    _hEcalHitsPerLayer = new TProfile(name,name,NLAYERSECAL,0,NLAYERSECAL);
    _hEcalHitsPerLayer->GetXaxis()->SetTitle("layer");
    _hEcalHitsPerLayer->GetYaxis()->SetTitle("# hits");


    // Hcal histos
//     _rootFile.mkdir("Hcal");
//     _rootFile.cd("Hcal");

    name = "HcalCellEnergy";
    _hHcalCellEnergy = new TH1F(name,name,100,0,100);
    _hHcalCellEnergy->GetXaxis()->SetTitle("Energy (mips)");
    _hHcalCellEnergy->GetYaxis()->SetTitle("# entries / bin");

    name = "HcalCellEnergyMIP";
    _hHcalCellEnergyMIP = new TH1F(name,name,60,0,3);
    _hHcalCellEnergyMIP->GetXaxis()->SetTitle("Energy (mips)");
    _hHcalCellEnergyMIP->GetYaxis()->SetTitle("# entries / bin");

    name = "HcalLiveEnergyPerLayer";
    _hHcalLiveEnergyPerLayer = new TH1F(name,name,120,0,3);
    _hHcalLiveEnergyPerLayer->GetXaxis()->SetTitle("Live energy (MeV)");
    _hHcalLiveEnergyPerLayer->GetYaxis()->SetTitle("# entries / bin");

    name = "HcalHitsPerEvent";
    _hHcalHitsPerEvent = new TH1F(name,name,100,0,200);
    _hHcalHitsPerEvent->GetXaxis()->SetTitle("# hits");
    _hHcalHitsPerEvent->GetYaxis()->SetTitle("# entries / bin");

    name = "HcalLayerEnergy";
    _hHcalLayerEnergy = new TProfile(name,name,NLAYERSHCAL,0,NLAYERSHCAL);
    _hHcalLayerEnergy->GetXaxis()->SetTitle("layer");
    _hHcalLayerEnergy->GetYaxis()->SetTitle("Energy (mips)");

    name = "HcalHitsPerLayer";
    _hHcalHitsPerLayer = new TProfile(name,name,NLAYERSHCAL,0,NLAYERSHCAL);
    _hHcalHitsPerLayer->GetXaxis()->SetTitle("layer");
    _hHcalHitsPerLayer->GetYaxis()->SetTitle("# hits");


//     // setup RawHits -> CalHits converter
//     _converter = new RawHitConverter(_eneFactor, _timeFactor);

//     _rootFile.cd("..");
  }

  void Analysis::end() {
    cout << "Analysis::end()  " << name()
	 << " processed " << _nEvt << " events in " << _nRun << " runs "
	 << endl ;

//     _rootFile.Write();
//     _rootFile.Close();
    // self destruction!!!  It seems Marlin does not destruct it...
    //delete this;
  }
}// namespace digisim
