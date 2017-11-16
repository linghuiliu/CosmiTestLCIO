#ifndef BmlEventData_HH
#define BmlEventData_HH 1


#include <iostream>

#include "lcio.h"
#include "IMPL/LCEventImpl.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
#include "UTIL/LCTime.h"
#include <sys/time.h>
#include <map>
#include "collection_names.hh"
#include "BmlEventDataSup.hh"
#include "BeTrgConf.hh"
#include <vector>

//a limit on the maximal number of signals per wire
//Frankly, speaking this is mainly such that we can squeeze the
//sequence of (useless) booleans of the starttime indicator
//for a given measurement into a 32 bit word. But a wire with 32 signals is really doubtful
//I'd say 
#define RINGCHNLIMIT 32

using namespace std;
using namespace lcio;
using namespace CALICE;



namespace CALICE {

   
  //BmlEventDataSup* BmlEventData::_bml_event_sup=0;

  //This map will contain all the tdc measurements of a given board
  typedef std::map< unsigned int, std::vector< std::pair<bool, int> > > TDCChannelContainer_t;


/** The indices of all the stored values.*/

  enum EBmlEventIntValues {kBmlEventBoardID, kBmlEventBaseAddress,  
                           kBmlEventRecordLabel, 
                           kBmlEventStatusRegister, 
                           kBmlEventNumberOfWords,
                           kBmlEventgeoAddress,
                           kBmlEventeventNumber,
                           kBmlEventStatus,
                           kBmlEventEventDataCounter,
                           kBmlEventNumberOfSignalChannels,
                           kBmlEventIntValues};


/**Class to store the BmlEventData. This is tailored to
 * the data extracted from the Caen767 TDC. 
 *  @author R. P�schl (LAL Orsay)
 *  @date Jul 24 2006
 */

class BmlEventData : public LCGenericObjectImplExt 
{
  

   public: 
  

  /** Default Constructor */
      BmlEventData() {
	//do some initialization
	setBoardID(0);
        setBaseAddress(0);
        setRecordLabel(0);
        setStatusRegister(0);
        setNumberOfWords(0);
        setGeoAddress(0); 
        setEventNumber(0); 
        setStatus(0); 
        setEventDataCounter(0);
        setNumberOfSignalChannels(0);
        _bml_event_sup = new BmlEventDataSup;
        _isSupInitialised=true;
      }; 

//............................................................................
   /**A copy constructor
    */
   BmlEventData(LCObject* obj) : LCGenericObjectImplExt(obj) 
   { 
     //_bmlEventDataSup=obj;
     _isSupInitialised=false;
   } 
//............................................................................
   /**The destructor 
    */
   virtual ~BmlEventData() 
   { 
    
   }
//............................................................................  
   /** Add supplementary information:
      Feed information not available in the actual BmlEventData to the object.
      This is necessary to handle more than 1 TDC type, in fact all information from TDCs others than the Caen767 will be
      handled as supplementary information, this ensures full backward compatibility and requires minimal code changes if
      information from other TDC is requested.*/
  void addSupplementaryInformation(LCEvent* aEvt, unsigned int ielm) {
    //Try to retrieve the supplementary information
    
    if(_bml_event_sup) delete _bml_event_sup;
    try {
      LCCollection* gentdcsupcol(aEvt->getCollection(COL_TDCSUP));
      //std::cout << "BmlEventSup Collection Size: " << gentdcsupcol->getNumberOfElements() << std::endl;
      _bml_event_sup = new BmlEventDataSup(gentdcsupcol->getElementAt(ielm));
      //_bmlEventDataSup(aEvt->getCollection(COL_TDCSUP)->getElementAt(ielm));
      _isSupInitialised=true;
    } catch (lcio::DataNotAvailableException err) {
      if(!_isWarning3Printed) {
	std::cout << "Warning: BmlEventData - Supplementary information not contained in event: " << aEvt->getEventNumber() << " of run: " << aEvt->getRunNumber() << std::endl;
        _isWarning3Printed=true;
      }
    }
    
  }



  /** set the packed board id.
   * @sa BoardID
   * For the Caen 767 TDC the boardID has a slightly different meaning compared with the CrcBoards 
   * slotID and board component numbers are just the MSB and LSB of
   * the base address.
   */
    BmlEventData &setBoardID(int boardID) {
       obj()->setIntVal(kBmlEventBoardID, boardID); 
       return *this;
    };

    /**get the board id */
    int getBoardID() const {return getIntVal(kBmlEventBoardID);};

