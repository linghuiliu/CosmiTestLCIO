#include <ModuleIndexReverseLookup.hh>
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif

namespace CALICE {
  void ModuleIndexReverseLookup::createIndexReverseLookup(const MappingAndAlignment &mapping) {
    if (mapping._moduleLocationList.empty() || mapping._moduleTypeList.empty()) return;
    _moduleIndexArray.clear();
    UInt_t module_index=0;

    // it is assumed that a module is unambigiously defined by the layer, the wafer row and column index.
    // so first all cells of all modules are scanned and a 3D array is filled with the module index where
    // the array compoonents are: layer number, wafer row, wafer column.
    // very efficient ;-)
    // TODO: better idea? more efficient, less memory consumption?

    for(MappingAndAlignment::ModuleList_t::const_iterator module_iter=mapping._moduleLocationList.begin();
	module_iter!=mapping._moduleLocationList.end();
	module_iter++, module_index++) {
	const ModuleDescription &description=mapping._moduleTypeList[module_iter->second.getModuleType()];
	for (UInt_t cell_i=0; cell_i<description.getNCells(); cell_i++) {
	  CellIndex index(description.getGeometricalCellIndex(cell_i)+module_iter->second.getCellIndexOffset());
	  UInt_t layer_i=index.getLayerIndex();
	  UInt_t row_i=index.getWaferRow();
	  UInt_t column_i=index.getWaferColumn();
	  if (_moduleIndexArray.size()<=layer_i) _moduleIndexArray.resize(layer_i+1);
	  if (_moduleIndexArray[layer_i].size()<=row_i) _moduleIndexArray[layer_i].resize(row_i+1);
	  if (_moduleIndexArray[layer_i][row_i].size()<=column_i) _moduleIndexArray[layer_i][row_i].resize(column_i+1,USHRT_MAX);
	  _moduleIndexArray[layer_i][row_i][column_i]=module_index;
	}
    }

    // it is assumed that the cell on top of a module is defined by the row, index the wafer index, the pad row
    // and the pad column.
    // So a multidimensional array is created which contains the cell index where the array components are:
    // wafer row, wafer column, pad_row, pad_column.
    // very efficient ;-)
    // TODO: better idea? more efficient, less memory consumption?

    _moduleTypeArray.clear();
    for(MappingAndAlignment::ModuleTypeList_t::const_iterator type_iter=mapping._moduleTypeList.begin();
	type_iter!=mapping._moduleTypeList.end();
	type_iter++) {
      _moduleTypeArray.push_back(CellIndexArray_t());
      for (UInt_t cell_i=0; cell_i<type_iter->getNCells(); cell_i++) {
	CellIndex index(type_iter->getGeometricalCellIndex(cell_i));
	UInt_t row_i=index.getWaferRow();
	UInt_t column_i=index.getWaferColumn();
	UInt_t pad_row_i=index.getPadRow();
	UInt_t pad_column_i=index.getPadColumn();
	if (_moduleTypeArray.back().size()<=row_i) _moduleTypeArray.back().resize(row_i+1);
	if (_moduleTypeArray.back()[row_i].size()<=column_i) _moduleTypeArray.back()[row_i].resize(column_i+1);
	if (_moduleTypeArray.back()[row_i][column_i].size()<=pad_row_i) _moduleTypeArray.back()[row_i][column_i].resize(pad_row_i+1);
	if (_moduleTypeArray.back()[row_i][column_i][pad_row_i].size()<=pad_column_i) _moduleTypeArray.back()[row_i][column_i][pad_row_i].resize(pad_column_i+1,USHRT_MAX);
	_moduleTypeArray.back()[row_i][column_i][pad_row_i][pad_column_i]=cell_i;
      }
    }
  }

}
