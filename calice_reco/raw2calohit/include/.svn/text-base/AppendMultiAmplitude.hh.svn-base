#ifndef AppendMultiAmplitude_hh_
#define AppendMultiAmplitude_hh_

#include <string>
#include <marlin/Processor.h>

#include "lccd/IConditionsChangeListener.hh"
#include "lccd.h"

#include "collection_names.hh"

namespace CALICE {

  /**
   * @brief Processor to add PAR_MULTI_AMPL (amplitude of the Multiplicity Counter) to the event.
   *
   * This processor adds an event parameter containing the amplitude of the analog multiplicity counter readout.
   * The mapping of the multiplicity counter connection is read from the LCIO stream.
   *
   * @version 1.0
   * @author Benjamin.Lutz@desy.de
   * @author Nils.Feege@desy.de
   * @date March 2010
   */
  class AppendMultiAmplitude : public marlin::Processor , public lccd::IConditionsChangeListener {

  public:

    Processor*  newProcessor() { return new AppendMultiAmplitude ; }

    AppendMultiAmplitude();

    ~AppendMultiAmplitude() {};

    void init();

    void processRunHeader( LCRunHeader* run) ;

    void processEvent( LCEvent * evt );

    void end();


  protected:

    // processor parameters

    std::string _adcColName;
    std::string _connectionColName;
    std::string _parNameMultiAmpl;

  private:

    // internal functions & global variables

    unsigned int _crate, _slot, _fe, _chip, _channel;

    void conditionsChanged(lcio::LCCollection* col);

    bool _connectionAvailable;


  };

}
#endif
