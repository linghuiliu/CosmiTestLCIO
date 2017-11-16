#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

//#ifdef HAVE_ROOT
//#include <TFile.h>
//#include <TGraph.h>
//#include <TGraphErrors.h>
//#endif
#include "AverageHistoryGraphs.hh"
#include <AdcValueAccess.hh>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <Average_t.hh>
#include <CellParameter.hh>
#include <EVENT/LCParameters.h>
#include <marlin/ConditionsProcessor.h>
#include <collection_names.hh>
#include <MakeGraph.hh>
#include <histmgr/GraphCollection_t.hh>
#include <histmgr/HistMgr.hh>
#include <ErrorBits.hh>

//#include <sstream>
#define PAR_EVENTS_SINCE_LAST_DAQ_STATE_CHANGE "CALDAQ_EventsSinceLastChange"

namespace CALICE {
  AverageHistoryGraphs a_AverageHistoryGraphs_instance;

  const char *AverageHistoryGraphs::__historyGraphNames[AverageHistoryGraphs::kNHistoryGraphs] ={
    "ADCHistory",
    "NoiseHistory",
    "PedestalHistory",
    "HitHistory"
  };

  const char *AverageHistoryGraphs::__graphTypeNames[AverageHistoryGraphs::kNGraphTypes] ={
    "mean",
    "rms",
    "min",
    "max",
    "weight"
  };

  UInt_t AverageHistoryGraphs::__nSamplesPerChip=18;
  UInt_t AverageHistoryGraphs::__nChips=12;

  AverageHistoryGraphs::AverageHistoryGraphs()
    :    VRawADCValueProcessor("AverageHistoryGraphs"),
	 _stateChangeHistoryKey("StateChangeHistory"),
	 _confChangeHistoryKey("ConfigurationChangeHistory")
  {

    //_outputFileName="history.root";
    //    registerProcessorParameter( "ResultsFile" , 
    //			       "The history graphs will be written to the ROOT file of this name." ,
    //			       _outputFileName ,
    //			       _outputFileName);

    //    _folderName="";
    //    registerProcessorParameter( "HistogramDirectoryName" , 
    //			       "The name of the directory inside the ROOT file to which the history graphs are written to." ,
    //			       _folderName ,
    //			       _folderName);

    _useTimeStamp=0;
    registerProcessorParameter( "UseTime" , 
			       "Use the time stamps instead of the event number." ,
			       _useTimeStamp,
			       _useTimeStamp);

    _cellParameterCollectionName="CellParameters";
    registerProcessorParameter( "CellParameterCollectionName" , 
				"The name of the collection which contains the pedestals, the noise, etc. of all the cells." ,
				_cellParameterCollectionName ,
				_cellParameterCollectionName);

    _monitorConf.clear();
    registerProcessorParameter( "MonitorConditionsChanges" , 
			       "Memorise the event numbers after configuration changes." ,
			       _monitorConf,
			       _monitorConf);

    _signalCut=5.;
    registerProcessorParameter( "SignalCut" , 
				"ADC values of cells with a signal-over-noise larger than this value will not be considered." ,
				_signalCut ,
				_signalCut);

    registerProcessorParameter( "SkipCalibrationEvents" , 
                               "Skip events for which the calibration chip was switched on (Otherwise they are treated as beam events)" ,
                               _skipCalibrationEventsAlways ,
                               0) ;

    _eventPar.clear();
    _eventPar.push_back(100);
    _eventPar.push_back(0);
    _eventPar.push_back(200000);
    registerOptionalParameter( "EventStepsAndRange" , 
    				"Event step size, first event, and last event. The history is extended in events steps given by the first parameter for events in the range indicated by the second and third parameter"  ,
				_eventPar ,
			       _eventPar);

    _avADCNMax=10;
    registerProcessorParameter( "AverageOverNEvents" , 
			       "Calculate per average ADC values per pad using this number of ADC values." ,
			       _avADCNMax,
			       _avADCNMax);

    registerProcessorParameter( "HistogramGroupName" , 
    				"The name of the histogram group under which the control histograms will be registered."  ,
				_graphGroupKey.nameStorage() ,
    				type() );
    
    for (UInt_t history_i=0; history_i<kNHistoryGraphs; history_i++) {
      _avValueHistoryKey.push_back(histmgr::Key_t(__historyGraphNames[history_i]));
    }
  }

