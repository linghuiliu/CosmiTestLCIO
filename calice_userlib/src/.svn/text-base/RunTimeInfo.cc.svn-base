#include "RunTimeInfo.hh"
#include "UTIL/LCTime.h"

namespace CALICE {



void RunTimeInfo::setRunStartTime(long64 starttime) {
  int theTime[2]; 
  convertlong6422int(starttime, theTime);
  obj()->setIntVal(kRunTimeStartMSB, theTime[0]);
  obj()->setIntVal(kRunTimeStartLSB, theTime[1]);
}

void RunTimeInfo::setRunStopTime(long64 stoptime) {
  int theTime[2]; 
  convertlong6422int(stoptime, theTime);
  obj()->setIntVal(kRunTimeStopMSB, theTime[0]);
  obj()->setIntVal(kRunTimeStopLSB, theTime[1]);
}

  long64 RunTimeInfo::getRunStartTime(){
    long64 timemsb = static_cast<long64>(getIntVal(kRunTimeStartMSB));
    long64 timelsb = static_cast<long64>(getIntVal(kRunTimeStartLSB));
    LCTime timemsb_lctime( static_cast<long64>( (timemsb << 32) & 0xFFFFFFFF00000000LL )  );
    long64 finaltime = ( ( (timemsb << 32 ) & 0xFFFFFFFF00000000LL )  | (timelsb &0xFFFFFFFFLL)  );
    return finaltime;
  }


  long64 RunTimeInfo::getRunStopTime(){
    long64 timemsb = static_cast<long64>(getIntVal(kRunTimeStopMSB)); 
    long64 timelsb = static_cast<long64>(getIntVal(kRunTimeStopLSB));

    long64 finaltime = ( ( (timemsb  << 32 )  &0xFFFFFFFF00000000LL  ) | (timelsb&0xFFFFFFFFLL) );
    return finaltime;
  }


  void  RunTimeInfo::convertlong6422int(long64 inptime, int t[]) {
    t[0] = static_cast<int>( (inptime&0xFFFFFFFF00000000LL) >> 32 );
    t[1] = static_cast<int>( (inptime&0xFFFFFFFFLL) );
  }
  

  void RunTimeInfo::print(  std::ostream& os ){
    
    LCTime starttime(getRunStartTime());
    LCTime stoptime(getRunStopTime());
    os << " Starttime: " <<   starttime.getDateString() << std::endl;
    os << " Stoptime: " <<   stoptime.getDateString() << std::endl;
  }
  
}
