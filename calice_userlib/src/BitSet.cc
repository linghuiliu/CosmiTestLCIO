#include "BitSet.hh"

namespace CALICE {

  std::ostream & operator << (std::ostream &os, const BitSet &bits) {
    std::vector<int> bitVec = bits.getIntVec();
    for (  std::vector<int>::const_iterator iter = bitVec.begin(); iter != bitVec.end(); ++iter) {
      os << *iter;
    }
    return os;
  }

}
