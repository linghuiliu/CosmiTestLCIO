#include "PedestalProcessor.hh"

#include <cmath>
#include <cassert>

/*ILC*/
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"
#include "lccd/LCConditionsMgr.hh"
#include "marlin/Exceptions.h"

/*CALICE*/
#include "collection_names.hh"
#include "TriggerBits.hh"
#include "MappingProcessor.hh"
#include "SimpleValue.hh"
#include "AdcBlock.hh"

using std::cout;
using std::endl;


namespace CALICE {

  PedestalProcessor aPedestalProcessor;

  PedestalProcessor::PedestalProcessor() : Processor("PedestalProcessor")
  {
    _description = "Pedestal subtraction using pedestal extracted on the fly from the same run.";

    registerProcessorParameter("InputCollection",
                               "Name of input collection of CaliceHits",
                               _inputColName,
                               std::string("RawCaliceHits"));

    registerProcessorParameter("OutputCollection",
                               "Name of output collection of pedestals",
                               _outputColName,
                               std::string("Pedestal"));

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides"
                                " the geometry of the detector.",
                                _mappingProcessorName,
                                std::string("AHC") ) ;

    registerProcessorParameter("minPedNumber", "Minimum number of pedestal "
                               "before the pedestal value is considered "
                               "valid and pedestal substraction is applied",
                               _minPedNumber,
                               (int) 450);

