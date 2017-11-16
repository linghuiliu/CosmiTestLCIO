#include <IntegratedHcalProcessor.hh>

#include <stdexcept>

#include <lcio.h>
#include <lccd.h>

#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/ConditionsProcessor.h>
#include <ConditionsChangeDelegator.hh>
#include <FastCaliceHit.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>
#include <VRawADCValueProcessor.hh>
#include <collection_names.hh>
#include <HcalTempModel.hh>
#include <CalibrationSet.hh>
#include <GainConstants.hh>
#include <InterConstants.hh>
#include <MIPConstants.hh>
#include <TriggerBits.hh>
#include <HcalCellIndex.hh>

#include "HcalTileIndex.hh"

//#define HCALRECO_DEBUG
//#define HCALDIGI_DEBUG

namespace CALICE {

  IntegratedHcalProcessor aIntegratedHcalProcessor;

IntegratedHcalProcessor::IntegratedHcalProcessor(const std::string processorName) : SiPmPropertiesProcessor(processorName),
///  _gainFitResultsMap(&CALICE::LinearFitResult::getID),
///  _MipFitResultsMap(&CALICE::LinearFitResult::getID),

    _gainFitConstantsMap(&CALICE::LinearFitConstant::getID),
    _gainFitSlopesMap(&CALICE::LinearFitSlope::getID),   
    
    _MipFitConstantsMap(&CALICE::LinearFitConstant::getID),
    _MipFitSlopesMap(&CALICE::LinearFitSlope::getID),
    
    
    _SatCorrMap(&CALICE::SatCorrItep::getIndex),			       
    _InterConstantsMap(&CALICE::LinearFitConstant::getID),
    
///    _gainCalibrationChange(this,&IntegratedHcalProcessor::gainCalibrationChanged),
///    _interCalibrationChange(this,&IntegratedHcalProcessor::interCalibrationChanged),
///    _mipCalibrationChange(this,&IntegratedHcalProcessor::mipCalibrationChanged),
    _ahcSroModDataChange(this, &IntegratedHcalProcessor::ahcSroModDataColChanged),
    
    _temperatureSensorCalibrationChange(this, &IntegratedHcalProcessor::temperatureSensorCalibrationChanged),
    
    _moduleTypeChange(this, &IntegratedHcalProcessor::moduleTypeChanged),
    _moduleLocationChange(this, &IntegratedHcalProcessor::moduleLocationChanged),
    _moduleConnectionChange(this, &IntegratedHcalProcessor::moduleConnectionChanged),
    _ahcSroModDataCol(0)

  {  
    _description = "Integrated data base based handling of the CALICE Hcal";
  
  registerProcessorParameter("ModuleConnectionCollectionName", "Name of the conditions data collection which describes the connection between modules and the DAQ front-ends (folder /CaliceEcal/module_connection)",
			       _colNameModuleConnection, std::string("ModuleConnection"));

  registerProcessorParameter("ModuleLocationCollectionName", "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_location)",
			       _colNameModuleLocation, std::string("ModuleLocation"));

  registerProcessorParameter("ModuleDescriptionCollectionName", "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_description)",
			       _colNameModuleDescription, std::string("ModuleDescription"));

  registerProcessorParameter("ViewMapping", "View the mapping between channels and modules when ever the module location or module connection conditions data change (set to 0 or !=0)",
			       _viewConnectionTree, 0);
			     
///  registerProcessorParameter("GainCalibrationCollectionTemplate", "Template for the name of the module wise collections with the gain calibration constants",
///			       _gainCalColName, std::string("Gain_"));
///
///  registerProcessorParameter("InterCalibrationCollectionTemplate", "Template for the name of the module wise collections with the inter calibration constants",
///			       _interCalColName, std::string("IC_"));
///
///  registerProcessorParameter("MIPCalibrationCollectionTemplate", "Template for the name of the module wise collections with the MIP calibration constants",
///			       _mipCalColName, std::string("MIP_"));			   
	   
    IntVec modulesExample;
    modulesExample.push_back(0);
    registerOptionalParameter("Module", "Add calibration for module", _modules, modulesExample, modulesExample.size());

    registerProcessorParameter("AHC_SRO_ModData","Name of the conditions collection for the AHC Slow r/o data.",
			     //                           _ahcSroModDataColName, std::string(COL_AHC_SRO_MOD));
			       _ahcSroModDataColName, std::string(""));

///    registerProcessorParameter("FudgeNonExistingSaturationCorrections", "set saturation corrections to 1 (1) or to 0 (0) if they don't exist",	
///			       _fudgeNonExistingSaturationCorrections, (int) 0);

    registerProcessorParameter( "GainCalibScalingFactor",
				"Global scaling factor to gain calibrations for systematic studies",
				_gainScalingFactor, float( 1. ) );

    registerProcessorParameter( "MipCalibScalingFactor",
				"Global scaling factor to mip calibrations for systematic studies",
				_mipScalingFactor, float( 1. ) );

///    registerProcessorParameter("CorrectLYSat","", _correctSatLY, false);

    registerProcessorParameter("TemperatureSensorCalibrationCollectionName",
			       "",
			       _temperatureSensorCalibrationColName,
			       std::string("TempSensorCalibCol"));

    registerProcessorParameter("GainFitConstantsCollectionName",
			       "",
			       _gainFitConstantsCollectionName,
			       std::string("GainFitConstantsCol"));

    registerProcessorParameter("GainFitSlopesCollectionName",
			       "",
			       _gainFitSlopesCollectionName,
			       std::string("GainFitSlopesCol"));

    registerProcessorParameter("MipFitConstantsCollectionName",
			       "",
			       _MipFitConstantsCollectionName,
			       std::string("MipFitConstantsCol"));

    registerProcessorParameter("SatCorrCollectionName",
			       "",
			       _SatCorrCollectionName,
			       std::string("AhcSatCorrItep"));

    registerProcessorParameter("MipFitSlopesCollectionName",
			       "",
			       _MipFitSlopesCollectionName,
			       std::string("MipFitSlopesCol"));

    registerProcessorParameter("InterConstantsCollectionName",
			       "",
			       _InterConstantsCollectionName,
			       std::string("InterConstantsCol"));
    
    registerProcessorParameter("doMipTemperatureCorrection",
			       "",
			       _doMipTempCorr,
			       false); 
  
    registerProcessorParameter("doGainTemperatureCorrection",
			       "",
			       _doGainTempCorr,
			       false); 

    registerProcessorParameter("doScaleResponseCurves",
			       "", _doSatScaling, (int)(0) );

    _tempModel = new HcalTempModel();
///    _gainCalibSet = new CalibrationSet<GainConstants>(_tempModel);
///    _interCalibSet = new CalibrationSet<InterConstants>(_tempModel);
///    _mipCalibSet = new CalibrationSet<MIPConstants>(_tempModel);
    _mapping.init();
    _mapping.setViewConnectionTree(_viewConnectionTree!=0);
  };
    

