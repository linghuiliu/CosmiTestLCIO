#include "CellNeighboursProcessor.hh"


// Marlin includes
#include "marlin/Exceptions.h"

// CALICE includes
#include "CellNeighbourCalculator.hh"
#include "MappingProcessor.hh"

namespace CALICE {

  // generate instances of static object
  std::map<std::string, MappedContainer<CellNeighbours>*> CellNeighboursProcessor::_cellNeighboursContainerMap;
  std::map<std::string, std::string > CellNeighboursProcessor::_cellNeighboursEncodingStringMap;

  MappedContainer<CellNeighbours>* CellNeighboursProcessor::getNeighbours(const std::string& processorName) {
    return _cellNeighboursContainerMap[processorName];
  }

  const std::string& CellNeighboursProcessor::getEncodingString(const std::string& processorName) {
    return _cellNeighboursEncodingStringMap[processorName];
  }

  CellNeighboursProcessor::CellNeighboursProcessor() : Processor("CellNeighboursProcessor") {

    _description = "Processor that provides a MappedContainer of CellNeighbours objects";

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

  }

  void CellNeighboursProcessor::init() {

    // usually a good idea
    printParameters();

    std::stringstream message;
    bool error=false;

    _mapper = MappingProcessor::getMapper(_mappingProcessorName);

    if ( ! _mapper ) {

      message << "MappingProcessor::getMapper("<< _mappingProcessorName << ") did not return a valid mapper." << std::endl;
      error = true;
    }

    if (error) {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

    _cellNeighboursContainerMap[name()] = new MappedContainer<CellNeighbours>(_mapper);
    _mapperVersion = _mapper->getVersion();
    generateNeighbours();

  }

  void CellNeighboursProcessor::generateNeighbours() {
    CellNeighbourCalculator calculator(_mapper);
    calculator.getNeighbours(_cellNeighboursContainerMap[name()]);
    _cellNeighboursEncodingStringMap[name()] = _mapper->getDecoder()->getCellIDEncoding();
  }

  void CellNeighboursProcessor::processEvent( LCEvent *evt ) {
    if (_mapper->getVersion() != _mapperVersion ) {
      generateNeighbours();
      _mapperVersion = _mapper->getVersion();
    }
  }

  void CellNeighboursProcessor::end() {
    delete _cellNeighboursContainerMap[name()];
    _cellNeighboursContainerMap.erase(name());
  }

  /* create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   */
  CellNeighboursProcessor aCellNeighboursProcessor;

} // end namespace CALICE
