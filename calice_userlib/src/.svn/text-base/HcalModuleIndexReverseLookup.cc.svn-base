#include <HcalModuleIndexReverseLookup.hh>
//#include <values.h>
#include <climits>
#include <cfloat>

//#define HCALRECO_DEBUG

namespace CALICE {

  void HcalModuleIndexReverseLookup::createIndexReverseLookup(const MappingAndAlignment &mapping) {
    if (mapping._moduleLocationList.empty() || mapping._moduleTypeList.empty()) return;

    _cellIndexArray.clear();
    UInt_t module_i(0);
    for (MappingAndAlignment::ModuleList_t::const_iterator module_iter=mapping._moduleLocationList.begin();
	module_iter!=mapping._moduleLocationList.end();
	module_iter++, module_i++) {
      const ModuleDescription &description=mapping._moduleTypeList[module_iter->second.getModuleType()];
      const UChar_t mtype = module_iter->second.getModuleType();
      int mmtype = (int)mtype;
      for (unsigned short cell_i= (mmtype == 7 ? 39 : 0); cell_i != (mmtype == 6 ? 72 : 108); cell_i++) {
#ifdef HCALRECO_DEBUG        
              std::cout << " cell_i " << cell_i << " geometricalIndex " << description.getGeometricalCellIndex(cell_i) << std::endl;
#endif
	const HcalCellIndex index(description.getGeometricalCellIndex(cell_i) + module_iter->second.getCellIndexOffset());
	const unsigned short layer_i=index.getLayerIndex();
	const unsigned short tile_row_i=index.getTileRow();
	const unsigned short tile_column_i=index.getTileColumn();
#ifdef HCALRECO_DEBUG 
        std::cout << "module " << module_i << " module type " << mmtype << " cell " << cell_i << " layer " << layer_i << " tile_row " << tile_row_i << " tile_column " << tile_column_i << std::endl;
#endif
	if (_cellIndexArray.size()<=layer_i) _cellIndexArray.resize(layer_i+1);
	if (_cellIndexArray[layer_i].size()<=tile_row_i) _cellIndexArray[layer_i].resize(tile_row_i+1);
	if (_cellIndexArray[layer_i][tile_row_i].size()<=tile_column_i) _cellIndexArray[layer_i][tile_row_i].resize(tile_column_i+1,USHRT_MAX);
	_cellIndexArray[layer_i][tile_row_i][tile_column_i]= cell_i + (module_i << 8);
      }
    }
  }
}
