#ifndef TBTrackProducerCheck_hpp
#define TBTrackProducerCheck_hpp

#include <string>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "lcio.h"

#include "TBTrackBaseProcessor.hh"




class TBTrackProducerCheck : public TBTrackBaseProcessor {
  
 public:
  
  virtual Processor*  newProcessor() {
    return new TBTrackProducerCheck;
  }
  
  
  TBTrackProducerCheck() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void Init() ;
  
  virtual void initHists(unsigned p, bool t) ;
  
  /** Called for every run.
   */
  virtual void ProcessRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void ProcessEvent( LCEvent * evt ) ; 
  
  /** Called after data processing for clean up.
   */
  virtual void End() ;

  /** Method to write the histograms in a ROOT file.
   */
  /* virtual void SaveHistograms();*/



 private:
  TH1F* hNumb[2][17];
  TH1F* hPatt[2][17];
  TH1F* hPar0[2][17];
  TH1F* hPar1[2][17];
  TH1F* hErr0[2][17];
  TH1F* hErr1[2][17];
  TH1F* hCorr[2][17];
  TH1F* hProb[2][17];
  TH1F* hHits[2][17][4];
 
  TH2F* hNmb2[2][17];
  TH2F* hPrb2[2][17];

  TH1F* hMPr0[2][17];
  TH1F* hMPr1[2][17];
  TH1F* hMDf0[2][17];
  TH1F* hMDf1[2][17];

} ;

#endif



