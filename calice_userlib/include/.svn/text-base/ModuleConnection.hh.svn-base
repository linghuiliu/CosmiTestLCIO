#ifndef ModuleConnection_hh
#define ModuleConnection_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <sstream>
#ifdef VALUE_CHECK
#  include <cassert>
#endif

using namespace lcio ;

namespace CALICE {

/** 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */

enum EModuleConnectionFloatType {kNModuleConnectionFloats};
enum EModuleConnectionIntType {kModuleConnectionIntCrate,
			       kModuleConnectionIntSlot, kModuleConnectionIntFrontEnd,kModuleConnectionIntConnectorType,
			       kModuleConnectionIntIndexOfLowerLeftCell,
			       kModuleConnectionIntModuleType,
			       kModuleConnectionIntModuleID,
			       kNModuleConnectionInts};

  enum EModuleConnectorType{ kLeftConnector, kRightConnector, kFullConnector, kNConnectorTypes};

  /** Defines the electric connection of a module.
   * A module location defined by @ref ModuleLocation may be connected to a connector of a CERC board.
   * The module location is identified by the index offset.
   */
class ModuleConnection : public LCFixedObject<kNModuleConnectionInts,kNModuleConnectionFloats,0> {
public: 
  ModuleConnection() : LCFixedObject<kNModuleConnectionInts,kNModuleConnectionFloats,0>() { };
    
  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  ModuleConnection(LCObject* obj) : LCFixedObject<kNModuleConnectionInts,kNModuleConnectionFloats,0>(obj) { } 
  
  ModuleConnection &  setCrate(int crate_number) 
  {
    obj()->setIntVal(kModuleConnectionIntCrate, crate_number);
    return *this;
  };
  int getCrate() const 
  {
    return getIntVal(kModuleConnectionIntCrate);
  };

  ModuleConnection &  setSlot(int slot_number) 
  {
    obj()->setIntVal(kModuleConnectionIntSlot, slot_number);
    return *this;
  };
  int getSlot() const 
  {
    return getIntVal(kModuleConnectionIntSlot);
  };

  ModuleConnection &  setFrontEnd(int front_end_number) 
  {
    obj()->setIntVal(kModuleConnectionIntFrontEnd, front_end_number);
    return *this;
  };
  int getFrontEnd() const 
  {
    return getIntVal(kModuleConnectionIntFrontEnd);
  };

  //  ModuleConnection &  setFrontEndSide(int front_end_side) 
  //  {
  //#ifdef VALUE_CHECK
  //    assert(front_end_side>=0 && front_end_side < 2)
  //#endif
  //    obj()->setIntVal(kModuleConnectionIntConnectorType, front_end_side);
  //    return *this;
  //  };
  //int getFrontEndSide() const 
  //  {
  //    return (getIntVal(kModuleConnectionIntConnectorType==kRightConnector ? 1 : 0);
  //  };

  /** Set the type of the connector (full, left, right).
   */
  ModuleConnection &  setConnectorType(EModuleConnectorType connector) 
  {
#ifdef VALUE_CHECK
    assert( connector >= kNConnctorTypes)
#endif
    obj()->setIntVal(kModuleConnectionIntConnectorType, connector);
    return *this;
  };

  /** Get the connector type full, left, right.
   */
  int getConnectorType() const 
  {
    return getIntVal(kModuleConnectionIntConnectorType);
  };


  /** Set the cell index (Mokka specification) of the cell in the lower left corner.
   * The index offset is used to uniquely define the module location.
   */
  ModuleConnection &  setIndexOfLowerLeftCell(unsigned int index_offset) 
  {
    obj()->setIntVal(kModuleConnectionIntIndexOfLowerLeftCell, index_offset);
    return *this;
  };

  /** Get the cell index (Mokka specification) of the cell in the lower left corner.
   * This index uniquely defines the module location.
   */
  int getIndexOfLowerLeftCell() const 
  {
#ifdef RECO_DEBUG
    std::cout << "Index: " << getIntVal(kModuleConnectionIntIndexOfLowerLeftCell) << std::endl;
#endif
    return getIntVal(kModuleConnectionIntIndexOfLowerLeftCell);
  };

  //  enum EModuleType {kCentral,kLowRight,kLowLeft,kNModuleTypes};
  //  typedef ModuleDescription::EModuleType EModuleType;

  //ModuleConnection &  setModuleType(EModuleType type) 
  //  {
  //    obj()->setIntVal(kModuleConnectionIntModuleType, (int) type);
  //    return *this;
  //  };
  //  EModuleType getModuleType() const
  //  {
  //    return (EModuleType) getIntVal(kModuleConnectionIntModuleType);
  //  };

  /** Set the module type.
   * The module type is not needed but this redundant information  allows to some verification whether the
   * configuration makes sense.
   * @todo remove this redundant information?
   */
  ModuleConnection &  setModuleType(unsigned char type) 
  {
    obj()->setIntVal(kModuleConnectionIntModuleType, (int) type);
    return *this;
  };

  /** Get the module type.
   * The module type is not needed but this redundant information  allows to verify
   * the correctness of the configuration.
   * @todo remove this redundant information?
   */
  unsigned char getModuleType() const
  {
    return static_cast<unsigned char>(getIntVal(kModuleConnectionIntModuleType));
  };

  /** Set the serial number of a module.
   * This number together with the module type is used to uniquely identfy a module.
   * This unique identification is for example needed to get the correct calibration
   * constants.
   */
  ModuleConnection &  setModuleID(int module_id) 
  {
    obj()->setIntVal(kModuleConnectionIntModuleID, module_id);
    return *this;
  };

  /** Set the serial number of a module.
   * @sa setModuleID
   */
  int getModuleID() const 
  {
    return getIntVal(kModuleConnectionIntModuleID);
  };

  //  unsigned int getUniqueModuleLocationID() const {
  //    UInt_t id=getLayerNumber()*2;
  //    //FIXME: Get the unique module location id from 
  //    if (getModuleType()!=(UInt_t) ModuleDescription::kCentral) {
  //      id++;
  //    }
  //    return id;
  //  };
  
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("ModuleConnection");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "i[" << kNModuleConnectionInts << "]: slot, fe, side, layer, module type, module id";
    return message.str();
  };

  /** Return the name of the connector.
   */
  const char *getConnectorTypeName() const {
    if (static_cast<unsigned int>(getConnectorType())>kNConnectorTypes) return _connectorNames[kNConnectorTypes];
    return _connectorNames[static_cast<unsigned int>(getConnectorType())];
  };

private:
  static const char *_connectorNames[kNConnectorTypes+1];
}; // class

}
#endif 
