#ifndef _HcalModuleIndexReverseLookup_hh_
#define _HcalModuleIndexReverseLookup_hh_
#include "MappingAndAlignment.hh"
#include <HcalCellIndex.hh>
#include <stdexcept>

namespace CALICE {
  /** Class to simplify the lookup geometrical index to hardware index.
   *
   * @author (probably) Sebastian Schmidt
   */
class HcalModuleIndexReverseLookup
{
public:
  void createIndexReverseLookup(const MappingAndAlignment &mapping);

  inline pair<UInt_t, UInt_t> getModuleAndCellIndex(const MappingAndAlignment &mapping, const HcalCellIndex &a_cell_index) const 
  {
    const unsigned short layer_i=a_cell_index.getLayerIndex();
    const unsigned short tile_row_i=a_cell_index.getTileRow();
    const unsigned short tile_column_i=a_cell_index.getTileColumn();
    if (layer_i>=_cellIndexArray.size()
	|| tile_row_i>=_cellIndexArray[layer_i].size()
	|| tile_column_i>=_cellIndexArray[layer_i][tile_row_i].size()) {
      return make_pair(0xFF, 0xFF);	
    }
    const unsigned _index=_cellIndexArray[layer_i][tile_row_i][tile_column_i];
    const unsigned short cell_index = _index & 0x00FF;
    const unsigned short module_index = (_index & 0xFF00) >> 8;
    return make_pair(module_index, cell_index);
  };

protected:
  typedef SimpleArray_t< SimpleArray_t< unsigned short > > CellIndexArray_t;
  SimpleArray_t< CellIndexArray_t > _cellIndexArray; /**< Array which contains for each cell the cell indices for pad row and column.*/
};
}
#endif
