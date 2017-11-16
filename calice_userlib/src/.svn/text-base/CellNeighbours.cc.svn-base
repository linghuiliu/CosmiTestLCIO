#include "CellNeighbours.hh"

#include "CaliceException.hh"

namespace CALICE {


  const std::string CellNeighbours::neighbourFixedNames[CellNeighbours::k_NfixedNumbers] = { "cellID" };
  const std::string CellNeighbours::neighbourTypeNames[CellNeighbours::k_NneighbourTypes] = { "direct",
                                                                                              "corner" };
  const std::string CellNeighbours::neighbourLocationNames[CellNeighbours::k_NneighbourLocations] = { "module",
                                                                                                      "forward",
                                                                                                      "backward"};

  CellNeighbours::CellNeighbours( const lcio::LCObject *obj) {

    const lcio::LCGenericObject *genericObj = dynamic_cast<const lcio::LCGenericObject*> (obj) ;

    if (genericObj) {
      copyFromLCGenericObject( *genericObj );
    }
    else throw WrongDataFormatException("CellNeighbours::CellNeighbours(): cannot cast input to LCGenericObject");
  }

  CellNeighbours::CellNeighbours( const lcio::LCGenericObject &obj) {
    copyFromLCGenericObject(obj);
  }


  void CellNeighbours::copyFromLCGenericObject( const lcio::LCGenericObject &obj ) {

    if (obj.getTypeName() != getTypeName() ) throw WrongDataFormatException("CellNeighbours::copyFromLCGenericObject(): LCGenericObject has different type name");

    for (int i = 0; i < obj.getNInt(); ++i ) _intVec.push_back( obj.getIntVal(i) );
    for (int i = 0; i < obj.getNFloat(); ++i ) _floatVec.push_back( obj.getFloatVal(i) );
    for (int i = 0; i < obj.getNDouble(); ++i ) _doubleVec.push_back( obj.getDoubleVal(i) );

    _isFixedSize = false;
    _dataDescription.str(obj.getDataDescription());

    loadValues();
  }

  CellNeighbours::CellNeighbours( const int cellID ) : lcio::LCGenericObjectImpl() {
    setIntVal( k_CellID, cellID);
  }

  void CellNeighbours::addNeighbour( const int cellID, const ENeighbourType type, const ENeighbourLocation location) {
    _neighbourVectors[type][location].push_back(cellID);
    saveValues();
  }

  int CellNeighbours::getCellID() const {
    return getIntVal( k_CellID );
  }

  const std::vector<int>& CellNeighbours::getNeighbours( const ENeighbourType type, const ENeighbourLocation location ) const {
    return _neighbourVectors[type][location];
  }

  const std::vector<int> CellNeighbours::getNeighbours( ) const {

    std::vector<int> result;

    for (unsigned int type = 0; type < k_NneighbourTypes; ++type)
      for (unsigned int location = 0; location < k_NneighbourLocations; ++location)
        result.insert(result.end(), _neighbourVectors[type][location].begin(), _neighbourVectors[type][location].end() );

    return result;
  }

  const std::vector<int> CellNeighbours::getNeighbours( const ENeighbourType type ) const {

    std::vector<int> result;

    for (unsigned int location = 0; location < k_NneighbourLocations; ++location)
      result.insert(result.end(), _neighbourVectors[type][location].begin(), _neighbourVectors[type][location].end() );

    return result;
  }

  const std::vector<int> CellNeighbours::getNeighbours( const ENeighbourLocation location ) const {

    std::vector<int> result;

    for (unsigned int type = 0; type < k_NneighbourTypes; ++type)
      result.insert(result.end(), _neighbourVectors[type][location].begin(), _neighbourVectors[type][location].end() );

    return result;
  }

  void CellNeighbours::loadValues() {

    int basePosition = k_NfixedNumbers; // fixed numbers are set and read directly
    int endOfSizes = k_NneighbourLocations*k_NneighbourTypes;
    int additionalElements = 0;

    for (unsigned int type = 0; type < k_NneighbourTypes; ++type)
      for (unsigned int location = 0; location < k_NneighbourLocations; ++location) {
        unsigned int nValues = getIntVal(basePosition+type*k_NneighbourLocations+location);
        for (unsigned int i = 0; i < nValues; ++i ) {
          _neighbourVectors[type][location].push_back( getIntVal(basePosition + endOfSizes + additionalElements + i) );
        }
        additionalElements += nValues;
      }

    for (unsigned int type = 0; type < k_NneighbourTypes; ++type)
      for (unsigned int location = 0; location < k_NneighbourLocations; ++location) {
        _dataDescription << ",i:" << neighbourTypeNames[type] << "_" <<  neighbourLocationNames[location] << "_size";
      }

    for (unsigned int type = 0; type < k_NneighbourTypes; ++type)
      for (unsigned int location = 0; location < k_NneighbourLocations; ++location)
        for (unsigned int i = 0; i < _neighbourVectors[type][location].size(); ++i ) {
          _dataDescription << ",i[" <<_neighbourVectors[type][location].size() << "]";
        }


  }

  void CellNeighbours::saveValues() {
    _dataDescription.str("");

    int runningPos = k_NfixedNumbers; // fixed numbers are set and read directly

    _dataDescription << "i:" << neighbourFixedNames[k_CellID];

    for (unsigned int type = 0; type < k_NneighbourTypes; ++type)
      for (unsigned int location = 0; location < k_NneighbourLocations; ++location) {
        _dataDescription << ",i:" << neighbourTypeNames[type] << "_" <<  neighbourLocationNames[location] << "_size";
        setIntVal( runningPos++, _neighbourVectors[type][location].size() );
      }

    for (unsigned int type = 0; type < k_NneighbourTypes; ++type)
      for (unsigned int location = 0; location < k_NneighbourLocations; ++location)
        for (unsigned int i = 0; i < _neighbourVectors[type][location].size(); ++i ) {
          _dataDescription << ",i[" <<_neighbourVectors[type][location].size() << "]";
        }

  }

} //end namespace CALICE
