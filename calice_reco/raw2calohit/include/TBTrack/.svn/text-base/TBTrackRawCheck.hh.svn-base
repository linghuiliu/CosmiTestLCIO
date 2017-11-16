#ifndef TBTrackRawCheck_hpp
#define TBTrackRawCheck_hpp

#include <string>
#include <fstream>

#include "TH1F.h"
#include "TH2F.h"

#include "lcio.h"

#include "marlin/Processor.h"

#include "TBTrackBaseProcessor.hh"


using namespace lcio ;
using namespace marlin ;
using namespace TBTrack;


class TBTrackRawCheck : public TBTrackBaseProcessor {
  
 public:
  
  virtual Processor*  newProcessor() {
    return new TBTrackRawCheck;
  }
  
  
  TBTrackRawCheck() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void Init() ;
  
  /** Called for every run.
   */
  virtual void ProcessRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void ProcessEvent( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void End() ;

  
  virtual void initHists(bool real) ;


  /** Method to write the histograms in a ROOT file.
   */
  /* virtual void SaveHistograms();*/



 private:
  bool _cern;

  TH1F* hDist[16];
  TH2F* hDi2d[16];
  TH1F* hDi2p[16];
  TH1F* hDi2m[16];

  /*

  TH1F* hNCol;
  TH1F* hNEnt;

  TH1F* hNumb[2][4];
  TH1F* hDist[2][4];
  TH1F* hSepr[2][4];

  TH1F* hLpmL[2][6];
  TH2F* hLvsL[2][6];

  TH1F* hDiff[2][4];
  */
} ;

#endif



