#ifdef USE_LCCD
#include <ConditionsDataWriter.hh>
#include <vector>
#include <EVENT/LCCollection.h>
#include <UTIL/LCTime.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>


namespace marlin {

  // create instance to make this Processor known to Marlin
  ConditionsDataWriter a_ConditionsDataWriter_instance;

  ConditionsDataWriter::ConditionsDataWriter() 
    : Processor("ConditionsDataWriter")
  {
    _description = "Collect the selected conditons data and write to the assigned CondDB folder." ;


    registerProcessorParameter( "DBInit" , 
				"Initialization string for conditions database"  ,
				_dbInit ,
				std::string("localhost:CaliceCondDB:user:password")) ;

    StringVec default_cond_collections ;
    default_cond_collections.push_back("CollectionName");
    default_cond_collections.push_back("/folder/name");
    default_cond_collections.push_back("HEAD");
    default_cond_collections.push_back("description");
    
    registerOptionalParameter( "ConditionsDataCollections" , 
			       "Define conditions data source (collection name) and destionation (CondDB folder, version and description) ",
			       _condDataCollections ,
			       default_cond_collections,
			       default_cond_collections.size() ) ;

  }

  void ConditionsDataWriter::init() {
    printParameters();

    if (_condDataCollections.size()%4!=0) {
      throw std::runtime_error("For each conditions data collection which should be written to the database\n4 strings are needed: collection name, folder name, version and description.");
    }
    
    for (UInt_t col_i=0; col_i+3<_condDataCollections.size(); col_i+=4) {

      _handler.push_back(new CALICE::ConditionsDataWriteHandler(_condDataCollections[col_i+0], 
								_dbInit, 
								_condDataCollections[col_i+1], 
								_condDataCollections[col_i+2], 
								_condDataCollections[col_i+3]));
    }

    _timeStampOfLastEvent=0;

    
#ifdef DEBUG_CONDDB_WRITER
    _newRun=0;
    _nRuns=0;
#endif
  }

  ConditionsDataWriter::~ConditionsDataWriter() {
    // delete all registered conditionsdata changed handler
    for (std::vector<CALICE::ConditionsDataWriteHandler *>::iterator handler_iter=_handler.begin();
	 handler_iter!=_handler.end();
	 handler_iter++) {
      delete *handler_iter;
    }
    _handler.clear();

  }

  void ConditionsDataWriter::processRunHeader( LCRunHeader* run) {
#ifdef DEBUG_CONDDB_WRITER
    if(_nRuns>0) {
      std::cout << "ConditionsDataWriter::processRunHeader>Run " << _lastRunNumber << " : ";
      UTIL::LCTime first(_timeStampOfFirstEvent);
      UTIL::LCTime last(_timeStampOfLastEvent);
      std::cout << _nEventsPerRun << " event during " <<  first.getDateString() << " - " << last.getDateString() << std::endl
		<< std::endl;
    }
    _nRuns++;
    _lastRunNumber=run->getRunNumber();
    _nEventsPerRun=0;
    _newRun=true;
#endif
    // --- debug
  }

  void ConditionsDataWriter::processEvent( LCEvent * evtP ) {
#ifdef DEBUG_CONDDB_WRITER
    // --- debug
    if (_newRun ) {
      _firstEvent=evtP->getEventNumber();
      _timeStampOfFirstEvent=evtP->getTimeStamp();
    }
    _lastEvent=evtP->getEventNumber();
    _nEventsPerRun++;
    // --- debug
#endif
    _timeStampOfLastEvent=evtP->getTimeStamp();
  }


  void ConditionsDataWriter::end() {

#ifdef DEBUG_CONDDB_WRITER
    // --- debug
    if(_nRuns>0) {
      std::cout << "ConditionsDataWriter::processRunHeader>Run " << _lastRunNumber << " : ";
      UTIL::LCTime first(_timeStampOfFirstEvent);
      UTIL::LCTime last(_timeStampOfLastEvent);
      std::cout << _nEventsPerRun << " event during " <<  first.getDateString() << " - " << last.getDateString() << std::endl
		<< std::endl;
    }
    // --- debug
#endif

    long64 last_record=_timeStampOfLastEvent;

    for (std::vector<CALICE::ConditionsDataWriteHandler *>::iterator handler_iter=_handler.begin();
	 handler_iter!=_handler.end();
	 handler_iter++) {
      
      //long64 temp=last_record;
      if (last_record<(*handler_iter)->currentValidTill() && (*handler_iter)->currentValidTill() != lccd::LCCDPlusInf) {
	last_record=(*handler_iter)->currentValidTill();
      }
      if (last_record<(*handler_iter)->currentValidSince() && (*handler_iter)->currentValidSince() != lccd::LCCDPlusInf) {
	last_record=(*handler_iter)->currentValidSince();
      }
    }
    if (last_record==_timeStampOfLastEvent) {
      std::cout << " ConditionsDataWriter::end> Conditionsdata for last event may be invalid, since time stamp of last event and last record are identical." 
		<< std::endl;
      last_record+=1LL;
    }

    // write the not yet written conditions data to the data base;
    for (std::vector<CALICE::ConditionsDataWriteHandler *>::iterator handler_iter=_handler.begin();
	 handler_iter!=_handler.end();
	 handler_iter++) {
      (*handler_iter)->writeConditionsData(last_record);
    };

    // output some statistics
    
    std::cout << "--- " << name() <<  " Report :" << std::endl;
    for (std::vector<CALICE::ConditionsDataWriteHandler *>::const_iterator handler_iter=_handler.begin();
	 handler_iter!=_handler.end();
	 handler_iter++) {
      std::cout << "\t" 
		<< std::setw(8) << (*handler_iter)->numberOfWrites() << " collections written after" 
		<< std::setw(8) << (*handler_iter)->changes() << " changes of "
		<< (*handler_iter)->name() << " : " 
		<< std::endl;
      UTIL::LCTime first((*handler_iter)->validAtMostSince());
      UTIL::LCTime last((*handler_iter)->validAtMostTill());

      //print validity interval only if data were really written to db 
      if((*handler_iter)->changes() > 0 )  std::cout << std::setw(8) << "" << "Validity interval:" <<  first.getDateString() << " - " << last.getDateString() << std::endl;
    }
    std::cout << std::endl;
    

  }
 
}

#endif
