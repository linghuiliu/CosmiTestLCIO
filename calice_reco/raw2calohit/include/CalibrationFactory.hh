#ifndef _CalibrationFactory_hh_
#define _CalibrationFactory_hh_

#include <string>
#include <sstream>
#include <stdexcept>
#include <map>

#include <CalibrationKit.hh>

/** Return a calibration object of the given name.
 * Calibration kits are registered with a certain name. This name which is for example
 * specified by a Marlin parameter can than be used to create the actual calibration 
 * object.
 */
class CalibrationFactory
{
protected:
  ~CalibrationFactory() {
    if (this==__instance) {
      __instance=0;
    }
  };

public:
  /** Get the global instance of the Calibration Object Factory.
   */
  static CalibrationFactory *getInstance() {
    if (!__instance) {
      __instance=new CalibrationFactory;
    }
    return __instance;
  };

  /** Register a Calibration object kit.
   */
  void registerCalibrationKit(const std::string &kit_name, CalibrationKit *a_kit) {
    if (a_kit) {
      _kits.insert(std::make_pair(kit_name,a_kit));
    }
  };

  /** Get an object for the calibration.
   */
  Calibration *createCalibrationObject(const std::string &name, const std::string &module_type_col_name,  const std::string &module_calibration_col_name) 
    throw(std::runtime_error)
  {
    std::map<std::string, CalibrationKit *>::const_iterator kit_iter=_kits.find(name);
    if (kit_iter==_kits.end()) {
      std::stringstream message;
      message << "CalibrationFactory::getCalibrationObject> No calibration object of name" << name << ".";
      throw std::runtime_error(message.str());
    }
    return kit_iter->second->create(module_type_col_name,module_calibration_col_name);
  };

  /** Show all registered kits.
   * For debugging.
   */
  void listKits();

private:
  std::map<std::string, CalibrationKit *> _kits; /**< List of registered Calibration kits.*/
  static CalibrationFactory *__instance;         /**< Pointer to the global CalibrationFactory instance.*/
};
#endif