  IntegratedHcalProcessor::~IntegratedHcalProcessor() {
///    if ( _gainCalibSet ) delete _gainCalibSet;
///    if ( _interCalibSet ) delete _interCalibSet;
///    if ( _mipCalibSet ) delete _mipCalibSet;
    if ( _tempModel ) delete _tempModel;
  };


  void IntegratedHcalProcessor::init(){
    // change module/chip/channel to temperature extrapolation method here
    _tempProvider = new AhcSimpleTempProvider();
    _tempProvider->setSanityRange(0,45);

    _mapping.init();
    _mapping.setViewConnectionTree(_viewConnectionTree!=0);
    SiPmPropertiesProcessor::init();
    std::stringstream message;
    bool error=false;
///    if (parameterSet("Module")) {
///      unsigned index = 0 ;
///      message << "Missing condition data: "; 
///      while (index < _modules.size() ){
///
///	char _mod[10];
///	sprintf(_mod,"%.2d",_modules[index++]); 	
///	std::string _module(_mod);
///      
///	std::string _finalGainCalColName = _gainCalColName + _module;
///	if (!marlin::ConditionsProcessor::registerChangeListener (&_gainCalibrationChange ,_finalGainCalColName)) {
///	  message << " " << _finalGainCalColName;
///	  error=true;
///	} else {
///#ifdef HCALRECO_DEBUG
///	  std::cout << "IntegratedHcalProcessor: register conditions data handler for gain calibration collection " << _finalGainCalColName << std::endl; 
///#endif	      
///	}
///	std::string _finalInterCalColName = _interCalColName + _module;
///	if (!marlin::ConditionsProcessor::registerChangeListener (&_interCalibrationChange ,_finalInterCalColName)) {
///	  message << " " << _finalInterCalColName;
///	  error=true;
///	} else {
///#ifdef HCALRECO_DEBUG
///	  std::cout << "IntegratedHcalProcessor: register conditions data handler for inter calibration collection " << _finalInterCalColName << std::endl; 
///#endif	      
///	}
///	std::string _finalMipCalColName = _mipCalColName + _module;
///	if (!marlin::ConditionsProcessor::registerChangeListener (&_mipCalibrationChange ,_finalMipCalColName)) {
///	  message << " " << _finalMipCalColName;
///	  error=true;
///	} else {
///#ifdef HCALRECO_DEBUG
///	  std::cout << "IntegratedHcalProcessor: register conditions data handler for MIP collection " << _finalMipCalColName << std::endl; 
///#endif	      
///	}
///      }
///    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleLocationChange,_colNameModuleLocation)) {
      message << " " << _colNameModuleLocation;
      error = true;
    } else {
#ifdef HCALRECO_DEBUG      
      std::cout << " IntegratedHcalProcessor: register conditions data handler for module locations " << std::endl;
#endif
    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleConnectionChange, _colNameModuleConnection)) {
      message << " " << _colNameModuleConnection;
      error = true;
    } else {
#ifdef HCALRECO_DEBUG      
      std::cout << " IntegratedHcalProcessor: register conditions data handler for module connections " << std::endl;
#endif
    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleTypeChange,_colNameModuleDescription) ) {
      message << " " << _colNameModuleDescription;
      error = true;
    } else {
#ifdef HCALRECO_DEBUG      
      std::cout << " IntegratedHcalProcessor: register conditions data handler for module descriptions " << std::endl;
#endif
    }
    if (!marlin::ConditionsProcessor::registerChangeListener( 
                                                             &_gainFitConstantsMap,
                                                             _gainFitConstantsCollectionName) 
	) {
      message << " " << _gainFitConstantsCollectionName;

      if(_doGainTempCorr) {
        error = true;
      }

    }
    if (!marlin::ConditionsProcessor::registerChangeListener( 
                                                             &_gainFitSlopesMap,
                                                             _gainFitSlopesCollectionName) 
	) {
      message << " " << _gainFitSlopesCollectionName;

      if(_doGainTempCorr) {
        error = true;
      }

    }
    if (!marlin::ConditionsProcessor::registerChangeListener( 
                                                             &_MipFitConstantsMap,
                                                             _MipFitConstantsCollectionName) 
	) {
      message << " " << _MipFitConstantsCollectionName;

      if(_doMipTempCorr) {
        error = true;
      }

    }
    if (!marlin::ConditionsProcessor::registerChangeListener( 
                                                             &_MipFitSlopesMap,
                                                             _MipFitSlopesCollectionName) 
	) {
      message << " " << _MipFitSlopesCollectionName;

      if(_doMipTempCorr) {
        error = true;
      }

    }
    if (!marlin::ConditionsProcessor::registerChangeListener( 
                                                             &_InterConstantsMap,
                                                             _InterConstantsCollectionName) 
	) {
      message << " " << _InterConstantsCollectionName;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener( 
                                                             &_temperatureSensorCalibrationChange,
                                                             _temperatureSensorCalibrationColName) 
	) {
      message << " " << _temperatureSensorCalibrationColName;
      // We allow to live without temperature sensor calibration.
      //error = true;
    }

    if ( !marlin::ConditionsProcessor::registerChangeListener(
							      &_SatCorrMap,
	     						      _SatCorrCollectionName ) ) {
      message << " " << _SatCorrCollectionName;
    }

    if (error) { 
      message <<  ".";
      throw ErrorMissingConditionsDataHandler(message.str());
    }

    if( !marlin::ConditionsProcessor::registerChangeListener(&_ahcSroModDataChange, _ahcSroModDataColName)) {
      std::stringstream message;
      message << _processorName <<"::init(): no conditions data handler " << _ahcSroModDataColName;
      throw ErrorMissingConditionsDataHandler(message.str());
    }
    //  for (unsigned short mod=0; mod<HCAL_N_MOD+1; mod++){
    //    for (unsigned short cell=0; cell<HCAL_N_CELL; cell++){
    //      _lightyield[mod][cell] = 0;
    //      _lightyieldError[mod][cell] = 0;
    //    }
    //  }
  };


