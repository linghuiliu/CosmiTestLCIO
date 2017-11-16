#ifndef FeConfigurationBlock_hh
#define FeConfigurationBlock_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"


//Header which allows us to build an md5 checksum
#include "md5.hh"

#include <cassert>

using namespace lcio ;

namespace CALICE {

  /** The indices of all the stored values.*/

  enum EFeConfBlockIntValues {kFeConfBlockIntValFeBoardId, 
			      kIntFeRecLabel,
			      kFeConfBlockIntValCalibStart,
			      kFeConfBlockIntValCalibWidth,
			      kFeConfBlockIntValHoldStart,
			      kFeConfBlockIntValHoldWidth,
			      kFeConfBlockIntValVfeResetStart,
			      kFeConfBlockIntValVfeResetEnd,
			      kFeConfBlockIntValVfeSrinStart,
			      kFeConfBlockIntValVfeSrinEnd,
			      kFeConfBlockIntValVfeMplexClockStart,
			      kFeConfBlockIntValVfeMplexClockMark,
			      kFeConfBlockIntValVfeMplexClockSpace,
			      kFeConfBlockIntValVfeMplexClockPulses,
			      kFeConfBlockIntValAdcStart,
			      kFeConfBlockIntValAdcEnd,
			      kFeConfBlockIntValAdcControl, 
			      kFeConfBlockIntValAdcDelay,
			      kFeConfBlockIntValFrameSyncDelay,
			      kFeConfBlockIntValQdrDataDelay, 
			      kFeConfBlockIntValDacData, 
			      kFeConfBlockIntValFlags, 
			      kFeConfBlockIntValVfeInfo, 
			      kFeConfBlockIntValEmcBot, 
			      kFeConfBlockIntValEmcTop, 
			      kFeConfBlockIntValEmcTopLowGainBit, 
			      kFeConfBlockIntValEmcBotLowGainBit, 
			      kNFeConfBlockIntValues};

/** Stores the configuration data of the front-end of one CERC board.
 * Based on @ref AdcBlock.
 * This is just a copy of the Calice DAQ class CrcFeConfigurationData. Since 
 * I don't have a profound understanding of the meaning of its members the 
 * documentation is mostly missing or at least very poor.
 * 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */
  class FeConfigurationBlock : public LCGenericObjectImplExt {

public: 
    
  //static unsigned int makeBoardID(const short crateID, const short slotID, const short boardComponentNumber, const short boardLabel) 
  //{
  //  return BoardID(crateID, slotID, boardComponentNumber, boardLabel);
  //};

  static unsigned int makeBoardID(const short crateID, const short slotID, const short boardComponentNumber) 
  {
    return BoardID(crateID, slotID, boardComponentNumber);
  };


  /** Convenient c'tor.
   * @param boardID CERC board ID (VME crate channel))
   */
  FeConfigurationBlock(int boardID) {
    obj()->setIntVal( kFeConfBlockIntValFeBoardId , boardID  ) ;
    //Initialize the Flags Field otherwise its content can float around
    obj()->setIntVal( kFeConfBlockIntValFlags , 0  ) ;
    //Initialize Values which are used only for the emc data 
    //to prevent them from floating around in case we fill hcal data
    obj()->setIntVal( kFeConfBlockIntValEmcBot, 0);
    obj()->setIntVal( kFeConfBlockIntValEmcTop, 0);
    obj()->setIntVal( kFeConfBlockIntValEmcBotLowGainBit, 0);
    obj()->setIntVal( kFeConfBlockIntValEmcTopLowGainBit, 0);
  };

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
    FeConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) 
    { 
      assert ( _obj->getNFloat() == 0 );
      assert ( _obj->getNDouble() == 0 );
      assert ( _obj->getNInt() == kNFeConfBlockIntValues );
    } 


  /** Set the crate ID */
  FeConfigurationBlock &  setCrateID(short crateID)
  {
#ifdef VALUE_CHECK
    assert((crateID & 0xffffff00) == 0)
#endif
    obj()->setIntVal(kFeConfBlockIntValFeBoardId,getIntVal(kFeConfBlockIntValFeBoardId) | (crateID << 16));
    return *this;
  };

  /** Get the crate ID */
  short getCrateID() const {
      return (short) ( (getIntVal(kFeConfBlockIntValFeBoardId) >> 16) & 0xff);
  };
  
  /** Set the slot ID */
  FeConfigurationBlock &  setSlotID(short slotID) 
  {
#ifdef VALUE_CHECK
    assert((slotID & 0xffffff00) == 0)
#endif
    obj()->setIntVal(kFeConfBlockIntValFeBoardId,getIntVal(kFeConfBlockIntValFeBoardId)| ((slotID & 0xff) << 8));
    return *this;
  };

