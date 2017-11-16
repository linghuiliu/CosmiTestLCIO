#include <MipSelect.hh>
#include <vector>
#include <algorithm>
#include <EVENT/LCIO.h>
#include <EVENT/LCCollection.h>
#include <EVENT/CalorimeterHit.h>
#include <IMPL/ClusterImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <NVector_t.hh>
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif
#include <Box.hh>
#include <LinearRegression.hh>
#include <marlin/ConditionsProcessor.h>
#include <cassert>


#ifdef WITH_CONTROL_HISTOGRAMS
#include <histmgr/HistMgr.hh>
#include <CellIndex.hh>
#include <histmgr/FloatHistogram1D.hh>
#include <ModuleLocation.hh>
#endif

#ifndef WITH_CONTROL_HISTOGRAMS
#include <cmath>
#endif

namespace CALICE {

  // create instance to make this Processor known to Marlin
  MipSelect a_MipSelect_instance;
  
  /** Select Mips .
   * Mip tracks are search within the calorimeter hits. 
   * Developped for cosmics.
   */
  MipSelect::MipSelect() 
    : Processor("MipSelect")
#ifdef WITH_CONTROL_HISTOGRAMS
    ,_histGroupKey(type()),
      _histHitPosKey("HitMap"),
      _moduleLocationChange(this,&MipSelect::moduleLocationChanged),
      _lastLayer(0)
#endif

  {
    _description = "Select mips and histogram the signal" ;

    _nHits.push_back(3);
    _nHits.push_back(50);
    registerProcessorParameter( "NHits" , 
			       "The minimum and the maximum number of hits per track for accepted tracks." ,
			       _nHits ,
			       _nHits);


    _maxDist=1.;
    registerProcessorParameter( "MaxDist" , 
			       "The maximum allowed distance in x, y and z of hits of grouped to the same cluster." ,
			       _maxDist ,
			       _maxDist);

    _clusterEnergy.push_back(1.);
    _clusterEnergy.push_back(30*150);
    registerProcessorParameter( "ClusterEnergy" , 
			       "Minimum and maximum total energy of accepted clusters." ,
			       _clusterEnergy ,
			       _clusterEnergy);

    _maxChi2=1000.;
    registerProcessorParameter( "MaxChi2" , 
			       "Maximum chi^2/n.d.f. of line fits to the x and y coordinates." ,
			       _maxChi2 ,
			       _maxChi2);


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
				_histGroupKey.nameStorage(),
    				_histGroupKey.name() );

    _signalHistPar.clear();
    _signalHistPar.push_back(200);
    _signalHistPar.push_back(0);
    _signalHistPar.push_back(3000);
    
    registerOptionalParameter( "SignalBinning" , 
			       "The binning for the control histograms of the signal distribution." ,
			       _signalHistPar ,
			       _signalHistPar ,
			       _signalHistPar.size() ) ;

    _posHistPar.clear();
    _posHistPar.push_back(400);
    _posHistPar.push_back(-150);
    _posHistPar.push_back(150);
    
    registerOptionalParameter( "PositionBinning" , 
			       "The binning of the histograms of the cluster barycentre." ,
			       _posHistPar ,
			       _posHistPar ,
			       _posHistPar.size() ) ;


    _indexHistPar.clear();
    _indexHistPar.push_back(19);
    _indexHistPar.push_back(-.5);
    _indexHistPar.push_back(19-.5);
    _indexHistPar.push_back(19);
    _indexHistPar.push_back(-.5);
    _indexHistPar.push_back(19-.5);
    
    registerProcessorParameter( "CellIndexBinning" , 
				"The binning of the hit histograms." ,
				_indexHistPar ,
				_indexHistPar ,
				_indexHistPar.size() ) ;


    registerProcessorParameter( "ModuleLocationCollectionName" , 
    				"Name of the conditions data collection which contains the description of the module location (Needed to adapt array sizes)."  ,
				_colNameModuleLocation ,
    				std::string("ModuleLocation") ) ;

#endif

  }

  void MipSelect::init() {
    printParameters();

#ifdef WITH_CONTROL_HISTOGRAMS
    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleLocationChange ,_colNameModuleLocation)) {
      std::stringstream message;
      message << "MipSelect::init> no conditions data handler for the collection:" << _colNameModuleLocation << ".";
      throw std::runtime_error(message.str());
    }

    assert ( _nHits.size() == 2 );
    assert ( _clusterEnergy.size() == 2 );

    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();

    _histCol[kH1NHits]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterNHits"),2,
						       HistPar(50,-.5,50-.5));
    _histCol[kH1ClusterSignal]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterSignal"),1,
							HistPar(static_cast<UInt_t>(_signalHistPar[0]), _signalHistPar[1], _signalHistPar[2]));
    _histCol[kH1SignalVsLayer]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterTotalSignalPerLayer"),1,
							       HistPar(_lastLayer+1, -.5, _lastLayer+.5),true);
    _histCol[kH1HitX]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterHitX"),_lastLayer+1,
						      HistPar(static_cast<unsigned int>(_indexHistPar[0]),_indexHistPar[1],_indexHistPar[2]),true);
    _histCol[kH1HitY]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterHitY"),_lastLayer+1,
						      HistPar(static_cast<unsigned int>(_indexHistPar[3]),_indexHistPar[4],_indexHistPar[5]),true);
    _histCol[kH1Mean]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterBaryCentre"),3,
							HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]));
    _histCol[kH1RMS]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterRMS"),3,
						     HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]));
    _histCol[kH1Angle]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterAngle"),2,
						       HistPar(static_cast<UInt_t>(500), -50, +50));
    _histCol[kH1Chi2]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("LineFitChi2"),2,
						       HistPar(100,0.,20.));
    _histCol[kH1ResidualX]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ResidualsX"),1,
							   HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]),true);
    _histCol[kH1ResidualY]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ResidualsY"),1,
							   HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]),true);

    histogramList->create2DHistograms(_histGroupKey,_histHitPosKey, 1,  
				      HistPar(static_cast<unsigned int>(_indexHistPar[0]),_indexHistPar[1],_indexHistPar[2]),
				      HistPar(static_cast<unsigned int>(_indexHistPar[3]),_indexHistPar[4],_indexHistPar[5]),
				      true);
    // verify code sanity: Are all the histograms created?
    assert(kNH1==11);
