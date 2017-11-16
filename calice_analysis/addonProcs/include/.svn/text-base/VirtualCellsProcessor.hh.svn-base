#ifndef VirtualCellsProcessor_hh
#define VirtualCellsProcessor_hh 1

// Marlin includes
#include "marlin/Processor.h"

// LCIO includes
#include "lcio.h"

// c++ includes
#include <string>
#include <map>

// CALICE includes
#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"
#include "VirtualCells.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /**
   * Processor that provides virtual cells of @f$1\mathrm{cm}\times1\mathrm{cm}@f$
   *
   * To obtain the object in other processors use:
   * VirtualCellsProcessor::getVirtualCells( "virtualCells processor name" )
   *
   * @par processor parameters
   * <table>
   * <tr><td> steering file parameter name </td><td> description </td></tr>
   * <tr><td><b><em> MappingProcessorName </em></b></td><td> name of the mapping processor that provides the necessary Mapper class</td></tr>
   * <tr><td><b><em> CellDescriptionProcessorName </em></b></td><td> name of the cell description processor that provides the necessary cell description information</td></tr>
   * </table>
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date June 2009
   */
  class VirtualCellsProcessor : public Processor {
  public:

    VirtualCellsProcessor();

    /**
     * static function to obtain a MappedContainer with virtual cells
     *
     * @param[in] processorName name of the VirtualCellsProcessor that takes care of this VirtualCells.
     * @returns pointer to the MappedContainer including VirtualCells
     */
    static MappedContainer<VirtualCells>* getVirtualCells(const std::string& processorName);

    virtual Processor*  newProcessor() { return new VirtualCellsProcessor ; }

    /* Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /* Called for every event.
     */
    virtual void processEvent( LCEvent * evt ) ;

    /* Called after data processing for clean up.
     */
    virtual void end() ;

  private:

    void calculateVirtualCells();

    static std::map<std::string, MappedContainer<VirtualCells>* > _virtualCellsContainerMap;

    std::string _mappingProcessorName;
    std::string _cellDescriptionProcessorName;

    const Mapper* _mapper;

    MappedContainer<CellDescription>* _cellDescription;
    unsigned int _cellDescriptionVersion;

  };


} // end namespace CALICE


#endif
