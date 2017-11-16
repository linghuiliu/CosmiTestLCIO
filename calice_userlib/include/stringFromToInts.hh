/** Store strings in LCGenericObjects.
 * TODO: Create LCGenericObjects which can store strings.
 */
#ifndef _stringFromToInts_hh_
#define _stringFromToInts_hh_

#include <string>

namespace CALICE {
  /** Get the number of integer elements needed to store the string.
   */
  unsigned int getNeededInts(const std::string &a_string);

  /** Get the number of integer elements needed to store the string.
   */
  inline unsigned int getNeededInts(unsigned int string_length) {
    return (string_length+sizeof(int)-1)/sizeof(int);
  }
  
  /** convert a string to integers which are stored at the given position in the integer array of the given LCGenericObject.
   * @param an_obj the generic LCObject in which the string is going to be stored.
   * @param a_string the string to be stored.
   * @param string_start_index the index of the element in the integer array to which the first character of the string will be written.
   * @param string_length_index the index of the element in the integer array where the length of the string will be stored.
   * @return the index of first unused element of the integer array after the string.
   */
  unsigned int convertStringToInts(lcio::LCGenericObjectImpl *an_obj, const std::string &a_string, unsigned int string_start_index, unsigned int string_length_index);

  /** Get a string which is encoded into an integer array.
   * @param an_obj the generic LCObject which contains a string.
   * @param string_start_index the index of the element in the integer array which contains the first character.
   * @param string_length_index the index of the element in the integer array which contains the string length.
   */
  std::string getStringFromInts(lcio::LCGenericObjectImpl *an_obj, unsigned int string_start_index, unsigned int string_length_index);
}
#endif
