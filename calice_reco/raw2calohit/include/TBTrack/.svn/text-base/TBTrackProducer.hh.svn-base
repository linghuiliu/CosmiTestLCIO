#ifndef TBTrackProducer_hpp
#define TBTrackProducer_hpp 1

#include "lcio.h"

#include "marlin/Processor.h"

#include "TrackFinder.hh"

#include "TBTrackBaseProcessor.hh"


class TBTrackProducer : public TBTrackBaseProcessor {
  
 public:
  
  virtual marlin::Processor* newProcessor() {
    return new TBTrackProducer;
  }
  
  TBTrackProducer() ;
  
  /** Called at the begin of the job before anything is read.
   */
  virtual void Init(){};
  
  /** Called for every run.
   */
  virtual void ProcessRunHeader(LCRunHeader *run){};
  
  /** Called for every event - the working horse.
   */
  virtual void ProcessEvent(LCEvent *evt); 
  
  /** Called after data processing for clean up.
   */
  virtual void End(){};


private:
  TBTrack::TrackFinder _finder;
  std::string _TDCHitColName;          /**< name of the Track output collection */

};

#endif
