#include <TestCondDataProcessor.hh>
#include <EVENT/LCGenericObject.h>
#include <EVENT/LCParameters.h>
#include <marlin/ConditionsProcessor.h>
#include <collection_names.hh>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <climits>
#include <lccd/IConditionsHandler.hh>
#include <lccd/LCConditionsMgr.hh>
#include <UTIL/LCTime.h>

TestCondDataProcessor a_TestCondDataProcessor_instance;

void print(const std::string &a_name) 
{
  std::cout << a_name << std::endl;
}

TestCondDataProcessor::TestCondDataProcessor()
  : marlin::Processor("TestCondDataProcessor"),
    _dbCondDataChange(this, &TestCondDataProcessor::dbCondDataChanged ),
    _conversionCondDataChange(this, &TestCondDataProcessor::conversionCondDataChanged )
{
  _colNameFromDB=COL_TRIGGER_ASSIGNMENT;
  _colNameFromDB+="_conddb";
  registerProcessorParameter( "CondDataColNameFromDB" , 
			      "The conditions data collection coming from the database or flat file (ideally, the trigger configuration data)." ,
			      _colNameFromDB ,
			      _colNameFromDB );

  _colNameFromConversion=COL_TRIGGER_ASSIGNMENT;
  registerProcessorParameter( "CondDataColNameFromConversion" , 
			      "The name of the conditions data produced by the converter (ideally, the trigger configuration data" ,
			      _colNameFromConversion ,
			      _colNameFromConversion );
}

void TestCondDataProcessor::init() 
{
  printParameters();

  marlin::ConditionsProcessor::registerChangeListener( &_dbCondDataChange,
							_colNameFromDB );

  marlin::ConditionsProcessor::registerChangeListener( &_conversionCondDataChange ,
							_colNameFromConversion );

  _dbCondDataHandler = lccd::LCConditionsMgr::instance()->getHandler( _colNameFromDB );
  _conversionCondDataHandler = lccd::LCConditionsMgr::instance()->getHandler( _colNameFromConversion );

  _dbSerialID=0;
  _dbThisId=0;
  _dbSince=lccd::LCCDMinusInf;
  _dbTill=lccd::LCCDMinusInf;
  _conversionThisId=0;
  _conversionSerialID=0;
  _realConversionSerialID=0;

  _conversionSince=lccd::LCCDMinusInf;
  _conversionTill=lccd::LCCDMinusInf;
  _nComparisons=0;

  _lastEventNumber=UINT_MAX;
  _eventOfLastConversionChange=UINT_MAX-1;
  _conversionCondDataCol=0;
  _dbCondDataCol=0;
}

void TestCondDataProcessor::dbCondDataChanged(EVENT::LCCollection *col)
{
  _dbCondDataCol=col;
  _dbSerialID++;

  _dbOlderId = _dbOldId;
  _dbOldId = _dbThisId;
  _dbThisId = _dbSerialID;
  _dbOlderSince=_dbOldSince;
  _dbOlderTill=_dbOldTill;
  _dbOldSince=_dbSince;
  _dbOldTill=_dbTill;
  _dbSince=_dbCondDataHandler->validSince();
  _dbTill=_dbCondDataHandler->validTill();

  compareCondData();
  _dbCondDataColStat.add(col->getNumberOfElements());

  EVENT::StringVec keys;
  UInt_t keys_total=0;

  col->getParameters().getIntKeys(keys);
  keys_total +=  keys.size();

  col->getParameters().getFloatKeys(keys);
  keys_total +=  keys.size();

  col->getParameters().getStringKeys(keys);
  keys_total +=  keys.size();

  _dbCondDataParStat.add( keys_total );

}

