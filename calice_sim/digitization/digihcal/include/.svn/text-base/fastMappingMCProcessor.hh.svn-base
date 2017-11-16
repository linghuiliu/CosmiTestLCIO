#ifndef fastMappingMCProcessor_h 
#define fastMappingMCProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif
 
#include "lcio.h"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include <MappingAndAlignment.hh>
#include <ConditionsChangeDelegator.hh>
#include <HcalModuleIndexReverseLookup.hh>

namespace CALICE { 

  class ErrorMissingConditionsDataHandler : public std::runtime_error
  {
  public:
    ErrorMissingConditionsDataHandler(const std::string &message) :std::runtime_error(message) {};
  };

/** A class which converts MC CalorimeterHits to CaliceHits according to the hardware conditions.
  * @author S.Schmidt DESY
  * @date April 2 2007
  */

class fastMappingMCProcessor : public marlin::Processor {

  public: 
    
    virtual Processor* newProcessor() { return new fastMappingMCProcessor; }
    
    fastMappingMCProcessor();
    
    virtual void init();
    
    virtual void processEvent(LCEvent* evt);

  protected:

    std::string _inputColName;  
    std::string _outputColName;
    std::string _colNameModuleDescription;
    std::string _colNameModuleLocation;
    std::string _colNameModuleConnection;
    ConditionsChangeDelegator<fastMappingMCProcessor> _moduleTypeChange;
    ConditionsChangeDelegator<fastMappingMCProcessor> _moduleLocationChange;   
    ConditionsChangeDelegator<fastMappingMCProcessor> _moduleConnectionChange; 
    
    MappingAndAlignment _mapping;
    int _viewConnectionTree;
    HcalModuleIndexReverseLookup _indexLookup;
    
    virtual void moduleTypeChanged(lcio::LCCollection* col);
    
    virtual void moduleLocationChanged(lcio::LCCollection* col);
    
    virtual void moduleConnectionChanged(lcio::LCCollection* col);
    
};

}

#endif