///  void IntegratedHcalProcessor::gainCalibrationChanged(lcio::LCCollection* col) {
///#ifdef HCALRECO_DEBUG      
/////  std::cout << "IntegratedHcalProcessor::gainCalibrationChanged()" << std::endl;
///#endif
///    _gainCalibSet->setScalingFactor( _gainScalingFactor );
///    if (_ahcSroModDataCol)
///      _gainCalibSet->fill(col, _ahcSroModDataCol);
///    else
///      _gainCalibSet->fill(col);
///  //  _gainCalibSet->applyScalingFactor( _gainScalingFactor );
///  //  calculateLightYield();
///  };
///
///
///  void IntegratedHcalProcessor::interCalibrationChanged(lcio::LCCollection* col) {
///#ifdef HCALRECO_DEBUG      
/////  std::cout << "IntegratedHcalProcessor::interCalibrationChanged()" << std::endl;
///#endif
///    if (_ahcSroModDataCol)
///      _interCalibSet->fill(col, _ahcSroModDataCol);
///    else
///      _interCalibSet->fill(col);
///  //  calculateLightYield();
///  };
///
///
///  void IntegratedHcalProcessor::mipCalibrationChanged(lcio::LCCollection* col) {
///#ifdef HCALRECO_DEBUG      
/////  std::cout << "IntegratedHcalProcessor::mipCalibrationChanged()" << std::endl;
///#endif
///    _mipCalibSet->setScalingFactor( _mipScalingFactor );
///    if (_ahcSroModDataCol)
///      _mipCalibSet->fill(col, _ahcSroModDataCol);
///    else
///      _mipCalibSet->fill(col);
///  // _mipCalibSet->applyScalingFactor( _mipScalingFactor );
///  //  calculateLightYield();
///  };


  void IntegratedHcalProcessor::ahcSroModDataColChanged(lcio::LCCollection* col) {
    //#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::ahcSroModDataColChanged() " << col << std::endl;
    //#endif
    _ahcSroModDataCol = col;
///    _gainCalibSet->setTemp(_ahcSroModDataCol);
///    _interCalibSet->setTemp(_ahcSroModDataCol);
///    _mipCalibSet->setTemp(_ahcSroModDataCol);
    //  calculateLightYield();

    streamlog_out(DEBUG0) << "calling _tempProvider->setAhcSroModBlocks(col);\n";
    _tempProvider->setAhcSroModBlocks(col);
    streamlog_out(DEBUG0) << "_tempProvider->setAhcSroModBlocks(col) done\n";

  };

  void IntegratedHcalProcessor::temperatureSensorCalibrationChanged(lcio::LCCollection* col) {
    //#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::temperatureSensorCalibrationChanged() " << col << std::endl;
    //#endif

    _tempProvider->setCalibrations(col);

  }

  void IntegratedHcalProcessor::SipmInfoChanged(lcio::LCCollection* col) {
#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::SiPmInfoChanged()" << std::endl;
#endif
    SiPmPropertiesProcessor::SipmInfoChanged(col);
    //  calculateLightYield();
    updateGeometryMaps();
  };
  
  
  void IntegratedHcalProcessor::SipmSaturationChanged(lcio::LCCollection* col) {
#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::SiPmSaturationChanged()" << std::endl;
#endif
    SiPmPropertiesProcessor::SipmSaturationChanged(col);
    //  calculateLightYield();
    updateGeometryMaps();
  };


  void IntegratedHcalProcessor::ModuleProductionChanged(lcio::LCCollection* col) {
#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::ModuleProductionChanged()" << std::endl;
#endif
    SiPmPropertiesProcessor::ModuleProductionChanged(col);
    //  calculateLightYield();
    updateGeometryMaps();
  };


  void IntegratedHcalProcessor:: moduleTypeChanged(lcio::LCCollection* col) {
#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::moduleTypeChanged()" << std::endl;
#endif
    _mapping.moduleTypeChanged(col);
    if (_mapping.isModuleConditionsDataComplete())
      _indexLookup.createIndexReverseLookup(_mapping);
    updateGeometryMaps();
  };


  void IntegratedHcalProcessor::moduleLocationChanged(lcio::LCCollection* col) {
#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::moduleLocationChanged()" << std::endl;
#endif
    _mapping.moduleLocationChanged(col);
    if (_mapping.isModuleConditionsDataComplete())
      _indexLookup.createIndexReverseLookup(_mapping);
    updateGeometryMaps();
  };


  void IntegratedHcalProcessor::moduleConnectionChanged(lcio::LCCollection* col) {
#ifdef HCALRECO_DEBUG      
    std::cout << "IntegratedHcalProcessor::moduleConnectionChanged()" << std::endl;
#endif
    _mapping.moduleConnectionChanged(col);
    if (_mapping.isModuleConditionsDataComplete())
      _indexLookup.createIndexReverseLookup(_mapping);
    updateGeometryMaps();
  };

  std::pair<unsigned int, unsigned int> IntegratedHcalProcessor::getModuleIDandCellkey(
										       unsigned int module,
										       unsigned int chip,
										       unsigned int channel) {

    short type = 0;
    if ( chip > 5 ) ++type;        //  lower part
    if ( module > 30 ) type += 2;  //  coarse modules

    unsigned int moduleID = ( module << 8 ) | type;

    unsigned int cellkey = (chip << 8) | channel;

    return std::make_pair(moduleID,cellkey);

  }



  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  float IntegratedHcalProcessor::getCorrectedAmplitude( int cellID, 
						        float satAmpl ) {
    
    try{
      SatCorrItep &satCorr = _SatCorrMap.find(cellID);
      if ( _doSatScaling != 0 ) {
	try {
	  float scaling = _pixelScaleFactorsMap.find( cellID ).getValue();
	  satCorr.setScaling( scaling );
	} catch ( ... ) {
	  satCorr.setScaling( _defaultPixelScaleFactor );
	}
      }

      streamlog_out(DEBUG0)<<" Using scaling factor "<< satCorr.getScaling() 
		   << " for cell ID " << cellID << std::endl;

      float correctedAmplitude = satCorr.deSaturate(satAmpl);
      return correctedAmplitude;
      
    } catch(lcio::Exception e)
      {
	streamlog_out(ERROR0) <<" No saturration correction found for cell ID "<<cellID<<endl;
      }
    return satAmpl;
  }
  
  
  float IntegratedHcalProcessor::getSaturatedAmplitude( int cellID,
							float linAmpl ) {
    try {
      SatCorrItep &satCorr = _SatCorrMap.find( cellID );
      if ( _doSatScaling != 0 ) {
	try {
	  float scaling = _pixelScaleFactorsMap.find( cellID ).getValue();
	  satCorr.setScaling( scaling );
	} catch ( ... ) {
	  satCorr.setScaling( _defaultPixelScaleFactor );
	}
      }

      streamlog_out(DEBUG0)<<" Using scaling factor "<< satCorr.getScaling() 
		   << " for cell ID " << cellID << std::endl;

      return satCorr.saturate( linAmpl );
    } catch ( lcio::Exception &e ) {
      streamlog_out(ERROR0) << "No response curve found for cell ID" 
			    << cellID << endl;
    }
    return linAmpl;
  }
  
  

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  float IntegratedHcalProcessor::getMip( int cellID ) {
    
    _cellCalibUsedMipTempCorr = false;

    try {
      
      LinearFitConstant& MipFitConstant = 
	_MipFitConstantsMap.find( cellID );
      if( _doMipTempCorr ) {
	const LinearFitSlope& MipFitSlope = 
	  _MipFitSlopesMap.find( cellID );
	
	MipFitConstant.setSlope(MipFitSlope.getSlope());
	
	const double currentTemperature = getCellTemp( cellID );
	
	streamlog_out(DEBUG4) << "Using mip temperature dependency"
			      << std::endl;
	
	_cellCalibUsedMipTempCorr=true;
	streamlog_out(DEBUG0) << "--getMip() ended" << std::endl; 
	
	return MipFitConstant.eval(currentTemperature);
      } else {
	return MipFitConstant.getConstant();
      }
      
    } catch( lcio::Exception &e ) { 
///      return 100000;
///      HcalTileIndex hti( cellID );
///      streamlog_out(DEBUG4) << "No mip temperature dependency found for "
///			    << "module: " << hti.getModule()
///			    << " chip: " << hti.getChip()
///			    << " channel: " << hti.getChannel() << '\n'
///			    << "Using fall back constant." << std::endl;
///      return getMip( hti.getModule(), hti.getChip(), hti.getChannel() );
    }
    return 100000;
  }
  
  
