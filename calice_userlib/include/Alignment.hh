#ifdef USE_LCCD
#ifndef Alignment_hh
#define Alignment_hh

#include <vector>
#include <map>
#include <utility>
#include <stdexcept>

#include <ModuleLocation.hh>
#include <ModuleDescription.hh>
#include <SimpleArray_t.hh>
#include <CellIndex.hh>

#if ( __GNUC__ < 3 )
#  include <math.h>
#else
#  include <cmath>
#endif

#include <NVector_t.hh>

typedef unsigned int UInt_t;
typedef float Float_t;

using namespace std;


namespace CALICE {

/** Class to comunicate/handle the translation into unique layer/cell ids and 3D spacial coordinates.
 * The class has to be informed about changes of the detector configuration using the methods
 * \ref CALICE::Alignment::moduleTypeChanged, \ref CALICE::Alignment::moduleLocationChanged, 
 * \ref CALICE::MappingAndAlignment::moduleConnectionChanged and Alignment::experimentalSetup.
 * This information is used to map the DAQ signals to cells and calculate 3D spacial coordinates
 * for hits.
 * 
 * The class performs range checks only if BOUNDARY_CHECK is defined.
 *
 */

 class Alignment
{
  // friend class VRawADCValueProcessor;
  // friend class ModuleIndexReverseLookup;
public:
  /** default constructor. 
   * The default constructor configures the conditions data change delegators.
   */
  Alignment();

  virtual ~Alignment() {};

  /**Does nothing for the time being.
   */
  void init();

  /** Get the unique cell index of the cell specified by the module and the cell index.
   * @param module_index unique index for modules which should run from 0..total number of modules 
   * @param cell_index within the module (0.. number of cells of this module type)
   * @return unique cell index.
   * @throw range_error if the module index is out of range and BOUNDARY_CHECK is defined
   *
   * The cell index conforms to the Mokka specification (see for
   * example @ref CellIndex). It is composed of the layer number, the
   * wafer column and row, and the pad column and row.
   */
  UInt_t getGeometricalCellIndex(UInt_t module_index,  UInt_t cell_index) const 
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getGeometricalCellIndex> Module " << module_index << " is not defined or module type does not exist.";
	throw range_error(message.str());
    }
#endif
    UInt_t module_type=_moduleLocationList[module_index].second.getModuleType();

    return 
	_moduleTypeList[module_type].getGeometricalCellIndex(cell_index)
	+_moduleLocationList[module_index].second.getCellIndexOffset();
  };

  /** Return true if the type of the given module is valid.
   * @param module_index the index of the given module.
   */
  bool isValid(UInt_t module_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size()) {
	std::stringstream message;
	message << "Alignment::isValid> Module index " << module_index << " is out of range.";
	throw range_error(message.str());
    }
#endif
    return _moduleLocationList[module_index].second.getModuleType() < _moduleTypeList.size();
  }

  /** Get the type of a certain module.
   * The module type together with the module ID uniquly identifies a module.
   * Different modules of one type always have different IDs but modules of different types may have the same ID.
   * In case of the ECAL the module ID is the serial number of the PCBs.
   * @param module_index the index of the module within the detector (not necessarily the same as the module ID)
   * @return the module type
   * @throw range_error if the module specified by module_index is not defined but only if BOUNDARY_CHECK is defined.
   * @see getModuleID()
   */
  UChar_t getModuleType(UInt_t module_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getModuleID> Module " << module_index << "is not defined.";
	throw range_error(message.str());
    }
