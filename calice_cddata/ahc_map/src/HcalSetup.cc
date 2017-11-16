#include "HcalSetup.hh"
#include "AHCReader.hh"
#include "AHCHcalEntry.hh"

#include "ModuleConnection.hh"
#include "ModuleLocation.hh"
#include "DetectorTransformation.hh"

#include "IMPL/LCCollectionVec.h"

#include <cmath>

HcalSetup::HcalSetup() : _modPosCol( 0 ),
			 _detPosCol( 0 ),
			 _modConCol( 0 ) {
}


HcalSetup::HcalSetup( const char* fname ) : _modPosCol( 0 ),
					    _detPosCol( 0 ),
					    _modConCol( 0 ) {
  read( fname );
}


HcalSetup::HcalSetup( EVENT::LCCollection* mPos, 
		      EVENT::LCCollection* dPos, 
		      EVENT::LCCollection* mCon ) : _modPosCol( 0 ),
						    _detPosCol( 0 ),
						    _modConCol( 0 ) {
  setPositions( mPos, dPos );
  if ( mCon ) setConnections( mCon );
}


HcalSetup::~HcalSetup() {
  reset();
}


void HcalSetup::reset(){
  if ( _modPosCol ) delete _modPosCol;
  if ( _modConCol ) delete _modConCol;
  if ( _detPosCol ) delete _detPosCol;
}


void HcalSetup::read( const char* fname ) {
  reset();
  _modPosCol = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );
  _modConCol = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );
  _detPosCol = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );

  AHCReader rdr( fname );
  for ( AHCReader::const_iterator iter = rdr.begin();
	iter != rdr.end(); ++iter ){
    AHCHcalEntry* ahcEntry = dynamic_cast< AHCHcalEntry* >(*iter);
    if ( ahcEntry ){
      _modConCol->addElement( new CALICE::ModuleConnection( ahcEntry->getConnection( true ) ) );
      _modConCol->addElement( new CALICE::ModuleConnection( ahcEntry->getConnection( false ) ) );
      _modPosCol->addElement( new CALICE::ModuleLocation( ahcEntry->getLocation( true ) ) );
      _modPosCol->addElement( new CALICE::ModuleLocation( ahcEntry->getLocation( false ) ) );
    }
  }

  _detPosCol->addElement( new CALICE::DetectorTransformation() );

}



void HcalSetup::setPositions( EVENT::LCCollection* mCol,
			      EVENT::LCCollection* dCol ){
  if ( !mCol || mCol->getNumberOfElements() < 1 )
    throw std::runtime_error( "HcalSetup::setPositions: called with NULL pointer or empty collection" );
  if ( _modPosCol ) delete _modPosCol;
  if ( _detPosCol ) delete _detPosCol;

  _modPosCol = cloneCollection<CALICE::ModuleLocation>( mCol );
  if ( dCol && dCol->getNumberOfElements() > 0 ){
    _detPosCol = cloneCollection<CALICE::DetectorTransformation>( dCol );
  } else {
    std::cout << "WARNING  HcalSetup::setPositions - will use default detector position" << std::endl;
    _detPosCol = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );
    _detPosCol->addElement( new CALICE::DetectorTransformation() );
  }
}



void HcalSetup::setConnections( EVENT::LCCollection* col ){
  if ( !col || col->getNumberOfElements() < 1 )
    throw std::runtime_error( "HcalSetup::setConnections: called with NULL pointer or empty collection" );
  if ( _modConCol ) delete _modConCol;
  _modConCol = cloneCollection<CALICE::ModuleConnection>( col );
}


const float HcalSetup::getAngle() {//const {
  float a = detPos()->getDetectorAngleZX();
  return a;
}

void HcalSetup::setAngle( const float angle ){
  float oldAngle = getAngle();
  detPos()->setDetectorAngleZX( angle );
  for ( int i = 0; i != _modPosCol->getNumberOfElements(); ++i ){
    CALICE::ModuleLocation* loc = dynamic_cast< CALICE::ModuleLocation* >
      (_modPosCol->getElementAt( i ) );
    float z = loc->getZ();
    //  staggering is done relative to old values - this
    //  preserves any corrections for mis-alignment
    float x = loc->getX();                    // current Position
    x -= ( z * -sin( deg2rad(oldAngle) ) );   // shift back to no staggering
    x += ( z * -sin( deg2rad(angle) ) );      // shift to new staggering
    loc->setX( x );
  }
}


EVENT::LCCollection* HcalSetup::getModulePositions() const {
  return cloneCollection<CALICE::ModuleLocation>( _modPosCol );
}

EVENT::LCCollection* HcalSetup::getModuleConnections() const {
  return cloneCollection<CALICE::ModuleConnection>( _modConCol );
}

EVENT::LCCollection* HcalSetup::getDetectorPosition() const {
  return cloneCollection<CALICE::DetectorTransformation>( _detPosCol );
}

template< class T >
EVENT::LCCollection* HcalSetup::cloneCollection( EVENT::LCCollection* col ) const {
  IMPL::LCCollectionVec* cOut = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );
  for ( int i=0; i!= col->getNumberOfElements(); ++i ){
    cOut->addElement( new T( col->getElementAt( i ) ) );
  }
  return cOut;
}

CALICE::DetectorTransformation* HcalSetup::detPos() {
  if ( !_detPosCol && _detPosCol->getNumberOfElements()<1 )
    throw std::runtime_error( "HcalSetup::detPos - no DetectorTransformation available" );
  return dynamic_cast< CALICE::DetectorTransformation* >
    ( _detPosCol->getElementAt( 0 ) );
}


//const CALICE::DetectorTransformation* HcalSetup::detPos() const {
//  return detPos();
//}


const float HcalSetup::deg2rad( const float a ) const {
  return ( a / 180. * acos( -1. ) );
}

const float HcalSetup::rad2deg( const float a ) const {
  return ( a / acos( -1. ) * 180. );
}
