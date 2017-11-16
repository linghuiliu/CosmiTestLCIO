#ifndef Ahc2ROCThresholdProcessor_hh
#define Ahc2ROCThresholdProcessor_hh 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>

#include "Ahc2CalibrationsProcessor.hh"
#include "Mapper.hh"
#include "Ahc2Mapper.hh"
#include "Ahc2Calibrations.hh"

#include <TH1.h>
#include <TFile.h>

using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace CALICE
{

  /**
  * @brief Class for doing the AHCAL ROC Simulating behavior
  *
  * @author eldwan.brianne@desy.de
  * @version 2.0
  * @date November 2015
  */

  class Ahc2ROCThresholdProcessor : public Processor
  {
  private:
    //processor parameters:
    float _tileEdgeX;/**<Tile size in X*/
    float _tileEdgeY;/**<Tile size in Y*/
    float _deadSpace;/**<dead space*/
    float _MIPvalue;/**<MIP Value*/
    float _MIPThr;/**<MIP Threshold*/
    float _tfast;/**<fast shaper integration time*/
    float _tslow;/**<slow shaper integration time*/
    int _NLayer;/**<Number of layers*/
    string _LayerPattern;/**<layer pattern*/
    vector<int> _LayerPatternVector;/**<layer pattern vector*/

  public:

    virtual Processor*  newProcessor() { return new Ahc2ROCThresholdProcessor ; }

    /**Constructor
    */
    Ahc2ROCThresholdProcessor() ;

    /**Destructor
    */
    ~Ahc2ROCThresholdProcessor() {};

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

    /** Print Parameters
    */
    virtual void printParameters();

    /** Check the layer pattern
    */
    bool isValidLayerPattern(string LayerPattern);

    /** Get layer pattern
    */
    vector<int> getLayerPatternVector(string LayerPattern);

  protected:
    std::string _calorimInpCollection;/**<input collection name*/
    std::string _calorimOutCollection;/**<output collection name*/

    std::string _encoding;

    float energyThreshold;/**<Threshold*/
    float halfdS;/**<half dead space*/

    int _nRun ;/**<run number*/
    int _nEvt ;/**<event number*/

    std::string _mappingProcessorName;        /**<name of the processor which provides the mapping*/
    std::string _calibProcessorName;          /**<name of the processor which provides the SiPM calibrations*/
    const Ahc2Mapper* _mapper;                               /**<the mapper*/
    MappedContainer<Ahc2Calibrations> *_calibContainer;      /**<mapped container of SiPM calibrations*/

    TFile *fOut;
    TH1F *hFillTimeROC;

    std::map<int, int> m_ChipTrigger; /**<map with number of triggered channels per chip for pedestal shift*/
    IntVec nHitsPerChipAboveThr;/**<vector to store the number of hits per Chips per event*/

  } ;
}
#endif
