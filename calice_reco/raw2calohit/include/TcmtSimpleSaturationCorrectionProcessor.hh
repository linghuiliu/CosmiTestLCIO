#ifndef TcmtSimpleSaturationCorrectionProcessor_h
#define TcmtSimpleSaturationCorrectionProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <TcmtHit.hh>
#include <TcmtSaturationConstants.hh>
#include "FastCalibrationProcessor.hh"

namespace CALICE {

  /**
   * Simple class to apply a simplified saturation correction to TCMT hits
   * @author Guilherme Lima, Kurt Francis (NIU)
   * @date March 15 2008
   */

  char TcmtSaturationCalibProcessorName[] = "TcmtSimpleSaturationCorrectionProcessor";

  typedef FastCalibProcessor<TcmtSaturationConstants,TcmtHit,TcmtSaturationCalibProcessorName> TcmtSimpleSaturationCorrectionProcessor;
}

#endif
