#ifndef AHC_DIGITISATION_PROCESSOR_HH
#define AHC_DIGITISATION_PROCESSOR_HH 1

#include "TRandom3.h"

#include "marlin/Processor.h"
#include "EVENT/LCEvent.h"
#include "IMPL/CalorimeterHitImpl.h"

#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"
#include "CellNeighbours.hh"
#include "SiPMCalibrations.hh"
#include "SiPMCalibrateProcessor.hh"

namespace CALICE
{
  /**
   * @brief Class for doing the AHCAL digitisation
   *
   * For more details, have a look here:
   *  http://www-flc.desy.de/hcal/documents/2008/diplom.2008.richter.pdf
   *
   * @author lucaci@mail.desy.de
   * @version 1.0
   * @date April 2010
   */

 class AhcDigitisationProcessor : public marlin::Processor
  {
  public:
    /**Constructor
     */
    AhcDigitisationProcessor(const std::string processorName = "AhcDigitisationProcessor");
    /**Destructor
     */
    virtual ~AhcDigitisationProcessor();
    
    AhcDigitisationProcessor* newProcessor()
    {
      return new AhcDigitisationProcessor;
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
    
  protected:
    /**Fill a mapped container with the ganged AHCAL cells.
       By default, Mokka creates 10x10 mm^2 cells.
       During ganging (i.e. grouping), the Mokka cells are grouped into
       30x30 mm^2, 60x60 mm^2 and 120x120 mm^2 cells
       @param inputCol the input collection, with the AHCAL Mokka hits
     */
    void fillGangedContainer(LCCollection *inputCol);

    /**Simulate the optical cross-talk (i.e. light leakage) 
       between the tiles
      */
    void simulateOpticalCrossTalk();

    /**Simulate the SiPM binomial behaviour
     */
    void simulateSiPMBehaviour();

    /**Add noise events from data
       @param noiseCol the noise collection
     */
    void addNoise(LCCollection *noiseCol);

    /**Create output collection of digitised AHCAL hits
       @param evt LCEvent to which the collection will be added
     */
    void createOutputCollection(LCEvent *evt);
    
    /**Write collections after each step in digitisation, for debugging purposes
       @param evt LCEvent to which the collection will be added
       @param colName name of the collection to be written
     */
    void writeCollectionForDebug(LCEvent *evt, const std::string colName);
  
  private:
    std::string _inputColName;                /**<name of the input collection*/
    std::string _outputColName;               /**<name of the output collection*/
    std::string _noiseColName;                /**<name of the AHCAL noise collection*/
    std::string _mappingProcessorName;        /**<name of the mapping processor name*/
    std::string _cellNeighboursProcessorName; /**<name of the processor which provides the cells neighbours*/
    std::string _cellDescriptionProcessorName;/**<name of the processor which provides the cells description*/
    std::string _calibProcessorName;          /**<name of the processor which provides the SiPM calibrations*/
    std::string _temperatureProcessorName;    /**<name of the processor which provides the cell temperature*/
    std::string _simRecRelationColName;       /**<name of the relation between Mokka SimCalorimeterHits 
						 and reconstructed CalorimeterHits*/

    MappedContainer<CALICE::CellDescription> *_cellDescriptions;    /**<mapped container of cells description*/
    MappedContainer<CALICE::CellNeighbours>  *_cellNeighbours;      /**<mapped container of cells neighbours*/
    MappedContainer<SiPMCalibrations>        *_calibContainer;      /**<mapped container of SiPM calibrations*/
    MappedContainer<CalorimeterHitImpl>      *_gangedContainer;     /**<mapped container of ganged AHCAL cells*/
 
    float _mipPerGeVFactor;          /**<the MIP to GeV factor (determined with muon events in Monte Carlo)*/
    float _lightLeakage;             /**<light leakage factor*/
    bool _createSimRecRelation;      /**<flag to disable/enable the creation of an LCRelation between 
					the Mokka SimCalorimeterHits and the reconstructed CalorimeterHits*/
    bool _doBinomialSmearing;        /**<flag to disable/enable binomial smearing*/
    bool _doSaturation;              /**<flag to disable/enable SiPM saturation behaver to ture MC*/
    bool _doOpticalCrossTalk;        /**<flag to disable/enable optical cross talk among neighbour tiles*/
    bool _doAddNoise;                /**<flag to disable/enable adding detector noise*/
    bool _doWriteCollectionForDebug; /**<flag to disable/enable the writing of collections after each step in digitisation, for debugging*/
    bool _doMipTempCorr;
    bool _doGainTempCorr;
    
    const Mapper *_mapper;       /**<pointer to the used mapped*/
    unsigned int _mapperVersion; /**<mapper version*/

    bool  _correctDefaultGainToLY;
    float _fixedLY;
     
    TRandom3 *_randomGenerator;/**<pointer to ROOTs random generator for pixel statistics*/
    int _randomSeed;           /**<random seed for the random generator. Steerable.*/

    std::string _mokkaEncodingString;/**<the Mokka encoding string*/

    static const unsigned int MAXADC = 31768; /**<conservative maximum number of ADC channels the hardware provides, 32768 - 1000 (average pedestal)*/

  };/*end of class*/

}/*end of namespace CALICE*/
#endif
