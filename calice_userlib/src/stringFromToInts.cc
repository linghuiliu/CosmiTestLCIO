#include <lcio.h>
#include <IMPL/LCGenericObjectImpl.h>
#include "stringFromToInts.hh"

namespace CALICE {
  unsigned int getNeededInts(const std::string &a_string) 
  {
    return getNeededInts(a_string.size());
  }
  
  unsigned int convertStringToInts(lcio::LCGenericObjectImpl *an_obj, const std::string &a_string, unsigned int string_start_index, unsigned int string_length_index)
  {
    // unsigned int name_ints=getNeededInts(a_string);

    // since strings are not supported in the LCGenericObject, the string is encoded as integers.
    an_obj->setIntVal(string_length_index,(int) a_string.size());
    unsigned int index=string_start_index;
    for (unsigned int i=0; i<a_string.size(); i+=sizeof(int)) {
      unsigned int encoded_chars=0;
      for (unsigned int char_i=sizeof(int); char_i-->0;) {
	encoded_chars<<=8;
	if (i+char_i < a_string.size()) {
	  //FIXME: will not work with UTF-16
	  encoded_chars+=(a_string[i+char_i] & 0xff);
	}
      }
      an_obj->setIntVal(index,static_cast<int>(encoded_chars));
      index++;
    }
    return index;
  }

  /** Get the name assigned to the module type.
   * The module type together with the module ID is considered to be unique.
   * Due to the character encoding in an integer array, the function may perform slowly.
   */
  std::string getStringFromInts(lcio::LCGenericObjectImpl *an_obj, unsigned int string_start_index, unsigned int string_length_index)
  {
    unsigned int string_length=an_obj->getIntVal(string_length_index);
    std::string a_string;
    a_string.resize(string_length);
    unsigned int index=string_start_index;
    for (unsigned int i=0; i<string_length; i+=sizeof(int)) {
      unsigned int encoded_chars=an_obj->getIntVal(index);
      unsigned int n_chars=sizeof(int);
      if (n_chars+i>string_length) {
	n_chars=string_length-i;
      }
      for (unsigned int char_i=0; char_i<n_chars; char_i++) {
	//FIXME: will not work with UTF-16
	a_string[i+char_i]=(encoded_chars & 0xff);
	encoded_chars>>=8;
      }
      index++;
    }
    return a_string;
  }
}
