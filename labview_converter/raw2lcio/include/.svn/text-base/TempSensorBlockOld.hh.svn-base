#ifndef TempSensorBlockOld_h
#define TempSensorBlockOld_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <string>

#define TSONINT 0
#define TSONFLOAT 9
#define TSONDOUBLE 0

using namespace lcio ;

namespace CALICE {
    
  class TempSensorBlockOld ;
    
  /**
   * Class for the Labview Data as acquired by the AHCAL Labview.
   * The class reflects that the data are received in the Labview
   * @author S. Lu DESY Hamburg
   * @date Dec 17 2012
   */
  class TempSensorBlockOld : public LCFixedObject<TSONINT,TSONFLOAT,TSONDOUBLE> {
        
  public:
        
    TempSensorBlockOld() {};
        
    /** Convenient c'tor.
     */
    TempSensorBlockOld(float layer, float T1, float T2, float T3, float T4, float T5, float T6, float TDIF, float TPWR) {
            
      obj()->setFloatVal(0, layer);
      obj()->setFloatVal(1, T1);
      obj()->setFloatVal(2, T2);
      obj()->setFloatVal(3, T3);
      obj()->setFloatVal(4, T4);
      obj()->setFloatVal(5, T5);
      obj()->setFloatVal(6, T6);
      obj()->setFloatVal(7, TDIF);
      obj()->setFloatVal(8, TPWR);
    }
        
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    TempSensorBlockOld(LCObject* obj) : LCFixedObject<TSONINT,TSONFLOAT,TSONDOUBLE>(obj) { }
        
    /** Important for memory handling*/
    virtual ~TempSensorBlockOld() { /* no op*/  }
        
    float GetLayer() const {
      return getFloatVal(0);
    }

    float GetT1() const {
      return getFloatVal(1);
    }
        
    float GetT2() const {
      return getFloatVal(2);
    }
        
    float GetT3() const {
      return getFloatVal(3);
    }
        
    float GetT4() const {
      return getFloatVal(4);
    }
        
    float GetT5() const {
      return getFloatVal(5);
    }
        
    float GetT6() const {
      return getFloatVal(6);
    }
        
    float GetTDIF() const {
      return getFloatVal(7);
    }
        
    float GetTPWR() const {
      return getFloatVal(8);
    }
        
    void print(  std::ostream& os, float ) ;
        
    // -------- need to implement abstract methods from LCGenericObject
        
    /** Return the type of the class
     */
    const std::string getTypeName() const {
      return"TempSensorBlockOld" ;
    }
        
    /** Return a brief description of the data members
     */
    const std::string getDataDescription() const {
      return "f:layer; f:T1; f:T2; f:T3; f:T4; f:T5; f:T6; f:TDIF; f:TPWR;" ;
    }
        
  };
    
}

#endif
//=============================================================================
