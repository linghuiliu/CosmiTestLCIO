#ifndef CaliceHit_h
#define CaliceHit_h 1

#ifdef HAVE_CONFIG_H
#include <config.h> 
#endif
 
#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define CALICEHITNINT 5
#define CALICEHITNFLOAT 2
#define CALICEHITNDOUBLE 0

namespace CALICE {

/**
  * Class for the CALICE Hcal calorimeter hit information during the
  * reconstruction. 
  *
  * Information is stored module wise (compared to x,y,z position in
  * CalorimeterHits). Energies are stored as float (compared to ADC counts in 
  * RawCalorimeterHits).
  * @author S. Schmidt DESY
  * @date Jun 15 2006
  */
  
class CaliceHit: public LCFixedObject<CALICEHITNINT,CALICEHITNFLOAT,CALICEHITNDOUBLE> {

public: 

  /** default constructor
   */
  CaliceHit() {};
  
  /** constructor, initialise CaliceHit according to parameters
    * @param moduleID module ID of the module the hit is in, low byte gives upper or 
    *   lower half, high byte gives module "stamp", used to find correct calibration
    * @param chip number of the chip the hit is in, used to find correct calibration
    * @param channel number of the channel the hist is in, used to find correct calibration
    * @param valueEnergy "energy" property of the hit (also ADC hits, energy in units of MIPs, etc.)
    * @param errorEnergy error on the "energy" property of the hit
    * @param timeStamp timeStamp of the hit
    * @param type of the hit, can be used to differentiate between different levels of CaliceHits (e.g. 
    *   hits with pedestal subtraction, with MIP calibration, etc.)
    */  
  CaliceHit(unsigned moduleID, unsigned chip, unsigned channel, float valueEnergy,
            float errorEnergy, int timeStamp, unsigned type) {
    
    obj()->setIntVal(4, timeStamp);
    obj()->setIntVal(0, moduleID);
    obj()->setIntVal(1, chip);
    obj()->setIntVal(2, channel);
    obj()->setIntVal(3, type);
  
    obj()->setFloatVal(1, errorEnergy);
    obj()->setFloatVal(0, valueEnergy);
  };
  
  CaliceHit(LCObject* obj): LCFixedObject<CALICEHITNINT,CALICEHITNFLOAT,CALICEHITNDOUBLE>(obj) {};
  
  virtual ~CaliceHit() {};
  
  /** get module number of hit
   */
  inline unsigned getModuleID() const {
    return (unsigned) (getIntVal(0));
  }; 
  
  /** get chip number of hit
   */
  inline unsigned getChip() const {
    return (unsigned) (getIntVal(1));
  };
  
  /** get channel number of hit
   */
  inline unsigned getChannel() const {
    return (unsigned) (getIntVal(2));
  };
  
  /** get cellKey
    */
  inline unsigned getCellKey() const {
    return (unsigned) ((getChip() << 8) + getChannel());
  }  
  
  /** get energy value of hit 
   */
  inline float getEnergyValue() const {
    return getFloatVal(0);
  };
  
  /** get energy error of hit
   */
  inline float getEnergyError() const {
    return getFloatVal(1);
  };
  
  /** get time stamp
   */
  inline int getTimeStamp() const {
    return getIntVal(4);
  };      
  
  /** get reconstruction status of hit
   */
  inline unsigned getType() const {
    return (unsigned) (getIntVal(3));
  };  

  /** set energy value of hit 
   */
  inline void setEnergyValue(float energyValue) {
    obj()->setFloatVal(0, energyValue);
  };
  
  /** set energy error of hit
   */
  inline void setEnergyError(float energyError) {
    obj()->setFloatVal(1, energyError);
  };
  
  /** set time stamp
   */
  inline void setTimeStamp(unsigned timeStamp) {
    obj()->setIntVal(4, timeStamp);
  };      
  
  /** set reconstruction status of hit
   */
  inline void setType(unsigned type) {
    obj()->setIntVal(3, type);
  };  

  /** convenient print method
   */
  void print (std::ostream& os);
  
  /** return the type of the class
   */
  const std::string getTypeName() const {
    return "CaliceHit";    
  }

  /** return a brief description of the data memeber
   */
  const std::string getDataDescription() const {
    return "i:ModuleID,i:Chip,i:Channel,f:EnergyValue,f:EnergyError,i:TypeStamp,i:Type";
  } 
  
};   
}  

#endif
  
  
