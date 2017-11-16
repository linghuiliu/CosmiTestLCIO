#ifndef _RunTimeProcessor_hh_
#define _RunTimeProcessor_hh_

#include <string>
#include <vector>
#include <iostream>
#include <marlin/Processor.h>


namespace marlin {
  /** Simple processor which allows for an eay relation bwetween runnumbers and
    * runtime and therefore a convenient browsing of the database.  
    * @author: R.Poeschl LAL
    *
    * @date Jun  2007
    */

  class RunTimeProcessor : public marlin::Processor
  {
  public:
    Processor*  newProcessor() { return new RunTimeProcessor ; }
    
    RunTimeProcessor();
    
    ~RunTimeProcessor() {};
    
    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

     /** Called for every run, e.g. overwrite to initialize run dependent
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run);
   
    
    /** Called for every event - the working horse.      */
    void processEvent( LCEvent * evtP );
    
    void end();
    
  protected:
    
    /*Runnmber which will be set as a validity time*/
    int _runnum;
    /* Run Start time */
    long64 _beginofrun; 
    /* Time of last event in run which will be used as run stop time */
    long64 _endofrun;

    /* Bool which turns to fals after the first encountered event = begin of run */
    bool _beginOfRunFetched;

    /*The folder in which we will store the run time info */
    std::string _colRuntime;

    /* The string containing the parameters to access the db */
    std::string _dbinit;

    bool _flag; //flag for same begin and end run
   
  };

}
#endif
