#ifndef AHC2TIMESMEARINGPROCESSOR_HH
#define AHC2TIMESMEARINGPROCESSOR_HH 1

/* Marlin Header */

#include "marlin/Processor.h"
#include "EVENT/LCEvent.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "lcio.h"

#include <string>
#include <vector>

/* CALICE Header */

#include "Ahc2CalibrationsProcessor.hh"
#include "Mapper.hh"
#include "Ahc2Mapper.hh"
#include "Ahc2Calibrations.hh"

#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <TRandom3.h>
#include <TGraph.h>

using namespace lcio ;
using namespace marlin ;
using std::vector;

namespace CALICE
{

  /**
  * @brief Class for doing the AHCAL digitisation (Time Smearing)
  *
  *
  * @author eldwan.brianne@desy.de
  * @version 1.0
  * @date July 2016
  */

  class Ahc2TimeSmearingProcessor : public Processor {

  public:

    virtual Processor*  newProcessor() { return new Ahc2TimeSmearingProcessor ; }

    /**Constructor
    */
    Ahc2TimeSmearingProcessor() ;

    /**Destructor
    */
    ~Ahc2TimeSmearingProcessor() {};

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

    /** Hardware Connection
    */
    void FillContainer( LCEvent * evt ) ;

  private:

    std::string _calorimInpCollection;/**<input collection name*/
    std::string _calorimOutCollection;/**<output collection name*/

    bool _doTimeSmearing; /**<do TimeSmearing*/
    bool _doTDCShift; /**<do TDC shift parametrization*/

    TRandom3 *_randomGenerator;/**<pointer to ROOTs random generator for gaussian smearing*/
    int _randomSeed;           /**<random seed for the random generator. Steerable.*/

    std::string _mokkaEncodingString;/**<the Mokka encoding string*/

    int _nRun ;
    int _nEvt ;

    float _mu;/**<mean simple gaus*/
    float _reso;/**<sigma simple gaus*/

    float _norm1;/**<norm1 double gaus*/
    float _mu1;/**<mean1 double gaus*/
    float _reso1;/**<sigma1 double gaus*/
    float _norm2;/**<norm2 double gaus*/
    float _mu2;/**<mean2 double gaus*/
    float _reso2;/**<sigma2 double gaus*/

    int Max_nHits;

    StringVec _ParaVector;/**<string vector for parameters for the TDC shift*/
    StringVec _KVector;/**<string vector for time resolution*/

    TFile *fOut;
    TH1F *hFillTimeSmeared;

    std::string _mappingProcessorName;        /**<name of the processor which provides the mapping*/
    const Ahc2Mapper* _mapper; /**<the mapper*/

    std::vector<int> nHitsPerChipAboveThr;/**vector for event parameter with number of triggered channels per chip above 0.5 MIP*/
    std::map<int, int> m_ChipTrigger; /**<map with number of triggered channels per chip for pedestal shift*/

    std::vector<float> _SigmaVector; /** Vector containing the sigma value of each bin */
    TGraph *_ShiftFcn;
    std::map<int, TF1*> m_function; /**<function used for smearing*/

    std::string _methodSmearing; /**<Method used for smearing*/
  };

}

#endif
