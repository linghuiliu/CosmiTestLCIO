#ifndef HCAL_TILE_INDEX_HH
#define HCAL_TILE_INDEX_HH 1

//#define HTI_MOD_MASK  0x0000003F
#define HTI_MOD_MASK  0x0000001F //for EPT Prototype
//#define HTI_CHIP_MASK 0x000007C0
#define HTI_CHIP_MASK 0x000003C0 //for EPT Prototype
//#define HTI_CHAN_MASK 0x0000F800
#define HTI_CHAN_MASK 0x0000FC00 //for EPT Prototype
#define HTI_SIPM_MASK 0xFFFF0000

#define HTI_MOD_SHIFT   0
#define HTI_CHIP_SHIFT  6
//#define HTI_CHAN_SHIFT 11
#define HTI_CHAN_SHIFT 10 //for EPT Prototype
#define HTI_SIPM_SHIFT 16

#include <string>
#include <sstream>

#include <iostream>

#include "EncodingStringHelper.hh"

namespace CALICE {
/**
   @brief Encodes/decodes hardware channel information for the CALICE AHCAL prototype

   This class encodes and decodes module, chip and channel as well as the 
   SiPM number into/from a single integer. This integer is intended to be 
   stored in the CellID1 field of LCIO CalorimeterHit objects for easy reference 
   to and unique identification of the hardware information of reconstructed hits.

   @author Niels.Meyer@desy.de
   @date Nov. 17, 2007
   @version 1.0

*/
class HcalTileIndex{
 public:
  /** Empty constructor. All parameters default to zero.
   */
  HcalTileIndex(){
    setModule(0).setChip(0).setChannel(0).setSipm(0);
  }

  /** Constructor from an integer for decoding
   */
  HcalTileIndex(int index){
    _index = index;
  }

  /** Constructor from the minimum set of parameters required for unique identification. 
      The SiPM number defaults to zero. 

      @param mod Module identifier
      @param chip ASIC chip identifier
      @param chan Multiplex channel identifier
   */
  HcalTileIndex(short mod, short chip, short chan) : _index(0) {
    setModule(mod).setChip(chip).setChannel(chan).setSipm(0);
  }

  /** Constructor from the full set of parameters

      @param mod Module identifier
      @param chip ASIC chip identifier
      @param chan Multiplex channel identifier
      @param sipm SiPM identifier
   */
  HcalTileIndex(short mod, short chip, short chan, short sipm){
    setModule(mod).setChip(chip).setChannel(chan).setSipm(sipm);
  }

  /** Destructor - does nothing
   */
  ~HcalTileIndex(){};

  /** Decode module identifier
   */
  unsigned short getModule() const {
    return (_index & HTI_MOD_MASK) >> HTI_MOD_SHIFT;
  }

  /**  Derive module type from module and chip identifier
   */
  unsigned short getModuleType() const {

    // module numbering starts at 1, always return type 0 for invalid
    // modules
    if(getModule() == 0) return 0;

    short type = 0;
    if ( getChip() > 5 ) ++type;        //  lower part
    if ( getModule() > 30 ) type += 2;  //  coarse modules
    return type;
  }

  /** Decode ASIC identifier
   */
  unsigned short getChip() const {
    return (_index & HTI_CHIP_MASK) >> HTI_CHIP_SHIFT;
  }

  /** Decode multiplex channel
   */
  unsigned short getChannel() const {
    return (_index & HTI_CHAN_MASK) >> HTI_CHAN_SHIFT;
  }
  
  /** Decode SiPM number
   */
  unsigned short getSipm() const {
    return (_index & HTI_SIPM_MASK) >> HTI_SIPM_SHIFT;
  }

  /** Return THE index
   */
  unsigned int getIndex() const {
    return _index;
  }

  /**  Encode module identifier
   */
  HcalTileIndex& setModule(short mod){
    _index = (_index & ~HTI_MOD_MASK) | ((mod << HTI_MOD_SHIFT) & HTI_MOD_MASK);
    return (*this);
  }

  /** Encode ASIC identifier
   */
  HcalTileIndex& setChip(short chip){
    _index = (_index & ~HTI_CHIP_MASK) | ((chip << HTI_CHIP_SHIFT) & HTI_CHIP_MASK);
    return (*this);
  }

  /** Encode multiplex channel
   */
  HcalTileIndex& setChannel(short chan){
    _index = (_index & ~HTI_CHAN_MASK) | ((chan << HTI_CHAN_SHIFT) & HTI_CHAN_MASK);
    return (*this);
  }

  /** Encode SiPM number
   */
  HcalTileIndex& setSipm(short sipm){
    _index = (_index & ~HTI_SIPM_MASK) | ((sipm << HTI_SIPM_SHIFT) & HTI_SIPM_MASK);
    return (*this);
  }

  /** Return the cellid encoding string according to an lcio BitField64
   */
  static std::string getEncodingString(const unsigned int startbit) {
    
    std::ostringstream EncodingString;
  
    EncodingString << EncodingStringHelper::GetFieldDesc("module",  HTI_MOD_MASK,  HTI_MOD_SHIFT, startbit)
                   << "," 
                   << EncodingStringHelper::GetFieldDesc("chip",    HTI_CHIP_MASK, HTI_CHIP_SHIFT, startbit)
                   << ","
                   << EncodingStringHelper::GetFieldDesc("channel", HTI_CHAN_MASK, HTI_CHAN_SHIFT, startbit)
                   << ","
                   << EncodingStringHelper::GetFieldDesc("SiPM",    HTI_SIPM_MASK, HTI_SIPM_SHIFT, startbit);
  
    return EncodingString.str();
  
  }



 protected:
  unsigned int _index;

 private:

};

inline std::ostream& operator<<(std::ostream& out, 
                         const CALICE::HcalTileIndex& hti) {

  out << "module: " << hti.getModule()
      << " chip: " << hti.getChip()
      << " channel: " << hti.getChannel()
    //        << " SiPM: " << hti.getSipm()
    ;

  return out;

}

}

#endif
