#ifndef _SquareFinder_hh_
#define _SquareFinder_hh_

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include <RtypesSubSet.h>
#include <string>
#include <iostream>
#include <marlin/Processor.h>
#include <vector>

#ifndef WITH_CONTROL_HISTOGRAMS
#  define WITH_CONTROL_HISTOGRAMS
#endif

#ifdef WITH_CONTROL_HISTOGRAMS
#  include "histmgr/Key_t.hh"
#endif

// FIXME: Should go into another namespace (?)
namespace CALICE {

  /** Processor to search for events where the whole border of a wafer is firing.
   */
  class SquareFinder : public marlin::Processor
  {
  public:
    Processor*  newProcessor() { return new SquareFinder ; }

    SquareFinder();
    
    ~SquareFinder() {};
    

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent 
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse. 
     */
    void processEvent( LCEvent * evtP );

    void end();

  protected:
    Int_t   _nHitsMin;                /**< minimum number of hits on the border of the wafer */
    Int_t   _nInsidedHitsMax;         /**< maximum number of hits inside the wafer.*/  
    std::string _colName;             /**< name of the hit input collection (Marlin processor parameter).*/
    std::string _clusterColName;      /**< name of the square cluster collection.*/
    std::string _squareFlagName;      /**< name of the event parameter which flags square events.*/
    FloatVec    _signalHistPar;       /**< binning of the border and total border signal histograms.*/

    UInt_t _nSquareEvents;            /**< Number of square event candidates.*/
    UInt_t _nEvents;                  /**< Total number of events with hits.*/

  private:

#ifdef WITH_CONTROL_HISTOGRAMS
    histmgr::Key_t _histGroupKey;      /**< Key for the histogram group.*/
    histmgr::Key_t _padIndexHistKey;   /**< Key for the pad index histograms.*/
    histmgr::Key_t _waferIndexHistKey; /**< Key for the wafer index histograms.*/
    histmgr::Key_t _nSquareHistKey;    /**< Key for the number-of-squares histogram.*/
    histmgr::Key_t _borderSignalHistKey;    /**< Key for the signals on the border of the wafer histogram.*/
    histmgr::Key_t _borderTotalSignalHistKey;    /**< Histogram key for the total signal of the border.*/
#endif

  };


}
#endif
