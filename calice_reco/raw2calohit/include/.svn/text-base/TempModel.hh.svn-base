#ifndef TempModel_h
#define TempModel_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <lcio.h>

using namespace lcio;

namespace CALICE {

class TempModel {

public: 
  TempModel() {};
  
  virtual ~TempModel() {};
  
  virtual float getTemp(lcio::LCCollection* col, unsigned moduleID, unsigned CellKey) {
    return 0;
  };
  
  virtual float getTempError(lcio::LCCollection* col, unsigned moduleID, unsigned CellKey) {
    return 0;
  };

};

}

#endif