  void AverageHistoryGraphs::init() {
    VRawADCValueProcessor::init();
    printParameters();

    assert(_eventPar.size()==3);
    assert(_eventPar[0]>0);
    assert(_eventPar[1]<=_eventPar[2]);

    _nStateChanges=0;
    _wasState=0;

    _confChanged=false;
    std::cout << "Monitor changes of  ";
    for (StringVec::const_iterator name_iter=_monitorConf.begin();
	 name_iter!=_monitorConf.end();
	 name_iter++) {
      ConditionsChangeDelegator<AverageHistoryGraphs> a(this,&AverageHistoryGraphs::configurationChanged);
      _confChanges.push_back( a );
    }

    UInt_t i=0;
    for (StringVec::const_iterator name_iter=_monitorConf.begin();
	 name_iter!=_monitorConf.end();
	 name_iter++, i++) {
      
      marlin::ConditionsProcessor::registerChangeListener( &(_confChanges[i]) ,
							   *name_iter );
      std::cout  << *name_iter << " ";

    }
    std::cout << endl;
    _avADCIsValid=false;
    _avADCn=0;

    EVENT::StringVec empty;
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histogramList->createHistogramGroup(_graphGroupKey);
    histogramList->lockGroup(_graphGroupKey);

    histogramList->createGraphCollection(_graphGroupKey,_stateChangeHistoryKey,1,empty,0,empty,false);
    histogramList->createGraphCollection(_graphGroupKey,_confChangeHistoryKey,1,empty,0,empty,false);

  }

  void AverageHistoryGraphs::resizeArrays() {
    if (!_mapping.isModuleConditionsDataComplete()) return;
    if (_mapping.getNModules()==0) return;

    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();

    Bool_t need_to_resize_array=false;
    assert (_avValueHistoryKey.size()>0 );
    if (_mapping.getNModules()!=_avADC.size() 
	|| !histogramList->histogramGroupExists(_graphGroupKey) 
	|| !histogramList->histogramGroup(_graphGroupKey).histogramCollectionExists(_avValueHistoryKey[0])
	|| !histogramList->histogramGroup(_graphGroupKey).histogramCollection(_avValueHistoryKey[0]).nMajor()==_mapping.getNModules()) {
      need_to_resize_array=true;
    }
    else {
      for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
	if (   _mapping.getNCellsPerModule(module_i) != _avADC[module_i].size()
	    || _mapping.getNCellsPerModule(module_i)/__nSamplesPerChip != histogramList->histogramGroup(_graphGroupKey).histogramCollection(_avValueHistoryKey[0]).nMinor(module_i)) {
	  need_to_resize_array=true;
	}
      }
    }

    if (!need_to_resize_array) return;

    histogramList->writeHistograms(_graphGroupKey, true);
    // writeGraphs();

