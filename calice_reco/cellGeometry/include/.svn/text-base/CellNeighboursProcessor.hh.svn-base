#ifndef CellNeighboursProcessor_hh
#define CellNeighboursProcessor_hh 1

// Marlin includes
#include "marlin/Processor.h"

// LCIO includes
#include "lcio.h"

// c++ includes
#include <string>
#include <map>

// CALICE includes
#include "AhcMapper.hh"
#include "MappedContainer.hh"
#include "CellNeighbours.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /**
   * Processor that provides cell neighbours information
   *
   * To obtain the object in other processors use:
   * CellNeighboursProcessor::getNeighbours( "cellNeighbours processor name" )
   *
   * @par processor parameters
   * <table>
   * <tr><td> steering file parameter name </td><td> description </td></tr>
   * <tr><td><b><em> MappingProcessorName </em></b></td><td> name of the mapping processor 
   that provides the necessary Mapper class</td></tr>
   * </table>
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date June 2009
   */
  class CellNeighboursProcessor : public Processor {
  public:

    CellNeighboursProcessor();

    /**
     * static function to obtain a MappedContainer with cell neighbours
     *
     * @param[in] processorName name of the CellNeighboursProcessor that takes care of this CellNeighbours.
     * @returns pointer to the MappedContainer including CellNeighbours
     */
    static MappedContainer<CellNeighbours>* getNeighbours(const std::string& processorName);

    static const std::string& getEncodingString(const std::string& processorName);

    virtual Processor*  newProcessor() { return new CellNeighboursProcessor ; }

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

    void generateNeighbours();

    static std::map<std::string, MappedContainer<CellNeighbours>* > _cellNeighboursContainerMap;
    static std::map<std::string, std::string > _cellNeighboursEncodingStringMap;

    std::string _mappingProcessorName;

    const Mapper* _mapper;
    unsigned int _mapperVersion;

  };


} // end namespace CALICE


#endif
