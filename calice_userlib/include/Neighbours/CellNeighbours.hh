#ifndef CellNeighbours_hh
#define CellNeighbours_hh 1

#include "lcio.h"
#include "EVENT/LCObject.h"
#include "IMPL/LCGenericObjectImpl.h"

#include <string>
#include <sstream>

namespace CALICE {


  /**
   * class to hold information of neighbouring cells
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date June 2009
   */
  class CellNeighbours : public lcio::LCGenericObjectImpl {

  private:
    // helper enumerations
    enum EInt { k_CellID,
                k_NfixedNumbers };

    const static std::string neighbourFixedNames[k_NfixedNumbers];

  public:
    /**
     * neighbour type
     */
    enum ENeighbourType { direct, /*!<  describes the neighbours that have a common
                                    side/surface with the original cell */
                          corner, /*!< ENeighbourType::corner describes the neighbours
                                    that connect to the original cell via a corner */
                          k_NneighbourTypes };


    /**
     * neighbour location
     */
    enum ENeighbourLocation { module,   /*!< neighbours within the same module */
                              forward,  /*!< neighbours in the next module (beam direction) */
                              backward, /*!< neighbours in the previous module */
                              k_NneighbourLocations };
  private:
    const static std::string neighbourTypeNames[k_NneighbourTypes];
    const static std::string neighbourLocationNames[k_NneighbourLocations] ;

  public:

    /**
     * copy constructor
     *
     * This constructor is meant to copy-convert a CellNeighbours object from a LCCollection::getElementAt() call.
     * In contrary to other LCGenericObject derivates, this doesn't reference to the old object, but makes a full copy.
     *
     * @param[in] obj LCObject pointer to a CellNeighbours object
     */
    CellNeighbours( const lcio::LCObject *obj );

    /**
     * copy constructor
     *
     * This constructor is meant to copy-convert a CellNeighbours object from a LCGenericObject.
     *
     * @param[in] obj LCGenericObject of type CellNeighbours which should be copied
     */
    CellNeighbours( const lcio::LCGenericObject& obj );

    /**
     * constructor
     *
     * @param[in] cellID (Mokka) cell ID of the master cell
     */
    CellNeighbours( const int cellID );

    /**
     * add a neighbour cell
     *
     * @param[in] cellID (Mokka) cell ID of the neighbour cell
     * @param[in] type cell neighbour type
     * @param[in] location cell neighbour location
     *
     * @see ENeighbourType
     * @see ENeighbourLocation
     */
    void addNeighbour( const int cellID, const ENeighbourType type, const ENeighbourLocation location);

    /**
     * get (Mokka) cell ID of master cell
     *
     * @returns (Mokka) cell ID of master cell
     */
    int getCellID() const;

    /**
     * get vector of cell neighbours
     *
     * This function is more efficient as the other getNeighbours function, as a const
     * reference to the internal data is returned and no copying of elements is necessary.
     *
     * @param[in] type type of the neighbours requested
     * @param[in] location location of the neighbours requested
     *
     * @returns vector with (Mokka) cell IDs from the neighbours of the requested type and location
     */
    const std::vector<int>& getNeighbours( const ENeighbourType type, const ENeighbourLocation location ) const;
    /**
     * get vector of cell neighbours
     *
     * @param[in] type type of the neighbours requested
     *
     * @returns vector with (Mokka) cell IDs from the neighbours of the requested type
     */
    const std::vector<int> getNeighbours( const ENeighbourType type ) const;
    /**
     * get vector of cell neighbours
     *
     * @param[in] location location of the neighbours requested
     *
     * @returns vector with (Mokka) cell IDs from the neighbours of the requested location
     */
    const std::vector<int> getNeighbours( const ENeighbourLocation location ) const;
    /**
     * get vector of cell neighbours
     *
     * @returns vector with (Mokka) cell IDs from all neighbours
     */
    const std::vector<int> getNeighbours() const;

    /**
     * @returns LCGenericObject data type name
     */
    const std::string getTypeName() const { return std::string("CellNeighbours"); }
    /**
     * @returns LCGenericObject data description
     */
    const std::string getDataDescription() const { return _dataDescription.str(); }

  private:

    void copyFromLCGenericObject( const lcio::LCGenericObject& obj );

    std::ostringstream _dataDescription;

    void loadValues();
    void saveValues();

    std::vector<int> _neighbourVectors[k_NneighbourTypes][k_NneighbourLocations];

  };

} //end namespace CALICE

#endif