#endif

  }

  void MipSelect::processEvent( LCEvent * evtP ) {
    if (evtP) {
      try {
	LCCollection* col_hits = evtP->getCollection( _colName ) ;
	//	_isolatedHits.clear();

#ifdef WITH_CONTROL_HISTOGRAMS
	histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
	histmgr::Histogram2DCollection_t & hit_map_h2=histogramList->getHistogram2DCollection(_histGroupKey,_histHitPosKey);
#endif

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
	// TODO:

	//	std::cout << evtP->getEventNumber() << std::endl;
	//	std::cout << "n_clusters=" << cluster.size() << std::endl;
	//	for(std::vector<Box>::iterator cluster_iter=cluster.begin(); cluster_iter!=cluster.end(); cluster_iter++) {
	//	  cluster_iter->show();
	//	}
      
	//	_isolatedHits.clear();
	
	// --- final step: create Cluster collection
	EVENT::LCCollection* col_cluster=new IMPL::LCCollectionVec( EVENT::LCIO::CLUSTER );
	for(std::vector<Box>::const_iterator cluster_iter=cluster.begin(); cluster_iter!=cluster.end(); cluster_iter++) {
	  if (!cluster_iter->isMerged()) {
#ifdef WITH_CONTROL_HISTOGRAMS
	    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1NHits]->getElementAt(0))->fill(cluster_iter->_hits.size());
#endif
	    if (   cluster_iter->_hits.size() >= static_cast<UInt_t>(_nHits[0]) 
		&& cluster_iter->_hits.size() <  static_cast<UInt_t>(_nHits[1]) ) {
	      IMPL::ClusterImpl *a_cluster=new IMPL::ClusterImpl;
	      Double_t sum[3]={0.,0.,0.};
	      Double_t sum2[3]={0.,0.,0.};
	      Double_t sum_of_weights=0.;
	      Double_t total_energy=0.;
	      UInt_t   n_weights=0;

	      LinearRegression linreg[2];

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
		    linreg[0].add((*hit_iter)->getPosition()[2],(*hit_iter)->getPosition()[0],weight);
		    linreg[1].add((*hit_iter)->getPosition()[2],(*hit_iter)->getPosition()[1],weight);
		    sum_of_weights+=weight;
		    n_weights++;
		  }
		}

		a_cluster->addHit(const_cast<EVENT::CalorimeterHit *>(*hit_iter),1.);
	      }

	      if (   n_weights >= static_cast<UInt_t>(_nHits[0]) 
		  && n_weights <  static_cast<UInt_t>(_nHits[1]) ) {
#ifdef WITH_CONTROL_HISTOGRAMS
		dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1ClusterSignal]->getElementAt(0))->fill(total_energy);
		std::vector<Double_t> energy_per_layer;
		energy_per_layer.resize(_lastLayer+1);
		std::fill(energy_per_layer.begin(),energy_per_layer.end(),0.);
