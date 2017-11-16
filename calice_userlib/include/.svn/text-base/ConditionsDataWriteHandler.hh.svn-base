#ifdef USE_LCCD
#ifndef _ConditionsDataWriterHandler_hh_
#define _ConditionsDataWriterHandler_hh_


#include <lccd/IConditionsChangeListener.hh>
#include <lccd/IConditionsHandler.hh>
#include <lccd/LCConditionsMgr.hh>
#include <lccd/DBInterface.hh>
#include <marlin/ConditionsProcessor.h>
#include <EVENT/LCCollection.h>
#include <string>
#include <stdexcept>
#include <sstream>

//#define DEBUG_CONDDB_WRITER
//#ifdef DEBUG_CONDDB_WRITER
//#include <iostream>
//#include <iomanip>
//#include <UTIL/LCTime.h>
//#endif


namespace CALICE {

  /** Clone a collection.
   * This will clone all the elements and the int, float and string parameters.
   * This should be everything.
   * NOTE: depending on the size of the collection, and the number of parameters this
   * may be a very expenisve operation.
   */
  EVENT::LCCollection *cloneCollection(EVENT::LCCollection *col);

  /** Handler of conditions data changes which writes the replaced collections to a conditions data base.
   * When ever conditions data changes the old (cloned) collection will be stored with the time stamps
   * of the event just before the former conditions data change and the time stamp of the last event.
   * At the end of the run, the conditions data has to be written manually by calling 
   * @ref writeConditionsData() with the time stamp of the last event.
   */
  class ConditionsDataWriteHandler : public lccd::IConditionsChangeListener 
  {
  public:
    
    /** constructor.
     * @param col_name the name of the to be monitored collection (not needed).
     * @param db_init_string specifier of the database host name, database name, user and password.
     * @param folder_name name of the destination folder.
     * @param tag to be applied at the end of the run to the folder.
     * @param description the description added to every collection written toe the data base.
     */
    ConditionsDataWriteHandler(const std::string &col_name, 
			       const std::string &db_init_string,
			       const std::string &folder_name,
			       const std::string &tag,
			       const std::string &description);

    /** destructor.
     */
    ~ConditionsDataWriteHandler() {
      delete _col;
    }

    /** Be notified if conditions data changes and redirect changes to responsible class.
     * @param col the collection containing the new conditions data
     */
    void conditionsChanged( lcio::LCCollection* col );

    /** write the conditions data to the conditions database.
     * Must be called at the end of the run to 
     */
    void writeConditionsData(const long64 &best_guess_of_till_time_stamp);

    /** Set the correct since time for the current conditions data. 
     * The since time will be initialised by the time of the 
     * last event. But this is just too early. So, the @ref marlin::ConditionsDataWriter
     * has to set the time of the next event.
     */
    void setSinceTime(long64 since_time) {_since=since_time;}

    const std::string &name() const {return _colName;}
    
    /** Get the number of collections written to the database;
     */ 
    unsigned int numberOfWrites() const {return _writes;}

    /** Get the number of recognised changes of the conditions data.
     */ 
    unsigned int changes() const {return _writes;}

    /** Get the time of the first recognised conditions data change.
     */
    long64 validAtMostSince() const {return _first;}

    /** Get the time of the last write to the data base.
     */
    long64 validAtMostTill()  const {return _last;}

    /** Get the time of the first recognised conditions data change.
     */
    long64 currentValidSince() const {return _since;}

    /** Get the time of the last write to the data base.
     */
    long64 currentValidTill()  const {return _till;}

    /**
   */
    const std::string &getTag() const {return _tag;}

  private:
    const std::string _colName;                /**< name of the collection (not used)*/
    lccd::DBInterface _db;

    lccd::IConditionsHandler *_changeHandler;   /**<handler of the conditions data*/

    const std::string _tag;        
    const std::string _description;

    long64 _since;                      /**< the time of the event just before the conditions data changed.*/
    long64 _till;                       /**< Best guess of the till time stamp (may be wrong).*/
    LCCollection *_col;                 /**< pointer to the clone of the conditions data collection.*/

    // to gather some statistics
    unsigned int _changes;              /**< keep track of the number of changes.*/
  
    unsigned int _writes;               /**< keep track of the number of conditions data written to the 
					     database.*/

    long64 _first;
    long64 _last;

  };
}
#endif

#endif //use LCCD
