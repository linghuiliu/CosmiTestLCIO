#ifndef _FeConnectionDef_t_H_
#define _FeConnectionDef_t_H_

#include <RtypesSubSet.h>

class FeConnectionDef_t
{
public:
  enum EModuleType {kCentral, kLowRight, kLowLeft};

  FeConnectionDef_t() {};

  FeConnectionDef_t(UInt_t slot, UInt_t front_end, UInt_t front_end_side, UInt_t layer_number, EModuleType type, UInt_t id, const string &module_name) 
    : _slot(slot),
      _frontEnd(front_end),
      _frontEndSide(front_end_side),
      _layerNumber(layer_number),
      _moduleType(type),
      _moduleID(id),
      _moduleName(module_name)
  {
  };
  //  ~FeConnectionDef_t();
  
  UInt_t getLayerNumber() const {return _layerNumber;};

  UInt_t getSlot() const {return _slot;};

  UInt_t getFrontEnd() const {return _frontEnd;};

  /** The side of the connector on the CERC board.
   * @return 0 for the connectors on the left (currently only the left side is used), 1 for connectors on the right.
   */
  UInt_t getFrontEndSide() const {return _frontEndSide;};

  EModuleType getModuleType() const {return _moduleType;};

  /** Get an identifier which uniquely identifies the module together with the module type.
   */
  UInt_t getModuleID() const {return _moduleID;};

  /** The module name is the unique module label.
   * The module name is compose of a number and an extension which defines the module type:
   * central _C, lower left (_G) and lower right (_D). The number runs from 1 to n for each
   * type separately.
   */
  const std::string &getModuleName() const {return _moduleName;};
  
  UInt_t _slot;
  UInt_t _frontEnd;
  UInt_t _frontEndSide;

  UInt_t _layerNumber;
  EModuleType _moduleType;
  UInt_t _moduleID;
  
  std::string _moduleName;
};
#endif
