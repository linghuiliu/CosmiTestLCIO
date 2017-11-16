#ifndef _TriggerHistory_hh_
#define _TriggerHistory_hh_

#include <EVENT/LCEvent.h>
#include <EVENT/LCParameters.h>

#ifdef BOUNDARY_CHECK
#  include <stdexcept>
#endif

#include <cassert>
#define  TICKS_TO_NS 25

namespace CALICE {

/** Utility class to chech the zero suppressed trigger history which 
 *  will be atteched to the lcio event.
  * @author G. Gaycken LLR
  * @date Dec 2 2005
  *
  */

class TriggerHistory
{
public:
  TriggerHistory(const EVENT::LCEvent &evt, const std::string &history_name) {
    {
      std::string pos_name(history_name);
      pos_name+="Pos";
      evt.getParameters().getIntVals(pos_name, _position);
    }
    {
      std::string bits_name(history_name);
      bits_name+="Bits";
      evt.getParameters().getIntVals(bits_name, _bits);
    }
    assert( _bits.size() == _position.size() );
  }

  /** Get the number of elements in the zero suppressed trigger history.
   */
  unsigned int getNumberOfElements() const {
    return _position.size();
  }

  /** Get the time difference to the main word in ns.
   */
  int getTimeDifferenceAt(unsigned int element_i) const {
#ifdef BOUNDARY_CHECK
    if (element_i<getNumberOfElements()) {
      throw std::runtime_error("ERROR: element not in trigger history range.");
    }
#endif
    return _position[element_i]*TICKS_TO_NS; //ns
  }

  /** Get the trigger bits which were set for the given element.
   */
  int getBitsAt(unsigned int element_i) const {
#ifdef BOUNDARY_CHECK
    if (element_i<getNumberOfElements()) {
      throw std::runtime_error("ERROR: element not in trigger history range.");
    }
#endif
    return _bits[element_i];
  }

  
  /** Return the position with in the given range where there is trigger activity.
   * @param length_in_ns the range of the trigger fifo in ns which should reflect inactivity.
   * @param trigger_bit_mask a bit mask of all the trigger bits which indicate activity.
   * @return the position in the fifo where there is trigger activity. If there is no trigger activity in the given range then zero is returned.
   * Note: it is not verified whether the main word is far enough away
   * from the corresponding end of the fifo.
   */
  unsigned int cleanUntil(int length_in_ns, unsigned int trigger_bit_mask) const {
    length_in_ns /= TICKS_TO_NS;
    for (unsigned int elm_i=0; elm_i<getNumberOfElements(); elm_i++) {
      if (getTimeDifferenceAt(elm_i)>length_in_ns) return 0;
      if (getBitsAt(elm_i) & trigger_bit_mask) return elm_i;
    }
    return 0;
  }
  
protected:
  std::vector<int> _position;
  std::vector<int> _bits;
};

}
#endif
