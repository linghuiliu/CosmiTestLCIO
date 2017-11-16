#ifndef multiCalibrator_hh_
#define multiCalibrator_hh_

#include <string>
#include <map>
#include <marlin/Processor.h>
#include "TH1F.h"

#include "lccd.h"
#include "lccd/IConditionsChangeListener.hh"

#define PAR_MULTI "MultiAmplitude"

namespace CALICE {

  /**
   * @brief Processor to add PAR_MULTI to the event and calibrate the threshold.
   *
   * This processor adds an event parameter containing the amplitude of the analog multiplicity counter readout.
   * In addition a histogram of the amplitudes is collected and fitted to calibrate the multi threshold.
   * The calibration procedure can be skipped by processor parameter. In this case,
   * only the event parameter is added.
   *
   * The mapping of the multiplicity counter connection is read from the LCIO stream.
   *
   * The results of the calibration can be written directly to a LCCD database. To enable this feature,
   * MULTI_DBINIT has to contain a valid  database connection string for write access at compile time.
   * In this case, two processor parameters to set the output folders for thresholds and fraction of
   * rejected events are available. If the string for a folder is empty, the writing of this folder
   * will be skipped.
   *
   * @version 1.0
   * @author Benjamin.Lutz@desy.de
   * @date Sep 2008
   */
  class multiCalibrator : public marlin::Processor, public lccd::IConditionsChangeListener  {
  public:
    Processor*  newProcessor() { return new multiCalibrator ; }

    multiCalibrator();

    ~multiCalibrator() {};

    virtual void init();

    virtual void processRunHeader( LCRunHeader* run) ;

    virtual void processEvent( LCEvent * evt );

    virtual void end();

    virtual void conditionsChanged(lcio::LCCollection* col);
  protected:

    // processor parameters
    std::string _adcColName;
    std::string _parNameTriggerEvent;
    std::string _outputFilePath;
    std::string _connectionColName;
    std::string _referenceThresholdColName;
    std::string _skipCalibration;

#ifdef MULTI_DBINIT
    std::string _thresholdFolderName;
    std::string _rejectedFractionFolderName;
    std::string _rejectedFractionReferenceFolderName;
#endif

    FloatVec _signalBinning;
    FloatVec _purityList;

    float _gausRange;

    int _rebin;
    int _granFac;

  private:
    // internal functions & global variables

    lccd::LCCDTimeStamp _startTime;
    lccd::LCCDTimeStamp _endTime;

    void fillHist( TH1F* hist, LCEvent* evt);
    unsigned _runNumber;
    unsigned int _crate, _slot, _fe, _chip, _channel;
    TH1F* _sigHist;
    TH1F* _pedHist;

    bool _connectionAvailable;

    typedef std::map<float,float> ThresholdMap_t;
    ThresholdMap_t _referenceThresholds;

  };


}
#endif
