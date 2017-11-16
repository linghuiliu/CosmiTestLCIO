#ifndef TcmtMappingIIProcessor_h 
#define TcmtMappingIIProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif

#include <lcio.h>
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <MappingAndAlignment.hh>
#include <ConditionsChangeDelegator.hh>
#include "BaseMappingIIProcessor.hh"

namespace CALICE 
{ 
  
  /*** A class which converts calibrated TcmtHits into CalorimeterHits
   * @author G.Lima (NIU)
   * @date August 01 2007
   *
   * 080417 - G.Lima - Use a base class to handle alignment and mapping stuff.
   */
  class TcmtMappingIIProcessor : public BaseMappingIIProcessor {
    
  public:
    
    Processor* newProcessor() { return new TcmtMappingIIProcessor; }
    
    TcmtMappingIIProcessor();
    
    void init();
    
    void processRunHeader(LCRunHeader* run){};
    
    void processEvent(LCEvent* evt);
    
    void check(LCEvent* evt){};
    
    void end(){};
    
  private:
    
    float _energyThreshold;
    int _outputCollectionType;
  };
  
} // end namespace CALICE

#endif
