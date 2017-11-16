#include "AppendMCParticleInformation.hh"

// ----- include for verbosity dependend logging ---------

// Marlin includes
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"

// LCIO includes
#include "IMPL/MCParticleImpl.h"

// CALICE includes
#include "AhcMapper.hh"
#include "MappingProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "CellIterator.hh"
#include "ClusterShapesTyped.hh"


using namespace lcio ;
using namespace marlin ;

namespace CALICE {

  AppendMCParticleInformation aAppendMCParticleInformation;


  AppendMCParticleInformation::AppendMCParticleInformation() : Processor("AppendMCParticleInformation"),
                                                               _mapper(0), _cellDescriptions(0){

    // modify processor description
    _description = "processor for appending longitudinal event properties";

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "HitCollection" ,
                             "Name of the input MCParticle collection"  ,
                             _colNameIn ,
                             std::string("MCParticle") ) ;

    registerProcessorParameter( "MappingProcessorName" ,
                                "name of MappingProcessor which takes care of the mapping",
                                _mappingProcessorName,
                                std::string("MappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "name of CellDescriptionProcessor which takes care of the cell description generation",
                                _cellDescriptionProcessorName,
                                std::string("CellDescriptionProcessor") ) ;

  }


  void AppendMCParticleInformation::init() {

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


  void AppendMCParticleInformation::processRunHeader( LCRunHeader* run) {

    _nRun++ ;

  }


  void AppendMCParticleInformation::processEvent( LCEvent * evt ) {

    // prepare vectors for event parameters
    FloatVec mcpStartPos;
    mcpStartPos.resize(3,false);
    FloatVec mcpEndPos;
    mcpEndPos.resize(3,false);
    int mcpEndLayer = -1;
    int mcpPDG = -999;

    LCCollection *inColMCParticle;

    // collect information from MCParticle collection
    try {

      inColMCParticle = evt->getCollection(_colNameIn);

      IMPL::MCParticleImpl* mcp = (IMPL::MCParticleImpl*)(inColMCParticle->getElementAt(0));

      const double *MCstart = mcp->getVertex();
      const double *MCend   = mcp->getEndpoint();

      for(int i=0; i!=3; ++i) {
        mcpStartPos[i] = MCstart[i];
        mcpEndPos[i] = MCend[i];
      }

      mcpPDG = mcp->getPDG();

      /* find shower start layer */
      // try dynamic cast of general mapper to AhcMapper
      const CALICE::AhcMapper *ahcMapper = dynamic_cast< const CALICE::AhcMapper *>( _mapper );

      /* if AHCAL mapper: get geometry information */
      if ( ahcMapper )
        {

          // get number of AHCAL layers
          unsigned nLayers = ( ahcMapper->getMaxK() - 1 ); // getMaxK() returns 39, need to subtract 1! Wrong encoding string?
          streamlog_out(DEBUG) << "number of AHCAL layers: " << nLayers << std::endl;

          /* loop over all layers, pick layer right after end of MCParticle as shower start layer */
          CALICE::CellDescription *cell_i = NULL;
          for ( unsigned lay_i = 1; lay_i <= nLayers; lay_i++ ){

            cell_i = NULL;
            cell_i = _cellDescriptions->getByCellID(ahcMapper->getTrueCellID(ahcMapper->getTrueCellID(25,25,lay_i)));

            if ( cell_i )
	      if ( cell_i->getZ() > mcpEndPos[2] )
		{
		  mcpEndLayer = lay_i;
		  break;
		}
          }

        }

    }
    catch ( DataNotAvailableException &err) {
      streamlog_out(WARNING) << "input collection " << _colNameIn << " not available" << std::endl;
    }

    // append longitudinal profile variables to event
    evt->parameters().setValues( name()+"_startPos", mcpStartPos );
    evt->parameters().setValues( name()+"_endPos", mcpEndPos );
    evt->parameters().setValue( name()+"_endLay", mcpEndLayer );
    evt->parameters().setValue( name()+"_PDG", mcpPDG );

  }


  void AppendMCParticleInformation::check( LCEvent * evt ) {

  }


  void AppendMCParticleInformation::end(){

  }

}
