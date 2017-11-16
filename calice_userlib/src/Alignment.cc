#ifdef USE_LCCD
#include <Alignment.hh>
#include <DetectorTransformation.hh>
#include <ExperimentalSetup.hh>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <climits>
#include <stdlib.h>

#include <Exceptions.h>

#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>

#include "UTIL/LCTime.h"

// -- LCCD headers
#include "lccd.h"

#include <EmcStageDataBlock.hh>
#include <AhcSlowReadoutBlock.hh>

namespace CALICE {

  const Double_t Alignment::__degToRad=M_PI/180.;

  Alignment::Alignment() : 
    _stageOffset_x(0),
    _stageOffset_y(0)
  {
    // set transformation to id operation
    setTransformation(0,kDetector);
    setTransformation(0,kReference);

    //Fill the map with pointers to the various types of stage parameters
    _knownStageTypes["EmcStageDataBlock"] = &Alignment::handleEmcStage;
    _knownStageTypes["AhcSlowReadoutBlock"] = &Alignment::handleAhcStage;

  }

  void Alignment::init()
  {
  }

  void Alignment::moduleTypeChanged(lcio::LCCollection* col)
  {
    if (!col) {
      throw std::runtime_error("Alignment::moduleTypeChanged> Missing ModuleDescription conditions data. Sorry: don't know the folder name neither the time stamp.");
    }
    _moduleTypeList.clear();
    for (UInt_t element_i=0; element_i<(UInt_t) col->getNumberOfElements(); element_i++) {
      ModuleDescription a_module(col->getElementAt(element_i));

      // the type index is equal to the array index
      unsigned int module_type=a_module.getModuleType();

      if (module_type<_moduleTypeList.size()) {
	_moduleTypeList[module_type]=a_module;
      }
      else {
	if (module_type>_moduleTypeList.size()) {

	  // the module type index is expected to be a small number
	  // if the module type is an arbitrary number then it is stupid to use a vector.
	  assert(module_type < 23);

	  _moduleTypeList.resize(module_type);
	}
	_moduleTypeList.push_back(a_module);
      }
    }
  }

  void Alignment::moduleLocationChanged(lcio::LCCollection* col) {
    if (!col) {
      throw std::runtime_error("Alignment::moduleLocationChanged> Missing ModuleLocation conditions data. Sorry: don't know the folder name neither the time stamp.");
    }
    //std::cout << "Alignment::moduleLocationChanged>" << std::endl;
    _moduleLocationList.clear();
    //    _moduleIndex.clear();
#ifdef RECO_DEBUG
    std::string since_string =  col->getParameters().getStringVal((lccd::DBSINCE).c_str());     
    LCTime since(static_cast<long64>(strtoll(since_string.c_str(),0,0)));
    std::string till_string =  col->getParameters().getStringVal((lccd::DBTILL).c_str());     
    LCTime till(static_cast<long64>(strtoll(till_string.c_str(),0,0)));
    std::string insertiontime_string =  col->getParameters().getStringVal((lccd::DBINSERTIONTIME).c_str());
    LCTime insertiontime(static_cast<long64>(strtoll(insertiontime_string.c_str(),0,0)));

    //std::cout << "Valid from: " << since.getDateString() << std::endl;
    //std::cout << "Valid to: " << till.getDateString() << std::endl;
    //std::cout << "Valid from: " << since_string << std::endl;
    //std::cout << "Valid to: " << till_string << std::endl;
    std::cout << "Valid from long: " << since.getDateString() << std::endl;
    std::cout << "Valid to long: " << till.getDateString()  << std::endl;
    std::cout << "Insertion time: " << insertiontime.getDateString() << std::endl; 
    std::cout << "Modulelocations Elements: " << col->getNumberOfElements() << std::endl; 
#endif
    for (UInt_t element_i=0; element_i<(UInt_t) col->getNumberOfElements(); element_i++) {
      ModuleLocation location(col->getElementAt(element_i));
#ifdef RECO_DEBUG
      std::cout << "ModuleLocation Changed x pos: " << location.getX() << std::endl;
      std::cout << "ModuleLocation Changed y pos: " << location.getY() << std::endl;
#endif
      _moduleLocationList.push_back(make_pair(UINT_MAX,location));
      //_moduleIndex.insert(make_pair(location.getCellIndexOffset(),static_cast<UInt_t>(_moduleLocationList.size()-1)));
    }
  }