void TestCondDataProcessor::conversionCondDataChanged(EVENT::LCCollection *col)
{
  _conversionCondDataCol=col;
  _realConversionSerialID++;

  bool skipped=true;
  if ( _eventOfLastConversionChange != _lastEventNumber ) {
    skipped=false;
    _conversionSerialID++;
  }
  _eventOfLastConversionChange = _lastEventNumber;

  _conversionOlderId = _conversionOldId;
  _conversionOldId = _conversionThisId;
  _conversionThisId = _conversionSerialID;
  _conversionOlderSince=_conversionOldSince;
  _conversionOlderTill=_conversionOldTill;
  _conversionOldSince=_conversionSince;
  _conversionOldTill=_conversionTill;

  _conversionSince=_conversionCondDataHandler->validSince();
  _conversionTill=_conversionCondDataHandler->validTill();

  if (!skipped) {
    compareCondData();
  }
  _conversionCondDataColStat.add(col->getNumberOfElements());

  EVENT::StringVec keys;
  UInt_t keys_total=0;

  col->getParameters().getIntKeys(keys);
  keys_total +=  keys.size();

  col->getParameters().getFloatKeys(keys);
  keys_total +=  keys.size();

  col->getParameters().getStringKeys(keys);
  keys_total +=  keys.size();

  _conversionCondDataParStat.add( keys_total );
  
}

void TestCondDataProcessor::compareCondData()
{
  if (_conversionSerialID == _dbSerialID) {
    _nComparisons++;
    if (!_conversionCondDataCol || !_dbCondDataCol) {
      std::stringstream message;
      message << "TestCondDataProcessor::compareCondData> ";
      if (_conversionCondDataCol) {
	message << "No collection from the converter.";
      }
      if (_dbCondDataCol) {
	message << "No collection from the database.";
      }
      throw std::runtime_error(message.str());
    }

    if (_conversionCondDataCol->getNumberOfElements() != _dbCondDataCol->getNumberOfElements()) {
      std::stringstream message;
      message << "TestCondDataProcessor::compareCondData> "
	      << "Collections contain different number of elements: "
	      << _colNameFromConversion << " != " << _colNameFromDB << " -> " 
	      << _conversionCondDataCol->getNumberOfElements() << " !=  " << _dbCondDataCol->getNumberOfElements();
      throw std::runtime_error(message.str());
    }

    for (unsigned int  element_i=0; element_i< static_cast<unsigned int>(_conversionCondDataCol->getNumberOfElements()); element_i++) {

      LCGenericObject *_genericObjDb=dynamic_cast<LCGenericObject *>(_dbCondDataCol->getElementAt(element_i));
      LCGenericObject *_genericObjConversion=dynamic_cast<LCGenericObject *>(_conversionCondDataCol->getElementAt(element_i));

      if (_genericObjConversion->getNInt() != _genericObjDb->getNInt() ) {
	std::stringstream message;
	message << "TestCondDataProcessor::compareCondData> "
		<< "Number of integer differs for element " << element_i << ":"
		<< _colNameFromConversion << " != " << _colNameFromDB << " -> " 
		<< _genericObjConversion->getNInt() << " != "  << _genericObjDb->getNInt();

	throw std::runtime_error(message.str());
      }

      for (unsigned int val_i=0; val_i < static_cast<unsigned int>(_genericObjConversion->getNInt()); val_i++) {
	if (_genericObjConversion->getIntVal(val_i) != _genericObjDb->getIntVal(val_i)) {
	  std::stringstream message;
	  message << "TestCondDataProcessor::compareCondData> "
		  << "Integers " << val_i << " of collection element " << element_i << " do not match :"
		  << _genericObjConversion->getIntVal(val_i) << " != "  << _genericObjDb->getIntVal(val_i);
	  throw std::runtime_error(message.str());
	}
      }

      for (unsigned int val_i=0; val_i < static_cast<unsigned int>(_genericObjConversion->getNFloat()); val_i++) {
	if (_genericObjConversion->getFloatVal(val_i) != _genericObjDb->getFloatVal(val_i)) {
	  std::stringstream message;
	  message << "TestCondDataProcessor::compareCondData> "
		  << "Float values " << val_i << " of collection element " << element_i << " do not match :"
		  << _genericObjConversion->getFloatVal(val_i) << " != "  << _genericObjDb->getFloatVal(val_i);
	  throw std::runtime_error(message.str());
	}
      }

      for (unsigned int val_i=0; val_i < static_cast<unsigned int>(_genericObjConversion->getNDouble()); val_i++) {
	if (_genericObjConversion->getDoubleVal(val_i) != _genericObjDb->getDoubleVal(val_i)) {
	  std::stringstream message;
	  message << "TestCondDataProcessor::compareCondData> "
		  << "Double values " << val_i << " of collection element " << element_i << " do not match :"
		  << _genericObjConversion->getDoubleVal(val_i) << " != "  << _genericObjDb->getDoubleVal(val_i);
	  throw std::runtime_error(message.str());
	}
      }
    }

    // compare parameters
    EVENT::StringVec conversion_keys;
    EVENT::StringVec db_keys;
    _conversionCondDataCol->getParameters().getIntKeys(conversion_keys);
    _dbCondDataCol->getParameters().getIntKeys(db_keys);
    
    EVENT::StringVec::iterator a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBLayer"));
    db_keys.erase(a_iter);

    if ( conversion_keys.size() != db_keys.size() ) {
      std::cout << "conversion keys:" << std::endl;
      for_each ( conversion_keys.begin(), conversion_keys.end(), print );

      std::cout << "db keys:" << std::endl;
      for_each ( db_keys.begin(), db_keys.end(), print );
      std::cout << std::endl;

      std::stringstream message;
      message << "TestCondDataProcessor::compareCondData> "
	      << "Number of int keys does not match "
	      << _colNameFromConversion << " != " << _colNameFromDB << " -> " 
	      << conversion_keys.size() << " !=  " << db_keys.size();
      throw std::runtime_error(message.str());
    }

    _conversionCondDataCol->getParameters().getFloatKeys(conversion_keys);
    _dbCondDataCol->getParameters().getFloatKeys(db_keys);
    if ( conversion_keys.size() != db_keys.size() ) {
      std::cout << "conversion keys:" << std::endl;
      for_each ( conversion_keys.begin(), conversion_keys.end(), print );

      std::cout << "db keys:" << std::endl;
      for_each ( db_keys.begin(), db_keys.end(), print );
      std::cout << std::endl;
      std::stringstream message;
      message << "TestCondDataProcessor::compareCondData> "
	      << "Number of float keys does not match "
	      << _colNameFromConversion << " != " << _colNameFromDB << " -> " 
	      << conversion_keys.size() << " !=  " << db_keys.size();
      throw std::runtime_error(message.str());
    }

    _conversionCondDataCol->getParameters().getStringKeys(conversion_keys);
    _dbCondDataCol->getParameters().getStringKeys(db_keys);

    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBLayer"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBFolder"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBInsertionTime"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBName"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBQueryTime"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBSince"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBTag"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DBTill"));;
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("DataDescription"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);
    a_iter=find(db_keys.begin(),db_keys.end(),std::string("TypeName"));
    if (a_iter!=db_keys.end()) db_keys.erase(a_iter);

    if ( conversion_keys.size() != db_keys.size() ) {
      std::cout << "conversion keys:" << std::endl;
      for_each ( conversion_keys.begin(), conversion_keys.end(), print );

      std::cout << "db keys:" << std::endl;
      for_each ( db_keys.begin(), db_keys.end(), print );
      std::cout << std::endl;
      std::stringstream message;
      message << "TestCondDataProcessor::compareCondData> "
	      << "Number of string keys does not match "
	      << _colNameFromConversion << " != " << _colNameFromDB << " -> " 
	      << conversion_keys.size() << " !=  " << db_keys.size();
      throw std::runtime_error(message.str());
    }

  }
}

