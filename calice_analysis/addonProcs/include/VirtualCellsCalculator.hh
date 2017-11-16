#ifndef VirtualCellsCalculator_hh
#define VirtualCellsCalculator_hh 1

#include "VirtualCells.hh"
#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"

namespace CALICE {

  /**
   * class to calculate virtual cells of given size
   *
   * A MappedContainer of VirtualCells is filled with virtual cell positions
   * for cells of the give size.
   * The size of the virtual cells has to be @f$size_{\mathrm{mother}}/N@f$
   * with @f$N@f$ beeing an integer.
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date July 2009
   */
  class VirtualCellsCalculator {
  public:

    /**
     * constructor
     *
     * @param[in] mapper Mapper that holds global mapping information.
     */
    VirtualCellsCalculator(const Mapper *mapper);

    /**
     * update the MappedContainer of VirtuallCells
     *
     * All old content will be erased. Sizes have to be smaller and an exact fraction of the mother size.
     * There is no checking of the values implemented.
     *
     * @param[in] cellDescription Container of CellDescriptions that is used for position calculation.
     * @param[in] virtualCellSizeX x-size of the virtual cells, has to be @f$sizeX_{\mathrm{mother}}/N@f$, @f$N=\mathrm{integer} @f$
     * @param[in] virtualCellSizeY y-size of the virtual cells, has to be @f$sizeY_{\mathrm{mother}}/N@f$, @f$N=\mathrm{integer} @f$
     * @param[out] container the VirtualCells container that should be updated
     */
    void calculate(MappedContainer<CellDescription> *cellDescription, const float virtualCellSizeX, const float virtualCellSizeY, MappedContainer<VirtualCells>* container);
  private:
    const Mapper *_mapper;
  };


} // end namespace CALICE

#endif
