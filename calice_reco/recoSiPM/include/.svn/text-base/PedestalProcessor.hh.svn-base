#ifndef PedestalProcessor_h
#define PedestalProcessor_h 1

#include <lcio.h>
#include <marlin/Processor.h>

#include "AhcMapper.hh"
#include "MappedContainer.hh"
#include "RunTimeConditionsHandler.hh"
#include "TriggerBits.hh"

namespace CALICE {

  /** Processor which calculates the pedestals from pedestal events and
   * writes them as a collection of SimpleValue. Note that the pedestal is the 
   * average energy (in ADC counts) of all pedestal hits.
   * @author angela-isabela.lucaci-timoce@desy.de
   * @date Dec 2009
   */

  class PedestalProcessor : public marlin::Processor{
  public:
    /**Default constructor
     */
    PedestalProcessor();
    
    /**Destructor
     */
    ~PedestalProcessor();

    /**Return new instance of this processor
     */
    PedestalProcessor* newProcessor(){return new PedestalProcessor;}

    /**Initialise necessary variables
     */
    virtual void init();

    /**Process event (the working horse)
       @param evt event to be processed
     */
    void processEvent(LCEvent* evt);

    /**Function called at the end of all events, for cleanup
     */
    virtual void end();

  protected:
    /**Update the mapper
     */
    void updateMapper();

  protected:
    std::string _inputColName;        /**<name of the input collection*/
    std::string _outputColName;       /**<name of the output collection*/
    std::string _mappingProcessorName;/**<name of the processor providing the mapper*/

    TriggerBits _triggerConf;
    int _minPedNumber;               /**<minimum number of events for which pedestal should be calculated*/
    int _updateFrequency;            /**<frequency with which the pedestal should be calculated*/
    int _pedCounter;                 /**<counter for the number of pedestal events*/
    bool _skipMinimumEvent;          /**<flag to skip a minPedNumber of events*/

    const AhcMapper* _mapper;        /**<the mapper*/
    unsigned int _mapperVersion;     /**<the mapper version (which changes when the mapper changes)*/

    LCCollection *_colPedestal;      /**<collection with pedestal events*/
    RunTimeConditionsHandler *_conditionsHandler; /**<pointer to the run time conditions handler*/

    float **_pedSum;                 /**<double array of [module][cell] for the pedestal sum*/
    float **_pedSumSquare;           /**<double array of [module][cell] for the pedestal sum squared*/
    unsigned int **_pedNum;          /**<double array of [module][cell] for the number of pedestal events*/
    float **_ped;                    /**<double array of [module][cell] for the pedestal value*/
    float **_pedError;               /**<double array of [module][cell] for the pedestal error*/

    unsigned int _ahcMaxNumberOfModules;  /**<AHCAL maximum number of modules (39)*/
    unsigned int _ahcMaxNumberOfChannels; /**<AHCAL maximum number of channels*/
    unsigned int _ahcMaxNumberOfChips;    /**<AHCAL maximum number of chips*/
    unsigned int _ahcMaxNumberOfCells;    /**<AHCAL maximum number of cells*/

  };

}

#endif
