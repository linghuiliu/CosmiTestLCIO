#include "TcmtEventTypeProcessor.hh"

namespace CALICE {

  TcmtEventTypeProcessor::TcmtEventTypeProcessor() : Processor("TcmtEventTypeProcessor") {

    _description = "Processor that analyses the type of signal in the TCMT";

    registerInputCollection(LCIO::CALORIMETERHIT,
                            "InputCollection",
                            "name of the TCMT input collection of calorimeter hits",
                            _inputColName,
                            std::string("TcmtCalorimeter_Hits") );

    registerProcessorParameter("TcmtEventBitName",
                               "Name of the event parameter name which contains the current TCMT event type bits .",
                               _parNameTcmtEventBit,
                               std::string("TcmtEventBit"));

    registerOptionalParameter("OverwriteEncoding",
                              "To fix a bad encoding string of the input collection set this to an alternative encoding string.",
                              _alternativeEncoding,
                              std::string("") );

  }


  void TcmtEventTypeProcessor::init() {

    // usually a good idea
    printParameters();

    _tcmtEventIdentifier = new TcmtEventIdentifier(16,9,_alternativeEncoding);

  }

  void TcmtEventTypeProcessor::processEvent(  LCEvent *evt ) {
    streamlog_out(DEBUG)<<"\n Event "<<evt->getEventNumber()<<" encoding: "<<_alternativeEncoding<<std::endl;

    try {

      LCCollection* col = evt->getCollection( _inputColName );

      _tcmtEventIdentifier->process(col);
      _tcmtEventIdentifier->addResults(col);
      _tcmtEventIdentifier->addResults(_parNameTcmtEventBit, evt);

    }
    catch ( DataNotAvailableException &err) {
      streamlog_out(DEBUG) << "input collection " << _inputColName << " not available in event " << evt->getEventNumber() << std::endl;
    }

  }

  void TcmtEventTypeProcessor::end() {
    delete _tcmtEventIdentifier;
  }


  /* create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   */
  TcmtEventTypeProcessor aTcmtEventTypeProcessor;

} // end namespace CALCICE
