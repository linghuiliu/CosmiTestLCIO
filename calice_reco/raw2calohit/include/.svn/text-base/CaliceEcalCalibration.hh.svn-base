#ifndef _CaliceEcalCalibration_hh_
#define _CaliceEcalCalibration_hh_

#include <RtypesSubSet.h>
#include <Calibration.hh>
#include <string>
#include <map>
#include <stdexcept>
#include <ConditionsChangeDelegator.hh>
#include <EcalModuleCalibration.hh>
#include <cmath>
#include <iostream>

namespace CALICE {

/** Calibration of the Calice ECAL modules
 */
class CaliceEcalCalibration : public Calibration
{
public:
  /** Create the calibration object for the Calice Ecal.
   * The constructor will register handlers for conditions data changes: changes of the module types 
   * and the calibration constants. 
   */
  CaliceEcalCalibration(const std::string &module_type_col_name, const std::string &calibration_constants_col_name)
        throw(std::runtime_error);

  /** Destructor.
   * FIXME: The destructor should deregister the conditions change handler. Unfortuneatly, this 
   * is not forseen in the Conditions Processor.
   */
  ~CaliceEcalCalibration() {};


  /** Determine from a pedestal subtracted ADC value a calibrated value.
   * @param module_id id Or serial number  of a module which uniquely identifies a detector module of a certain type.
   * @param module_type the type of the detector module
   * @param cell_index the cell_index in read order: first the ADC values of the first sample from all chips, then the next sample ...
   * @param adc_value the ADC value which should be calibrated.
   * @return the calibrated value.
   */
  Float_t getCalibratedValue(UInt_t module_id, UInt_t module_type, UInt_t cell_index, Float_t adc_value) const 
  {
#ifdef BOUNDARY_CHECK
    if (module_type >= _calibrationConstantsPerType.size()) {
      throw std::runtime_error("CaliceEcalCalibration::getCalibratedValue> The given module type is out of range");
    }

    if (module_id >=  _calibrationConstantsPerType[module_type].first->size()) {
      throw std::runtime_error("CaliceEcalCalibration::getCalibratedValue> The given module serial number is out of range");
    }

    if (cell_index >= (*_calibrationConstantsPerType[module_type].first)[module_id].getNCells()) {
      throw std::runtime_error("CaliceEcalCalibration::getCalibratedValue> The cell index is out of range.");
    }
#endif
    return adc_value*(*_calibrationConstantsPerType[module_type].first)[module_id].getCalibrationConstant(cell_index);
  };

  /** Return true if a the calibration constants for the given cell are in the allowed range.
   * @param module_id id Or serial number  of a module which uniquely identifies a detector module of a certain type.
   * @param module_type the type of the detector module
   * @param cell_index the cell_index in read order: first the ADC values of the first sample from all chips, then the next sample ...
   * @return true for cells with valid calibration constants, false for cells which should be declared dead.
   *
   * This method is used to find, after module connection changes, cells which should be declared dead.
   */
  Bool_t isValid(UInt_t module_id, UInt_t module_type, UInt_t cell_index) const
  {
#ifdef BOUNDARY_CHECK
    if (module_type >= _calibrationConstantsPerType.size()) {
      throw std::runtime_error("CaliceEcalCalibration::isValid> The given module type is out of range");
    }

    if (module_id >=  _calibrationConstantsPerType[module_type].first->size()) {
      std::cout << "module_id=" << module_id 
	        << _calibrationConstantsPerType[module_id].second
		<< " " << (void *) _calibrationConstantsPerType[module_id].first 
		<< " list=" << _calibrationConstantsPerType[module_id].first->size() 
		<< std::endl;
      throw std::runtime_error("CaliceEcalCalibration::isValid> The given module serial number is out of range");
    }

    if (cell_index >= (*_calibrationConstantsPerType[module_type].first)[module_id].getNCells()) {
      throw std::runtime_error("CaliceEcalCalibration::isValid> The cell index is out of range.");
    }
#endif

    // FIXME: Is zero or negative values a good marker for dead cells ?
    return ( (*_calibrationConstantsPerType[module_type].first)[module_id].getCalibrationConstant(cell_index) > 0. );
  };

  /** Verify that calibration constants exist for a certain module.
   * @param module_id id Or serial number  of a module which uniquely identifies a detector module of a certain type.
   * @param module_type the type of the detector module
   * @param n_cells the number of cells on this module.
   * Return true if calibration constants exist for the given module and the number of cells matches the number of calibration constants.
   */
  Bool_t checkForCalibrationConstantsOfModule(UInt_t module_id, UInt_t module_type, UInt_t n_cells) const {

    if (module_type >= _calibrationConstantsPerType.size()) return false;
    if (module_id >=  _calibrationConstantsPerType[module_type].first->size()) return false;
    return n_cells==(*_calibrationConstantsPerType[module_type].first)[module_id].getNCells();
  };

  /** Get the minimum adc value mips will have for the given energy fraction on all pads.
   * @param mip_energy_fraction the energy in mips
   * @return minimum adc value which is below the given energy on all pads.
   * This method can be used to get the lowest adc value a mip of the given energy fraction
   * will have on all pads. This value is useful to select candidates before actually performing th
   * calibration.
   * This function is intended to be called at the beginning of each event.
   */
  Int_t  getMiniumADCForMipThreshold(Float_t mip_energy_fraction) const {
    return static_cast<Int_t>(floor(mip_energy_fraction *_minInvCalibrationConstant));
  }

    /** Notify the calibration object in case the definition of the module types has changed.
   * This function must be called at least once before \ref getCalibratedValue() is used.
   */
  void moduleTypeChanged(lcio::LCCollection* col);

  /** Notify the calibration object when ever the calibration constants change.
   * This function must be called at least once before \ref getCalibratedValue() is used.
   */
  void calibrationConstantChanged(lcio::LCCollection* col);

private:

  typedef std::vector< EcalModuleCalibration> ModuleTypeCalibrationConstantList_t;

  std::map<std::string, ModuleTypeCalibrationConstantList_t > _calibrationConstants;

  typedef std::vector< std::pair<const ModuleTypeCalibrationConstantList_t *,std::string> > CalibrationConstantPerTypeList_t;
  CalibrationConstantPerTypeList_t _calibrationConstantsPerType;  
  /**< Calibration constatns per Module. The first array component is the module type, the second the serial number of the module.*/

  ConditionsChangeDelegator<CaliceEcalCalibration> _moduleTypeChange;
  ConditionsChangeDelegator<CaliceEcalCalibration> _calibrationConstantChange;

  static EcalModuleCalibration _empty; /**< Used to initialise the vector. Otherwise for each element for which the default constructor 
				   is called an object is created.*/

  Float_t _minInvCalibrationConstant;
};
}
#endif
