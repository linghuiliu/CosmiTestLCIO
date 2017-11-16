#include <HoldScanAnalysis.hh>
#include <EVENT/LCCollection.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/Cluster.h>
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif
#include <string>
#include <stdexcept>
#include <marlin/ConditionsProcessor.h>
#include <histmgr/HistMgr.hh>
#include <histmgr/HistPar.hh>
#include <histmgr/FloatHistogram1D.hh>
#include <collection_names.hh>

#include <FeConfigurationBlock.hh>

#ifdef BOUNDARY_CHECK
#include <cassert>
#endif

namespace CALICE {

  // create instance to make this Processor known to Marlin
  HoldScanAnalysis a_HoldScanAnalysis_instance;
  
  HoldScanAnalysis::HoldScanAnalysis() 
    : VRawADCValueProcessor("HoldScanAnalysis"),
      _histGroupKey(type()),
      _feConfChange(this, &HoldScanAnalysis::feConfChanged ),
      _nHist(0)
      //      _experimentalSetupChange(this,&HoldScanAnalysis::experimentalSetupChanged)
  {

    _description = "HoldScanAnalysis" ;


    _signalHistPar.clear();
    _signalHistPar.push_back(1000);
    _signalHistPar.push_back(0);
    _signalHistPar.push_back(3000);
    registerOptionalParameter( "SignalBinning" , 
			       "The binning of the Signal histograms of individual hits" ,
			       _signalHistPar ,
			       _signalHistPar ,
			       _signalHistPar.size() ) ;

#ifdef WITH_CONTROL_HISTOGRAMS
    _clusterSignalHistPar.clear();
    _clusterSignalHistPar.push_back(1000);
    _clusterSignalHistPar.push_back(0);
    _clusterSignalHistPar.push_back(3000);
    registerOptionalParameter( "ClusterSignalBinning" , 
			       "The binning of the total cluster signal histogram" ,
			       _clusterSignalHistPar ,
			       _clusterSignalHistPar ,
			       _clusterSignalHistPar.size() ) ;
#endif

    _minNumberOfHits = 3;
    registerProcessorParameter( "MinNumberOfHits" , 
			       "Minimum number of hits of accepted clusters" ,
			       _minNumberOfHits ,
			       _minNumberOfHits) ;

    _minClusterSignal = 30*3;
    registerProcessorParameter( "MinimumSiginal" , 
			       "Minimum total signal of accepted clusters." ,
			       _minClusterSignal ,
			       _minClusterSignal) ;

    _clusterColName="EcalClusters";
    registerProcessorParameter( "ClusterCollectionName" , 
			       "The name of the cluster collection" ,
			       _clusterColName ,
			       _clusterColName);

    _feConfColName=COL_EMC_FE_CONF;
    registerProcessorParameter( "FeCollectionName" , 
			       "The name of the front-end configuration data collection" ,
			       _feConfColName ,
			       _feConfColName);

    registerProcessorParameter( "HistogramGroupName" , 
			       "The name of the histogram group to which the signal histograms will be assigned." ,
				_histGroupKey.nameStorage(),
				type() );
    _nHistMax = 20;
    registerProcessorParameter( "NumberOfHistograms" , 
			       "Maximum number of histograms to be used for signals per hold value." ,
			       _nHistMax ,
			       _nHistMax) ;

    _histKey[kHistSignalPerHoldValue]=histmgr::Key_t("SignalPerHoldValue");
    _histKey[kHistTotalSignalPerHoldValue]=histmgr::Key_t("TotalSignalPerHoldValue");
 }

  HoldScanAnalysis::~HoldScanAnalysis() 
  {
  }

