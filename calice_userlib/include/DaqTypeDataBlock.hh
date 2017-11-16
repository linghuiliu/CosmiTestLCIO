#ifndef DaqTypeDataBlock_hh
#define DaqTypeDataBlock_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "UTIL/LCTime.h"
#include "collection_names.hh"
#include <sys/time.h>
#include <map>

#ifdef VALUE_CHECK
#  include <cassert>
#endif

using namespace lcio;
using namespace std;


namespace CALICE {

 
  /**The map which contains the measured values of the different data types*/
  typedef std::map<std::string, std::vector<double> > DaqTypeDataDblMap_t;   
  typedef std::map<std::string, std::vector<int> > DaqTypeDataIntMap_t;   
  typedef std::map<std::string, std::vector<unsigned int> > DaqTypeDataUIntMap_t;   
  typedef std::map<std::string, std::vector<float> > DaqTypeDataFloatMap_t;   
  /**  and for the timestamps */
  typedef std::map<std::string, LCTime > TimeStampMap_t;   

  /** The indices of the the stored values.*/

  enum DaqTypeIntValues   {kIntDaqTypeTimeStamps, kIntDaqTypeTimeStampNamesPointer,kIntDaqTypeTimeStampNamesLength,
			    kIntDaqTypeIntTypes, kIntDaqTypeDblTypes, kIntDaqTypeFloatTypes,
			    kIntDaqTypeIntNamesPointer, kIntDaqTypeDblNamesPointer, kIntDaqTypeFloatNamesPointer,
			    //kIntBmlSroRunIntNames, kIntBmlSroRunDblNames, kIntBmlSroRunFloatNames,
			    kIntDaqTypeIntNamesLength, kIntDaqTypeDblNamesLength, kIntDaqTypeFloatNamesLength,
			    kIntDaqTypeIntSizes, kIntDaqTypeDblSizes, kIntDaqTypeFloatSizes,
			    kIntDaqTypeDataStart};
   
 

  enum DaqTypeDblValues{kIntDaqTypeDblReadings};


  //for completeness ...
  enum DaqTypeFloatValues {kIntDaqTypeFloatReadings};

  /** 
   * Base Class to handle all daq types (for the time being apart from the actual crc data)
   * This base class constains the functions to store the data
   * The actual handling of the data is left to dedicated classes which have to derive from this class.
   * The implementation and the interpretation of the stored values is left to the derived classes.
   * The actual meaning of the entries is propagated from a daq class into the database. 
   * Note, that in principle really all data types (apart from crc data) can be handled by this class
   * However, existing interface classes (written before Oct. 2008) will remain untouched or exchanged gradually. 
   * if considered to be useful.
   * This class allows for a quick reply to changes in the daq. In particular on the appearance of new data types.
   * @author: Roman Pöschl LAL/Orsay
   * @date Oct  2008
   */
  class DaqTypeDataBlock : public LCGenericObjectImplExt {
    

  public: 
    
    /** Constructor */
    DaqTypeDataBlock() {
      //Reset the maps which buffer the values
      _timeStampMapRaw.clear();
      _timeStampMap.clear();
      _bufferOfDbls.clear();
      _bufferOfInts.clear();
      _bufferOfFloats.clear();      
      
    };
    
    
    //Set Parameters of the board by which the data were taken  
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    DaqTypeDataBlock(LCObject* obj, bool restoreMaps=true) : LCGenericObjectImplExt(obj) { 
      //Reset the maps which buffer the values
      if(restoreMaps) {
	_timeStampMapRaw.clear();
	_timeStampMap.clear();
	_bufferOfDbls.clear();
	_bufferOfInts.clear();
	_bufferOfFloats.clear();      
	RestoreMaps();
      }
    } 

    
    /** Set the time stamps or better initialize the timestamp map which will be stored
     * Remember: We store UTC!!!!
     */
    DaqTypeDataBlock& setTimeStamps(std::map<std::string,time_t> timeStampMapRaw) 
    {_timeStampMapRaw=timeStampMapRaw;
      return *this;}
    
    
    TimeStampMap_t getTimeStamps(){return _timeStampMap;};

