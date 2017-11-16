#include "mappingIconditionsProcessor.hh"
 
#include "IMPL/LCCollectionVec.h"
#include "marlin/ConditionsProcessor.h"
#include "marlin/Exceptions.h"
#include "lccd/LCConditionsMgr.hh"
#include "lccd/IConditionsHandler.hh"

#include "AhcConditions.hh"
#include "FeConfigurationBlock.hh"
#include "collection_names.hh"
#include "MappingProcessor.hh"

namespace CALICE {

  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  mappingIconditionsProcessor::mappingIconditionsProcessor(): Processor("mappingIconditionsProcessor")
  {
    _description = "This processor collects the conditions information for the AHCAL modules"
      " and provides a collection with a set for each module.";
    
    registerOutputCollection( LCIO::LCGENERICOBJECT, "OutputCollectionName", 
			      "Name of the output collection",
			      _outputColName, 
			      std::string("AhcConditions"));
    
    registerInputCollection(  LCIO::LCGENERICOBJECT, "AHC_FE_Configuration", 
			      "Name of the conditions collection for the AHC FE configuration",
			      _colAhcFeConf,
			      std::string(COL_AHC_FE_CONF));
    
    registerInputCollection(  LCIO::LCGENERICOBJECT, "AHC_VFE_Configuration", 
			      "Name of the conditions collection for the AHC VFE configuration",
			      _colAhcVfeConf,
			      std::string(COL_AHC_VFE_CONF));
    
    registerProcessorParameter("MappingProcessorName", 
			      "Name of the MappingProcessor",
			       _mappingProcessorName,
			       std::string("AhcMappingProcessor"));
  }
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::init() 
  {
    printParameters();
    
    AhcConditions conditions;

    std::stringstream message;
    bool error = false;
    
    _ahcMapper = dynamic_cast<const AhcMapper*>(MappingProcessor::getMapper(_mappingProcessorName));
    
    if ( ! _ahcMapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName 
		<< ") did not return a valid mapper." << std::endl;
        error = true;
      }
    /* registration of the conditions change listeners has to go here
     *
     * should not break immediately but append report to message and set error to true,
     * if registration not possible
     *
     */
    if (!ConditionsProcessor::registerChangeListener(this, _colAhcFeConf))
      {
        message << " undefined conditions: " << _colAhcFeConf << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _colAhcVfeConf))
      {
        message << " undefined conditions: " << _colAhcVfeConf << std::endl;
        error = true;
      }
     
    if (error) 
      {
	streamlog_out(ERROR) << message.str();
	throw marlin::StopProcessingException(this);
      }
    
