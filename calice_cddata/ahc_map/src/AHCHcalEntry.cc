#include "AHCHcalEntry.hh"
#include <sstream>

AHCHcalEntry::AHCHcalEntry( const std::string& line ){
  init( line );
}


AHCHcalEntry::~AHCHcalEntry(){
}


void AHCHcalEntry::init( const std::string& line ){
  std::stringstream ln( line );
  ln >> _slot >> _fe >> _type 
     >> _modId >> _layer 
     >> _cmbId >> _cmbCanAdr >> _cmbPinId
     >> _holdExt >> _holdLedCm >> _holdLedPm
     >> _vcalibCm >> _vcalibPm;
}

const CALICE::ModuleConnection AHCHcalEntry::getConnection( const bool upper ) const {
  CALICE::ModuleConnection ret;
  ret.setCrate( HcalImplicits::crate() )
    .setSlot( _slot )
    .setFrontEnd( _fe )
    .setConnectorType( HcalImplicits::connectorType( upper ) )
    .setIndexOfLowerLeftCell( HcalImplicits::geometricalModuleIndex( _layer, upper ) )
    .setModuleType( HcalImplicits::moduleType( _modId, upper ) )
    .setModuleID( _modId );
  return ret;
}


const CALICE::ModuleLocation AHCHcalEntry::getLocation( const bool upper ) const {
  CALICE::ModuleLocation ret;
  ret.setX( HcalImplicits::nominalPosX() )
    .setY( HcalImplicits::nominalPosY( upper ) )
    .setZ( HcalImplicits::nominalPosZ( _layer, _absorberFeW ) )
    .setCellIndexOffset( HcalImplicits::geometricalModuleIndex( _layer, upper ) )
    .setModuleType( HcalImplicits::moduleType( _modId, upper ) + 4 );//types had different numbering scheme in connection (HcalMapping 0, 1, 2, 3);  types had the same numbering scheme in description (4, 5, 6, 7) and locationReference (4, 5, 6, 7).
  return ret;
}
