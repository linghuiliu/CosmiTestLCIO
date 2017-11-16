#ifdef USE_LCCD
#ifndef TRIGGERHANDLERCALICE_H
#define TRIGGERHANDLERCALICE_H 1


#include "RtypesSubSet.h"
#include "TriggerHandler.hh"
#include "TriggerBits.hh"


#include <stdexcept>
#include <map>
#include <vector>
#include <string>
#ifdef BOUNDARY_CHECK
#  include <cassert>
#endif
#include <climits>
#include <algorithm>

#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "LCIOSTLTypes.h"
#include <iostream>

// Define this variable if the trigger handler should be a  singleton
//#ifdef TRIGGER_HANDLER_IS_SINGLETON


namespace CALDAQ {
  class ReadOutConfigurationBlock;
  class FeConfigurationBlock;
}

//A delegator class where functions can be registered in case
//conditions data do change
#include <ConditionsChangeDelegator.hh>
#include <MultipleConditionsChangeDelegator.hh>

namespace CALICE {

/** Implementation of TriggerHandler base class for calice purposes
  * Trigger conditions are read from a conditions data source or
  * from the event information. 
  * Database accesses and necessary event analysis
  * are completely hidden from the user.
  * The results are made availabe by access functions which
  * can be used to update event parameters
  * The triggerhandler can act as a singleton class and can 
  * in principle be plugged in at any point in the processors.
  * @author R. Pöschl DESY, major update by G.Gaycken (LLR) Aug. 2006
  * @date May 20 2005
  *
  */

class TriggerHandlerCalice: public TriggerHandler {


public:

  /** Destructor
   */
  virtual ~TriggerHandlerCalice() {/*no op*/}

 /**returns pointer to only instance of the TriggerHandlerCalice
   */ 
#ifdef TRIGGER_HANDLER_IS_SINGLETON
  static TriggerHandlerCalice* getInstance();
#endif

  /** This adds the trigger information to the event
   * (well it did in older versions, now it only calls the method
   * to get the fifo content )
   */
  virtual void setTrigger(lcio::LCEvent*);


  /** Search for the trigger main word.
   * The search is conducted starting at the main words in both time directions.
   * First in future direction then past.
   * The trigger handler has to be prepared first.
   * @sa setTrigger
   */
  unsigned int searchTriggerMainWord();
  
  /** Search the trigger history for triggers firing at and before the main word.
   * This will fill a map which contains for each position in the past history where a 
   * trigger was firing the trigger bits of all firing triggers. Also the event
   * trigger bits will be set.
   */
  void searchTriggerHistory();

  /** Get a list trigger and their positions wrt. to the main word  which fired before the main word (past).
   */
  const std::map<unsigned int, unsigned int> &getPreTriggerHistory() const {
    return _distanceToPreTrigger;
  }

  /** Get a list trigger and their positions wrt. to the main word  which fired after the main word (future).
   */
  const std::map<unsigned int, unsigned int> &getPostTriggerHistory() const {
    return _distanceToPostTrigger;
  }


  /** Trigger Validation. It checks whether the event was triggered
    * properly. Note, that this is fairly independent of the trigger
    * requested by the user in the method above.
    * currently it does nothing
    */
  virtual bool isValidTrigger(const lcio::LCEvent*);

  


  // #ifdef TRIGGER_HANDLER_IS_SINGLETON
  // need to implement interface from abstract base class
  /** A initialization method
    */  
  virtual void init(); 
  // #endif

  /** Another initialization method
   * @param col_trigger_assignment name of the trigger assignment conditions data.
   * @param col_trigger_check name of the trigger check conditions data.
   * @param colBeTrgConf name of the conditions data of the back-end trigger configuration.
   * @param col_name_readout_conf name of the conditions data of the readout configuration.
   * @param col_name_trgro_conf name of the conditions data of the trigger readout configuration.
   * @param col_name_fe_conf vector of names of the conditions data of the front-end configuration.
   * @param col_name_module_connection vector of names of the conditions data which describes the connections.
   *            between detector modules and front-ends (needed to find out about calibration mode).
   */  
  void init(const string& col_trigger_assignment, 
	    const string& col_trigger_check, 
	    const string& colBeTrgConf,
	    const string& col_name_readout_conf,
	    const string& col_name_trgro_conf,
	    const lcio::StringVec& col_name_fe_conf, 
	    const lcio::StringVec& col_name_module_connection); 

