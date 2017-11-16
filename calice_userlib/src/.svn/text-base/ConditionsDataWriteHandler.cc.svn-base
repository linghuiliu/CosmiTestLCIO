#ifdef USE_LCCD
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <EVENT/LCParameters.h>
#include <EVENT/LCObject.h>
#include "ConditionsDataWriteHandler.hh"
#include <cloneUtils.hh>

#include <iostream>
#include <iomanip>
#include <UTIL/LCTime.h>

namespace CALICE {

  ConditionsDataWriteHandler::ConditionsDataWriteHandler(const std::string &col_name, 
							 const std::string &db_init_string,
							 const std::string &folder_name,
							 const std::string &tag,
							 const std::string &description)
    : _colName(col_name),
      _db(db_init_string, folder_name, true), 
      _tag(tag),
      _description( description ),
      _col(0),
      _changes(0),
      _writes(0),
      _first( lccd::LCCDPlusInf ),
      _last( lccd::LCCDMinusInf )
  {
    
    
    marlin::ConditionsProcessor::registerChangeListener( this, _colName );
    std::cout << "ConditionsDataWriteHandler::ctor> New conditions data writer for collection \"" << _colName << std::endl; 
    
    _changeHandler = lccd::LCConditionsMgr::instance()->getHandler( _colName );
    std::cout << "collection name, _changeHandler: " << _colName <<
      ", " << _changeHandler << std::endl;		
    
    std::cout << "Address of this ConditionDataWriteHandler: " << this << std::endl;
    
    if (!_changeHandler) {
      std::stringstream message;
      message << "ConditionsDataWriteHandler::ctor> No conditions data handler for collection " << _colName << ".";
      throw std::runtime_error(message.str());
    }
    
    //CRP 4/2/06 Initialize _since and _till variables
    //If the collection or conditions data which is treated by the handler
    //does never appear this values do not check and thus we can
    //check for them. 
    //Remember this handler treats conditions data which should
    //have a well defined validity time
    _since = lccd::LCCDMinusInf;
    _till = lccd::LCCDPlusInf;
    
  }
  

  void ConditionsDataWriteHandler::conditionsChanged( lcio::LCCollection* col ) 
  {
    _changes++;
#ifdef DEBUG_CONDDB_WRITER
    {
      

      std::cout << "ConditionsDataWriteHandler::conditionsChanged> change nr " << _changes << " -- " << _colName << " -- " <<  std::endl;
      //Get the change handler for this collection
      //Cannot set it in constructor since we don't know a priori whether the 
      //corresponding data type exists in the data stream
      //_changeHandler = lccd::LCConditionsMgr::instance()->getHandler( _colName );      

      UTIL::LCTime since( _changeHandler->validSince() );
      UTIL::LCTime till( _changeHandler->validTill() );
      std::cout << std::setw(20) << "new conddb handler: " << since.getDateString() << " - " << till.getDateString()
		<< std::endl;
    }
#endif

    if (col) {
      //      lccd::IConditionsHandler *change_handler = lccd::LCConditionsMgr::instance()->getHandler( _colName );
      ///      if (_changeHandler) {

      if ( _col ) {

	// write conditionsData

	// take since time from new conditions data as till time
	// of the to be written conditions data 
	writeConditionsData( _changeHandler->validSince() );
	
	// delete the cloned collection.
	delete _col;
      }

      // memorise the new collection.
      // Unfortunately, this requires an expensive cloning of the collection.
      _col=cloneCollection(col);

      // get  time stamps
      _since=_changeHandler->validSince();        // should be trustworthy
      _till=_changeHandler->validTill();          // may not be trustworthy since this information is not knwon for 
	
#ifdef DEBUG_CONDDB_WRITER
      UTIL::LCTime first(_since);
      UTIL::LCTime last(_till);
	
      std::cout << std::setw(20) << "new validity interval:" <<  first.getDateString() << " - " << last.getDateString() << std::endl;
#endif	
    }
  }

  void ConditionsDataWriteHandler::writeConditionsData(const long64 &best_guess_of_till_time_stamp) 
  {
    
    if (!_col) return;
    
    long64 till_time_stamp = best_guess_of_till_time_stamp;
    if (till_time_stamp<_since) {
      till_time_stamp=lccd::LCCDPlusInf;
    }


    // memorise the minimum maximum validity bounds for the final report (no other purpose)
    if (_since<_first) _first=_since;
    if (till_time_stamp>_last) _last=till_time_stamp;

    //CRP 4/2/06 A final sanity check 
    if( till_time_stamp > _since ) { 


      _db.storeCollection(_since, till_time_stamp, _col, _description);


#ifdef DEBUG_CONDDB_WRITER
      std::cout << "ConditionsDataWriteHandler::writeConditionsData> store collection -- " << _colName 
		<< " -- (" << _description << ")" << std::endl;
	
      UTIL::LCTime first(_since);
      UTIL::LCTime last(till_time_stamp);
      
      std::cout << std::setw(20) << "Validity interval:" <<  first.getDateString() << " - " << last.getDateString() << std::endl;
#endif
      // do some statistics for the final report
      _writes++;

      //Unavoidably there are collections for which till=since
      //e.g. the run sammary coming along with the run end record
    } else if (till_time_stamp == _since){
      _db.storeCollection(_since, _since+1LL, _col, _description);


#ifdef DEBUG_CONDDB_WRITER
      std::cout << "ConditionsDataWriteHandler::writeConditionsData> store collection -- " << _colName 
		<< " -- (" << _description << ")" << std::endl;
	
      UTIL::LCTime first(_since);
      UTIL::LCTime last(till_time_stamp);
      
      std::cout << std::setw(20) << "Validity interval:" <<  first.getDateString() << " - " << last.getDateString() << std::endl;
#endif
      // do some statistics for the final report
      _writes++;
     }else {

      //CRP Issue the warning for all collections except the 
      //Run Info Collection where this case occurs "by construction"
      std::cout << "ConditionsDataWriteHandler::writeConditionsData> failed to store collection -- " << _colName  << " -- (" << _description << ")" << std::endl;
	
      std::cout << "Since Time >= Till Time !" << std::endl; 
      UTIL::LCTime first(_since);
      UTIL::LCTime last(till_time_stamp);
      
      std::cout << std::setw(20) << "Validity interval:" <<  first.getDateString() << " - " << last.getDateString() << std::endl;
	
    } 
  }
  
}

#endif
