#ifndef _ModuleIndexReverseLookup_hh_
#define _ModuleIndexReverseLookup_hh_
#include "MappingAndAlignment.hh"
#include <stdexcept>

namespace CALICE {
/** Creates huge arrays which allows to find out the module and the cell index using the Mokka conform geometrical index.
 */
class ModuleIndexReverseLookup
{
public:
  //  ModuleIndexReverseLookup();
  //  ~ModuleIndexReverseLookup();

  /** Create a huge arrays which contains for each cell the module and the cell index.
   * The array which contains the cell indices uses the module type, the wafer and pad 
   * row and column indices. The array which contains the module indices uses the wafer
   * row and column index. 
   */
  void createIndexReverseLookup(const MappingAndAlignment &mapping);

  /** Return the module and cell index for the given Mokka conform geometrical cell index.
   * @param mapping reference to the mapping object.
   * @param a_cell_index the Mokka conform geometrical cell index
   * @return the module index and the cell index on the module.
   * The module index is the index in the list of defined module locations (coditions datae). 
   * The cell index is the index of the cells on the module. The pads are multiplexed on 12(6) 
   * lines (readout chips). First the 12(6) cells of the fist sample are stored than the cells 
   * of next sample.
   */
  inline pair<UInt_t, UInt_t> getModuleAndCellIndex(const MappingAndAlignment &mapping, const CellIndex &a_cell_index) const 
#ifdef BOUNDARY_CHECK
    throw(std::logic_error)
#endif    
  {
    UInt_t layer_i=a_cell_index.getLayerIndex();
    UInt_t row_i=a_cell_index.getWaferRow();
    UInt_t column_i=a_cell_index.getWaferColumn();
    ///#ifdef BOUNDARY_CHECK
    if (   layer_i>=_moduleIndexArray.size() 
	|| row_i>=_moduleIndexArray[layer_i].size()
	|| column_i>=_moduleIndexArray[layer_i][row_i].size() ) {
      throw std::range_error("ModuleIndexReverseLookup::getModuleAndCellIndex> The layer index or the wafer row or column points beyond the module lookup array.");
    }

    //#endif
    UInt_t module_index=_moduleIndexArray[layer_i][row_i][column_i];
#ifdef BOUNDARY_CHECK
    if (a_cell_index.getCellIndex()<mapping.getCellIndexOffset(module_index)) {
      throw std::logic_error("ModuleIndexReverseLookup::getModuleAndCellIndex> The given geometrical cell index does not seem to be assigned to a pad of a defined module.");
    }
#endif

    CellIndex geom_cell_index(a_cell_index.getCellIndex()-mapping.getCellIndexOffset(module_index));
    row_i=geom_cell_index.getWaferRow();
    column_i=geom_cell_index.getWaferColumn();
    UInt_t pad_row_i=geom_cell_index.getPadRow();
    UInt_t pad_column_i=geom_cell_index.getPadColumn();

    // If BOUNDARY_CHECK is defined the following method should throw an exception if module_index is out of bounds or
    // the type does not exist.
    UInt_t module_type=mapping.getModuleType(module_index);

#ifdef BOUNDARY_CHECK
    if (module_type>=_moduleTypeArray.size() 
	|| row_i>=_moduleTypeArray[module_type].size()
	|| column_i>=_moduleTypeArray[module_type][row_i].size() 
	|| pad_row_i>=_moduleTypeArray[module_type][row_i][column_i].size()
	|| pad_column_i>=_moduleTypeArray[module_type][row_i][column_i][pad_row_i].size()) {
      throw std::range_error("ModuleIndexReverseLookup::getModuleAndCellIndex> The wafer or pad row or column points beyond the cell lookup array.");
    }
#endif
    UInt_t cell_index=_moduleTypeArray[module_type][row_i][column_i][pad_row_i][pad_column_i];
#ifdef BOUNDARY_CHECK
    if (cell_index>mapping.getNCellsPerModule(module_index)) {
      throw std::range_error("MipSignalHistograms::processEvent> could not identify cell of a hit.");
    }
#endif		    

    return make_pair(module_index,cell_index);
  };

protected:
  SimpleArray_t< SimpleArray_t<  SimpleArray_t< unsigned short > > > _moduleIndexArray; /**< Array which contains for each layer and wafer row and column the module index.*/
  typedef SimpleArray_t< SimpleArray_t<  SimpleArray_t< SimpleArray_t< unsigned short > > > > CellIndexArray_t;
  SimpleArray_t< CellIndexArray_t > _moduleTypeArray;   /**< Array which contains for each module type the cell indices for a pad of a given wafer, pad row and column.*/
};
}
#endif
