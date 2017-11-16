#ifndef HBUTemperatureBlock_h
#define HBUTemperatureBlock_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 0
#define NFLOAT 6
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

class HBUTemperatureBlock ;

/**
 * Class for the Labview Data as acquired by the AHCAL Labview.
 * The class reflects that the data are received in the Labview
 * @author S. Lu DESY Hamburg
 * @date Oct 02 2012
 */
class HBUTemperatureBlock : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
public: 
  
  HBUTemperatureBlock() {};

  /** Convenient c'tor.
   */
  HBUTemperatureBlock(float temp1,float temp2,float temp3,float temp4,float temp5,float temp6 ) {

    obj()->setFloatVal( 0 , temp1  ) ;
    obj()->setFloatVal( 1 , temp2  ) ;
    obj()->setFloatVal( 2 , temp3  ) ;
    obj()->setFloatVal( 3 , temp4  ) ;
    obj()->setFloatVal( 4 , temp5  ) ;
    obj()->setFloatVal( 5 , temp6  ) ;
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  HBUTemperatureBlock(LCObject* obj) : LCFixedObject<NINT,NFLOAT,NDOUBLE>(obj) { } 

  /** Important for memory handling*/
  virtual ~HBUTemperatureBlock() { /* no op*/  }
  
  /** get the iTh temperature value.
   */ 
  int GetTemperature(int i) const {
    return getIntVal(i);
  }

  
  /** Convenient print method 
   */
  void print(  std::ostream& os, int ) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"HBUTemperatureBlock" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "f:tempSensor1; f:tempSensor2; f:tempSensor3; f:tempSensor4; f:tempSensor5; f:tempSensor6" ;
  }

}; // class
}

#endif 
//=============================================================================
