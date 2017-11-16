#ifndef AhcVfeConfigurationBlock_hh
#define AhcVfeConfigurationBlock_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
//#ifdef VALUE_CHECK
#include <cassert>
//#endif

/*Fixme the following information might have to go to to the database*/
/*Number of Shift registers in Fine Hcal Module*/
#define NAHCSHREGDATA 12
/*Number of Shift registers in Coarse Hcal Module*/
#define NAHCSHREGCOARSE 8
/*Existing Shift Registers in Coarse Hcal Modules */
#define SHREGEXCOARSELO 3
#define SHREGEXCOARSEHI 8

using namespace lcio ;

namespace CALICE {

/** The indices of the the stored values.
 */
enum AhcVfeIntValues {kIntValVfeBoardId, kIntAhcVfeRecLabel, kIntVerificationData, kIntShiftRegisterDataStart};

/** 
 * Interface Class to access the AhcVfeConfigurationData 
 * @author Roman Pöschl DESY
 * @date Nov  2005
 */
class AhcVfeConfigurationBlock : public LCGenericObjectImplExt 
{
  public: 

  static unsigned int makeBoardID(const short crateID, const short slotID, const short boardComponentNumber) 
  {
    return BoardID(crateID, slotID, boardComponentNumber);
  }


  /** Convenient c'tor.
   * @param boardID CERC board ID (VME crate channel))
   */
  AhcVfeConfigurationBlock(int boardID) 
  {
    obj()->setIntVal( kIntValVfeBoardId , boardID  ) ;
  }


  /*Set Paramters of the board by which the data were taken */

  /**Set the crateID */
   AhcVfeConfigurationBlock &  setCrateID(short crateID)
  {
#ifdef VALUE_CHECK
    assert((crateID & 0xffffff00) == 0);
#endif
    obj()->setIntVal(kIntValVfeBoardId,getIntVal(kIntValVfeBoardId) | (crateID << 16));
    return *this;
  }
  
  /**Get the crate ID*/
  short getCrateID() const 
  {
    return (short) ( (getIntVal(kIntValVfeBoardId) >> 16) & 0xff);
  };


  /**Set the Slot ID*/
  AhcVfeConfigurationBlock &  setSlotID(short slotID) 
  {
#ifdef VALUE_CHECK
    assert((slotID & 0xffffff00) == 0);
#endif
    obj()->setIntVal(kIntValVfeBoardId,getIntVal(kIntValVfeBoardId)| ((slotID & 0xff) << 8));
    return *this;
  }
  
  /**Get the Slot ID*/
  short getSlotID() const 
  {
    return (short) ( (getIntVal(kIntValVfeBoardId) >> 8) & 0xff);
  };
  

  /** Set the BoardComponentNumber */
  AhcVfeConfigurationBlock &  setBoardComponentNumber(short componentNumber) 
  {
#ifdef VALUE_CHECK
    assert((componentNumber & 0xffffff00) == 0);
#endif
    obj()->setIntVal(kIntValVfeBoardId,getIntVal(kIntValVfeBoardId)| (componentNumber & 0xff));
    return *this;
  }

  /**Get the BoardComponentNumber */
  short getBoardComponentNumber() const 
  {
    return (short) ( getIntVal(kIntValVfeBoardId) & 0xff);
  }


  /** Set the Record Label */
  void setRecordLabel(int label) {obj()->setIntVal(kIntAhcVfeRecLabel,label);};

  /** Get the Record Label */
  short getRecordLabel() const 
  {
    return (short) ( getIntVal(kIntAhcVfeRecLabel) & 0xff);
  }
  

  /**Set the board ID */
  AhcVfeConfigurationBlock & setBoardID(int boardID) 
  {
    obj()->setIntVal(kIntValVfeBoardId, boardID); 
    return *this;
  }

  int getBoardID() const {return getIntVal(kIntValVfeBoardId);};

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
    AhcVfeConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { 
      assert ( _obj->getNFloat() == 0 );
      assert ( _obj->getNDouble() == 0 );
      assert ( _obj->getNInt() <= kIntShiftRegisterDataStart+NAHCSHREGDATA );
    } 
  

  /** Set Vfe Verification Data */
  AhcVfeConfigurationBlock & setVerificationData(int verifData) 
  {
    obj()->setIntVal(kIntVerificationData,verifData);
    return *this;
  }  

  /** Get Vfe Verification Data */
  unsigned int getVerificationData() { return (unsigned int) obj()->getIntVal(kIntVerificationData);}  


  /** Set the Shiftregister data */
  AhcVfeConfigurationBlock & setShiftRegisterData(int ipos, int ival) {
    if(ipos < NAHCSHREGDATA) {
      obj()->setIntVal(ipos+kIntShiftRegisterDataStart, ival);
    } else {
      std::cerr << "Warning Requested Shift Register ID exceeds " << NAHCSHREGDATA << std::endl;
      std::cerr << "Will not fill this value !!!!" << std::endl;
    }
    return *this;

  }  

  bool isCoarseLayer() 
  {
    if(obj()->getNInt() - kIntShiftRegisterDataStart == NAHCSHREGDATA ) return false;
    if(obj()->getNInt() - kIntShiftRegisterDataStart == NAHCSHREGCOARSE) return true;
  }

  /** Get the Shiftregister data */
  int getShiftRegisterData(int ipos) 
  {
    if (ipos < NAHCSHREGDATA) {
      /*Check for fine layer*/
      if(obj()->getNInt() - kIntShiftRegisterDataStart == NAHCSHREGDATA ) 
	return obj()->getIntVal(ipos+kIntShiftRegisterDataStart);
      
      if(obj()->getNInt() - kIntShiftRegisterDataStart ==  NAHCSHREGCOARSE) 
	{
	  /*Check whether this register is defined in the coarse layer*/
	  if(ipos <= SHREGEXCOARSELO  || ipos >= SHREGEXCOARSEHI) 
	    {
	      if(ipos >= SHREGEXCOARSEHI) ipos = ipos-1-SHREGEXCOARSELO; 
	      return obj()->getIntVal(ipos+kIntShiftRegisterDataStart);
	    } 
	  else 
	    {
	      std::cout << "The register " << ipos << " does not exist for this board !!! " << std::endl; 
	      std::cout << "Will return 0" << std::endl; 
	      return 0;
	    }
	} 
      else 
	{
	  std::cerr << "Warning Requested Shift Register ID exceeds " << NAHCSHREGDATA << std::endl;
	  std::cerr << "Will return 0" << std::endl;
	  return 0;
	}
      
    }  
    return 0;
  }
  
  /** Convenient print method 
   */
  void print(  std::ostream& os) {
    os << BoardID(getBoardID()) << std::endl;
    os << "Verification Data: " << getVerificationData() << std::endl;
    os << "Number of Ints " << getNInt() << std::endl;;
    os << "Coarse Layer " << isCoarseLayer() << std::endl;;

    for(int i = 0; i < NAHCSHREGDATA; i++) 
      { 
	if(i <= SHREGEXCOARSELO) os << "HAB(" << i << "): " <<  getShiftRegisterData(i) << std::endl;
	if(i > SHREGEXCOARSELO && i < SHREGEXCOARSEHI && !(isCoarseLayer())) 
	  os << "HAB(" << i << "): " <<  getShiftRegisterData(i) << std::endl;
	if(i >= SHREGEXCOARSEHI) os << "HAB(" << i << "): " <<  getShiftRegisterData(i) << std::endl;
      }
  }
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"AhcVfeConfigurationBlock" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:boardID, i:VerificationData, i[0-11]:ShiftRegisterData" ;
  }

}; // class

}
#endif 
