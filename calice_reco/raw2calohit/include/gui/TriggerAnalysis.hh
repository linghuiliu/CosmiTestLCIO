#ifndef _TriggerAnalysis_H_
#define _TriggerAnalysis_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CALICEGUI
class TriggerDisplay;
#endif

#include <TriggerProcessorBase.hh>
#include <string>
#include <histmgr/Key_t.hh>

namespace CALICE {


  /** Search for good trigger words.
   * 
   */
  class TriggerAnalysis : public CALICE::TriggerProcessorBase
  {
  public:
    TriggerAnalysis();
    ~TriggerAnalysis();
    
    
    Processor*  newProcessor() { return new TriggerAnalysis ; }
    
    /** Set up conditions data handler, verify parameters et.c
     */
    void init();
    
    //  /** Called for every run (does nothing.)
    //  */
    //  void processRunHeader( LCRunHeader* run) { } 

    /** Search good trigger words.
     * If no good trigger word is found a damaged flag is set in the event
     * header.
     */
    void processEvent( LCEvent * evtP );
    
    void end();
        
  private:
         
#ifdef HAVE_CALICEGUI
    int _visualise;                 /**< set to non zero to visualise the hits and the fitting*/
    TriggerDisplay *_display;
#endif
    std::string  _parNameTriggerMainWord;         /**< Par. name of the trigger main word.*/

    unsigned int _nEvents;
    unsigned int _missingTriggerEventData;

    histmgr::Key_t       _histGroupKey;               /**< Key for the histogram group.*/
    histmgr::Key_t       _histTriggerOnPosisitonKey;  /**< Key for the trigger on position histograms. */
    histmgr::Key_t       _histTriggerLengthKey;       /**< Key for the trigger pulse length histograms. */

  };
}
#endif
