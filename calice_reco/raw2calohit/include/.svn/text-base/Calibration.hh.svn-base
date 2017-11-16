#ifndef _Calibration_hh_
#define _Calibration_hh_

#include <RtypesSubSet.h>

/** Abstract interface of a calibration object.
 * The calibration object returns for a value measured with a certain cell a calibrated value.
 */
class Calibration
{
public:
  virtual ~Calibration() {};

  /** Determine from a pedestal subtracted ADC value a calibrated value.
   * @param module_id id Or serial number  of a module which uniquely identifies a detector module of a certain type.
   * @param module_type the type of the detector module
   * @param cell_index the cell_index in read order: first the ADC values of the first sample from all chips, then the next sample ...
   * @param adc_value the ADC value which should be calibrated.
   * @return the calibrated value.
   */
  virtual Float_t getCalibratedValue(UInt_t module_id, UInt_t module_type, UInt_t cell_index, Float_t adc_value) const = 0;

  /** Return true if a the calibration constants for the given cell are in the allowed range.
   * @param module_id id Or serial number  of a module which uniquely identifies a detector module of a certain type.
   * @param module_type the type of the detector module
   * @param cell_index the cell_index in read order: first the ADC values of the first sample from all chips, then the next sample ...
   * @return true for cells with valid calibration constants, false for cells which should be declared dead.
   *
   * This method is used to find, after module connection changes, cells which should be declared dead.
   */
  virtual Bool_t isValid(UInt_t module_id, UInt_t module_type, UInt_t cell_index) const {return true;};
  
  /** Verify that calibration constants exist for a certain module.
   * @param module_id id Or serial number  of a module which uniquely identifies a detector module of a certain type.
   * @param module_type the type of the detector module
   * @param n_cells the number of cells on this module.
   * Return true if calibration constants exist for the given module and the number of cells matches the number of calibration constants.
   */
  virtual Bool_t checkForCalibrationConstantsOfModule(UInt_t module_id, UInt_t module_type, UInt_t n_cells) const =0;

  /** Get the minimum adc value mips will have for the given energy fraction on all pads.
   * @param mip_energy_fraction the energy in mips
   * @return minimum adc value which is below the given energy on all pads.
   * This method can be used to get the lowest adc value a mip of the given energy fraction
   * will have on all pads. This value is useful to select candidates before actually performing th
   * calibration.
   * This function is intended to be called at the beginning of each event.
   */
  virtual Int_t  getMiniumADCForMipThreshold(Float_t mip_energy_fraction) const =0;
};
#endif
