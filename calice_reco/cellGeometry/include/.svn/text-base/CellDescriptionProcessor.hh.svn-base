#ifndef CellDescriptionProcessor_hh
#define CellDescriptionProcessor_hh 1

/* Marlin includes*/
#include "marlin/Processor.h"

/* LCIO includes*/
#include "lcio.h"
#include "lccd/IConditionsChangeListener.hh"

/* c++ includes*/
#include <string>
#include <map>

/* CALICE includes*/
#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"
#include "ConditionsChangeDelegator.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /**
   * Processor that provides CellDescription for all cells.
   *
   * The processor generates and updates a MappedContainer of CellDescription objects from the conditions data.
   * To obtain the MappedCollection in other processors use:
   * CellDescriptionProcessor::getCellDescriptions( "cellDescription processor name" )
   *
   * @par processor parameters
   * <table>
   * <tr><td> steering file parameter name </td><td> description </td></tr>
   * <tr><td><b><em> DetectorTransformation </em></b></td><td> Name of the DetectorTransformation collection</td></tr>
   * <tr><td><b><em> MappingProcessorName </em></b></td><td> Name of the mapping processor that provides the necessary Mapper class</td></tr>
   * <tr><td><b><em> ModuleConnection </em></b></td><td> Name of the ModuleConnection collection</td></tr>
   * <tr><td><b><em> ModuleDescription </em></b></td><td> Name of the ModuleDescription collection</td></tr>
   * <tr><td><b><em> ModuleLocation </em></b></td><td> Name of the ModuleLocation collection</td></tr>
   * </table>
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.2
   * @date November 2009
   */
  class CellDescriptionProcessor : public Processor, public lccd::IConditionsChangeListener {
  public:

    CellDescriptionProcessor();

    /**
     * static function to obtain cell description container
     *
     * @param[in] processorName name of the CellDescriptionProcessor that takes care of this cell descriptions.
     * @returns pointer to the MappedContainer of CellDescriptions
     */
    static MappedContainer<CellDescription>* getCellDescriptions(const std::string& processorName);

    virtual Processor* newProcessor() { return new CellDescriptionProcessor ; }

    /* Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /* Called for every event.
     */
    virtual void processEvent( LCEvent * evt ) ;

    /* Called after data processing for clean up.
     */
    virtual void end() ;

    /*
     * callback functions for LCCD
     */
    virtual void conditionsChanged( LCCollection *col ) ;


  private:

    static std::map<std::string, MappedContainer<CellDescription>*> _cellDescriptionContainerMap;

    void generateCellDescription();

    const Mapper *_mapper;
    unsigned int _mapperVersion;

    std::string _mappingProcessorName;

    std::string _colNameModuleDescription;
    std::string _colNameModuleConnection;
    std::string _colNameModuleLocation;
    std::string _colNameDetectorTransformation;

    bool _moduleDescriptionChanged;
    bool _moduleConnectionChanged;
    bool _moduleLocationChanged;
    bool _detectorTransformationChanged;

    LCCollection* _colModuleDescription;
    LCCollection* _colModuleConnection;
    LCCollection* _colModuleLocation;
    LCCollection* _colDetectorTransformation;

  };


} /* end namespace CALICE*/


#endif
