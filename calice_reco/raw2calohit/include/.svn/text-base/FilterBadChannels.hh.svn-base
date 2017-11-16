#ifndef __FilterBadChannels_hh__
#define __FilterBadChannels_hh__

#include "marlin/Processor.h"
#include "EVENT/LCEvent.h"

#include "lccd/ConditionsMap.hh"

#include "CellQuality.hh"

namespace CALICE{

class FilterBadChannels : public marlin::Processor {

 public:

  FilterBadChannels();
  ~FilterBadChannels();

  FilterBadChannels* newProcessor(){ return new FilterBadChannels(); }

  virtual void init();

  virtual void processEvent(LCEvent* evt);

  virtual void end();

 protected:

 private:
  std::string _inColName;
  std::string _outColName;
  std::string _statusColName;

  typedef lccd::ConditionsMap< const int, CellQuality > Map_t;
  Map_t *_statusMap;

}; // class FilterBadChannels

} // namespace CALICE

#endif

