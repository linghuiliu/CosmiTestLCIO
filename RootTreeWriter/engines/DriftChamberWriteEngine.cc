#include "DriftChamberWriteEngine.hh"

#include <cfloat>
#include <cmath>

#include "EVENT/CalorimeterHit.h"
#include "UTIL/LCTypedVector.h"
#include <IMPL/TrackerHitImpl.h>
#include <IMPL/TrackImpl.h>
#include <EVENT/Track.h>
#include <TriggerBits.hh>
#include <collection_names.hh>


using namespace lcio;
using namespace std;
using namespace CALICE;

#define DDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  void DriftChamberWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterInputCollection(LCIO::RECONSTRUCTEDPARTICLE,_engineName+"_InCol",
						"Name of input collection",
						_driftChamberColName, std::string("driftchamber_track")  );

    _hostProcessor.relayRegisterProcessorParameter("PrototypeModel" , 
						   "Specify the prototype setup: TBDesy0506, TBDesy0506_1, TBCern0806,...." ,
						   _model, std::string("TBCern0707_01")  );
  }

  void DriftChamberWriteEngine::registerBranches( TTree* hostTree )
  {
    hostTree->Branch("DC_EcalXYZ", &_hitsFill.EcalXYZ,"ecalXYX[3]/F");
    hostTree->Branch("DC_HcalXYZ", &_hitsFill.HcalXYZ,"hcalXYZ[3]/F");
    hostTree->Branch("DC_Phi"    , &_hitsFill.Phi    ,"phi/F");
    hostTree->Branch("DC_Lambda" , &_hitsFill.Lambda ,"lambda/F");
    hostTree->Branch("DC_Chi2"   , &_hitsFill.Chi2   ,"chi2/F");
  }

  void DriftChamberWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    const TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));
    if (trigBits.isBeamTrigger()){
    
    LCCollection* inCol;
    try
      {
	inCol = evt->getCollection( _driftChamberColName );

	Track* trk = dynamic_cast<Track*>( inCol->getElementAt( 0 ) ) ;
	//	if (trk->getChi2()<5)_goodtrack = true;   
	_hitsFill.EcalXYZ[0]=trk->getD0();
	_hitsFill.EcalXYZ[1]=trk->getZ0();
	_hitsFill.Phi=trk->getPhi();
	_hitsFill.Lambda=atan(trk->getTanLambda());
	_hitsFill.Chi2=trk->getChi2();

	float _front_face_ECAL_z=0;
	float _front_face_HCAL_z=0;
	if (_model.compare("TBCern0806")==0){ 
	  //CERN 08/06 old coordinate system
	  _front_face_ECAL_z = -202.7;
	  _front_face_HCAL_z = 655.0;
	}
	if (_model.compare("TBCern1006")==0){ 
	  //CERN 10/06 old coordinate system    
	  _front_face_ECAL_z = -202.7;
	  _front_face_HCAL_z = 57.0;
	}
	if (_model.compare("TBCern0806_01")==0){ 
	//CERN 08/06 new coordinate system
	  _front_face_ECAL_z = 759.15;
	  _front_face_HCAL_z = 1616.0;
	}
	if (_model.compare("TBCern1006_01")==0){ 
	  //CERN 10/06 new coordinate system
	  _front_face_ECAL_z = 2054.15;
	  _front_face_HCAL_z = 2313.0;
	}
	if (_model.compare("TBCern0707")==0||_model.compare("TBCern0807")==0||_model.compare("TBCern07")==0){ 
	  /*  //CERN 07/07 old coordinate system    
	  std::cout<<"Deprecated coordinate system, no more supported, use at your risk!!!!!!"<<std::endl;
	  _front_face_ECAL_z = -202.7;
	  _front_face_HCAL_z = 31.73;
	  }
	  if (_model.compare("TBCern0707_01")==0){ 
	  */

	  //CERN 07/07 new coordinate system
	  _front_face_ECAL_z = 1487;
	  _front_face_HCAL_z = 1716.0;
	}

	_hitsFill.EcalXYZ[2]=_front_face_ECAL_z;
	_hitsFill.HcalXYZ[2]=_front_face_HCAL_z;

	// project track impact point from front ECAL to front HCAL
	_hitsFill.HcalXYZ[0]=_hitsFill.EcalXYZ[0]/_front_face_ECAL_z*_front_face_HCAL_z;
	_hitsFill.HcalXYZ[1]=_hitsFill.EcalXYZ[1]/_front_face_ECAL_z*_front_face_HCAL_z;
      }

    catch ( DataNotAvailableException err )
      {
	//	cout <<  "WARNING: Collection "<< _driftChamberColName
	//	     << " not available in event "<< evt->getEventNumber() << endl;
	for(int i=0;i<3;i++){
	  _hitsFill.HcalXYZ[i] = 0;
	  _hitsFill.EcalXYZ[i] = 0;
	  _hitsFill.Phi        = 0;
	  _hitsFill.Lambda     = 0;
	  _hitsFill.Chi2       = 999;
	}
      }
    }
    
  }

}//namespace marlin