    registerProcessorParameter("skipMinimumEvent", "Skip the mipPedNumber events",
                               _skipMinimumEvent,
                               (bool) false);

  }

  /*****************************************************************************/
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*****************************************************************************/
  PedestalProcessor::~PedestalProcessor()
  {
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*****************************************************************************/
  void PedestalProcessor::init(){
    printParameters();

    _pedCounter = 0;

    std::stringstream message;
    message << "undefined conditionsdata: ";
    bool error = false;

   /*-------------------------------------------------------------*/
    _mapper = dynamic_cast<const AhcMapper*>(MappingProcessor::getMapper(_mappingProcessorName));
    if ( ! _mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName
                << ") did not return a valid mapper." << endl;
        error = true;
      }

    if (error)
      {
        streamlog_out(ERROR) << message.str();
        throw marlin::StopProcessingException(this);
      }

    _mapperVersion = _mapper->getVersion();
    /*-------------------------------------------------------------*/
    _colPedestal = NULL;

    _conditionsHandler = new CALICE::RunTimeConditionsHandler(_outputColName);
    lccd::LCConditionsMgr::instance()->registerHandler(_outputColName,_conditionsHandler);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*****************************************************************************/
  void PedestalProcessor::updateMapper()
  {
    _ahcMaxNumberOfModules  = _mapper->getMaxModule();
    _ahcMaxNumberOfChannels = _mapper->getMaxChannel();
    _ahcMaxNumberOfChips    = _mapper->getMaxChip();
    _ahcMaxNumberOfCells    = _ahcMaxNumberOfChannels * _ahcMaxNumberOfChips;

    streamlog_out(DEBUG0)<<"\n\n\n ahcMaxNumberOfModules: "<<_ahcMaxNumberOfModules
		 <<" ahcMaxNumberOfChannels: "<<_ahcMaxNumberOfChannels
		 <<" ahcMaxNumberOfChips: "<<_ahcMaxNumberOfChips
		 <<" ahxMaxNumberOfCells: "<<_ahcMaxNumberOfCells
		 <<endl;
    
    _pedSum       = new float *[_ahcMaxNumberOfCells];
    _pedSumSquare = new float *[_ahcMaxNumberOfCells];
    _pedNum       = new unsigned int *[_ahcMaxNumberOfCells];
    _ped          = new float *[_ahcMaxNumberOfCells];
    _pedError     = new float *[_ahcMaxNumberOfCells];

    for (unsigned int mod = 0; mod < _ahcMaxNumberOfModules; ++mod)
      {
        _pedSum[mod]       = new float [_ahcMaxNumberOfCells];
        _pedSumSquare[mod] = new float[_ahcMaxNumberOfCells];
        _pedNum[mod]       = new unsigned int[_ahcMaxNumberOfCells];
        _ped[mod]          = new float[_ahcMaxNumberOfCells];
        _pedError[mod]     = new float[_ahcMaxNumberOfCells];

	for (unsigned int cell = 0; cell < _ahcMaxNumberOfCells; ++cell)
	  {
	    _pedSum[mod][cell]       = 0;
	    _pedSumSquare[mod][cell] = 0;
	    _pedNum[mod][cell]       = 0;
	    _ped[mod][cell]          = 0;
	    _pedError[mod][cell]     = 0;
	  }

      }

    _mapperVersion = _mapper->getVersion();
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*****************************************************************************/
  void PedestalProcessor::processEvent(LCEvent *evt)
  {
    streamlog_out(DEBUG0) <<"\n EVENT: "<<evt->getEventNumber()<<endl;
 
    TriggerBits old_trigger_conf=_triggerConf;
    _triggerConf = evt->getParameters().getIntVal(PAR_TRIGGER_CONF);

    /*check mapper version and update mapper if necessary,
      since the mapper gets updated with the first event*/
    if (_mapperVersion != _mapper->getVersion()) this->updateMapper();

    /*look only at pedestal events*/
    TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));

    if( _triggerConf != old_trigger_conf ) {
      streamlog_out(DEBUG) << " --------- Trigger Conf Change Occured -----------" << endl;
      if( trigBits.isPurePedestalTrigger() == 1 && _triggerConf.getTriggerBits() == 4 ){
	streamlog_out(DEBUG) << "Yes, it is Trigger configure 1. Cleanup Pedestal." << endl;
	//prepare for next 500 pedestal
	for (unsigned short mod = 1; mod < _ahcMaxNumberOfModules; ++mod)
	  {
	    for (unsigned short cell = 0; cell < _ahcMaxNumberOfCells; ++cell)
	      {
		_pedSum[mod][cell]        = 0.0;
		_pedSumSquare[mod][cell]  = 0.0;
		_pedNum[mod][cell]        = 0;
	      }
	  }
	_pedCounter = 0;
      }else if( trigBits.isCalibTrigger() == 1 &&  _triggerConf.getTriggerBits() == 20 ){
	streamlog_out(DEBUG) << "Yes, it is Trigger configure 2.  Update  Pedestal." << endl;
	streamlog_out(DEBUG) << "There are in total "<< _pedCounter <<" pedestal events in last configuration will be used for the following beam event." <<endl;

        _colPedestal = new LCCollectionVec(LCIO::LCGENERICOBJECT);

	/*AHCAL MODULE: module starts from 1, not from 0!*/
	/*
	  BE AWARE: done for 38x216 channels! no check for coarse modules!
	*/
	for (unsigned short mod = 1; mod < _ahcMaxNumberOfModules; ++mod)
	  {
	    for (unsigned short cell = 0; cell < _ahcMaxNumberOfCells; ++cell)
	      {
		unsigned int channel = cell % 18;
		unsigned int chip    = cell/18;
		
		try
		  {
		    const int cellID = _mapper->getCellIDFromModChipChan(mod, chip, channel);
		    
		    SimpleValue *pedestal = new SimpleValue(cellID, _ped[mod][cell],
							    _pedError[mod][cell], 0);
		    
		    _colPedestal->addElement(pedestal);
		    
		    streamlog_out(DEBUG0)<<" mod="<<mod<<" cell="<<cell<<" pedestal="
				 <<_ped[mod][cell]<<" +- "
				 <<_pedError[mod][cell]
				 <<" pedNum="<<_pedNum[mod][cell]
				 <<" pedSum="<<_pedSum[mod][cell]
				 <<" pedSumSquare="<<_pedSumSquare[mod][cell]
				 <<endl;
		  }
		catch(BadDataException& e)
		  {
		    streamlog_out(DEBUG0) << " invalid cell id for module " << mod 
				  <<", chip "<<chip
				  <<" and channel "<<channel<<" \n"<< e.what() 
				  << endl;
		  }
	      }
	  }/*------------- end of loop over mod--------------*/
	
	const std::string encoding = _mapper->getDecoder()->getCellIDEncoding();
	LCParameters &theParam =  _colPedestal->parameters();
	theParam.setValue(LCIO::CellIDEncoding, encoding);
	
	_conditionsHandler->update(_colPedestal);
	
	streamlog_out(DEBUG0)<<" updating colPedestal: "<<_colPedestal<<endl;
	
      }else{ 
	streamlog_out(DEBUG) <<"Yes, it is trigger configuration 3. PedestalProcessor will do nothing, return!" <<std::endl;
	return;
      }
    }else if( _pedCounter == 501 ){ //For pure noise runs

      streamlog_out(DEBUG) << "For pure noise runs.  Update  Pedestal once after first 500 events." << endl;

      _colPedestal = new LCCollectionVec(LCIO::LCGENERICOBJECT);

      /*AHCAL MODULE: module starts from 1, not from 0!*/
      /*
	BE AWARE: done for 38x216 channels! no check for coarse modules!
      */
      for (unsigned short mod = 1; mod < _ahcMaxNumberOfModules; ++mod)
	{
	  for (unsigned short cell = 0; cell < _ahcMaxNumberOfCells; ++cell)
	    {
	      unsigned int channel = cell % 18;
	      unsigned int chip    = cell/18;
	      
	      try
		{
		  const int cellID = _mapper->getCellIDFromModChipChan(mod, chip, channel);
		  
		  SimpleValue *pedestal = new SimpleValue(cellID, _ped[mod][cell],
							  _pedError[mod][cell], 0);
		  
		  _colPedestal->addElement(pedestal);
		  
		  streamlog_out(DEBUG0)<<" mod="<<mod<<" cell="<<cell<<" pedestal="
			       <<_ped[mod][cell]<<" +- "
			       <<_pedError[mod][cell]
			       <<" pedNum="<<_pedNum[mod][cell]
			       <<" pedSum="<<_pedSum[mod][cell]
			       <<" pedSumSquare="<<_pedSumSquare[mod][cell]
			       <<endl;
		}
	      catch(BadDataException& e)
		{
		  streamlog_out(DEBUG0) << " invalid cell id for module " << mod 
				<<", chip "<<chip
				<<" and channel "<<channel<<" \n"<< e.what() 
				<< endl;
		}
	    }
	}/*------------- end of loop over mod--------------*/
      
      const std::string encoding = _mapper->getDecoder()->getCellIDEncoding();
      LCParameters &theParam =  _colPedestal->parameters();
      theParam.setValue(LCIO::CellIDEncoding, encoding);
      
      _conditionsHandler->update(_colPedestal);
      
      streamlog_out(DEBUG0)<<"updating colPedestal for each 500 pedstal counts: "<<_colPedestal<<endl;
    }    
    

    if ( (!trigBits.isPurePedestalTrigger()) || (_triggerConf.getTriggerBits() != 4) )  
      {
	streamlog_out(DEBUG0) <<" This event ("<<evt->getEventNumber()
		     <<") has no pure pedestal trigger, return"<<endl;
	return;
      }

  
    LCCollection *inputCol = NULL;
    try
      {
        inputCol = evt->getCollection(_inputColName);
        int noElem = inputCol->getNumberOfElements();

	streamlog_out(DEBUG0)<<"  noElem = "<<noElem<<endl;


        if (noElem > 0)
          {
            for (unsigned int i = 0; i < static_cast<unsigned int>(noElem); ++i)
              {
                LCObject *obj = inputCol->getElementAt(i);
                AdcBlock adcBlock(obj);

               short channel = adcBlock.getMultiplexPosition();


                for (unsigned short chip = 0; chip < _ahcMaxNumberOfChips; ++chip)
                  {
		    bool isValid;
                    const int daqChannelID = _mapper->getDecoder()->getDAQID(adcBlock.getCrateID(),//crate 
									     adcBlock.getSlotID(),//slot
									     adcBlock.getBoardFrontEnd(),//fe
									     chip, channel);
                    const unsigned int module = _mapper->getModuleFromDAQID(daqChannelID, isValid);
                    if ( ! isValid ) continue;

                    unsigned short cell = chip * 18 + channel;

                    assert( (module < _ahcMaxNumberOfModules) && (cell < _ahcMaxNumberOfCells));

                    float energy = (float)adcBlock.getAdcVal(chip);

                    _pedSum[module][cell] += energy;
                    _pedSumSquare[module][cell] += pow(energy, 2);
                    _pedNum[module][cell]++;

                    /*the pedestal is the average of all pedestal "hits"*/
                    _ped[module][cell] = _pedSum[module][cell]/_pedNum[module][cell];

                    /*the pedestal error is RMS/sqrt(N)*/
                    if (_pedNum[module][cell] > 0)
                      _pedError[module][cell] =
                        sqrt(_pedSumSquare[module][cell]/_pedNum[module][cell]
                             - pow(_pedSum[module][cell]/_pedNum[module][cell],2))
                        /sqrt(_pedNum[module][cell]);
                    else
                      _pedError[module][cell] = 0;


                  }/*------------------------- end loop over chip ----------------------------*/
              }/*----------------------------- end loop over i < noElem ---------------------*/


            _pedCounter++;



	    if (_skipMinimumEvent && _pedCounter < _minPedNumber) 
	      {
		streamlog_out(DEBUG0)<<"skip event "<<evt->getEventNumber()<<endl;
		throw marlin::SkipEventException(this);
	      }

          }/*--------- end if (noElem > 0)---------------------------*/


      }
    catch (EVENT::DataNotAvailableException e){
      streamlog_out(DEBUG0) << "No pedestal collection " << _inputColName << " found" << endl;
      return;
    }

  }

  /*****************************************************************************/
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*                                                                           */
  /*****************************************************************************/
  void PedestalProcessor::end()
  {
    for (unsigned int mod = 0; mod < _ahcMaxNumberOfModules; ++mod)
      {
	delete [] _pedSum[mod] ;
	delete [] _pedSumSquare[mod];
	delete [] _pedNum[mod];
	delete [] _ped[mod];
	delete [] _pedError[mod];
      }

    delete [] _pedSum ;
    delete [] _pedSumSquare;
    delete [] _pedNum;
    delete [] _ped;
    delete [] _pedError;
      
  }
    
}/*end of namespace CALICE*/
