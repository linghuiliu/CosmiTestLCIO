#ifndef _TLineIterator_hh_
#define _TLineIterator_hh_
#include <RtypesSubSet.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

class TLineIterator
{
public:
  //! Object which allows to iterate over one line
  /*!
    \param buffer pointer to the line buffer. The buffer is expected to be an null terminated ASCII coded buffer.
  */
  TLineIterator(const Char_t *buffer) : fPtr(buffer) {UnSpace();};

  Bool_t IsEmpty() const {return *fPtr==0;};

  //! Read the current value, which is supposed to be an unsigned integer
  /*!
    \return the integer value at the current position

    If the value is not an unsigned integer or the end is already reach an exception of type runtime_error is thrown.
  */
  Int_t GetInteger() throw(runtime_error) 
  {
    if (!UnSpace()) throw runtime_error("Missing integer");

    const Char_t *ptr=fPtr;
    if (*ptr=='-') ptr++;
    while (isdigit(*ptr)) ptr++;
    if (*ptr && !isspace(*ptr)) throw runtime_error("Not an integer");
    Int_t value=atoi(fPtr);
    fPtr=ptr;
    UnSpace();
    return value;
  };

  //! Check if the current value is a character.
  /*!
    \return true if the current value is a character
  */
  Bool_t IsAlpha()
  {
    if (!UnSpace()) return false;

    return isalpha(*fPtr);
  };

  //! Check if the next value is a number
  /*!
    \param allow_sign if set to true than a negative sign is allowed at the beginning.
    \return true if the next values form a number
  */
  Bool_t IsInteger(bool allow_sign=false)
  {
    
    const Char_t *ptr=fPtr;
    if (!UnSpace()) return false;
    if (allow_sign && (*ptr=='-')) ptr++;
    while (isdigit(*ptr)) ptr++;

    return (!(*ptr) || isspace(*ptr));
  };


  //! Check whether the next word is a hexadeciaml number.
  /*! 
    \return true if the next word is a hexadeciaml number.
  */
  Bool_t IsHex()
  {
    const Char_t *ptr=fPtr;
    if (!UnSpace()) return false;
    if ((*ptr=='0')) ptr++;
    if ((*ptr=='x')) ptr++;
    while (isdigit(*ptr) || (*ptr>='A' && *ptr<='F') || (*ptr>='a' && *ptr<='f')) ptr++;
    
    if (!(*ptr) || isspace(*ptr)) {
      return true;
    }
    else return false;
  }
  
  //! Check if the next value is binary number
  /*!
    \return true if the next values form a number
  */
  Bool_t IsBinary()
  {
    
    const Char_t *ptr=fPtr;
    std::cout << "Unspace: " << UnSpace() <<std::endl;
    if (!UnSpace()) return false;
    while ((*ptr)=='0' || (*ptr)=='_' || (*ptr=='1')) ptr++;
    

    //const UInt_t* myintPtr = reinterpret_cast<const UInt_t*>(ptr);
    //const long myint =  reinterpret_cast<const long>(myintPtr);
    std::cout << "Diff: " << reinterpret_cast<unsigned long>(ptr)-reinterpret_cast<unsigned long>(fPtr) << std::endl;
    //return true;
    return (!(*ptr) || isspace(*ptr) && (reinterpret_cast<unsigned long>(ptr)-reinterpret_cast<unsigned long>(fPtr))>=8);
  };

  //! Check if the current value is a comment marker (#).
  /*!
    \return the integer value at the current position

    If the value is not an unsigned integer or the end is already reach an exception of type runtime_error is thrown.
  */
  Bool_t IsComment()
  {
    if (!UnSpace()) return false;

    return *fPtr=='#';
  };

  //! Read the current value, which is supposed to be a character array without white spaces and which contains printable ASCII letters
  /*!
    \return the string 

    If the value contains "non printable" characters or the end is already reach an exception of type runtime_error is thrown.
  */
  string GetWord(bool allow_start_with_non_alpha=true) throw(runtime_error);

  //! Read the current value, which is supposed to be a character array without white spaces and which contains printable ASCII letters
  /*!
    \param dest the character array is appended to the given string

    If the value contains "non printable" characters or the end is already reach an exception of type runtime_error is thrown.
  */
  void GetWord(string &dest, bool allow_start_with_non_alpha=true) throw(runtime_error);

  const char *GetBuffer() const {
    return fPtr;
  };

  //! Read the current value, which is supposed to be a positive or negative integer
  /*!
    \return the integer value at the current position

    If the value is not an integer or the end is already reach an exception of type runtime_error is thrown.
  */
  UInt_t GetUnsignedInteger() throw(runtime_error) 
  {
    if (!UnSpace()) throw runtime_error("Missing integer");

    const Char_t *ptr=fPtr;
    while (isdigit(*ptr)) ptr++;
    if (*ptr && !isspace(*ptr)) throw runtime_error("Not an integer");
    Int_t value=atoi(fPtr);
    fPtr=ptr;
    UnSpace();
    return value;
  };

  //! Get a binary number.
  /*!
    \return the binary number
    \throw runtime_error If the value is not a binary number.
  */
  UInt_t GetBinary();


  //! Read the current value, which is supposed to be a floating point value.
  /*!
    \return the floating point value at the current position converted to a double

    If the value is not a flaoting point value or the end is already reach an exception of type runtime_error is thrown.
  */
  Double_t GetDouble() throw(runtime_error)
  {
    if (!UnSpace()) throw runtime_error("Missing floating point value");

    const Char_t *ptr;
    Double_t value=strtod(fPtr,(Char_t **) &ptr);
    if (*ptr && !isspace(*ptr))  throw runtime_error("Not a floating point value");
    fPtr=ptr;
    UnSpace();
    return value;
  };

  //! Get a hexadecimal number.
  UInt_t GetHex();

  
  //! Check whether the end of the buffer is reached. 
  /*!
    \return kTRUE if the end of the buffer is reached. Otherwise kFALSE is returned.
   */
  Bool_t eof() const {return *fPtr=='\0';};

protected:
  //! skip white spaces
  /*!
    \return kFAALSE if the end of the buffer is reached. Otherwise kTRUE is returned.
   */
  Bool_t UnSpace() {
    while (*fPtr) {
      if (!isspace(*fPtr)) return true;
      fPtr++;
    }
    return true;
  };

private:
  const Char_t *fPtr; //!< pointer to current position in the buffer
};

#endif