    /** These methods receive the vectors with the measured values
     * Mainly to store large arrays of a given datatype but in
     * principle this can be extended to every entry in the interface class
     */
    virtual void setDblArrays(const DaqTypeDataDblMap_t& daqTypeDataDblMap) 
    {_bufferOfDbls=daqTypeDataDblMap;  }
    virtual void setFloatArrays(const DaqTypeDataFloatMap_t& daqTypeDataFloatMap) 
    {_bufferOfFloats=daqTypeDataFloatMap;}
    virtual void setIntArrays(const DaqTypeDataUIntMap_t&); 
    virtual void setIntArrays(const DaqTypeDataIntMap_t&); 
    

    /** The following methods allow to retrieve the maps
      @todo : Should these methods really be public or only accessible by friend functions or ...?
      @todo : Do we have to check whether they has been filled or not before we hand it to the user?
    */
    DaqTypeDataIntMap_t getIntArrays(){return _bufferOfInts;}
    DaqTypeDataDblMap_t getDblArrays(){return _bufferOfDbls;}
    DaqTypeDataFloatMap_t getFloatArrays(){return _bufferOfFloats;}


    /** A method which finally fills the object which is stored 
        @todo can the algorithms in this method be templated?*/
    DaqTypeDataBlock* finalize();

    /** A method to calculate the beam energy which has to be implemented by the derived classes */
    virtual const double getBeamEnergy(){
      std::cout << "WARNING: Using method of base class please refer to members of derived classes" << std::endl;
      return 0;
    }


    /** convenient print method*/
    virtual void print(std::ostream&);