  /** Get the slot ID */
  short getSlotID() const {
      return (short) ( (getIntVal(kFeConfBlockIntValFeBoardId) >> 8) & 0xff);
  };
  

  /** Set the Boardcomponentnumber */ 
  FeConfigurationBlock &  setBoardComponentNumber(short componentNumber) 
  {
#ifdef VALUE_CHECK
    assert((componentNumber & 0xffffff00) == 0)
#endif
   obj()->setIntVal(kFeConfBlockIntValFeBoardId,getIntVal(kFeConfBlockIntValFeBoardId)| (componentNumber & 0xff) );
    return *this;
  };

  /** Get the Boardcomponentnumber */
  short getBoardComponentNumber() const {
      return (short) (  (getIntVal(kFeConfBlockIntValFeBoardId) & 0xff) );
  };
  

  /** Set the RecordLabel, i.e. whether this dataset was written to
   * or read from a CRC board */ 
  void setRecordLabel(int label) {obj()->setIntVal(kIntFeRecLabel,label);};


  /** Get the record label */
  short getRecordLabel() const {
   return (short) ( getIntVal(kIntFeRecLabel) & 0xff);
  };


  /** Set the boardID composed by crate, slot and component number */
  FeConfigurationBlock &setBoardID(int boardID) {obj()->setIntVal(kFeConfBlockIntValFeBoardId,boardID); return *this;};

  /** Get the boardID */
  int getBoardID() const {return getIntVal(kFeConfBlockIntValFeBoardId);};

  enum EFlags {kFlagsCalibEnable=1,kFlagsHoldInvert=2};

  /** Set Calibration pulse start value.
   * @param calibStart pulse start in units of 6.25 ns.
   */
  FeConfigurationBlock &  setCalibStart(int calibStart) {obj()->setIntVal(kFeConfBlockIntValCalibStart,calibStart);return *this;};

  /** Get calibration pulse start value.
   * @return  pulse start in units of 6.25 ns.
   */
  int  getCalibStart() {return getIntVal(kFeConfBlockIntValCalibStart);};

  /** Set duration of the calibration pulse.
   * @param calibWidth pulse duration in units of 6.25 ns.
   */
  FeConfigurationBlock &  setCalibWidth(int calibWidth) {obj()->setIntVal(kFeConfBlockIntValCalibWidth,calibWidth);return *this;};

  /** Get duration of the calibration pulse.
   * @return pulse duration in units of 6.25 ns.
   */
  int  getCalibWidth() {return getIntVal(kFeConfBlockIntValCalibWidth);};


  /** Se the calib enable value ???? */
  FeConfigurationBlock &  setCalibEnable(bool calib_enable) {
    if (calib_enable) {
      return setCalibEnable();
    }
    else {
      return clearCalibEnable();
    }
  };

  /** Method called by previously defined method ???? */
  FeConfigurationBlock &  setCalibEnable() {obj()->setIntVal(kFeConfBlockIntValFlags,getIntVal(kFeConfBlockIntValFlags)| kFlagsCalibEnable);return *this;};

  /** clear the calib enable flag */
  FeConfigurationBlock &  clearCalibEnable() {obj()->setIntVal(kFeConfBlockIntValFlags,getIntVal(kFeConfBlockIntValFlags) & (0xffff-kFlagsCalibEnable));return *this;};

  /** Check the calib enable flag */
  bool isCalibEnable() {return getIntVal(kFeConfBlockIntValFlags) & kFlagsCalibEnable;};

  /** Set the start of holding the signal */
  FeConfigurationBlock &  setHoldStart(int holdStart) {obj()->setIntVal(kFeConfBlockIntValHoldStart,holdStart);return *this;};

  /** Get the signel hold start value */
  int  getHoldStart() {return getIntVal(kFeConfBlockIntValHoldStart);};
  FeConfigurationBlock &  setHoldWidth(int holdWidth) {obj()->setIntVal(kFeConfBlockIntValHoldWidth,holdWidth);return *this;};

  /** Set the hold width */
  int  getHoldWidth() {return getIntVal(kFeConfBlockIntValHoldWidth);};

  
  /** Set whether the holded signal is to be inverted */
  FeConfigurationBlock &  setHoldInvert(bool hold_invert) {
    if (hold_invert) {
      return setHoldInvert();
    }
    else {
      return setHoldInvert();
    }
  };

  
  /** Method called by previously defined method ???? */
  FeConfigurationBlock &  setHoldInvert() {obj()->setIntVal(kFeConfBlockIntValFlags,getIntVal(kFeConfBlockIntValFlags)| kFlagsHoldInvert);return *this;};

