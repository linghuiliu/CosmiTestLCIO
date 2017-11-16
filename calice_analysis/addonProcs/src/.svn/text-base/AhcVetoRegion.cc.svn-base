#include "AhcVetoRegion.hh"

#include <iterator>
#include <vector>
#include <cmath>
#include <algorithm>

#include "marlin/Exceptions.h"

#include "UTIL/LCTypedVector.h"
#include "EVENT/CalorimeterHit.h"

#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"


namespace CALICE {

  AhcVetoRegion aAhcVetoRegion;

  AhcVetoRegion::AhcVetoRegion() : marlin::Processor("AhcVetoRegion"),
                                   _mapper(0), _cellDescriptions(0) {

    _description=( "Processor generating a Bit absed on energy and number of hits in a defined region of intrerest in the AHCAL" );


    registerProcessorParameter( "AHC_HitColName" ,
                                "The name of the AHC hit collection" ,
                                _ahcHitColName ,
                                std::string("AhcCalorimeter_Hits") );

    registerProcessorParameter( "MappingProcessorName" ,
                                "name of MappingProcessor which takes care of the mapping",
                                _mappingProcessorName,
                                std::string("AhcMappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "name of CellDescriptionProcessor which takes care of the cell description generation",
                                _cellDescriptionProcessorName,
                                std::string("AhcCellDescriptionProcessor") ) ;

    _hitThreshold.push_back(0.5);
    registerProcessorParameter( "HitThreshold" ,
                                "energy threshold for hits [mip]" ,
                                _hitThreshold ,
                                _hitThreshold );

    _roiFirstLayer.push_back(1);
    registerProcessorParameter( "roiFirstLayer" ,
                                "first layer of the AHC region-of-interest" ,
                                _roiFirstLayer ,
                                _roiFirstLayer );

    _roiLastLayer.push_back(38);
    registerProcessorParameter( "roiLastLayer" ,
                                "last layer of the AHC region-of-interest" ,
                                _roiLastLayer ,
                                _roiLastLayer );

    _roiCells.push_back(21);
    registerProcessorParameter( "roiCells" ,
                                "cells included in region-of-interest: 3 - only 3x3 cells, 6 - only 6x6 cells, 12 - only 12x12 cells, 9 - 3x3 and 6x6 cells, 15 - 3x3 and 12x12 cells, 18 - 6x6 and 12x12 cells, 21 - all cells" ,
                                _roiCells ,
                                _roiCells );

    _roiInnerRadius.push_back(0.);
    registerProcessorParameter( "roiInnerRadius" ,
                                "only cells beyond this minimum distance to the nominal center (in mm) are assigned to the region-of-interest" ,
                                _roiInnerRadius ,
                                _roiInnerRadius );

    _roiOuterRadius.push_back(5000.);
    registerProcessorParameter( "roiOuterRadius" ,
                                "only cells closer than this maximum distance to the nominal center (in mm) are assigned to the region-of-interest" ,
                                _roiOuterRadius ,
                                _roiOuterRadius );

    _roiInnerSideLength.push_back(0.);
    registerProcessorParameter( "roiInnerSideLength" ,
                                "only cells beyond a square with this side length (in mm) around the center are assigned to the region-of-interest" ,
                                _roiInnerSideLength ,
                                _roiInnerSideLength );

    _roiOuterSideLength.push_back(5000.);
    registerProcessorParameter( "roiOuterSideLength" ,
                                "only cells inside a square with this side length (in mm) around the center are assigned to the region-of-interest" ,
                                _roiOuterSideLength ,
                                _roiOuterSideLength );

    _minHits.push_back(10);
    registerProcessorParameter( "minHits" ,
                                "minimum number of hits in AHC region-of-interest to set AhcBit=1" ,
                                _minHits ,
                                _minHits );

    _minESum.push_back(0);
    registerProcessorParameter( "minESum" ,
                                "minimum energy sum in AHC region-of-interest to set AhcBit=1" ,
                                _minESum ,
                                _minESum );

  }

  void AhcVetoRegion::init() {

    printParameters();

    /* set ceneter of ROI to module center of AHCAL */
    //_roiCenter[0]=450;
    //_roiCenter[1]=450;

    /* set ceneter of ROI to 0 of coordinate system */
    _roiCenter[0]=0;
    _roiCenter[1]=0;

    bool error = false;

    if ( _hitThreshold.size() != _roiCells.size() || \
         _hitThreshold.size() != _roiFirstLayer.size() || \
         _hitThreshold.size() != _roiLastLayer.size() || \
         _hitThreshold.size() != _roiInnerRadius.size() || \
         _hitThreshold.size() != _roiOuterRadius.size() || \
         _hitThreshold.size() != _roiInnerSideLength.size() || \
         _hitThreshold.size() != _roiOuterSideLength.size() || \
         _hitThreshold.size() != _minHits.size() || \
         _hitThreshold.size() != _minESum.size() ) {
      streamlog_out(ERROR) << "Parameters need to have the same number of elements." << std::endl
                   << "HitThreshold has       " << _hitThreshold.size() << " elements" << std::endl
                   << "roiCells has           " << _roiCells.size() << " elements" << std::endl
                   << "roiFirstLayer has      " << _roiFirstLayer.size() << " elements" << std::endl
                   << "roiInnerRadius has     " << _roiInnerRadius.size() << " elements" << std::endl
                   << "roiOuterRadius has     " << _roiOuterRadius.size() << " elements" << std::endl
                   << "roiInnerSideLength has " << _roiInnerSideLength.size() << " elements" << std::endl
                   << "roiOuterSideLength has " << _roiOuterSideLength.size() << " elements" << std::endl
                   << "minHits has            " << _minHits.size() << " elements" << std::endl
                   << "minESum has            " << _minESum.size() << " elements" << std::endl;
      error = true;
    }

    _mapper = dynamic_cast<const CALICE::AhcMapper*> ( CALICE::MappingProcessor::getMapper(_mappingProcessorName) );
    if ( ! _mapper) {
      streamlog_out(ERROR) << "Cannot obtain AhcMapper from MappingProcessor "<<_mappingProcessorName<<". Mapper not present or wrong type." << std::endl;
      error = true;
    }
    _mapperVersion = _mapper->getVersion();


    _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions ) {
      streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor "<<_cellDescriptionProcessorName<<". Maybe, processor is not present" << std::endl;
      error = true;
    }

    if (error) throw StopProcessingException(this);

  }

