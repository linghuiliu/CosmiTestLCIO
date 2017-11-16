#ifndef FastInterCalibrationProcessor_h
#define FastInterCalibrationProcessor_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <FastCalibrationProcessor.hh>
#include <InterConstants.hh>
#include <FastCaliceHit.hh>

namespace CALICE {

/**
  * Simple class to demonstrate the usage of the CalibProcessor template
  * All the real code is encapsulated in the MIPConstants class
  */

char FastInterProcessorName[] = "FastInterCalibrationProcessor";

  typedef FastCalibProcessor<InterConstants,FastCaliceHit,FastInterProcessorName> FastInterCalibrationProcessor;

}

#endif
