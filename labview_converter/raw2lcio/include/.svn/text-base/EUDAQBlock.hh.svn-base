#ifndef EUDAQBlock_h
#define EUDAQBlock_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 9
#define NFLOAT 0
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

class EUDAQBlock ;

/**
 * Class for the SLCIO EUDAQ Data as acquired by the EUDAQ system.
 * @author A. Irles, based on the LabviewBlock2 writen by S. Lu DESY Hamburg
 * @date May 20 2015
 * Created for 2015 testbeams EUDAQ data format.
 */
class EUDAQBlock : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
public: 
  
  EUDAQBlock() {};

  /** Convenient c'tor.
   */
  EUDAQBlock(int CycleNr, int BunchXID, int ChipID, int EvtNr, int Channel, int TDC, int ADC, int HitBit, int GainBit) {

    obj()->setIntVal( 0 , CycleNr  ) ;
    obj()->setIntVal( 1 , BunchXID  ) ;
    obj()->setIntVal( 2 , ChipID  ) ;
    obj()->setIntVal( 3 , EvtNr ) ;  
    obj()->setIntVal( 4 , Channel ) ;  
    obj()->setIntVal( 5 , TDC ) ;  
    obj()->setIntVal( 6 , ADC ) ;  
    obj()->setIntVal( 7 , HitBit ) ;  
    obj()->setIntVal( 8 , GainBit ) ;  
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  EUDAQBlock(LCObject* obj) : LCFixedObject<NINT,NFLOAT,NDOUBLE>(obj) { } 

  /** Important for memory handling*/
  virtual ~EUDAQBlock() { /* no op*/  }
  
  /** get the CycleNr.
   */ 
  int GetCycleNr() const {
    return getIntVal(0);
  }

  
  /** get the BunchXID.
   */ 
  int GetBunchXID() const {
    return getIntVal(1);
  }

  /** get the ChipID.
   */ 
  int GetChipID() const {
    return getIntVal(2);
  }

  
  /** get the EvtNr.
   */ 
  int GetEvtNr() const {
    return getIntVal(3);
  }

  
  /** get the Channel.
   */ 
  int GetChannel() const {
    return getIntVal(4);
  }

  
  /** get the TDC.
   */ 
  int GetTDC() const {
    return getIntVal(5);
  }

  
  /** get the ADC.
   */ 
  int GetADC() const {
    return getIntVal(6);
  }

  
  /** get the HitBit.
   */ 
  int GetHitBit() const {
    return getIntVal(7);
  }

  
  /** get the GainBit.
   */ 
  int GetGainBit() const {
    return getIntVal(8);
  }

  
  /** Convenient print method 
   */
  void print(  std::ostream& os, int ) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"EUDAQBlock" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:CycleNr; i:BunchXID; i:ChipID; i:EvtNr; i:Channel; i:TDC; i:ADC; i:HitBit; i:GainBit" ;
  }

}; // class
}

#endif 
//=============================================================================