void TestCondDataProcessor::processEvent( LCEvent * evtP ) 
{
  _lastEventNumber=static_cast<unsigned int>( evtP->getEventNumber() );
  verifyCondDataValidity( evtP );
}

void TestCondDataProcessor::verifyCondDataValidity( LCEvent *evtP )
{
  if (!_conversionCondDataCol || !_dbCondDataCol) {
    eventInfo(evtP);
    std::stringstream message;
    message << "TestCondDataProcessor::compareCondData> ";
    if (_conversionCondDataCol) {
      message << "No collection from the converter.";
    }
    if (_dbCondDataCol) {
      message << "No collection from the database.";
    }
    throw std::runtime_error(message.str());
  }

  if ( _dbSerialID != _conversionSerialID) {
    eventInfo(evtP);
    std::stringstream message;
    message << "TestCondDataProcessor::compareCondData> conditions change counter do not match : "
	    << _dbSerialID << " != " << _conversionSerialID;

    throw std::runtime_error(message.str());
  }

  if ( _dbSerialID != _nComparisons ) {
    eventInfo(evtP);
    std::stringstream message;
    message << "TestCondDataProcessor::compareCondData> Not every conditions data change could be compared : "
	    << _dbSerialID << " != " << _nComparisons;

    throw std::runtime_error(message.str());
  }

}