    _avADC.clear();
    _avADC.resize(_mapping.getNModules());
    for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
      _avADC[module_i].resize(_mapping.getNCellsPerModule(module_i));
    }
    _avADCIsValid=false;
    _avADCn=0;
    for (UInt_t history_i=0; history_i<kNHistoryGraphs; history_i++) {
      _avValues[history_i].clear();
      _avValues[history_i].resize(_mapping.getNModules());
      for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
	_avValues[history_i][module_i].resize(_mapping.getNCellsPerModule(module_i)/__nSamplesPerChip);
      }
    }


    EVENT::StringVec types;
    for (UInt_t type_i=0; type_i<kNGraphTypes; type_i++) {
      types.push_back(__graphTypeNames[type_i]);
    }
    
    EVENT::IntVec n_chips;
    for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
      n_chips.push_back(_mapping.getNCellsPerModule(module_i)/__nSamplesPerChip);
    }
    EVENT::StringVec empty;
 
    for (std::vector<histmgr::Key_t>::const_iterator history_iter=_avValueHistoryKey.begin();
	 history_iter!=_avValueHistoryKey.end();
	 history_iter++) {

      EVENT::StringVec module_names;
      for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
	std::stringstream a_name;
	CellIndex cell_index(_mapping.getGeometricalCellIndex(module_i,0));
	a_name << history_iter->name() << "_module_" << module_i << "_" << _mapping.getModuleName(module_i) << "_layer_" << cell_index.getLayerIndex();
	module_names.push_back(a_name.str());
      }

      histogramList->createGraphCollection(_graphGroupKey,*history_iter,n_chips,types,0,module_names,true);
    }
    
    //    for (UInt_t history_i=0; history_i<kNHistoryGraphs; history_i++) {
    //      for (UInt_t graph_i=0; graph_i<kNGraphs; graph_i++) {
    //	_history[history_i][graph_i].clear();
    //	_history[history_i][graph_i].resize(_mapping.getNModules());
    //	for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
    //	  _history[history_i][graph_i][module_i].resize(_mapping.getNCellsPerModule(module_i)/__nSamplesPerChip);
    //	}
    //      }
    //    }
    //
    //    for (UInt_t hist_i=0; hist_i<kNHistoryStamps; hist_i++) {
    //      _historyStamps[hist_i].clear();
    //    }

  }

  void AverageHistoryGraphs::processEvent( LCEvent * evtP ) {

    CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
	
    UInt_t event_nr=evtP->getEventNumber();
    Int_t isState=evtP->parameters().getIntVal(PAR_RECO_STATE);

    UInt_t events_since_last_slow_conf_record=evtP->parameters().getIntVal(PAR_EVENTS_SINCE_LAST_DAQ_STATE_CHANGE);
    
    //FIXME: does not make any sense if runs are chained.
    if (event_nr>=static_cast<UInt_t>(_eventPar[1]) && event_nr<=static_cast<UInt_t>(_eventPar[2])) {
      
      histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
      // FIXME: cannot store nanosecond resolution !
      Float_t stamp=(_useTimeStamp>0 ? static_cast<Float_t>(evtP->getTimeStamp()) : static_cast<Float_t>(event_nr));
      
      if (error.corruptEventRecord() || (_skipCalibrationEventsAlways!=0 && isState==kRecoStateCalibration)) {
        isState=(error.corruptEventRecord() ? -2 : -1 );
	if (_wasState != isState ) {
	  histmgr::GraphCollection_t &graph_col=histogramList->getGraphCollection(_graphGroupKey,_stateChangeHistoryKey);
	  UInt_t val_i = graph_col.appendXValue(stamp);
	  graph_col.setYValue(0,0,val_i, static_cast<float>(isState));
	  _nStateChanges++;
	  //	  _historyStamps[kHistoryStateStamp].push_back(stamp);
	  //	  _historyStamps[kHistoryStateValue].push_back( isState );
        }
        _wasState=isState;
   
	return;
      }
      
      if (_wasState==isState && events_since_last_slow_conf_record==0) {
	isState=-5;
      }

      if (_wasState != isState ) {
	_nStateChanges++;
	histmgr::GraphCollection_t &graph_col=histogramList->getGraphCollection(_graphGroupKey,_stateChangeHistoryKey);
	UInt_t val_i = graph_col.appendXValue(stamp);
	graph_col.setYValue(0,0,val_i, static_cast<float>(isState));

	//_historyStamps[kHistoryStateStamp].push_back(stamp);
	//_historyStamps[kHistoryStateValue].push_back(isState);
	_wasState=isState;
      }
      if (_confChanged) {
	histmgr::GraphCollection_t &graph_col=histogramList->getGraphCollection(_graphGroupKey,_confChangeHistoryKey);
	UInt_t val_i = graph_col.appendXValue(stamp);
	graph_col.setYValue(0,0,val_i, 0.);
	//	_historyStamps[kHistoryConfStamp].push_back(stamp);
      }
      
      LCCollection* cell_parameters=0;
      IntVec module_index_array;
      try {
	cell_parameters = evtP->getCollection( _cellParameterCollectionName ) ;
	cell_parameters->getParameters().getIntVals(std::string("ModuleStartIndex"),module_index_array);
	
      }
      catch (  DataNotAvailableException &err) {
      }
      
      
      try {
	LCCollection* col_adc = evtP->getCollection( _adcColName ) ;
	//TODO: not very efficient to copy the int vector every time. In particular, since it rarly changes.
	if (col_adc && col_adc->getNumberOfElements()>0) {
	    
	    AdcValueAccess adc_access(col_adc, &_mapping, 0, 0);
	    if (adc_access.hasConnectedBlocks()) {
	      if (!_avADCIsValid) {
		do {
#ifdef BOUNDARY_CHECK
		  assert(	adc_access.getModuleIndex()< _avADC.size());
#endif		 
		  
		  do {
		    UInt_t cell_i=adc_access.getCellIndexOnModule();
#ifdef BOUNDARY_CHECK
		    assert(cell_i < _avADC[adc_access.getModuleIndex()].size());
#endif		    
		    Int_t raw_adc_value=adc_access.getAdcValue();
		    
		    _avADC[adc_access.getModuleIndex()][cell_i].add(raw_adc_value);
		  } while (adc_access.nextValue());
		} while (adc_access.nextBlock());

		if (_avADCn++>static_cast<UInt_t>(_avADCNMax)) {
		  for (std::vector< std::vector<Average_t> >::iterator module_iter=_avADC.begin();
		       module_iter!=_avADC.end();
		       module_iter++) {
		    for (std::vector<Average_t>::iterator cell_iter=module_iter->begin();
			 cell_iter!=module_iter->end();
			 cell_iter++) {
		      cell_iter->calculate();
		    }
		  }
		  _avADCIsValid=true;
		}
	      }
	      else {
		do {
#ifdef BOUNDARY_CHECK
		  assert(	adc_access.getModuleIndex()< _avValues[kNoiseHistory].size());
#endif		 
		  UInt_t module_index_offset=UINT_MAX;
		  if (module_index_array.size()>0) {
		    UInt_t module_index=adc_access.getModuleIndex();
		    //		  UInt_t cell_index=adc_access.getCellIndexOnModule();
		    if ((module_index_array.size()) >static_cast<UInt_t>(module_index) + 1) {
		      module_index_offset=static_cast<UInt_t>(module_index_array[module_index]);
		    }
		  }

		  do {
		    UInt_t chip_i=adc_access.getCellIndexOnModule()%__nChips;
#ifdef BOUNDARY_CHECK
		    assert(chip_i < _avValues[kNoiseHistory][adc_access.getModuleIndex()].size());
#endif		    
		    Int_t raw_adc_value=adc_access.getAdcValue();

		    Float_t noise=FLT_MAX;
		    Float_t pedestal=0;
		    if (module_index_offset<UINT_MAX) {
		      CellParameter cell_parameter(cell_parameters->getElementAt(module_index_offset+adc_access.getCellIndexOnModule()));
		      noise=cell_parameter.getNoise();
		      pedestal=cell_parameter.getPedestal();
		      _avValues[kNoiseHistory][adc_access.getModuleIndex()][chip_i].add(noise);
		      _avValues[kPedestalHistory][adc_access.getModuleIndex()][chip_i].add(pedestal);
		    }
		    if (raw_adc_value-pedestal<_signalCut*noise) {
		      _avValues[kADCHistory][adc_access.getModuleIndex()][chip_i].add(raw_adc_value-_avADC[adc_access.getModuleIndex()][adc_access.getCellIndexOnModule()].mean());
		    }
		    else {
		      _avValues[kHitHistory][adc_access.getModuleIndex()][chip_i].add(1);
		    }
		  } while (adc_access.nextValue());
		} while (adc_access.nextBlock());
	      }
	    }
	}
	
	if (event_nr%static_cast<UInt_t>(_eventPar[0])==0) {
	  if (_avADCIsValid) {
	    for (UInt_t history_i=0; history_i<kNHistoryGraphs; history_i++) {
#ifdef BOUNDARY_CHECK
	      assert( history_i<_avValueHistoryKey.size() );
#endif
	      histmgr::GraphCollection_t &graph_col=histogramList->getGraphCollection(_graphGroupKey,_avValueHistoryKey[history_i]);
	      UInt_t history_val_i = graph_col.appendXValue(stamp);

	      for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
		for (UInt_t chip_i=0; chip_i<_mapping.getNCellsPerModule(module_i)/__nSamplesPerChip; chip_i++) {
		  if (_avValues[history_i][module_i][chip_i].weight()>=0) {
		    _avValues[history_i][module_i][chip_i].calculate();

		    graph_col.setYValue(module_i,chip_i,kGraphMean,history_val_i, _avValues[history_i][module_i][chip_i].mean());
		    graph_col.setYValue(module_i,chip_i,kGraphRMS,history_val_i, _avValues[history_i][module_i][chip_i].sigma());
		    graph_col.setYValue(module_i,chip_i,kGraphMin,history_val_i, _avValues[history_i][module_i][chip_i].min());
		    graph_col.setYValue(module_i,chip_i,kGraphMax,history_val_i, _avValues[history_i][module_i][chip_i].max());
		    graph_col.setYValue(module_i,chip_i,kGraphWeight,history_val_i, _avValues[history_i][module_i][chip_i].weight());

// 		    _history[history_i][kGraphMean][module_i][chip_i].push_back(_avValues[history_i][module_i][chip_i].mean());
// 		    _history[history_i][kGraphRMS][module_i][chip_i].push_back(_avValues[history_i][module_i][chip_i].sigma());
// 		    _history[history_i][kGraphMin][module_i][chip_i].push_back(_avValues[history_i][module_i][chip_i].min());
// 		    _history[history_i][kGraphMax][module_i][chip_i].push_back(_avValues[history_i][module_i][chip_i].max());
// 		    _history[history_i][kGraphWeight][module_i][chip_i].push_back(_avValues[history_i][module_i][chip_i].weight());
		  }
		  else if (history_val_i>0) {
		    graph_col.setYValue(module_i,chip_i,kGraphMean,history_val_i,graph_col.getYValue(module_i,chip_i,kGraphMean,history_val_i-1));
		    graph_col.setYValue(module_i,chip_i,kGraphRMS,history_val_i,graph_col.getYValue(module_i,chip_i,kGraphRMS,history_val_i-1));
		    graph_col.setYValue(module_i,chip_i,kGraphMin,history_val_i,graph_col.getYValue(module_i,chip_i,kGraphMin,history_val_i-1));
		    graph_col.setYValue(module_i,chip_i,kGraphMax,history_val_i,graph_col.getYValue(module_i,chip_i,kGraphMax,history_val_i-1));
		    graph_col.setYValue(module_i,chip_i,kGraphWeight,history_val_i,graph_col.getYValue(module_i,chip_i,kGraphWeight,history_val_i-1));
// 		    _history[history_i][kGraphMean][module_i][chip_i].push_back(_history[history_i][kGraphMean][module_i][chip_i].back());
// 		    _history[history_i][kGraphRMS][module_i][chip_i].push_back(_history[history_i][kGraphRMS][module_i][chip_i].back());
// 		    _history[history_i][kGraphMin][module_i][chip_i].push_back(_history[history_i][kGraphMin][module_i][chip_i].back());
// 		    _history[history_i][kGraphMax][module_i][chip_i].push_back(_history[history_i][kGraphMax][module_i][chip_i].back());
// 		    _history[history_i][kGraphWeight][module_i][chip_i].push_back(_history[history_i][kGraphWeight][module_i][chip_i].back());
		  }
		  else {
		    graph_col.setYValue(module_i,chip_i,kGraphMean,history_val_i,0.);
		    graph_col.setYValue(module_i,chip_i,kGraphRMS,history_val_i,0.);
		    graph_col.setYValue(module_i,chip_i,kGraphMin,history_val_i,0.);
		    graph_col.setYValue(module_i,chip_i,kGraphMax,history_val_i,0.);
		    graph_col.setYValue(module_i,chip_i,kGraphWeight,history_val_i,0.);
// 		    _history[history_i][kGraphMean][module_i][chip_i].push_back(0.);
// 		    _history[history_i][kGraphRMS][module_i][chip_i].push_back(0.);
// 		    _history[history_i][kGraphMin][module_i][chip_i].push_back(0.);
// 		    _history[history_i][kGraphMax][module_i][chip_i].push_back(0.);
// 		    _history[history_i][kGraphWeight][module_i][chip_i].push_back(0.);
		  }
		  _avValues[history_i][module_i][chip_i]=Average_t();
		}
	      }
	    }
	    //	    _historyStamps[kHistoryStamp].push_back(stamp);
	  }
	}
      }
      catch (  DataNotAvailableException &err) {
      }
    }
    _confChanged=false;
  }

  void AverageHistoryGraphs::end() 
  {
    //    std::cout << "---- AverageHistoryGraphs: report>" << std::endl;
    //    std::cout << "\t" << _historyStamps[kHistoryStamp].size() << " : number of events per graph." << std::endl;
    //    writeGraphs();
  }

