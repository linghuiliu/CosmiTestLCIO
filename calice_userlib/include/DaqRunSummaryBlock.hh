#ifndef DaqRunSummaryBlock_hh
#define DaqRunSummaryBlock_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
//CRP Use 'local' interface to LCGenericObjects
#include "LCGenericObjectImplExt.hh"

using namespace lcio ;
//using namespace CALICE;

namespace CALICE {
  
  /** The indices of all the stored values.
   */
  enum EDaqSumIntValues{kDaqRunNum,
                        kDaqNumConf,
			kDaqNumSro,
			kDaqNumAcq,
			kDaqNumEvt,
                        kDaqRunTime,
			kDaqNumIntVals};
  
  
  /** Stores a summary of a run in terms of Number of Congfiuration Changes, Slow Readouts,
   * Acquisitions and  Number of Events in additions it stores the time of a run and for convenience
   * the run number
   * @sa ConditionsChangeDelegator
   * @author R. Poeschl LAL (based on the other interface classes) 
   * 
   * @date Sep  2007
   */
  
  class DaqRunSummaryBlock : public LCGenericObjectImplExt{

    
    
  public:
    
    /** Default Constructor */
    DaqRunSummaryBlock() {}; 
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      DaqRunSummaryBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 
    
    
    /** Set the run number*/
    DaqRunSummaryBlock &setRunNumber(int runnum) {
      obj()->setIntVal(kDaqRunNum, runnum); 
      return *this;
    }    
    
    /**Return the Number of Configurations*/
    int getRunNumber() {return getIntVal(kDaqRunNum);}

    /** Set the number of configurations */
    DaqRunSummaryBlock &setNumberOfConfigurations(int numconf) {
      obj()->setIntVal(kDaqNumConf, numconf); 
      return *this;
    }    
    
    /**Return the Number of Configurations*/
    int getNumberOfConfigurations() {return getIntVal(kDaqNumConf);}


    /** Set the number of Slowreadouts */
    DaqRunSummaryBlock &setNumberOfSlowReadouts(int numsro) {
      obj()->setIntVal(kDaqNumSro, numsro); 
      return *this;
    }    
    
    /**Get the Number of Slowreadouts*/
    int getNumberOfSlowReadouts() {return getIntVal(kDaqNumSro);}


    /** Set the number of  Acquisitions*/
    DaqRunSummaryBlock &setNumberOfAcquisitions(int numacq) {
      obj()->setIntVal(kDaqNumAcq, numacq); 
      return *this;
    }    
    
    /**Get the Number of Acquisitions*/
    int getNumberOfAcquisitions() {return getIntVal(kDaqNumAcq);}


    /** Set the number of  Acquisitions*/
    DaqRunSummaryBlock &setNumberOfEvents(int numevt) {
      obj()->setIntVal(kDaqNumEvt, numevt); 
      return *this;
    }    
    
    /**Get the Number of Events*/
    int getNumberOfEvents() {return getIntVal(kDaqNumEvt);}

    /** Set the Run duration*/
    DaqRunSummaryBlock &setRunDuration(int runtime) {
      obj()->setIntVal(kDaqRunTime, runtime); 
      return *this;
    }    
    
    /**Get the Number of Events*/
    int getRunDuration() {return getIntVal(kDaqRunTime);}

    
    
    /** Convenient print method */
    void print(  std::ostream& os); 

   // -------- need to implement abstract methods from LCGenericObject
    
    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"DaqRunSummaryBlock" ;
    } 

    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:numconf,i:numsro,i:numsro,i:numevt,i:runduration";}
    
    

  };


}

#endif
