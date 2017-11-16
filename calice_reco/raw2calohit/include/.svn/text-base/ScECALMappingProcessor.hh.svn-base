#ifndef ScECALMappingProcessor_h 
#define ScECALMappingProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif

#define b111129 0

#include <iostream>
#include <fstream>
#include <utility>

#include "lcio.h"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include <MappingAndAlignment.hh>
#include <ConditionsChangeDelegator.hh>

#include "ScECALParameters.hh"

namespace CALICE { 

  class ErrorMissingConditionsDataHandler : public std::runtime_error
  {
  public:
    ErrorMissingConditionsDataHandler(const std::string &message) :std::runtime_error(message) {};
  };

  /** A class which converts ADCColl to CalorimeterHits with mapping change
   * Copied and modified from fastMappingIIProcessor
   * Satoru Uozumi
   * @date Nov 5 2008
   */

  class ScECALMappingProcessor : public marlin::Processor, public lccd::IConditionsChangeListener {

  public: 
    
    virtual Processor* newProcessor() { return new ScECALMappingProcessor; }
    
    ScECALMappingProcessor();
    
    virtual void conditionsChanged(LCCollection *col); //for DB

    virtual void init();
    
    virtual void processRunHeader(LCRunHeader* run);
    
    virtual void processEvent(LCEvent* evt);
    
    virtual void check(LCEvent* evt);
    
    virtual void end();
    
  protected:

//110906 for DB
    std::string _ScECALMappingColName;
    LCCollection *_ScECALMappingCol;
    bool _MappingChanged;

//TODO; temporally for 2008
    std::string _mappingData;          
    std::string _inputColName;  
    std::string _outputColName;
    //std::string _colNameModuleDescription;
    //std::string _colNameModuleLocation;
    //std::string _colNameModuleConnection;
    //std::string _colNameDetectorTransformation;
    //std::string _colNameReferenceTransformation;
    //std::string _colNameStageCollection;
    //ConditionsChangeDelegator<ScECALMappingProcessor> _moduleTypeChange;
    //ConditionsChangeDelegator<ScECALMappingProcessor> _moduleLocationChange;   
    //ConditionsChangeDelegator<ScECALMappingProcessor> _moduleConnectionChange; 
    //ConditionsChangeDelegator<ScECALMappingProcessor> _detectorTransformationChange; 
    //ConditionsChangeDelegator<ScECALMappingProcessor> _referenceTransformationChange; 
    //ConditionsChangeDelegator<ScECALMappingProcessor> _stagePositionChange;
    //
    //MappingAndAlignment _mapping;
    //int _viewConnectionTree;
    //std::map<unsigned,unsigned> _inverseModuleMap; 
    //
    //virtual void updateInverseMap();
    //
    //virtual void moduleTypeChanged(lcio::LCCollection* col) {
    //  _mapping.moduleTypeChanged(col);
    //  updateInverseMap();
    //};
    //
    //virtual void moduleLocationChanged(lcio::LCCollection* col) {
    //  _mapping.moduleLocationChanged(col);
    //  updateInverseMap();
    //};
    //
    //virtual void moduleConnectionChanged(lcio::LCCollection* col) {
    //  _mapping.moduleConnectionChanged(col);
    //  updateInverseMap();
    //};
    //
    //virtual void detectorTransformationChanged(lcio::LCCollection* col) {
    //  _mapping.detectorTransformation(col);
    //  updateInverseMap();
    //};
    //
    //virtual void referenceTransformationChanged(lcio::LCCollection* col) {
    //  _mapping.referenceTransformation(col);
    //  updateInverseMap();
    //};
    //
    //virtual void stagePositionChanged(lcio::LCCollection* col) {
    //  _mapping.stagePositionChanged(col);
    //  updateInverseMap();
    //};

    // indices are slot(5 or 7), FE, chip, chan.
    std::pair<int,int> _ScECALmap[ScECAL_NSLOT][ScECAL_NFE][ScECAL_NCHIP][ScECAL_NCHAN];

    inline std::pair<int,int> getScECALChannelID(int slot, int fe,
						 int chip, int chan)
    {
      if (slot!=5&&slot!=7) throw std::runtime_error("Invalid ScECAL channel.");
      if (fe<0||fe>ScECAL_NFE) throw std::runtime_error("Invalid ScECAL channel.");
      if (chip<0||chip>ScECAL_NCHIP) throw std::runtime_error("Invalid ScECAL channel.");
      if (chan<0||chan>ScECAL_NCHAN) throw std::runtime_error("Invalid ScECAL channel.");

      // FEs not used
      if (slot==5&&fe==7) throw std::runtime_error("Invalid ScECAL channel.");
      if (slot==7&&(fe%1==1)) throw std::runtime_error("Invalid ScECAL channel.");

      return _ScECALmap[(slot==5?0:1)][fe][chip][chan];

    };

    inline ThreeVector_t getChannelPosition(int layer, int strip)
    {
      if (layer<0||layer>ScECAL_NLAYERS) throw std::runtime_error("Invalid ScECAL channel.");
      if (strip<0||strip>ScECAL_NSTRIPS) throw std::runtime_error("Invalid ScECAL channel.");

      
//111129 Boris found bugs and improved;
      layer -=1;
      strip -=1;

      ThreeVector_t ret;
#if b111129
      cout << "In_ScECALMappingProcessor.hh_strip = " << strip << endl;
#endif 

//111129      if (layer%2==1) {
       if (layer%2==0) {
           ret[0] = 10*(0.5+(strip%18)*1-9);//9.5-(strip%18);
           ret[1] = -10*((4.5/2.)+(strip/18)*4.5-2*4.5);//(1.5-(strip/18));
       }
       else {
           ret[0] = 10*((4.5/2.)+(strip/18)*4.5-2*4.5); //4.5*(1.5-(strip/18));
           ret[1] = 10*(0.5+(strip%18)*1-9);//(strip%18)-9.5;
       }
//111129 coterra       ret[2] = 5.04+(layer-1)*8.77+ECAL_START_POSITION;
       ret[2] = 5.04+(layer)*8.77; // +ECAL_START_POSITION;

      return ret;

    };

  };

}

#endif
