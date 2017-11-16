//
// File:    GLTimeUtil.hh
// Purpose:
//    Decodes time stamp in input arguments, from YYYY:MM:DD:HH:mm:ss
//    into LCCDTimeStamp format.
//
// 070806 - G.Lima - extracted from cddata/dbfill/sipmmapfromdb.cc
//
#include "lccd.h"
#include <iostream>
#include <string>
#include <cstdlib>

// -- To get the current time
#define SECTONS 1000000000LL
#define MUSECTONS 1000LL
#include <ctime>
//to set the desired time
#define TIMEVECSIZE 6
#define YEAR 0
#define MONTH 1
#define DAY 2
#define HOUR 3
#define MINUTE 4
#define SECONDS 5

lccd::LCCDTimeStamp interpretTimeStamp(char* arg) {
  //The usertime
  std::string usertimeStr(arg);
  std::vector<int> usertimevec;
  std::string usertime_stream;
  for (unsigned i = 0; i < usertimeStr.length(); ++i) {
    if ( usertimeStr[i] != ':' ) {
      if( !isdigit( usertimeStr[i] ) ) {
	std::cout << "Error in reading <timestamp> value " << std::endl;
	std::cout << "Check format of <timestamp> value " << std::endl;
	std::cout << "Will leave the program now " << std::endl;
	exit(1);
      }
      usertime_stream += usertimeStr[i];
    }
    else {
      usertimevec.push_back( atoi ( usertime_stream.c_str() ) );
      usertime_stream = " ";
    }

    if (i == usertimeStr.length()-1) {
      usertimevec.push_back( atoi ( usertime_stream.c_str() ) );
    }
  }

  if ( usertimevec.size() < TIMEVECSIZE ) {
    std::cout << "Error in reading <timestamp> value " << std::endl;
    std::cout << "Don't have the desired six time parameters" << std::endl;
    std::cout << "Will leave the program now " << std::endl;
    exit(1);
  }

//   std::cout <<"usertime="<< usertimevec.at(YEAR) <<":"<< usertimevec.at(MONTH) <<":"<< usertimevec.at(DAY) <<":"<< usertimevec.at(HOUR) <<":"<< usertimevec.at(MINUTE) <<":"<< usertimevec.at(SECONDS) << std::endl;
  LCTime usertime( usertimevec.at(YEAR),
		   usertimevec.at(MONTH),
		   usertimevec.at(DAY),
		   usertimevec.at(HOUR),
		   usertimevec.at(MINUTE),
		   usertimevec.at(SECONDS)) ;

  return usertime.timeStamp();
}
