#ifndef Ahc2CalibrateProcessor_hh
#define Ahc2CalibrateProcessor_hh 1

#include <map>

#include "lcio.h"
#include "marlin/Processor.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "UTIL/LCRelationNavigator.h"


/* CALICE Header */
#include "Mapper.hh"
#include "MappedContainer.hh"
#include "Ahc2Mapper.hh"
#include "AhcAmplitude.hh"
#include "Ahc2CalibrationsProcessor.hh"
#include "AhcTempProvider.hh"


namespace CALICE
{
  /**
  * Processor that does the SiPM calibration. The calibration is done according to the formula:
  * \f$E_{calibrated}=f_{saturation}((E_{raw}-pedestal) \cdot IC/gain) \cdot gain/IC/MIP\f$
  *
  * Please note:
  *  - this processor does the calibration for both data and Monte Carlo, based on the type of the input collection
  *    ("LCGenericObject" for data, and "CalorimeterHit" for Monte Carlo);
  *  - for noise creation, the saturation correction is disabled (doSaturationCorrection=false),
  *    as well as the ZeroSuppression (ZeroSuppression = false)
  *  - for Monte Carlo, the pedestal subtraction is disabled (PedestalSubtraction=false)
  *
  * @par processor parameters
  * <table>
  * <tr><td> steering file parameter name </td><td> description </td></tr>
  * <tr><td><b><em> InputCollectionName  </em></b></td><td> name of the input collection, with energy in ADC counts</td></tr>
  * <tr><td><b><em> OutputCollectionName </em></b></td><td> name of the output collection, with energy in MIPs</td></tr>
  * <tr><td><b><em> MappingProcessorName </em></b></td><td> name of the mapping processor that provides the necessary Mapper class</td></tr>
  * <tr><td><b><em> Ahc2CalibrationsProcessorName </em></b></td><td> name of the processor which provides the SiPM calibrations</td></tr>
  * <tr><td><b><em> SiPMTemperatureProcessorName </em></b></td><td> name of the processor which provides the SiPM temperature</td></tr>
  * <tr><td><b><em> PedestalSubtraction </em></b></td><td> Flag to enable/disable pedestal subtraction</td></tr>
  * <tr><td><b><em> ZeroSuppression </em></b></td><td> Flag to enable/disable zero suppression (intention to apply the MIP cut)</td></tr>
  * <tr><td><b><em> MipCut </em></b></td><td> Value of the MIP cut (default: 0.4 MIPs)</td></tr>
  * <tr><td><b><em> doMipConversion </em></b></td><td> Flag to enable/disable MIP Calibration</td></tr>
  * <tr><td><b><em> doMipTemperatureCorrection </em></b></td><td> Flag to enable/disable temperature correction of the MIP</td></tr>
  * <tr><td><b><em> doGainTemperatureCorrection </em></b></td><td> Flag to enable/disable temperature correction of the gain</td></tr>
  * <tr><td><b><em> doSaturationCorrection </em></b></td><td> Flag to enable/disable the saturation correction</td></tr>
  * <tr><td><b><em> doTimeConversion </em></b></td><td> Flag to enable/disable the TDC Calibration</td></tr>
  * <tr><td><b><em> doErrorCalculation </em></b></td><td> Flag to enable/disable error calculation</td></tr>
  * <tr><td><b><em> filterDeadCells </em></b></td><td> Flag to enable/disable the filtering of dead cells</td></tr>
  * <tr><td><b><em> filterDefaultCells </em></b></td><td> Flag to enable/disable the filtering of default dead cells</td></tr>
  * </table>
  *
  * @author shaojun.lu@desy.de
  * @version 0.1
  * @date January 2013
  */

  class Ahc2CalibrateProcessor: public marlin::Processor
  {
  public:
    /*Return a new instance of this processor
    */
    Ahc2CalibrateProcessor *newProcessor()
    {
      return new Ahc2CalibrateProcessor;
    }

    /*Default constructor
    */
    Ahc2CalibrateProcessor();

    /*Destructor
    */
    ~Ahc2CalibrateProcessor(){};

    /*Initialise variables, if needed
    */
    virtual void init();