#endif
    return (UChar_t) _moduleLocationList[module_index].second.getModuleType();
  }
 
  ThreeVector_t getPosition(UInt_t module_index, UInt_t cell_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {

#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getPosition> Module " << module_index << "is not defined.";
	throw range_error(message.str());
    }
    
    if (cell_index>=_moduleTypeList[_moduleLocationList[module_index].second.getModuleType()].getNCells()) {
	std::stringstream message;
	message << "Alignment::getPosition> cell index " << cell_index << " is out of range for module of type "
		<< (int) _moduleLocationList[module_index].second.getModuleType() << ".";
	throw range_error(message.str());
    }
#endif

    ThreeVector_t a_position;

    const pair<UInt_t, ModuleLocation> &the_location=_moduleLocationList[module_index];
    const ModuleDescription &the_module_type=_moduleTypeList[the_location.second.getModuleType()];

    a_position[0]=  the_location.second.getX()
                  + the_module_type.getCellXPos(cell_index);

    a_position[1]=  the_location.second.getY()
                  + the_module_type.getCellYPos(cell_index);

    a_position[2]=  the_location.second.getZ();

#ifdef RECO_DEBUG
    std::cout << "Module Index: " << module_index << std::endl;
    std::cout << "Module X Position: " << the_location.second.getX() << std::endl;
    std::cout << "x position: " << a_position[0] << std::endl;
#endif

    //CRP 20/12/06 Commented out since it requires new ModuleDescription Class
    //Might be re-activated later
    //    if (the_module_type.hasCellZPos()) {
    //  a_position[2] += the_module_type.getCellZPos(cell_index);
    //}
    return transformPosition(a_position);
  };

  ThreeVector_t transformPosition(ThreeVector_t &a_position) const {

    if (_detectorAngleZX[kEffective]!=0.) {
      const Double_t c=_detectorAngleZXCos;
      const Double_t s=_detectorAngleZXSin;


      Double_t delta_x = a_position[0];
      Double_t delta_z = a_position[2];

      //CRP 26/1/07 To my mind this is the wrong rotation matrix      
      //a_position[0]=c*delta_x - s *delta_z ;
      //a_position[2]=s*delta_x + c *delta_z;

      //This seems to be more correct
      a_position[0]=c*delta_x + s *delta_z ;
      a_position[2]=-s*delta_x + c *delta_z;

    }

    for(UInt_t i=0; i<3; i++) {
      a_position[i]+=_detectorPos[kEffective][i];
    }

//     std::cout << "detect. x=" << _detectorPos[kEffective][0]
//     	      << " y=" << _detectorPos[kEffective][1]
//     	      << " z=" << _detectorPos[kEffective][2]
// //            << "\tmodule: x=" << _moduleLocationList[module_index].second.getX()
// //     	      << " y=" << _moduleLocationList[module_index].second.getY()
// //            << " z=" << _moduleLocationList[module_index].second.getZ()
// //            << "cell:"
// // 	      << " x=" << _moduleTypeList[_moduleLocationList[module_index].second.getModuleType()].getCellXPos(cell_index)
// //            << " y=" << _moduleTypeList[_moduleLocationList[module_index].second.getModuleType()].getCellYPos(cell_index)
//        	      << "\thit: x=" << a_position[0]
//      	      << " y=" << a_position[1]
//     	      << " z="  << a_position[2]
//      	      << std::endl;

    return a_position;
  }

  UInt_t getNModules() const {return _moduleLocationList.size();};

  /** Return false if the module index is out of range.
   */
  Bool_t isModule(UInt_t module_index) const {
    return module_index<getNModules();
  }

  /** Return the description object for the given module.
   * @param module_index the index of the moduel.
   */
  const ModuleDescription &getModuleDescription(UInt_t module_index) const {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
      std::stringstream message;
      message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
      throw range_error(message.str());
    }
#endif
    UInt_t module_type=_moduleLocationList[module_index].second.getModuleType();
    return _moduleTypeList[module_type];
  };


  UInt_t getNCellsPerModule(UInt_t module_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
	throw range_error(message.str());
      }
#endif
      // std::cout << "Query nr of cells for module " << module_index << " which is of the type " << (UInt_t) _moduleLocationList[module_index].second.getModuleType() << "->"
      //	<< _moduleTypeList[_moduleLocationList[module_index].second.getModuleType()].getNCells()
      //	<< std::endl;
      return _moduleTypeList[_moduleLocationList[module_index].second.getModuleType()].getNCells();
  }

  /** Get the cell width of the given module and cell.
   */
  Float_t getModuleWidth(UInt_t module_index) const 
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
	throw range_error(message.str());
      }
