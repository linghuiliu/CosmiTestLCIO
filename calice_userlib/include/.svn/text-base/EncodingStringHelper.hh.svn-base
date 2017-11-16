#ifndef ENCODING_STRING_HELPER
#define ENCODING_STRING_HELPER 1

#include <string>

namespace CALICE {

  /** Utility class to get the field description according to an lcio BitField64
   *   @author S. Richter (DESY)
   *   @date March 13 2007
   */

class EncodingStringHelper {

public:

  /**
   *
   * @param FieldName - the identifier of the field, e.g. "module"
   * @param FieldMask - the bitmask which specifies the bits used for this field
   * @param FieldShift - using the shift together with the mask you get field at the lowest bits
   * @param startbit - an overall shift of all fields
   *
   */
  static std::string GetFieldDesc(const std::string FieldName, 
                                  const unsigned int FieldMask, 
                                  const unsigned int FieldShift, 
                                  const unsigned int startbit) ;
};

}

//}

#endif