  /** Set the name of the trigger event collection.
   */
  void setColNameTriggerEvent(const std::string &col_name) {
    _colNameTriggerEvent=col_name;
    setTriggerEventClass();
  };

  /** Set the trigger event class BeTrgEvent/FeTrgEvent.
   */
  void setTriggerEventClass();


  /** A convenient print method to get trigger definitions
    * and assignment 
    */

  void printTriggerDefinitions(const lcio::LCEvent*); 

  /** Return the configuration mask 
    * All trigger inputs which are active for a given configuration
    * within a run <-> subsample of the principle mask 
    */

  int getTrgConfigurationMask() {return _confTriggerMask;}

  /** Return the principle trigger mask 
    * basically the or of all trigger inputs which 
    * could be active for the given run
    */
  int getPrincipleTriggerMask() {return _principleTriggerMask;}


  /** Return the Trigger Definitions Map (for those who need it) */
  //typedef std::map<int,string> TriggerDefinitionMap_t; 
  //std::map<int,string> getTriggerDefinitionMap() {return *_trdefMap;}


  /** Get the trigger configuration.
   */
  TriggerBits getTriggerConfiguration() const {return _conf_triggerBits;}

  /** Return the and enable word 
    * @todo in priniple there are four enable words
    * and in prenicple we need the enable word to a trigger bit in the
    * configuration mask*/ 

  TriggerBits getAndEnableBits(){return _andenable_triggerBits;}

  /** Get the event trigger bits.
   */
  TriggerBits getTriggerEvent() const {return _event_triggerBits;}

  /** Get the trigger bits which fire simultaniously with the main word but which are not insided the fifo range.
   */
  TriggerBits getOutOfRangeTriggerBits() const {return _outOfRange_triggerBits;}


  /** The mask and value pair which identifies a physical trigger.
   * Usually ther physical trigger is identified by one bit in the trigger word 
   * (or input enable mask for the configuration).
   * The physical trigger bits will not be export but the logical trigger to
   * which the physical trigger is assigned.
   * @sa TriggerDef_t
   */
  class TriggerValue_t {
  public:
    TriggerValue_t() : _mask(0) {};
    TriggerValue_t(unsigned int a_mask, short delay, unsigned short jitter) : _mask(a_mask), _delay(delay),_jitter(jitter) {};

    unsigned int mask() const {return _mask;};
    int delay() const {return _delay;};
    int jitter() const {return _jitter;};

    /** Compare two trigger values.
     * Needed, since the TriggerValue_t is used as a key for the trigger definition map.
     */
    bool operator<(const TriggerValue_t &a) const { return ( mask() < a.mask() ? true : false );} 

    void set(unsigned int a_mask) {_mask=a_mask;};
    
    unsigned int _mask;
    short          _delay;
    unsigned short _jitter;
  };

  /** Simple class which contains the data of logical trigger.
   * The logical triggers have a name and a bit which will be added to the event 
   * header. The physical triggers are mapped to the logcal trigger.
   * @sa TriggerValue_t
   */
  class TriggerDef_t {
  public:
    TriggerDef_t(const std::string &name) : _name(name), _bitMask(0) {}

    /** the name of a trigger.
     */
    const std::string &name() const { return _name;}

    /** Set the bit mask which will appear in the event header.
     */
    void setBitMask(unsigned int bit_mask) {_bitMask=bit_mask;}

    /** the bit mask which will appear in the event header.
     */
    unsigned int bitMask() const { return _bitMask; }
    
  private:
    std::string _name;
    unsigned int _bitMask;
  };

  typedef std::map< TriggerValue_t, TriggerDef_t > TriggerDefinitionMap_t; 
  const TriggerDefinitionMap_t &getTriggerDefinitionMap() const {return _trdefMap;}
  
  /** Return the Fifo Content */

  const IntVec &getTriggerFifoContent() const {return _theTriggerFifoCont;}

  /** Return the number of usable fifo words.
   * The last two words of the fifo are used differently. So, they should not be used in 
   * history scans. 
   */
  unsigned int getUsedFifoDepth() const {return (_theTriggerFifoCont.size()>=2 ? _theTriggerFifoCont.size()-2 : 0);}

  /** Return if the trigger fifo is not empty.
   */
  bool hasTriggerFifo() const {return (_theTriggerFifoCont.size()>0);}
 
