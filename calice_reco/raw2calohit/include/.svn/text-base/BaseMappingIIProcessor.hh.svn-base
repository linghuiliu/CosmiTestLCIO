#ifndef BaseMappingIIProcessor_h 
#define BaseMappingIIProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif
 
#include "lcio.h"
#include "EVENT/LCObject.h"
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include "MappingAndAlignment.hh"
#include "ConditionsChangeDelegator.hh"

namespace CALICE { 

class ErrorMissingConditionsDataHandler
  : public std::runtime_error
{
public:
  ErrorMissingConditionsDataHandler(const std::string &message)
    : std::runtime_error(message) {};
};

/*** A base class to handle the common mapping and alignment information
 * @author G.Lima (NIU)
 * @date April 17 2008
 */
class BaseMappingIIProcessor : public marlin::Processor {

public:

//   virtual Processor* newProcessor() { return new BaseMappingIIProcessor; }

  BaseMappingIIProcessor(const std::string& typeName);

  virtual void init();

  virtual void processRunHeader(LCRunHeader* run);

  virtual void processEvent(LCEvent* evt) = 0;

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
  ConditionsChangeDelegator<BaseMappingIIProcessor> _moduleTypeChange;
  ConditionsChangeDelegator<BaseMappingIIProcessor> _moduleLocationChange;
  ConditionsChangeDelegator<BaseMappingIIProcessor> _moduleConnectionChange;
  ConditionsChangeDelegator<BaseMappingIIProcessor> _detectorTransformationChange;
  ConditionsChangeDelegator<BaseMappingIIProcessor> _referenceTransformationChange;

  MappingAndAlignment _mapping;
  bool _viewConnectionTree;
  std::map<unsigned,unsigned> _inverseModuleMap; // should be moved to _mapping?

  virtual void updateInverseMap();

  virtual void moduleTypeChanged(lcio::LCCollection* col) {
    _mapping.moduleTypeChanged(col);
    updateInverseMap();  // unnecessary?
  };

  virtual void moduleLocationChanged(lcio::LCCollection* col) {
    _mapping.moduleLocationChanged(col);
    updateInverseMap();
  };

  virtual void moduleConnectionChanged(lcio::LCCollection* col) {
    _mapping.moduleConnectionChanged(col);
    updateInverseMap();  // unnecessary?
  };
 
  virtual void detectorTransformationChanged(lcio::LCCollection* col) {
    _mapping.detectorTransformation(col);
    updateInverseMap();  // unnecessary?
  };

  virtual void referenceTransformationChanged(lcio::LCCollection* col) {
    _mapping.referenceTransformation(col);
    updateInverseMap();  // unnecessary?
  };

};

} // end namespace CALICE

#endif