    /*Process event (the working horse)
    @param evt event to be processed
    */
    virtual void processEvent(LCEvent *evt);

    /*Function called after all events have been processed, for cleanup
    */
    virtual void end(){};

  protected:

    void FillContainer(LCEvent *evt);

    /**Calibrate the energy (convert from energy in ADC counts to energy in MIPs)
    @param calibration calibration of the current SiPM
    @param rawEnergy raw energy, in ADC counts
    @param rawTDC raw time, in TIC counts
    @param RampID BunchXID%2
    @param MEMCELLNr Memory cell ID
    @param NCHIPS chip ID
    @param ahcHitOutputCol the AHC hit output collection, of type CalorimeterHit
    */
    void calibrateEnergyAndFillOutputCollections(Ahc2Calibrations* calibration,
      const int GainBit,
      const float rawEnergy,
      const float rawTDC,
      const int BXID,
      const int Mem,
      LCCollection* ahcHitOutputCol
    );

    bool checkHit(CalorimeterHitImpl *hit);

  protected:
    std::string _inputColName;                /**<name of the input collection*/
    std::string _ahcHitOutputColName;         /**<name of the output AHC hit collection*/
    std::string _mappingProcessorName;        /**<name of the processor which provides the mapping*/
    std::string _calibProcessorName;          /**<name of the processor which provides the SiPM calibrations*/
    std::string _Ahc2HardwareConnectionName;
    std::string _temperatureProcessorName;    /**<name of the processor which provides the SiPM temperature*/

    std::string _cellDescriptionProcessorName; /**<name of the processor which provides the cells description*/
    MappedContainer<CellDescription> *_cellDescriptions; /**<mapped container of cells description*/

    bool _scaleEnergy;                                   /**<flag to enable/disable the scaling of the hits energy*/
    float _energyScaleFactor;                            /**<factor to scale the hits energy*/

    MappedContainer<CalorimeterHitImpl>  *_recoContainer; /**<mapped container of reconstructed AHCAL hits*/


    const Mapper* _mapper; /**<the mapper*/

    MappedContainer<Ahc2Calibrations> *_calibContainer;      /**<mapped container of SiPM calibrations*/
    MappedContainer<SimpleValue>      *_temperatureContainer;/**<mapped container of cells temperature*/

    bool _newdataformat;               /**<flag for the new data format in EUDAQ*/
    bool _pedestalSubtraction;     /**<flag to enable/disable pedestal subtraction*/
    bool _doMipTempCorr;           /**<flag to enable/disable the temperature correction of the MIP*/
    bool _doGainTempCorr;          /**<flag to enable/disable the temperature correction of the gain*/
    bool _doSaturationCorr;        /**<flag to enable/disable the saturation correction*/
    bool _zeroSuppression;         /**<flag to enable/disable the zero suppression, that is if you want
    to apply the 0.4 MIP cut or not*/
    bool _isPhysicsMode;           /**<flag to enable/disable the physics mode energy calibration*/

    bool _filterDeadCells;         /**<flag to enable/disable the filtering of dead cells*/
    bool _filterDefaultCells;      /**<flag to enable/disable the filgering of default cells*/
    bool _doErrorCalculation;      /**<flag to enable/disable the error calculation*/

    float _mipCut;             /**<value of the MIP cut (default: 0.4 MIPs)*/
    float _mipToGeVFactor;     /**<conversion factor from MIPs to GeV*/

    bool _doMipConversion; /**Convert ADC to MIP or not**/
    bool _doTimeConversion; /**Convert TDC to ns or not**/

    bool _isDATA;       /**<flag to switch to DATA processing */
    bool _isMC;         /**<flag to switch to MC processing */
    bool _isFirstEvent; /**<flag to switch to DATA/MC at the first event, the following events should be the same. */

    MappedContainer<CalorimeterHitImpl> *_HitContainer;     /**<mapped container of AHCAL cells*/
    map<int, pair<int, int> > _HardwareConnnectionContainer;/**<map containing relationship between ChipID and Module/ChipNb*/

  };//end of class Ahc2CalibrateProcessor

}//end of namespace CALICE
#endif
