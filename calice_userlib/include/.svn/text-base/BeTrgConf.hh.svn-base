#ifndef BeTrgConf_hh
#define BeTrgConf_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
//CRP Use 'local' interface to LCGenericObjects
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
#define NLENGAND 4




// #define NINT  // see enum EIntValues
//#define NFLOAT 0
//#define NDOUBLE 0

using namespace lcio ;
using namespace CALICE;

namespace CALICE {

  /** The indices of all the stored values.
   */
  enum EBeTrfConfIntValues {kBeTrgConfIntValBoardId, kIntRecLabel,
			    kBeTrgConfIntValInputEnable,
			    kBeTrgConfIntValOscillatorEnable,
			    kBeTrgConfIntValGeneralEnable,
			    kBeTrgConfIntValOscillationPeriod,
		            kBeTrgConfIntValBurstCounter,
		            kBeTrgConfIntValConfiguration,
		            kBeTrgConfIntValFifoIdleDepth,
		            kBeTrgConfIntValBusyTimeout,
		            kBeTrgConfIntValBurstTimeout,
		            kBeTrgConfIntValExtBeamMode,
		            kBeTrgConfIntValInputInvert,
		            kBeTrgConfIntValQdrConfig,
		            kBeTrgConfIntValSequencerControl,
		            kBeTrgConfIntValAndEnable,
			    kNBeTrgConfIntValues=kBeTrgConfIntValAndEnable+NLENGAND};

  /** Stores the configuration data of the trigger.
   *
   * To acces the configuration:
   * <pre>
   *   void BrTrgCondChangeListener(LCCollection *col)  {
   *        assert (col->getNumberOfElements()==1)
   *        BeTrgConf conf(col->getElementAt(0));
   *
   *        UInt_t enable_mask=conf.getEnableMask();
   *   }
   * </pre>
   *
   * @sa ConditionsChangeDelegator
   * @author Götz Gaycken LLR (Ecole Polytechnique)
   * @date Sep  2005
   */

  //Useful information (extracted from the corresponding DAQ classe
  //Relation data<-> Words Registers
  //UtlPack  _inputEnable;       // Reg  1
  //UtlPack  _generalEnable;     // Reg  4
  //unsigned _oscillationPeriod; // Reg  6
  //unsigned _burstCounter;      // Reg  7
  //UtlPack  _configuration;     // Reg 15
  //unsigned _busyTimeout;       // Reg 16
  //unsigned _burstTimeout;      // Reg 17
  //unsigned _andEnable[4];      // Reg 19, 20, 21, 22
  //unsigned _extBeamMode;       // Reg 23
  //unsigned _inputInvert;       // Reg 24
  //unsigned _qdrConfiguration;  // Reg 25
  //unsigned _sequencerControl;  // Reg 28
  //This assignment is relevant for this version of the routine


    class BeTrgConf : public LCGenericObjectImplExt{

  
  public: 
    
      /** Default Constructor */
      BeTrgConf() {/*no op*/}; 
    /** Ctor.
     * @param boardID the packed board id (@ref BoardID). 
     */
    BeTrgConf(int boardID) {
      obj()->setIntVal( kBeTrgConfIntValBoardId , boardID  ) ;
    };

    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      BeTrgConf(LCObject* obj) : LCGenericObjectImplExt(obj) { } 

    /** set the packed board id.
     * @sa BoardID
     */
    BeTrgConf &setBoardID(int boardID) {obj()->setIntVal(kBeTrgConfIntValBoardId,boardID); return *this;};

    /**Information on the FE chip the data was received
     */
    int getBoardID() const {return getIntVal(kBeTrgConfIntValBoardId);};

    /** Set the Record Label*/
    void setRecordLabel(int label) {obj()->setIntVal(kIntRecLabel,label);};

    /** Return the Record Label */
    short getRecordLabel() const {
      return (short) ( getIntVal(kIntRecLabel) & 0xff);
    };



    /** Set the trigger enable bit mask.
     * For each aenabled trigger a bit is set.
     * The meaning of the bits is defined by conditions data.
     * @sa TriggerMask.
     */
    BeTrgConf &setInputEnableMask(int input_enable) {
      obj()->setIntVal(kBeTrgConfIntValInputEnable,input_enable); 
      return *this;
    };

    /**Get the trigger enable bit mask.
     * For each aenabled trigger a bit is set.
     * The meaning of the bits is defined by conditions data.
     * @sa TriggerMask.
     */
    int getInputEnableMask() const {return getIntVal(kBeTrgConfIntValInputEnable);};

      /** Set Oscillator Enable */
      BeTrgConf &setOscillatorEnable(int osc_enable) {
      obj()->setIntVal(kBeTrgConfIntValOscillatorEnable,osc_enable); 
      return *this;
    };

      /** Get Oscillator Enable */
    int getOscillatorEnable() const {return getIntVal(kBeTrgConfIntValOscillatorEnable);};


      /** Set General Enable */
      BeTrgConf &setGeneralEnable(int gen_enable) {
      obj()->setIntVal(kBeTrgConfIntValGeneralEnable,gen_enable); 
      return *this;
    };

      /** Get General Enable */
    int getGeneralEnable() const {return getIntVal(kBeTrgConfIntValGeneralEnable);};


