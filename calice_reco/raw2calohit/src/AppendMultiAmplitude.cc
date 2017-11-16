
#include "AppendMultiAmplitude.hh"

// LCIO includes
#include <EVENT/LCParameters.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCGenericObject.h>

// Marlin includes
#include <marlin/ConditionsProcessor.h>
#include <marlin/Exceptions.h>

// CALICE includes
#include "AdcBlock.hh"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

// include connectionDataClass
#include "DAQconnection.hh"


namespace CALICE {

  AppendMultiAmplitude::AppendMultiAmplitude() :  marlin::Processor("AppendMultiAmplitude"),_connectionAvailable(false)  {

    _description = " This processor adds the analog multiplicity counter amplitude as " PAR_MULTI_AMPL " to the event";

    registerInputCollection( LCIO::LCGENERICOBJECT, "ADCCollectionName" ,
                             "The name of the adc collection (input) to be used" ,
                             _adcColName ,
                             std::string(COL_ADC) );

    registerInputCollection( LCIO::LCGENERICOBJECT, "connectionCollectionName" ,
                             "The name of the DAQconnection collection (input) to be used" ,
                             _connectionColName ,
                             std::string("VetoConnection") );

    registerInputCollection( LCIO::LCGENERICOBJECT, "MultiAmplitudeParameterName" ,
                             "The name of the event parameter (input) to be used" ,
                             _parNameMultiAmpl,
                             std::string(PAR_MULTI_AMPL) );
  }


  void AppendMultiAmplitude::conditionsChanged(lcio::LCCollection* col) {
    if (col->getNumberOfElements() != 1) {
      streamlog_out(WARNING1) << "unnatural connection collection size: " << col->getNumberOfElements() << std::endl;
    }
    for (unsigned int elm_i=0;elm_i< (unsigned int)col->getNumberOfElements() ; elm_i++) {
      DAQconnection connection(col->getElementAt(elm_i));
      _crate   = connection.getCrate();
      _slot    = connection.getSlot();
      _fe      = connection.getFe();
      _chip    = connection.getChip();
      _channel = connection.getChannel();
      _connectionAvailable = true;
      streamlog_out(MESSAGE) << "found veto connection info: " << _crate << "," << _slot << "," << _fe << "," << _chip << "," << _channel << std::endl;
    }
  }


  void AppendMultiAmplitude::init() {

    printParameters();

    marlin::ConditionsProcessor::registerChangeListener( this,_connectionColName);

  }


  void AppendMultiAmplitude::processRunHeader( LCRunHeader* run) {

  }


  void AppendMultiAmplitude::processEvent( LCEvent * evt ) {

    if (!_connectionAvailable) {
      streamlog_out(WARNING1) << "No VETO connection information available, cannot process veto" << std::endl;
      return;
    }

    try {
      LCCollection* col_adc = evt->getCollection( _adcColName ) ;

      for (unsigned int elm_i=0;elm_i<(unsigned int)(col_adc->getNumberOfElements()); elm_i++) {
        AdcBlock adc_block(col_adc->getElementAt(elm_i));

        if ((unsigned int)(adc_block.getCrateID()) == _crate
            && (unsigned int)(adc_block.getSlotID()) == _slot
            && (unsigned int)(adc_block.getBoardFrontEnd()) == _fe
            && (unsigned int)(adc_block.getMultiplexPosition()) == _channel) {
          evt->parameters().setValue(_parNameMultiAmpl,(float)(adc_block.getAdcVal(_chip)));

          // there is only one veto counter
          break;
        }
      }
    }
    catch (  DataNotAvailableException &err ) {
    }
  }


  void AppendMultiAmplitude::end()  {
  }


  AppendMultiAmplitude aAppendMultiAmplitude;

}