///  float IntegratedHcalProcessor::getMip(unsigned int module,
///					unsigned int chip,
///					unsigned int channel) {
///    streamlog_out(DEBUG0) << "--getMip() started" << std::endl; 
///    _cellCalibUsedMipTempCorr = false;
///
///    if( _doMipTempCorr ) {
///      try {
///
///	LinearFitConstant& MipFitConstant = 
///	  _MipFitConstantsMap.find(HcalTileIndex( module,
///						  chip,
///						  channel).getIndex());
///	const LinearFitSlope& MipFitSlope = 
///	  _MipFitSlopesMap.find(HcalTileIndex( module,
///					       chip,
///					       channel).getIndex());
///
///	MipFitConstant.setSlope(MipFitSlope.getSlope());
///
///	const double currentTemperature = getCellTemp(module, chip, channel);
///
///	streamlog_out(DEBUG4) << "Using mip temperature dependency"
///			      << std::endl;
///
///	_cellCalibUsedMipTempCorr=true;
///	streamlog_out(DEBUG0) << "--getMip() ended" << std::endl; 
///
///	return MipFitConstant.eval(currentTemperature);
///
///      } catch(lcio::Exception e) {
///
///	streamlog_out(DEBUG4) << "No mip temperature dependency found for "
///			      << "module: " << module
///			      << " chip: " << chip
///			      << " channel: " << channel << '\n'
///			      << "Using fall back constant." << std::endl;
///      }
///    }
///
///    std::pair<unsigned int, unsigned int> IDandKey = 
///      getModuleIDandCellkey(module,chip,channel);
///  
///    int moduleid = IDandKey.first;
///    int cellkey = IDandKey.second;
///
///    MIPConstants* mipCalib = 
///      _mipCalibSet->getCalib(moduleid, cellkey);
///
///    // default mip is 100000
///    float mip = 100000;
///    if ( mipCalib ) {
///      mip = mipCalib->getMIPValue();
///    }
///    
///    _cellCalibUsedMipTempCorr = false;
///    streamlog_out(DEBUG0) << "--getMip() ended" << std::endl; 
///    return mip;
///  
///  };

  float IntegratedHcalProcessor::getGain( int cellID ) {
    try {
      LinearFitConstant& gainFitConstant = 
	_gainFitConstantsMap.find( cellID );
      
      
      if( _doGainTempCorr ) {
	const LinearFitSlope& gainFitSlope = 
	  _gainFitSlopesMap.find( cellID );
	
	gainFitConstant.setSlope(gainFitSlope.getSlope());
	
	const double currentTemperature = getCellTemp( cellID );
	
	//            streamlog_out(DEBUG4) << "Using gain temperature dependency\n"
	//                << "for " << hti << '\n'
	//                << gainFit << std::endl;
	
	//    std::cout << CALICE::HcalTileIndex(1,1,1) << std::endl;
	
	_cellCalibUsedGainTempCorr = true;
	streamlog_out(DEBUG0) << "--getGain() ended" << std::endl; 
	return gainFitConstant.eval(currentTemperature);
      } else {
	return gainFitConstant.getConstant();
      }
      
    } catch(lcio::Exception e) {
///      return 400;
///      HcalTileIndex hti( cellID );
///      streamlog_out(DEBUG4) << "No gain temperature dependency found for "
///			    << "module: " << hti.getModule()
///			    << " chip: " << hti.getChip()
///			    << " channel: " << hti.getChannel() << '\n'
///			    << "Using fall back constant." << std::endl;
///      
///      return getGain( hti.getModule(), hti.getChip(), hti.getChannel() );
    }
    return 400;
  }
  
