#ifndef Ahc2CalibrationsProcessor_hh
#define Ahc2CalibrationsProcessor_hh 1

// Marlin includes
#include "marlin/Processor.h"

// LCIO includes
#include "lcio.h"

// LCCD includes
#include "lccd/IConditionsChangeListener.hh"

// c++ includes
#include <string>
#include <map>

// CALICE includes
#include "Mapper.hh"
#include "MappedContainer.hh"
#include "Ahc2Calibrations.hh"
#include "SiPMCalibrationsProcessor.hh"

using namespace marlin;
using namespace lcio;


namespace CALICE {

  /**
   * Processor that provides Ahc2 prototype calibration information to other processors
   * 
   *
   * To obtain the object in other processors use:
   * Ahc2CalibrationsProcessor::getCalibrations( "Ahc2SiPMCalibrations processor name" )
   *
   * @par processor parameters
   * <table>
   * <tr><td> steering file parameter name </td><td> description </td></tr>
   * <tr><td><b><em> MappingProcessorName </em></b></td><td> name of the mapping processor that provides the necessary Mapper class</td></tr>
   * <tr><td><b><em> MipConstantCollection </em></b></td><td> name of the MIP constants collection</td></tr>
   * <tr><td><b><em> MipSlopesCollection </em></b></td><td> name of the MIP slopes collection</td></tr>
   * <tr><td><b><em> GainConstantCollection </em></b></td><td> name of the gain constants collection</td></tr>
   * <tr><td><b><em> GainSlopeCollection </em></b></td><td> name of the gain slopes collection</td></tr>
   * <tr><td><b><em> PedestalCollection </em></b></td><td> name of the pedestal collection</td></tr>
   * <tr><td><b><em> SaturationParametersCollection </em></b></td><td> name of the saturation parameters collection</td></tr>
   * <tr><td><b><em> TimeSlopesParametersCollection </em></b></td><td> name of the timeslopes parameters collection</td></tr>
   * <tr><td><b><em> TimePedParametersCollection </em></b></td><td> name of the time pedestal parameters collection</td></tr
   * <tr><td><b><em> PixelScaleFactorsCollection </em></b></td><td> name of the pixel scale factors collection</td></tr>
   * <tr><td><b><em> GlobalPixelScaleFactor </em></b></td><td> global value for the pixel scale factor</td></tr>
   * <tr><td><b><em> CellQualityCollection </em></b></td><td> name of the saturation collection</td></tr>
   * </table>
   *
   * @author Shaojun.lu@desy.de
   * @version 0.1
   * @date January 2013
   */
  class Ahc2CalibrationsProcessor : public Processor, public lccd::IConditionsChangeListener {
  public:

    Ahc2CalibrationsProcessor();
    ~Ahc2CalibrationsProcessor();

    /**
     * static function to obtain a MappedContainer with cell neighbours
     *
     * @param[in] processorName name of the Ahc2CalibrationsProcessor that takes care of this Ahc2Calibrations.
     * @returns pointer to the MappedContainer including SiPMCalibrations
     */
    static MappedContainer<Ahc2Calibrations>* getCalibrations(const std::string& processorName);

    virtual Processor*  newProcessor() { return new Ahc2CalibrationsProcessor; }

    /* Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /* Called for every event.
     */
    virtual void processEvent(LCEvent *evt) ;

    /* Called after data processing for clean up.
     */
    virtual void end() ;

    /* callback function for the condition changes
     */
    virtual void conditionsChanged(LCCollection *col);

  private:

    void updateMapping();
    void updateMIPCalibration();
    void updateGainCalibration();
    void updatePedestal();
    void updateTemperature();
    void updateInterCalibration();
    void updatePhysicsCalibIC();
    void updateSaturationParameters();
    void updateTimeSlopesParameters();
    void updateTimePedestalParameters();
    void updateCellQuality();

    static std::map<std::string, MappedContainer<Ahc2Calibrations>* > _Ahc2CalibrationsContainerMap;
    MappedContainer<Ahc2Calibrations>* _container;
    std::vector<Ahc2Calibrations*> _allCalibrations;


    std::string _mappingProcessorName;

    std::string _E4DMapperColName;
    std::string _MIPConstantColName;
    std::string _MIPSlopeColName;
    std::string _gainConstantColName;
    std::string _gainSlopeColName;
    std::string _interCalibrationColName;
    std::string _PhysicsCalibICColName;
    std::string _pedestalColName;
    std::string _temperatureColName;
    std::string _saturationParametersColName;
    std::string _timeSlopesParametersColName;
    std::string _timePedestalParametersColName;
    std::string _cellQualityColName;

    LCCollection *_E4DMapperCol;
    LCCollection *_MIPConstantCol;
    LCCollection *_MIPSlopeCol;
    LCCollection *_gainConstantCol;
    LCCollection *_gainSlopeCol;
    LCCollection *_interCalibrationCol;
    LCCollection *_PhysicsCalibICCol;
    LCCollection *_pedestalCol;
    LCCollection *_temperatureCol;
    LCCollection *_saturationParametersCol;
    LCCollection *_timeSlopesParametersCol;
    LCCollection *_timePedestalParametersCol;
    LCCollection *_cellQualityCol;

    bool _E4DMapperChanged;
    bool _MIPConstantChanged;
    bool _MIPSlopeChanged;
    bool _gainConstantChanged;
    bool _gainSlopeChanged;
    bool _interCalibrationChanged;
    bool _PhysicsCalibICChanged;
    bool _pedestalChanged;
    bool _temperatureChanged;
    bool _saturationParametersChanged;
    bool _timeSlopesParametersChanged;
    bool _timePedestalParametersChanged;
    bool _cellQualityChanged;

    float _globalPixelScaleFactor;

    // factors for systematic studies
    bool _MIPConstantScaled;
    bool _MIPSlopeScaled;
    bool _gainConstantScaled;
    bool _gainSlopeScaled;
    bool _interCalibrationScaled;
    bool _PhysicsCalibICScaled;
    float _MIPConstantScaleFactor;
    float _MIPSlopeScaleFactor;
    float _gainConstantScaleFactor;
    float _gainSlopeScaleFactor;
    float _interCalibrationScaleFactor;
    float _PhysicsCalibICScaleFactor;

    const Mapper* _mapper;
    unsigned int _mapperVersion;

  };


} // end namespace CALICE



#endif
