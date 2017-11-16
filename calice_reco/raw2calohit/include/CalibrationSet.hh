#ifndef CalibrationSet_h
#define CalibrationSet_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <map>

#include <lcio.h>
#include <EVENT/LCCollection.h>
#include <TempModel.hh>

//#define HCALRECO_DEBUG

namespace CALICE {

template <class T>
class CalibrationSet;

/** Class which handles the storage of a set of LCHcalCalibrationObject 
  * @author S.Schmidt, DESY
  * @date Jun 15 2007 
  **/
  
template <class T>
class CalibrationSet {

public:
  typedef std::map<unsigned,T*> CalibModuleMap;
  typedef std::pair<lcio::LCCollection*, CalibModuleMap*> CalibModuleData;
  typedef std::map<unsigned,CalibModuleData> CalibReadMap;
  
  CalibReadMap _readMap;
  TempModel* _tempModel;

public:

  CalibrationSet(): _tempModel(0), _scalingFactor( 1 ) {
};

  CalibrationSet(TempModel* tempModel): _tempModel(tempModel), _scalingFactor( 1 ) {
};


virtual ~CalibrationSet() {};


bool empty() {
  return _readMap.empty();
};


void fill(lcio::LCCollection* calCol)
{
  if (!calCol) return;
  unsigned _moduleID = calCol->getParameters().getIntVal("ModuleID");
#ifdef HCALRECO_DEBUG
  std::cout << "CalibrationSet<T>::fill(): adding ModuleID " << std::hex << _moduleID << std::dec << std::endl;
#endif	      
  typename CalibReadMap::iterator _mapIter = _readMap.find(_moduleID);
  if (_mapIter != _readMap.end()) {
    CalibModuleData _oldModuleData = _mapIter->second;
    CalibModuleMap* _oldModuleMap = _oldModuleData.second;
    delete _oldModuleMap;
  }
  CalibModuleMap* _moduleMap = new CalibModuleMap();
#ifdef HCALRECO_DEBUG
  std::cout << " adding CellKey ";
#endif	      
  for (int i=0; i< calCol->getNumberOfElements(); i++ ){	
    T* _obj = new T(calCol->getElementAt(i)); 
    _obj->setConstant( _scalingFactor * _obj->getConstant() );
    _obj->setConstantError( _scalingFactor * _obj->getConstantError() );
   _moduleMap->insert(std::make_pair(_obj->getCellKey(), _obj));
#ifdef HCALRECO_DEBUG
    std::cout << std::hex << _obj->getCellKey() << std::dec << " ";
#endif	      
  }
#ifdef HCALRECO_DEBUG
  std::cout << std::endl;
#endif	      
  CalibModuleData _moduleData = std::make_pair(calCol, _moduleMap);
  _readMap.insert(std::make_pair(_moduleID, _moduleData));
}; 


void fill(lcio::LCCollection* calCol, lcio::LCCollection* sroCol)
{
  if (!calCol) return;
  unsigned _moduleID = calCol->getParameters().getIntVal("ModuleID");
#ifdef HCALRECO_DEBUG
  std::cout << "CalibrationSet<T>::fill(): adding ModuleID " << std::hex << _moduleID << std::dec << std::endl;
#endif	      
  typename CalibReadMap::iterator _mapIter = _readMap.find(_moduleID);
  if (_mapIter != _readMap.end()) {
    CalibModuleData _oldModuleData = _mapIter->second;
    CalibModuleMap* _oldModuleMap = _oldModuleData.second;
    delete _oldModuleMap;
  }
  CalibModuleMap* _moduleMap = new CalibModuleMap();
#ifdef HCALRECO_DEBUG
  std::cout << " adding CellKey ";
#endif	      
  for (int i=0; i< calCol->getNumberOfElements(); i++ ){	
    T* _obj = new T(calCol->getElementAt(i));
    _obj->setConstant( _scalingFactor * _obj->getConstant() );
    _obj->setConstantError( _scalingFactor * _obj->getConstantError() );
    if (sroCol && _tempModel) {
      float _tempValue = _tempModel->getTemp(sroCol, _moduleID, _obj->getCellKey());
      float _tempError = _tempModel->getTempError(sroCol, _moduleID, _obj->getCellKey());
      _obj->setCellTemp(_tempValue, _tempError);
    }  
    _moduleMap->insert(std::make_pair(_obj->getCellKey(), _obj));
#ifdef HCALRECO_DEBUG
    std::cout << std::hex << _obj->getCellKey() << std::dec << " ";
#endif	      
  }
#ifdef HCALRECO_DEBUG
  std::cout << std::endl;
#endif	      
  CalibModuleData _moduleData = std::make_pair(calCol, _moduleMap);
  _readMap.insert(std::make_pair(_moduleID, _moduleData));
};


T* getCalib(unsigned moduleID, unsigned cellKey)
{
  typename CalibReadMap::const_iterator _mapIter = _readMap.find(moduleID);
  if (_mapIter == _readMap.end()) {
#ifdef HCALRECO_DEBUG
    std::cout << "CalibrationSet<T>::getCalib(): no moduleID " << std::hex << moduleID << std::dec << std::endl;
#endif		     
    return NULL;
  }
  CalibModuleMap* _ModuleMap = _mapIter->second.second;
  typename CalibModuleMap::iterator it = _ModuleMap->find(cellKey);
  if (it == _ModuleMap->end()) {
#ifdef HCALRECO_DEBUG
    std::cout << "CalibrationSet<T>::getCalib(): no cellKey " << std::hex << cellKey << " in module " << moduleID << std::dec << std::endl;
#endif
    return NULL;
  }	
  return it->second;
};

// Simple wraper method to access calibration via module, chip, channel;
// code from FastCaliceHit and HcalTileIndex
T* getCalib(unsigned module, unsigned chip, unsigned channel)
{

  short type = 0;
  if ( chip > 5 ) ++type;        //  lower part
  if ( module > 30 ) type += 2;  //  coarse modules

  int moduleID = ( module << 8 ) | type;
  int cellkey = (chip << 8) | channel;

  return this->getCalib(moduleID, cellkey);

}

void setTemp(lcio::LCCollection* sroCol)
{
  if (!sroCol || !_tempModel) return;
  for (typename CalibReadMap::iterator _mapIter=_readMap.begin(); _mapIter!=_readMap.end(); _mapIter++) {
    CalibModuleMap* _ModuleMap=_mapIter->second.second;
    for (typename CalibModuleMap::iterator it=_ModuleMap->begin(); it!=_ModuleMap->end(); it++) {
      unsigned _moduleID = _mapIter->first;
      unsigned _cellKey = it->second->getCellKey();
      float _tempValue = _tempModel->getTemp(sroCol, _moduleID, _cellKey);
      float _tempError = _tempModel->getTempError(sroCol, _moduleID, _cellKey);
      it->second->setCellTemp(_tempValue, _tempError);
#ifdef HCALRECO_DEBUG
      std::cout << "CalibrationSet<T>::setTemperature(): " << _moduleID << "/ " << 
	std::hex << _cellKey << std::dec << ": " << _tempValue << "+-" << _tempError << std::endl;
      it->second->print(std::cout);   
#endif
    }
  }
};

void applyScalingFactor( float f ){
  for ( typename CalibReadMap::iterator outIter = _readMap.begin(); 
	outIter != _readMap.end(); ++outIter ){
    CalibModuleMap* modmap = (*outIter).second.second;
    for ( typename CalibModuleMap::iterator inIter = modmap->begin();
	  inIter != modmap->end(); ++inIter ){
      T* calobj = (*inIter).second;
      calobj->setConstant( f* calobj->getConstant() );
      calobj->setConstantError( f* calobj->getConstantError() );
    }
  }
};

void setScalingFactor( float f ){
  _scalingFactor = f;
};

private:
float _scalingFactor;

};

}

#endif