#endif
      UInt_t module_type=_moduleLocationList[module_index].second.getModuleType();
      return _moduleTypeList[module_type].getWidth();
    };

  /** Get the cell width of the given module and cell.
   */
  Float_t getModuleHeight(UInt_t module_index) const 
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
	throw range_error(message.str());
      }
#endif
      UInt_t module_type=_moduleLocationList[module_index].second.getModuleType();
      return _moduleTypeList[module_type].getHeight();
    };


  /** Get the cell width of the given module and cell.
   */
  Float_t getCellWidth(UInt_t module_index, UInt_t cell_index) const 
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
	throw range_error(message.str());
      }
#endif
      UInt_t module_type=_moduleLocationList[module_index].second.getModuleType();
 //CRP 20/12/06 Commented out since it requires new ModuleDescription Class
//Might be re-activated later 
//      if (_moduleTypeList[module_type].hasCellDimensionsPerCell()) {
//	return _moduleTypeList[module_type].getIndividualCellWidth(cell_index);
//      }
//      else {
	return _moduleTypeList[module_type].getCellWidth();
//      }
    };

  /** Get the cell width of the given module.
   * It is assumed that the cells of one module all have the same width.
   */
  Float_t getCellHeight(UInt_t module_index, UInt_t cell_index) const 
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
	throw range_error(message.str());
      }
#endif
      UInt_t module_type=_moduleLocationList[module_index].second.getModuleType();

 //CRP 20/12/06 Commented out since it requires new ModuleDescription Class
//Might be re-activated later 
//      if (_moduleTypeList[module_type].hasCellDimensionsPerCell()) {
//	return _moduleTypeList[module_type].getIndividualCellHeight(cell_index);
//      }
//      else {
	return _moduleTypeList[module_type].getCellHeight();
//      }
    };

  /** Get the position of the detector in its proper coordinate system
   */
  ThreeVector_t getPosition() const 
  {
    ThreeVector_t position;
    position[0]=_detectorPos[kEffective][0];
    position[1]=_detectorPos[kEffective][1];
    position[2]=_detectorPos[kEffective][2];

    return position;
  }

  /** Get the upper right corner of a module.
   */
  ThreeVector_t getModuleProperCenter(UInt_t module_index) const 
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
      std::stringstream message;
      message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
      throw range_error(message.str());
    }
#endif
    UInt_t module_type=_moduleLocationList[module_index].second.getModuleType();
    const ModuleDescription &module_description = _moduleTypeList[module_type];

    ThreeVector_t position;
    position[0]=_moduleLocationList[module_index].second.getX()+module_description.getWidth()*.5;
    position[1]=_moduleLocationList[module_index].second.getY()+module_description.getHeight()*.5;
    position[2]=_moduleLocationList[module_index].second.getZ();
    return position;
  }

  /** Get the position of a module in the proper coordinate system of the detector.
   */
  ThreeVector_t getModuleProperPosition(UInt_t module_index) const 
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size()) {
      std::stringstream message;
      message << "Alignment::getNCellsPerModule> Module " << module_index << "is not defined.";
      throw range_error(message.str());
    }
