#include "AppendLongitudinalObservables.hh"

// ----- include for verbosity dependend logging ---------

// Marlin includes
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"

// LCIO includes
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCTOOLS.h"
#include "EVENT/CalorimeterHit.h"

#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCCollectionVec.h"

// CALICE includes
#include "AhcMapper.hh"
#include "MappingProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "CellIterator.hh"
#include "ClusterShapesTyped.hh"

// MarlinUtil includes
#include "ClusterShapes.h"

using namespace lcio ;
using namespace marlin ;

namespace CALICE {

  AppendLongitudinalObservables aAppendLongitudinalObservables;


  AppendLongitudinalObservables::AppendLongitudinalObservables() : Processor("AppendLongitudinalObservables"),
                                                                   _mapper(0), _cellDescriptions(0){

    // modify processor description
    _description = "processor for appending longitudinal event properties";

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "HitCollection" ,
                             "Name of the inpur CalorimeterHit collection"  ,
                             _colNameIn ,
                             std::string("Calorimeter_Hits") ) ;

    registerProcessorParameter( "MappingProcessorName" ,
                                "name of MappingProcessor which takes care of the mapping",
                                _mappingProcessorName,
                                std::string("MappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "name of CellDescriptionProcessor which takes care of the cell description generation",
                                _cellDescriptionProcessorName,
                                std::string("CellDescriptionProcessor") ) ;

    registerProcessorParameter( "ShowerStartPositionParameterName" ,
                                "name of the event parameter (float vector) which contains the shower start position",
                                _parNameShowerStartPos,
                                std::string("showerStartPos") ) ;

    registerProcessorParameter( "CenterOfGravityXYZParameterName" ,
                                "name of the event parameter (float vector) which contains the center of gravity position",
                                _parNameCOG,
                                std::string("cogXYZ") ) ;

  }