    _mapperVersion = _ahcMapper->getVersion();
    _ahcMaxNumberOfModules  = _ahcMapper->getMaxModule();
    _ahcMaxNumberOfChips    = _ahcMapper->getMaxChip();
    _ahcMaxNumberOfChannels = _ahcMapper->getMaxChannel();
    
  }
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::processRunHeader(LCRunHeader* run) 
  {
    _vfeDataAvailable = false;
    _feDataAvailable  = false;  
  }
  
  /***************************************************************************************/
  /* function to listen for condition changes in LCCD                                    */
  /*                                                                                     */
  /* should only remember pointer of collection and set flag for the changed conditions; */
  /* processing should wait till process event. this ensures that constants that depend  */
  /* on two different conditions collections have both collections available             */
  /*                                                                                     */
  /* the callbacks from LCCD have no guarantied order                                    */
  /*                                                                                     */
  /***************************************************************************************/
  void mappingIconditionsProcessor::conditionsChanged( LCCollection * col ) 
  {
    std::string colName = col->getParameters().getStringVal("CollectionName") ;
    
    if (colName == _colAhcFeConf)
      {
        _feCol = col;
        _ahcFeConfigurationChanged = true;
      }
    
    else if (colName == _colAhcVfeConf)
      {
        _vfeCol = col;
        _ahcVfeConfigurationChanged = true;
      }
    
    else 
      {
	streamlog_out(ERROR) << "Called as conditions listener for collection " 
		     << colName << ", but not responsible." << std::endl;
	throw StopProcessingException(this);
      }
    
  }
  
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::setAhcFeConfigCol(EVENT::LCCollection *feCol) 
  {
    if (!feCol) 
      {
	streamlog_out(ERROR) << "setAhcFeConfigCol: change call received for " 
		     << _colAhcFeConf << " but link invalid!!" << std::endl; 
	return;
      }
    
    int fe_confblocks = feCol->getNumberOfElements();
    streamlog_out(DEBUG0)<<" fe_confblocks: "<<fe_confblocks<<std::endl;
    streamlog_out(DEBUG0)<<" ahcMaxNumberOfChips: "<<_ahcMaxNumberOfChips
		 <<" ahcMaxNumberOfChannels: "<<_ahcMaxNumberOfChannels
		 <<std::endl;

    for (int ife = 0; ife < fe_confblocks; ife++) 
      {
	FeConfigurationBlock feConfBlock(feCol->getElementAt(ife)); 
	
	int crate = feConfBlock.getCrateID();
	int slot  = feConfBlock.getSlotID(); 
	int fe    = feConfBlock.getBoardComponentNumber();

	streamlog_out(DEBUG0)<<"   crate/slot/fe="<<crate<<"/"<<slot<<"/"<<fe<<std::endl;
	
	for (unsigned short chip = 0; chip < _ahcMaxNumberOfChips; ++chip)
	  {
	    for (unsigned int channel = 0; channel < _ahcMaxNumberOfChannels; ++channel)
	      {
		bool isValid;
		const int daqChannelID    = _ahcMapper->getDecoder()->getDAQID(crate, slot, fe, chip, channel);

		const unsigned int module = _ahcMapper->getModuleFromDAQID(daqChannelID, isValid);
		if ( ! isValid ) continue;
		
		streamlog_out(DEBUG0)<<"   module/chip/channel="<<module<<"/"<<chip<<"/"<<channel<<std::endl;

		_foundFeData[module] = true;
		_feDataAvailable     = true;
		_calibStart[module]  = feConfBlock.getCalibStart();
		_calibWidth[module]  = feConfBlock.getCalibWidth();
		_calibEnable[module] = feConfBlock.isCalibEnable();
		_hold[module]        = feConfBlock.getHoldStart();
		_holdWidth[module]   = feConfBlock.getHoldWidth();
		_multiplex[module]   = feConfBlock.getVfeMplexClockPulses();
		_vcalib[module]      = feConfBlock.getDacDataTop();  
		_moduleID[module]    = _ahcMapper->getDecoder()->getModuleID(module, chip, channel);

		streamlog_out(DEBUG0)<<"     -->vcalib="<<_vcalib[module]<<std::endl;
		
	      }/*----------------- end loop over channels ---------------*/
	  }/*--------------------- end loop over chips ------------------*/
	
      }/*------- end loop over ife --------------------*/
    
    if (!_feDataAvailable) streamlog_out(WARNING) 
      << "setAhcFeConfigCol: call for fe conditions but could not find any valid module config" 
      << std::endl;
  }
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::setAhcVfeConfigCol(EVENT::LCCollection *vfeCol) 
  {
    if (!vfeCol) 
      {
	streamlog_out(ERROR) << "setAhcVfeConfigCol: change call received for " 
		     << _colAhcVfeConf << " but link invalid!!" << std::endl; 
	return;
      }
    
    /*
      if (!alignmentAvailable()) 
      {
      _vfeSave = vfeCol;
      _condDataSaved = true;
      return;
      }
    */
    
    int vfeConfigBlocks = vfeCol->getNumberOfElements();
    streamlog_out(DEBUG0)<<"\n vfeConfigBlocks="<<vfeConfigBlocks<<std::endl;

    for(int ivfe = 0; ivfe < vfeConfigBlocks; ivfe++) 
      {
	AhcVfeConfigurationBlock vfeConfBlock(vfeCol->getElementAt(ivfe));
	int label = vfeConfBlock.getRecordLabel();
	
	/*for the moment use only the writes -- because of the serial line 
	  the proper reads will come after the NEXT write*/
	if (label == 1) 
	  { 
	    int crate = vfeConfBlock.getCrateID();
	    int slot = vfeConfBlock.getSlotID(); 
	    int fe = vfeConfBlock.getBoardComponentNumber();
	    
	    streamlog_out(DEBUG0)<<"  crate/slot/fe="<<crate<<"/"<<slot<<"/"<<fe<<std::endl;
	    
	    if (slot == 255)  /*slot broadcast*/
	      {
		for (slot = 5; slot < 25; slot++) /*confirm range*/
		  {
		    processVfeForSlot(crate, slot, fe, vfeConfBlock);
		  }
	      }
	    else processVfeForSlot(crate, slot, fe, vfeConfBlock);
	  }
      }/*-------------- end loop over ivfe ----------------------------*/

    if (!_vfeDataAvailable) 
      streamlog_out(WARNING) 
	<< "setAhcVfeConfigCol: call for vfe conditions but could not find any valid module config" 
	<< std::endl;
  }
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::processVfeForSlot(int crate, int slot, int fe, 
						      AhcVfeConfigurationBlock vfeConfBlock) 
  {
    if (fe == 13) /* FE broadcast*/
      { 
	for (fe = 0; fe < 8; fe++)
	  { 
	    processVfe(crate, slot, fe, vfeConfBlock);
	  }
      }
    else processVfe(crate, slot, fe, vfeConfBlock);
  }
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::processVfe(int crate, int slot, int fe, 
					       AhcVfeConfigurationBlock vfeConfBlock) 
  {
    for (unsigned short chip = 0; chip < _ahcMaxNumberOfChips; ++chip)
      {
	for (unsigned int channel = 0; channel < _ahcMaxNumberOfChannels; ++channel)
	  {
	    bool isValid;
	    const int daqChannelID    = _ahcMapper->getDecoder()->getDAQID(crate, slot, fe, chip, channel);
	    const unsigned int module = _ahcMapper->getModuleFromDAQID(daqChannelID, isValid);

	    streamlog_out(DEBUG0)<<"processVfe: module/chip="<<module<<"/"<<chip<<std::endl;
	    if ( ! isValid ) continue;
	    
	    _vfeDataAvailable     = true;
	    _foundVfeData[module] = true;
	    _verification[module] = vfeConfBlock.getVerificationData();
	    _sr[module][chip] = vfeConfBlock.getShiftRegisterData(chip);
	    vfeConfBlock.getShiftRegisterData(chip);
	    
	  }/*----------------- end loop over channels ---------------*/
      }/*--------------------- end loop over chips ------------------*/
    
  }
  
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::updateMapper() 
  {
    _ahcMaxNumberOfModules  = _ahcMapper->getMaxModule();
    _ahcMaxNumberOfChips    = _ahcMapper->getMaxChip();
    _ahcMaxNumberOfChannels = _ahcMapper->getMaxChannel();
    _mapperVersion = _ahcMapper->getVersion();

    _foundFeData  = new bool [_ahcMaxNumberOfModules];
    _foundVfeData = new bool [_ahcMaxNumberOfModules];
    _calibStart   = new int [_ahcMaxNumberOfModules];
    _calibWidth   = new int [_ahcMaxNumberOfModules];
    _calibEnable  = new bool [_ahcMaxNumberOfModules];
    _hold         = new int [_ahcMaxNumberOfModules];
    _holdWidth    = new int [_ahcMaxNumberOfModules];
    _multiplex    = new int [_ahcMaxNumberOfModules];
    _vcalib       = new int [_ahcMaxNumberOfModules];
    _verification = new int [_ahcMaxNumberOfModules];
    
    _moduleID = new unsigned int [_ahcMaxNumberOfModules];
   
    _sr = new int *[_ahcMaxNumberOfModules];
    
    for (unsigned int i = 0; i < _ahcMaxNumberOfModules; i++) 
      {
	_foundFeData[i]  = false;
	_foundVfeData[i] = false;
	_calibStart[i]   = -1;
	_calibWidth[i]   = -1;
	_calibEnable[i]  = -1;
	_hold[i]         = -1;
	_holdWidth[i]    = -1;
	_multiplex[i]    = -1;
	_vcalib[i]       = -1;  
	_verification[i] = -1;
	_moduleID[i]     = 0;
	
	_sr[i] = new int [_ahcMaxNumberOfChips];
	for (unsigned int j = 0; j < _ahcMaxNumberOfChips; j++)
	  {
	    _sr[i][j] = -1;
	  }
      }
  }
  
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::processEvent(LCEvent* evt) 
  {
    streamlog_out(DEBUG0)<<"\n\n EVENT: "<<evt->getEventNumber()<<std::endl;
    
    if (_mapperVersion != _ahcMapper->getVersion()) 
      {
	streamlog_out(DEBUG0) <<" Mapper version changed..."<<std::endl;
	this->updateMapper();
      }
    this->setAhcFeConfigCol(_feCol);
    this->setAhcVfeConfigCol(_vfeCol);
    
    streamlog_out(DEBUG0)<<" _feDataAvailable="<<_feDataAvailable
		 <<" _vfeDataAvailable="<<_vfeDataAvailable<<std::endl;

    if (_feDataAvailable && _vfeDataAvailable) 
      {
	LCCollectionVec* outputCol = new LCCollectionVec(LCIO::LCGENERICOBJECT);
	
	int goodCol = 0;
	
	for (unsigned int i = 1; i < _ahcMaxNumberOfModules; i++) 
	  {
	    streamlog_out(DEBUG0)<<"     ---> module="<<i<<" foundFeData="<<_foundFeData[i]
			 <<" founVfeData="<<_foundVfeData[i]<<std::endl;

	    if (_foundFeData[i] && _foundVfeData[i]) 
	      {
		AhcConditions *aCond = new AhcConditions(i, _moduleID[i], _calibStart[i],
							 _calibWidth[i], _calibEnable[i], 
							 _hold[i],_holdWidth[i], _multiplex[i],
							 _vcalib[i],_verification[i],_sr[i]);
		
		outputCol->addElement(aCond);
		goodCol++;
	      }
	  }/*-------------- end loop over modules ---------------*/
	
	if (goodCol > 0) 
	  {
	    evt->addCollection(outputCol, _outputColName);
	  }
	else 
	  {
	    streamlog_out(WARNING) << "processEvent: feData and vfeData available but no matching pair" 
			   << std::endl;
	  }

	streamlog_out(DEBUG0)<<"\n Event "<<evt->getEventNumber()<<" has "<<outputCol->getNumberOfElements()
		     <<" elements in the "<<_outputColName<<" collection"<<std::endl;
      }
    
  }
  
  
  /******************************************************************/
  /*                                                                */
  /*                                                                */
  /*                                                                */
  /******************************************************************/
  void mappingIconditionsProcessor::end() 
  {
  }
  /***************************************************************************************
   * create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   ***************************************************************************************/
  mappingIconditionsProcessor amappingIconditionsProcessor;

}
