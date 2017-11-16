#ifndef ReadOutConfigurationBlock_hh
#define ReadOutConfigurationBlock_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
//CRP Use 'local' interface to LCGenericObjects
#include "LCGenericObjectImplExt.hh"
#define NLENSLOFE 5
#define BESOFTBIT 9

#ifdef BOUNDARY_CHECK
#  include <cassert>
#endif

using namespace lcio ;
using namespace CALICE;

namespace CALICE {

  /** The indices of all the stored values.
   */
enum ERoConfIntValues {kRoConfIntNumbers,
                            kRoConfIntSlotEnable,
                            kRoConfIntSlotFeEnable,
                            kRoConfIntVmePeriod=kRoConfIntSlotFeEnable+NLENSLOFE,
                            kRoConfIntBePeriod,
                            kRoConfIntBecPeriod,
                            kRoConfIntFePeriod,
                            kRoConfIntBeSoftTrigger,
                            kRoConfIntFeSoftTrigger,
                            kRoConfIntFeBrcSoftTrigger,
                            kRoIntConfMode,
                            kNRoConfIntValues=kRoIntConfMode};


 /** Stores the general readout configuration.
   * e.g. to decide whether  a softtrigger was enabled
   * In addition information about general hardware
   * configuration can be retrieved.  
   * For a give crate the enabled slots can be
   * obtained by the getSlotEnable method
   * For this slot in turn the enabled Fe can be found
   * by withe getFeSlotEnable(int) method  
   * e.g. 
   * fe = getFeSlotEnable((slot-2)/4) 
   * byte = (slot-2)%4
   * => bits_enabled = (ife>>(8*byte))&0xff;
   * The eight bits of enabled tells you which
   * fe's were enabled in the given slot
   * The eight LSB returned by the getFeSoftTrigger method
   * tells you whether softtriggers were enabled for
   * a given set of fe's.
   * 
   * @sa ConditionsChangeDelegator
   * @author R. Poeschl LAL (based on the other interface classes) 
   * 
   * @date May  2006
   */

class ReadOutConfigurationBlock : public LCGenericObjectImplExt{


public:

  /** Default Constructor */
      ReadOutConfigurationBlock() {/*no op*/}; 

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      ReadOutConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 


     /** Set the crate number */
#ifdef DEPRECATED 
    ReadOutConfigurationBlock &setCrateNumber(int number) {setCrateID(number); return *this;}
#endif

    ReadOutConfigurationBlock &setCrateID(int number) {
     obj()->setIntVal(kRoConfIntNumbers, number); 
     return *this;
    }    

#ifdef DEPRECATED 
    /** Get the crate number */
    int getCrateNumber() const {
       return getCrateID();

    } 
#endif

    /** Get the crate number */
    int getCrateID() const {
       return getIntVal(kRoConfIntNumbers);
    } 


    /** Set the slot enable */
    ReadOutConfigurationBlock &setSlotEnable(int slotEnable) {
     obj()->setIntVal(kRoConfIntSlotEnable, slotEnable); 
     return *this;
      
    }    
 
    /** Get the slot enable */
    int getSlotEnable() const {
       return getIntVal(kRoConfIntSlotEnable);

    } 
  
    /** is the slot enabled? */
    bool isSlotEnabled(unsigned int slot_i) const {
#ifdef BOUNDARY_CHECK
      assert(slot_i >=getFirstSlot() && slot_i<=getLastSlot() );
#endif
      return static_cast<bool>( getSlotEnable() & (1<<slot_i));
    };


    /** Set the Slot FeEnable */
    ReadOutConfigurationBlock &setFeSlotEnable(int slotFeEnable, int pos) {
       obj()->setIntVal(kRoConfIntSlotFeEnable+pos, slotFeEnable); 
      return *this;

    }    
 
    /** Get the Slot FeEnable */
    int getFeSlotEnable(int pos) const {
        return getIntVal(kRoConfIntSlotFeEnable+pos);
    } 


