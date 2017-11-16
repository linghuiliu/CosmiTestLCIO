#ifndef TriggerSim_h
#define TriggerSim_h 1

#include <string>

#include "marlin/Processor.h"
#include "lcio.h"
#include "EVENT/SimCalorimeterHit.h"


/** Processor to generate trigger bits in case of Monte Carlo files
    @author David Ward
    @author Benjamin.Lutz@desy.de
    @date Feb 2010
    @date 18 Feb 2010 add trigger mode (BL)
*/
class TriggerSim : public marlin::Processor {

public:

  virtual Processor*  newProcessor() { return new TriggerSim ; }


  TriggerSim() ;

  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;

  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;

  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ;

  /** Called after data processing for clean up.
   */
  virtual void end() ;

private:

protected:

  enum ETriggerSimMode { kSimTriggerMode10x10Coincidence,   /**< 10x10 coincidence is used as beam bit*/
                         kSimTriggerMode20x20,              /**< 20x20 is used as beam bit*/
                         kSimTriggerMode100x100Coincidence, /**< 100x100 coincidence is used as beam bit*/
                         kNSimTriggerModes };

  int _simTriggerMode;
  int _simTriggerModeParameter;

  int _nRun;         /**<runs counter*/
  int _nEvt;         /**<event counter*/
  int _nSc1_10x10;   /**<number of events passing through the scintillator 10x10 SC1 */
  int _nSc2_10x10;   /**<number of events passing through the scintillator 10x10 SC2 */
  int _nSc1_3x3;     /**<number of events passing through the scintillator 3x3 SC1 */
  int _nSc2_3x3;     /**<number of events passing through the scintillator 3x3 SC2 */
  int _nSc1_100x100; /**<number of events passing through the scintillator 100x100 SC1 */
  int _nSc2_100x100; /**<number of events passing through the scintillator 100x100 SC2 */
  int _nVetoTrigger; /**<number of events passing through the veto trigger*/
  int _nVeto_20x20;  /**<number of events passing through the 20x20 veto counter*/
  int _nBeamTrigger; /**<number of beam trigger events*/

  std::string _DetectorName; /**<detector name*/
} ;

#endif