  /** Return true if the trigger fifo is deep enough.
   * @todo what is the required fifo depth ? 
   */
  bool isTriggerFifoDepthGood() {return (_theTriggerFifoCont.size()>static_cast<unsigned int>(_main_word+1));}

  /** Return the content of the main word in the Fifo*/
  inline const int getTriggerFifoContMainWord() {
    if(_this_trigger_main_word > 0) {
      if( _theTriggerFifoCont.size() > static_cast<unsigned int>(_this_trigger_main_word)) return
											     _theTriggerFifoCont.at(static_cast<unsigned int>(_this_trigger_main_word));           
      else {
	std::cout << "Main word is at " << _this_trigger_main_word << " but size of fifo is: " << _theTriggerFifoCont.size() << std::endl;
	std::cout << "Will return 0" << std::endl;
        return 0; 
      }
    }
    return 0;
  }


  /** Return a given word in the the Fifo Content */
  int getTriggerFifoContaWord(unsigned int theWord) const {

    if( _theTriggerFifoCont.size() > theWord) return _theTriggerFifoCont.at(theWord);
    else {
      std::cout << "Required word is " << theWord << " but size of fifo is: " << _theTriggerFifoCont.size() << std::endl;
      std::cout << "Will return 0" << std::endl;
    }   
    return 0;
  }


  /** Return the main word itself 
   * i.e. the word in fifo in which we look mainly for the
   * event trigger 
   */
  int getMainWord() const { return _main_word;}


  /** Return the trigger main word smeared by +-1.
   */
  const unsigned int getSmearedTriggerMainWord() {
#ifdef TRIGGER_HANDLER_IS_SINGLETON
    if(!_isInitialized) init();
#endif

    // todo: only if boundary check is enabled ?
    unsigned int used_fifo_depth=getUsedFifoDepth();
    if(_this_trigger_main_word < 0 ) return 0;
    if (used_fifo_depth < static_cast<unsigned int>(_this_trigger_main_word)) return 0;

    unsigned int smeared_trigger_word = _theTriggerFifoCont.at(static_cast<unsigned int>(_this_trigger_main_word));
    if (_this_trigger_main_word>0)  smeared_trigger_word |= _theTriggerFifoCont.at(_this_trigger_main_word-1);
    if (static_cast<unsigned int>(_this_trigger_main_word+1) < used_fifo_depth ) 
      smeared_trigger_word |= _theTriggerFifoCont.at(_this_trigger_main_word+1);

    return smeared_trigger_word;
  }

  /** Return the status of the general s/w trigger */
  inline bool isGenSWTriggerOn() const {return _swTriggerOnGen;}

  /** Is at least on one baord a s/w trigger configured */
  inline bool isIndSWTriggerOn() const {return _swTriggerOnInd;}

  /** Is there somewhere a software trigger? */
  inline bool isSWTriggerOn() const { return (_swTriggerOnInd || _swTriggerOnGen);}

  /** Return the status of the calibration chip.*/
  bool isCalibrationOn() const {return _calibrationOn;}


  /** Return the number of (Trigger) Configuration Changes */
  unsigned int getNConfigurationChanges() const { return _nConfigurationChanges;}

  /** Get the number of events with missing trigger event data.*/
  unsigned int getNEventsWithoutTriggerEventData() const {return _noTriggerEvents;}


  //More getter functions to come ... ????  

#ifdef TRIGGER_HANDLER_IS_SINGLETON
  private:
#endif
  /** The private constructor
    */
  TriggerHandlerCalice();  

  private:
   /** This method verifies the validity of the
     * current trigger configuration mask
     * Was used to manage trigger changes in older versions
     */
   void TriggerConditionsListener();

  
  /** This method creates the map of the trigger definition
    * plus the so called principle trigger mask, 
    * the trigger bits which have well assigned signals
    */
  void CreateTriggerDefinitionMap(EVENT::LCCollection *TrAssCol);


  /** The listener which will be called in case the trigger
   * assignemnt changes 
   */

   ConditionsChangeDelegator<TriggerHandlerCalice> _triggerAssignment;

  /** This method extracts the environmental setting of the 
    * trigger, the BeTrgConfigurationsData from a conditions data source. 
    */

  void ExtractTriggerEnvironment(EVENT::LCCollection *confCol);

  /** The listener which will be called in case the trigger
   * configuration changes 
   */
   ConditionsChangeDelegator<TriggerHandlerCalice> _triggerConfChange;

