#ifndef TBTrackMokkaCheck_hpp
#define TBTrackMokkaCheck_hpp

#include <string>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "lcio.h"

#include "marlin/Processor.h"

#include "TBTrackBaseProcessor.hh"


class TBTrackMokkaCheck : public TBTrackBaseProcessor {
  
 public:
  
  virtual marlin::Processor*  newProcessor() {
    return new TBTrackMokkaCheck;
  }
  
  TBTrackMokkaCheck();
  
  /** Called at the begin of the job
   */
  virtual void Init();
  
  /** Called for every run.
   */
  virtual void ProcessRunHeader( LCRunHeader* run );
  
  /** Called for every event - the working horse.
   */
  virtual void ProcessEvent( LCEvent * evt ); 

  /** Called after data processing for clean up.
   */
  virtual void End();

  virtual void initHists();

 private:
  TH1F* hNCol;
  TH1F* hNEnt;

  TH1F* hNumb[2][4];
  TH1F* hCell[2][4];
  TH1F* hPosn[2][4];
  TH1F* hPosz[2][4];
  TH1F* hEner[2][4];
  TH1F* hTime[2][4];
  TH1F* hTana[2][4];

};

#endif