  /** Clear the hold invert signel */
  FeConfigurationBlock &  clearHoldInvert() {obj()->setIntVal(kFeConfBlockIntValFlags,getIntVal(kFeConfBlockIntValFlags)& (0xffff-kFlagsHoldInvert));return *this;};
  bool isHoldInvert() {return getIntVal(kFeConfBlockIntValFlags) & kFlagsHoldInvert;};

  /** Start of the Vfe Reset */
  FeConfigurationBlock &  setVfeResetStart(int vfeResetStart) {obj()->setIntVal(kFeConfBlockIntValVfeResetStart,vfeResetStart);return *this;};
  int  getVfeResetStart() {return getIntVal(kFeConfBlockIntValVfeResetStart);};

  /** Set the end of the Vfe Reset */
  FeConfigurationBlock &  setVfeResetEnd(int vfeResetEnd) {obj()->setIntVal(kFeConfBlockIntValVfeResetEnd,vfeResetEnd);return *this;};

  /** Get the end of the Vfe Reset */
  int  getVfeResetEnd() {return getIntVal(kFeConfBlockIntValVfeResetEnd);};

  /** I have no idea what the follwing functions really do 
    * as long as the data are correct the experts can work 
    * with them */  
  FeConfigurationBlock &  setVfeSrinStart(int vfeSrinStart) {obj()->setIntVal(kFeConfBlockIntValVfeSrinStart,vfeSrinStart);return *this;};
  int  getVfeSrinStart() {return getIntVal(kFeConfBlockIntValVfeSrinStart);};

  FeConfigurationBlock &  setVfeSrinEnd(int vfeSrinEnd) {obj()->setIntVal(kFeConfBlockIntValVfeSrinEnd,vfeSrinEnd);return *this;};
  int  getVfeSrinEnd() {return getIntVal(kFeConfBlockIntValVfeSrinEnd);};

  FeConfigurationBlock &  setVfeMplexClockStart(int vfeMplexClockStart) {obj()->setIntVal(kFeConfBlockIntValVfeMplexClockStart,vfeMplexClockStart);return *this;};
  int  getVfeMplexClockStart() {return getIntVal(kFeConfBlockIntValVfeMplexClockStart);};

  FeConfigurationBlock &  setVfeMplexClockMark(int vfeMplexClockMark) {obj()->setIntVal(kFeConfBlockIntValVfeMplexClockMark,vfeMplexClockMark);return *this;};
  int  getVfeMplexClockMark() {return getIntVal(kFeConfBlockIntValVfeMplexClockMark);};

  FeConfigurationBlock &  setVfeMplexClockSpace(int vfeMplexClockSpace) {obj()->setIntVal(kFeConfBlockIntValVfeMplexClockSpace,vfeMplexClockSpace);return *this;};
  int  getVfeMplexClockSpace() {return getIntVal(kFeConfBlockIntValVfeMplexClockSpace);};

  FeConfigurationBlock &  setVfeMplexClockPulses(int vfeMplexClockPulses) {obj()->setIntVal(kFeConfBlockIntValVfeMplexClockPulses,vfeMplexClockPulses);return *this;};
  int  getVfeMplexClockPulses() {return getIntVal(kFeConfBlockIntValVfeMplexClockPulses);};

  /** Start of the ADC Reading */
  FeConfigurationBlock &  setAdcStart(int adcStart) {obj()->setIntVal(kFeConfBlockIntValAdcStart,adcStart);return *this;};
  int  getAdcStart() {return getIntVal(kFeConfBlockIntValAdcStart);};

  /** End of the ADC Reading, kind of gate width */
  FeConfigurationBlock &  setAdcEnd(int adcEnd) {obj()->setIntVal(kFeConfBlockIntValAdcEnd,adcEnd);return *this;};
  int  getAdcEnd() {return getIntVal(kFeConfBlockIntValAdcEnd);};


  /** To whom it may concern ;-) */
  FeConfigurationBlock &  setAdcControlBits(int adcControl) {obj()->setIntVal(kFeConfBlockIntValAdcControl,adcControl);return *this;};
  int  getAdcControlBits() {return getIntVal(kFeConfBlockIntValAdcControl);};

  /** Delay of the ADC readout */
  FeConfigurationBlock &  setAdcDelay(int adcDelay) {obj()->setIntVal(kFeConfBlockIntValAdcDelay,adcDelay);return *this;};
  int  getAdcDelay() {return getIntVal(kFeConfBlockIntValAdcDelay);};

