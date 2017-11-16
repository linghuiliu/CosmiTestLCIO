#ifndef _VRawADCValueProcessor_hh_
#define _VRawADCValueProcessor_hh_

#include <marlin/Processor.h>
#include <MappingAndAlignment.hh>
#include <ConditionsChangeDelegator.hh>

namespace CALICE {

  class ErrorMissingConditionsDataHandler : public std::runtime_error
  {
  public:
    ErrorMissingConditionsDataHandler(const std::string &message) :std::runtime_error(message) {};
  };

/** Abstract class which implements the minimal functionality to access raw ADC values.
 */
  class VRawADCValueProcessor : public marlin::Processor
{
protected:
  VRawADCValueProcessor(const std::string &processor_name);
  
  void init();

  MappingAndAlignment _mapping;

  // -- collection name
  std::string _adcColName;           /**< Name of the input collection (INPUT).*/

  // -- conditions data names
  std::string _colNameModuleDescription; /**< Name of the conditions data collection containing module descriptions.*/
  std::string _colNameModuleLocation;    /**< Name of the conditions data collection containing module location.*/
  std::string _colNameModuleConnection;    /**< Name of the conditions data collection containing module location.*/
  std::string _colNameStageCollection;    /**< Name of the conditions data collection containing stage positions.*/

  // -- conditions data handler objects
  ConditionsChangeDelegator<VRawADCValueProcessor> _moduleTypeChange;       /**< helper class to listen for changes of the
									   module types  (conditions data)*/
  ConditionsChangeDelegator<VRawADCValueProcessor> _moduleLocationChange;   /**< helper class to listen for changes of the
									 location of modules  (conditions data)*/
  ConditionsChangeDelegator<VRawADCValueProcessor> _moduleConnectionChange; /**< helper class to listen for changes of the
									 connection between modules and the DAQ front-ends (conditions data)*/
  ConditionsChangeDelegator<VRawADCValueProcessor> _stagePositionChange; /**< helper class to listen for changes of 
									 stage positions (conditions data)*/

  // -- the actual condtions data handler

  virtual void moduleTypeChanged(lcio::LCCollection* col) {
    showCollectionParameters(col);
    //correctModuleDescriptionOrder(col);
    _mapping.moduleTypeChanged(col);
  };

  virtual void moduleLocationChanged(lcio::LCCollection* col) {
    _mapping.moduleLocationChanged(col);
  };

  virtual void stagePositionChanged(lcio::LCCollection* col) {
    _mapping.stagePositionChanged(col);
  };

  virtual void moduleConnectionChanged(lcio::LCCollection* col) {
    _mapping.moduleConnectionChanged(col);
  };

  void showCollectionParameters(LCCollection *colP);
  void correctModuleDescriptionOrder(LCCollection *colP);
 
  unsigned int _wrongOrderTill;
};
}

#endif
