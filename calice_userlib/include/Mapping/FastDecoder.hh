#ifndef FASTDECODER_HH
#define FASTDECODER_HH

#include <string>
#include "CaliceException.hh"
#include <iostream>

namespace CALICE {

  /** decode IDs with encoding string
   *
   * This class decodes a single value from a CELL ID. In differs to the LCIO::BitField in two ways.
   *
   * @par interpretation of Y-n
   * If the encoding string contains a variable name in the form of \f$ name\pm N \f$,
   * \f$ N \f$ is interpreted as offset to variable \f$ name \f$. It will be subsracted automatically.
   * Example: If encodingstring is "K-1:3,I:3,J:3" and variable name is "K", \f$ value+1 \f$ will be returned.
   *
   * @par no string comparison in decoding
   * All necessary string comparison is done during the initialisation. This should give some performance increase,
   * if the same decoder is used many times.
   *
   * The constructor is private. Use the FastDecoder::generateDecoder() function to get an object.
   *
   * @author Benjamin.Lutz@desy.de
   * @date   April 2009
   */
  class FastDecoder {

  public:
    /** function to generate a specific decoder for one variable
     * @param[in] encodingString encoding string used to interprete the input
     * @param[in] variableName variable the encoder should extract
     * @returns decoder for specific string and variable
     */
    static FastDecoder* generateDecoder(const std::string& encodingString, const std::string& variableName);

    /** decode signed
     * @param[in] input ID to be interpreted
     * @returns value of the variable
     * @throws WrongDataFormatException if called for an unsigned variable
     */
    int decode(const int input) const {
      if (! _isSigned) throw WrongDataFormatException(std::string("cannot decode unsigned value as signed\n"));
      return  ((input >> _position) & _mask) - _offset;
    }

    /** decode unsigned
     * @param[in] input ID to be interpreted
     * @returns value of the variable
     * @throws WrongDataFormatException if called for a signed variable
     */
    unsigned int decodeU(const int input) const {
      if (_isSigned)  throw WrongDataFormatException(std::string("cannot decode signed value as unsigned\n"));
      return  ((input >> _position) & _mask) - _offset;
    }

    /** encode signed
     * @param[in] value to encode
     * @returns ID with value set
     * @throws WrongDataFormatException if called for an unsigned variable
     */
    int encode(const int value) const {
      if (! _isSigned) throw WrongDataFormatException(std::string("cannot encode unsigned value as signed\n"));
      return (((value + _offset) & _mask ) << _position);
    }

    /** encode signed
     * @param[in] value to encode
     * @returns ID with value set
     * @throws WrongDataFormatException if called for an unsigned variable
     */
    int encodeU(const unsigned int value) const {
      if (_isSigned) throw WrongDataFormatException(std::string("cannot encode signed value as unsigned\n"));
      return (((value + _offset) & _mask ) << _position);
    }

  private:
    FastDecoder(const unsigned int position, const unsigned int mask, const bool isSigned, const int offset) {
      _position = position;
      _mask = mask;
      _isSigned = isSigned;
      _offset = offset;
    }

    unsigned int _position;
    unsigned int _mask;
    bool _isSigned;
    int _offset;
  };

} //namspace CALICE


#endif