  void Alignment::setTransformation(lcio::LCCollection* col, ETransformation transformation_type)
  {
    if (!col) {
      // if the collection is missing, assume an untransformed detector
      for (UInt_t i=0; i<3; i++) {
	_detectorPos[transformation_type][i]=0;
      }
      _detectorRotationX0[transformation_type]=0;
      _detectorRotationZ0[transformation_type]=0;
      _detectorAngleZX[transformation_type]=0;
    }
    else if (col->getNumberOfElements()==1) {
      try {

	CALICE::DetectorTransformation transformation(col->getElementAt(0));

        if(transformation_type == kDetector) {
          //The sign of the stage correction is chosen according to the observations during 2006 running
	  //_detectorPos[transformation_type][0]=transformation.getDetectorX0()-_stageOffset_x;
	  //_detectorPos[transformation_type][1]=transformation.getDetectorY0()+_stageOffset_y;
	  _detectorPos[transformation_type][0]=transformation.getDetectorX0()-_stageOffset_x;
	  _detectorPos[transformation_type][1]=transformation.getDetectorY0()-_stageOffset_y;
#ifdef RECO_DEBUG         
	  std::cout << " Detector Transformation X: " <<  transformation.getDetectorX0() << std::endl;
	  std::cout << " Detector Transformation Y: " <<  transformation.getDetectorY0() << std::endl;
	  
	  std::cout << " Detector Transformation XStand: " <<  _stageOffset_x << std::endl;
	  std::cout << " Detector Transformation YStand: " <<  _stageOffset_y << std::endl;
#endif
        }
        else {
	  _detectorPos[transformation_type][0]=transformation.getDetectorX0();
	  _detectorPos[transformation_type][1]=transformation.getDetectorY0();
	}
	_detectorPos[transformation_type][2]=transformation.getDetectorZ0();
	_detectorAngleZX[transformation_type]=transformation.getDetectorAngleZX() *__degToRad ;
	_detectorRotationX0[transformation_type]=transformation.getDetectorRotationX0();
	_detectorRotationZ0[transformation_type]=transformation.getDetectorRotationZ0();
	
      }
      catch (lcio::Exception &err) {
	// backward compatibility:
	CALICE::ExperimentalSetup setup(col->getElementAt(0));
	_detectorPos[transformation_type][0]=setup.getDetectorX0();
	_detectorPos[transformation_type][1]=setup.getDetectorY0();
	_detectorPos[transformation_type][2]=setup.getDetectorZ0();
	_detectorAngleZX[transformation_type]=setup.getDetectorAngleZX() *__degToRad ;
	_detectorRotationX0[transformation_type]=setup.getDetectorRotationX0();
	_detectorRotationZ0[transformation_type]=setup.getDetectorRotationZ0();
      }
    }
    else {
      std::stringstream message;
      message << "Alignment::setTransformation> Expecting collection with one Transformation object "
	         "but got a collection with " << col->getNumberOfElements() << " elements." ;
      throw std::runtime_error(message.str());
    }
    calculateEffectiveTransformation();
  }

  void Alignment::calculateEffectiveTransformation() {
    // calculate an effective translation and rotation angle 
    // such that : x_prime = Rot(alpha) x   + x_trans.
    // Note, the rotation axis is the y-axis, thus the y-component is only translated.

    // If the reference and the transformation are bitwise identical then do not rotate:
    if (_detectorAngleZX[kDetector]==_detectorAngleZX[kReference]) {

      _detectorAngleZX[kEffective]=0.;
      _detectorAngleZXSin=0.;
      _detectorAngleZXCos=1.;

    }
    else {
      _detectorAngleZX[kEffective]=_detectorAngleZX[kDetector]-_detectorAngleZX[kReference];
      _detectorAngleZXSin=sin(_detectorAngleZX[kEffective]);
      _detectorAngleZXCos=cos(_detectorAngleZX[kEffective]);
    }

    // inverse rotation :
    double c =   cos(_detectorAngleZX[kReference]);
    double s =   sin(_detectorAngleZX[kReference]);

    _detectorRotationX0[kEffective]=0;
    _detectorRotationZ0[kEffective]=0;

    double rel_pos_x =  _detectorPos[kDetector][0]+_detectorRotationX0[kDetector]
                      - _detectorPos[kReference][0]-_detectorRotationX0[kReference];

    double rel_pos_z =  _detectorPos[kDetector][2]+_detectorRotationZ0[kDetector]
                      - _detectorPos[kReference][2]-_detectorRotationZ0[kReference];

    _detectorPos[kEffective][0] = 
       _detectorRotationX0[kReference]
      + c * rel_pos_x + s * rel_pos_z
      - _detectorAngleZXCos * _detectorRotationX0[kDetector] 
      - _detectorAngleZXSin * _detectorRotationZ0[kDetector];

    _detectorPos[kEffective][1] = _detectorPos[kDetector][1] - _detectorPos[kReference][1];

    _detectorPos[kEffective][2] = 
       _detectorRotationZ0[kReference]
      - s * rel_pos_x + c * rel_pos_z
      + _detectorAngleZXSin * _detectorRotationX0[kDetector] 
      - _detectorAngleZXCos * _detectorRotationZ0[kDetector];
  }

  void Alignment::stagePositionChanged(lcio::LCCollection* col) {
    std::map<std::string,StageChangeHandleFunc_t>::iterator iter=_knownStageTypes.find(col->getParameters().getStringVal("TypeName")); 
    if(iter != _knownStageTypes.end()) {StageChangeHandleFunc_t aFunc = iter->second;
    (this->*aFunc)(col);} 
    {}

    std::cout  << "Number of Elements: " << col->getNumberOfElements() << std::endl;
 }

  void Alignment::handleEmcStage(LCCollection* col) {
    std::cout << "In handle EmcStage" << std::endl;
    if(col) { 
       EmcStageDataBlock emcStgDat(col->getElementAt(0));
       _stageOffset_x = static_cast<float>(emcStgDat.getXStandPosition()*0.1);
       _stageOffset_y = static_cast<float>(emcStgDat.getYStandPosition()*0.1);
       std::cout << " xStandPosition/mm: " <<  _stageOffset_x << std::endl;
       std::cout << " yStandPosition/mm: " <<  _stageOffset_y << std::endl;
       emcStgDat.print(std::cout);

    } 

  }

  void Alignment::handleAhcStage(LCCollection* col) {
    std::cout << "In handle AhcStage" << std::endl;
    if(col) { 
      AhcSlowReadoutBlock ahcSroDat(col->getElementAt(0)); 
      _stageOffset_x = static_cast<float>(ahcSroDat.get_xPosition_mm());
      _stageOffset_y = static_cast<float>(ahcSroDat.get_xPosition_mm());
      std::cout << "Ahc: Movable stage position x[mm]: " << ahcSroDat.get_xPosition_mm() << std::endl;
      std::cout << "Ahc: Movable stage position y[mm]: " << ahcSroDat.get_yPosition_mm() << std::endl;
      ahcSroDat.print(std::cout);
    } 

  }




}
#endif
