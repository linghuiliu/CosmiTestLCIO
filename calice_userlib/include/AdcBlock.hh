#ifndef AdcBlock_h
#define AdcBlock_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 11
#define NFLOAT 0
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

class AdcBlock ;

  typedef int AdcBlockArray[6];

/**
 * Class for the ADC Data as acquired by the CALICE DAQ.
 * The class reflects that the data are received in chunks
 * of 12 ADC's within a series of 18 transmissions (multiplexing).

 * ADC data are stored together with the indices of the board by which
 * they were received, the FE (0-8) and their position within the multiplexed
 * series (0-17). The class 
 * provides interface functions to get the adc value within the
 * adc block as well as an associated electronic channel id:
 * ElecChannelID = CrateID , SlotID, FE,   Multipl., ADC_i: 
 *            Bit: 31-24   , 23-16, 15-12, 11-4    , 3-0
 * @author R. Pöschl DESY
 * @date Mar 5 2005
 */
class AdcBlock : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
public: 
  
  AdcBlock() {};

  /** Convenient c'tor.
   */
  AdcBlock(int boardID, int label, int Fe, int Imul, const AdcBlockArray &adcBlock, int status) {

    obj()->setIntVal( 0 , boardID  ) ;
    obj()->setIntVal( 1 , label  ) ;
    obj()->setIntVal( 2 , Fe  ) ;
    obj()->setIntVal( 3 , Imul ) ;  
    for (int iadc = 0; iadc < 6; iadc++) {
      obj()->setIntVal(4+iadc, adcBlock[iadc]);
    }
    obj()->setIntVal( 10 ,  status ) ;  
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  AdcBlock(LCObject* obj) : LCFixedObject<NINT,NFLOAT,NDOUBLE>(obj) { } 

  /** Important for memory handling*/
  virtual ~AdcBlock() { /* no op*/  }
  
  /** get the board id.
   */ 
  unsigned int getBoardID() const {
    return getIntVal(0);
  }

  /** the class interface:
   */
  /**get electronic channel id assigned to the adc
   */
  unsigned getElecChannel(int) const;

  /**and the actual ADC calue
   */
  short getAdcVal(int i) const {
    //    int mod=i%2;
    int ipos = 4 + i/2;
  
    if ((i & 1)==0) return (getIntVal(ipos) >> 16) & 0xffff;
    else        return (getIntVal(ipos) >> 0) & 0xffff;
  };


  /**Information on the FE chip the data was received
   */
  short getBoardFrontEnd() const {
    return (short) ( (getIntVal(2) >> 0) & 0xff);
  };

  /**and the position within the series of transmissions
   */
  short getMultiplexPosition() const {
    return (short) ( (getIntVal(3) >> 0) & 0xff);
  }

  /**Some general information on the board on which the data was
   *received
   *on the Crate Id
  */
  short getCrateID() const {
    return (short) ( (getIntVal(0) >> 16) & 0xff);
  };

  /**on the Slot Id
   */
  short getSlotID() const {
    return (short) ( (getIntVal(0) >> 8) & 0xff);
  };

  /**on the BoardComponent Number
   */
  short getBoardComponentNumber() const {
    return (short) ( (getIntVal(0) >> 0) & 0xff);
  };

  /**on the Board Label
   */
  short getRecordLabel() const {
    return (short) ( (getIntVal(1) >> 0) & 0xff);
  };

  /**Transmission of data from piggy back to FE
   */
  short getTransmissionStatus() const { return getIntVal( 10 );}
  
  /** Convenient print method 
   */
  void print(  std::ostream& os, int ) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"AdcBlock" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:CrateID/SlotID,i:RecordLabel,i:Fe:,i:Imul,i[6]:AdcVals,i:TransissionStatus" ;
  }

}; // class
}

#endif 
//=============================================================================
