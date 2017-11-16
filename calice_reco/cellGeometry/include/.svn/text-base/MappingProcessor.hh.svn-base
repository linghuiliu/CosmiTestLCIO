#ifndef MappingProcessor_hh
#define MappingProcessor_hh 1

/* ILC soft includes */
#include "marlin/Processor.h"
#include "lcio.h"
#include "lccd/IConditionsChangeListener.hh"

/* c++ includes*/
#include <string>
#include <map>

/* CALICE includes*/
#include "Mapper.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /**
   * Processor that provides a CALICE Mapper object.
   *
   * The processor generates and updates a CALICE Mapper object from the conditions data.
   * To obtain the object in other processors use:
   * MappingProcessor::getMapper( "mapping processor name" )
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.2
   * @date November 2009
   */
  class MappingProcessor : public Processor, public lccd::IConditionsChangeListener
  {
  public:
    
    MappingProcessor();

    /**
     * static function to obtain mapper
     *
     * @param[in] processorName name of the MappingProcessor that takes care of this Mapper.
     * @returns pointer to the mapper
     */
    static const Mapper* getMapper(const std::string& processorName);


    virtual Processor*  newProcessor() { return new MappingProcessor ; }

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

    void conditionsChanged(LCCollection *col);

    static std::map<std::string, Mapper*> _mapperMap;

    std::string _mapperTypeName;

    std::string _colNameModuleDescription;
    std::string _colNameModuleConnection;

    bool _moduleDescriptionChanged;
    bool _moduleConnectionChanged;

    LCCollection* _colModuleDescription;
    LCCollection* _colModuleConnection;


  };


} // end namespace CALICE


#endif
