#ifndef AppendProcessor_h
#define AppendProcessor_h 1

#include "marlin/Processor.h"
#include "lcio.h"

namespace CALICE {

/**  
 * @brief Processor which allows to append events from additional LCIO files.
 * 
 * @authors Sebastian Schmidt, Joergen Samson, Guilherme Lima, Sebastian Richter
 * @version 1.0
 */

class AppendProcessor : public marlin::Processor {
  
 public:
  
  virtual Processor* newProcessor() { return new AppendProcessor; }
  
  AppendProcessor();
  
  virtual void init();
  
  virtual void processEvent(lcio::LCEvent *evt); 
  
 protected:

  lcio::StringVec _appendFileNames;
  lcio::StringVec _appendCollectionInputNames;
  lcio::StringVec _appendCollectionOutputNames;
  lcio::IntVec _appendCollectionFixLevels;

  lcio::LCReader* _lcReader;

  lcio::IntVec _useTcmtHits;
  enum HitsType_t {TCMTHIT, FASTCALICEHIT};
  HitsType_t _hitsType;

  bool _appendRepeatCollections;
};

}

#endif
