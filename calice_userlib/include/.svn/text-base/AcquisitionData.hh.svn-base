#ifndef AcquisitionData_HH
#define AcquisitionData_HH 1


#include <iostream>

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"

using namespace std;
using namespace lcio;
using namespace CALICE;


//class EventHeader : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {

namespace CALICE {

enum EAcqDataValues { kAcqDataIntValAcqNumberRun,
                      kAcqDataIntValAcqNumberConf,
                      kAcqDataIntValAcqEvtInfo,
                      kAcqDataIntValAcqTimeInfSec,
                      kAcqDataIntValAcqTimeInfMus,
                      kAcqDataIntValAcqState,
                      kAcqDataIntValHasDifInfo,
                      kAcqDataIntValDifTriggerCounter,
                      kAcqDataIntValDifNdataBufferWords,
                      kAcqDataIntValues};



class AcquistionData;

/**Class to store acquisition data info, 
 * i.e. acquisiton# in run, acquisition# in configuration,max. events
 * in acquistion 
 * 
 *  Note this is for internal use only
 *  \todo should we make a regular userlib class out of it? 
 *  @author R. Pöschl (LAL Orsay)
 *  @date Feb 27 2006
 */

class AcquisitionData : public LCGenericObjectImplExt 
{
  

   public: 
  
   /**The constructor
    */

  AcquisitionData(int acquisitionNumberInRun, 
               int acquisitionNumberInConfiguration)
    //int maxTimeOfAcquisitionsec,
    //int maxTimeOfAcquisitionmus) 
   {
      obj()->setIntVal(kAcqDataIntValAcqNumberRun, acquisitionNumberInRun);
      obj()->setIntVal(kAcqDataIntValAcqNumberConf, acquisitionNumberInConfiguration);
      //obj()->setIntVal(kAcqDataIntValAcqMaxEvts, maxEventsInAcquisition);  
      obj()->setIntVal(kAcqDataIntValAcqEvtInfo, 0);  
      obj()->setIntVal(kAcqDataIntValAcqTimeInfSec, 0);  
      obj()->setIntVal(kAcqDataIntValAcqTimeInfMus, 0);  
      //obj()->setIntVal(kAcqDataIntValAcqMaxTimesec, maxTimeOfAcquisitionsec);  
      //obj()->setIntVal(kAcqDataIntValAcqMaxTimemus, maxTimeOfAcquisitionmus);  
      //DIF data 5/12/10
      obj()->setIntVal(kAcqDataIntValHasDifInfo, 0);   
      obj()->setIntVal(kAcqDataIntValDifTriggerCounter, 0);   
      obj()->setIntVal(kAcqDataIntValDifNdataBufferWords, 0);   
      }
//............................................................................
   /**A copy constructor
    */
   AcquisitionData(LCObject* obj) : LCGenericObjectImplExt(obj) 
   { 
   } 
//............................................................................
   /**The destructor 
    */
   virtual ~AcquisitionData() 
   { 
   }
//............................................................................  
  /** Sets the acquisition state, start=1, end=0 */
  AcquisitionData &setAcquisitionState(bool acqstate) {
    obj()->setIntVal(kAcqDataIntValAcqState, static_cast<int>(acqstate));
    return *this;
   }

  /** Returns the acquisition state */
  bool getAcquisitionState() const {
    return static_cast<bool>(getIntVal(kAcqDataIntValAcqState));
  } 

  /** Set the time information for the given state (in sec), note that the meaning is different for start and end.
      This will be handled further down */
   AcquisitionData &setTimeInformationSec(unsigned int timeinfo) {
   obj()->setIntVal(kAcqDataIntValAcqTimeInfSec, static_cast<int>(timeinfo));
    return *this;
   }

  /** Set the time information for the given state (in sec), note that the meaning is different for start and end.
      This will be handled further down */
  AcquisitionData &setTimeInformationMuSec(unsigned int timeinfo) {
    obj()->setIntVal(kAcqDataIntValAcqTimeInfMus, static_cast<int>(timeinfo));
    return *this;
   }

  

   /**returns the acquisition number in run  
    */
   unsigned int getAcquisitionNumberInRun() 
   {
      return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqNumberRun));
   }
