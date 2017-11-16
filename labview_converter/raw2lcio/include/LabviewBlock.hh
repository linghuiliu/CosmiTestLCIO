#ifndef LabviewBlock_h
#define LabviewBlock_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 12
#define NFLOAT 0
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

class LabviewBlock ;

/**
 * Class for the Labview Data as acquired by the AHCAL Labview.
 * The class reflects that the data are received in the Labview
 * @author S. Lu DESY Hamburg
 * @date Aug 16 2012
 */
class LabviewBlock : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
public: 
  
  LabviewBlock() {};

  /** Convenient c'tor.
   */
  LabviewBlock(int BunchXID, int CycleNr, int ChipID, int ASICNr, int EvtNr, int Channel, int TDC, int ADC, int XPos, int YPos, int HitBit, int GainBit) {

    obj()->setIntVal( 0 , BunchXID  ) ;
    obj()->setIntVal( 1 , CycleNr  ) ;
    obj()->setIntVal( 2 , ChipID  ) ;
    obj()->setIntVal( 3 , ASICNr ) ;  
    obj()->setIntVal( 4 , EvtNr ) ;  
    obj()->setIntVal( 5 , Channel ) ;  
    obj()->setIntVal( 6 , TDC ) ;  
    obj()->setIntVal( 7 , ADC ) ;  
    obj()->setIntVal( 8 , XPos ) ;  
    obj()->setIntVal( 9 , YPos ) ;  
    obj()->setIntVal( 10 , HitBit ) ;  
    obj()->setIntVal( 11 , GainBit ) ;  
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  LabviewBlock(LCObject* obj) : LCFixedObject<NINT,NFLOAT,NDOUBLE>(obj) { } 

  /** Important for memory handling*/
  virtual ~LabviewBlock() { /* no op*/  }
  
  /** get the BunchXID.
   */ 
  int GetBunchXID() const {
    return getIntVal(0);
  }

  /** get the CycleNr.
   */ 
  int GetCycleNr() const {
    return getIntVal(1);
  }

  
  /** get the ChipID.
   */ 
  int GetChipID() const {
    return getIntVal(2);
  }

  
  /** get the ASICNr.
   */ 
  int GetASICNr() const {
    return getIntVal(3);
  }

  
  /** get the EvtNr.
   */ 
  int GetEvtNr() const {
    return getIntVal(4);
  }

  
  /** get the Channel.
   */ 
  int GetChannel() const {
    return getIntVal(5);
  }

  
  /** get the TDC.
   */ 
  int GetTDC() const {
    return getIntVal(6);
  }

  
  /** get the ADC.
   */ 
  int GetADC() const {
    return getIntVal(7);
  }

  
  /** get the XPos.
   */ 
  int GetXPos() const {
    return getIntVal(8);
  }

  
  /** get the YPos.
   */ 
  int GetYPos() const {
    return getIntVal(9);
  }

  
  /** get the HitBit.
   */ 
  int GetHitBit() const {
    return getIntVal(10);
  }

  
  /** get the GainBit.
   */ 
  int GetGainBit() const {
    return getIntVal(11);
  }

  
  /** Convenient print method 
   */
  void print(  std::ostream& os, int ) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"LabviewBlock" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:BunchXID; i:CycleNr; i:ChipID; i:ASICNr; i:EvtNr; i:Channel; i:TDC; i:ADC; i:XPos; i:YPos; i:HitBit; i:GainBit" ;
  }

}; // class
}

#endif 
//=============================================================================
