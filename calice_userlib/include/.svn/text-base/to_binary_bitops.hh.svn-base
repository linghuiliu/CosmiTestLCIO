#ifndef to_binary_bitops_hh
#define to_binary_bitops_hh

#include <iostream>
using namespace std;

namespace CALICE {

/** Utility function to print the bit representation of a integer
  *  @author R. Poeschl (LAL/Orsay) 'stolen' from Fred Swartz  - 2001-09-04
  *  @date Jul 21 2006
  * 'stolen' from Fred Swartz  - 2001-09-04 'stolen' from Fred Swartz  - 2001-09-04
  */



  inline void to_binary_bitops(int toprint) {
    cout << "binary : ";
    for (int i=31; i>=0; i--) {
      int bit = ((toprint >> i) & 1);
      cout << bit;
    }
    cout << endl;
    
    
  }

}
#endif
