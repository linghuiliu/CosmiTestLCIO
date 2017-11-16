#include <Clusteriser.hh>
#include <vector>
#include <EVENT/LCIO.h>
#include <EVENT/LCCollection.h>
#include <EVENT/CalorimeterHit.h>
#include <IMPL/ClusterImpl.h>
#include <IMPL/LCCollectionVec.h>
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif
#include <cassert>
#include <Box.hh>

#ifdef WITH_CONTROL_HISTOGRAMS
#include <histmgr/HistMgr.hh>
#include <CellIndex.hh>
#include <histmgr/FloatHistogram1D.hh>
#include <ModuleLocation.hh>
#endif

#ifndef WITH_CONTROL_HISTOGRAMS
#include <cmath>
#endif

#define CAN_CAST_CALORIMETERHIT_GETPOSITION

using namespace std;

namespace CALICE {

  inline Double_t sqr(const Double_t a) {return a*a;}

  // create instance to make this Processor known to Marlin
  Clusteriser a_Clusteriser_instance;
  
  /** Select Mips .
   * Mip tracks are search within the calorimeter hits. 
   * Developped for cosmics.
   */
  Clusteriser::Clusteriser() 
    : Processor("Clusteriser")
  {
    _description = "Select mips and histogram the signal" ;

    _nHitsMin=3;
    registerProcessorParameter( "NHitsMin" , 
			       "The minimum number of hits per track for accepted tracks." ,
			       _nHitsMin ,
			       _nHitsMin);

    _maxDist=1.;
    registerProcessorParameter( "MaxDist" , 
			       "The maximum allowed distance in x, y and z of hits of grouped to the same cluster." ,
			       _maxDist ,
			       _maxDist);

    _minEnergy=1.;
    registerProcessorParameter( "MinClusterEnergy" , 
			       "Minimum total energy of accepted clusters." ,
			       _minEnergy ,
			       _minEnergy);

    _w0=4.8;
    registerProcessorParameter( "WeightCutOff" , 
			       "Cut parameter which defines the energy threshold of hits considered in the centre-of-gravity determination." ,
			       _w0 ,
			       _w0);

    _posError.clear();
    _posError.push_back(1.5);
    _posError.push_back(1.5);
    _posError.push_back(.5);
    registerProcessorParameter( "ClusterPositionError" , 
			       "The error on the cluster position." ,
				_posError ,
				_posError,
				_posError.size());

    //    _nNeighboursMax=3;
    //    registerProcessorParameter( "NNeighboursMax" , 
    //			       "The maximum allowed number of neighbour hits per track." ,
    //			       _nNeighboursMax,
    //			       _nNeighboursMax);

    //    _neighbourDist=2.;
    //    registerProcessorParameter( "NeighbourDist" , 
    //			       "The distance in x+y of the same layer in which hits are considered to be neighbours" ,
    //			       _neighbourDist ,
    //				_neighbourDist);

    registerProcessorParameter( "HitCollectionName" , 
    				"Name of the Calorimeter hit collection"  ,
				_colName ,
    				std::string("calorimeterHits") ) ;

    registerProcessorParameter( "ClusterCollectionName" , 
    				"Name of the Cluster collection"  ,
				_clusterColName ,
    				std::string("EcalClusters") ) ;

#ifdef WITH_CONTROL_HISTOGRAMS
    // ------------ parameters of some control histograms
    registerProcessorParameter( "HistogramGroupName" , 
    				"The name of the histogram group under which the control histograms will be registered."  ,
				_histogramGroupName ,
    				type() );

    _signalHistPar.clear();
    _signalHistPar.push_back(200);
    _signalHistPar.push_back(0);
    _signalHistPar.push_back(1000);
    
    registerOptionalParameter( "SignalBinning" , 
			       "The binning for the control histograms of the signal distribution." ,
			       _signalHistPar ,
			       _signalHistPar ,
			       _signalHistPar.size() ) ;

    _posHistPar.clear();
    _posHistPar.push_back(200);
    _posHistPar.push_back(0);
    _posHistPar.push_back(1000);
    
    registerOptionalParameter( "PositionBinning" , 
			       "The binning of the histograms of the cluster barycentre." ,
			       _posHistPar ,
			       _posHistPar ,
			       _posHistPar.size() ) ;


#endif
  }

