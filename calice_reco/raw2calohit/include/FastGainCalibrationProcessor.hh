#ifndef FastGainCalibrationProcessor_h
#define FastGainCalibrationProcessor_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <FastCalibrationProcessor.hh>
#include <GainConstants.hh>
#include <FastCaliceHit.hh>

namespace CALICE {

/**
  * Simple class to demonstrate the usage of the CalibProcessor template
  * All the real code is encapsulated in the GainConstants class
  */

char FastGainProcessorName[] = "FastGainCalibrationProcessor";

  typedef FastCalibProcessor<GainConstants,FastCaliceHit,FastGainProcessorName> FastGainCalibrationProcessor;

}

#endif
