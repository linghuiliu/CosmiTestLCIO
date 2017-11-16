#ifndef AdcValueAccess_hh
#define AdcValueAccess_hh

#ifdef BOUNDARY_CHECK
#include <stdexcept>
#endif
#include <iostream>

#include <utility>

#include <AdcBlock.hh>
#include <MappingAndAlignment.hh>
#include <Calibration.hh>
#include <EVENT/LCCollection.h>

#include <CellParameterArray_t.hh>
#include <BoardID.hh>

class Calibration;

namespace CALICE {

typedef unsigned int UInt_t;
  typedef float Float_t;

  /** Hide hit reconstruction: ADC value to position mapping, calibration.
   * The ADC collection must not be empty.
   */
  class AdcValueAccess
  {
  public:
    AdcValueAccess(const LCCollection *adc_collection, const MappingAndAlignment *alignment, const Calibration *calibration, CellParameterArray_t *cell_parameter) 
      : _adcCollection(adc_collection),
	_currentAdcBlock(_adcCollection->getElementAt(0)), //Must be initialsised with a dummy element to prevent a memory leak.
	_mapping(alignment),
	_calibration(calibration),
	_cellParameter(cell_parameter)

    
      //  ,_nBlocks(_adcCollection->getNumberOfElements())
    {

      //    for (UInt_t block_i=0; block_i<(UInt_t) _adcCollection->getNumberOfElements(); block_i++) {
      //      _currentAdcBlock=AdcBlock(_adcCollection->getElementAt(block_i));
      //      std::cout << "access block=" << block_i
      //		<<  " crate=" << _currentAdcBlock.getCrateID()
      //		<<  " slot=" << _currentAdcBlock.getSlotID()
      //		<<  " fe=" << _currentAdcBlock.getBoardFrontEnd()
      //		<<  " label=" << _currentAdcBlock.getBoardLabel()
      //		<< std::endl;
      //      
      //    }

      setToFirstBlock();
    };

    UInt_t getNBlocks() const {
      return _adcCollection->getNumberOfElements()  ;
      //    return _nBlocks;
    };

    UInt_t getNValuesPerBlock() const {
      return _nValuesPerBlock;
    };
  
    void setToFirstBlock() 
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (getNBlocks()<=0) {
	throw std::range_error("AdcValueAccess::setToFirstBlock> There are no adc blocks.");
      }
#endif
      _useRightSide=false;
      _nValuesPerBlock=0;
      _blockIndex=(unsigned int) -1;
      nextBlock();
    };

    bool hasConnectedBlocks() {
      return _blockIndex<getNBlocks();
    };
    
    bool nextBlock()
    {
      if (!_useRightSide) {
	do {
	  if (++_blockIndex>=getNBlocks()) return false;
#ifdef RECO_DEBUG
	  std::cout << "access block=" << _blockIndex << "/" << getNBlocks() << " line=" << _lineIndex <<std::endl;
#endif
	  _currentAdcBlock=AdcBlock(_adcCollection->getElementAt(_blockIndex));
	  _moduleIndices=_mapping->getModuleIndex(_currentAdcBlock.getCrateID(),_currentAdcBlock.getSlotID(),_currentAdcBlock.getBoardFrontEnd());
	} while ( (_moduleIndices.first>=UINT_MAX && _moduleIndices.second>=UINT_MAX) || _currentAdcBlock.getMultiplexPosition()>=18);

	if (_moduleIndices.first<UINT_MAX) {
	  _lineIndex=_mapping->getLineIndexOfLeftSide();
	  _cellIndex.first=_moduleIndices.first;
	  _cellIndex.second=_mapping->getCellIndex(_cellIndex.first, _currentAdcBlock.getMultiplexPosition(),0)-_lineIndex;
	  _useRightSide=_mapping->hasRightSide(_moduleIndices);
	  _nValuesPerBlock=_mapping->getNumberOfLines(_moduleIndices,false);
	  return true;
	}
      } 
      _lineIndex=_mapping->getLineIndexOfRightSide();
      _cellIndex.first=_moduleIndices.second;
      _cellIndex.second=_mapping->getCellIndex(_cellIndex.first, _currentAdcBlock.getMultiplexPosition(),0)-_lineIndex;
      _useRightSide=false;
      _nValuesPerBlock=_mapping->getNumberOfLines(_moduleIndices,true);
      return true;
    };

