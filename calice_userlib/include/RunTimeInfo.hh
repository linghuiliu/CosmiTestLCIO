#ifndef RunTimeInfo_h
#define RunTimeInfo_h 1

#include "lcio.h"
// #include "LCGenericObjectImplExt.hh"
#include "UTIL/LCFixedObject.h"

using namespace lcio ;

namespace CALICE {
  
   enum EBeEventIntValues {kRunTimeStartMSB,
                           kRunTimeStartLSB,
                           kRunTimeStopMSB,
                           kRunTimeStopLSB,
                           kNRunTimeIntValues};
 
  /** Simple class which holds the start and stop times of runs.
   *  <p>
   * Access to this info should happen via the interface class RunTimeWhizard
   * @author R. Poeschl LAL
   * @date Jun. 2007
   */
  class RunTimeInfo : public LCFixedObject< 4, 0, 0 > {
    
  public: 
    
    /** Convenient c'tor.
     */
    RunTimeInfo() {
      obj()->setIntVal( 0 , 0  ) ;
      obj()->setIntVal( 1 , 0  ) ;
      obj()->setIntVal( 2 , 0  ) ;
      obj()->setIntVal( 3 , 0  ) ;
    }
    
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    RunTimeInfo(LCObject* obj) : LCFixedObject< 4, 0, 0 >( obj ) { } 
    
    /** Important for memory handling*/
    virtual ~RunTimeInfo() { /* no op*/  }
    
    
    /** set the run start time */
    void setRunStartTime(long64); 

    /** get the run start time */
    long64 getRunStartTime(); 
  
    /** set the run stop time */
    void setRunStopTime(long64); 

    /** get the run stop time */
    long64 getRunStopTime(); 
    
    /** Convenient print method */
    void print(  std::ostream& os ) ;
    
    // -------- need to implement abstract methods from LCGenericObject
    
    /** Return type name */
    const std::string getTypeName() const { 
      return"RunTimeInfo   " ;
    } 
    
    /** Return data description */
    const std::string getDataDescription() const {
      return "i:runstart_32msb,i:runstart_32lsb,i:runstart_32msb,i:runstart_32lsb" ;
    }
    
  private: 
    //    int _time[2];
    void convertlong6422int(long64, int[]);    

    
  }; // class
  
}
#endif 
//=============================================================================
