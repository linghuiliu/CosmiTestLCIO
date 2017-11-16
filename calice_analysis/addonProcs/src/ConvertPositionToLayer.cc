#include "ConvertPositionToLayer.hh"

// ----- include for verbosity dependend logging ---------

// Marlin includes
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"

// CALICE includes
#include "AhcMapper.hh"
#include "MappingProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "CellIterator.hh"


using namespace lcio ;
using namespace marlin ;

namespace CALICE {

  ConvertPositionToLayer aConvertPositionToLayer;


  ConvertPositionToLayer::ConvertPositionToLayer() : Processor("ConvertPositionToLayer"),
                                                     _mapper(0), _cellDescriptions(0){

    // modify processor description
    _description = "processor for appending longitudinal event properties";

    registerProcessorParameter( "InputPisitionParamterName" ,
                                "Name of the input parameter with a position (x,y,z vector)"  ,
                                _parNameIn ,
                                std::string("position") ) ;

    registerProcessorParameter( "MappingProcessorName" ,
                                "name of MappingProcessor which takes care of the mapping",
                                _mappingProcessorName,
                                std::string("MappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "name of CellDescriptionProcessor which takes care of the cell description generation",
                                _cellDescriptionProcessorName,
                                std::string("CellDescriptionProcessor") ) ;

  }


  void ConvertPositionToLayer::init() {

    streamlog_out(DEBUG) << "     MyProcessor::init()  " << std::endl ;

    // usually a good idea to
    printParameters();

    bool error = false;

    /* get mapper and cell descriptions processor if specified */
    if ( parameterSet("MappingProcessorName") || parameterSet("CellDescriptionProcessorName") ) {
      // get mapper
      _mapper = dynamic_cast<const CALICE::Mapper*> ( CALICE::MappingProcessor::getMapper(_mappingProcessorName) );
      if ( ! _mapper) {
        streamlog_out(ERROR) << "Cannot obtain Mapper from MappingProcessor "<<_mappingProcessorName<<". Mapper not present or wrong type." << std::endl;
        error = true;
      }
      _mapperVersion = _mapper->getVersion();

      // get cell description
      _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
      if ( ! _cellDescriptions ) {
        streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor "<<_cellDescriptionProcessorName<<". Maybe, processor is not present" << std::endl;
        error = true;
      }

      // break if error occured
      if (error) throw StopProcessingException(this);

    }

    _nRun = 0 ;
    _nEvt = 0 ;

  }


  void ConvertPositionToLayer::processRunHeader( LCRunHeader* run) {

    _nRun++ ;

  }


  void ConvertPositionToLayer::processEvent( LCEvent * evt ) {

    // prepare vectors for event parameters
    FloatVec position_in;
    (evt->parameters()).getFloatVals(_parNameIn, position_in);

    unsigned nPositions = ( position_in.size() / 3 );
    IntVec layer_out;
    layer_out.resize( nPositions );

    /* find layer */
    // try dynamic cast of general mapper to AhcMapper
    const CALICE::AhcMapper *ahcMapper = dynamic_cast< const CALICE::AhcMapper *>( _mapper );

    /* if AHCAL mapper: get geometry information */
    if ( ahcMapper )
      {

        // get number of AHCAL layers
        unsigned nLayers = ( ahcMapper->getMaxK() - 1 ); // getMaxK() returns 39, need to subtract 1! Wrong encoding string?
        streamlog_out(DEBUG) << "number of AHCAL layers: " << nLayers << std::endl;

        for ( unsigned pos_i = 0; pos_i < nPositions; pos_i++)
          {

            float zpos_i = position_in[ pos_i*3+2 ];

            /* loop over all layers, pick layer right after end of MCParticle as shower start layer */
            CALICE::CellDescription *cell_i = NULL;
            for ( unsigned lay_i = 1; lay_i <= nLayers; lay_i++ ){

              cell_i = NULL;
              cell_i = _cellDescriptions->getByCellID(ahcMapper->getTrueCellID(ahcMapper->getTrueCellID(25,25,lay_i)));

              if ( cell_i )
                if ( cell_i->getZ() > zpos_i )
                  {
                    layer_out[pos_i] = lay_i;
                    break;
                  }
            }
          }

      }


    /* set event parameter output */
    evt->parameters().setValues( _parNameIn+"_layer", layer_out );

  }


  void ConvertPositionToLayer::check( LCEvent * evt ) {

  }


  void ConvertPositionToLayer::end(){

  }

}