//   void AverageHistoryGraphs::writeGraphs() {
// #ifdef HAVE_ROOT
//     Bool_t have_events=false;
//     for (UInt_t hist_i=0; hist_i<kNHistoryStamps; hist_i++) {
//       if (_historyStamps[hist_i].size()>0 ) {
// 	have_events=true;
// 	break;
//       }
//     }
//     if (!have_events) return;


//     TFile a_file(_outputFileName.c_str(),"UPDATE");
    
//     TDirectory *folder_dir=&a_file;
//     if (!_folderName.empty()) {
//       TObject *obj=a_file.Get(_folderName.c_str());
//       if (obj && obj->InheritsFrom(TDirectory::Class())) {
// 	TDirectory *a_dir=(TDirectory *) obj;
// 	a_dir->cd();
// 	folder_dir=a_dir;
//       }
//       else {
// 	a_file.mkdir(_folderName.c_str());

// 	TObject *obj=a_file.Get(_folderName.c_str());
// 	if (obj && obj->InheritsFrom(TDirectory::Class())) {
// 	  TDirectory *a_dir=(TDirectory *) obj;
// 	  a_dir->cd();
// 	  folder_dir=a_dir;
// 	}
//       }
//     }
    
//     UInt_t n_written_graphs=0;
//     {
//       if (_historyStamps[kHistoryConfStamp].size()>0) {
// 	vector<unsigned int> yarr;
// 	for (vector<Double_t>::const_iterator point_iter=_historyStamps[kHistoryConfStamp].begin();
// 	     point_iter!=_historyStamps[kHistoryConfStamp].end();
// 	     point_iter++) {
// 	  yarr.push_back(0);
// 	}
// 	TGraph *a_graph=makeGraph("configChanges",_historyStamps[kHistoryConfStamp],yarr);
// 	if (a_graph) {
// 	  a_graph->Write();
// 	  n_written_graphs++;
// 	}
//       }
//       if (_historyStamps[kHistoryStateStamp].size()>0) {
// 	TGraph *a_graph=makeGraph("stateChanges",_historyStamps[kHistoryStateStamp],_historyStamps[kHistoryStateValue]);
// 	if (a_graph) {
// 	  a_graph->Write();
// 	  n_written_graphs++;
// 	}
//       }
//     }
//     if (_historyStamps[kHistoryStamp].size()>0) {
//       for (UInt_t history_i=0; history_i<kNHistoryGraphs; history_i++) {
	
