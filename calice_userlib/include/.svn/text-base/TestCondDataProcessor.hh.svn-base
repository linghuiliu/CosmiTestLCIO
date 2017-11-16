#ifndef _TestCondDataProcessor_hh_
#define _TestCondDataProcessor_hh_

#include <ConditionsChangeDelegator.hh>
#include <marlin/Processor.h>
#include <ConditionsChangeDelegator.hh>
#include <Average_t.hh>

using namespace CALICE;

namespace lccd {
  class IConditionsHandler;
}

/** Test processor to verify that the conditions data from the database and the converter matches.
 */
class TestCondDataProcessor : public marlin::Processor
{
public:
  TestCondDataProcessor();
  ~TestCondDataProcessor() {};

  marlin::Processor *newProcessor() {return new TestCondDataProcessor;};
  void init() ;

  void processEvent( LCEvent * evtP );

  void eventInfo( LCEvent *evtP );

  void end() ;


protected:
  virtual void dbCondDataChanged(EVENT::LCCollection *col);
  virtual void conversionCondDataChanged(EVENT::LCCollection *col);

  void compareCondData();

  void verifyCondDataValidity( LCEvent * evtP );
private:
  ConditionsChangeDelegator<TestCondDataProcessor> _dbCondDataChange;
  lccd::IConditionsHandler *_dbCondDataHandler;
  ConditionsChangeDelegator<TestCondDataProcessor> _conversionCondDataChange;
  lccd::IConditionsHandler *_conversionCondDataHandler;

  std::string _colNameFromDB;
  std::string _colNameFromConversion;

  EVENT::LCCollection *_conversionCondDataCol;
  unsigned int _conversionSerialID;
  unsigned int _realConversionSerialID;

  EVENT::LCCollection *_dbCondDataCol;
  unsigned int _dbSerialID;

  unsigned int _nComparisons;

  Average_t  _dbCondDataColStat;
  Average_t  _conversionCondDataColStat;

  Average_t  _dbCondDataParStat;
  Average_t  _conversionCondDataParStat;

  unsigned int _dbThisId;
  unsigned int _dbOldId;
  unsigned int _dbOlderId;
  long64     _dbSince;
  long64     _dbTill;
  long64     _dbOldSince;
  long64     _dbOldTill;
  long64     _dbOlderSince;
  long64     _dbOlderTill;

  unsigned int _conversionThisId;
  unsigned int _conversionOldId;
  unsigned int _conversionOlderId;
  long64     _conversionSince;
  long64     _conversionTill;
  long64     _conversionOldSince;
  long64     _conversionOldTill;
  long64     _conversionOlderSince;
  long64     _conversionOlderTill;
  
  unsigned int _eventOfLastConversionChange;
  unsigned int _lastEventNumber;

  std::vector<std::string> _removeIntKeys;
  std::vector<std::string> _removeStringKeys;
  
};
#endif
