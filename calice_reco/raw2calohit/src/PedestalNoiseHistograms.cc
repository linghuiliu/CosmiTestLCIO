#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include "PedestalNoiseHistograms.hh"

#include <stdexcept>
#include <iostream>
#include <cassert>

#include <CellParameter.hh>
#include <CellParameterAccess.hh>
#include <histmgr/HistMgr.hh>
#include <histmgr/HistPar.hh>
#include <histmgr/FloatHistogram1D.hh>
#include <collection_names.hh>

#include <Average_t.hh>

namespace CALICE {
  PedestalNoiseHistograms a_PedestalNoiseHistograms_instance;

  PedestalNoiseHistograms::PedestalNoiseHistograms()
    :    VRawADCValueProcessor("PedestalNoiseHistograms")
  {

    registerProcessorParameter( "HistogramGroupName" , 
    				"The name of the histogram group under which the control histograms will be registered."  ,
				_histogramGroupKey.nameStorage() ,
    				type() ) ;

    _cellParameterCollectionName="CellParameters";
    registerProcessorParameter( "CellParameterCollectionName" , 
				"The name of the collection which contains the pedestals, the noise, etc. of all the cells." ,
				_cellParameterCollectionName ,
				_cellParameterCollectionName);

    _noiseHistPar.clear();
    _noiseHistPar.push_back(200);
    _noiseHistPar.push_back(-100);
    _noiseHistPar.push_back(100);
    
    registerOptionalParameter( "NoiseBinning" , 
			       "The binning for the control histograms of the noise distribution." ,
			       _noiseHistPar ,
			       _noiseHistPar ,
			       _noiseHistPar.size() ) ;

    _adcHistPar.clear();
    _adcHistPar.push_back(500);
    _adcHistPar.push_back(-1000);
    _adcHistPar.push_back(1000);
    
    registerOptionalParameter( "AdcBinning" , 
			       "The binning for the control histograms of the distribution of the adc values." ,
			       _adcHistPar ,
			       _adcHistPar ,
			       _adcHistPar.size() ) ;


    _eventPar.clear();
    _eventPar.push_back(100);
    _eventPar.push_back(0);
    _eventPar.push_back(200000);
    registerOptionalParameter( "EventStepsAndRange" , 
    				"Event step size, first event, and last event. The history is extended in events steps given by the first parameter for events in the range indicated by the second and third parameter"  ,
			       _eventPar ,
			       _eventPar,
			       _eventPar.size());

    _histKey[kH1Noise]=histmgr::Key_t("PadNoise");
    _histKey[kH1Pedestal]=histmgr::Key_t("PadPedestal");

  }

  void PedestalNoiseHistograms::init() {
    VRawADCValueProcessor::init();
    printParameters();

    assert(_eventPar.size()==3);
    assert(_eventPar[0]>0);
    assert(_eventPar[1]<=_eventPar[2]);

    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histogramList->createHistogramGroup(_histogramGroupKey);
    _missingCellParameters=0;
  }

  void PedestalNoiseHistograms::processEvent( LCEvent * evtP ) {
    try {
      UInt_t event_nr=evtP->getEventNumber();

      UInt_t reco_state=static_cast<UInt_t>(evtP->getParameters().getIntVal(PAR_RECO_STATE));

      // do not use events with permanent signals
      if (reco_state==kRecoStateBeam || reco_state==kRecoStateCalibration) return;

      //FIXME: does not make any sense if runs are chained.
      if (event_nr>=static_cast<unsigned int>(_eventPar[1]) && event_nr<=static_cast<unsigned int>(_eventPar[2])) {
	if (event_nr%_eventPar[0]==0) {

	  histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
	  histmgr::HistogramCollection_t & noise_hist_arr=histogramList->getHistogramCollection(_histogramGroupKey,_histKey[kH1Noise]);
	  histmgr::HistogramCollection_t & pedestal_hist_arr=histogramList->getHistogramCollection(_histogramGroupKey,_histKey[kH1Pedestal]);

	  CALICE::CellParameterAccess cell_parameter_col(_cellParameterCollectionName,evtP);

	  UInt_t n_modules=_mapping.getNModules();
	  for(UInt_t module_i=0; module_i<n_modules; module_i++) {
	    UInt_t n_cells=_mapping.getNCellsPerModule(module_i);
	    for (UInt_t cell_i=0; cell_i<n_cells; cell_i++) {
	      CellParameter cell_parameter=cell_parameter_col.getCellParameter(module_i,cell_i);
	      noise_hist_arr.histogram(module_i,cell_i)->fill(cell_parameter.getNoise());
	      pedestal_hist_arr.histogram(module_i,cell_i)->fill(cell_parameter.getPedestal());
	    }
	  }
	}
      }
    }
    catch (  DataNotAvailableException err) {
      _missingCellParameters++;
    }

  }

