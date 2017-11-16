#ifndef SIMPLE_MAPPER_H
#define SIMPLE_MAPPER_H

#include <string>
#include <map>

/** The SimpleMapper helper class reads in the AHC.cfg text file and
 *  provides the mapping (slot, frontend) -> (module, chip)
 */

class SimpleMapper
{
 public:
  SimpleMapper(std::string configFileName);

  /// A pair of ints, the first one being the moduleID, 
  /// the second one the layer number
  typedef  std::pair<int, int> ModuleLayerID;

  /// A pair of ints, the first one being the slot number, 
  /// the second one the front end ID
  typedef  std::pair<int, int> SlotFrontendID;

  /// Get the ModuleLayerID from knowing the SlotFrontendID
  ModuleLayerID getModuleLayerID( SlotFrontendID sfID );

  /// Check if a module is AHC8 (larger 6x6 cells also in the middle of the module,
  /// thus only 8 chips are needed instead of 12 on a normal module).
  bool isAHC8( SlotFrontendID sfID );

  /// Check if a module is from the tail catcher (normal module with 12 chips)
  bool isTCMT( SlotFrontendID sfID );

  /// A debug function. Dumps the map to the console.
  void print();

  /// Get the module type from the SlotFrontEndID
  unsigned int getModuleType( SlotFrontendID sfID );

 //protected:

  /// Internal helper struct for the value: a module description consisting of
  //  moduleID, layerID and moduleTypeWord
  struct ModuleDescription
  {
    int _module;
    int _layer;
    unsigned int _moduleType;

    /// A constructor to initialise convenitently (and consistently)
    ModuleDescription (int module =-1, int layer=-1, unsigned int moduleType = 0);
  };

  /// A map with the SlotFrontendID as key.
  /// The value consists of the ModuleLayerID and a flag word with the module type
  std::map< SlotFrontendID, ModuleDescription > _sf2mlMap;
};

#endif //SIMPLE_MAPPER_H