///  float IntegratedHcalProcessor::getGain(unsigned int module,
///					 unsigned int chip,
///					 unsigned int channel) {
///    streamlog_out(DEBUG0) << "--getGain() started" << std::endl; 
///
///    _cellCalibUsedGainTempCorr = false;
///
///
///    std::pair<unsigned int, unsigned int> IDandKey = 
///      getModuleIDandCellkey(module,chip,channel);
///  
///    int moduleid = IDandKey.first;
///    int cellkey = IDandKey.second;
///
///    GainConstants* gainCalib = 
///      _gainCalibSet->getCalib(moduleid, cellkey);
///
///    // default gain is 400
///    float gain = 400;
///    if ( gainCalib ) {
///      gain = gainCalib->getGainValue();
///    }
///
///    _cellCalibUsedGainTempCorr = false;
///    streamlog_out(DEBUG0) << "--getGain() ended" << std::endl; 
///    return gain;
///
///  };

  float IntegratedHcalProcessor::getIC( int cellID ) {
    try{
      LinearFitConstant& interConstant =
	_InterConstantsMap.find( cellID );
      return interConstant.getConstant();
    } catch ( ... ){
    }
    return 10;
///    HcalTileIndex hti( cellID );
///    return getIC( hti.getModule(), hti.getChip(), hti.getChannel() );
  }

///  float IntegratedHcalProcessor::getIC(unsigned int module,
///				       unsigned int chip,
///				       unsigned int channel) {
///    streamlog_out(DEBUG0) << "--getIC() started" << std::endl; 
///
///    std::pair<unsigned int, unsigned int> IDandKey = 
///      getModuleIDandCellkey(module,chip,channel);
///  
///    int moduleid = IDandKey.first;
///    int cellkey = IDandKey.second;
///
///    InterConstants* icCalib = 
///      _interCalibSet->getCalib(moduleid, cellkey);
///
///    // default ic is 10
///    float ic = 10;
///    if ( icCalib ) {
///      ic = icCalib->getInterValue();
///    }
///
///    streamlog_out(DEBUG0) << "--getIC() ended" << std::endl; 
///    return ic;
///
///  };



 void IntegratedHcalProcessor::fillTempCountMaps(unsigned int module,
						  unsigned int chip,
						  unsigned int channel) {
    HcalTileIndex hti( module, chip, channel );
    fillTempCountMaps( hti.getIndex() );
  }
  
  void IntegratedHcalProcessor::fillTempCountMaps( int cellID ) {
    _cellOccurenceCounter[cellID]++;
    
    if( _cellCalibUsedMipTempCorr ) {
      _cellMipTempCorrCounter[cellID]++;
    } else {
      _cellNoMipTempCorrCounter[cellID]++;
    }
    
    if( _cellCalibUsedGainTempCorr ) {
      _cellGainTempCorrCounter[cellID]++;
    } else {
      _cellNoGainTempCorrCounter[cellID]++;
    }
  }
  
  void IntegratedHcalProcessor::printTempCountMaps( std::ostream& out )
  {
    std::map<unsigned,unsigned>::const_iterator it;

    for(it=_cellOccurenceCounter.begin(); it!=_cellOccurenceCounter.end(); it++)
      {
        HcalTileIndex idx( (*it).first );
        out //<< std::setw(32) 
	  << idx << " occured " << std::setw(6) << (*it).second << " times\n";
      } 

    for(it=_cellMipTempCorrCounter.begin();
        it!=_cellMipTempCorrCounter.end(); it++)
      {
        HcalTileIndex idx( (*it).first );
        out //<< std::setw(32) 
	  << idx << " was calibrated with temperature corrected mip constant "
	  << std::setw(6) << (*it).second << " times\n";
      } 

    for(it=_cellNoMipTempCorrCounter.begin();
        it!=_cellNoMipTempCorrCounter.end(); it++)
      {
        HcalTileIndex idx( (*it).first );
        out //<< std::setw(32) 
	  << idx<<" was calibrated without temperature corrected mip constant "
	  << std::setw(6) << (*it).second << " times\n";
      } 

    for(it=_cellGainTempCorrCounter.begin();
        it!=_cellGainTempCorrCounter.end(); it++)
      {
        HcalTileIndex idx( (*it).first );
        out //<< std::setw(32) 
	  << idx << " was calibrated with temperature corrected gain constant "
	  << std::setw(6) << (*it).second << " times\n";
      } 

    for(it=_cellNoGainTempCorrCounter.begin();
        it!=_cellNoGainTempCorrCounter.end(); it++)
      {
        HcalTileIndex idx( (*it).first );
        out //<< std::setw(32) 
	  <<idx<<" was calibrated without temperature corrected gain constant "
	  << std::setw(6) << (*it).second << " times\n";
      } 
  }


  float IntegratedHcalProcessor::getCellTemp( int cellID ){
    HcalTileIndex hti( cellID );
    return getCellTemp( hti.getModule(), hti.getChip(), hti.getChannel() );
  }


  float IntegratedHcalProcessor::getCellTemp(unsigned int module,
					     unsigned int chip,
					     unsigned int channel) {

    return _tempProvider->getCellTemp(module, chip, channel);

  }


