#include "CellQuality.hh"

#include <stdexcept>

namespace CALICE {

CellQuality::CellQuality() {
}


CellQuality::CellQuality( const int id, const int status ) {
  _obj->setIntVal( 0, id );
  setStatus( status );
}

// The setting of the status is needed at several places in the class.
// To avoid duplicatind the information that status is int number 1 in the _obj
// this helper function is introduced. It is private and for internal use only.
void CellQuality::setStatus(int status)
{
  _obj->setIntVal( 1, status );
}

CellQuality::CellQuality( EVENT::LCObject* o ) :
  UTIL::LCFixedObject< 2, 0, 0 >( o ) { }


CellQuality::~CellQuality() {
}


const int CellQuality::getCellID() const {
  return _obj->getIntVal( 0 );
}

const int CellQuality::getStatus() const {
  return _obj->getIntVal( 1 );
}


const std::string CellQuality::getTypeName() const {
  return "CellQuality";
}


const std::string CellQuality::getDataDescription() const {
  return "i:ID, i:status";
}

bool CellQuality::isDead() const {
  return ( getStatus() & _dead );
}

bool CellQuality::isNoisy() const {
  return ( getStatus() & _noisy );
}

void CellQuality::setDead( bool dead )
{
  if (dead)
  {
    // Set the dead bit.
    // Apply a bitwise OR which turns on this bit and leaves the others unchanged.
    setStatus( getStatus() | _dead );
  }
  else
  {
    // Delete the dead bit.
    // This is done by inverting the _dead mask, where all bits are set
    // except for the dead bit. Applying a bitwise AND will leave all 
    // bits unchanged except for the dead bit.
    setStatus( getStatus() & ~_dead );
  }

}

void CellQuality::setNoisy( bool noisy )
{
  // for docu see setDead
  if (noisy)
  {
    setStatus( getStatus() | _noisy );
  }
  else
  {
    setStatus( getStatus() & ~_noisy );
  }
}

} // namespace CALICE
