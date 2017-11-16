#ifdef USE_LCCD
#ifndef _ConditionsDataWriter_hh_
#define _ConditionsDataWriter_hh_

#ifndef DEBUG_CONDDB_WRITER
//#define DEBUG_CONDDB_WRITER
#endif

#include <RtypesSubSet.h>
#include <string>
#include <vector>

#include <marlin/Processor.h>
#include <lccd/DBInterface.hh>
#include <lccd/IConditionsChangeListener.hh>

#include "ConditionsDataWriteHandler.hh"

namespace marlin {


  /** Marlin processr which catches selected conditions data changes and writes them to the conditions data base.
   * The processor does not verify whether the data already exists it writes every change 
   * to the database.
   *
   * The conditions data changes are catched by individual handlers which clone the 
   * conditions data collection and store the cloned collection when the handler is 
   * notified of the next change.
   *
   * The conditions data change handler does not have any knowledge of the record time 
   * which triggered the conditions data change. To solve this problem, the handlers request
   * a correction of the since time stamp. This request will be approximatively fulfilled
   * by setting the since time stamp to the time stamp of the event which arrived just
   * after the conditions data change.
   */
  class ConditionsDataWriter : public marlin::Processor
  {
    friend class CALICE::ConditionsDataWriteHandler;
  public:
    Processor*  newProcessor() { return new ConditionsDataWriter ; };

    ConditionsDataWriter();
    
    ~ConditionsDataWriter();
    

    /** Install conditions change listener.
     * The listeners are installed for the conditions data specified by the processor 
     * parameters.
     */
    void init();

    /** Do nothing at run start.
     */
    void processRunHeader( LCRunHeader* run);

    /** Memorise the event time stamps. 
     * The time stamp of the last event before the member function end is called is needed to 
     * write the dangling conditions data with correct "since" and "till" time stamps to the
     * conditions data base.
     */
    void processEvent( LCEvent * evtP );

    /** Write all not yet written conditions data to the conditions data base. 
     * The writing of the conditions data is delayed until the conditions data changes to 
     * set the "since" and "till" time stamps. All the conditions data which is not yet stored
     * in the conditions database is written now.
     * Finally if the the version is not HEAD, the folders are tagged.
     */
    void end();

    long64 getTimeStampOfLastEvent() const {return _timeStampOfLastEvent; };

  protected:
    
    std::string _dbInit;            /**< parameter to be filled with the host name, database name,user name and the password.*/
    StringVec _condDataCollections; /**< collection name, destination CondDB folder and version of 
				         the conditions data to be written to the conditions database.*/ 

    long64 _timeStampOfLastEvent;    /**< The time stamp of the last event that was processed.*/

#ifdef DEBUG_CONDDB_WRITER
    UInt_t _lastRunNumber;
    UInt_t _nEventsPerRun;
    UInt_t _nRuns;
    long64 _timeStampOfFirstEvent;
    bool _newRun;
    UInt_t _firstEvent;
    UInt_t _lastEvent;
#endif

    std::vector<CALICE::ConditionsDataWriteHandler *> _handler;
  };
}
#endif

#endif //USE_LCCD