//............................................................................    
   /**returns the acquisition number in configuration
    */
   unsigned int getAcquisitionNumberInConfiguration() 
   {
      return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqNumberConf));
   }

  /** Sets the eventnumber info */
  AcquisitionData &setEventNumberInfo(unsigned int evnuminfo) {
    obj()->setIntVal(kAcqDataIntValAcqEvtInfo, static_cast<int>(evnuminfo));
    return *this;
   }

   /**returns the maximal number of events in Acquisition
    */
   unsigned int getMaxEventNumberInAcquisition()  
    {
      if(!getAcquisitionState()) {
	std::cout << "Acquisition data - warning: Asking max event number in acq for wrong state" << std::endl;
	std::cout << "Acquisition data - warning: State is Acquisition end" << std::endl;
	std::cout << "Acquisition data - warning: Can only ask for actual event number" << std::endl;
	std::cout << "Acquisition data - warning: Will return 0" << std::endl;
        return 0;
      }

     return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqEvtInfo));
   }

   /**returns the actual number of events in Acquisition
    */
   unsigned int getActEventNumberInAcquisition()  
    {
      if(getAcquisitionState()) {
	std::cout << "Acquisition data - warning: Asking act event number in acq for wrong state" << std::endl;
	std::cout << "Acquisition data - warning: State is Acquisition start" << std::endl;
	std::cout << "Acquisition data - warning: Can only ask for maximal event number" << std::endl;
	std::cout << "Acquisition data - warning: Will return 0" << std::endl;
        return 0;
      }

     return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqEvtInfo));
   }


  /** Returns the maximum acquisition time - seconds */

   unsigned int getMaxAcquisitionTimeSec() 
    {

      if(!getAcquisitionState()) {
	std::cout << "Acquisition data - warning: Asking max acquisition time (in sec) for wrong state" << std::endl;
	std::cout << "Acquisition data - warning: State is Acquisition end" << std::endl;
	std::cout << "Acquisition data - warning: Can only ask for actual acquisition time" << std::endl;
	std::cout << "Acquisition data - warning: Will return 0" << std::endl;
        return 0;
      }
      return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqTimeInfSec));
   }

  /** Returns the maximum acquisition time - microseconds */

   unsigned int getMaxAcquisitionTimeMus() 
    {
      if(!getAcquisitionState()) {
	std::cout << "Acquisition data - warning: Asking max acquisition time (in musec) for wrong state" << std::endl;
	std::cout << "Acquisition data - warning: State is Acquisition end" << std::endl;
	std::cout << "Acquisition data - warning: Can only ask for actual acquisition time" << std::endl;
	std::cout << "Acquisition data - warning: Will return 0" << std::endl;
        return 0;
      }
     return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqTimeInfMus));
   }


  /** Returns the actual acquisition time - seconds */

   unsigned int getActAcquisitionTimeSec() 
    {

      if(getAcquisitionState()) {
	std::cout << "Acquisition data - warning: Asking actual acquisition time (in sec) for wrong state" << std::endl;
	std::cout << "Acquisition data - warning: State is Acquisition start" << std::endl;
	std::cout << "Acquisition data - warning: Can only ask for maximal acquisition time" << std::endl;
	std::cout << "Acquisition data - warning: Will return 0" << std::endl;
        return 0;
      }
      return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqTimeInfSec));
   }

  /** Returns the actual acquisition time - microseconds */

   unsigned int getActAcquisitionTimeMus() 
    {
      if(getAcquisitionState()) {
	std::cout << "Acquisition data - warning: Asking actual acquisition time (in sec) for wrong state" << std::endl;
	std::cout << "Acquisition data - warning: State is Acquisition start" << std::endl;
	std::cout << "Acquisition data - warning: Can only ask for maximal acquisition time" << std::endl;
	std::cout << "Acquisition data - warning: Will return 0" << std::endl;
        return 0;
      }
      return static_cast<unsigned int>(obj()->getIntVal(kAcqDataIntValAcqTimeInfMus));
    }



  /** Set whether there are dif data in the acquisition */
  AcquisitionData &setDifInfoPresent(bool difdatapresent) {
   obj()->setIntVal(kAcqDataIntValHasDifInfo, static_cast<int>(difdatapresent));
    return *this;
   }

  /** Return whether dif data are present in the acquisition */
  bool getDifDataPresent() const {
    return static_cast<bool>(getIntVal(kAcqDataIntValHasDifInfo));
  } 

  /** Check dif trigger counter at acquisition start and end */
  AcquisitionData &setDifTriggerCounter(unsigned int triggercounter) {
   obj()->setIntVal(kAcqDataIntValDifTriggerCounter, static_cast<int>(triggercounter));
    return *this;
   }

  /** Return dif trigger counter*/
  unsigned int getDifTriggerCounter() const {
    return static_cast<unsigned int>(getIntVal(kAcqDataIntValDifTriggerCounter));
  } 

  /** Check number of dif buffer words*/
  AcquisitionData &setNumberofDifBufferWords(unsigned int difbufferwords) {
   obj()->setIntVal(kAcqDataIntValDifNdataBufferWords, static_cast<int>(difbufferwords));
    return *this;
   }

  /** Return number of dif trigger buffer words */
  unsigned int getNumberofDifBufferWords() {
    return static_cast<unsigned int>(getIntVal(kAcqDataIntValDifNdataBufferWords));
  } 
  
  

  /**returns the the type name
   */
  const std::string getTypeName() const 
  { 
    return "Acquisitiondata" ;
  } 
  //............................................................................      
  /**returns a brief description of the data stored 
   */
  const std::string getDataDescription() const 
  {
    return "i:acquisitionNumberInRun,i:acquisitionNumberInConfiguration,i:maxEventNumberInAcquisition,i:maxAcqusitionTims_sec,i:maxAcqusitionTims_mus,i:acuisitionstate,i:hasdifdata,i:diftriggercounter,i:numberofdifbufferwords" ;
  }
  //............................................................................      
  /**dumps data content 
   */
  std::ostream& print(std::ostream &ostrm);  
  //............................................................................      
  
  
}; //class AcquisitionData

}
#endif 
//............................................................................
