#ifdef RTW_USE_CLUSTERING

#include "DeepAnaWriteEngine.hh"
#include <DeepAnalysis.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/ClusterImpl.h>
#include <cfloat>
#include "DeepAnalysisProcessor.hh"
#include <TriggerBits.hh>
#include <collection_names.hh>

using namespace std;
using namespace CALICE;

#define DDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDD (-DBL_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  void DeepAnaWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterInputCollection(LCIO::RECONSTRUCTEDPARTICLE,
						_engineName+"_InCol",
						"Name of input collection",
						_deepanaInColName, 
						std::string("AhcClusters")  );
 
   _hostProcessor.relayRegisterInputCollection(LCIO::RECONSTRUCTEDPARTICLE,
						_engineName+"_InNeutrCol",
						"Name of input collection",
						_deepanaInNeutrColName, 
						std::string("AhcNeutrClusters")  );

  }

  void DeepAnaWriteEngine::registerBranches( TTree* hostTree )
  {
    //electromagnetic
    hostTree->Branch("elmNoClusters",  &_elmNoClusters , "elmNoClusters/I"  );
    hostTree->Branch("elmNoHits",      &_elmNoHits,      "elmNoHits/I"      );
    hostTree->Branch("elmEngySum",     &_elmEngySum,     "elmEngySum/D"     );
    //track
    hostTree->Branch("trkNoClusters",  &_trkNoClusters , "trkNoClusters/I"  );
    hostTree->Branch("trkNoHits",      &_trkNoHits,      "trkNoHits/I"      );
    hostTree->Branch("trkEngySum",     &_trkEngySum,     "trkEngySum/D"      );
    //hadronic
    hostTree->Branch("hadNoClusters",  &_hadNoClusters , "hadNoClusters/I"  );
    hostTree->Branch("hadNoHits",      &_hadNoHits,      "hadNoHits/I"      );
    hostTree->Branch("hadEngySum",     &_hadEngySum,     "hadEngySum/D"      );
    //neutral
    hostTree->Branch("neutrNoHits",      &_neutrNoHits,      "neutrNoHits/I"      );
    hostTree->Branch("neutrEngySum",     &_neutrEngySum,     "neutrEngySum/D"      );
   
    hostTree->Branch("elmX", &_elmX, "elmX/D");
    hostTree->Branch("elmY", &_elmY, "elmY/D");
    hostTree->Branch("elmZ", &_elmZ, "elmZ/D");

    hostTree->Branch("trkX", &_trkX, "trkX/D");
    hostTree->Branch("trkY", &_trkY, "trkY/D");
    hostTree->Branch("trkZ", &_trkZ, "trkZ/D");

    hostTree->Branch("hadX", &_hadX, "hadX/D");
    hostTree->Branch("hadY", &_hadY, "hadY/D");
    hostTree->Branch("hadZ", &_hadZ, "hadZ/D");

    hostTree->Branch("X", &_X, "X/D");
    hostTree->Branch("Y", &_Y, "Y/D");
    hostTree->Branch("Z", &_Z, "Z/D");

  }


  void DeepAnaWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    DeepAnalysis deepAnalysis; 
    LCCollection* clusterCollection;

    try{
	//accept only beam events
	const TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));
	if (trigBits.isBeamTrigger()){

	  clusterCollection = evt->getCollection( _deepanaInColName );

	  _elmNoClusters = 0;
	  _elmEngySum    = 0;
	  _trkNoClusters = 0;
	  _trkEngySum    = 0;
	  _hadNoClusters = 0;
	  _hadEngySum    = 0;
	  _neutrEngySum  = 0;
	  _neutrNoHits   = 0;

	  _elmX = 0;
	  _elmY = 0;
	  _elmZ = 0;

	  _trkX = 0;
	  _trkY = 0;
	  _trkZ = 0;
	  
	  _hadX = 0;
	  _hadY = 0;
	  _hadZ = 0;

	  _X = 0;
	  _Y = 0;
	  _Z = 0;

	  _elmNoHits  = 0;
	  _trkNoHits  = 0;
	  _hadNoHits  = 0;
	  _neutrNoHits  = 0;

	  //loop over all elements in the input collection
	  for(int elem_i = 0; elem_i < clusterCollection->getNumberOfElements(); elem_i++){
	    EVENT::Cluster *a_cluster = dynamic_cast<EVENT::Cluster *>(clusterCollection->getElementAt(elem_i));
	    int clusterType = a_cluster->getType();

	    //see calice_reco/clustering/include/DeepAnalysisProcessor.hh
	    //ClusterTypes elm   = CLUSTER_EM;  //10
	    //ClusterTypes trk   = CLUSTER_TRK; //11
	    //ClusterTypes had   = CLUSTER_HAD; //12
	    //ClusterTypes neutr = CLUSTER_NEUTR; //13

	    if ( (clusterType >> CLUSTER_EM) ==1 ){
	      _elmEngySum += a_cluster->getEnergy();
	      _elmNoClusters++;
	    }
	    else if ( (clusterType >> CLUSTER_TRK) ==1 ){
	      _trkEngySum += a_cluster->getEnergy();
	      _trkNoClusters++;
	    }
	    else if ( (clusterType >> CLUSTER_HAD) ==1 ){
	      _hadEngySum += a_cluster->getEnergy();
	      _hadNoClusters++;
	    }


	    unsigned int size = (a_cluster->getCalorimeterHits()).size();
	    
	    if (      (clusterType >> CLUSTER_TRK) ==1 ) {
	      _trkNoHits = _trkNoHits + size;
	    }
	    else if ( (clusterType >> CLUSTER_EM)  ==1 ){
	      _elmNoHits = _elmNoHits + size;
	    }
	    else if ( (clusterType >> CLUSTER_HAD) ==1 ) {
	      _hadNoHits = _hadNoHits +  size;
	    }

	    const EVENT::CalorimeterHitVec &hit_col = a_cluster->getCalorimeterHits();
	    for (EVENT::CalorimeterHitVec::const_iterator hit_iter = hit_col.begin();
		 hit_iter!=hit_col.end();
		 hit_iter++) {
	      
	      double x = (*hit_iter)->getPosition()[0];
	      double y = (*hit_iter)->getPosition()[1];
	      double z = (*hit_iter)->getPosition()[2];
	      
	      if (      (clusterType >> CLUSTER_TRK) ==1 ) {
		_trkX = x;
		_trkY = y;
		_trkZ = z;
	      }
	      else if ( (clusterType >> CLUSTER_EM)  ==1 ) {
		_elmX = x;
		_elmY = y;
		_elmZ = z;
	      }
	      else if ( (clusterType >> CLUSTER_HAD) ==1 ){
		_hadX = x;
		_hadY = y;
		_hadZ = z;
	      }

	      _X = x;
	      _Y = y;
	      _Z = z;
	      
	    }//end loop over CalorimeterHits
	  }//end loop over elements in input collection

	  //=====================================================================
	  //=====================================================================
	  //=====================================================================
	  LCCollection *neutrCol = evt->getCollection(_deepanaInNeutrColName);
	  for (int j = 0; j < neutrCol->getNumberOfElements(); j++){
	    CalorimeterHit *a_hit = dynamic_cast<CalorimeterHit *>(neutrCol->getElementAt(j));
	    _neutrEngySum += a_hit->getEnergy();
	    _neutrNoHits++;
	  }
	  //=====================================================================
	  //=====================================================================
	  //=====================================================================

	}//end of if(isTriggerBeam)
      }//end of try

    catch ( DataNotAvailableException err ) {
      cout <<  "WARNING: Collection "<< _deepanaInColName
	   << " not available in event "<< evt->getEventNumber() << endl;
      
      _elmNoClusters = INVALIDI;
      _elmNoHits     = INVALIDI;
      _elmEngySum    = INVALIDD;
      _trkNoClusters = INVALIDI;
      _trkNoHits     = INVALIDI;
      _trkEngySum    = INVALIDD;
      _hadNoClusters = INVALIDI;
      _hadNoHits     = INVALIDI;
      _hadEngySum    = INVALIDD;
      _neutrNoHits   = INVALIDI;
      _neutrEngySum  = INVALIDD;
      
      _elmX = INVALIDD;
      _elmY = INVALIDD;
      _elmZ = INVALIDD;
      _trkX = INVALIDD;
      _trkY = INVALIDD;
      _trkZ = INVALIDD;
      _hadX = INVALIDD;
      _hadY = INVALIDD;
      _hadZ = INVALIDD;
      _X = INVALIDD;
      _Y = INVALIDD;
      _Z = INVALIDD;
    }
    
    
  }

}//namespace marlin


#endif //RTW_USE_CLUSTERING