#endif
		
		if (total_energy >= _clusterEnergy[0] && total_energy < _clusterEnergy[1]) {
		  Float_t pos[3];
		  EVENT::FloatVec pos_err;
		  pos_err.resize(6,0.);
		  linreg[0].calc();
		  linreg[1].calc();
		  
		  Double_t chi2=0;
		  Double_t sum_of_weights=0;
		  UInt_t   n_weights=0;
		  for (std::vector<const EVENT::CalorimeterHit *>::const_iterator hit_iter=cluster_iter->_hits.begin();
		       hit_iter != cluster_iter->_hits.end();
		       hit_iter++) {
		    
		    Double_t energy=(*hit_iter)->getEnergy();
		    Double_t weight=_w0+std::log(energy/total_energy);
		    
		    if (energy>0 && weight>0) {
		      Double_t res_x=(*hit_iter)->getPosition()[0]-linreg[0].eval((*hit_iter)->getPosition()[2]);
		      Double_t res_y=(*hit_iter)->getPosition()[1]-linreg[1].eval((*hit_iter)->getPosition()[2]);

#ifdef WITH_CONTROL_HISTOGRAMS
		      CALICE::CellIndex cell_index((*hit_iter)->getCellID0());
		      UInt_t layer_index=cell_index.getLayerIndex();
		      UInt_t cell_x=(cell_index.getWaferColumn())*6+(cell_index.getPadColumn()-1);
		      UInt_t cell_y=(cell_index.getWaferRow()-1)*6+(cell_index.getPadRow()-1);
		      if (layer_index>static_cast<UInt_t>(_histCol[kH1ClusterSignal]->getNumberOfElements())) {
#ifdef BOUNDARY_CHECK
			assert( _histCol[kH1ClusterSignal]->getNumberOfElements()>0 );
#endif
			layer_index = _histCol[kH1ClusterSignal]->getNumberOfElements()-1;
		      }
#ifdef BOUNDARY_CHECK
		      assert( layer_index <= _lastLayer && layer_index>0);
#endif
		      dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1HitX]->getElementAt(layer_index))->fill(cell_x, weight);
		      dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1HitY]->getElementAt(layer_index))->fill(cell_y, weight);
		      hit_map_h2.histogram(0)->fill(cell_x,cell_y,weight);

		    energy_per_layer[layer_index]+=(*hit_iter)->getEnergy();
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1ResidualX]->getElementAt(layer_index))->fill(res_x);
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1ResidualY]->getElementAt(layer_index))->fill(res_y);
#endif
		      chi2+=sqr(res_x/weight);
		      chi2+=sqr(res_y/weight);
		      sum_of_weights+=weight;
		      n_weights++;
		    }
		  }
		  
		  if (n_weights>0) {
		    chi2 /= (sum_of_weights-sum_of_weights*2/n_weights);
#ifdef WITH_CONTROL_HISTOGRAMS
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1Chi2]->getElementAt(0))->fill(chi2);
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1Angle]->getElementAt(0))->fill(atan(linreg[0].ascent())*180/M_PI);
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1Angle]->getElementAt(1))->fill(atan(linreg[1].ascent())*180/M_PI);
		    for (UInt_t layer_i=0; layer_i<=_lastLayer; layer_i++) {
		      dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1SignalVsLayer]->getElementAt(0))->fill(layer_i,energy_per_layer[layer_i]);
		    }
#endif
		  }
		  
		  if (sum_of_weights>0. && chi2<_maxChi2) {
		    
#ifdef WITH_CONTROL_HISTOGRAMS
		    dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1NHits]->getElementAt(0))->fill(n_weights);
#endif
		    for (UInt_t coord_i=0; coord_i<3; coord_i++) {
		      Double_t mean=sum[coord_i]/sum_of_weights;
		      sum2[coord_i]=sqrt((sum2[coord_i]-mean*sum[coord_i])/(sum_of_weights));
		      pos[coord_i]=mean;
#ifdef WITH_CONTROL_HISTOGRAMS
#ifdef BOUNDARY_CHECK
		      assert(_histCol[kH1Mean]->getNumberOfElements()==3);
#endif		    
		      dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1Mean]->getElementAt(coord_i))->fill(mean);
		      dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1RMS]->getElementAt(coord_i))->fill(sum2[coord_i]);
#endif
		    }
		    
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
		    continue;
		  }
		}
	      }
	      delete a_cluster;
	    }
	  }
	}
	evtP->addCollection(col_cluster, _clusterColName);
	
      }
      catch (lcio::DataNotAvailableException err) {
      }
    }
  }

#ifdef WITH_CONTROL_HISTOGRAMS
  void MipSelect::moduleLocationChanged(lcio::LCCollection* col) {
    UInt_t last_layer=0;
    for (UInt_t elm_i=0; elm_i<static_cast<UInt_t>(col->getNumberOfElements()); elm_i++) {
      CALICE::ModuleLocation location(col->getElementAt(elm_i));
      CALICE::CellIndex cell_index(location.getCellIndexOffset());
      if (cell_index.getLayerIndex()>last_layer) {
	last_layer=cell_index.getLayerIndex();
      }
    }
    _lastLayer=last_layer;
    
    // recreate per layer histograms:
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();

    _histCol[kH1ResidualX]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ResidualsX"),last_layer+1,
							   HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]),true);
    _histCol[kH1ResidualY]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ResidualsY"),last_layer+1,
							   HistPar(static_cast<UInt_t>(_posHistPar[0]), _posHistPar[1], _posHistPar[2]),true);
    _histCol[kH1SignalVsLayer]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterTotalSignalPerLayer"),1,
							       HistPar(last_layer+1, -.5, last_layer+.5),true);
    _histCol[kH1HitX]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterHitX"),last_layer+1,
						      HistPar(18,-.5,18-.5),true);
    _histCol[kH1HitY]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("ClusterHitY"),last_layer+1,
						      HistPar(18,-.5,18-.5),true);

  }
#endif

  void MipSelect::end() {
  }

}
