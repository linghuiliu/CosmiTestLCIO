#include "VirtualCellsProcessor.hh"


// Marlin includes
#include "marlin/Exceptions.h"

// CALICE includes
#include "VirtualCellsCalculator.hh"
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"

namespace CALICE {

  // generate instances of static object
  std::map<std::string, MappedContainer<VirtualCells>*> VirtualCellsProcessor::_virtualCellsContainerMap;


  MappedContainer<VirtualCells>* VirtualCellsProcessor::getVirtualCells(const std::string& processorName) {
    return _virtualCellsContainerMap[processorName];
  }

  VirtualCellsProcessor::VirtualCellsProcessor() : Processor("VirtualCellsProcessor") {

    _description = "Processor that provides a MappedContainer of VirtualCells objects";

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "Name of the CellDescriptionProcessor instance that provides the cell properties." ,
                                _cellDescriptionProcessorName,
                                std::string("MyCellDescriptionProcessor") ) ;

  }

  void VirtualCellsProcessor::init() {

    // usually a good idea
    printParameters();

    std::stringstream message;
    bool error=false;

    _mapper = MappingProcessor::getMapper(_mappingProcessorName);

    if ( ! _mapper ) {
      message << "MappingProcessor::getMapper("<< _mappingProcessorName << ") did not return a valid mapper." << std::endl;
      error = true;
    }

    _cellDescription = CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);

    if ( ! _cellDescription ) {
      message << "MappingProcessor::getMapper("<< _cellDescriptionProcessorName << ") did not return a valid mapped container" << std::endl;
      error = true;
    }

    if (error) {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

    _virtualCellsContainerMap[name()] = new MappedContainer<VirtualCells>(_mapper);
    calculateVirtualCells();

  }

  void VirtualCellsProcessor::calculateVirtualCells() {
    VirtualCellsCalculator calculator(_mapper);
    calculator.calculate(_cellDescription,10.,10.,_virtualCellsContainerMap[name()]);
  }

  void VirtualCellsProcessor::processEvent( LCEvent *evt ) {
    if (_cellDescription->getVersion() != _cellDescriptionVersion ) {
      calculateVirtualCells();
      _cellDescriptionVersion = _cellDescription->getVersion();
    }
  }

  void VirtualCellsProcessor::end() {
    delete _virtualCellsContainerMap[name()];
    _virtualCellsContainerMap.erase(name());
  }

  /* create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   */
  VirtualCellsProcessor aVirtualCellsProcessor;

} // end namespace CALICE
