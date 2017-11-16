#ifndef TcmtMipCalibrationProcessor_h
#define TcmtMipCalibrationProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "FastCalibrationProcessor.hh"
#include "MIPConstants.hh"
#include "TcmtHit.hh"

namespace CALICE {

  /**
   * Simple class to apply MIP calibration to TCMT strips
   * @author Guilherme Lima (NIU)
   * @date June 15 2007
   */

  char TcmtMipCalibProcessorName[] = "TcmtMipCalibrationProcessor";

  typedef FastCalibProcessor<MIPConstants,TcmtHit,TcmtMipCalibProcessorName> TcmtMipCalibrationProcessor;
}

#endif
