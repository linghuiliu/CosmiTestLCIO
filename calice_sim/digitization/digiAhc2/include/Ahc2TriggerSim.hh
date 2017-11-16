#ifndef AHC2TRIGGERSIM_HH
#define AHC2TRIGGERSIM_HH 1

#include <string>

#include "marlin/Processor.h"
#include "lcio.h"
#include "EVENT/SimCalorimeterHit.h"

#include "BitSet.hh"

using namespace lcio ;
using namespace marlin ;
using std::vector;

namespace CALICE
{
  /**
  * @brief Class for setting trigger bits in the event for MC
  *
  *
  * @author eldwan.brianne@desy.de
  * @version 1.0
  * @date January 2017
  */

  class Ahc2TriggerSim : public Processor {

  public:

    virtual Processor*  newProcessor() { return new Ahc2TriggerSim ; }

    /**Constructor
    */
    Ahc2TriggerSim() ;

    /**Destructor
    */
    ~Ahc2TriggerSim() {};

    /**Initialise
    */
    virtual void init() ;

    /**Process header
    */
    virtual void processRunHeader( LCRunHeader* run ) ;

    /**Process event (the working horse)
    */
    virtual void processEvent( LCEvent * evt ) ;

    /**Check
    */
    virtual void check( LCEvent * evt ) ;

    /**End of processing
    */
    virtual void end() ;

    /**Print Parameters
    */
    virtual void printParameters();

  protected:

    enum ETriggerSimMode {
      kSimTriggerMode100x100Coincidence=1,   /**< 100x100 coincidence is used as beam bit*/
      kSimTriggerMode500x500Coincidence=2, /**< 500x500 coincidence is used as beam bit*/
    };

    ETriggerSimMode _simTriggerMode;
    int _simTriggerModeParameter;

    int _nSc1_100x100; /**<number of events passing through the scintillator 100x100 SC1 */
    int _nSc2_100x100; /**<number of events passing through the scintillator 100x100 SC2 */
    int _nSc1_500x500; /**<number of events passing through the scintillator 500x500 SC1 */
    int _nSc2_500x500; /**<number of events passing through the scintillator 500x500 SC2 */
    int _nBeamTrigger; /**<number of beam trigger events*/

    std::string _DetectorName; /**<detector name*/

    int _nRun ; /**<run number*/
    int _nEvt ; /**<evt number*/
  };

  /** A small class which allow for the easy analysis of core trigger
  * information. It allows the user to test which trigger was set in
  * a given event. The information has to be shipped as an event parameter.
  * @author eldwan.brianne@desy.de
  * @version 1.0
  * @date January 2017
  */

  class Ahc2TriggerBits : public BitSet
  {
    public :

    enum EAhc2TriggerBitsNo{
      kSC1_100x100, /* bit for SC1 100x100 */
      kSC2_100x100, /* bit for SC2 100x100 */
      kSC1_500x500, /* bit for SC1 500x500 */
      kSC2_500x500, /* bit for SC2 500x500 */
      kBeam, /* bit for fold coincidence */
      kNoTriggers }; /* just count number of bits */

      Ahc2TriggerBits() : BitSet(kNoTriggers) {};
      Ahc2TriggerBits(const int trigger_bits) : BitSet(kNoTriggers, trigger_bits) {};

      ~Ahc2TriggerBits(){};

      /*Set Methods */

      void setBeamTrigger( const bool state = true ) { setBit(kBeam, state); }
      void setSc1_100x100( const bool state = true ) { setBit(kSC1_100x100, state); }
      void setSc2_100x100( const bool state = true ) { setBit(kSC2_100x100, state); }
      void setSc1_500x500( const bool state = true ) { setBit(kSC1_500x500, state); }
      void setSc2_500x500( const bool state = true ) { setBit(kSC2_500x500, state); }

      /*Get Methods */

      bool isBeamTrigger() const { return getBit(kBeam); }
      bool isSc1_100x100() const { return getBit(kSC1_100x100); }
      bool isSc2_100x100() const { return getBit(kSC2_100x100); }
      bool isSc1_500x500() const { return getBit(kSC1_500x500); }
      bool isSc2_500x500() const { return getBit(kSC2_500x500); }
    };
  }
  #endif
