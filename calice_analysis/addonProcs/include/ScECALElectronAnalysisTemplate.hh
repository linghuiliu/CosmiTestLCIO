/* Processor which reads in CALICE Testbeam Data */
//#ifdef USE_LCCD

#ifndef SCECALELECTRONANALYSISTEMPLATE_h
#define SCECALELECTRONANALYSISTEMPLATE_h 1

#include <string>

//#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
//9809.1549
#include <TNtuple.h>


#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h" 
#include "IMPL/LCTOOLS.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "UTIL/LCTime.h"
#include "LCIOTypes.h"
#include "marlin/Processor.h"

//9816.1759 added stream issues.
#include <iostream>
#include <fstream>


using namespace lcio;
using namespace marlin;

namespace marlin {

  class ScECALElectronAnalysisTemplate : public Processor {

  public:
    virtual Processor* newProcessor() { return new ScECALElectronAnalysisTemplate;}
    ScECALElectronAnalysisTemplate();
    ~ScECALElectronAnalysisTemplate();
    virtual void init();  
    virtual void processEvent( LCEvent * evt ) ;
    virtual void end() {
      //TFile* outfile = new TFile("hist.root","RECREATE");
      _Esumhist->Write();
      std::cout << " Histograms were written. " << std::endl;
//9809.1425
      _fNEvents->Write();
      std::cout << " event loop Ntuple was written. " << std::endl;
      _fNHits->Write();
      std::cout << " hit Ecal loop Ntuple was written. " << std::endl;
//9816.1421
      _fNHitsH->Write();
      std::cout << " hit Hcal loop Ntuple was written. " << std::endl;
//
//   
      _outfile->Write();
      _outfile->Close();
      std::cout << " root file was closed." << std::endl;
      delete _outfile;
    };

  private:
    LCCollection* _inCol;
    LCCollection* _inColH;

    TFile* _outfile; 
    TH1F* _Esumhist;
//9809.1431
//9909.1550    TNtuple * _fNEvents;
    TTree * _fNEvents;
    TNtuple * _fNHits;
//9816.1422
    TNtuple * _fNHitsH;
//9816.1838 from CaliceVerySimpleExampleProcessor.cc
    enum{ nlayerH = 38 };
    int layerHD;
    int zposHD;
    int zposH[nlayerH];
    enum{ nlayer = 30 };
    enum{ nstrip = 72 };

//9909.1612 to convert TNtuple to TTree for _fNEvents.

    int    n_Evt;      
    double Esum;      
    double EsumH;    
    double EsumHinEM; 
    double EcenterX; 
    double EcenterY; 
    double EcenterZ; 
    float  edep_zHmax;  
    int    z_edepHmax; 
    float  edep_zmax;  
    int    z_edepmax;  
    double EcenterHX;
    double EcenterHY;
    double EcenterHZ;
    float  EHlastLmax; 
    float  EHlastLmaxEM;
	   
    //120508.1953 add isMonteCarloEvents
    bool _isMonteCarloEvents;

  };
}
#endif

//#endif