    /** Set the oscillation period.
     */
    BeTrgConf &setOscillationPeriod(int oscillation_period) {
      obj()->setIntVal(kBeTrgConfIntValOscillationPeriod,oscillation_period); 
      return *this;
    };

    /**Get the oscillation period.
     */
    int getOscillationPeriod() const {return getIntVal(kBeTrgConfIntValOscillationPeriod);};


    /** Set the burst counter.
     */
    BeTrgConf &setBurstCounter(int burst_count) {
      obj()->setIntVal(kBeTrgConfIntValBurstCounter, burst_count); 
      return *this;
    };

    /**Get the burst counter.
     */
    int getBurstCounter() const {return getIntVal(kBeTrgConfIntValBurstCounter);};


    /** Set the configuration word.
     */
    BeTrgConf &setConfiguration(int config) {
      obj()->setIntVal(kBeTrgConfIntValConfiguration, config); 
      return *this;
    };

    /**Get the configuration word.
     */
    int getConfiguration() const {return getIntVal(kBeTrgConfIntValConfiguration);};

    /** Set the fifo idle depth.
     */
    BeTrgConf &setFifoIdleDepth(int fifo_idle_depth) {
      obj()->setIntVal(kBeTrgConfIntValFifoIdleDepth, fifo_idle_depth); 
      return *this;
    };

    /**Get the fifo idle depth.
     */
    int getFifoIdleDepth() const {return getIntVal(kBeTrgConfIntValFifoIdleDepth);};

    /** Set the busy time out.
     */
    BeTrgConf &setBusyTimeout(int busy_timeout) {
      obj()->setIntVal(kBeTrgConfIntValBusyTimeout, busy_timeout); 
      return *this;
    };

   /**Get the busy time out */
    int getBusyTimeout() const {return getIntVal(kBeTrgConfIntValBusyTimeout);};


    /** Set the burst time out.
     */
    BeTrgConf &setBurstTimeout(int burst_timeout) {
      obj()->setIntVal(kBeTrgConfIntValBurstTimeout, burst_timeout); 
      return *this;
    };

   /**Get the burst time out */
      int getBurstTimeout() const {return getIntVal(kBeTrgConfIntValBurstTimeout);};


    /** Set the andEnable
     */
    BeTrgConf &setAndEnable(int and_enable, int pos) {
      obj()->setIntVal(kBeTrgConfIntValAndEnable+pos, and_enable); 
      return *this;
    };

   /**Get the and Enable */
      int getAndEnable(int pos) const {return getIntVal(kBeTrgConfIntValAndEnable+pos);};

    /** Set the extBeam Mode
     */
    BeTrgConf &setExtBeamMode(int ext_beammode) {
      obj()->setIntVal(kBeTrgConfIntValExtBeamMode, ext_beammode); 
      return *this;
    };

   /**Get the extBeam Mode */
    int getExtBeamMode() const {return getIntVal(kBeTrgConfIntValExtBeamMode);};

    /** Set the input invert
     */
    BeTrgConf &setInputInvert(int input_invert) {
      obj()->setIntVal(kBeTrgConfIntValInputInvert, input_invert); 
      return *this;
    };

   /**Get the input invert */
    int getInputInvert() const {return getIntVal(kBeTrgConfIntValInputInvert);};


    /** Set the qdr configuration
     */
    BeTrgConf &setQdrConfiguration(int qdr_config) {
      obj()->setIntVal(kBeTrgConfIntValQdrConfig, qdr_config); 
      return *this;
    };

   /**Get the qdr configuration */
    int getQdrConfiguration() const {return getIntVal(kBeTrgConfIntValQdrConfig);};


    /** Set the sequencer control
     */
    BeTrgConf &setSequencerControl(int sequence_contr) {
      obj()->setIntVal(kBeTrgConfIntValSequencerControl, sequence_contr); 
      return *this;
    };

   /**Get the sequencer control */
    int getSequencerControl() const {return getIntVal(kBeTrgConfIntValSequencerControl);};



    /** Convenient print method 
     */
    void print(  std::ostream& os) {
      os << " board: " << BoardID(getBoardID()) 
	 << " Record Label=" << getRecordLabel()
	 << " enabled mask=" << std::hex << getInputEnableMask()
	 << " osc. enable=" << getOscillatorEnable()
	 << " general enable=" << getGeneralEnable()
	 << " osc. period=" << getOscillationPeriod()
	 << " burst counter=" << getBurstCounter()
	 << " configuration=" << getConfiguration()
	 << " fifo idle depth=" << getFifoIdleDepth()
	 << " Busy Timeout=" << getBusyTimeout()
	 << " Burst Timeout=" << getBurstTimeout()
	 << " and Enable 0=" << getAndEnable(0)
	 << " and Enable 1=" << getAndEnable(1)
	 << " and Enable 2=" << getAndEnable(2)
	 << " and Enable 3=" << getAndEnable(3)
	 << " ext Beammode=" << getExtBeamMode()
	 << " Input Invert=" << getInputInvert()
	 << " QDR Configuration=" << getQdrConfiguration()
	 << " Sequencer Control=" << getSequencerControl()
	 << std::endl;
    };

    // -------- need to implement abstract methods from LCGenericObject

    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"BeTrgConf" ;
    } 
  
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:board ID,i:record_label,i:enable_mask,i:oscillation_enable,general_enable,i:oscillation period,i:burst_counter,i:" ;
    }

  }; // class

}
#endif 
