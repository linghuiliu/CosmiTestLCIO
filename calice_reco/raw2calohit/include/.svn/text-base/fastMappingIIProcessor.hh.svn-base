#ifndef fastMappingIIProcessor_h 
#define fastMappingIIProcessor_h 1

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

namespace CALICE { 

  class ErrorMissingConditionsDataHandler : public std::runtime_error
  {
  public:
    ErrorMissingConditionsDataHandler(const std::string &message) :std::runtime_error(message) {};
  };

/** A class which converts CaliceHits with all calibrations applied into CalorimeterHits
  * @author S.Schmidt DESY
  * @date July 31 2006
  */

class fastMappingIIProcessor : public marlin::Processor {

  public: 
    
    virtual Processor* newProcessor() { return new fastMappingIIProcessor; }
    
    fastMappingIIProcessor();
    
    virtual void init();
    
    virtual void processRunHeader(LCRunHeader* run);
    
    virtual void processEvent(LCEvent* evt);
    
    virtual void check(LCEvent* evt);
    
    virtual void end();
    
  protected:
      
    std::string _inputColName;  
    std::string _outputColName;
    std::string _colNameModuleDescription;
    std::string _colNameModuleLocation;
    std::string _colNameModuleConnection;
    std::string _colNameDetectorTransformation;
    std::string _colNameReferenceTransformation;
    std::string _colNameStageCollection;
    ConditionsChangeDelegator<fastMappingIIProcessor> _moduleTypeChange;
    ConditionsChangeDelegator<fastMappingIIProcessor> _moduleLocationChange;   
    ConditionsChangeDelegator<fastMappingIIProcessor> _moduleConnectionChange; 
    ConditionsChangeDelegator<fastMappingIIProcessor> _detectorTransformationChange; 
    ConditionsChangeDelegator<fastMappingIIProcessor> _referenceTransformationChange; 
    ConditionsChangeDelegator<fastMappingIIProcessor> _stagePositionChange;
    
    MappingAndAlignment _mapping;
    int _viewConnectionTree;
    std::map<unsigned,unsigned> _inverseModuleMap; 
    
    virtual void updateInverseMap();
    
    virtual void moduleTypeChanged(lcio::LCCollection* col) {
      _mapping.moduleTypeChanged(col);
      updateInverseMap();
    };

    virtual void moduleLocationChanged(lcio::LCCollection* col) {
      _mapping.moduleLocationChanged(col);
      updateInverseMap();
    };

    virtual void moduleConnectionChanged(lcio::LCCollection* col) {
      _mapping.moduleConnectionChanged(col);
      updateInverseMap();
    };
 
    virtual void detectorTransformationChanged(lcio::LCCollection* col) {
      _mapping.detectorTransformation(col);
      updateInverseMap();
    };

    virtual void referenceTransformationChanged(lcio::LCCollection* col) {
      _mapping.referenceTransformation(col);
      updateInverseMap();
    };
 
    virtual void stagePositionChanged(lcio::LCCollection* col) {
      _mapping.stagePositionChanged(col);
      updateInverseMap();
    };

};

}

#endif
