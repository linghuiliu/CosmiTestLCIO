#ifndef __DAQconnection_hh__
#define __DAQconnection_hh__ 1

#include "UTIL/LCFixedObject.h"
#include "UTIL/BitField64.h"
#include "EVENT/LCObject.h"
#include <string>


#define defaultEncodingString "channel:8,chip:4,fe:3,slot:5,crate:8"

namespace CALICE{



/**  @brief  Class to store the DAQ address of a singel channel
 *
 *   This class uses the mechanism of the BitField64 to encode and decode the bit positions using a string value.
 *   User defined encoding strings are possible. These have to define following fields:
 *   @li crate   The crate
 *   @li slot    Slot in the crate
 *   @li fe      Frontend on the card in slot
 *   @li chip    Chip on the frontend
 *   @li channel Channel on the chip
 *
 *   @author Benjamin.Lutz@desy.de
 *   @version 1.0
 *   @date September 2008
 */
  class DAQconnection : public UTIL::LCFixedObject<2, 0, 0> {
  public:
    /**  Constructor with initial values
     */
    DAQconnection( const unsigned int crate, const unsigned slot, const unsigned int fe, const unsigned int chip, const unsigned int channel, const std::string enc = defaultEncodingString ):encodingString(enc),bitField(enc){
      setCrate(crate);
      setSlot(slot);
      setFe(fe);
      setChip(chip);
      setChannel(channel);
    }
    
    /**  Constructor from LCObject
     */
    DAQconnection( EVENT::LCObject* object, const std::string enc = defaultEncodingString) : UTIL::LCFixedObject<2,0,0>( object ),encodingString(enc),bitField(enc){
      lcio::long64 value;
      value = ( obj()->getIntVal(0) & 0xFFFFFFFF);
      value = (value << 32) | (obj()->getIntVal(1) & 0xFFFFFFFF);
      bitField.setValue(value);
    }

    /**  Destructor
     */
    ~DAQconnection(){
    }

    void setChannel(unsigned int channel);
    void setChip(unsigned int chip);
    void setFe(unsigned int fe);
    void setSlot(unsigned int slot);
    void setCrate(unsigned int crate);

    unsigned int getChannel() const;
    unsigned int getChip() const;
    unsigned int getFe() const;
    unsigned int getSlot() const;
    unsigned int getCrate() const;

    const std::string getEncodingString() const;

    const std::string getTypeName() const;
    const std::string getDataDescription() const;
    
  private:
    void updateLCobject();
    const std::string encodingString;
    //    static const std::string defaultEncodingString; 

    BitField64 bitField;

  };  // class DAQconnection

  // const std::string DAQconnection::defaultEncodingString = "channel:8,chip:4,fe:3,slot:5,crate:8"; //nice idea but doesn't work, gives problem when destructing class (double free)

}  // namespace CALICE

#endif

