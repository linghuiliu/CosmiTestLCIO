#ifndef ModuleLocation_hh
#define ModuleLocation_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <ModuleDescription.hh>
#include <sstream>
#include <iostream>

#include <RtypesSubSet.h>

using namespace lcio ;
namespace CALICE {
//namespace CALICE {

/** 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */

enum EModuleLocationFloatType {kModuleLocationFloatX, kModuleLocationFloatY, kModuleLocationFloatZ, kNModuleLocationFloats};
enum EModuleLocationIntType {kModuleLocationIntModuleType,kModuleLocationIntCellIndexOffset,
			     kNModuleLocationInts};

  /** Define possible positions of modules. 
   * A module position is not necessarily connected. A connected to the DAQ is
   * established via @ref ModuleConnection. A module position is uniquely defined by the 
   * index offset i.e. two modules may not have the same index offset.
   */
class ModuleLocation : public LCFixedObject<kNModuleLocationInts,kNModuleLocationFloats,0> {
public: 
  ModuleLocation() : LCFixedObject<kNModuleLocationInts,kNModuleLocationFloats,0>() { };
    
  /** 'Copy constructor', create the ModuleLocation interface for the generic objects read from file/database.
   * The objects stored in the file or database are of type LCGenericObject. To generate the interface of the
   * object ModuleLocation:
   * @code
   * ModuleLocation a_module_location(lc_collection->getElementAt(index));
   * @endcode
   * @todo Currenlty, there is no serious type checking. Only the number of integers, floats and doubles is used
   *       to verify whether the LCGenericObject is of the type ModuleLocation
   */
  ModuleLocation(LCObject* obj) : LCFixedObject<kNModuleLocationInts,kNModuleLocationFloats,0>(obj) { } 
  
  ModuleLocation &  setX(float value) 
  {
    obj()->setFloatVal(kModuleLocationFloatX, value);
    return *this;
  };
  float getX() const
  {
    return getFloatVal(kModuleLocationFloatX);
  };

  ModuleLocation &  setY(float value) 
  {
    obj()->setFloatVal(kModuleLocationFloatY, value);
    return *this;
  };
  float getY() const
  {
    return getFloatVal(kModuleLocationFloatY);
  };

  ModuleLocation &  setZ(float value) 
  {
    obj()->setFloatVal(kModuleLocationFloatZ, value);
    return *this;
  };
  float getZ() const
  {
    return getFloatVal(kModuleLocationFloatZ);
  };


  /** Set the offset to the cell index to form the full cell index of the layer.
   * The cell index is formed by a cell index calculated for a certain module type 
   * plus this offset. The result is a unique cell index to identify a
   * cell unambiguously.
   * @sa CellIndex
   */
  ModuleLocation &  setCellIndexOffset(int first_cell_index) 
  {
    obj()->setIntVal(kModuleLocationIntCellIndexOffset, first_cell_index);
    return *this;
  };

  /** Get the offset to the cell index to form the full cell index of the layer.
   * @sa setCellIndexOffset
   */
  int getCellIndexOffset() const 
  {
    return getIntVal(kModuleLocationIntCellIndexOffset);
  };

  /** Set the module type.
   * Currently, a module location is identified by the layer number and the
   * module type i.e. each module type may be used at most once per layer.
   */
  ModuleLocation&  setModuleType(UChar_t type) 
  {
    obj()->setIntVal(kModuleLocationIntModuleType, (int) type);
    return *this;
  };

  /** Get the type of a module.
   * @sa setModuleType
   */
  UChar_t getModuleType() const
  {
    return (UChar_t) getIntVal(kModuleLocationIntModuleType);
  };

  void print(  std::ostream& os );
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("ModuleLocation");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "i[" << kNModuleLocationInts << "]:type, cellIndexOffset, f[" << kNModuleLocationFloats<< "]: x,y,z";
    return message.str();
  };

}; // class

//}
}
#endif 
