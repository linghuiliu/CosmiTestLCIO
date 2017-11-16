#include "CalibrationFactory.hh"
#include <iostream>

CalibrationFactory *CalibrationFactory::__instance=0;

void CalibrationFactory::listKits()
{
  std::cout << " --- Calibration Kits: "  << std::endl;
  for (std::map<std::string, CalibrationKit *>::const_iterator  kit_iter=_kits.begin();
       kit_iter!=_kits.end();
       kit_iter++) {
    std::cout << "\t- " << kit_iter->first << std::endl;
  }
  std::cout << std::endl;
}