  void AppendLongitudinalObservables::init() {

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


  void AppendLongitudinalObservables::processRunHeader( LCRunHeader* run) {

    _nRun++ ;

  }


  void AppendLongitudinalObservables::processEvent( LCEvent * evt ) {

    // longitudinal profiles
    // prepare vector for bin centers (z); use fake initial values that should cover all geometries
    int nLayers = 1;
    float front_center_z = 0;
    float back_center_z = 0;
    float binWidthZ = 120000;

    FloatVec binCenterZ;

    // try dynamic cast of general mapper to AhcMapper
    const CALICE::AhcMapper *ahcMapper = dynamic_cast< const CALICE::AhcMapper *>( _mapper );

    /* if AHCAL mapper: get geometry information */
    if ( ahcMapper )
      {

        // get number of AHCAL layers
        nLayers = ( ahcMapper->getMaxK() - 1 ); // getMaxK() returns 39, need to subtract 1! Wrong encoding string?
        streamlog_out(DEBUG) << "number of AHCAL layers: " << nLayers << std::endl;

        // find bin width for Ahcal z-bins and fill bin center vector
        CALICE::CellDescription *cell_front_ll = _cellDescriptions->getByCellID(ahcMapper->getTrueCellID(ahcMapper->getTrueCellID(25,25,1)));
        CALICE::CellDescription *cell_front_ur = _cellDescriptions->getByCellID(ahcMapper->getTrueCellID(ahcMapper->getTrueCellID(61,61,1)));

        CALICE::CellDescription *cell_back_ll = _cellDescriptions->getByCellID(ahcMapper->getTrueCellID(ahcMapper->getTrueCellID(25,25,nLayers)));
        CALICE::CellDescription *cell_back_ur = _cellDescriptions->getByCellID(ahcMapper->getTrueCellID(ahcMapper->getTrueCellID(61,61,nLayers)));

        front_center_z = 0.5 * ( cell_front_ll->getZ() + cell_front_ur->getZ() );
        back_center_z = 0.5 * ( cell_back_ll->getZ() + cell_back_ur->getZ() );

        // calculate bin width in z and fill vector with bin centers
        binWidthZ = ( back_center_z - front_center_z ) / ( nLayers - 1 );

      }

    // continue creating vector for z-bins
    for ( int i = 1; i <= nLayers; i++ ) {
      binCenterZ.push_back( front_center_z + ( i - 1 ) * binWidthZ );
    }

    // prepare vector for eSum and hits per zBin, set initial values to 0
    FloatVec i_eSumPerZBin;
    IntVec i_nHitsPerZBin;
    FloatVec i_radiusPerZBin;

    for ( unsigned int i = 0; i < binCenterZ.size(); i++ ) {
      i_eSumPerZBin.push_back(0.);
      i_nHitsPerZBin.push_back(0);
      i_radiusPerZBin.push_back(0.);
    }


    // loop over all hits, sum up parameters
    try {

      LCCollection* col = evt->getCollection( _colNameIn );

      /* get reference center of gravity */
      FloatVec cogPos;
      col->parameters().getFloatVals( _parNameCOG , cogPos );
      if ( cogPos.size() < 2 ) {
	cogPos.clear();
	cogPos.resize(2,0.);
      }

      if( col->getNumberOfElements() > 0 ){

        // if mapper available: make sure to use the right encoding for the collection just fetched
        if ( _mapper ){
          _cellIDEncoding = col->getParameters().getStringVal("CellIDEncoding");
          _mapper->getDecoder()->setCellIDEncoding( _cellIDEncoding );
        }

        LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

        // loop over all hits to get longitudinal profiles
        for (LCTypedVector<CalorimeterHit>::iterator iter = calorimterHitsCol.begin();iter != calorimterHitsCol.end();++iter) {

          // calculate longitudinal observables, get z-position
          float energy = (*iter)->getEnergy();
          float zpos   = (*iter)->getPosition()[2];

          /* calculate position of hit w.r.t. center of gravity */
          float d_cog_x = (*iter)->getPosition()[0] - cogPos[0];
          float d_cog_y = (*iter)->getPosition()[1] - cogPos[1];
          float d_cog_r = sqrt( ( d_cog_x * d_cog_x ) + ( d_cog_y * d_cog_y ) );

          // search z-bin for hit
          bool searchBin = true;
          unsigned bini  = 0;

          while ( searchBin ) {

            // dump z-bin ranges and throw exception if hit could not be assinged to any valid bin -> wrong mapping?
            if (bini >= binCenterZ.size() ) {
              streamlog_out(ERROR) << "Hit at z = " << zpos << " could not be assigned to any z-bin!" << std::endl;

              streamlog_out(DEBUG) << binCenterZ.size() << " bins from " << binCenterZ[0] << " to " << binCenterZ[(binCenterZ.size()-1)] << " , bin width = " << binWidthZ << std::endl;
              for ( unsigned i = 0; i <  binCenterZ.size(); i++ ) {
                streamlog_out(DEBUG) << " -> bin " << i << " from " << binCenterZ[i] - ( binWidthZ / 2) << " to " << binCenterZ[i] + ( binWidthZ / 2) << std::endl;
              }

              throw StopProcessingException(this);
            }

            // if correct bin found -> fill hit infomration to bin
            if ( zpos >= binCenterZ[bini] - ( binWidthZ / 2) && zpos <= binCenterZ[bini] + ( binWidthZ / 2) ) {

              i_eSumPerZBin[bini] += energy;
              i_nHitsPerZBin[bini]++;

              i_radiusPerZBin[bini] += energy * d_cog_r;

              searchBin = false;

            }

            bini++;

          }

        }

	/* normalize radius per layer */
	for ( unsigned i = 0; i < i_radiusPerZBin.size(); i++ )
	  i_radiusPerZBin[i] /= i_eSumPerZBin[i];

      }
      else streamlog_out(WARNING) << "input collection " << _colNameIn << " includes no element" << std::endl;
    }
    catch ( DataNotAvailableException &err) {
      streamlog_out(WARNING) << "input collection " << _colNameIn << " not available" << std::endl;
    }

    // append longitudinal profile variables to event
    evt->parameters().setValues( name()+"_binCenterZ", binCenterZ );
    evt->parameters().setValues( name()+"_eSumPerZBin", i_eSumPerZBin );
    evt->parameters().setValues( name()+"_nHitsPerZBin", i_nHitsPerZBin );
    evt->parameters().setValues( name()+"_radiusPerZBin", i_radiusPerZBin );

  }


  void AppendLongitudinalObservables::check( LCEvent * evt ) {

  }


  void AppendLongitudinalObservables::end(){

  }

}