    /** is the slot enabled? */
    bool isFeEnabled(unsigned int slot_i, unsigned int fe_i) const {
#ifdef BOUNDARY_CHECK
      assert(slot_i >=getFirstSlot() && slot_i<=getLastSlot() );
      assert(fe_i < 8 );
#endif
      unsigned int eff_slot_i=slot_i-getFirstSlot();
      // for each slot 2-22 one byte is used which are packed in 5 integers
      return static_cast<bool>( getFeSlotEnable(eff_slot_i/4) & ( 1<< (fe_i+(8*eff_slot_i%4)) ) );
    };


    /** Set the vme period */
    ReadOutConfigurationBlock &setVmePeriod(int vmePeriod) {
     obj()->setIntVal(kRoConfIntVmePeriod,vmePeriod); 
     return *this;


    }    
 
    /** Get the vme period */

    int getVmePeriod() const {
       return getIntVal(kRoConfIntVmePeriod);
    } 

    /** Set the be period */
    ReadOutConfigurationBlock &setBePeriod(int bePeriod) {
     obj()->setIntVal(kRoConfIntBePeriod,bePeriod); 
     return *this;
    }    
 
    /** Get the be period */
    int getBePeriod() const {
       return getIntVal(kRoConfIntBePeriod);

    } 

    /** Set the bec period */
    ReadOutConfigurationBlock &setBecPeriod(int becPeriod) {
     obj()->setIntVal(kRoConfIntBecPeriod,becPeriod); 
     return *this;
    }    
 
    /** Get the bec period */
    int getBecPeriod() const {
       return getIntVal(kRoConfIntBecPeriod);

    } 

    /** Set the fe period */
    ReadOutConfigurationBlock &setFePeriod(int fePeriod) {
     obj()->setIntVal(kRoConfIntFePeriod,fePeriod); 
     return *this;
    }    
 
    /** Get the fe period */
    int getFePeriod() const {
       return getIntVal(kRoConfIntFePeriod);
    } 

    /** Set the Be Soft Trigger */
    ReadOutConfigurationBlock &setBeSoftTrigger(int beSoftTrigger) {
     obj()->setIntVal(kRoConfIntBeSoftTrigger,beSoftTrigger); 
     return *this;
    }    
 
    /** Get the Be Soft Trigger Config.*/
    int getBeSoftTrigger() const {
       return getIntVal(kRoConfIntBeSoftTrigger);
    } 


    /** Set the Fe Soft Trigger(s) Config.*/
    ReadOutConfigurationBlock &setFeSoftTrigger(int feSoftTrigger) {
     obj()->setIntVal(kRoConfIntFeSoftTrigger,feSoftTrigger); 
     return *this;
    }    
 
    /** Get the Fe Soft Trigger Config.*/
    int getFeSoftTrigger() const {
       return getIntVal(kRoConfIntFeSoftTrigger);
    } 


     /** Get the front-end soft trigger */
    bool getFeSoftTrigger(unsigned int fe_i) const {
#ifdef BOUNDARY_CHECK
      assert(fe_i < 8 );
#endif
      return static_cast<bool>(  getFeSoftTrigger() & (1<<fe_i));
    }


    /** Set the Broadcast Soft Trigger */
    ReadOutConfigurationBlock &setFeBrcSoftTrigger(int feBrcSoftTrigger) {
     obj()->setIntVal(kRoConfIntFeBrcSoftTrigger, feBrcSoftTrigger); 
     return *this;
    }    
 
    /** Get the Broadcast Soft Trigger Config.*/
    int getFeBrcSoftTrigger() const {
       return getIntVal(kRoConfIntFeBrcSoftTrigger);
    } 


    /** Set the Conf Mode */
    ReadOutConfigurationBlock &setConfMode(int confMode) {
     obj()->setIntVal(kRoIntConfMode, confMode); 
     return *this;
    }    
 
    /** Get the Conf Mode */
    int getConfMode() const {
       return getIntVal(kRoIntConfMode);
    } 

    static unsigned int getFirstSlot() {
      return 2;
    }

    static unsigned int getLastSlot() {
      return 21;
    }


    /** Convenient print method 
     */

     void print(  std::ostream& os);

   // -------- need to implement abstract methods from LCGenericObject

    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"ReadOutConfigurationBlock" ;
    } 

     /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:crate_number,i:slotEnable,i[5]:slotFeEnable,i:vmePeriod,i:bePeriod,i:becPeriod,i:FePeriod,i:SoftTrigger,i:ConfMode" ;}



 };


}

#endif