  /** Update the configuration trigger bits which will be attached to the event header.
   * This method should be called after trigger configuration changes,
   * trigger assignment changes, readout configuration changes (soft trigger),
   * front-end configuration changes (calib).
   * @sa 
   */
  void updateTriggerConfBits();

  /** This method extracts the info on how to verify and get the
    * trigger decision,
    * i.e. which words have to be checked in the fifo to get the trigger decision  
    * and which words have to be scanned in the fifo to validate the trigger decision
    */

  void ExtractTriggerCheckConditions(EVENT::LCCollection *checkCol);



  /** The listener which will be called in case the trigger
   * check conditions change 
   */
   ConditionsChangeDelegator<TriggerHandlerCalice> _triggerCheck;


  /** Memorise new front-end configuration conditions dataa.
   */
  void feConfigurationChange(EVENT::LCCollection *feConfCol, unsigned int set_i);
  std::vector<LCCollection *> _feConfCol;
  std::vector<CALICE::MultipleConditionsChangeDelegator<TriggerHandlerCalice> > _feConfChange;

  /** Analyse the front-end configuration data and set calibration on flags accrodingly.
   * Only connected front-ends are consdiered. The calibration flags are set per
   * sub detector and a global flag is set if one of the calibration flag of one 
   * sub detector is set.
   */
  bool checkForCalibrationOn();

  /**Variables which contains the status of the calibration bit in
   *the FeConfiguration Data*/

  std::vector<bool> _calibrationOnVec; /**< calibration on flags per sub detector. */
  bool _calibrationOn;    /**< Global calibration on set of calibration on is set for
			       at least one sub detector.*/

  /** Memorise readout configuration data.
   * The readout configuration data is needed to identify software triggers.
   */
  void readoutConfChange(EVENT::LCCollection *roConfCol);

  /** The listener which will be called in case the ReadOut Configuration
   * Changes. Like that we can indentify software triggers. 
   */
  
  ConditionsChangeDelegator<TriggerHandlerCalice> _roConfChange;

  lcio::LCCollection *_roConfCol;                   /**< points to the last readout configuration conditions data.*/



  /** This method checks for individual software triggers i.e. on each
   *  board
   *\todo currently a software trigger is declared whenever a s/w trigger is enabled on one board this has to be individualized
   */

  void ExtractGenSoftTrigConfiguration(EVENT::LCCollection*);


  /** The listener which will be called in case the TrgReadoutConfiguration
   * Changes. Like that we can indentify software triggers. 
   */

  ConditionsChangeDelegator<TriggerHandlerCalice> _trgroConfChange;

  /** Analyse the readout configuration data and set the software trigger flags accordingly.
   * If at least one front-end soft trigger or the back-end trigger is activated for one 
   * front-end which is connected to a sub-detector or one crate which contains front-ends
   * which are conntected to a sub detector, then the soft trigger flag of the corresponding
   * sub detector is set.
   * If the soft trigger of one sub detector is set, then also the global soft trigger
   * flag is set.
   */
  bool checkForSoftwareTriggerOn();

  /**Variable which contains the information whether a s/w
    trigger is configured at least in one board or not*/

  bool _swTriggerOnInd; 

  std::vector< unsigned int > _swTriggerOnVec; /**< Soft trigger flags per sub detector. */

  /**Variable which contains the information whether a s/w
    trigger is configured in the central trigger  board */

  bool _swTriggerOnGen; 


  /** The data members which contain the info about the words which are
    * to be checked 
    */ 
  int _main_word;
  int _tolerance; 
  int _word_up;
  int _word_down;

  /** The Trigger Definition map
    */
  TriggerDefinitionMap_t _trdefMap;
   
  /** The trigger mask as defined by the Trigger Definition map
    */
  int _principleTriggerMask; 


  /** The trigger configuration as defined in the configuration data,
   *  CrcBeTrgConfiguration Data
   */
  int _confTriggerMask; 


  /** The And Enable word(0), only by this word we can recognize which 
    * Triggers are suppose to fire in case conincidences are setup  
    */

  int _andEnable0;

  /** The generic mask. We need this since one bit comes
   *  always along with the trigger information in an event
   *  which is not present in the configuration data
   */
  int _genericTriggerMask;

  //  std::vector<std::pair<UInt_t, UInt_t> > _triggerMaskValue[kNTriggerTypes];
  std::map<std::string,unsigned int> _knownTrigger;

