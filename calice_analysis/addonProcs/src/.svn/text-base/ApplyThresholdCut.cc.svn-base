#include "ApplyThresholdCut.hh"

#include <iostream>
#include <fstream>
#include <cmath>
#include <iterator>

#include "marlin/Exceptions.h"

#include "UTIL/LCTypedVector.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"

#include "CellIndex.hh"

namespace CALICE {

  ApplyThresholdCut aApplyThresholdCut;

  ApplyThresholdCut::ApplyThresholdCut() : marlin::Processor("ApplyThresholdCut") {


    _description = "processor for applying a cut for noise rejection";

    registerProcessorParameter( "AHC_HitColName_beforeCut" ,
                                "The name of the AHC hit collection without amplitude cut (input)" ,
                                _ahcHitColName_noCut ,
                                std::string("AhcCalorimeter_Hits_noCut") );

    registerProcessorParameter( "AHC_HitColName_afterCut" ,
                                "The name of the AHC hit collection after amplitude cut (output)" ,
                                _ahcHitColName_wCut ,
                                std::string("AhcCalorimeter_Hits") );

    registerProcessorParameter( "MipCut" ,
                                "level of noise cut ( in MIP )",
                                _mipCut,
                                float(0.5) ) ;

  }


  void ApplyThresholdCut::init() {

    printParameters();

  }

  void ApplyThresholdCut::processRunHeader( LCRunHeader* run) {

  }

  void ApplyThresholdCut::processEvent( LCEvent * evt ) {

    LCCollection* hitCol_noCut;
    LCCollection* hitCol_wCut = new LCCollectionVec( LCIO::CALORIMETERHIT );

    try {

      hitCol_noCut = evt->getCollection( _ahcHitColName_noCut );

      LCTypedVector<CalorimeterHit> ahcHits_noCut( hitCol_noCut );
      LCTypedVector<CalorimeterHit>::iterator ahcHitIt;

      for ( ahcHitIt=ahcHits_noCut.begin(); ahcHitIt != ahcHits_noCut.end(); ahcHitIt++ ) {

        if ( ( (*ahcHitIt)->getEnergy() ) >= _mipCut ) {
          streamlog_out(DEBUG)<<"Hit accepted (energy: "<< (*ahcHitIt)->getEnergy() <<" mip )"<<std::endl;

          //create copy of calorimeter hit
          CalorimeterHitImpl* hit_copy = new CalorimeterHitImpl();
          hit_copy->setEnergy( (*ahcHitIt)->getEnergy() );
          hit_copy->setEnergyError( (*ahcHitIt)->getEnergyError() );
          hit_copy->setCellID0( (*ahcHitIt)->getCellID0() );
          hit_copy->setCellID1( (*ahcHitIt)->getCellID1() );
          hit_copy->setTime( (*ahcHitIt)->getTime() );
          hit_copy->setPosition( (*ahcHitIt)->getPosition() );

          //add hit to new collection
          hitCol_wCut->addElement(hit_copy);
        }
        else
          streamlog_out(DEBUG) << "Hit rejected (energy: "<< (*ahcHitIt)->getEnergy() <<" mip )"<<std::endl;
      }

      //set flag and additional parameters (int, float, string)
      hitCol_wCut->setFlag( hitCol_noCut->getFlag() );

      StringVec parameters_intKeys;
      hitCol_noCut->parameters().getIntKeys( parameters_intKeys );
      for ( unsigned int i = 0; i < parameters_intKeys.size(); i++) {
        std::vector<int> tempVec_int;
        hitCol_noCut->parameters().getIntVals( parameters_intKeys[i], tempVec_int );
        hitCol_wCut->parameters().setValues( parameters_intKeys[i], tempVec_int );
      }

      StringVec parameters_floatKeys;
      hitCol_noCut->parameters().getFloatKeys( parameters_floatKeys );
      for ( unsigned int i = 0; i < parameters_intKeys.size(); i++) {
        std::vector<float> tempVec_float;
        hitCol_noCut->parameters().getFloatVals( parameters_floatKeys[i], tempVec_float );
        hitCol_wCut->parameters().setValues( parameters_floatKeys[i], tempVec_float );
      }

      StringVec parameters_stringKeys;
      hitCol_noCut->parameters().getStringKeys( parameters_stringKeys );
      for ( unsigned int i = 0; i < parameters_stringKeys.size(); i++) {
        std::vector<std::string> tempVec_string;
        hitCol_noCut->parameters().getStringVals( parameters_stringKeys[i], tempVec_string );
        hitCol_wCut->parameters().setValues( parameters_stringKeys[i], tempVec_string );
      }

      evt->addCollection( hitCol_wCut, _ahcHitColName_wCut );

    }
    catch ( DataNotAvailableException err ) {

      std::cout <<  "ApplyThresholdCut WARNING: Collection "<< _ahcHitColName_noCut
                << " not available in event "<< evt->getEventNumber() << std::endl;

    }

  }

  void ApplyThresholdCut::end() {

  }

}
