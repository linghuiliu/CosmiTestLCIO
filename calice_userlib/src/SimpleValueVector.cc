#include "SimpleValueVector.hh"

#include <stdexcept>
#include <cassert>
#include <vector>

namespace CALICE {

  /******************************************************
    construct from LCObject
  ******************************************************/
  SimpleValueVector::SimpleValueVector( EVENT::LCObject* obj ) 
  {
    EVENT::LCGenericObject* gObj = dynamic_cast< EVENT::LCGenericObject* >( obj );

    if ( gObj == NULL )
      throw std::invalid_argument( "SimpleValueVector: this LCObject is not LCGenericObject" );

    if ( gObj->getNInt() != gObj->getNFloat()/2+2 || gObj->getNFloat()%2 != 0 )
      throw std::invalid_argument( "SimpleValueVector: suspicious number of elements in this LCGenericObject" );

    for ( int i = 0; i < gObj->getNInt(); ++i )
      setIntVal( i, gObj->getIntVal( i ) );

    for ( int j = 0; j < gObj->getNFloat(); ++j )
      setFloatVal( j, gObj->getFloatVal( j ) );
  }


  /**************************************************
    construct from HcalTileIndex
  ************************************************/
  SimpleValueVector::SimpleValueVector( const int id,
					const unsigned int nMemo,
					const std::vector<float> val,
					const std::vector<float> err,
					const std::vector<int> stat )
  {
    setCellID( id );
    setSize(nMemo);

    for ( unsigned int i = 0; i < nMemo; ++i )
      {
        setValue ( i, val.at(i)  );
        setError ( i, err.at(i)  );
        setStatus( i, stat.at(i) );
      }

  }

}  /* namespace CALICE */