  void AhcVetoRegion::processRunHeader( LCRunHeader* run) {

    run->parameters().setValues(name()+"_hitThresholds",_hitThreshold);
    run->parameters().setValues(name()+"_roiCells",_roiCells);

    run->parameters().setValues(name()+"_roiFirstLayer",_roiFirstLayer);
    run->parameters().setValues(name()+"_roiLastLayer",_roiLastLayer);

    run->parameters().setValues(name()+"_roiInnerRadius",_roiInnerRadius);
    run->parameters().setValues(name()+"_roiOuterRadius",_roiOuterRadius);

    run->parameters().setValues(name()+"_roiInnerSideLength",_roiInnerSideLength);
    run->parameters().setValues(name()+"_roiOuterSideLength",_roiOuterSideLength);

    run->parameters().setValues(name()+"_minHits",_minHits);
    run->parameters().setValues(name()+"_minESum",_minESum);

  }

  void AhcVetoRegion::processEvent( LCEvent * evt ) {


    /* number of different configurations checked */
    unsigned nConfig = _hitThreshold.size();

    /* prepare result vectors for all configurations */
    IntVec countConfig;
    IntVec ahcBit;
    IntVec ahc_nHits;
    FloatVec ahc_ESum;

    /* get hits from input collection */
    LCCollection* hitCol;

    try {

      hitCol = evt->getCollection( _ahcHitColName );

      _cellDescriptions->getDecoder()->setCellIDEncoding( hitCol->getParameters().getStringVal("CellIDEncoding") );

      // get decoder for correct I,J,K values
      //      CALICE::FastDecoder * decoder_i = CALICE::FastDecoder::generateDecoder( hitCol->getParameters().getStringVal("CellIDEncoding"), "I");
      //      CALICE::FastDecoder * decoder_j = CALICE::FastDecoder::generateDecoder( hitCol->getParameters().getStringVal("CellIDEncoding"), "J");
      CALICE::FastDecoder * decoder_k = CALICE::FastDecoder::generateDecoder( hitCol->getParameters().getStringVal("CellIDEncoding"), "K");

      LCTypedVector<CalorimeterHit> ahcHits( hitCol );

      /* loop over all configurations, get energy and hits in veto region, check if sums are above limits */
      for ( unsigned conf = 0; conf < nConfig; conf++ ) {

        countConfig.push_back(conf+1);
        ahcBit.push_back(0);
        ahc_nHits.push_back(0);
        ahc_ESum.push_back(0);

        std::vector<float> v_allowedCellSizes;
        switch ( _roiCells[conf] )
          {
          case 3:
            v_allowedCellSizes.push_back(30);
            break;

          case 6:
            v_allowedCellSizes.push_back(60);
            break;

          case 12:
            v_allowedCellSizes.push_back(120);
            break;

          case 9:
            v_allowedCellSizes.push_back(30);
            v_allowedCellSizes.push_back(60);
            break;

          case 15:
            v_allowedCellSizes.push_back(30);
            v_allowedCellSizes.push_back(120);
            break;

          case 18:
            v_allowedCellSizes.push_back(60);
            v_allowedCellSizes.push_back(120);
            break;

          case 21:
            v_allowedCellSizes.push_back(30);
            v_allowedCellSizes.push_back(60);
            v_allowedCellSizes.push_back(120);
            break;

          }

        /* create hit iterator, iterate over all hits */
        LCTypedVector<CalorimeterHit>::iterator ahcHitIt;

        for ( ahcHitIt=ahcHits.begin(); ahcHitIt != ahcHits.end(); ahcHitIt++ ) {

          int hit_cellID = (*ahcHitIt)->getCellID0();

          //int hit_i = decoder_i->decodeU( (*ahcHitIt)->getCellID0() );
          //int hit_j = decoder_j->decodeU( (*ahcHitIt)->getCellID0() );
          int hit_k = decoder_k->decodeU( (*ahcHitIt)->getCellID0() );

          float hit_pos[2] = { (*ahcHitIt)->getPosition()[0], (*ahcHitIt)->getPosition()[1] };

          float hit_energy = (*ahcHitIt)->getEnergy();

          if ( hit_energy < _hitThreshold[conf] ) continue;

          CALICE::CellDescription* hit_cellDescription = _cellDescriptions->getByCellID(hit_cellID);
          double hit_cellSize_xy[2]  = {hit_cellDescription->getSizeX(),hit_cellDescription->getSizeY()};

          /* calc position wrt center in global coordinates */
          float hit_dx = sqrt( pow( ( hit_pos[0] - _roiCenter[0] ), 2 ) );
          float hit_dy = sqrt( pow( ( hit_pos[1] - _roiCenter[1] ), 2 ) );

          float hit_radius = sqrt( pow( hit_dx, 2 ) + pow( hit_dy, 2 ) );

          int hit_layer = hit_k;

          /* hit lies within region of interest if all of these conditions are met:
           *
           * condition_a) cell size in vector of allowed cell sizes
           *
           * condition_b) first layer <= hit layer <= maximum layer of ROI
           *
           * condition_c) distance ( x_hit -> x_roi_center ) + 1/2 cell size > 1/2 inner side length of ROI -OR- (same for y)
           *
           * condition_d) distance ( x_hit -> x_roi_center ) - 1/2 cell size <= 1/2 outer side length of ROI -OR- (same for y)
           *
           * condition_e) radius ( hit -> roi_center ) + 1/2 cell size * sqrt(2) > inner radius of ROI
           *
           * condition_f) radius ( hit -> roi_center ) - 1/2 cell size * sqrt(2) < outer radius of ROI
           *
           */

          bool condition_a = false;
          if ( std::binary_search (v_allowedCellSizes.begin(), v_allowedCellSizes.end(),  hit_cellSize_xy[0]) )
            condition_a = true;

          bool condition_b = false;
          if ( hit_layer >= _roiFirstLayer[conf] && hit_layer <= _roiLastLayer[conf] )
            condition_b = true;

          bool condition_c = false;
          if ( ( hit_dx + ( hit_cellSize_xy[0] / 2. ) ) > ( (float)_roiInnerSideLength[conf]/2. ) || \
               ( hit_dy + ( hit_cellSize_xy[0] / 2. ) ) > ( (float)_roiInnerSideLength[conf]/2. ) )
            condition_c = true;

          bool condition_d = false;
          if ( ( hit_dx - ( hit_cellSize_xy[0] / 2. ) ) <= ( (float)_roiOuterSideLength[conf]/2. ) || \
               ( hit_dy - ( hit_cellSize_xy[0] / 2. ) ) <= ( (float)_roiOuterSideLength[conf]/2. ) )
            condition_d = true;

          bool condition_e = false;
          if ( ( hit_radius + ( hit_cellSize_xy[0] * sqrt(2.) ) ) > _roiInnerRadius[conf] )
            condition_e = true;

          bool condition_f = false;
          if ( ( hit_radius - ( hit_cellSize_xy[0] * sqrt(2.) ) ) <= _roiOuterRadius[conf] )
            condition_f = true;


          if ( condition_a && condition_b && condition_c && condition_d && condition_e && condition_f ) {

            ahc_nHits[conf]++;
            ahc_ESum[conf] += hit_energy;

          }

        }

        if( ahc_nHits[conf] >= _minHits[conf] && ahc_ESum[conf] >= _minESum[conf] ) ahcBit[conf] = 1;

      }

    }
    catch ( DataNotAvailableException err ) {

      std::cout <<  "AhcVetoRegion WARNING: Collection "<< _ahcHitColName
                << " not available in event "<< evt->getEventNumber() << std::endl;

    }

    evt->parameters().setValues(name()+"_config",countConfig);
    evt->parameters().setValues(name()+"_Bit",ahcBit);
    evt->parameters().setValues(name()+"_ESum",ahc_ESum);
    evt->parameters().setValues(name()+"_nHits",ahc_nHits);

  }

  void AhcVetoRegion::end() {

  }

}
