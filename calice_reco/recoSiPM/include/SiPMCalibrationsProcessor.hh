#ifndef SiPMPropertiesProcessor_hh
#define SiPMPropertiesProcessor_hh 1

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
#include "SiPMCalibrations.hh"

#include "SaturationParameters.hh"

using namespace marlin;
using namespace lcio;

/*
  TO DO: scaling factor from the data base
*/

namespace CALICE {

  /**
   * Processor that provides SiPM calibration information to other processors
   *
   * To obtain the object in other processors use:
   * SiPMCalibrationsProcessor::getCalibrations( "SiPMCalibrations processor name" )
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
   * <tr><td><b><em> SaturationCollection </em></b></td><td> name of the saturation collection</td></tr>
   * <tr><td><b><em> PixelScaleFactorsCollection </em></b></td><td> name of the pixel scale factors collection</td></tr>
   * <tr><td><b><em> GlobalPixelScaleFactor </em></b></td><td> global value for the pixel scale factor</td></tr>
   * <tr><td><b><em> CellQualityCollection </em></b></td><td> name of the saturation collection</td></tr>
   * </table>
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date October 2009
   */
  class SiPMCalibrationsProcessor : public Processor, public lccd::IConditionsChangeListener {
  public:

    SiPMCalibrationsProcessor();
    ~SiPMCalibrationsProcessor();

    /**
     * static function to obtain a MappedContainer with cell neighbours
     *
     * @param[in] processorName name of the SiPMCalibrationsProcessor that takes care of this SiPMCalibrations.
     * @returns pointer to the MappedContainer including SiPMCalibrations
     */
    static MappedContainer<SiPMCalibrations>* getCalibrations(const std::string& processorName);

    virtual Processor*  newProcessor() { return new SiPMCalibrationsProcessor; }

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
    void updateSaturation();
    void updateCellQuality();

    static std::map<std::string, MappedContainer<SiPMCalibrations>* > _SiPMCalibrationsContainerMap;
    MappedContainer<SiPMCalibrations>* _container;
    std::vector<SiPMCalibrations*> _allCalibrations;


    std::string _mappingProcessorName;

    std::string _MIPConstantColName;
    std::string _MIPSlopeColName;
    std::string _gainConstantColName;
    std::string _gainSlopeColName;
    std::string _interCalibrationColName;
    std::string _pedestalColName;
    std::string _temperatureColName;
    std::string _saturationColName;
    std::string _saturationParametersColName;
    std::string _pixelScaleFactorsColName;
    std::string _cellQualityColName;

    LCCollection *_MIPConstantCol;
    LCCollection *_MIPSlopeCol;
    LCCollection *_gainConstantCol;
    LCCollection *_gainSlopeCol;
    LCCollection *_interCalibrationCol;
    LCCollection *_pedestalCol;
    LCCollection *_temperatureCol;
    LCCollection *_saturationCol;
    LCCollection *_saturationParametersCol;
    LCCollection *_pixelScaleFactorsCol;
    LCCollection *_cellQualityCol;

    bool _MIPConstantChanged;
    bool _MIPSlopeChanged;
    bool _gainConstantChanged;
    bool _gainSlopeChanged;
    bool _interCalibrationChanged;
    bool _pedestalChanged;
    bool _temperatureChanged;
    bool _saturationChanged;
    bool _saturationParametersChanged;
    bool _pixelScaleFactorsChanged;
    bool _cellQualityChanged;

    bool  _useDBDefaultValuesOnly;
    bool  _useIndividualScaleFactor;
    float _globalPixelScaleFactor;
    int   _saturationProcedureType;

    // factors for systematic studies
    bool _MIPConstantScaled;
    bool _MIPSlopeScaled;
    bool _gainConstantScaled;
    bool _gainSlopeScaled;
    bool _interCalibrationScaled;
    bool _saturationCorrectionScaled;
    float _MIPConstantScaleFactor;
    float _MIPSlopeScaleFactor;
    float _gainConstantScaleFactor;
    float _gainSlopeScaleFactor;
    float _interCalibrationScaleFactor;
    float _saturationCorrectionScaleFactor;


    const Mapper* _mapper;
    unsigned int _mapperVersion;

  };


} // end namespace CALICE



#endif