//  // //void IntegratedHcalProcessor::calculateLightYield() {
//  // //  if (_gainCalibSet->empty() || _interCalibSet->empty() || _mipCalibSet->empty() || SiPmPropertiesProcessor::empty()) return;
//  // //#ifdef HCALRECO_DEBUG      
//  // //  std::cout << "IntegratedHcalProcessor::calculateLightYield()" << std::endl;
//  // //#endif
//  // //  for (CalibrationSet<GainConstants>::CalibReadMap::const_iterator modIter = _gainCalibSet->_readMap.begin(); modIter != _gainCalibSet->_readMap.end(); modIter++) {
//  // //    unsigned module = modIter->first;
//  // //    CalibrationSet<GainConstants>::CalibModuleData _moduleData = modIter->second;
//  // //    CalibrationSet<GainConstants>::CalibModuleMap* _cellMap = _moduleData.second;
//  // //    for (CalibrationSet<GainConstants>::CalibModuleMap::const_iterator cellkeyIter = _cellMap->begin(); cellkeyIter != _cellMap->end(); cellkeyIter++) {
//  // //      const unsigned short mod = (module & 0xFF00) >> 8;
//  // //      const unsigned short cellkey = cellkeyIter->first;
//  // //      const unsigned short chip = (cellkey & 0xFF00) >> 8;
//  // //      const unsigned short channel = cellkey & 0x00FF;
//  // //      const unsigned short cellindex = chip*18+channel;
//  // //      const GainConstants* gainCalib = _gainCalibSet->getCalib(module, cellkey);
//  // //      const InterConstants* interCalib = _interCalibSet->getCalib(module, cellkey);
//  // //      const MIPConstants* mipCalib = _mipCalibSet->getCalib(module, cellkey);
//  // //      if (mipCalib) {
//  // //        if (gainCalib && interCalib) {	  
//  // //    	  if (gainCalib->calibrationValid() && interCalib->calibrationValid() && mipCalib->calibrationValid()) {
//  // //	      _lightyield[mod][cellindex] = interCalib->applyCalibration(gainCalib->applyCalibration(mipCalib->cancelCalibration(1.)));
//  // //#ifdef HCALRECO_DEBUG
//  // //	      std::cout << "Begin Lightyield decomposition" << std::endl;
//  // //	      std::cout << "mipCalib->cancelCalibration(): " << mipCalib->cancelCalibration(1.) <<std::endl;  
//  // //	      std::cout << "gainCalib->applyCalibration(mipCalib->cancelCalibration(1.)): " << gainCalib->applyCalibration(mipCalib->cancelCalibration(1.)) << std::endl;  
//  // //	      std::cout << "interCalib->applyCalibration(gainCalib->applyCalibration(mipCalib->cancelCalibration(1.)))" << interCalib->applyCalibration(gainCalib->applyCalibration(mipCalib->cancelCalibration(1.))) << std::endl;
//  // //              std::cout << "lightyield (" << mod << ", " << cellindex << "):" << _lightyield[mod][cellindex] << std::endl;
//  // //	      std::cout << "End Lightyield decomposition" << std::endl;
//  // //#endif
//  // //              if(_correctSatLY == true) {
//  // //
//  // //                streamlog_out(DEBUG0) << "\"Correcting\" light yield for saturation\n"
//  // //                                      << "uncorrected light yield is: " << _lightyield[mod][cellindex]
//  // //                                      << '\n'
//  // //                                      << "factor is: " 
//  // //                                      << getSiPmSaturationCorrection(mod, 
//  // //                                                                     cellindex, 
//  // //                                                                     _lightyield[mod][cellindex]) 
//  // //                                      << '\n';
//  // //
//  // //                _lightyield[mod][cellindex] = getSiPmSaturationCorrection(mod, cellindex, _lightyield[mod][cellindex]) * _lightyield[mod][cellindex];
//  // //
//  // //              }
//  // ////fixme
//  // //              if (isnan(_lightyield[mod][cellindex])) _lightyield[mod][cellindex] = 0;
//  // //	      _lightyieldError[mod][cellindex] = 0;
//  // //#ifdef HCALRECO_DEBUG
//  // //              std::cout << "lightyield (" << mod << ", " << cellindex << "):" << _lightyield[mod][cellindex] << std::endl;
//  // //#endif
//  // //	    }
//  // //	  else {
//  // //#ifdef HCALRECO_DEBUG
//  // //	    std::cout << "lightyield (" << mod << ", " << cellindex << "): not valid calibration" << std::endl;
//  // //#endif
//  // //	   }  
//  // //        } else {
//  // //#ifdef HCALRECO_DEBUG
//  // //          std::cout << "lightyield (" << mod << " " << module << ", " << cellindex << " " << std::hex << cellindex << std::dec << "): no calibration" << std::endl;
//  // //#endif
//  // //        }
//  // //      } else {
//  // //#ifdef HCALRECO_DEBUG
//  // //        std::cout << "lightyield (" << mod << " " << module << ", " << cellindex << " " << std::hex << cellindex << std::dec << "): no mip calibration" << std::endl;
//  // //#endif
//  // //      }	
//  // //    }
//  // //  };
//  // //};


  void IntegratedHcalProcessor::establishTileCoverage(const unsigned mainCellIndex, const int rowOffset, const int columnOffset) {
    const HcalCellIndex _mainCellIndex(mainCellIndex);
    HcalCellIndex _coveredCellIndex(mainCellIndex);
    const unsigned short _row = _coveredCellIndex.getTileRow();
    const unsigned short _column = _coveredCellIndex.getTileColumn();
    _coveredCellIndex.setTileRow(_row+rowOffset).setTileColumn(_column+columnOffset);
    _cellMap.insert(std::make_pair(_coveredCellIndex.getCellIndex(), _mainCellIndex.getCellIndex()));
#ifdef HCALDIGI_DEBUG
    std::cout << "establishing coverage of " << std::hex << _coveredCellIndex.getCellIndex() << " by " << _mainCellIndex.getCellIndex() << std::dec << std::endl;
#endif  
  };


 void IntegratedHcalProcessor::considerNeighbourRelation(const unsigned centralCellIndex, const int rowOffset, const int columnOffset) {

#ifdef HCALDIGI_DEBUG
    std::cout << "considering " << std::hex << centralCellIndex << std::dec << " " << rowOffset << " " << columnOffset << std::endl;
#endif
    const HcalCellIndex _centralCellIndex(centralCellIndex);
    const unsigned short _row = _centralCellIndex.getTileRow();
    const unsigned short _column = _centralCellIndex.getTileColumn();
    HcalCellIndex _neighbourCellIndex(centralCellIndex);
    _neighbourCellIndex.setTileRow(_row+rowOffset).setTileColumn(_column+columnOffset);
#ifdef HCALDIGI_DEBUG
    std::cout << " checking neighbour " << std::hex << _neighbourCellIndex.getCellIndex() << std::dec << std::endl;
#endif
    const std::map<unsigned,unsigned>::const_iterator _aCell=_cellMap.find(_neighbourCellIndex.getCellIndex());
    if (_aCell==_cellMap.end()) {
#ifdef HCALDIGI_DEBUG
      std::cout << "neighbour not in map" << std::endl;
#endif  
      return;
    }  
    const std::map<unsigned,unsigned>::const_iterator _centralCell=_cellMap.find(_centralCellIndex.getCellIndex());
    if (_centralCell==_cellMap.end()) {
      throw logic_error("every cell center should be in the list of cells!");
      return;
    }  
#ifdef HCALDIGI_DEBUG
    std::cout << "  " << std::hex << _neighbourCellIndex.getCellIndex() << std::dec << " belongs to " << std::hex << _aCell->second << std::dec << std::endl;
    std::cout << "  " << std::hex << _centralCellIndex.getCellIndex() << std::dec << " belongs to " << std::hex << _centralCell->second << std::dec << std::endl;
#endif  
    // check that the cell is not occupied by the same tile
    if (_aCell->second==_centralCell->second) {
#ifdef HCALDIGI_DEBUG  
      std::cout << "neighbour and central cell are the same" << std::endl;
#endif    
      return;
    } 
    // check if there is already a list of neighbours for the central cell
    const std::pair<unsigned, unsigned> central_module_and_cell_index=_indexLookup.getModuleAndCellIndex(_mapping, _centralCell->second);
    const unsigned short centralModule = _mapping.getModuleID(central_module_and_cell_index.first);
    const unsigned short centralModuleType = _mapping.getModuleType(central_module_and_cell_index.first);
    const unsigned short centralChip = central_module_and_cell_index.second / 18 + (((centralModuleType==5) || (centralModuleType==7)) ? 6 : 0);
    const unsigned short centralChannel = central_module_and_cell_index.second % 18;
#ifdef HCALDIGI_DEBUG
    std::cout << "  central: " << centralModule << ", " << centralChip << ", " << centralChannel << std::endl;
#endif  
    const unsigned centralKey = (centralModule << 16) + (centralChip << 8) + centralChannel;
    NeighbourMap::iterator neighbourEntry = _neighbourMap.find(centralKey);
    if (neighbourEntry==_neighbourMap.end()) {
#ifdef HCALDIGI_DEBUG
      std::cout << "   new central cell " << std::hex << centralKey << std::dec << std::endl;
#endif  
      std::vector<NeighbourItem>* _neighbourVector = new std::vector<NeighbourItem>;
      neighbourEntry = _neighbourMap.insert(make_pair(centralKey,_neighbourVector)).first;
    }
    // insert new neighbour relation
    const std::pair<unsigned, unsigned> neighbour_module_and_cell_index=_indexLookup.getModuleAndCellIndex(_mapping, _aCell->second);
    NeighbourItem aNeighbourItem;
    aNeighbourItem.module = _mapping.getModuleID(neighbour_module_and_cell_index.first);
    const unsigned short neighbourModuleType = _mapping.getModuleType(neighbour_module_and_cell_index.first);
    const unsigned short neighbourChip = neighbour_module_and_cell_index.second / 18 + (((neighbourModuleType==5) || (neighbourModuleType==7)) ? 6 : 0);
    const unsigned short neighbourChannel = neighbour_module_and_cell_index.second % 18;
#ifdef HCALDIGI_DEBUG
    std::cout << "(" << centralModule << ", " << centralChip << ", " << centralChannel << ") and (" << 
      aNeighbourItem.module << ", " << neighbourChip << ", " << neighbourChannel << ") are neighbours " << std::endl;
#endif
    aNeighbourItem.cellkey = (neighbourChip << 8) + neighbourChannel;
    aNeighbourItem.weight = 1;
    bool found=false;
    for (std::vector<NeighbourItem>::iterator vectorIter = neighbourEntry->second->begin(); vectorIter !=neighbourEntry->second->end(); vectorIter++) {
      if ((aNeighbourItem.module == vectorIter->module) && (aNeighbourItem.cellkey == vectorIter->cellkey)) {
	vectorIter->weight++;
	found = true;
      }
    }
    if (!found) neighbourEntry->second->push_back(aNeighbourItem);
  };
  

  void IntegratedHcalProcessor::updateGeometryMaps() {

    if (SiPmPropertiesProcessor::empty() || !_mapping.isModuleConditionsDataComplete()) return;
#ifdef HCALDIGI_DEBUG
    std::cout << "calculating neighbour maps" << std::endl;
#endif
    _inverseModuleMap.clear();

    for (unsigned _moduleIndex = 0;  _moduleIndex < _mapping.getNModules(); _moduleIndex++) {
      _inverseModuleMap[(_mapping.getModuleID(_moduleIndex) << 8) + 
			(unsigned short)_mapping.getModuleType(_moduleIndex)-4] = _moduleIndex;	   
    }

    _cellMap.clear();
    const unsigned short minimalTileSize = 3;
    for (unsigned short _moduleIndex = 0; _moduleIndex < _mapping.getNModules(); _moduleIndex++) {
      const unsigned short _module = _mapping.getModuleID(_moduleIndex);
      //  _module ^= module number [1-38]
      //  _moduleIndex = 2*layer - 1
#ifdef HCALDIGI_DEBUG
      std::cout << "_module " << _module << " _moduleIndex " << _moduleIndex << std::endl;
      std::cout << _moduleIndex << ", " << _mapping.getModuleID(_moduleIndex) << ", " << (unsigned) _mapping.getModuleType(_moduleIndex) << std::endl;
#endif  
      const unsigned firstIndex = (_mapping.getModuleType(_moduleIndex) == 7) ? 39 : 0 ;
      const unsigned lastIndex = (_mapping.getModuleType(_moduleIndex) == 6) ? 72 : 108 ;
      for (unsigned short _cellIndex = firstIndex; _cellIndex != lastIndex; _cellIndex++) {
	const unsigned short _channel = _cellIndex % 18;
	const unsigned short _chip = (_cellIndex-_channel) / 18 + (((_mapping.getModuleType(_moduleIndex)==5) || (_mapping.getModuleType(_moduleIndex)==7)) ? 6 : 0);      
	const HcalCellIndex _geomCellIndex(_mapping.getGeometricalCellIndex(_moduleIndex, _cellIndex));
#ifdef HCALDIGI_DEBUG
	std::cout << "module: " << _moduleIndex << " cell: " << _cellIndex << " " << std::hex << _geomCellIndex.getCellIndex() << std::dec << std::endl; 
	std::pair<unsigned, unsigned> test_module_and_cell_index=_indexLookup.getModuleAndCellIndex(_mapping, _geomCellIndex);
	std::cout << "module: " << test_module_and_cell_index.first << " cell:" << test_module_and_cell_index.second << std::endl;
	if ((test_module_and_cell_index.first!=_moduleIndex) || (test_module_and_cell_index.second!=_cellIndex)) {
	  throw logic_error("inverse cellmapping incorrect");
	}
#endif     
	const unsigned short _tileSize = getTileSize(_module, _chip, _channel);
#ifdef HCALDIGI_DEBUG
	std::cout << " considering (module, chip, channel) " << _module << ", " << _chip << ", " << ", " << _channel << std::endl;
	std::cout << "    " << std::hex << _geomCellIndex.getCellIndex() << std::dec << ", " << _tileSize << std::endl;
#endif      
	establishTileCoverage(_geomCellIndex.getCellIndex(), 0, 0);
	if (_tileSize>minimalTileSize) {
	  establishTileCoverage(_geomCellIndex.getCellIndex(), minimalTileSize, 0);
	  establishTileCoverage(_geomCellIndex.getCellIndex(), 0, minimalTileSize);
	  establishTileCoverage(_geomCellIndex.getCellIndex(), minimalTileSize, minimalTileSize);
	  if (_tileSize>2*minimalTileSize) {
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 0, 2*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), minimalTileSize, 2*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 2*minimalTileSize, 2*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 2*minimalTileSize, minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 2*minimalTileSize, 0);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 0, 3*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), minimalTileSize, 3*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 2*minimalTileSize, 3*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 3*minimalTileSize, 3*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 3*minimalTileSize, 2*minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 3*minimalTileSize, minimalTileSize);
	    establishTileCoverage(_geomCellIndex.getCellIndex(), 3*minimalTileSize, 0);
	  }
	}
      }
    
    } 
    for (NeighbourMap::iterator mapIter = _neighbourMap.begin(); mapIter != _neighbourMap.end(); mapIter++) {
      delete mapIter->second;
    }
    _neighbourMap.clear();
    for (std::map<unsigned,unsigned>::iterator mapIter=_cellMap.begin(); mapIter!=_cellMap.end(); mapIter++) {
      //     const unsigned short _centralModule = (mapIter->first & 0xFF0000) >> 16;
      //     const unsigned short _centralChip = (mapIter->first & 0x00FF00) >> 8; 
      //     const unsigned short _centralChannel = mapIter->first & 0x0000FF; 
      considerNeighbourRelation(mapIter->first, -minimalTileSize, 0);
      considerNeighbourRelation(mapIter->first, minimalTileSize, 0);
      considerNeighbourRelation(mapIter->first, 0, -minimalTileSize);
      considerNeighbourRelation(mapIter->first, 0, minimalTileSize);
    }
