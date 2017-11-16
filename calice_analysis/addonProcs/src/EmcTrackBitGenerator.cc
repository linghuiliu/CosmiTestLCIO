#include "EmcTrackBitGenerator.hh"

#include <iterator>
#include <vector>

#include "UTIL/LCTypedVector.h"
#include "EVENT/CalorimeterHit.h"
#include "CellIndex.hh"

namespace CALICE {

  EmcTrackBitGenerator aEmcTrackBitGenerator;

  EmcTrackBitGenerator::EmcTrackBitGenerator() : marlin::Processor("EmcTrackBitGenerator")  {

    registerProcessorParameter( "ECAL_HitColName" ,
				"The name of the ECAL hit collection" ,
				_emcHitColName ,
				std::string("EmcCalorimeter_Hits") );

    registerProcessorParameter( "outputParName_trackBit" ,
				"The name of the EMC track bit output parameter" ,
				_outputParName_trackBit ,
				std::string("EmcTrackBit") );

    registerProcessorParameter( "maxHits" ,
				"maximum number of hits in ECAL allowed for EmcTrackBitBit=1" ,
				_maxHits ,
				int(50) );

    registerProcessorParameter( "maxESum" ,
				"maxmum energy sum in ECAL allowed for EmcTrackBitBit=1" ,
				_maxESum ,
				float(9999999) );

  }

  void EmcTrackBitGenerator::init() {

    printParameters();

  }

  void EmcTrackBitGenerator::processRunHeader( LCRunHeader* run) {

  }

  void EmcTrackBitGenerator::processEvent( LCEvent * evt ) {

    int emcTrackBit = 0;

    int emc_nHits = 0;
    float emc_ESum = 0.;

    LCCollection* hitCol;

    try {

      hitCol = evt->getCollection( _emcHitColName );

      LCTypedVector<CalorimeterHit> emcHits( hitCol );
      LCTypedVector<CalorimeterHit>::iterator emcHitIt;

      for ( emcHitIt=emcHits.begin(); emcHitIt != emcHits.end(); emcHitIt++ ) {

	float emc_hitEnergy = (*emcHitIt)->getEnergy();

	emc_nHits++;
	emc_ESum += emc_hitEnergy;

      }
    }
    catch ( DataNotAvailableException err ) {

      streamlog_out(WARNING) <<  "EmcTrackBitGenerator WARNING: Collection "<< _emcHitColName
	   << " not available in event "<< evt->getEventNumber() << std::endl;

    }

    if( emc_nHits <= _maxHits && emc_ESum <= _maxESum ) emcTrackBit = 1;

    streamlog_out(DEBUG) <<"Event: "<< evt->getEventNumber() << ", EMC hits: "<< emc_nHits <<", EMC ESum: "<<emc_ESum<<" -> Bit "<<emcTrackBit<<std::endl;

    evt->parameters().setValue(_outputParName_trackBit,emcTrackBit);
  }

  void EmcTrackBitGenerator::end() {

  }

}