  void HoldScanAnalysis::createHistograms()
  {
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    if (_mapping.getNModules()<=0) {
      histogramList->createHistogramGroup(_histGroupKey);
    }
    else {
      std::cout << "--- " << name() << " Notification:" << std::endl;
      std::cout << " -  create Histograms." << std::endl;
      
      lcio::IntVec n_hist_arr;
      for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
	n_hist_arr.push_back(_nHistMax);
      }
      histogramList->createHistograms(_histGroupKey, _histKey[kHistSignalPerHoldValue], n_hist_arr,
						       HistPar((UInt_t) _signalHistPar[0], _signalHistPar[1], _signalHistPar[2]));
#ifdef WITH_CONTROL_HISTOGRAMS
      histogramList->createHistograms(_histGroupKey, _histKey[kHistTotalSignalPerHoldValue], _nHistMax + 1,
						       HistPar((UInt_t) _clusterSignalHistPar[0], _clusterSignalHistPar[1], _clusterSignalHistPar[2]));
#endif
    }
  }

  void HoldScanAnalysis::init() {
    printParameters();

    createHistograms();

    // empty clusters should not exist anyway.
    if (_minNumberOfHits<=0) _minNumberOfHits=1;

    _nClusters=0;
    _nMissingHoldChanges=0;
    _moduleIndexOutOfRange=0;
    _nBeamOrCosmicsEvents=0;
    _currentCommonHoldStart=static_cast<UInt_t>(-1);

    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
#ifdef WITH_CONTROL_HISTOGRAMS
    // some isolated histograms
    _nHitsHistogram=histogramList->createHistograms(_histGroupKey, histmgr::Key_t("HoldScanClusterNHits"),1,HistPar(200,-.5,200-.5));
    _clusterSignalHistogram=histogramList->createHistograms(_histGroupKey, histmgr::Key_t("HoldScanClusterSignal"),1,
							    HistPar((UInt_t) _clusterSignalHistPar[0], _clusterSignalHistPar[1], 
								    _clusterSignalHistPar[2]));
#endif
    _holdValueContainer=histogramList->createHistograms(_histGroupKey, histmgr::Key_t("HoldValueContainer"),1,HistPar(_nHistMax,-.5,_nHistMax-.5));

    if (!marlin::ConditionsProcessor::registerChangeListener( &_feConfChange,
							      _feConfColName )) {
      std::cout << "HoldScan::init> no conditions data handler for collection " << _feConfColName 
		<< ". The conditions data may be generated by the converter if the native files are read." << std::endl;
    }

    VRawADCValueProcessor::init();
  }

  void HoldScanAnalysis::processEvent( LCEvent * evtP ) {
    if (evtP) {
      try {

	UInt_t reco_state=static_cast<UInt_t>(evtP->getParameters().getIntVal(PAR_RECO_STATE));
	if (reco_state!=kRecoStateBeam && reco_state!=kRecoStateCosmics) return;

	_nBeamOrCosmicsEvents++;

	LCCollection* col_cluster = evtP->getCollection( _clusterColName ) ;
	histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
	histmgr::HistogramCollection_t & signal_per_module_per_hold=histogramList->getHistogramCollection(_histGroupKey,_histKey[kHistSignalPerHoldValue]);
	histmgr::HistogramCollection_t & total_signal_per_module_per_hold=histogramList->getHistogramCollection(_histGroupKey,_histKey[kHistTotalSignalPerHoldValue]);
	if (col_cluster->getNumberOfElements()>0) {

	  for (UInt_t element_i=0; element_i< static_cast<UInt_t>(col_cluster->getNumberOfElements()); element_i++) {
	    EVENT::Cluster *a_cluster=dynamic_cast<EVENT::Cluster *>(col_cluster->getElementAt(element_i));
	    if (a_cluster) {

	      Double_t energy_sum=0;
	      const EVENT::CalorimeterHitVec &a_hit_col=a_cluster->getCalorimeterHits();
	      if (a_hit_col.size() >= static_cast<UInt_t>(_minNumberOfHits)) {
		for (EVENT::CalorimeterHitVec::const_iterator hit_iter=a_hit_col.begin();
		     hit_iter!=a_hit_col.end();
		     hit_iter++) {
		  //		  CellIndex geom_cell_index((*hit_iter)->getCellID0());
		  //std::pair<UInt_t,UInt_t> module_and_cell_index=_indexLookup.getModuleAndCellIndex(_mapping,geom_cell_index);
		  if ((*hit_iter)->getEnergy()>0.) {
		    
		    energy_sum+=(*hit_iter)->getEnergy();
		  }
		}
		
#ifdef WITH_CONTROL_HISTOGRAMS
		dynamic_cast<histmgr::FloatHistogram1D*>(_clusterSignalHistogram->getElementAt(0))->fill(energy_sum);
#endif
		if (energy_sum>_minClusterSignal) {
#ifdef WITH_CONTROL_HISTOGRAMS
		  dynamic_cast<histmgr::FloatHistogram1D*>(_nHitsHistogram->getElementAt(0))->fill(a_hit_col.size());
#endif
		  for (EVENT::CalorimeterHitVec::const_iterator hit_iter=a_hit_col.begin();
		       hit_iter!=a_hit_col.end();
		       hit_iter++) {
		    CellIndex geom_cell_index((*hit_iter)->getCellID0());
		    
		    std::pair<UInt_t, UInt_t> module_and_cell_index=_indexLookup.getModuleAndCellIndex(_mapping,geom_cell_index);
		    if (module_and_cell_index.first >= _histogramIndex.size()) {
		      _moduleIndexOutOfRange++;
		    }
		    else {
		      signal_per_module_per_hold.histogram(module_and_cell_index.first,_histogramIndex[module_and_cell_index.first])->fill((*hit_iter)->getEnergy());
		    }
		  }
		  if (_currentCommonHoldStart<static_cast<UInt_t>(-1)) {
		    total_signal_per_module_per_hold.histogram(_currentCommonHoldStart)->fill(energy_sum);
		  }
		  _nClusters++;
		}
	      }
	    }
	  }
	}
      }
      catch (lcio::DataNotAvailableException &err) {
      }
    }
  }

  void HoldScanAnalysis::moduleTypeChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleTypeChanged(col);
    if (_mapping.isModuleConditionsDataComplete()) {
      _indexLookup.createIndexReverseLookup(_mapping);
      createHistograms();
    }
  }

  void HoldScanAnalysis::moduleLocationChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleLocationChanged(col);
    if (_mapping.isModuleConditionsDataComplete()) {
      _indexLookup.createIndexReverseLookup(_mapping);
      createHistograms(); 
    }
  }

  void HoldScanAnalysis::moduleConnectionChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleConnectionChanged(col);
    if (_mapping.isModuleConditionsDataComplete()) {
      _indexLookup.createIndexReverseLookup(_mapping);
      createHistograms();
    }
  }


  void HoldScanAnalysis::end() 
  {
    std::cout << "--- " << name() << " Report:" << std::endl
              << std::setw(8) << _nClusters << " clusters used." << std::endl
	      << std::setw(8) << _moduleIndexOutOfRange << " hits for which the module index was out of the allowed range." << std::endl
	      << std::setw(8) << _nMissingHoldChanges << " configuration changes in which hold values were changed only for some modules." << std::endl
	      << std::setw(8) << _nBeamOrCosmicsEvents << " beam or cosmics events." << std::endl
	      << std::endl;

    // write out the mean signal 
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & signal_per_module_per_hold=histogramList->getHistogramCollection(_histGroupKey,_histKey[kHistSignalPerHoldValue]);
    for (UInt_t module_i=0; module_i<signal_per_module_per_hold.nMajor(); module_i++) {
      std::cout << "Module " << module_i << ":" << std::endl;
      for (std::map<int,UInt_t>::const_iterator hist_iter=_holdStartMap.begin();
	   hist_iter!=_holdStartMap.end();
	   hist_iter++) {
	//      std::cout << hist_iter->first << ":" << std::endl;
	histmgr::FloatHistogram1D *histP=signal_per_module_per_hold.histogram(module_i,hist_iter->second);
	std::cout << module_i << ":"   << hist_iter->first << ":" << histP->mean() << "+-" << histP->rms() << std::endl;
      }
      std::cout << std::endl;
    }

    std::cout << std::endl;

  }

  void HoldScanAnalysis::feConfChanged(EVENT::LCCollection *col) 
  {
    _histogramIndex.resize(_mapping.getNModules(),0);
    UInt_t hold_changes=0;
    Bool_t same_hold=true;
    UInt_t last_hold_start=static_cast<UInt_t>(-1);
    UInt_t last_histogram_index=_nHistMax;
    for (UInt_t fe_i=0; fe_i< static_cast<UInt_t>(col->getNumberOfElements()); fe_i++) {
      CALICE::FeConfigurationBlock fe_conf(col->getElementAt(fe_i));
      std::pair<UInt_t,UInt_t> module_index_left_right=_mapping.getModuleIndex(fe_conf.getCrateID(),fe_conf.getSlotID(),fe_conf.getBoardComponentNumber());
      // DEBUG
      for (UInt_t i=0; i<2; i++) {
	UInt_t module_index=(i==0 ? module_index_left_right.first : module_index_left_right.second);
	if (module_index<_mapping.getNModules()) {
	  UInt_t hold_start=static_cast<UInt_t>(fe_conf.getHoldStart());
	  if (last_hold_start!=hold_start && last_hold_start!=static_cast<UInt_t>(-1)) {
	    same_hold=false;
	  }
	  last_hold_start=hold_start;
	  UInt_t hist_index=0;
	  std::map<int,UInt_t>::const_iterator iter=_holdStartMap.find(hold_start);
	  if (iter == _holdStartMap.end()) {
	    if ( _nHist < static_cast<UInt_t>(_nHistMax) ) {
	      std::pair< std::map<int,UInt_t>::const_iterator, bool > res=_holdStartMap.insert(std::make_pair<int,UInt_t>(hold_start, _nHist++));
	      if (res.second) {
		hist_index = res.first->second;
#ifdef BOUNDARY_CHECK
		assert(_holdValueContainer->getNumberOfElements()==1);
#endif	      
		histmgr::FloatHistogram1D *histP=dynamic_cast<histmgr::FloatHistogram1D*>(_holdValueContainer->getElementAt(0));
		if (hist_index<histP->nBins()) {
		  histP->setBinContent(histP->firstBinIndex()+hist_index,hold_start);
		}
		else {
		  histP->addToBinContent(histP->firstBinIndex(),1);
		}
	      }
	      else {
		_nHist--;
#ifdef BOUNDARY_CHECK
		assert(_holdValueContainer->getNumberOfElements()==1);
#endif	      
		histmgr::FloatHistogram1D *histP=dynamic_cast<histmgr::FloatHistogram1D*>(_holdValueContainer->getElementAt(0));
		histP->addToBinContent(histP->firstBinIndex(),1);
	      }
	    }
	  }
	  else {
	    hist_index=iter->second;
	    last_histogram_index=hist_index;
	  }
	  if (hist_index!=_histogramIndex[module_index]) {
	    //DEBUG	    
	    // 	    std::cout << "hold change:: "
	    // 		      << "crate=" << fe_conf.getCrateID() << " slot=" << fe_conf.getSlotID() << " fe=" << fe_conf.getBoardComponentNumber()
	    // 		      << " label=" << fe_conf.getRecordLabel() << " hold_start=" << fe_conf.getHoldStart() 
	    // 		      << " -> " << hist_index
	    // 		      << std::endl;
	    _histogramIndex[module_index]=hist_index;
	    hold_changes++;
	  }
	}
      }
    }
    _currentCommonHoldStart=(last_histogram_index < static_cast<UInt_t>(_nHistMax) ? last_histogram_index : static_cast<UInt_t>(_nHistMax));
    if (hold_changes!=_mapping.getNModules()) {
      _nMissingHoldChanges++;
    }
  }

}
