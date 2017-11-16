#include <string>
#include <sstream>
#include <bitset>

#include "EncodingStringHelper.hh"

std::string CALICE::EncodingStringHelper::GetFieldDesc(const std::string FieldName, 
                                                       const unsigned int FieldMask, 
                                                       const unsigned int FieldShift, 
                                                       const unsigned int startbit) 
{

  std::bitset<64> aBitset(FieldMask);
  std::ostringstream FieldDesc;

  FieldDesc << FieldName << ":" << FieldShift + startbit << ":" << aBitset.count();

  return FieldDesc.str();
    
}
