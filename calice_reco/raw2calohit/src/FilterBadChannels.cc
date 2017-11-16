#include "FilterBadChannels.hh"
#include "marlin/ConditionsProcessor.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCCollection.h"
#include "EVENT/LCParameters.h"
#include "IMPL/LCCollectionVec.h"

namespace CALICE
{

FilterBadChannels aFilterBadChannels;

FilterBadChannels::FilterBadChannels() : marlin::Processor("FilterBadChannels"){
  registerInputCollection( LCIO::CALORIMETERHIT,
			   "InputCollection",
			   "Name of input hit collection",
			   _inColName,
			   std::string( "CalorimeterHits" ) );

  registerOutputCollection( LCIO::CALORIMETERHIT,
			    "OutputCollection",
			    "Name of filtered hit collection",
			    _outColName,
			    std::string( "FilteredCalorimeterHits" ) );

  registerInputCollection( LCIO::LCGENERICOBJECT,
			   "CellQuality",
			   "Collection with qiuality flags",
			   _statusColName,
			   std::string( "CellQuality" ) );
  _description = "Filter hits from bad cells out of list of CalorimeterHits";

}

FilterBadChannels::~FilterBadChannels(){
}

void FilterBadChannels::init(){
  printParameters();

  _statusMap = new Map_t( &CellQuality::getCellID );

  marlin::ConditionsProcessor::registerChangeListener( _statusMap, 
						       _statusColName );
}

void FilterBadChannels::processEvent(LCEvent *evt){
  EVENT::LCCollection* inCol = 0;
  try {
    inCol = evt->getCollection( _inColName );
  } catch ( lcio::DataNotAvailableException &err ) {
  }
  
  if ( inCol ) {
    IMPL::LCCollectionVec* outCol = 
      new IMPL::LCCollectionVec( LCIO::CALORIMETERHIT );
    outCol->setFlag( inCol->getFlag() );
    outCol->setSubset( true );
    std::string encStr = 
      inCol->getParameters().getStringVal( LCIO::CellIDEncoding ) ;
    outCol->parameters().setValue( LCIO::CellIDEncoding, encStr );
    for ( int i=0; i!= inCol->getNumberOfElements(); ++i ){
      EVENT::CalorimeterHit* hit = 
	dynamic_cast< EVENT::CalorimeterHit* >( inCol->getElementAt( i ) );
      if ( hit ) {
	if ( _statusMap->map().find( hit->getCellID1() ) ==
	     _statusMap->map().end() )
	  outCol->addElement( hit );
      }
    }
    evt->addCollection( outCol, _outColName );
  }
}

void FilterBadChannels::end(){
}

} // namespace CALICE