   // -------- need to implement abstract methods from LCGenericObject
    
    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"DaqTypeDataBlock" ;
    } 
    
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "Base class for beam line slow control data";
    }
    
  private:
    

    /*********************************** CRP New methods 27/8/08 Begin **********************************/
    /** Store the number of int data types */
    void setNumberOfIntTypes(int numvals) {
      obj()->setIntVal(kIntDaqTypeIntTypes, numvals);
      }
    
    /** Retrieve the number of int data types*/
    int getNumberOfIntTypes() const {
      return getIntVal(kIntDaqTypeIntTypes);
      };  


    /** Store the number of timestamps */
    void setNumberOfTimeStamps(int numvals) {
      obj()->setIntVal(kIntDaqTypeTimeStamps, numvals);
      }
    
    /** Retrieve the number of int data types*/
    int getNumberOfTimeStamps() const {
      return getIntVal(kIntDaqTypeTimeStamps);
      };  

    /** Store the index at which the pointer to the names of the int data data types do start*/
    void setPointerToIndexOfIntTypeNames(int nameind) {
      obj()->setIntVal(kIntDaqTypeIntNamesPointer, nameind);
      }
    
    /** Retrieve the index at which the pointer to the names of int data types do start */
    int getPointerToIndexOfIntTypeNames() const {
      return getIntVal(kIntDaqTypeIntNamesPointer);
      };  

    /** Store the index at which the pointer to the names of the time stamps do start*/
    void setPointerToIndexOfTimeStampNames(int nameind) {
      obj()->setIntVal(kIntDaqTypeTimeStampNamesPointer, nameind);
      }
    
    /** Retrieve the index at which the pointer to the names of the time stamps do start*/
    int getPointerToIndexOfTimeStampNames() const {
      return getIntVal(kIntDaqTypeTimeStampNamesPointer);
      };  



    /** Set the index of a given data type name */
    void setTypeIndex(int pos, int value) {
      obj()->setIntVal(pos, value);
    } 


    /** Store the index at which the lengths of the names of the int data types do start*/
    void setLengthIndexOfIntTypeNames(int lengthindex) {
      obj()->setIntVal(kIntDaqTypeIntNamesLength, lengthindex);
      }
    
    /** Retrieve the index at which the lengths of the names of the int data types do start*/
    int getLengthIndexOfIntTypeNames() const {
      return getIntVal(kIntDaqTypeIntNamesLength);
      };  


    /** Store the index at which the lengths of the names of the time stamps do start*/
    void setLengthIndexOfTimeStampNames(int lengthindex) {
      obj()->setIntVal(kIntDaqTypeTimeStampNamesLength, lengthindex);
      }
    
    /** Retrieve the index at which the lengths of the names of the int data types do start*/
    int getLengthIndexOfTimeStampNames() const {
      return getIntVal(kIntDaqTypeTimeStampNamesLength);
      };  

    /** Store the Index where the sizes of the int data data types do start*/
    void setSizeIndexOfIntType(int size) {
      obj()->setIntVal(kIntDaqTypeIntSizes, size);
      }
    
    /** Retrieve index where the sizes of int data types do start */
    int getSizeIndexOfIntType() const {
      return getIntVal(kIntDaqTypeIntSizes);
      };  

    /** Store the number of double data types */
    void setNumberOfDblTypes(int numvals) {
      obj()->setIntVal(kIntDaqTypeDblTypes, numvals);
      }
    
    /** Retrieve the number of double data types*/
    int getNumberOfDblTypes() const {
      return getIntVal(kIntDaqTypeDblTypes);
      };  

    /** Store the at which the names the double data data types do start*/
    void setPointerToIndexOfDblTypeNames(int nameind) {
      obj()->setIntVal(kIntDaqTypeDblNamesPointer, nameind);
      }
    
    /** Retrieve index at which the names of double data types do start */
    int getPointerToIndexOfDblTypeNames() const {
      return getIntVal(kIntDaqTypeDblNamesPointer);
      };  


    /** Store the index at which lengths of the names of the double data types do start*/
    void setLengthIndexOfDblTypeNames(int length) {
      obj()->setIntVal(kIntDaqTypeDblNamesLength, length);
      }
    
    /** Retrieve the index at which the lengths of the names of the double data types do start*/
    int getLengthIndexOfDblTypeNames() const {
      return getIntVal(kIntDaqTypeDblNamesLength);
      }  

    /** Store the Index where the sizes of the double data data types do start*/
    void setSizeIndexOfDblType(int size) {
      obj()->setIntVal(kIntDaqTypeDblSizes, size);
      }


    /** Retrieve index where the sizes of double data types do start */
    int getSizeIndexOfDblType() const {
      return getIntVal(kIntDaqTypeDblSizes);
      };  

    /** Store the number of float data types */
    void setNumberOfFloatTypes(int numvals) {
      obj()->setIntVal(kIntDaqTypeFloatTypes, numvals);
      }
    
    /** Retrieve the number of float data types*/
    int getNumberOfFloatTypes() const {
      return getIntVal(kIntDaqTypeFloatTypes);
      };  


    /** Store the index at which  the names of the float data types do start*/
    void setPointerToIndexOfFloatTypeNames(int nameind) {
      obj()->setIntVal(kIntDaqTypeFloatNamesPointer, nameind);
      }
    
    /** Retrieve index at which the names of the float data types do start */
    int getPointerToIndexOfFloatTypeNames() const {
      return getIntVal(kIntDaqTypeFloatNamesPointer);
      };  

    /** Store the index at which the lengths of the names of the float data types do start*/
    void setLengthIndexOfFloatTypeNames(int length) {
      obj()->setIntVal(kIntDaqTypeFloatNamesLength, length);
      }
    
    /** Retrieve the index at which the lengths of the names of the float data types do start*/
    int getLengthIndexOfFloatTypeNames() const {
      return getIntVal(kIntDaqTypeFloatNamesLength);
      };  


    /** Store the Index where the sizes of the float data data types do start*/
    void setSizeIndexOfFloatType(int size) {
      obj()->setIntVal(kIntDaqTypeFloatSizes, size);
      }


    /** Retrieve index where the sizes of double data types do start */
    int getSizeIndexOfFloatType() const {
      return getIntVal(kIntDaqTypeFloatSizes);
      };  


    /** A map holds the nature and the value of the timestamps */
    std::map<std::string,time_t> _timeStampMapRaw;
    /** Another map now with the time stamp using LCIO Utils */
    TimeStampMap_t _timeStampMap;

    /** A simple array which holds the timestamp decomposed into two ints
        containing a) ymd and b) hms */
    int _timestamptoint[2];

    /** Simple method to decompose the struct tm containing the timestamp into ints 
        which can be stored in the LCGenericObject and therefore in the database */
    void calculateTimeStamp(struct tm*);

    /**turn the two ints containing the timestamp info into a LCTime Object*/
    LCTime composeTimeStamp();


    /** A method which restores the map from the generic object*/
    void RestoreMaps();

    /*********************************** CRP New methods 27/8/08 End **********************************/

    /** Variables which buffer the maps passed by the converter 
        Buffering is needed since we don't know a priori when the objects are passed to us and
        which data types they contain
        the 'finalize' method (see below) will finally create the object from the buffers
        Buffer for doubles */
    DaqTypeDataDblMap_t _bufferOfDbls;   
    /** Buffer for Floats */
    DaqTypeDataFloatMap_t _bufferOfFloats;   
    /** Buffer for Ints */
    DaqTypeDataIntMap_t _bufferOfInts;   



  }; // class
  
}
#endif 
