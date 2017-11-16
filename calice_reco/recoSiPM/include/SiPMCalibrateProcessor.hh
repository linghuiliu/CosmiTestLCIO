#ifndef SiPMCalibrateProcessor_h
#define SiPMCalibrateProcessor_h 1

#include "lcio.h"
#include "marlin/Processor.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "UTIL/LCRelationNavigator.h"

#include "SiPMCalibrationsProcessor.hh"
#include "Mapper.hh"
#include "AhcMapper.hh"
#include "AhcAmplitude.hh"
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
   * <tr><td><b><em> SiPMCalibrationsProcessorName </em></b></td><td> name of the processor which provides the SiPM calibrations</td></tr>
   * <tr><td><b><em> SiPMTemperatureProcessorName </em></b></td><td> name of the processor which provides the SiPM temperature</td></tr>
   * <tr><td><b><em> PedestalSubtraction </em></b></td><td> Flag to enable/disable pedestal subtraction</td></tr>
   * <tr><td><b><em> ZeroSuppression </em></b></td><td> Flag to enable/disable zero suppression (intention to apply the MIP cut)</td></tr>
   * <tr><td><b><em> MipCut </em></b></td><td> Value of the MIP cut (default: 0.4 MIPs)</td></tr>
   * <tr><td><b><em> doMipTemperatureCorrection </em></b></td><td> Flag to enable/disable temperature correction of the MIP</td></tr>
   * <tr><td><b><em> doGainTemperatureCorrection </em></b></td><td> Flag to enable/disable temperature correction of the gain</td></tr>
   * <tr><td><b><em> doSaturationCorrection </em></b></td><td> Flag to enable/disable the saturation correction</td></tr>
   * <tr><td><b><em> doErrorCalculation </em></b></td><td> Flag to enable/disable error calculation</td></tr>
   * <tr><td><b><em> filterDeadCells </em></b></td><td> Flag to enable/disable the filtering of dead cells</td></tr>
   * <tr><td><b><em> filterDefaultCells </em></b></td><td> Flag to enable/disable the filtering of default dead cells</td></tr>
   * </table>
   *
   * @author angela-isabela.lucaci-timoce@desy.de
   * @version 0.2
   * @date May 2010
   */

  class SiPMCalibrateProcessor: public marlin::Processor
  {
  public:
    /**Return a new instance of this processor
     */
    SiPMCalibrateProcessor *newProcessor()
    {
      return new SiPMCalibrateProcessor;
    }

    /**Default constructor
     */
    SiPMCalibrateProcessor();

    /**Destructor
     */
    ~SiPMCalibrateProcessor(){};

    /**Initialise variables, if needed
     */
    virtual void init();

    /**Process event (the working horse)
       @param evt event to be processed
     */
    virtual void processEvent(LCEvent *evt);

    /**Function called after all events have been processed, for cleanup
     */
    virtual void end(){};

  protected:
    /**Calibrate the energy (convert from energy in ADC counts to energy in MIPs)
       @param calibration calibration of the current SiPM
       @param rawEnergy raw energy, in ADC counts
       @param ahcHitOutputCol the AHC hit output collection, of type CalorimeterHit
       @param ahcAmplOutputCol the AHC amplitude output collection, of type LCGenericObject
       @param relationNavigator navigator through the relation between CalorimeterHit and AhcAmplitude
     */
    void calibrateEnergyAndFillOutputCollections(SiPMCalibrations* calibration, 
						 const float rawEnergy,
						 LCCollection* ahcHitOutputCol,
						 LCCollection* ahcAmplOutputCol,
						 LCRelationNavigator &relationNavigator);
    void checkHitAmplRelation(LCEvent *evt);

    void createSimRecRelation(LCEvent *evt);
     
  protected:
    std::string _inputColName;                /**<name of the input collection*/
    std::string _ahcHitOutputColName;         /**<name of the output AHC hit collection*/
    std::string _ahcAmplOutputColName;        /**<name of the output AHC amplitude collection*/
    std::string _hitAmplRelationColName;      /**<name of the LCRelation collection between CalorimeterHit and AhcAmplitude*/
    std::string _mappingProcessorName;        /**<name of the processor which provides the mapping*/
    std::string _calibProcessorName;          /**<name of the processor which provides the SiPM calibrations*/
    std::string _temperatureProcessorName;    /**<name of the processor which provides the SiPM temperature*/
    
    std::string _cellDescriptionProcessorName; /**<name of the processor which provides the cells description*/
    MappedContainer<CellDescription> *_cellDescriptions; /**<mapped container of cells description*/

    bool _scaleEnergy;                                   /**<flag to enable/disable the scaling of the hits energy*/
    float _energyScaleFactor;                            /**<factor to scale the hits energy*/

    bool _createSimRecRelation;                           /**<flag to disable/enable the creation of an LCRelation between 
							     the Mokka SimCalorimeterHits and the reconstructed CalorimeterHits*/
    MappedContainer<CalorimeterHitImpl>  *_recoContainer; /**<mapped container of reconstructed AHCAL hits*/
    std::string _simHitColName;                           /**<name of collection containing SimCalorimeterHits*/
    std::string _simRecRelationColName;                   /**<name of the relation between Mokka SimCalorimeterHits 
							     and reconstructed CalorimeterHits*/


    const Mapper* _mapper; /**<the mapper*/

    MappedContainer<SiPMCalibrations> *_calibContainer;      /**<mapped container of SiPM calibrations*/
    MappedContainer<SimpleValue>      *_temperatureContainer;/**<mapped container of cells temperature*/

    bool _pedestalSubtraction;     /**<flag to enable/disable pedestal subtraction*/
    bool _doMipTempCorr;           /**<flag to enable/disable the temperature correction of the MIP*/
    bool _doGainTempCorr;          /**<flag to enable/disable the temperature correction of the gain*/
    bool _doSaturationCorr;        /**<flag to enable/disable the saturation correction*/
    bool _zeroSuppression;         /**<flag to enable/disable the zero suppression, that is if you want 
				      to apply the 0.4 MIP cut or not*/
    bool _filterDeadCells;         /**<flag to enable/disable the filtering of dead cells*/
    bool _filterDefaultCells;      /**<flag to enable/disable the filgering of default cells*/
    bool _doErrorCalculation;      /**<flag to enable/disable the error calculation*/
    bool _doWriteOnlyRawAmplitude; /**<flag to enable/disable the writing of raw AHCal hits (in ADC counts)*/

    float _mipCut;             /**<value of the MIP cut (default: 0.4 MIPs)*/
    float _mipToGeVFactor;     /**<conversion factor from MIPs to GeV*/
    
    bool  _correctDefaultGainToLY; 
    float _fixedLY;

    bool _isDATA;       /**<flag to switch to DATA processing */
    bool _isMC;         /**<flag to switch to MC processing */
    bool _isFirstEvent; /**<flag to switch to DATA/MC at the first event, the following events should be the same. */

  };/*end of class SiPMCalibrateProcessor*/
  
}/*end of namespace CALICE*/
#endif
