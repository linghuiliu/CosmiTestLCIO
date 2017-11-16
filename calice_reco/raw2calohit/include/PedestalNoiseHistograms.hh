#ifndef _PedestalNoiseHistograms_hh_
#define _PedestalNoiseHistograms_hh_

#include <string>
#include <VRawADCValueProcessor.hh>
#include <lcio.h>
#include <EVENT/LCCollection.h>
#include <histmgr/Key_t.hh>


// FIXME: Should go into another namespace (?)
namespace CALICE {

class PedestalNoiseHistograms : public VRawADCValueProcessor
{
public:
  Processor*  newProcessor() { return new PedestalNoiseHistograms ; }

  PedestalNoiseHistograms();
    
  ~PedestalNoiseHistograms() {};
    
    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
  void init();

  /** Called for every run, e.g. overwrite to initialize run dependent 
   *  histograms.
   */
  void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse.      */
  void processEvent( LCEvent * evtP );

  void end();

protected:

  void moduleTypeChanged(lcio::LCCollection* col);
  void moduleLocationChanged(lcio::LCCollection* col);
  void moduleConnectionChanged(lcio::LCCollection* col);
  void detectorChanged();
  void createHistograms(); 

  std::string _cellParameterCollectionName;

  IntVec  _eventPar;
  histmgr::Key_t _histogramGroupKey;
  lcio::FloatVec _noiseHistPar;
  lcio::FloatVec _adcHistPar;

  // 2d collections of 1D histograms
  //  enum EH1Col2DType {kH1Col2DNoise,kH1Col2DPedestal,kNH1Col2D};
  //  LCCollection *_histCol2D[kNH1Col2D];
  enum EH1Type {kH1Noise, kH1Pedestal, kNH1};
  histmgr::Key_t _histKey[kNH1];

  UInt_t _missingCellParameters;
};


}
#endif
