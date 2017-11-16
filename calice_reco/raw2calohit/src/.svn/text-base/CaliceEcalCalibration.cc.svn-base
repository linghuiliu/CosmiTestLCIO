#include <CaliceEcalCalibration.hh>
#include <ModuleDescription.hh>
#include <sstream>
#include <iostream>

#include "CalibrationKit.hh"
#include "CalibrationFactory.hh"
#include <marlin/ConditionsProcessor.h>
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif

namespace CALICE {

  // ------------------------------------------------------------
  // Calibration kit for the Calice ECAL calibration module
  // ------------------------------------------------------
  // Definition and instantiation

  /** Create an Calice ECAL calibration object.
   * @sa CaliceEcalCalibration.
   */
  class CaliceEcalCalibrationKit : public CalibrationKit
  {
  protected:
    CaliceEcalCalibrationKit() {
      CalibrationFactory *instance=CalibrationFactory::getInstance();
      instance->registerCalibrationKit("CaliceEcalCalibration",this);
    };

    static CaliceEcalCalibrationKit __instance;
  public:
    Calibration *create(const std::string &module_type_col_name,  const std::string &module_calibration_col_name) const {
      std::cout << "CalibrationKit::create> Create CaliceEcalCalibration." << std::endl;
      return new CaliceEcalCalibration(module_type_col_name, module_calibration_col_name);
    };
  };
  
  CaliceEcalCalibrationKit CaliceEcalCalibrationKit::__instance;

  // Calibration kit for the Calice ECAL calibration module
  //____________________________________________________________


  //------------------------------------------------------------
  // The Calice ECAL Calibration object

  // dummy object used to initialise arrays.
  EcalModuleCalibration CaliceEcalCalibration::_empty("",0,0);

