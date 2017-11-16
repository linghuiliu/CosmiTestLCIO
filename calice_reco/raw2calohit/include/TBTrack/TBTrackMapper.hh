#ifndef _TBTrackMapper_hh_
#define _TBTrackMapper_hh_

#include <string>
#include <iostream>

#include "lccd/ConditionsMap.hh"

#include "TBTrackBaseProcessor.hh"
#include "TdcConnection.hh"

//namespace CALICE {
  
class TBTrackMapper : public TBTrackBaseProcessor {

public:
  /**  Default constructor
   */
  TBTrackMapper();
  
  /** Initialize the job - called at the end of the
   *    TBTrackBaseProcessor::init  method
   */
  void Init();
  
  /**  Process a RunHeader - called at the end of the
   *    TBTrackBaseProcessor::processRunHeader  method
   */
  void ProcessRunHeader( LCRunHeader* run);
  
  /**  Process an Event - called at the end of the 
   *    TBTrackBaseProcessor::processEvent  method  
   */
  void ProcessEvent( LCEvent * evtP );

  /**  End of job - called at the end of the
   *     TBTrackBaseProcessor::end  method
   */
  void End();

  Processor*  newProcessor() { return new TBTrackMapper; }
  

private:
  //  typedefs: for convenience, internally used only
  typedef std::map< int, CALICE::TdcConnection > mMap_t;
  typedef lccd::ConditionsMap< int, CALICE::TdcConnection > cMap_t;
  typedef std::vector< int > value_t;
  typedef std::map< int, value_t > vMap_t;

  //  private methods
  void reset();                            // reset private data members
  void findEdges( LCEvent * evtP, EVENT::LCCollection* );  // find signal son a TDC line
  void findHits();                         // find hits from the TDC signals
  EVENT::LCCollection* outputCollection( const vMap_t& );  // provide output collection

  //  private data members
  //    steering parameters:
  std::string _TDCHitColName;   // output hit collection
  std::string _mappingColName;  // name of the conditions collection for the
                                //  TDC-to-channel mapping
  int _TDC_Upper_Limit;         // sanity interval for the TDC range
  int _TDC_Lower_Limit;         // according to trigger start the first sensible TDC time
  bool _TDC_CAEN1290;            //remember which TDC to switch between CEAN 790 and new 1290
  //    data container
  cMap_t* _mapping;      //  the conditions data for the TDC-to-channel mapping
  vMap_t _edges;         //  container for the TDC signals
  vMap_t _hits;          //  container for the hits
  bool _twoLines;        //  flag whether the reco is differential
  bool _firstEvent;      //  flag for the first event in a run
};

//}

#endif
