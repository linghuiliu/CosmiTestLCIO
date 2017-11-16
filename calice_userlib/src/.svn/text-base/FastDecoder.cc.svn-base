#include "FastDecoder.hh"


#include <iomanip>
#include <sstream>
#include <cmath>

namespace CALICE {

  FastDecoder* FastDecoder::generateDecoder(const std::string& encoding, const std::string& variableName) {

    unsigned int position=0;
    unsigned int mask=0;
    int offset=0;
    bool isSigned=false;

    unsigned int currentPosition = 0;

    std::stringstream stream;
    if (encoding == "")  throw WrongDataFormatException(std::string("encoding string cannot be empty\n"));
    stream << encoding;
    std::string token="";
    while ( ! ( stream.fail() | stream.eof() ) )
      {
        //std::cout<<" \nFastDecoder: encoding="<<encoding<<std::endl;

        std::getline(stream, token, ',');
        std::size_t firstSep = token.find(':');
        int size = 0;

        //check if relative or absolute position
        std::size_t secondSep = token.find(':',firstSep+1);
        if (secondSep == std::string::npos)
          {
            std::stringstream converterSize;
            converterSize << token.substr(firstSep+1);

            if ((converterSize >> size).fail())
              {
                std::stringstream errormessage;
                errormessage << "cannot convert size "<< converterSize.str() << " to integer " << std::endl;
                throw  WrongDataFormatException( errormessage.str() );
              }
            if (size>0) currentPosition += size;
            else currentPosition -= size;
            //      std::cout << "relative position increased to " << currentPosition << std::endl;
          }
        else
          {
            std::stringstream converterSize;
            converterSize << token.substr(secondSep+1);

            if ((converterSize >> size).fail())
              {
                std::stringstream errormessage;
                errormessage << "cannot convert size "<< converterSize.str() << " to integer " << std::endl;
                throw  WrongDataFormatException( errormessage.str() );
              }


            std::stringstream converterPosition;
            converterPosition << token.substr(firstSep+1,secondSep-firstSep-1);
            if ((converterPosition >> currentPosition).fail()) {
              std::stringstream errormessage;
              errormessage << "cannot convert position "<< converterPosition.str() << " to integer " << std::endl;
              throw  WrongDataFormatException( errormessage.str() );
            }
            currentPosition += size;
          }

        //      std::cout<<" FastDecoder: variableName: "<<variableName<<" bool1: "<<(token.substr(0,firstSep).find(variableName) == 0)
        //               <<" bool2: "<<(token.find_first_of("+-:") == variableName.size())
        //               <<std::endl;
        //      std::cout<<"   token="<<token<<" firstStep="<<firstSep<<" token.subr: "<<token.substr(0,firstSep)<<std::endl;

        if (token.substr(0,firstSep).find(variableName) == 0 && token.find_first_of("+-:") == variableName.size() )
          { //starts with the searched name and next is one of +-:

            std::string identifier = token.substr(0,firstSep);

            //check for offset
            if (identifier.size() > variableName.size()) {
              std::stringstream converter;
              converter << identifier.substr(variableName.size());
              if ((converter >> offset).fail()) {
                std::stringstream errormessage;
                errormessage << "cannot convert offset "<< converter.str() << " to integer " << std::endl;
                throw  WrongDataFormatException( errormessage.str() );
              }
            }



            if (size < 0 ) {
              isSigned = true;
              mask = (unsigned int)pow(2,-size)-1;
            }
            else {
              isSigned = false;
              mask = (unsigned int)pow(2,size)-1;
            }

            position=currentPosition-size;
            /*
              std::cout << "found " ;
              if (isSigned) std::cout << " signed ";
              else std::cout << "unsigned ";
              std::cout << variableName << " at " << position << " size: " << size <<" mask: " << std::hex << mask << " with offset " << std::dec << offset << std::endl;
            */
            //std::cout<<" FastDecoder: end"<<std::endl;

            return new FastDecoder(position,mask,isSigned,offset);
          }


      }
    return 0;
  }
}
