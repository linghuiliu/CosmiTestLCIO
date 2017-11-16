#include "CellIterator.hh"

#include "Mapper.hh"

namespace CALICE {

  CellIterator::CellIterator(const Mapper* mapper, const unsigned int index) : _mapper(mapper), _index(index) {

    _currentCellID = 0;

    if (_index == mapper->getMaxIndex() ) return; // special one after the end element

    try {
      _currentCellID = mapper->getCellIDOfIndex(_index);
    }
    catch (BadDataException &error) {
      ++(*this);
    }
  }

  void CellIterator::operator ++ () {

    bool validCell = false;

    while ( ! ( validCell || _index >= _mapper->getMaxIndex() ) ) {

      try {
        _currentCellID = _mapper->getCellIDOfIndex(++_index);
        validCell = true;
      }
      catch (BadDataException &error) {
      }

    }

    /* If mapping has changed and we are behind the one after the end element,
     * fix to new one after the end element.
     */
    if (_index > _mapper->getMaxIndex() ) _index = _mapper->getMaxIndex();
  }


} //end namespsace CALICE