#ifdef HCALDIGI_DEBUG
    std::cout << "scaling weights" << std::endl;
#endif
    for (NeighbourMap::iterator mapIter = _neighbourMap.begin(); mapIter != _neighbourMap.end(); mapIter++) {
      const unsigned short _centralModule = (mapIter->first & 0xFF0000) >> 16;
      const unsigned short _centralChip = (mapIter->first & 0x00FF00) >> 8; 
      const unsigned short _centralChannel = mapIter->first & 0x0000FF; 
      const unsigned short _centraltileSize = getTileSize(_centralModule, _centralChip, _centralChannel);
#ifdef HCALDIGI_DEBUG
      std::cout << "(" << _centralModule << ", " << _centralChip << ", " << _centralChannel << ") has neighbours: " << std::endl;
#endif    
      for (std::vector<NeighbourItem>::iterator vectorIter = mapIter->second->begin(); vectorIter != mapIter->second->end(); vectorIter++) {
	const unsigned short _chip = (vectorIter->cellkey & 0xFF00) >> 8;
	const unsigned short _channel = vectorIter->cellkey & 0x00FF;
	const unsigned short _tileSize = getTileSize(vectorIter->module, _chip, _channel);
	if (_tileSize) {
	  vectorIter->weight = vectorIter->weight / 4 / (_centraltileSize/minimalTileSize);
#ifdef HCALDIGI_DEBUG
	  std::cout << "  (" << vectorIter->module << ", " << _chip << ", " << _channel << ") " << vectorIter->weight << std::endl; 
#endif
	} else {
	  throw runtime_error("no tilesize available or invalid tilesize");
	}	
      }
    }
  };

  std::vector<IntegratedHcalProcessor::NeighbourItem>* IntegratedHcalProcessor::getNeighbourList(const unsigned short module, const unsigned cellkey) const {
    const unsigned _completeCellkey = (module << 16) + cellkey;
    NeighbourMap::const_iterator neighbourEntry = _neighbourMap.find(_completeCellkey);
    if (neighbourEntry==_neighbourMap.end()) {
#ifdef HCALDIGI_DEBUG
      std::cout << "cell not in neighbour list: size of list " << _neighbourMap.size() << " "<< module << " " << std::hex << cellkey << " -> " << _completeCellkey << std::dec << std::endl;
#endif    
      return 0;
    }  
    return neighbourEntry->second;
  } 

  int IntegratedHcalProcessor::reverseLookup(int I, int J, int K) const {

    streamlog_out(DEBUG) << "reverseLookup() called\n";

    const HcalCellIndex hci(J, I, K);

    const int aTileRow = hci.getTileRow();
    const int aTileColumn = hci.getTileColumn();
    const int aLayer = hci.getLayerIndex();

    const std::pair<unsigned, unsigned> module_and_cell_index = 
      _indexLookup.getModuleAndCellIndex(_mapping, hci);

    const unsigned _moduleIndex = module_and_cell_index.first;

    if (_moduleIndex==0xFF) {
      streamlog_out(WARNING) << "unknown module (" 
			     << "J: " << aTileRow << ", " 
			     << "I: " << aTileColumn  << ", " 
			     << "K: " << aLayer 
			     << ")" 
			     << std::endl;
    }

    const unsigned short _moduleType = _mapping.getModuleType(_moduleIndex);
    const unsigned _cellIndex = module_and_cell_index.second;

    if (_cellIndex==0xFF) {
      streamlog_out(WARNING) << "unknown cell (" 
			     << "J: " << aTileRow << ", " 
			     << "I: " << aTileColumn  << ", " 
			     << "K: " << aLayer << ")" 
			     << std::endl;
    }

    const unsigned chip = 
      _cellIndex / 18 + (((_moduleType == 5) || (_moduleType == 7)) ? 6 : 0);

    const unsigned channel = _cellIndex % 18;

    /*
      modulenumber is the module production number, 1-38
    */
    const unsigned int modulenumber = _mapping.getModuleID(_moduleIndex);

    streamlog_out(DEBUG) << "reverseLookup() finished" << std::endl;

    return CALICE::HcalTileIndex(modulenumber, chip, channel).getIndex();

  }

  int IntegratedHcalProcessor::geometricalLookup(int module, int chip, int channel) {

    short type = 0;
    if ( chip > 5 ) ++type;        //  lower part
    if ( module > 30 ) type += 2;  //  coarse modules

    int moduleID = ( module << 8 ) | type;

    unsigned _moduleIndex = _inverseModuleMap[moduleID];
    unsigned _cellIndex = channel + chip * 18;

    if (_cellIndex>107) _cellIndex=_cellIndex-108; 

    return _mapping.getGeometricalCellIndex(_moduleIndex, _cellIndex);
    
  }

};

