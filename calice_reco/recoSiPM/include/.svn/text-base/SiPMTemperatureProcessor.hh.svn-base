#ifndef SiPMTemperatureProcessor_hh
#define SiPMTemperatureProcessor_hh 1

/*Marlin includes*/
#include "marlin/Processor.h"

/*LCCD includes*/
#include "lccd/IConditionsChangeListener.hh"

/*LCIO included*/
#include "EVENT/LCFloatVec.h"

/*CALICE includes*/
#include "AhcTempProvider.hh"
#include "MappedContainer.hh"
#include "AhcMapper.hh"

#include "RunTimeConditionsHandler.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE
{
 /**
   * Processor that provides the SiPM temperature
   *
   * The processor generates and updates a CALICE MappedContainer object from the conditions data.
   * To obtain the object in other processors use:
   * SiPMTemperatureProcessor::getTemperatureContainer("temperature processor name");

   * @author angela-isabela.lucaci-timoce@desy.de
   * @version 0.2
   * @date May 2010
   */
  class SiPMTemperatureProcessor: public Processor, public lccd::IConditionsChangeListener
  {
  public:
    /**Default constructor
     */
    SiPMTemperatureProcessor();

    /**Destructor
     */
    ~SiPMTemperatureProcessor(){};

    /**Return a new instance of the processor
     */
    virtual SiPMTemperatureProcessor *newProcessor()
    {
      return new SiPMTemperatureProcessor();
    }

    /** Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /** Called for every event.
	@param evt event to be processed
     */
    virtual void processEvent( LCEvent * evt ) ;

    /** Called after data processing (after all events are processed) for clean up.
     */
    virtual void end(){} ;


    /** Callback function for the condition changes
	@param col collection to be checked if it has changed or not
     */
    virtual void conditionsChanged( LCCollection * col );

    /** Get the temperature provider
	@param processorName name of the processor returning the temperature provider
	@return AhcTempProvider
     */
    static AhcTempProvider* getTemperatureProvider(const std::string& processorName);

  private:
    std::string _ahcSroModDataColName; /**<name of the collection containing the slow control data*/
    std::string _temperatureSensorCalibrationColName;/**<name of the collection containing the temperature 
						      sensor calibration*/

    LCCollection *_ahcSroModDataCol;/**<collection containing the slow control data*/
    LCCollection *_temperatureSensorCalibrationCol;/**<collection containing the temperature 
						      sensor calibration*/

    bool _ahcSroModDataColChanged;/**<flag to signalise if the slow control data collection has changed*/
    bool _temperatureSensorCalibrationColChanged;/**<flag to signalise if the collection 
						    containing the temperature sensor calibration
						    hass changed or not*/

    std::string _mappingProcessorName;/**<name of the processor name which provides the mapping*/

    AhcTempProvider* _tempProvider; /**<pointer to the AHC temperature provider*/
    static std::map<std::string, AhcTempProvider*> _tempProviderMap; /**<map of AhcTempProvider*/
    const AhcMapper* _mapper; /**<the mapper*/
 
    lcio::FloatVec _tempProviderSanityRangeVec;/**<vector of floats to save the sanity range of 
						the temperature provider*/
    std::string _tempProviderModelName; /**<name of the model used for the temperature provider. 
					   If the model is "simple", use AhcSimpleTempProvider*/

    std::string _outputColName; /**<name of the output collection*/
    LCCollection *_colTemperature; /**<collection of temperatures*/
    RunTimeConditionsHandler *_conditionsHandler; /**<pointer to the run time conditions handler*/


  };/*end of class*/


}/*end of namespace CALICE*/
#endif