  /** This method extracts the eventwise trigger data
    */
   void PrepareTriggerEventData(const LCEvent* evt);

  /** The famous fifo to verify the trigger quality
    */
  IntVec _theTriggerFifoCont;


#ifdef TRIGGER_HANDLER_IS_SINGLETON
  /** The instance of the TriggerHandler
   */
  static TriggerHandlerCalice* _thandler_instance;
#endif

  lcio::LCTime _theTime;

  /** Some check variables
   */
  bool _isInitialized;   
  
  /** Object which holds the event trigger(s) */
  TriggerBits _event_triggerBits;
  
  /** Object which holds the configured trigger(s) */
  TriggerBits _conf_triggerBits;

  /** Object which holds the trigger(s) enabled for coincidence*/
  TriggerBits _andenable_triggerBits;
  
  /** Object which holds the trigger(s) which would be expected outside of the fifo depth. */
  TriggerBits _outOfRange_triggerBits;

  /** variable which holds the result of the trigger polling*/
  bool _triggerPollTimeout;
  
  /** A counter of TriggerConfiguration Changes (suggested by Goetz)
   */
  unsigned int _nConfigurationChanges;
  unsigned int _lastConfigurationChange;
  
  typedef std::vector<unsigned int>            FrontEndList_t; /**< Collection type which can hold sub detector ids per front end of a slot.*/
  typedef std::vector<FrontEndList_t>          SlotList_t;     /**< Collection type which can hold front-end collections per crate slot.*/
  
  /** Samll helper class which contains a collection of slots and which remebers all the sub detector ids conencted to it.
   */
  class Crate_t {
  public:
    Crate_t() : _subDetID(UINT_MAX),_oneDetectorOnly(true) {};
    
    Crate_t(unsigned int sub_det_i) : _subDetID(sub_det_i),_oneDetectorOnly(true) {};
    
    /** Return true if this crate is connected to at least on known detector.
     */
    bool isUsed() const {
      return !_subDetID.empty();
    }
    
    /** Return true if the crate is connected to only one sub detector.
     */
    bool isUsedByMultipleSubDetectors() const {return _subDetID.size()>1;};
    
    /** Return the ID of the sub detector which is connected to this crate.
     * @sa set
     */
    void setSubDetectorID(unsigned int a_sub_dector) {
      _subDetID.push_back(a_sub_dector);
    };
    
    /** Return true if the crate is conencted to the sub detector.
     */
    bool isConnectedToSubDetector(unsigned int a_sub_dector) const {
      return std::find(_subDetID.begin(), _subDetID.end(), a_sub_dector ) != _subDetID.end();
    }
    
    /** Get the slotlist for read/write access.
     */
    SlotList_t &slotList()             {return _slots;};
    
    /** Get the slotlist for read only access.
     */
    const SlotList_t &slotList() const {return _slots;};
    
  private:
    std::vector<unsigned int> _subDetID;
    bool         _oneDetectorOnly;
    SlotList_t   _slots;
  };
  
  typedef std::map<unsigned int, Crate_t >  CrateList_t; /**< Collection of crates.*/
  
  CrateList_t _connections; //* connection tree */
  std::vector<unsigned int> _nFrontEnds; /**< Number of front-ends connected to one sub detector.*/
  
  /** listen for module connection changes and build a connection tree to find out about connected front-ends.
   */
  void moduleConnectionChange(EVENT::LCCollection *module_connection_col, unsigned int set_i);
  std::vector<CALICE::MultipleConditionsChangeDelegator<TriggerHandlerCalice> > _moduleConnectionChange;
  
  /** Print the module connections to the screen (debug).
   * This method is indented for debug purposes only.
   */
  void printConnections();
  
  std::string _colNameTriggerEvent; /**< Name of the trigger event collection.*/
  
  unsigned int _errorCounter;
  unsigned int _noTriggerEvents;
  
  enum ETrgEventClass {kFeTrgEvent,kBeTrgEvent,kUnknown} ;
  ETrgEventClass _trgEventClass ;
  bool _verbose;
  
  int _this_trigger_main_word; /**< The trigger main word of this event.*/
  unsigned int _this_trigger_length;    /**< The width of the trigger enable pulse.*/
  
  std::map<unsigned int,unsigned int> _distanceToPreTrigger;
  std::map<unsigned int,unsigned int> _distanceToPostTrigger;
  
};
  
}
#endif

#endif //USE LCCD
