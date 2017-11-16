#ifndef FastCalibProcessor_h
#define FastCalibProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <map>

#include <lcio.h>
#include <lccd.h>


#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <LCHcalCalibrationObject.hh>
#include <ConditionsChangeDelegator.hh>
#include <FastCaliceHit.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <marlin/ConditionsProcessor.h>
#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>
#include <VRawADCValueProcessor.hh>
#include <collection_names.hh>
#include <TempModel.hh>
#include <CalibrationSet.hh>

//#define HCALRECO_DEBUG
//#define HCALRECO_TEMP_DEBUG

namespace CALICE {
  
template <class T, const char *str, class M = TempModel>
class FastCalib2DProcessor;

/** Class which handles the application of a certain type T:LCHcalCalibrationObject to an 
  * input collection of CaliceHit and writes an output collection of CaliceHit
  **/
  
template <class T, const char *str, class M >
class FastCalib2DProcessor : public marlin::Processor {

  public: 
    typedef std::map<int,int> parameterMap;

    virtual Processor* newProcessor() {return new FastCalib2DProcessor<T,str,M>();}
  
    FastCalib2DProcessor(): marlin::Processor(str),
    _calibrationChange(this,&FastCalib2DProcessor<T,str,M>::calibrationChanged),
    _ahcSroModDataChange(this, &FastCalib2DProcessor<T,str,M>::ahcSroModDataColChanged),
    _ahcSroModDataCol(0)
    {
       _description = "This processor makes a calibration constant available which is stored module wise";
       registerProcessorParameter("InputCollectionName", "Name of the input collection",
                           _inputColName, std::string("CaliceHitsLevel1"));
			    
       registerProcessorParameter("ParameterCollectionName", "Name of the parameter collection",
                           _parameterColName, std::string("CaliceHitsParameter1"));
			    
       registerProcessorParameter("OutputCollectionName", "Name of the output collection",
                           _outputColName, std::string("CaliceHitsLevel2"));
			   
       registerProcessorParameter("CalibrationCollectionTemplate", "Template for the name of the module wise collections with the calibration constants",
                           _calColName, std::string("Calibration_"));
	   
       registerProcessorParameter("AHC_SRO_ModData","Name of the conditions collection for the AHC Slow r/o data.",
                           _ahcSroModDataColName, std::string(COL_AHC_SRO_MOD));
       
       IntVec modulesExample;
       modulesExample.push_back(0);
  
       registerOptionalParameter("Module", "Add calibration for module", _modules, modulesExample, modulesExample.size());	   
       registerProcessorParameter("ZeroSuppression", 
    			    "apply (1) or ignore (0) calibration induced cut",
			    _zeroSuppression,
    			    1);
       _tempModel = new M();
       _calibSet = new CalibrationSet<T>(_tempModel);
    };
    
    
    virtual ~FastCalib2DProcessor() {
      delete _calibSet;
      delete _tempModel;
    };
    
    
    virtual void init() {
      printParameters();
      std::stringstream message;
      bool error=false;
      if (parameterSet("Module")) {
        unsigned index = 0 ;
        message << "Missing condition data: "; 
        while (index < _modules.size() ){

          char _mod[10];
          sprintf(_mod,"%.2d",_modules[index++]); 	
          std::string _module(_mod);
          std::string _finalCalColName = _calColName + _module;
	
          if (!marlin::ConditionsProcessor::registerChangeListener (&_calibrationChange ,_finalCalColName)) {
            message << " " << _finalCalColName;
            error=true;
          } else {
#ifdef HCALRECO_DEBUG
        std::cout << "FastCalib2DProcessor: register conditions data handler for collection " << _finalCalColName << std::endl; 
#endif	      
	  }
        }
       if (error) {
          message <<  ".";
          throw ErrorMissingConditionsDataHandler(message.str());
        }
      }
      _overwrite = (_inputColName == _outputColName);
      if (_overwrite)
        std::cout << "Replacing collection " << _outputColName << std::endl;
      else
        std::cout << "Appending collection " << _outputColName << std::endl; 
    };


    void calibrationChanged(lcio::LCCollection* col) {
      _calibSet->fill(col);
      _calibSet->setTemp(_ahcSroModDataCol);
    };


    void ahcSroModDataColChanged(lcio::LCCollection* col) {
      _ahcSroModDataCol = col;
      _calibSet->setTemp(_ahcSroModDataCol);
    };


