#include "AppendIntegralObservables.hh"

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
#include "ClusterShapesTyped.hh"
#include "DecoderSet.hh"

// MarlinUtil includes
#include "ClusterShapes.h"

using namespace lcio ;
using namespace marlin ;

namespace CALICE {

  AppendIntegralObservables aAppendIntegralObservables;


  AppendIntegralObservables::AppendIntegralObservables() : Processor("AppendIntegralObservables"){

    // modify processor description
    _description = "processor for appending integral event properties";

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "HitCollection" ,
                             "Name of the inpur CalorimeterHit collection"  ,
                             _colNameIn ,
                             std::string("Calorimeter_Hits") ) ;

  }


  void AppendIntegralObservables::init() {

    streamlog_out(DEBUG) << "     MyProcessor::init()  " << std::endl ;

    // usually a good idea to
    printParameters();

    _nRun = 0 ;
    _nEvt = 0 ;

  }


  void AppendIntegralObservables::processRunHeader( LCRunHeader* run) {

    _nRun++ ;

  }


  void AppendIntegralObservables::processEvent( LCEvent * evt ) {

    // reset all integral variables
    float i_eSum  = 0.;
    int   i_nHits = 0;

    float i_cogX  = 0.;
    float i_cogY  = 0.;
    float i_cogZ  = 0.;

    float i_eSum5Layer  = 0.;

    float i_cogX5Layer  = 0.;
    float i_cogY5Layer  = 0.;
    float i_cogZ5Layer  = 0.;

    float i_radius_1 = 0.;
    float i_radius_2 = 0.;

    float i_radius5Layer_1 = 0.;
    float i_radius5Layer_2 = 0.;

    float i_length_1 = 0.;
    float i_length_2 = 0.;

    // loop over all hits, sum up parameters
    try {

      LCCollection* col = evt->getCollection( _colNameIn );

      if( col->getNumberOfElements() > 0 ){

        ClusterShapesTyped clusterAllHits;
        clusterAllHits.fill<CalorimeterHit>(col);

        // get overall integral variables
        i_eSum  = 0;
        i_nHits = 0;
        i_cogX  = clusterAllHits.getClusterShapesPointer()->getCentreOfGravity()[0];
        i_cogY  = clusterAllHits.getClusterShapesPointer()->getCentreOfGravity()[1];
        i_cogZ  = clusterAllHits.getClusterShapesPointer()->getCentreOfGravity()[2];

        // get additional variables
        LCTypedVector<CalorimeterHit> calorimeterHitsCol(col);
        LCTypedVector<CalorimeterHit>::iterator hitIt;

        DecoderSet *decoder = new DecoderSet( col->getParameters().getStringVal("CellIDEncoding") , "", "" );

        /* first loop: get COG in first five layers which is needed for later calculation of radius */
        for ( hitIt = calorimeterHitsCol.begin();  hitIt < calorimeterHitsCol.end(); hitIt++ ) {

          // calculate center of gravity in first 5 layers

          /* get k of hit from decoder
           */
          int k_pos = decoder->getKFromCellID( (*hitIt)->getCellID0() );

          /* if in first five layers, add to COG_5layers- otherwise ignore hit
           */
          if ( k_pos <= 5 && (*hitIt)->getEnergy() > 0 ) {

            i_eSum5Layer += (*hitIt)->getEnergy();

            i_cogX5Layer += (*hitIt)->getPosition()[0] * (*hitIt)->getEnergy();
            i_cogY5Layer += (*hitIt)->getPosition()[1] * (*hitIt)->getEnergy();
            i_cogZ5Layer += (*hitIt)->getPosition()[2] * (*hitIt)->getEnergy();

          }

        }

        /* normalize COG 5 layers
         */
        if ( i_eSum5Layer > 0 ) {
          i_cogX5Layer/=i_eSum5Layer;
          i_cogY5Layer/=i_eSum5Layer;
          i_cogZ5Layer/=i_eSum5Layer;
        }


        /* second loop: calc rest */
        for ( hitIt = calorimeterHitsCol.begin();  hitIt < calorimeterHitsCol.end(); hitIt++ ) {

          i_eSum += (*hitIt)->getEnergy();
          i_nHits++;

          float d_cog_x = (*hitIt)->getPosition()[0] - i_cogX;
          float d_cog_y = (*hitIt)->getPosition()[1] - i_cogY;
          float d_cog_r = sqrt( ( d_cog_x * d_cog_x ) + ( d_cog_y * d_cog_y ) );

          i_radius_1 += (*hitIt)->getEnergy() * d_cog_r;
          i_radius_2 += (*hitIt)->getEnergy() * d_cog_r * d_cog_r;

          i_length_1 += (*hitIt)->getEnergy() * (*hitIt)->getPosition()[2];
          i_length_2 += (*hitIt)->getEnergy() * (*hitIt)->getPosition()[2] * (*hitIt)->getPosition()[2];

          float d_cog_x_5layer = (*hitIt)->getPosition()[0] - i_cogX5Layer;
          float d_cog_y_5layer = (*hitIt)->getPosition()[1] - i_cogY5Layer;
          float d_cog_r_5layer = sqrt( ( d_cog_x_5layer * d_cog_x_5layer ) + ( d_cog_y_5layer * d_cog_y_5layer ) );

          i_radius5Layer_1 += (*hitIt)->getEnergy() * d_cog_r_5layer;
          i_radius5Layer_2 += (*hitIt)->getEnergy() * d_cog_r_5layer * d_cog_r_5layer;

        }
        // normalize observables
        i_radius_1 /= i_eSum;
        i_radius_2 /= i_eSum;
        i_radius_2 = sqrt( i_radius_2 - ( i_radius_1 * i_radius_1 ) );

        i_length_1 /= i_eSum;
        i_length_2 /= i_eSum;
        i_length_2 = sqrt( i_length_2 - ( i_length_1 * i_length_1 ) );

	i_radius5Layer_1 /= i_eSum;
        i_radius5Layer_2 /= i_eSum;
        i_radius5Layer_2 = sqrt( i_radius5Layer_2 - ( i_radius5Layer_1 * i_radius5Layer_1 ) );

      }
      else streamlog_out(WARNING) << "input collection " << _colNameIn << " includes no element" << std::endl;
    }
    catch ( DataNotAvailableException &err) {
      streamlog_out(WARNING) << "input collection " << _colNameIn << " not available" << std::endl;
    }

    FloatVec i_v_cogXYZ;
    i_v_cogXYZ.push_back( i_cogX );
    i_v_cogXYZ.push_back( i_cogY );
    i_v_cogXYZ.push_back( i_cogZ );

    FloatVec i_v_cogXYZ5Layer;
    i_v_cogXYZ5Layer.push_back( i_cogX5Layer );
    i_v_cogXYZ5Layer.push_back( i_cogY5Layer );
    i_v_cogXYZ5Layer.push_back( i_cogZ5Layer );

    // append integral observables to event
    evt->parameters().setValue( name()+"_eSum",  i_eSum );
    evt->parameters().setValue( name()+"_nHits", i_nHits );
    evt->parameters().setValue( name()+"_cogX",  i_cogX );
    evt->parameters().setValue( name()+"_cogY",  i_cogY );
    evt->parameters().setValue( name()+"_cogZ",  i_cogZ );
    evt->parameters().setValue( name()+"_cogX5Layer",  i_cogX5Layer );
    evt->parameters().setValue( name()+"_cogY5Layer",  i_cogY5Layer );
    evt->parameters().setValue( name()+"_cogZ5Layer",  i_cogZ5Layer );

    evt->parameters().setValue( name()+"_rad1",  i_radius_1 );
    evt->parameters().setValue( name()+"_rad2",  i_radius_2 );

    evt->parameters().setValue( name()+"_rad5Layer1",  i_radius5Layer_1 );
    evt->parameters().setValue( name()+"_rad5Layer2",  i_radius5Layer_2 );

    evt->parameters().setValue( name()+"_length1",  i_length_1 );
    evt->parameters().setValue( name()+"_length2",  i_length_2 );

    evt->parameters().setValues( name()+"_cogXYZ",  i_v_cogXYZ );
    evt->parameters().setValues( name()+"_cogXYZ5Layer",  i_v_cogXYZ5Layer );

  }


  void AppendIntegralObservables::check( LCEvent * evt ) {

  }


  void AppendIntegralObservables::end(){

  }

}