  void PedestalNoiseHistograms::end() 
  {
    std::cout << "---- PedestalNoiseHistograms: report>" << std::endl
	      << "\t" << _missingCellParameters  << " : events without collection of cell parameter."  <<std::endl
	      << std::endl;

    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & noise_hist_arr=histogramList->getHistogramCollection(_histogramGroupKey,_histKey[kH1Noise]);
    histmgr::HistogramCollection_t & pedestal_hist_arr=histogramList->getHistogramCollection(_histogramGroupKey,_histKey[kH1Pedestal]);
    
#ifdef BOUNDARY_CHECK
    assert(noise_hist_arr.nMajor() == _mapping.getNModules());
    assert(pedestal_hist_arr.nMajor() == _mapping.getNModules());
#endif      
    std::cout << " Mean pedestals and noise per module: " <<std::endl;
    for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
#ifdef BOUNDARY_CHECK
      assert(noise_hist_arr.nMinor(module_i) == _mapping.getNCellsPerModule(module_i) );
      assert(pedestal_hist_arr.nMinor(module_i) == _mapping.getNCellsPerModule(module_i) );
#endif      
      Average_t av_noise;
      Average_t av_pedestal;
      for (UInt_t cell_i=0; cell_i<_mapping.getNCellsPerModule(module_i);cell_i++) {
	av_noise.add(noise_hist_arr.histogram(module_i,cell_i)->mean());
	av_pedestal.add(pedestal_hist_arr.histogram(module_i,cell_i)->mean());
      }
      av_noise.calculate();
      av_pedestal.calculate();
      std::cout << "  " << std::setw(2) << module_i << ":: noise: " << av_noise << "   pedestal" << av_pedestal << std::endl;
    }

  }

  void PedestalNoiseHistograms::moduleTypeChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleTypeChanged(col);
    detectorChanged();
  }

  void PedestalNoiseHistograms::moduleLocationChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleLocationChanged(col);
    detectorChanged();
  }

  void PedestalNoiseHistograms::moduleConnectionChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleConnectionChanged(col);
    detectorChanged();
  }

  void PedestalNoiseHistograms::detectorChanged()
  {
    createHistograms();
  }
  
  void PedestalNoiseHistograms::createHistograms() 
  {
    if (!_mapping.isModuleConditionsDataComplete()) return;
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    if (_mapping.getNModules()>0) {
      lcio::IntVec cell_arr;
      lcio::StringVec noise_name;
      lcio::StringVec pedestal_name;
      for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
	cell_arr.push_back(_mapping.getNCellsPerModule(module_i));
	std::stringstream a_name_ext;
	CellIndex cell_index(_mapping.getGeometricalCellIndex(module_i,0));
	a_name_ext << "_" << _mapping.getModuleName(module_i) << "_layer_" << cell_index.getLayerIndex();
	
	noise_name.push_back(std::string("PadNoise")+a_name_ext.str());
	pedestal_name.push_back(std::string("PadPedestal")+a_name_ext.str());
      }
      histogramList->createHistograms(_histogramGroupKey,_histKey[kH1Noise],noise_name,cell_arr,
								HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]));
      histogramList->createHistograms(_histogramGroupKey,_histKey[kH1Pedestal],pedestal_name,cell_arr,
								HistPar((UInt_t) _adcHistPar[0], _adcHistPar[1], _adcHistPar[2]));
    }
    
    // verify code sanity: Are all the histograms created?
    assert(kNH1==2);
  }


}