void TestCondDataProcessor::eventInfo( LCEvent *evt)
{
  UTIL::LCTime event_time( evt->getTimeStamp() );
  UTIL::LCTime conversion_since( _conversionCondDataHandler->validSince() );
  UTIL::LCTime conversion_till( _conversionCondDataHandler->validTill() );
  UTIL::LCTime db_since( _dbCondDataHandler->validSince() );
  UTIL::LCTime db_till( _dbCondDataHandler->validTill() );

  UTIL::LCTime db_lastSince( _dbSince );
  UTIL::LCTime db_lastTill( _dbTill );
  UTIL::LCTime db_priorSince( _dbOldSince );
  UTIL::LCTime db_priorTill( _dbOldTill );
  UTIL::LCTime db_agedSince( _dbOlderSince );
  UTIL::LCTime db_agedTill( _dbOlderTill );

  UTIL::LCTime conversion_lastSince( _conversionSince );
  UTIL::LCTime conversion_lastTill( _conversionTill );
  UTIL::LCTime conversion_priorSince( _conversionOldSince );
  UTIL::LCTime conversion_priorTill( _conversionOldTill );
  UTIL::LCTime conversion_agedSince( _conversionOlderSince );
  UTIL::LCTime conversion_agedTill( _conversionOlderTill );

  std::cout << "TestCondDataProcessor::eventInfo> run=" << evt->getRunNumber() << " event=" << evt->getEventNumber() 
	    << " " << event_time.getDateString() << std::endl
	    << std::setw(20) << "Conversion:" << " serial nr=" << std::setw(6) << _conversionSerialID 
	    << "(" << _realConversionSerialID - _conversionSerialID << " changes skipped)" << std::endl
	    << std::setw(20) << "corrent:    " << conversion_since.getDateString()      << " - " << conversion_till.getDateString()  << std::endl
	    << std::setw(20) << "last change:" << conversion_lastSince.getDateString()  << " - " << conversion_lastTill.getDateString() 
	    << " id=" << _conversionThisId<< std::endl
	    << std::setw(20) << "prior last :" << conversion_priorSince.getDateString() << " - " << conversion_priorTill.getDateString()
	    << " id=" << _conversionOldId<< std::endl
	    << std::setw(20) << "prior prior:" << conversion_agedSince.getDateString() << " - " << conversion_agedTill.getDateString()
	    << " id=" << _conversionOlderId<< std::endl
    	    << std::setw(20) <<  " DB: " << "serial nr=" << std::setw(6) << _dbSerialID << std::endl
	    << std::setw(20) << "corrent:    " << db_since.getDateString()      << " - " << db_till.getDateString() << std::endl
	    << std::setw(20) << "last change:" << db_lastSince.getDateString()  << " - " << db_lastTill.getDateString()
	    << " id=" << _dbThisId<< std::endl
	    << std::setw(20) << "prior last :" << db_priorSince.getDateString() << " - " << db_priorTill.getDateString()
	    << " id=" << _dbOldId<< std::endl
	    << std::setw(20) << "prior prior:" << db_agedSince.getDateString() << " - " << db_agedTill.getDateString()
	    << " id=" << _dbOlderId<< std::endl
	    << std::endl;
}


void TestCondDataProcessor::end()
{
  std::cout << "--- " << name() << " Report :" << std::endl
	    << std::setw(8) << _nComparisons              << " comparisons of the conditions data." << std::endl
	    << std::setw(8) << _dbSerialID                << " conditions data changes from DB (" << _colNameFromDB << ")." << std::endl
	    << std::setw(8) << _conversionSerialID        << " conditions data changes from Conversion (" << _colNameFromConversion << ")." << std::endl
	    << std::setw(8) << _realConversionSerialID -_conversionSerialID << " conditions data changes from Conversion skipped " <<  std::endl
	    << std::setw(8) << " DB         cond data collection stat: " << _dbCondDataColStat << std::endl
	    << std::setw(8) << " Conversion cond data collection stat: " << _conversionCondDataColStat << std::endl
	    << std::setw(8) << " DB         cond data parameter  stat: " << _dbCondDataParStat << std::endl
	    << std::setw(8) << " Conversion cond data parameter  stat: " << _conversionCondDataParStat << std::endl
	    << std::endl;

}
