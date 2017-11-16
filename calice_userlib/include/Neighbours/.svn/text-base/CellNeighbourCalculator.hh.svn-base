#ifndef CellNeighbourCalculator_hh
#define CellNeighbourCalculator_hh 1

#include "Mapper.hh"

#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "CellNeighbours.hh"
#include "MappedContainer.hh"

#include <set>

namespace CALICE {

  /**
   * class to calculate all neighbour cells
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date June 2009
   */
  class CellNeighbourCalculator {
  public:
    /**
     * constructor
     *
     * @param mapper Mapper that holds the mapping information from which the neighbours can be generated.
     */
    CellNeighbourCalculator(const Mapper* mapper);

    /**
     * get cell neighbours for all valid cells
     *
     * @returns LCCollection of CellNeighbours objects
     */
    lcio::LCCollection* getNeighbours() const;

    /**
     * get cell neighbours for all valid cells
     *
     * @param[out] container MappedContainer where the CellNeighbours objects should be stored
     */
    void getNeighbours(MappedContainer<CellNeighbours>* container) const;

  protected:
    void checkForCell(const unsigned int i, const unsigned int j, const unsigned int k, std::set<int>& cellIDset) const;

    CellNeighbours* getNeighbours(const int cellID) const;

  private:
    const Mapper* _mapper;

  };


} // end namespace CALICE

#endif
