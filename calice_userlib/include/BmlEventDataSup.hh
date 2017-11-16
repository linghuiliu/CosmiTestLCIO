#ifndef BmlEventDataSup_HH
#define BmlEventDataSup_HH 1


#include <iostream>
#include <sstream>

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
#include "UTIL/LCTime.h"
#include <sys/time.h>
#include <map>
#include <stringFromToInts.hh>

using namespace std;
using namespace lcio;
using namespace CALICE;


namespace CALICE {

/** The indices of all the stored values.*/

  enum EBmlEventSupIntValues {kBmlEventSupFifo,
                              kBmlEventSupEventIDTrailer, 
                              kBmlEventSupBunchID, 
                              kBmlEventSupTDCErrors,  
                              kBmlEventSupBufferOverflow, 
                              kBmlEventSupTriggerLost, 
                              kBmlEventSupTDCWordCountTrailer,
			      //kBmlEventSupStatus,
			      //kBmlEventSupEventSupDataCounter,
			      //kBmlEventSupNumberOfSignalChannels,
                              //We store the name of the tdc in the class (see also DaqTypeDataBlock).
                              //Therefore we need to memorise the length of the name and where in the
                              //heap the characters do start 
                              //Like this we can always extend the object
                              kBmlEventNamesPointer, kBmlEventNamesLength, 
                              kBmlEventSupIntValues};


/**Class to store supplementary BmlEventData. This is tailored to
 * the data extracted from the Caen TDCs in extension of the actual class BmlEventData 
 * This class is written such that it should be only accessed via the aforementioned class
 * The user is strongly discouraged to instantiate by him/herself. It has only private functions and declares
 * BmlEventData to be its friend.
 *  @author R. Pöschl (LAL Orsay)
 *  @date Oct 13 2010
 */

class BmlEventDataSup : public LCGenericObjectImplExt 
{
  
  friend class BmlEventData;


   private: 
  

  /** Default Constructor */
      BmlEventDataSup() {
	//do some initialization
        setTDCType("dummyTDC");        
        setEventFifoCont(0);        
        setEventIDTrailer(0);        
	setBunchID(0);
        setTDCErrors(0);
        setBufferOverflow(0);
        setTriggerLost(0);
        setTDCWordCountTrailer(0);
        //setGeoAddress(0); 
        //setEventNumber(0); 
        //setStatus(0); 
        //setEventDataCounter(0);
        //setNumberOfSignalChannels(0);
      }; 

//............................................................................
   /**A copy constructor
    */
   BmlEventDataSup(LCObject* obj) : LCGenericObjectImplExt(obj) 
   { 
   } 

//............................................................................
   /**The destructor 
    */
   virtual ~BmlEventDataSup() 
   { 
   }
//............................................................................ 
  /** set the tdc type*/
  BmlEventDataSup &setTDCType(std::string typeStr) {
    //stringstream mystream(typeStr);
    //int myint; 
    //mystream >> myint;
    /** Store the index at which the pointer to the names of the int data data types do start*/
    /** This written such that it can be extended in case more strings need to be stored */ 
    /** Would however trigger a bit of re-writing but at least it _sure_ that it remains always */
    /** compatible and flexible */
    /** We use the same ugly but successful technique as in DaqTypeDataBlock (we may even be able to inherit from it */
    /** now we've simply copied a few useful methods -> to be checked */ 
    setPointerToIndexOfIntTypeNames(kBmlEventSupIntValues);
    setLengthIndexOfIntTypeNames(kBmlEventSupIntValues+1);
    int namepointer(kBmlEventSupIntValues+2);
    setTypeIndex(getPointerToIndexOfIntTypeNames(), namepointer);
    //std::cout << "String length: " << typeStr.size() << std::endl;
    convertStringToInts(obj(),typeStr, namepointer, getLengthIndexOfIntTypeNames() );
    _obj=obj();
    return *this;
  }
 
  void setPointerToIndexOfIntTypeNames(int nameind) { 
     obj()->setIntVal(kBmlEventNamesPointer, nameind);
  }


  void setLengthIndexOfIntTypeNames(int lengthindex) {
      obj()->setIntVal(kBmlEventNamesLength, lengthindex);
      }