  /** Dac Data setting */
  FeConfigurationBlock &  setDacData(int top_value, int bottom_value) 
  {

    unsigned int comb= ((((unsigned int) top_value) & 0xffff) << 16) 
                      | (((unsigned int) bottom_value) & 0xffff); 
    obj()->setIntVal(kFeConfBlockIntValDacData,(int) comb);
    return *this;
  };

  /** Daq data on top side of the ADC sample*/
  int getDacDataTop() const
  {
    return (int) (((unsigned int) getIntVal(kFeConfBlockIntValDacData)) >> 16);
  };

  /** Daq data onbottom side of the ADC sample*/
  int getDacDataBottom() const
  {
    return (int) (((unsigned int) getIntVal(kFeConfBlockIntValDacData)) & 0xffff);
  };


  /**Again not many ideas what this means */
  FeConfigurationBlock &  setFrameSyncDelay(int frameSyncDelay) {obj()->setIntVal(kFeConfBlockIntValFrameSyncDelay,frameSyncDelay);return *this;};

  int  getFrameSyncDelay() {return getIntVal(kFeConfBlockIntValFrameSyncDelay);};

  FeConfigurationBlock &  setQdrDataDelay(int qdrDataDelay) {obj()->setIntVal(kFeConfBlockIntValQdrDataDelay,qdrDataDelay);return *this;};

  int  getQdrDataDelay() {return getIntVal(kFeConfBlockIntValQdrDataDelay);};
  

  /** Set Vfe Info for Boards not identified as belonging to the SiW Ecal (only the word itself for the
     time being*/
  FeConfigurationBlock &  setVfeInfo(int vfeInfo) {obj()->setIntVal(kFeConfBlockIntValVfeInfo,vfeInfo);return *this;};

  /** Set Vfe Info for the Emc Information */
  FeConfigurationBlock &  setEmcVfeInfo(int vfeInfo, int iBot, int
  iTop, int iBotLowGainBit, int iTopLowGainBit) {
     obj()->setIntVal(kFeConfBlockIntValVfeInfo,vfeInfo);
     //In principle this is information which does not change very often 
     //But if we're store the user interface is immune against a sudden change
       obj()->setIntVal(kFeConfBlockIntValEmcBot,iBot);
       obj()->setIntVal(kFeConfBlockIntValEmcTop,iTop);
       obj()->setIntVal(kFeConfBlockIntValEmcBotLowGainBit,iBotLowGainBit);
       obj()->setIntVal(kFeConfBlockIntValEmcTopLowGainBit,iTopLowGainBit);
   return *this;};

  /** Get complete the Vfe Info */
  int  getVfeInfo() {return getIntVal(kFeConfBlockIntValVfeInfo);};

  /** Get some detailed emc (!!) vfe info (basically taken from Pauls online software */

  int emcVfeBottomEnable (int chip) {
#ifdef VALUE_CHECK
    assert (chip < 6);
#endif
    return  static_cast<bool> ( ( (getVfeInfo() & 0xfff) >> (chip+6) ) & 0x1);       

  }


  int emcVfeTopEnable (int chip) {
#ifdef VALUE_CHECK
    assert (chip < 6);
#endif
    return static_cast<bool>( ( (getVfeInfo() & 0xfff) >> chip ) & 0x1);       
  }

  bool emcVfeLowGainBottom () {
    return static_cast<bool>( ( (getVfeInfo() & 0xffff) >> getIntVal(kFeConfBlockIntValEmcBotLowGainBit) ) & 0x1);       
  }

  bool emcVfeLowGainTop () {
    return   static_cast<bool>( ( (getVfeInfo() & 0xffff) >> getIntVal(kFeConfBlockIntValEmcTopLowGainBit) ) & 0x1);       
  }


  /** Convenient print method 
   */
  void print(  std::ostream& os, int ) {
    os << "Not implemented (I am sorry)." << std::endl;
  };
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"FeConfigurationBlock" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:BoardID,i:RecordLabel,i:CalibStart,i:CalibWidth,i:CalibEnable,i:HoldStart,i:HoldWidth,i:HoldInvert,i:VfeResetStart,i:VfeResetEnd,i:VfeSrinStart,i:VfeSrinEnd,i:VfeMplexClockStart,i:VfeMplexClockMark,i:VfeMplexClockSpace,i:VfeMplexClockPulses,i:AdcStart,i:setAdcEnd,i:AdcControlBits,i:setAdcDelay,x:DacData,i:FrameSyncDelay,i:QdrDataDelay" ;
  }

}; // class

}
#endif 