    //    void setToFirstValue() {
    //      _lineIndex=0;
    //    };

    bool nextValue() {
      return ++_lineIndex<_nValuesPerBlock;
    };

    int getAdcValue() const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (_lineIndex>=_nValuesPerBlock) {
	throw std::range_error("AdcValueAccess::getAdcValueOfCurrentBlock> value index out of bounds");
      }
#endif
      return _currentAdcBlock.getAdcVal(_lineIndex);
    };

    Float_t getCalibratedValue(Float_t pedestal) const {
      // ---- DEBUG:
      //     CellIndex index(getGeometricalCellIndex());
      //     std::cout << "l:" << index.getLayerIndex() << " pr:" << index.getPadRow() << " pc:" << index.getPadColumn() 
      // 	      << " wr:" << index.getWaferRow() << " wl:" << index.getWaferColumn()
      // 	      << " mod=" << _cellIndex.first << " cell=" << _cellIndex.second+_lineIndex
      // 	      << "  id=" << _mapping->getModuleID(_cellIndex.first)
      // 	      << " cal =" << _calibration->getCalibratedValue(_mapping->getModuleID(_cellIndex.first),
      // 					   _mapping->getModuleType(_cellIndex.first),
      // 					   _cellIndex.second+_lineIndex,
      // 					   1.)
      //	      << std::endl;
      // ___DEBUG___

      return _calibration->getCalibratedValue(_mapping->getModuleID(_cellIndex.first),
					      _mapping->getModuleType(_cellIndex.first),
					      _cellIndex.second+_lineIndex,
					      getAdcValue()-pedestal);
    };

    // FIXME: Change to unsigned int ?
    int getModuleIndex() const {
      return _cellIndex.first;
    };

    // FIXME: Change to unsigned int ?
    int getCellIndexOnModule() const {
      // TODO: verify that +_lineIndex is correct.
      return _cellIndex.second+_lineIndex;
    };

    //  int getLayerIndex() const {
    //    return _mapping->getLayerIndex(_cellIndex.first,_cellIndex.second);
    //  };

    // FIXME: Change to unsigned int ?
    int getGeometricalCellIndex() const {
      return _mapping->getGeometricalCellIndex(_cellIndex.first,_cellIndex.second+_lineIndex);
    };

    ThreeVector_t getPosition() const {
      return _mapping->getPosition(_cellIndex.first,_cellIndex.second+_lineIndex);
    };

    CellParameter &getParameter() {
      return ((*_cellParameter)[_cellIndex.first])[_cellIndex.second+_lineIndex];
    };

    const CellParameter &getParameter() const {
      return ((*_cellParameter)[_cellIndex.first])[_cellIndex.second+_lineIndex];
    };

    CALICE::BoardID getBoardID() const {
      return CALICE::BoardID(_currentAdcBlock.getBoardID());
    }

    UInt_t getBoardFrontEnd() const {
      return _currentAdcBlock.getBoardFrontEnd();
    }

    UInt_t getSlotID() const {
      return _currentAdcBlock.getSlotID();
    }

    bool isRightSide() const {
      return _lineIndex >= _mapping->getLineIndexOfRightSide();
    }

    //private:
    //  unsigned int _nBlocks;
    UInt_t _nValuesPerBlock;
    bool _useRightSide;

    UInt_t _blockIndex;
    UInt_t _lineIndex;
    std::pair<UInt_t,UInt_t> _moduleIndices;
    std::pair<UInt_t, UInt_t> _cellIndex;

    const LCCollection *_adcCollection;
    AdcBlock      _currentAdcBlock;

    const MappingAndAlignment     *_mapping;
    const Calibration   *_calibration; 
    CellParameterArray_t  *_cellParameter;
  };


}
#endif