    /** Set the Base Address of this TDC*/
    BmlEventData &setBaseAddress(int baseaddress) {
       obj()->setIntVal(kBmlEventBaseAddress, baseaddress);
       return *this;
    };

    /** Return the Base Address */
    short getBaseAddress() const {
      return (short) ( getIntVal(kBmlEventBaseAddress) );
    };

    /** Set the Record Label*/
    BmlEventData &setRecordLabel(int label) {
       obj()->setIntVal(kBmlEventRecordLabel,label);
       return *this;
    };

    /** Return the Record Label */
    short getRecordLabel() const {
      return (short) ( getIntVal(kBmlEventRecordLabel) & 0xff);
    };


  /** Set the status register Value*/
  BmlEventData &setStatusRegister(int statreg){
    obj()->setIntVal(kBmlEventStatusRegister, statreg);
    return *this;
  }

  /** Get the status register value*/
  int getStatusRegister() const {
    return ( getIntVal(kBmlEventStatusRegister) );
  };

  /** Set the number of words*/
  BmlEventData &setNumberOfWords(int numwords){
    obj()->setIntVal(kBmlEventNumberOfWords, numwords);
    return *this;
  }

  /** Get the number of words*/
  int getNumberOfWords() const {
    return ( getIntVal(kBmlEventNumberOfWords) );
  };


  /** Set the geo Address of the TDC*/
  BmlEventData &setGeoAddress(int geoaddress){
    obj()->setIntVal(kBmlEventgeoAddress, geoaddress);
    return *this;
  }

  /** Get the geo Address*/
  int getGeoAddress() const {
    return ( getIntVal(kBmlEventgeoAddress) );
  };

  /** Set the event number as counted by the TDC*/
  BmlEventData &setEventNumber(int eventnumber){
    obj()->setIntVal(kBmlEventeventNumber, eventnumber);
    return *this;
  }

  /** Get the event Number*/
  int getEventNumber() const {
    return ( getIntVal(kBmlEventeventNumber) );
  };


  /** Set status as present when the the eob record appears*/
  BmlEventData &setStatus(int status){
    obj()->setIntVal(kBmlEventStatus, status);
    return *this;
  }

  /** Get the status value*/
  int getStatus() const {
    return ( getIntVal(kBmlEventStatus) );
  };

  /** Set the EventData counter as present when the the eob record appears*/
  BmlEventData &setEventDataCounter(int evtdatacounter){
    obj()->setIntVal(kBmlEventEventDataCounter, evtdatacounter);
    return *this;
  }

  /** Get the event data counter value*/
  int getEventDataCounter() const {
    return ( getIntVal(kBmlEventEventDataCounter) );
  };
  

  /**Get the number of channels which carry a signal*/
  int getNumberOfSignalChannels() const { 
    return ( getIntVal(kBmlEventNumberOfSignalChannels) );
}

  /** Here follow the setting and getting of the information from the class holding supplementary information */
  /** set the TDC type */

   BmlEventData &setTDCType(std::string typeStr) {
    _bml_event_sup->setTDCType(typeStr);
     return *this;
    };

  /** retrieve the typename */
  std::string getTDCType() {
    std::string retStr(CAEN_767);
    if(_isSupInitialised) retStr=_bml_event_sup->getTDCType();
    else { 
      if(!_isWarning1Printed) {
	std::cout << "BmlEventData - Info/Warning: Object containing supplementary information not initialised. Will return default: " << CAEN_767 <<  " which might be what you want, please verify."  << std::endl;
	_isWarning1Printed=true;
      }
    }
    return retStr;
  }


   /** set the Fifo Cont id.*/
    BmlEventData &setEventFifoCont(int fifocont) {
       _bml_event_sup->setEventFifoCont(fifocont);
       return *this;
    };


  /**get the word count */
  unsigned short getWordCount() {
    unsigned short retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getWordCount();
    else printInitWarning(); 
    return retval;
  }


  /**get the event count */
  unsigned short getEventCount() {
    unsigned short retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getEventCount();
    else printInitWarning();
    return retval;
  }



  /** set the bunch id.*/
    BmlEventData &setBunchID(int bunchID) {
       _bml_event_sup->setBunchID(bunchID); 
       return *this;
    };


  /**get the bunch id */
  unsigned int getBunchID() {
    int retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getBunchID();
    else printInitWarning();
    return static_cast<unsigned int>(retval);
  }



  /** Set the eventid from the TDC trailer*/
  BmlEventData &setEventIDTrailer(int eventIDTrailer) {
    _bml_event_sup->setEventIDTrailer(eventIDTrailer); 
    return *this;
  };
  
