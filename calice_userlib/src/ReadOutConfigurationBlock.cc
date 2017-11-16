#include <ReadOutConfigurationBlock.hh>
#include <ostream>

namespace CALICE {
    /** Convenient print method 
     */

     void ReadOutConfigurationBlock::print(  std::ostream& os) {
         os << " Crate Number=" << std::hex << getCrateID()
	    << " Slot enable=" << getSlotEnable()
	    << " Slot FeEnable 0=" << getFeSlotEnable(0)
	    << " Slot FeEnable 1=" << getFeSlotEnable(1)
	    << " Slot FeEnable 2=" << getFeSlotEnable(2)
	    << " Slot FeEnable 3=" << getFeSlotEnable(3)
	    << " Slot FeEnable 4=" << getFeSlotEnable(4)
	    << " Vme Period=" << getVmePeriod()
	    << " Be Period=" << getBePeriod()
	    << " Bec Period=" << getBecPeriod()
	    << " Fe Period=" << getFePeriod()
	    << " Be Soft Trigger=" << getBeSoftTrigger()
	    << " Fe Broadcast Soft Trigger=" << getFeBrcSoftTrigger()
	    << " Conf Mode=" << getConfMode()
	    << std::dec << std::endl;

    }
}
