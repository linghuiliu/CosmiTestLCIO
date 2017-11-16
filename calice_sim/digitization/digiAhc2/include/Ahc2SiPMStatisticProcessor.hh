#ifndef AHC2SIPMSTATISTICPROCESSOR_HH
#define AHC2SIPMSTATISTICPROCESSOR_HH 1

/* Marlin Header */

#include "marlin/Processor.h"
#include "EVENT/LCEvent.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "lcio.h"

#include <string>
#include <vector>

/* CALICE Header */

#include "Ahc2CalibrationsProcessor.hh"
#include "CellNeighbours.hh"
#include "CellDescription.hh"
#include "Mapper.hh"
#include "Ahc2Mapper.hh"
#include "Ahc2Calibrations.hh"

#include <TH1.h>
#include <TFile.h>
#include "TRandom3.h"

using namespace lcio ;
using namespace marlin ;
using std::vector;

namespace CALICE
{

  /**
  * @brief Class for doing the AHCAL digitisation (SiPM treatment)
  *
  *
  * @author eldwan.brianne@desy.de
  * @version 1.0
  * @date December 2015
  */

  class Ahc2SiPMStatisticProcessor : public Processor {

  public:

    virtual Processor*  newProcessor() { return new Ahc2SiPMStatisticProcessor ; }

    /**Constructor
    */
    Ahc2SiPMStatisticProcessor() ;

    /**Destructor
    */
    ~Ahc2SiPMStatisticProcessor() {};

    /**Initialise
    */
    virtual void init() ;

    /**Process header
    */
    virtual void processRunHeader( LCRunHeader* run ) ;

    /**Process event (the working horse)
    */
    virtual void processEvent( LCEvent * evt ) ;

    /**Check
    */
    virtual void check( LCEvent * evt ) ;

    /**End of processing
    */
    virtual void end() ;

    /**Print Parameters
    */
    virtual void printParameters();

  protected:

    /**Fill a mapped container with the AHCAL cells.
    @param inputCol the input collection, with the AHCAL Mokka hits
    */
    void fillContainer(LCCollection *inputCol);

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
    void createOutputCollection(LCEvent *evt, const std::string colName);

  private:

    std::string _calorimInpCollection;/**<input collection name*/
    std::string _calorimOutCollection;/**<output collection name*/
    std::string _noiseColName;/**<Noise collection name*/
    std::string _mappingProcessorName;/**<Mapping Processor Name*/
    std::string _cellNeighboursProcessorName; /**<name of the processor which provides the cells neighbours*/
    std::string _cellDescriptionProcessorName;/**<CellDescription Processor name*/
    std::string _calibProcessorName;/**<Calibration Processor Name*/

    bool _filterDeadCells;/**<filter dead cells*/
    bool _doMipTempCorr;/**<do MIP temperature correction*/
    bool _doGainTempCorr;/**<do Gain temperature correction*/
    bool _filterDefaultCells;/**<filter cells with default values*/
    bool _doSaturation;/**<do SiPM Saturation*/
    bool _doBinomialSmearing;/**<do Binomial Smearing*/
    bool _doAddNoise;/**<Add Noise*/
    bool _correctDefaultGainToLY;/**<Correct LY if default gain*/
    bool _physicsMode; /**<physics mode for new ITEP*/
    bool _noiseEnergyMIP; /**<energy of noise hits are in MIP*/
    bool _doOpticalCrossTalk;        /**<flag to disable/enable optical cross talk among neighbour tiles*/

    float _fixedLY;/**<fix light yield if default gain*/

    StringVec _lightLeakage;             /**<light leakage factor vector*/
    std::map<int, float> m_leakage; /**<map containing ight leakage factor for each layer*/

    TRandom3 *_randomGenerator;/**<pointer to ROOTs random generator for pixel statistics*/
    int _randomSeed;           /**<random seed for the random generator. Steerable.*/

    std::string _mokkaEncodingString;/**<the Mokka encoding string*/

    const Ahc2Mapper *_mapper;       /**<pointer to the used mapped*/
    unsigned int _mapperVersion; /**<mapper version*/

    MappedContainer<CALICE::CellDescription> *_cellDescriptions;    /**<mapped container of cells description*/
    MappedContainer<CALICE::CellNeighbours>  *_cellNeighbours;      /**<mapped container of cells neighbours*/
    MappedContainer<Ahc2Calibrations> *_calibContainer;      /**<mapped container of SiPM calibrations*/
    MappedContainer<CalorimeterHitImpl> *_HitContainer;     /**<mapped container of AHCAL cells*/

    int _nRun ;
    int _nEvt ;

    static const unsigned int MAXADC = 36000; /**<conservative maximum number of ADC channels the hardware provides, 40960 - 4000 (average dynamic range)*/

    TFile *fOut;
    TH1F *hFillTimeSiPM;
  };

}

#endif