   /** Retrieve the index at which the lengths of the names of the int data types do start*/
    int getLengthIndexOfIntTypeNames() const {
      return getIntVal(kBmlEventNamesLength);
      };  



    /** Retrieve the index at which the pointer to the names of int data types do start */
    int getPointerToIndexOfIntTypeNames() const {
      return getIntVal(kBmlEventNamesPointer);
      };  


  /** Set the index of a given data type name */
    void setTypeIndex(int pos, int value) {
      obj()->setIntVal(pos, value);
    } 

  /** get the tdc type */
  /** Currently since only one string in object -> to be modified if more strings*/
  std::string getTDCType() const {
     return getStringFromInts(_obj, _obj->getIntVal(getPointerToIndexOfIntTypeNames()), getLengthIndexOfIntTypeNames() );
  }

  /**Store the FIFO content */
  BmlEventDataSup &setEventFifoCont(int fifocont) {
   obj()->setIntVal(kBmlEventSupFifo, fifocont);
   return *this;
  }

  /**get the event count */
  unsigned short getEventCount() const {
    //std::cout << "Retrieved Fifocont: " << std::hex << getIntVal(kBmlEventSupFifo) << std::dec << std::endl;
    return (( getIntVal(kBmlEventSupFifo) >> 16) & 0xffff);
  }

  /**get the word count */
  unsigned short getWordCount() const {return (getIntVal(kBmlEventSupFifo) & 0xffff);}


  /** set the bunch id*/
    BmlEventDataSup &setBunchID(int bunchid) {
       obj()->setIntVal(kBmlEventSupBunchID, bunchid); 
       return *this;
    };
 
    /**get the bunch id */
    int getBunchID() const {return getIntVal(kBmlEventSupBunchID);};

    /** Set the eventid from the TDC trailer*/
    BmlEventDataSup &setEventIDTrailer(int eventIDTrailer) {
       obj()->setIntVal(kBmlEventSupEventIDTrailer, eventIDTrailer);
       return *this;
    };

    /** Return the  eventid from the TDC trailer*/
    short getEventIDTrailer() const {
      return (short) ( getIntVal(kBmlEventSupEventIDTrailer) );
    };

    /** Set the TDC errors*/
    BmlEventDataSup &setTDCErrors(int tdcErrors) {
       obj()->setIntVal(kBmlEventSupTDCErrors, tdcErrors);
       return *this;
    };

  /** Get the TDC errors*/
  int getTDCErrors() const {
    return ( getIntVal(kBmlEventSupTDCErrors) );
  };

  /** Set the buffer overflow*/
  BmlEventDataSup &setBufferOverflow(int bufferOverflow){
    obj()->setIntVal(kBmlEventSupBufferOverflow, bufferOverflow);
    return *this;
  }

  /** Get the buffer overflow*/
  int getBufferOverflow() const {
    return ( getIntVal(kBmlEventSupBufferOverflow) );
  };

  /** Set the number of words in the trailer*/
  BmlEventDataSup &setTriggerLost(int triggerlost){
    obj()->setIntVal( kBmlEventSupTriggerLost, triggerlost);
    return *this;
  }

  /** Get the number of lost triggers*/
  int getTriggerLost() const {
    return ( getIntVal(kBmlEventSupTriggerLost) );
  };

  /** Set the number of lost triggers*/
  BmlEventDataSup &setTDCWordCountTrailer(int numwords){
    obj()->setIntVal(kBmlEventSupTDCWordCountTrailer, numwords);
    return *this;
  }

  /** Get the number of words in the trailer*/
  int getTDCWordCountTrailer() const {
    return ( getIntVal(kBmlEventSupTDCWordCountTrailer) );
  };


   /**returns the the type name
    */
   const std::string getTypeName() const 
   { 
      return "BmlEventDataSup" ;
   } 
//............................................................................      
   /**returns a brief description of the data stored 
    */
   const std::string getDataDescription() const 
   {
      return "short:eventcount,short:wordcount,int:eventidtrailer, int:bunchid, int:tdcErrors, int:bufferoverflow, int:triggerlost, int:tdceordcounttrailer" ;
   }
//............................................................................      
   /**dumps data content 
    */
  std::ostream& print(std::ostream &ostrmsup);  

}; //class BmlEventDataSup

}
#endif 
//............................................................................
