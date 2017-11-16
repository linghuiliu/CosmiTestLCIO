#ifndef SatCorrFunction_hh
#define SatCorrFunction_hh

#include "EVENT/LCGenericObject.h"

namespace CALICE {

  /**
   * interface for a function to correct (or apply) the SiPM saturation
   *
   * This function has to return a saturated or saturation corrected
   * amplitude for a specific description of the saturation.
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date October 2009
   */

  class SatCorrFunction {
  public:
    virtual ~SatCorrFunction() {}

    /**
     * apply saturation correction to a signal amplitude
     * @param saturatedSignal saturated signal
     * @returns unsaturated signal
     */
    virtual float deSaturate(const float saturatedSignal) const = 0;
    /**
     * error after application of saturation correction to a signal
     * @param saturatedSignal saturated signal
     * @param saturatedSignalError error of the saturated signal
     * @returns error of the unsaturated signal
     */
    virtual float deSaturatedError(const float saturatedSignal,
                                   const float saturatedSignalError) const = 0;

    /**
     * apply saturation to a signal amplitude
     * @param unsaturatedSignal unsaturated signal
     * @returns saturated signal
     */
    virtual float saturate(const float unsaturatedSignal) const = 0;
    /**
     * error after application of saturation to a signal
     * @param unsaturatedSignal unsaturated signal
     * @param unsaturatedSignalError error of the unsaturated signal
     * @returns error of saturated signal
     */
    virtual float saturatedError(const float unsaturatedSignal,
                                 const float unsaturatedSignalError) const = 0;

    /**
     * get the applied scaling
     */
    virtual float getScaling() const = 0;
  };

} // end namespace CALICE

#endif
