#ifndef TcmtMappingIProcessor_h
#define TcmtMappingIProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "EVENT/LCEvent.h"
#include "VRawADCValueProcessor.hh"

namespace CALICE {

  /*** A class which converts TCMT ADChits into intermediate CaliceHits which can be used
   * for calibration, etc.
   * @author G.Lima (NIU), adapted from S.Schmidt's original Ahcal code
   * @date July 17 2007
   */

  class TcmtMappingIProcessor : public VRawADCValueProcessor {

  public:

    virtual Processor* newProcessor() { return new TcmtMappingIProcessor; }

    TcmtMappingIProcessor();

    virtual void init();

    virtual void processRunHeader(LCRunHeader* run){};

    virtual void processEvent(LCEvent* evt);

    virtual void check(LCEvent* evt){};

    virtual void end(){};

  protected:

    std::string _outputColName;
    int _viewConnectionTree;
    int _pickModule;
  };

}

#endif