  void Clusteriser::init() {
    printParameters();
    assert( _posError.size()==3 );

#ifdef WITH_CONTROL_HISTOGRAMS
    _histGroupKey=histmgr::Key_t(_histogramGroupName);
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();

    _histCol[kH1NHits]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("NHits"),2,
						       HistPar(50,-.5,50-.5));
    _histCol[kH1ClusterSignal]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterSignal"),1,
							HistPar(static_cast<UInt_t>(_signalHistPar[0]), _signalHistPar[1], _signalHistPar[2]));
    _histCol[kH1Mean]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterBaryCentre"),3,
							HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]));
    _histCol[kH1RMS]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterRMS"),3,
						     HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]));
    // verify code sanity: Are all the histograms created?
    assert(kNH1==4);
#endif

  }


  void Clusteriser::processEvent( LCEvent * evtP ) {
    if (evtP) {
      try {
	LCCollection* col_hits = evtP->getCollection( _colName ) ;
	//	_isolatedHits.clear();

	std::vector<Box> cluster;

	// --- first step group nearby hits
	for(UInt_t element_i=0; element_i < (UInt_t) col_hits->getNumberOfElements(); element_i++) {
	  EVENT::CalorimeterHit *a_hit=dynamic_cast<EVENT::CalorimeterHit *>(col_hits->getElementAt(element_i));

#ifdef CAN_CAST_CALORIMETERHIT_GETPOSITION	
	  const ThreeVector_t *pointP=reinterpret_cast<const ThreeVector_t *>(a_hit->getPosition());
#else
	  ThreeVector_t temp;
	  temp.set(a_hit->getPosition());
	  const ThreeVector_t *pointP=&temp;
#endif
	
	  Double_t min_distance=FLT_MAX;
	  UInt_t min_cluster_index=cluster.size();
	  UInt_t cluster_index=0;
	  for(std::vector<Box>::const_iterator cluster_iter=cluster.begin(); cluster_iter!=cluster.end(); cluster_iter++, cluster_index++) {
	    if (cluster_iter->closerToEnvelopThan(_maxDist,pointP)) {
	      Double_t a_min_distance=cluster_iter->minDistanceToPoints(pointP);
	      if (a_min_distance<_maxDist) {
		if (a_min_distance<min_distance) {
		  min_cluster_index=cluster_index;
		}
	      }
	    }
	  }

	  if (min_cluster_index<cluster.size()) {
	    cluster[min_cluster_index].add(a_hit);
	  }
	  else {
	    cluster.push_back(Box());
	    cluster.back().add(a_hit);
	  }

	}

	// --- second step merge nearby groups 
	std::vector<Box> merged_cluster;
	for(std::vector<Box>::iterator cluster_iter=cluster.begin(); cluster_iter!=cluster.end(); cluster_iter++) {
	  std::vector<Box>::iterator iter=cluster_iter;
	  iter++;
	  for(; iter!=cluster.end(); iter++) {
	    if (iter->closerToEnvelopThan(_maxDist,*cluster_iter)) {
	      iter->add(*cluster_iter);
	      cluster_iter->setMerged();
	      break;
	    }
	  }
	}

	// --- third step split clusters
	// TODO
	
	// --- final step: create Cluster collection
	EVENT::LCCollection* col_cluster=new IMPL::LCCollectionVec( EVENT::LCIO::CLUSTER );
	UInt_t cluster_i=0;
	for(std::vector<Box>::const_iterator cluster_iter=cluster.begin(); cluster_iter!=cluster.end(); cluster_iter++,cluster_i++) {
	  if (!cluster_iter->isMerged()) {
#ifdef WITH_CONTROL_HISTOGRAMS
	    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1NHits]->getElementAt(0))->fill(cluster_iter->_hits.size());
#endif
	    if (cluster_iter->_hits.size()>=static_cast<UInt_t>(_nHitsMin) ) {
	      IMPL::ClusterImpl *a_cluster=new IMPL::ClusterImpl;
	      Double_t sum[3]={0.,0.,0.};
	      Double_t sum2[3]={0.,0.,0.};
	      Double_t sum_of_weights=0.;
	      Double_t total_energy=0.;
	      
	      for (std::vector<const EVENT::CalorimeterHit *>::const_iterator hit_iter=cluster_iter->_hits.begin();
		   hit_iter != cluster_iter->_hits.end();
		   hit_iter++) {
		if ((*hit_iter)->getEnergy()>0.) {
		  total_energy+=(*hit_iter)->getEnergy();
		}
	      }

	      for (std::vector<const EVENT::CalorimeterHit *>::const_iterator hit_iter=cluster_iter->_hits.begin();
		   hit_iter != cluster_iter->_hits.end();
		   hit_iter++) {
		if ((*hit_iter)->getEnergy()>0.) {
		  Double_t energy=(*hit_iter)->getEnergy();
		  Double_t weight=_w0+std::log(energy/total_energy);
		  if (weight>0) {
		    for (UInt_t coord_i=0; coord_i<3; coord_i++) {
		      sum[coord_i]+=(*hit_iter)->getPosition()[coord_i] * weight;
		      sum2[coord_i]+= sqr((*hit_iter)->getPosition()[coord_i]) * weight;
		    }
		    sum_of_weights+=weight;
		  }
		  a_cluster->addHit(const_cast<EVENT::CalorimeterHit *>(*hit_iter),1.);
		}
	      }

#ifdef WITH_CONTROL_HISTOGRAMS
	      dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1ClusterSignal]->getElementAt(0))->fill(total_energy);
#endif
	      if (total_energy>_minEnergy) {
		Float_t pos[3];
		EVENT::FloatVec pos_err;
		pos_err.resize(6,0.);
		if (sum_of_weights>0.) {
		  
		  for (UInt_t coord_i=0; coord_i<3; coord_i++) {
		    Double_t mean=sum[coord_i]/sum_of_weights;
		    sum2[coord_i]=sqrt((sum2[coord_i]-mean*sum[coord_i])/(sum_of_weights));
		    pos[coord_i]=mean;
#ifdef WITH_CONTROL_HISTOGRAMS
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1Mean]->getElementAt(coord_i))->fill(mean);
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1RMS]->getElementAt(coord_i))->fill(sum2[coord_i]);
#endif
		  }
		}
#ifdef WITH_CONTROL_HISTOGRAMS
		dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1NHits]->getElementAt(0))->fill(cluster_iter->_hits.size());
#endif

 		pos_err[1]=pos_err[2]=pos_err[4]=0.;
// 		pos_err[0]=sum2[0];
// 		pos_err[3]=sum2[1];
// 		pos_err[5]=sum2[2];
		pos_err[0]=_posError[0];
		pos_err[3]=_posError[1];
		pos_err[5]=_posError[2];
		a_cluster->setPosition(pos);
		a_cluster->setPositionError(pos_err);
		a_cluster->setEnergy(total_energy);
		
		// TODO: calculate cluster thrust axis.
		
		col_cluster->addElement(a_cluster);
	      }
	      else {
		delete a_cluster;
	      }
	    }
	  }
	}
	evtP->addCollection(col_cluster, _clusterColName);
	
      }
      catch (lcio::DataNotAvailableException err) {
      }
    }
  }

  void Clusteriser::end() {
  }

}
