#include "parseDateTime.hh"
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

UTIL::LCTime parseDateTime(const std::string &datetime1, const std::string &datetime2)
{
  const std::string *dateP=0;
  const std::string *timeP=0;
  bool lcdate=false;
  if (datetime1.find('/')!=std::string::npos && datetime1.find(':')==std::string::npos) {
    dateP=&datetime1;
  }
  else if (datetime1.find('/')==std::string::npos && datetime1.find(':')!=std::string::npos) {
    timeP=&datetime1;
  }
  else {
    if (datetime1.find('.')!=std::string::npos && datetime1.find(':')==std::string::npos) {
      dateP=&datetime1;
      lcdate=true;
    }
    else if (datetime1.find(':')!=std::string::npos && (datetime1.find('.')==std::string::npos || datetime1.find('.')> datetime1.rfind(':'))) {
      timeP=&datetime1;
    }
    else {
      std::stringstream message;
      message << "Bad date or time format:\"" << datetime1 <<  "\".";
      throw std::runtime_error(message.str());
    }
  }

  if (datetime2.find('/')!=std::string::npos && datetime2.find(':')==std::string::npos) {
    dateP=&datetime2;
  }
  else if (datetime2.find('/')==std::string::npos && datetime2.find(':')!=std::string::npos) {
    timeP=&datetime2;
  }
  else {
    if (datetime2.find('.')!=std::string::npos && datetime2.find(':')==std::string::npos) {
      dateP=&datetime2;
      lcdate=true;
    }
    else if (datetime2.find(':')!=std::string::npos && (datetime2.find('.')==std::string::npos || datetime2.find('.')> datetime2.rfind(':'))) {
      timeP=&datetime2;
    }
    else {
      std::stringstream message;
      message << "Bad date or time format:\"" << datetime2 <<  "\".";
      throw std::runtime_error(message.str());
    }
  }
  if (!dateP) {
    throw std::runtime_error("Missing date argument.");
  }
  if (!timeP) {
    throw std::runtime_error("Missing time argument.");
  }
  
  if (lcdate) {
    // 01234567890
    // 28.02.2005
    if ( dateP->size()!=2+1+2+1+4
	 || !isdigit((*dateP)[0]) 
	 || (*dateP)[2]!='.' 
	 || !isdigit((*dateP)[3]) 
	 || (*dateP)[5]!='.'
	 || !isdigit((*dateP)[6]) ) {
      std::stringstream message;
      message << "Badly formed date string :\"" << *dateP <<  "\" expecting 28.02.2005.";
      throw std::runtime_error(message.str());
    }
    
    if ( (timeP->size()!=2+1+2+1+2  && timeP->size()<2+1+2+1+2+1)
	 || !isdigit((*timeP)[0]) 
	 || (*timeP)[2]!=':' 
	 || !isdigit((*timeP)[3]) 
	 || (*timeP)[5]!=':'
	 || !isdigit((*timeP)[6])
	 || (timeP->size()>2+1+2+1+2+1 && ((*timeP)[8]!='.' || !isdigit((*timeP)[9]) )) ) {
      std::stringstream message;
      message << "Badly formed date string :\"" << *timeP <<  "\" expecting 01:01:00 or 01:01:00.0.";
      throw std::runtime_error(message.str());
    }
    UTIL::LCTime atime(atoi(&((*dateP)[6])),
		       atoi(&((*dateP)[3])),
		       atoi(&((*dateP)[0])),
		       atoi(&((*timeP)[0])),
		       atoi(&((*timeP)[3])),
		       atoi(&((*timeP)[6])));
    if (timeP->size()>2+1+2+1+2+1) {
      atime=UTIL::LCTime(atime.timeStamp()+atoi(&((*timeP)[9])));
    }
    return atime;
  }
  else {

  if ( dateP->size()!=4+1+2+1+2 
       || !isdigit((*dateP)[0]) 
       || (*dateP)[4]!='/' 
       || !isdigit((*dateP)[5]) 
       || (*dateP)[7]!='/'
       || !isdigit((*dateP)[8]) ) {
    std::stringstream message;
    message << "Badly formed date string :\"" << *dateP <<  "\" expecting 2005/02/28.";
    throw std::runtime_error(message.str());
  }

    if ( (timeP->size()!=2+1+2+1+2  && timeP->size()<2+1+2+1+2+1)
	 || !isdigit((*timeP)[0]) 
	 || (*timeP)[2]!=':' 
	 || !isdigit((*timeP)[3]) 
	 || (*timeP)[5]!=':'
	 || !isdigit((*timeP)[6])
	 || (timeP->size()>2+1+2+1+2+1 && ((*timeP)[8]!='.' || !isdigit((*timeP)[9]) )) ) {
      std::stringstream message;
      message << "Badly formed date string :\"" << *timeP <<  "\" expecting 01:01:00 or 01:01:00.0.";
      throw std::runtime_error(message.str());
  }
    UTIL::LCTime atime(atoi(&((*dateP)[0])),
		       atoi(&((*dateP)[5])),
		       atoi(&((*dateP)[8])),
		       atoi(&((*timeP)[0])),
		       atoi(&((*timeP)[3])),
		       atoi(&((*timeP)[6])));
    if (timeP->size()>2+1+2+1+2+1) {
      atime=UTIL::LCTime(atime.timeStamp()+atoi(&((*timeP)[9])));
    }
    return atime;
  }
}
