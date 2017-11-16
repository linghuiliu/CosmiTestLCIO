#ifndef __Ahc2HardwareConnection_hh__
#define ___Ahc2HardwareConnection_hh__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE{

  /**  @brief  Class to store int values for HardwareConnection plus
   *           integer cell index in LCIO
   *
   *   This class provides an interface to store four numbers inside an LCIO
   *   object:
   *   - an integer identifier, e.g. the index of a calorimeter cell
   *   - a 1st int value, ChipID
   *   - a 2nd int value, ModuleNumber 
   *   - a 3rd int value, ChipNumber
   *
   *   This class is used in Ahc2CalibrateProcessor to get ModuleNumber and ChipNumber from 
   *   Hardware information
   *
   *   @author Eldwan Brianne
   *   @date   November 2015
   */

  class Ahc2HardwareConnection : public UTIL::LCFixedObject<4, 0, 0> {
  public:
    /**  Empty constructor
     */
    Ahc2HardwareConnection() {};

    /**  Constructor with initial values
     */
    Ahc2HardwareConnection( const int id, const int Chip, const int ModuleNumber, const int ChipNr )
    {
      obj()->setIntVal( 0, id );
      obj()->setIntVal( 1, Chip );
      obj()->setIntVal( 2, ModuleNumber );
      obj()->setIntVal( 3, ChipNr );
    }
    
    /**  Constructor from LCObject
     */
    Ahc2HardwareConnection( EVENT::LCObject* obj ) : UTIL::LCFixedObject<4,0,0>( obj ){}

    /**  Destructor
     */
    ~Ahc2HardwareConnection(){}

    /**  Return index identifier
     */
    int getID() const {
      return getIntVal( 0 );
    }

    /**  Return the ChipID
     */
    int getChip() const {
      return getIntVal( 1 );
    }

    /**  Return the ModuleNumber
     */
    int getModuleNumber() const {
      return getIntVal( 2 );
    }

    /**  Return the ChipNumber
     */
    int getChipNumber() const {
      return getIntVal( 3 );
    }

    /**  set index identifier
     */
    void setID(const int ID) {
      obj()->setIntVal( 0, ID );
    }

    /** set the ChipID
     */
    void setChip(const int Chip) {
      obj()->setIntVal( 1, Chip );
    }

    /**  set the ModuleNumber
     */
    void setModuleNumber(const int ModuleNumber) {
      obj()->setIntVal( 2, ModuleNumber );
    }

    /**  set the ChipNumber
     */
    void setChipNumber(const int ChipNumber) {
      obj()->setIntVal( 3, ChipNumber );
    }

    /**  Implementation of LCGenericObject::getTypeName
     */
    virtual const std::string getTypeName() const {
      return "Ahc2HardwareConnection";
    }

    /**  Implementation of LCGenericObject::getDataDescription
     */
    virtual const std::string getDataDescription() const {
      return "i:ID,i:Chip,i:ModuleNumber,i:ChipNumber";
    }

  };  // class Ahc2HardwareConnection

}// namespace CALICE

#endif