#endif

    ThreeVector_t position;
    position[0]=_moduleLocationList[module_index].second.getX();
    position[1]=_moduleLocationList[module_index].second.getY();
    position[2]=_moduleLocationList[module_index].second.getZ();
    return position;
  }

  /** Get the position of a module in the reference coordinate system.
   */
  ThreeVector_t getModulePosition(UInt_t module_index) const 
  {
    ThreeVector_t position=getModuleProperPosition(module_index);
    return transformPosition(position);
  }


  /** Get the center of a module in the reference coordinate system.
   */
  ThreeVector_t getModuleCenter(UInt_t module_index) const 
  {
    ThreeVector_t position=getModuleProperCenter(module_index);
    return transformPosition(position);
  }

  /** Return the effective rotation angle of the detector.
   */
  Float_t getDetectorAngleZX() const {
    return _detectorAngleZX[kEffective];
  }

  /**Inform the object about changes of the module types.
   * @param col collection of  @ref ModuleDescription objects
   * This class must be called before using any(most) of the other methods.
   * The function call will cause a rebuild of the module DAQ front-end connection tree.
   */
  virtual void moduleTypeChanged(lcio::LCCollection* col);

  /**Inform the object about changes of the module locations.
   * @param col collection of  @ref ModuleLocation objects
   * This class must be called before using any(most) of the other methods.
   * The function call will cause a rebuild of the module DAQ front-end connection tree.
   */
  virtual void moduleLocationChanged(lcio::LCCollection* col);

  /**Inform the object about changes in the stage positions on which the detector is mounted.
   * @param col collection of StageParameters objects
   * This class must be called before calculating the actual cell positions
   * The function will provide the xy values for a correct transversal alignment of the detectors.
   */
  virtual void stagePositionChanged(lcio::LCCollection* col);


  /**Inform the object about changes of the detector position and rotation.
   * @param col collection of  @ref DetectorTransformation objects
   * This class must be called before querying the postion of cells.
   * The position is calculated in the reference coorindate system.
   */
  virtual void detectorTransformation(lcio::LCCollection* col) {
    setTransformation(col, kDetector);
  }

  /**Inform the object about changes of the reference position and rotation.
   * @param col collection of  @ref DetectorTransformation objects
   * This class must be called before querying the postion of cells.
   * The position is calculated in the reference coordinate system.
   */
  virtual void referenceTransformation(lcio::LCCollection* col) {
    setTransformation(col,kReference);
  }


  /** Get the cell index offset of a module.
   */
  UInt_t getCellIndexOffset(UInt_t module_index) const {
    return _moduleLocationList[module_index].second.getCellIndexOffset();
  };

protected:
  typedef SimpleArray_t<pair<UInt_t,ModuleLocation> > ModuleList_t;   /**< Defined module locations (not necessarily connected to a front-end.
									 First component contains the module ID, the second the module 
									 location information.*/
private:
  enum ETransformation {kDetector, kReference, kEffective, kNTransformations };

  /** Set the transformation parameters from the conditions data collection
   */
  void setTransformation(lcio::LCCollection* col, ETransformation transformation_type);

  /** Calculate the effective transformation from the detector and reference transformation.*/
  void calculateEffectiveTransformation();

protected:
  ModuleList_t _moduleLocationList;

protected:

  typedef SimpleArray_t<ModuleDescription> ModuleTypeList_t; 
  ModuleTypeList_t _moduleTypeList;

private:
  Float_t _detectorPos[kNTransformations][3];     /**< the position of the detector*/
  Float_t _detectorAngleZX[kNTransformations];    /**< configuration angle w.r.t. to the beam axis in the horizontal plane */
  Double_t  _detectorAngleZXSin;    /**< sin of the effective rotation angle in the z-x plane */
  Double_t  _detectorAngleZXCos;    /**< cos of the effective rotation angle in the z-x plane */

  Float_t _detectorRotationX0[kNTransformations]; /**< origin of the detector ratation (x-direction)*/
  Float_t _detectorRotationZ0[kNTransformations]; /**< origin of the detector ratation (z-direction)*/
  
  /** A map which holds pointers to functions to access the parameters of teh movable stage(s) */
   typedef void (Alignment::* StageChangeHandleFunc_t)(lcio::LCCollection* col);
  std::map<std::string,StageChangeHandleFunc_t> _knownStageTypes;
  void handleEmcStage(LCCollection*); 
  void handleAhcStage(LCCollection*); 

  /** The offsets due to the movable stage */
  float _stageOffset_x;
  float _stageOffset_y;

  static const Double_t __degToRad;

};
}
#endif
#endif
