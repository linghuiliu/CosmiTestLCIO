#ifndef AHC_GANGING_PROCESSOR_HH
#define AHC_GANGING_PROCESSOR_HH 1

#include "marlin/Processor.h"
#include "IMPL/CalorimeterHitImpl.h"

#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"
#include "CellNeighbours.hh"

namespace CALICE
{

  /**
   * @brief Class for doing the AHCal ganging
   *
   * For more details, have a look here:
   *  http://www-flc.desy.de/hcal/documents/2008/diplom.2008.richter.pdf
   *
   * @author lucaci@mail.desy.de
   * @version 1.0
   * @date May 2010
   */
 class AhcGangingProcessor : public marlin::Processor
  {
  public:
    /**Constructor
     */
    AhcGangingProcessor(const std::string processorName = "AhcGangingProcessor");
    /**Destructor
     */
    virtual ~AhcGangingProcessor();
    
    AhcGangingProcessor* newProcessor()
    {
      return new AhcGangingProcessor;
    }

    /**Initialise
     */
    virtual void init();

    /**Process event (the working horse)
     */
    virtual void processEvent(lcio::LCEvent*);
 
   /**End of processing
     */
    virtual void end();

  private:
    std::string _inputColName;                /**<name of the input collection*/
    std::string _outputColName;               /**<name of the output collection*/
    std::string _mappingProcessorName;        /**<name of the mapping processor name*/
    std::string _cellNeighboursProcessorName; /**<name of the processor which provides the cells neighbours*/
    std::string _cellDescriptionProcessorName;/**<name of the processor which provides the cells description*/
    
    MappedContainer<CALICE::CellDescription> *_cellDescriptions;    /**<mapped container of cells description*/
    MappedContainer<CALICE::CellNeighbours>  *_cellNeighbours;      /**<mapped container of cells neighbours*/

    const Mapper *_mapper;       /**<pointer to the used mapped*/

  };/*end of class*/

}/*end of namespace CALICE*/
#endif
