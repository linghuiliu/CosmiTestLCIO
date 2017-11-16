#ifndef _CellParameterAccess_hh_
#define _CellParameterAccess_hh_

#include <string>
#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <CellParameter.hh>

#ifdef BOUNDARY_CHECK
#include <sstream>
#include <stdexcept>
#endif

namespace CALICE {
/** Wrapper around the cell parameter collection to facilitate the access.
 */
class CellParameterAccess
{
public:
  /** Create a simple wrapper to easily access the cell parameters from the LCCollection.
   * @throw DataNotAvailableException if the cell paramter collection does not access.
   */
  CellParameterAccess(const std::string &col_name, EVENT::LCEvent * evtP) {
    // may throw exception:
    _cellParameterCol = evtP->getCollection( col_name ) ;
    
    _indexOffset.clear();
    _cellParameterCol->getParameters().getIntVals(std::string("ModuleStartIndex"),_indexOffset);
  };
 
  /** Get the index of the first cell of the given module.
   * If the code was compiled with BOUNDARY_CHECK defined an exception is thrown if 
   * the module index is larger than the number of modules.
   */
  UInt_t getIndexOffset(UInt_t module_index) const {
#ifdef BOUNDARY_CHECK
    if (module_index>=_indexOffset.size()) {
      std::stringstream message;
      message << "CellParameterAccess::getIndexOffset> module index (" << module_index << ") is out of range ("
	      << _indexOffset.size() << ")";
      throw std::out_of_range(message.str());
    }
#endif    
    return _indexOffset[module_index];
  };

  /** Get the cell index for the given module and cell index.
   * If the code was compiled with BOUNDARY_CHECK defined an exception is thrown if 
   * the module index or cell index is larger than the number of modules.or cells.
   */
  UInt_t getIndex(UInt_t module_index, UInt_t cell_index) const
  {
    UInt_t index_offset=getIndexOffset(module_index);
#ifdef BOUNDARY_CHECK
    UInt_t next_cell=_cellParameterCol->getNumberOfElements();
    if (module_index+1<_indexOffset.size()) {
      next_cell=_indexOffset[module_index+1];
    }
    if (cell_index >= next_cell-_indexOffset[module_index]) {
      std::stringstream message;
      message << "CellParameterAccess::getIndex> cell index (" << cell_index << ") is out of range ("
	      << next_cell-_indexOffset[module_index] << ")";
      throw std::out_of_range(message.str());
    }
#endif
    index_offset+=cell_index;
    return index_offset;
  };

  /** Get the cell parameters of the specified cell (read/write).
   * @param module_index the index of the module (starting from zero).
   * @param cell_index the index of the cell on the specified module (starting from zero).
   * @return a reference of the cell parameters (read/write access).
   */
  CellParameter getCellParameter(UInt_t module_index, UInt_t cell_index) {
    // may throw an exception 
    UInt_t index=getIndex(module_index,cell_index);

    return CellParameter(_cellParameterCol->getElementAt(index));
  };

  /** Get the cell parameters of the specified cell (read only).
   * @param module_index the index of the module (starting from zero).
   * @param cell_index the index of the cell on the specified module (starting from zero).
   * @return a reference of the cell parameters (read only access).
   */
  const CellParameter getCellParameter(UInt_t module_index, UInt_t cell_index) const {
    // may throw an exception 
    UInt_t index=getIndex(module_index,cell_index);

    const CellParameter * const cell_parameter=dynamic_cast<CellParameter *>(_cellParameterCol->getElementAt(index));
#ifdef BOUNDARY_CHECK
    if (!cell_parameter ) {
      throw std::logic_error("CellParameterAccess::getCellParamter> Collection contains elements which are not of type CellParameter");
    }
#endif
    return *cell_parameter;
  };

  /** Get the cell parameters of the specified cell (read/write).
   * @param index the internal index of the cell (combination of module and cell index).
   * @return a reference of the cell parameters (read/write access).
   * The internal index is composed of an offset, which depends on the module, to which
   * the cell index is added.
   */
  CellParameter getCellParameter(UInt_t index) {
#ifdef BOUNDARY_CHECK
    if (index>=static_cast<UInt_t>(_cellParameterCol->getNumberOfElements())) {
      std::stringstream message;
      message << "CellParameterAccess::getCellParameter> index (" << index << ") is out of range ("
	      <<  _cellParameterCol->getNumberOfElements() << ")";
      throw std::out_of_range(message.str());
    }
#endif    

    return CellParameter(_cellParameterCol->getElementAt(index));
  };

  /** Get the cell parameters of the specified cell (read only).
   * @param index the internal index of the cell (combination of module and cell index).
   * @return a reference of the cell parameters (read only access).
   * The internal index is composed of an offset, which depends on the module, to which
   * the cell index is added.
   */
  const CellParameter getCellParameter(UInt_t index) const {
#ifdef BOUNDARY_CHECK
    if (index>=static_cast<UInt_t>(_cellParameterCol->getNumberOfElements())) {
      std::stringstream message;
      message << "CellParameterAccess::getCellParameter> index (" << index << ") is out of range ("
	      <<  _cellParameterCol->getNumberOfElements() << ")";
      throw std::out_of_range(message.str());
    }
#endif    

    return CellParameter(_cellParameterCol->getElementAt(index));
  };
  
private:
  LCCollection *_cellParameterCol; /**< The collection of the cell parameters*/
  IntVec _indexOffset;             /**< copy of the array which contains the index offset 
				        of the modules. This is array is a prameter of the 
					collection.*/
};
}
#endif
