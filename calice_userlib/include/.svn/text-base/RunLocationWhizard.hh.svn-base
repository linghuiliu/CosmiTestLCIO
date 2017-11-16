#ifdef USE_LCCD
#ifndef RunLocationWhizard_h
#define RunLocationWhizard_h 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"

#include "lccd.h"


#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"


using namespace lcio;
using namespace lccd;
using namespace std;

namespace CALICE {
  
 
  /** Simple interface class to retrieve information about the location of a run
   *  It accesses the runlocation folder in the database. The access to this
   * should happen via this class.
   *  <p>
   * @author R. Poeschl LAL
   * @date Jun. 2007
   */
  class RunLocationWhizard{
    
  public: 
    
    /** Constructor taking db collection name and the string to access the db as input*/
    RunLocationWhizard(std::string, std::string);
    
    /** Simple Constructor with using defaults for the values*/
    RunLocationWhizard();

    /** Important for memory handling*/
    ~RunLocationWhizard() { /* no op*/  }
    
    
    /** get the run location, input is runnumber */
    std::string getRunLocation(int); 
  
    /** get the generic run, type input is runnumber*/
    std::string getGenericRunType(int); 

    /** get the generic month of running, type input is runnumber*/
    std::string getGenericRunMonth(int); 
    
     
    /** Convenient print method */
    void print( int,  std::ostream& os ) ;
    

    
  private: 
    /** Folder name */
    std::string _folder;
    /** dbinit string */
    std::string _dbinit;
    /** A conditions data handler */
    lccd::IConditionsHandler* _conData;
    /** A suimple method to create the conditions handler */
    void createConditionsHandler();


  }; // class
  
}
#endif 
#endif
//=============================================================================
