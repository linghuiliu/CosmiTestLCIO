#ifndef MODULE_TYPE_H
#define MODULE_TYPE_H

/** A class to define the different module types.
 *  Currently there are the 
 *  \li AHC Analogue HCal
 *  \li AHC8 Analogue HCal for coarse layers. Only 8 chips instead of 12.
 *  \li TCMT Tail catcher and muon tracker
 *
 *  The constants are defined to be a bit pattern, so they can be combined.
 *  \code
 *  // only use AHC and AHC8
 *  UInt_t useModulesMask = AHC | AHC8;
 *  \endcode
 */

class ModuleType
{
 public:
    static const unsigned int AHC  = 0x1; //The bit for normal HCal modules: 0x1
    static const unsigned int AHC8 = 0x2; //The bit for coarse HCal modules: 0x2
    static const unsigned int TCMT = 0x4; //The bit for the tail catcher: 0x4
  };

#endif// MODULE_TYPE_H