    virtual void processEvent(LCEvent *evt) {
      try {
        lcio::LCCollection* inVector = evt->getCollection(_inputColName);
	lcio::LCCollection* parameterVector = evt->getCollection(_parameterColName);
	lcio::LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::RAWCALORIMETERHIT);
        lcio::LCFlagImpl hitFlag(_outputCol->getFlag());
        hitFlag.setBit(LCIO::RCHBIT_TIME);
        hitFlag.setBit(LCIO::CHBIT_ID1);
        _outputCol->setFlag(hitFlag.getFlag());
	parameterMap _parameterMap;
	for (int j = 0; j < parameterVector->getNumberOfElements(); j++) {
	  FastCaliceHit* parameterHit = dynamic_cast<FastCaliceHit*>(parameterVector->getElementAt(j));
	  _parameterMap[parameterHit->getModuleID()<<16+parameterHit->getCellKey()] = j;
	}
#ifdef HCALRECO_DEBUG
        std::cout << "FastCalib2DProcessor<T,str,M>::processEvent() # of oldhits: " << inVector->getNumberOfElements() << std::endl;
#endif	
        for (int i = 0; i < inVector->getNumberOfElements(); i++) {
          FastCaliceHit* oldHit = dynamic_cast<FastCaliceHit*>(inVector->getElementAt(i));
#ifdef HCALRECO_DEBUG
          std::cout << "FastCalib2DProcessor<T,str,M>::processEvent() oldHit: " << _inputColName << " ";
          oldHit->print(std::cout);
#endif	  
	  T* aCalibration = _calibSet->getCalib(oldHit->getModuleID(), oldHit->getCellKey());
	  if (aCalibration) {
	    parameterMap::const_iterator mapIter=_parameterMap.find(oldHit->getModuleID()<<16+oldHit->getCellKey());
	    FastCaliceHit* aFastCaliceHit;
	    if ((mapIter!=_parameterMap.end()) && aCalibration->calibrationValid()) {
  	      FastCaliceHit* parameterHit = dynamic_cast<FastCaliceHit*>(parameterVector->getElementAt(mapIter->second));
	      float _newValue = aCalibration->applyCalibration(oldHit->getEnergyValue(), parameterHit->getEnergyValue());
	      float _newError = aCalibration->applyCalibrationError(oldHit->getEnergyValue(), oldHit->getEnergyError(), 
		  		                                    parameterHit->getEnergyValue(), parameterHit->getEnergyError());
	      if (!_overwrite) { 
                aFastCaliceHit = new FastCaliceHit(oldHit->getModuleID(), oldHit->getChip(), oldHit->getChannel(), 
                                           _newValue, _newError, oldHit->getTimeStamp());
                if ((_zeroSuppression!=1) ||
	             aCalibration->keepEvent(_newValue, _newError)) { 
#ifdef HCALRECO_DEBUG
                  std::cout << "FastCalib2DProcessor<T,str,M>::processEvent() newHit:";
                  aFastCaliceHit->print(std::cout);
#endif
	          if (aFastCaliceHit) _outputCol->addElement(aFastCaliceHit);
	        } else
	            delete aFastCaliceHit;
	      } else {
	        if ((_zeroSuppression!=1) ||
	             aCalibration->keepEvent(_newValue, _newError)) {
  	          oldHit->setEnergyValue(_newValue);
	          oldHit->setEnergyError(_newError);
	        } else {
	          inVector->removeElementAt(i--);
		  delete oldHit;
		  }
	        }
	    } else {
	        if (_overwrite) { inVector->removeElementAt(i--); delete oldHit; }
#ifdef HCALRECO_DEBUG
                std::cout << "FastCalib2DProcessor<T,str,M>::processEvent(): calibration not valid" << std::endl;;
#endif			     
	      }
	  } else {
	      if (_overwrite) { inVector->removeElementAt(i--); delete oldHit; }
#ifdef HCALRECO_DEBUG
              std::cout << "FastCalib2DProcessor<T,str,M>::processEvent(): no calibration" << std::endl;
#endif			     
	    } 
        }
        if (!_overwrite) evt->addCollection(_outputCol, _outputColName);
      }
      catch (DataNotAvailableException &e) {
#ifdef HCALRECO_DEBUG
      std::cout << "FastCalib2DProcessor<T,str,M>::processEvent(): data not available exception" << std::endl;
#endif
      };
    };

  protected:
    std::string _inputColName;
    std::string _parameterColName;
    std::string _outputColName;
    std::string _calColName;
    std::string _ahcSroModDataColName;
    IntVec _modules;
    bool _overwrite;
    int _zeroSuppression;
    CalibrationSet<T>* _calibSet;
    M* _tempModel;
 
  private:
    ConditionsChangeDelegator<FastCalib2DProcessor<T,str,M> > _calibrationChange;
    ConditionsChangeDelegator<FastCalib2DProcessor<T, str, M> > _ahcSroModDataChange;
    lcio::LCCollection* _ahcSroModDataCol;

};

} 

#endif