  CaliceEcalCalibration::CaliceEcalCalibration(const std::string &module_type_col_name, const std::string &calibration_constants_col_name) 
    throw(std::runtime_error)
    : _moduleTypeChange(this, &CaliceEcalCalibration::moduleTypeChanged),
      _calibrationConstantChange(this, &CaliceEcalCalibration::calibrationConstantChanged)
  {
    if (module_type_col_name.empty()) {
      throw std::runtime_error("CaliceEcalCalibration::ctor> The name for the collection of module types (i.e. module descriptions) must not be empty.");
    }
    if (calibration_constants_col_name.empty()) {
      throw std::runtime_error("CaliceEcalCalibration::ctor> The name for the calibration constant collection must not be empty.");
    }

    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleTypeChange ,module_type_col_name)) {
      std::stringstream message;
      message << "CaliceEcalCalibration::CaliceEcalCalibration> no conditions data handler for collection " << module_type_col_name << ".";
      throw std::runtime_error(message.str());
    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_calibrationConstantChange , calibration_constants_col_name)) {
      std::stringstream message;
      message << "CaliceEcalCalibration::CaliceEcalCalibration> no conditions data handler for collection " << calibration_constants_col_name << ".";
      throw std::runtime_error(message.str());
    }
  }


  void CaliceEcalCalibration::moduleTypeChanged(lcio::LCCollection* col) {

    _calibrationConstantsPerType.clear();

    std::pair<ModuleTypeCalibrationConstantList_t *,std::string> dummy_element
      =make_pair((ModuleTypeCalibrationConstantList_t *) 0,std::string(""));

    for (UInt_t type_i=0; type_i< static_cast<UInt_t>(col->getNumberOfElements()); type_i++) {
      ModuleDescription a_module(col->getElementAt(type_i));
      if (a_module.getModuleType()>_calibrationConstantsPerType.size()) {
	_calibrationConstantsPerType.resize(a_module.getModuleType()+1,dummy_element );
      }
      if (a_module.getModuleType()==_calibrationConstantsPerType.size()) {
	_calibrationConstantsPerType.push_back(make_pair(dummy_element.first,a_module.getModuleTypeName()));
      }
      else {
	_calibrationConstantsPerType[a_module.getModuleType()]=make_pair(dummy_element.first,a_module.getModuleTypeName());
      }
    }

    if (!_calibrationConstants.empty()) {
      // now correctly set the pointers to the arrays with the calibration constants
      for(CalibrationConstantPerTypeList_t::iterator per_type_iter=_calibrationConstantsPerType.begin();
	  per_type_iter!=_calibrationConstantsPerType.end();
	  per_type_iter++) {
	
	// search module type 
	std::map<std::string, ModuleTypeCalibrationConstantList_t >::const_iterator type_iter=_calibrationConstants.find(per_type_iter->second);
	
	// Module types may be defined which are actually not used.
	// Thus, it is a valid case that no calibration constants are assigned to a certain module type
	if (type_iter!=_calibrationConstants.end()) {
	  //	  std::stringstream message;
	  //	  message << "CaliceEcalCalibration::calibrationConstantChanged> Do not find calibration constants for type " << per_type_iter->second 
	  //		  << ".";
	  //	  throw std::runtime_error(message.str());
	  per_type_iter->first=&(type_iter->second);
	}
      }
    }
  }

  void CaliceEcalCalibration::calibrationConstantChanged(lcio::LCCollection* col) 
  {
    _calibrationConstants.clear();
    
    // first put the calibration constants of all modules in arrays for each module type 
    // the module types are "C", "G", "D"
    // seperately and using the module type ID as an array index.
    Float_t maxCalibrationConstant=-FLT_MAX;
    for (UInt_t type_i=0; type_i< static_cast<UInt_t>(col->getNumberOfElements()); type_i++) {
      EcalModuleCalibration a_module(col->getElementAt(type_i));

      // Issue a warning if the serial number is large.
      // The serial number is used as an array index (for simplicity).
      // This an array from the zero to the largest serial number is
      // generated. This will lead to very large arrays if the serial
      // numbers are not small (1-~50) as expected.
      if (a_module.getModuleID()>10*static_cast<UInt_t>(col->getNumberOfElements())) {
	std::cerr << "CaliceEcalCalibration::moduleTypeChanged> Very large serial number. This may blow up the memory consumption since the serial number is used as an array index.";
      }

      for (UInt_t cell_i=0; cell_i<a_module.getNCells(); cell_i++) {
	Float_t calib_const=a_module.getCalibrationConstant(cell_i);
	if (calib_const>maxCalibrationConstant) {
	  maxCalibrationConstant=calib_const;
	}
      }
      
      // the method is slow so we cache the result.
      std::string module_type_name=a_module.getModuleTypeName();
      
      // search module type 
      std::map<std::string, ModuleTypeCalibrationConstantList_t >::iterator type_iter=_calibrationConstants.find(module_type_name);

      if (type_iter==_calibrationConstants.end()) {
	std::pair< std::map<std::string, ModuleTypeCalibrationConstantList_t>::iterator , Bool_t > result;
	result=_calibrationConstants.insert(make_pair(module_type_name,ModuleTypeCalibrationConstantList_t()));

	// I belief that the following check is not necessary
	if (!result.second) throw std::runtime_error("CaliceEcalCalibration::calibrationConstantChanged> Failed to add Calibration constants to list.");
	type_iter=result.first;
      }

      if (a_module.getModuleID()>type_iter->second.size()) {
	type_iter->second.resize(a_module.getModuleID(),EcalModuleCalibration(_empty.obj()));
      }
      if (a_module.getModuleID()==type_iter->second.size()) {
	type_iter->second.push_back(a_module);
      }
      else {
	type_iter->second[a_module.getModuleID()]=a_module;
      }
    }

    if (maxCalibrationConstant<=0) {
      throw std::runtime_error("CaliceEcalCalibration::calibrationConstantChanged> The maximum calibration constant is not a positive value.");
    }
    _minInvCalibrationConstant=1/maxCalibrationConstant;

    // now rebuild the array which contains for each module type a pointer to the array with the calibration constants for 
    // each the modules identified by their module ID. 
    for(CalibrationConstantPerTypeList_t::iterator per_type_iter=_calibrationConstantsPerType.begin();
	per_type_iter!=_calibrationConstantsPerType.end();
	per_type_iter++) {

      // search module type 
      std::map<std::string, ModuleTypeCalibrationConstantList_t >::const_iterator type_iter=_calibrationConstants.find(per_type_iter->second);
//       if (type_iter==_calibrationConstants.end()) {
// 	std::stringstream message;
// 	message << "CaliceEcalCalibration::calibrationConstantChanged> Do not find calibration constants for type " << per_type_iter->second 
// 		<< ".";
// 	throw std::runtime_error(message.str());
//       }
      per_type_iter->first=&(type_iter->second);
    }


  }

  // The Calice ECAL calibration object
  //____________________________________________________________
}
