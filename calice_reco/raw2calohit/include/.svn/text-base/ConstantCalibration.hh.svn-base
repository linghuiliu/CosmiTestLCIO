#ifndef _ConstantCalibration_hh_
#define _ConstantCalibration_hh_

#include <Calibration.hh>
#include <cmath>

/** Apply the same calibration constant to all values.
 * Useful to perform a very rough calibration such that all the energy
 * parameters, energy dependent bin limits, etc. need not be changed.
 * The collection name of the calibration constant collection
 * is abused to pass the scale paramer (as a string) to this class 
 * when created by the kit.
 */
class ConstantCalibration : public Calibration
{
public:
  ConstantCalibration(Float_t a_scale) : _scale(a_scale) {};
  
  /** Determine from a pedestal subtracted ADC value a calibrated value which are identical for this very lazy calibrator.
   * @param module_index index of the module a module is considered to be a unit which is always calibrated in one calibration run.
   * @param module_type the type of the detector module
   * @param cell_index
   * @param adc_value the ADC value which should be calibrated.
   */
  Float_t getCalibratedValue(UInt_t module_index, UInt_t module_type, UInt_t cell_index, Float_t adc_value) const {return adc_value * _scale;};

  /** Verify that calibration constants exist for a certain module.
   * @param module_id id Or serial number  of a module which uniquely identifies a detector module of a certain type.
   * @param module_type the type of the detector module
   * @param n_cells the number of cells on this module.
   * Return true if calibration constants exist for the given module and the number of cells matches the number of calibration constants.
   */
  Bool_t checkForCalibrationConstantsOfModule(UInt_t module_id, UInt_t module_type, UInt_t n_cells) const {return true;};

  /** Get the minimum adc value mips will have for the given energy fraction on all pads.
   * @param mip_energy_fraction the energy in mips
   * @return minimum adc value which is below the given energy on all pads.
   * This method can be used to get the lowest adc value a mip of the given energy fraction
   * will have on all pads. This value is useful to select candidates before actually performing th
   * calibration.
   * This function is intended to be called at the beginning of each event.
   */
  Int_t  getMiniumADCForMipThreshold(Float_t mip_energy_fraction) const {
    return static_cast<Int_t>(floor(mip_energy_fraction / _scale));
  };

private:
  Float_t _scale;
};
#endif
