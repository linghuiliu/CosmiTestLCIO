#ifdef USE_LCCD
#ifndef RunTimeWhizard_h
#define RunTimeWhizard_h 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"

#include "lccd.h"

using namespace lcio;
using namespace lccd;


namespace CALICE {
  
 
  /** Simple interface class to retrieve the run start and stop times by means of the 
   * runnumber. It accesses the runtime info folder in the database. The access to this
   * should happen via this class.
   *  <p>
   * @author R. Poeschl LAL
   * @date Jun. 2007
   */
  class RunTimeWhizard{
    
  public: 
    
    /** Constructor taking db collection name and the string to access the db as input*/
    RunTimeWhizard(std::string, std::string);
    
    /** Simple Constructor with using defaults for the values*/
    RunTimeWhizard();

    /** Important for memory handling*/
    ~RunTimeWhizard() { /* no op*/  }
    
    
    /** get the run start time, input is runnumber */
    long64 getRunStartTime(int); 
  
    /** get the run stop time input is runnumber*/
    long64 getRunStopTime(int); 
    
    /** Convenient print method */
    void print( int,  std::ostream& os ) ;
    
    
  private: 
    /** Folder name */
    std::string _folder;
    /** dbinit string */
    std::string _dbinit;
    

  }; // class
  
}
#endif 
#endif
//=============================================================================
