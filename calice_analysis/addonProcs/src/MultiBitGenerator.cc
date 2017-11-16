#include <sstream>

#include <iostream>

#include "marlin/ConditionsProcessor.h"

#include "MultiBitGenerator.hh"
#include "AdcBlock.hh"

#include "EVENT/LCParameters.h"
#include "EVENT/LCCollection.h"


namespace CALICE {
  MultiBitGenerator aMultiBitGenerator;

  MultiBitGenerator::MultiBitGenerator() :  marlin::Processor("MultiBitGenerator"), _thresholdAvailable(false) {

    registerProcessorParameter( "parNameMultiAmplitude" ,
                                "The name of the veto amplitude parameter" ,
                                _parNameMultiAmplitude ,
                                std::string(PAR_MULTI_AMPL) );

    registerProcessorParameter( "parNameMultiBit" ,
                                "The name of the output parameter (multiplicity bit)" ,
                                _parNameMultiBit ,
                                std::string(PAR_MULTI_BIT) );

    registerProcessorParameter( "purity" ,
                                "required purity for single particle sample in percent (90, 95, 99, 99.9)" ,
                                _setPurity ,
                                (float)99.9 );

    registerProcessorParameter( "useFixedThreshold" ,
                                "false - use threshold based on chosen purity, true - specify threshold via parameter threshold" ,
                                _fixThreshold ,
                                (bool)false );

    registerProcessorParameter( "threshold" ,
                                "set the amplitude threshold for the multiplicity bit" ,
                                _setThreshold ,
                                (float)0 );


    registerProcessorParameter( "thresholdCollectionName" ,
				"The name of the threshold collection (input)" ,
				_thresholdColName ,
				std::string("MultiThreshold") );

  }


  void MultiBitGenerator::conditionsChanged(lcio::LCCollection* col) {

    for (int i=0; i < col->getNumberOfElements(); i++){
      CALICE::VetoThreshold* vetoThrValues = new CALICE::VetoThreshold(col->getElementAt(i));

      if ( vetoThrValues->getPurity() == _setPurity ) {

        _effPurity = vetoThrValues->getEffectivePurity();
        _setThreshold = vetoThrValues->getThreshold();
        _setThresholdErr = vetoThrValues->getThresholdError();

        _thresholdAvailable = true;

        std::cout << "Set purity: " << _setPurity << std::endl;
        std::cout << "Effective purity: " << _effPurity << std::endl;
        std::cout << "Set threshold: " << _setThreshold << std::endl;
        std::cout << "Threshold error: " << _setThresholdErr << std::endl;

      }

    }

  }


  void MultiBitGenerator::init() {

    printParameters();


    if( _fixThreshold ){

      std::cout << "Using fixed threshold of " << _setThreshold << " ADC channels" << std::endl;

      _thresholdAvailable = true;

    }
    else{

      std::cout << "Using thresholds for " << _setPurity << "% purity from collection " << _thresholdColName << std::endl;

      marlin::ConditionsProcessor::registerChangeListener( this, _thresholdColName);

    }

  }


  void MultiBitGenerator::processRunHeader( LCRunHeader* run) {

  }


  void MultiBitGenerator::processEvent( LCEvent * evt ) {

    float multiAmpl = evt->getParameters().getFloatVal(_parNameMultiAmplitude);

    int MultiBit;

    if (multiAmpl < _setThreshold) MultiBit = 0;
    else MultiBit = 1;

    evt->parameters().setValue(_parNameMultiBit,MultiBit);

  }


  void MultiBitGenerator::end() {

  }


}