// 	if (folder_dir) {
// 	  folder_dir->cd();
// 	  TObject *obj=folder_dir->Get(__historyGraphNames[history_i]);
// 	  if (obj && obj->InheritsFrom(TDirectory::Class())) {
// 	    TDirectory *a_dir=(TDirectory *) obj;
// 	    a_dir->cd();
// 	  }
// 	  else {
// 	    folder_dir->mkdir(__historyGraphNames[history_i]);
	    
// 	    TObject *obj=folder_dir->Get(__historyGraphNames[history_i]);
// 	    if (obj && obj->InheritsFrom(TDirectory::Class())) {
// 	      TDirectory *a_dir=(TDirectory *) obj;
// 	      a_dir->cd();
// 	    }
// 	  }
// 	}

// 	for (UInt_t graph_i=0; graph_i<kNGraphs; graph_i++) {
// 	  if (graph_i==kGraphRMS) continue;
// 	  UInt_t module_i=0;
// 	  for (std::vector< std::vector< std::vector< Float_t> > >::const_iterator arr_iter=_history[history_i][graph_i].begin();
// 	       arr_iter!=_history[history_i][graph_i].end();
// 	       arr_iter++,module_i++) {
// 	    UInt_t chip_i=0;
// 	    for (std::vector< std::vector< Float_t> >::const_iterator iter=arr_iter->begin();
// 		 iter!=arr_iter->end();
// 		 iter++,chip_i++) {
// #ifdef BOUNDARY_CHECK
// 	      assert(iter->size()==_historyStamps[kHistoryStamp].size() );
// #endif
// 	      TGraph *a_graph=0;
// 	      std::stringstream name;
// 	      name << __historyGraphNames[history_i] << "_" << __graphNames[graph_i] << module_i << "_" << chip_i;
// 	      if (graph_i==kGraphMean) {
// 		a_graph=makeGraph(name.str().c_str(),_historyStamps[kHistoryStamp],(*iter),
// 				  _history[history_i][kGraphRMS][module_i][chip_i]);
// 	      }
// 	      else {
// 		a_graph=makeGraph(name.str().c_str(),_historyStamps[kHistoryStamp],(*iter));
// 	      }
// 	      if (a_graph) {
// 		a_graph->Write();
// 		n_written_graphs++;
// 		delete a_graph;
// 	      }
// 	    }
// 	  }
// 	}
//       }
//     }
//     {
//       for (UInt_t history_i=0; history_i<kNHistoryGraphs; history_i++) {
// 	_historyStamps[history_i].clear();
//       }
//       for (UInt_t history_i=0; history_i<kNHistoryGraphs; history_i++) {
// 	for (UInt_t graph_i=0; graph_i<kNGraphs; graph_i++) {
// 	  for (std::vector< std::vector< std::vector< Float_t> > >::iterator arr_iter=_history[history_i][graph_i].begin();
// 	       arr_iter!=_history[history_i][graph_i].end();
// 	       arr_iter++) {
// 	    for (std::vector< std::vector< Float_t> >::iterator iter=arr_iter->begin();
// 		 iter!=arr_iter->end();
// 		 iter++) {
// 	      iter->clear();
// 	    }
// 	  }
// 	}
//       }
//     }

//     std::cout << "\t Write: " << n_written_graphs << " graphs to " << _outputFileName.c_str() << std::endl;
    
// #endif      
//   }


}
