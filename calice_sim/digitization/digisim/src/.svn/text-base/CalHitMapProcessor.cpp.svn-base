#include "CalHitMapProcessor.hpp"

#include "EVENT/LCParameters.h"

#include <cassert>
#include <iostream>
using std::cout;
using std::endl;

namespace digisim {

CalHitMapProcessor aCalHitMapProcessor;

//.. constructor
CalHitMapProcessor::CalHitMapProcessor() : Processor("CalHitMapProcessor") {

  _description=" A singleton providing fast data access to cal hits via maps:\n no need to fill those maps yourself.";

  //.. create an identity modifier
  _mgr = CalHitMapMgr::getInstance();
  assert(_mgr);
}

//.. destructor
  CalHitMapProcessor::~CalHitMapProcessor() {
    _mgr->destroy();
  }


  void CalHitMapProcessor::processRunHeader( LCRunHeader* run) {
    // GL: Removed from here... this is taken care of by RunInfoProcessor
//     cout << "DigiSimProcessor::processRun()  " << name()
// 	 << " in run " << run->getRunNumber()
// 	 << endl ;

//     LCParameters *param = &(run->parameters());
//     param->setValue("RunNumber",230101);
//     param->setValue("StartTime",1148334082);
//     param->setValue("EndTime",1148341279);
  }


void CalHitMapProcessor::processEvent( LCEvent * evt ) {
//   cout << "CalHitMapProcessor::processEvent()  " << name()
// 	    << " in event " << evt->getEventNumber()
// 	    << " (run " << evt->getRunNumber() << ") "
// 	    << endl ;

//   cout<< " Calling setEvent: evt="<< evt << endl;


  _mgr->setEvent(*evt);
}

}// namespace digisim
