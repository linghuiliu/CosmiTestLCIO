#ifndef TilesItepSaturation_h
#define TilesItepSaturation_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NTISINT 1
#define NTISFLO 40
#define NTISDBL 0


using namespace lcio ;


namespace CALICE {
class TilesItepSaturation ;

/** Parameters of the sipm delivered by ITEP
  */
class TilesItepSaturation : public LCFixedObject<NTISINT,NTISFLO,NTISDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  TilesItepSaturation(int SIPM, float _SipmPixelSat[20], float _PmtMipSat[20]) {

    obj()->setIntVal(0, SIPM);
    for(int i=0;i<20;i++){      
      obj()->setFloatVal(i, _SipmPixelSat[i]);
      obj()->setFloatVal(i+20, _PmtMipSat[i]);
    }
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  TilesItepSaturation(LCObject* obj) : LCFixedObject<NTISINT,NTISFLO,NTISDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~TilesItepSaturation	() { /* no op*/  }

  /** the class interface:    */
  int getSIPMID()   { return obj()->getIntVal(0) ;  } 

  float getSipmPixelSat(unsigned point) {
    return obj()->getFloatVal(point);
  }
  float getPmtMipSat(unsigned point) {
    return obj()->getFloatVal(20+point);
  }

  /* ITEP FILES
==>  pixs.dat - number of pixels for saturation curve measurements
==>  pmts.dat - amount of light for corresponding pixs.dat (mip)
  */

  /** convenient print function */ 
  void print(std::ostream& os) ;
  
  // -------- need to implement abstract methods from LCGenericObject
  const std::string getTypeName() const { 
    return "SiPM_MappingHcal";
  } 
  
  const std::string getDataDescription() const {
    return "i:SiPM,f:SipmPixelSat[20],f:PmtMipSat[20]"; 
  }

}; // class

}
#endif 
