#ifndef __CaliceException_hh__
#define __CaliceException_hh__ 1

#include <exception>
#include <string>

namespace CALICE {

  /**
   * @brief exception to be thrown when the data type in a parameter is wrong
   *
   */
  class WrongDataFormatException : public std::exception {
  public:

    WrongDataFormatException(std::string text) throw() {
      _message = std::string("CALICE::wrongDataFormatException: ") + text;
    } 

    const char* what() const throw() {
      return _message.c_str();
    }
    ~WrongDataFormatException() throw() {;}

    
    
  protected:
    WrongDataFormatException(){;}
    std::string _message;
  };


  /**
   * @brief exception to be thrown when data leads to a conflict
   *
   */
  class BadDataException : public std::exception {
  public:

    BadDataException(std::string text) throw() {
      _message = std::string("CALICE::BadDataException: ") + text;
    } 

    const char* what() const throw() {
      return _message.c_str();
    }
    ~BadDataException() throw() {;}

    
    
  protected:
    BadDataException(){;}
    std::string _message;
  };

}

#endif