  /**get the event id trailer */
  unsigned int getEventIDTrailer() {
    int retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getEventIDTrailer();
    else printInitWarning();
    return static_cast<unsigned int>(retval);
  }



  /** Set the TDC errors*/
  BmlEventData &setTDCErrors(int tdcErrors) {
    _bml_event_sup->setTDCErrors(tdcErrors); 
    return *this;
  };


  /**get the tdc errors */
  unsigned int getTDCErrors() {
    int retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getTDCErrors();
    else printInitWarning();
    return static_cast<unsigned int>(retval);
  }
  

  /** Set the buffer overflow*/
  BmlEventData &setBufferOverflow(int bufferOverflow){
    _bml_event_sup->setBufferOverflow(bufferOverflow); 
    return *this;
  }

 /**get the buffer overflow */
  unsigned int getBufferOverflow() {
    int retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getBufferOverflow();
    else printInitWarning();
    return static_cast<unsigned int>(retval);
  }

  /** Set the number of words in the trailer*/
  BmlEventData &setTriggerLost(int triggerlost){
    _bml_event_sup->setTriggerLost(triggerlost); 
    return *this;
  }


 /** Get information on lost triggers*/
  unsigned int getTriggerLost() {
    int retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getTriggerLost();
    else printInitWarning();
    return static_cast<unsigned int>(retval);
  }
  


  /** Set the number of words in the trailer*/
  BmlEventData &setTDCWordCountTrailer(int numwords){
    _bml_event_sup->setTDCWordCountTrailer(numwords); 
    return *this;
  }

 /** Get the number of words in the trailer*/
  unsigned int getTDCWordCountTrailer() {
    int retval(0);
    if(_isSupInitialised) retval=_bml_event_sup->getTDCWordCountTrailer();
    else printInitWarning();
    return static_cast<unsigned int>(retval);
  }


   /** return the supplementary object */
  BmlEventDataSup* getSupObject() {return _bml_event_sup;}


  /** Get measurements for each tdc channel 
    * The data are stored at the end of the collection with the
    * following 'protocol' 
    * channelnumber
    * number of signals m 
    * startime word, i.e. each measurement occupies one bit in this word
    * signal 1
    * ...
    * signal m
    */
  void addTDCChannels(const TDCChannelContainer_t&);


  /**Returns a container with tdc channel info 
    * The first part of the map is the channelnumber
    * The second part of the map is a pair consisting of
    * an indicator whether the measurement is a starttime 
    * and the actual time measurement (the core of it all)
    * falling egdes are indicated by a minus sign
    */
  const TDCChannelContainer_t& getTDCChannelContainer();

   /**returns the the type name
    */
   const std::string getTypeName() const 
   { 
      return "BmlEventData" ;
   } 
//............................................................................      
   /**returns a brief description of the data stored 
    */
   const std::string getDataDescription() const 
   {
      return "int:BoardID, int:baseaddress, int:recordlabel, int:statusRegister, int:numberOfWords, int:geoaddress, int:eventnumber, int:status, int:eventdatacounter, int:numberof_signal_channels, mxint[n]:TDC_Channels_entries" ;
   }
//............................................................................      
   /**dumps data content 
    */
  std::ostream& print(std::ostream &ostrm);  

  /** A convenient method which prints a warning in case the supplementary information is not initialiasied */ 
  void printInitWarning() {
    if(!_isWarning2Printed) {
      std::cout << "BmlEventData - Warning: Object containing supplementary information not initialised. Will return dummy" << std::endl;
      _isWarning2Printed=true;
    } 
  } 
  
private:

   /**Set the number of channels which carry a signal*/    
   void setNumberOfSignalChannels(int numsig) {
   obj()->setIntVal(kBmlEventNumberOfSignalChannels, numsig);
  }

  /** The channel container which we will return to the user */
  TDCChannelContainer_t _tdcChannelContainer;

/** An object of the associated class */
  static BmlEventDataSup* _bml_event_sup;

/** A bool deciding whether supplementary object has been correctly created*/
  bool _isSupInitialised;
/** A bool which prevents  prohibitive numerous printing of warnings*/
  static bool _isWarning1Printed;
/** Another bool which prevents  prohibitive numerous printing of warnings*/
  static bool _isWarning2Printed;
/** Yet Another bool which prevents  prohibitive numerous printing of warnings*/
  static bool _isWarning3Printed;


}; //class EmlEventData

}
#endif 
//............................................................................
