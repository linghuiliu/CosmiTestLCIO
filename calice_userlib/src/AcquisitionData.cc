#include "AcquisitionData.hh"


std::ostream& AcquisitionData::print(std::ostream &ostrm) {

  ostrm << "AcquisitonData" << std::endl;
  ostrm << "Acquisition start? " << getAcquisitionState() << std::endl;
  ostrm <<  "Acquisition number in run " << getAcquisitionNumberInRun() << std::endl;
  ostrm << " In configuration " << getAcquisitionNumberInConfiguration()  << std::endl;
  if(getAcquisitionState()) { 
    ostrm << " Max. events in acquisition " << static_cast<unsigned int>(getMaxEventNumberInAcquisition()) <<  std::endl;
    ostrm << " Max time of acquisition/sec: " << getMaxAcquisitionTimeSec() << "." << getMaxAcquisitionTimeMus() << std::endl;
  }
  else { 
    ostrm << " Actual events in acquisition " << static_cast<unsigned int>(getActEventNumberInAcquisition()) <<  std::endl;
    ostrm << " Actual time of acquisition/sec: " << getActAcquisitionTimeSec() << "." << getActAcquisitionTimeMus() << std::endl;
  }
  ostrm << " Has dif data: " << getDifDataPresent()  << std::endl;
  ostrm << " DIF Triggercounter: " << getDifTriggerCounter()  << std::endl;
  ostrm << " DIF Number of buffer words: " << getNumberofDifBufferWords()  << std::endl;
  ostrm << " ***************************************************************" << std::endl;
  return ostrm;

}
