#include "DaqTypeDataBlock.hh"
#include "to_binary_bitops.hh"

#include <RtypesSubSet.h>
#include <stringFromToInts.hh>

namespace CALICE{
   


  void DaqTypeDataBlock::setIntArrays(const DaqTypeDataIntMap_t& daqTypeDataIntMap) {
    //if the buffer is empty we make a simple copy
    if(_bufferOfInts.size()==0) {
      _bufferOfInts=daqTypeDataIntMap;
      //if there is already something in the buffer we append the new elements
    } else {
      for(DaqTypeDataIntMap_t::const_iterator theiter = daqTypeDataIntMap.begin(); theiter != daqTypeDataIntMap.end(); theiter++) {
	if(!_bufferOfInts.insert(std::pair<std::string, std::vector<int> >(theiter->first,theiter->second)).second) std::cout << "Problems with inserting element " <<  theiter->first << " into buffer" << std::endl; 
      }
    }    

  }


  void DaqTypeDataBlock::setIntArrays(const DaqTypeDataUIntMap_t& daqTypeDataUIntMap) {
    //fill the unsigned ints into the int arrays, the LCGenericObject offers no other possibility
      for(DaqTypeDataUIntMap_t::const_iterator theiter = daqTypeDataUIntMap.begin(); theiter != daqTypeDataUIntMap.end(); theiter++) {
	//can this be done more elegantly?
	std::vector<int> v;
	for(std::vector<unsigned int>::const_iterator veciter=theiter->second.begin(); veciter != theiter->second.end(); veciter++) v.push_back(static_cast<int>((*veciter)));
	if(!_bufferOfInts.insert (std::pair<std::string, std::vector<int> >(theiter->first, v)).second) std::cout << "Problems with inserting element " <<  theiter->first << " into buffer" << std::endl; 
      }
    
  }


  DaqTypeDataBlock*  DaqTypeDataBlock::finalize(){ 
#ifdef RECO_DEBUG
    std::cout << "Finalizing the object" << std::endl;
#endif    
    //Store the ints in the object
    //We will start with the time stamp as these are not ordinary beamline readings
    //and might need to be accessed seperately from the former 
    setNumberOfTimeStamps(static_cast<int>(_timeStampMapRaw.size()));
#ifdef RECO_DEBUG
    std::cout << "number of time stamps: " << getNumberOfTimeStamps() << std::endl;
#endif
    //Pointer to the first start indexpointer for the names of the time stamps 
    unsigned int namestartindexpointer(kIntDaqTypeDataStart);
    setPointerToIndexOfTimeStampNames(static_cast<int>(namestartindexpointer));
    //Where do we store the length of the names?
    unsigned int namelength_memo(getPointerToIndexOfTimeStampNames()+getNumberOfTimeStamps());
    setLengthIndexOfTimeStampNames(static_cast<int>(namelength_memo));
    //Where do the decomposed names of the int data types start?
    unsigned int namestartpointer(getPointerToIndexOfTimeStampNames()+2*getNumberOfTimeStamps());
    //now loop over the time stamps
    //Initialise the valuepointer to be the namestartpointer
    unsigned int valuepointer(namestartpointer);
    for(std::map<std::string, time_t>::iterator timestamp_iter=_timeStampMapRaw.begin(); timestamp_iter != _timeStampMapRaw.end(); timestamp_iter++){
      //The nextindex is the place where we can e.g. the next entry into the generic object
#ifdef CONV_DEBUG
      std::cout << "Timestamp type is: " << (*timestamp_iter).first << std::endl;
      std::cout << "Timestamps - Value Pointer is: " << valuepointer << std::endl;
      std::cout << "Timestamps - Namelength memo before: " <<  namelength_memo << std::endl;  
#endif
      namestartpointer = convertStringToInts(obj(),(*timestamp_iter).first, valuepointer, namelength_memo);
#ifdef CONV_DEBUG      
      std::cout << "The name of the Timestamp is: " << getStringFromInts(obj(), valuepointer, namelength_memo) << std::endl;
      std::cout << "Timestamp from data: " << &(timestamp_iter->second) << "= "<< gmtime(&(timestamp_iter->second)) << std::endl;
      std::cout << "Timezone: " << gmtime( &(timestamp_iter->second))->tm_zone << std::endl;
#endif
      setTypeIndex(static_cast<int>(namestartindexpointer), static_cast<int>(valuepointer));
#ifdef CONV_DEBUG
      std::cout << "Start of Name of Timestamp is memorized at: " << namestartindexpointer << std::endl; 
      std::cout << "Length of Name of Timestamp is memorized at: "  << namelength_memo << std::endl; 
      std::cout << "Length of Name of Timestamp is: "  << obj()->getIntVal(namelength_memo) << std::endl; 
#endif
      namestartindexpointer++;
      namelength_memo++;
      valuepointer=namestartpointer;
      //Convert the time_t timestamp into two ints which we will store
      calculateTimeStamp(gmtime( &(timestamp_iter->second) ));
      for(unsigned itime=0; itime<2; itime++){
#ifdef CONV_DEBUG
	std::cout << "Timestamp Valuepointer " << itime << "  is: " << valuepointer << std::endl;
	std::cout << "Timestamp[" << itime << "]=" << _timestamptoint[itime] << std::endl;
#endif
	obj()->setIntVal(valuepointer, _timestamptoint[itime]);
	valuepointer++;
      }
      
   }


    //Number of int data types
    setNumberOfIntTypes(static_cast<int>(_bufferOfInts.size()));
#ifdef CONV_DEBUG
    std::cout << "num of datatypes: " << getNumberOfIntTypes() << std::endl;
#endif
    //Pointer to the first startindex pointer for the names of the data type
    namestartindexpointer=valuepointer;
    setPointerToIndexOfIntTypeNames(static_cast<int>(namestartindexpointer));
    //Where to we store the length of the names?
    namelength_memo=getPointerToIndexOfIntTypeNames()+getNumberOfIntTypes();
    setLengthIndexOfIntTypeNames(static_cast<int>(namelength_memo));
    //Where to we store the sizes of the data types
    unsigned int size_memo(getPointerToIndexOfIntTypeNames()+2*getNumberOfIntTypes());
    setSizeIndexOfIntType(static_cast<int>(size_memo));
    //Where do the decomposed names of the int data types start?
    namestartpointer=getPointerToIndexOfIntTypeNames()+3*getNumberOfIntTypes();
    setTypeIndex(getPointerToIndexOfIntTypeNames(),  namestartpointer);
    //For each int data type we store the sequence Name, Values. Therefore the first entry in each block is a part of the name string
    //in particular the one which follows the last value
    //Initialise the valuepointer to be the namestartpointer
    valuepointer=namestartpointer;
    for ( DaqTypeDataIntMap_t::iterator slowmap_iter = _bufferOfInts.begin(); slowmap_iter != _bufferOfInts.end(); slowmap_iter++) {
      //The nextindex is the place where we can e.g. the next entry into the generic object
#ifdef CONV_DEBUG
      std::cout << "Data type is: " << (*slowmap_iter).first << std::endl;
      std::cout << "Value Pointer is: " << valuepointer << std::endl;
      std::cout << "Namelength memo before: :" <<  namelength_memo << std::endl;  
#endif
      namestartpointer = convertStringToInts(obj(),(*slowmap_iter).first, valuepointer, namelength_memo);
#ifdef CONV_DEBUG
      std::cout << "The name is: " << getStringFromInts(obj(), valuepointer, namelength_memo) << std::endl;
#endif
      setTypeIndex(static_cast<int>(namestartindexpointer), static_cast<int>(valuepointer));
#ifdef CONV_DEBUG
      std::cout << "Start of Name is memorized at: " << namestartindexpointer << std::endl; 
      std::cout << "Length of Name is memorized at: "  << namelength_memo << std::endl; 
      std::cout << "Length of Name is: "  << obj()->getIntVal(namelength_memo) << std::endl; 
#endif
      namestartindexpointer++;
      namelength_memo++;
      //memorize the size of the datatype
      setTypeIndex(static_cast<int>(size_memo), static_cast<int>((*slowmap_iter).second.size()));
      size_memo++;
      valuepointer=namestartpointer;
      for(std::vector<int>::const_iterator thevals_iter =
	    (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ){
#ifdef CONV_DEBUG
	std::cout << "Valuepointer is: " << valuepointer << std::endl;
	std::cout << "Value: " << (*thevals_iter) << std::endl;
#endif
	obj()->setIntVal(valuepointer, (*thevals_iter));
	valuepointer++;
      }  


    }
    

    //store the doubles in the object
    //Number of double data types
    setNumberOfDblTypes(static_cast<int>(_bufferOfDbls.size()));
#ifdef CONV_DEBUG
    std::cout << "num of datatypes: " << getNumberOfDblTypes() << std::endl;
#endif
    //Pointer to the first startindex pointer for the names of the data type
    namestartindexpointer=valuepointer;
    setPointerToIndexOfDblTypeNames(static_cast<int>(namestartindexpointer));
    //Where to we store the length of the names
    namelength_memo=getPointerToIndexOfDblTypeNames()+getNumberOfDblTypes();
    setLengthIndexOfDblTypeNames(static_cast<int>(namelength_memo));
    //Where to we store the sizes of the data types
    size_memo=getPointerToIndexOfDblTypeNames()+2*getNumberOfDblTypes();
    setSizeIndexOfDblType(static_cast<int>(size_memo));
    //Where do the decomposed names of the int data types start?
    namestartpointer=getPointerToIndexOfDblTypeNames()+3*getNumberOfDblTypes();
    obj()->setIntVal(getPointerToIndexOfDblTypeNames(), static_cast<int>(namestartpointer)); 
    unsigned int iposDbl(kIntDaqTypeDblReadings);
    for ( DaqTypeDataDblMap_t::iterator slowmap_iter = _bufferOfDbls.begin(); slowmap_iter != _bufferOfDbls.end(); slowmap_iter++) {
#ifdef CONV_DEBUG
      std::cout << "Data type is: " << (*slowmap_iter).first << std::endl;
      std::cout << "Namestartpointer Pointer is: " << namestartpointer << std::endl;
#endif
      unsigned int storepointer=namestartpointer;
      namestartpointer = convertStringToInts(obj(),(*slowmap_iter).first, namestartpointer, namelength_memo);
#ifdef CONV_DEBUG
      std::cout << "The name is: " << getStringFromInts(obj(), storepointer, namelength_memo) << std::endl; 
#endif
      setTypeIndex(static_cast<int>(namestartindexpointer), static_cast<int>(storepointer));
#ifdef CONV_DEBUG
      std::cout << "Start of Name is memorized at: " << namestartindexpointer << std::endl; 
      std::cout << "Length of Name is memorized at: "  << namelength_memo << std::endl; 
      std::cout << "Length of Name is: "  << obj()->getIntVal(namelength_memo) << std::endl; 
#endif
      namestartindexpointer++;
      namelength_memo++;
      //memorize the size of the datatype
      //obj()->setIntVal(size_memo, static_cast<int>((*slowmap_iter).second.size()));
      setTypeIndex(static_cast<int>(size_memo), static_cast<int>((*slowmap_iter).second.size()));
      size_memo++;
      for(std::vector<double>::const_iterator thevals_iter =
	    (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ){
#ifdef CONV_DEBUG
	std::cout << "Valuepointer is: " << iposDbl << std::endl;
	std::cout << "Value: " << (*thevals_iter) << std::endl;
#endif
	obj()->setDoubleVal(iposDbl, (*thevals_iter));
	iposDbl++;
      }  
    }
    

    
    //Store the floats in the object
    //Number of int data types
    setNumberOfFloatTypes(static_cast<int>(_bufferOfFloats.size()));
#ifdef CONV_DEBUG
    std::cout << "num of float datatypes: " << getNumberOfFloatTypes() << std::endl;
#endif
    //Pointer to the first startindex pointer for the names of the data type
    namestartindexpointer=namestartpointer;
    setPointerToIndexOfFloatTypeNames(static_cast<int>(namestartindexpointer));
    //Where to we store the length of the names
    namelength_memo=getPointerToIndexOfFloatTypeNames()+getNumberOfFloatTypes();
    setLengthIndexOfFloatTypeNames(static_cast<int>(namelength_memo));
    //Where to we store the sizes of the data types
    size_memo=getLengthIndexOfFloatTypeNames()+2*getNumberOfFloatTypes();
    setSizeIndexOfFloatType(static_cast<int>(size_memo));
    //Where do the decomposed names of the int data types start?
    namestartpointer=getPointerToIndexOfFloatTypeNames()+3*getNumberOfFloatTypes();
    obj()->setIntVal(getPointerToIndexOfFloatTypeNames(), static_cast<int>(namestartpointer)); 
    
    unsigned int iposFloat(kIntDaqTypeFloatReadings);
    for ( DaqTypeDataFloatMap_t::iterator slowmap_iter = _bufferOfFloats.begin(); slowmap_iter != _bufferOfFloats.end(); slowmap_iter++) {
#ifdef CONV_DEBUG
      std::cout << "Data type is: " << (*slowmap_iter).first << std::endl;
      std::cout << "Namestartpointer Pointer is: " << namestartpointer << std::endl;
#endif
      unsigned int storepointer=namestartpointer;
      namestartpointer = convertStringToInts(obj(),(*slowmap_iter).first, storepointer, namelength_memo);
#ifdef CONV_DEBUG
      std::cout << "The name is: " << getStringFromInts(obj(), storepointer, namelength_memo) << std::endl; 
#endif
      setTypeIndex(static_cast<int>(namestartindexpointer), static_cast<int>(namestartpointer));
#ifdef CONV_DEBUG
      std::cout << "Start of Name is memorized at: " << namestartindexpointer << std::endl; 
      std::cout << "Length of Name is memorized at: "  << namelength_memo << std::endl; 
      std::cout << "Length of Name is: "  << obj()->getIntVal(namelength_memo) << std::endl; 
#endif
      namestartindexpointer++;
      namelength_memo++;
      for(std::vector<float>::const_iterator thevals_iter =
	    (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ){
#ifdef CONV_DEBUG
	std::cout << "Valuepointer is: " << iposFloat << std::endl;
	std::cout << "Value: " << (*thevals_iter) << std::endl;
#endif
	obj()->setFloatVal(iposFloat, (*thevals_iter));
	iposFloat++;
      }  
    }
    
    _obj=obj(); 
    //Restore the maps immediately to have them ready for the print function below
    _timeStampMapRaw.clear();
    _timeStampMap.clear();
    _bufferOfDbls.clear();
    _bufferOfInts.clear();
    _bufferOfFloats.clear();   
    RestoreMaps();
    return this;
  }

  void DaqTypeDataBlock::RestoreMaps(){

#ifdef RECO_DEBUG
    std::cout << "Restoring the maps " << std::endl;
#endif
    //Restoring the maps containing the timestamps
    //get the number of int data data types
    unsigned int timestamps_int(getNumberOfTimeStamps());
#ifdef RECO_DEBUG
    std::cout << "Number of Timestamps: " << timestamps_int << std::endl;
#endif
    //loop over the number of datatypes and reconstruct the names and values
    unsigned int namestartpointer(getPointerToIndexOfTimeStampNames());
    unsigned int namelength_memo(getLengthIndexOfTimeStampNames());
    for( unsigned int itime=0; itime<timestamps_int;itime++) {
#ifdef RECO_DEBUG
      std::cout << "Timestamp - NamestartPointer memo: " << namestartpointer << std::endl;
      std::cout << "Timestamp - NamestartPointer: " << _obj->getIntVal(namestartpointer) << std::endl;
      std::cout << "Timestamp Nameslength memo pointer: " << namelength_memo << std::endl;
      std::cout << "Timestamp Namelength: " <<  _obj->getIntVal(namelength_memo) << std::endl;
      std::cout << "Timestamp Restored name: " << getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo) << std::endl;
      std::cout << "********************************************" << std::endl;
#endif
      //Refilling the map
      //where to the actual readings start
      //The complicates expression below is caused by the fact that the integer values are stored with other information
      //like indexing and in particular the decomposed names of the data types. The latter can occupy either an odd or an even number in memory
      unsigned int posval=((_obj->getIntVal(namelength_memo)%sizeof(int))==0) ?  static_cast<unsigned int>(_obj->getIntVal(namestartpointer)) + static_cast<unsigned int>(_obj->getIntVal(namelength_memo)/sizeof(int) ) : static_cast<unsigned int>(_obj->getIntVal(namestartpointer))+static_cast<unsigned int>(_obj->getIntVal(namelength_memo)/sizeof(int) )+1;
#ifdef RECO_DEBUG
      std::cout << "Timestamp Position Value is: " << posval << std::endl;
#endif
      _timestamptoint[0]=obj()->getIntVal(posval);
      _timestamptoint[1]=obj()->getIntVal(++posval);
#ifdef RECO_DEBUG
      std::cout << "Restored Time Stamp: " <<  composeTimeStamp().getDateString() << std::endl;
#endif
      if(!_timeStampMap.insert( std::pair<string,LCTime > ( getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo), composeTimeStamp() )  ).second) {std::cout << "Warning problems with filling LCTime Timestamp for Timestamp " << getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo) << std::endl; 
      std::cout << "Will fill 0" << std::endl;
      _timeStampMap.insert( std::pair<string, LCTime > ( getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo), 0)  );
      }
      //increment the positions for the pointer storage
      namestartpointer++;
      namelength_memo++;
    }


    //Restore the int maps
#ifdef RECO_DEBUG
    std::cout << "Restoring the int maps" << std::endl;
#endif
    //get the number of int data data types
    unsigned int datatypes_int(getNumberOfIntTypes());
#ifdef RECO_DEBUG
    std::cout << "Number of Int data types: " << datatypes_int << std::endl;
#endif
    //loop over the number of datatypes and reconstruct the names and values
    namestartpointer=getPointerToIndexOfIntTypeNames();
    namelength_memo=getLengthIndexOfIntTypeNames();
    unsigned int size_memo(getSizeIndexOfIntType());
    for( unsigned int itype=0; itype<datatypes_int;itype++) {
#ifdef RECO_DEBUG
      std::cout << "NamestartPointer memo: " << namestartpointer << std::endl;
      std::cout << "NamestartPointer: " << _obj->getIntVal(namestartpointer) << std::endl;
      std::cout << "Nameslength memo pointer: " << namelength_memo << std::endl;
      std::cout << "Namelength: " <<  _obj->getIntVal(namelength_memo) << std::endl;
      std::cout << "Restored name: " << getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo) << std::endl;
      std::cout << "********************************************" << std::endl;
#endif
      //Refilling the map
    //where to the actual readings start
      unsigned int posval=((_obj->getIntVal(namelength_memo)%sizeof(int))==0) ?  static_cast<unsigned int>(_obj->getIntVal(namestartpointer)) + static_cast<unsigned int>(_obj->getIntVal(namelength_memo)/sizeof(int) ) : static_cast<unsigned int>(_obj->getIntVal(namestartpointer))+static_cast<unsigned int>(_obj->getIntVal(namelength_memo)/sizeof(int) )+1;
#ifdef RECO_DEBUG
      std::cout << "Size is memorized at: " << size_memo << std::endl;
      std::cout << "Values for datatype: " << static_cast<unsigned int>(_obj->getIntVal(size_memo)) << std::endl;
      std::cout << "Position Value is: " << posval << std::endl;
#endif
      for(unsigned int ival=0; ival < static_cast<unsigned int>(_obj->getIntVal(size_memo)); ival++) {
#ifdef RECO_DEBUG
	std::cout << "Restored Value: " <<  _obj->getIntVal(posval) << std::endl;
#endif
        _bufferOfInts.insert( std::pair<string,std::vector<int> > ( getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo), std::vector<int> () )  ).first->second.push_back( _obj->getIntVal(posval) );
        posval++;
      }
      //increment the positions for the pointer storage
      namestartpointer++;
      namelength_memo++;
      size_memo++;
    }


#ifdef RECO_DEBUG
    std::cout << "Restoring the double maps" << std::endl;
#endif
    _bufferOfDbls.clear();

    //get the number of int data data types
    unsigned int datatypes_dbl(getNumberOfDblTypes());
#ifdef RECO_DEBUG
    std::cout << "Number of Dbl data types: " << datatypes_dbl << std::endl;
#endif
    //loop over the number of datatypes and reconstruct the names and values
    namestartpointer=getPointerToIndexOfDblTypeNames();
    namelength_memo=getLengthIndexOfDblTypeNames();
    size_memo=getSizeIndexOfDblType();
    //where to the actual readings start
    unsigned int posval(kIntDaqTypeDblReadings);
    for( unsigned int itype=0; itype<datatypes_dbl;itype++) {
#ifdef RECO_DEBUG
      std::cout << "NamestartPointer memo: " << namestartpointer << std::endl;
      std::cout << "NamestartPointer: " << _obj->getIntVal(namestartpointer) << std::endl;
      std::cout << "Nameslength memo pointer: " << namelength_memo << std::endl;
      std::cout << "Namelength: " <<  _obj->getIntVal(namelength_memo) << std::endl;
      std::cout << "Restored name: " << getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo) << std::endl;
      std::cout << "********************************************" << std::endl;
      std::cout << "Size is memorized at: " << size_memo << std::endl;
      std::cout << "Values for datatype: " << static_cast<unsigned int>(_obj->getIntVal(size_memo)) << std::endl;
      std::cout << "Position Value is: " << posval << std::endl;
#endif      
      //Refilling the map
      for(unsigned int ival=0; ival < static_cast<unsigned int>(_obj->getIntVal(size_memo)); ival++) {
#ifdef RECO_DEBUG
	std::cout << "Restored Value: " <<  _obj->getDoubleVal(posval) << std::endl;
#endif
        _bufferOfDbls.insert( std::pair<string,std::vector<double> > ( getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo), std::vector<double> () )  ).first->second.push_back( _obj->getDoubleVal(posval) );
        posval++;
      }
      //increment the positions for the pointer storage
      namestartpointer++;
      namelength_memo++;
      size_memo++;
    }

#ifdef RECO_DEBUG
    std::cout << "Restoring the float maps" << std::endl;
#endif
    //get the number of int data data types
    unsigned int datatypes_float(getNumberOfFloatTypes());
#ifdef RECO_DEBUG
    std::cout << "Number of Float data types: " << datatypes_float << std::endl;
#endif
    //loop over the number of datatypes and reconstruct the names and values
    namestartpointer=getPointerToIndexOfFloatTypeNames();
    namelength_memo=getLengthIndexOfFloatTypeNames();
    size_memo=getSizeIndexOfFloatType();
    //where to the actual readings start
    posval=kIntDaqTypeFloatReadings;
    for( unsigned int itype=0; itype<datatypes_float;itype++) {
#ifdef RECO_DEBUG
      std::cout << "NamestartPointer memo: " << namestartpointer << std::endl;
      std::cout << "NamestartPointer: " << _obj->getIntVal(namestartpointer) << std::endl;
      std::cout << "Nameslength memo pointer: " << namelength_memo << std::endl;
      std::cout << "Namelength: " <<  _obj->getIntVal(namelength_memo) << std::endl;
      std::cout << "Restored name: " << getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo) << std::endl;
      std::cout << "********************************************" << std::endl;
      std::cout << "Size is memorized at: " << size_memo << std::endl;
      std::cout << "Values for datatype: " << static_cast<unsigned int>(_obj->getIntVal(size_memo)) << std::endl;
      std::cout << "Position Value is: " << posval << std::endl;
#endif
      //Refilling the map
      for(unsigned int ival=0; ival < static_cast<unsigned int>(_obj->getIntVal(size_memo)); ival++) {
#ifdef RECO_DEBUG
	std::cout << "Restored Value: " <<  _obj->getFloatVal(posval) << std::endl;
#endif
        _bufferOfFloats.insert( std::pair<string,std::vector<float> > ( getStringFromInts(_obj, _obj->getIntVal(namestartpointer), namelength_memo), std::vector<float> () )  ).first->second.push_back( _obj->getFloatVal(posval) );
        posval++;
      }
      //increment the positions for the pointer storage
      namestartpointer++;
      namelength_memo++;
      size_memo++;
    }
  }


  void DaqTypeDataBlock::calculateTimeStamp(struct tm* timestamp){
    //Reset the array containing the timestamp variables
    _timestamptoint[0]=0;
    _timestamptoint[1]=0;

    if(timestamp) {
      //Timestamp year month day
      _timestamptoint[0] = _timestamptoint[0] | ( ( (timestamp->tm_year + TM_YEARSTART) & 0xffff) << YEARSHIFT) 
	| ( ( (timestamp->tm_mon + 1) & 0xff) << MONTHSHIFT)
	| ( (  timestamp->tm_mday & 0xff) << DAYSHIFT);  
      
      //Timestamp hour minute second
      
      _timestamptoint[1] = _timestamptoint[1] | ( (timestamp->tm_hour & 0xff) << HOURSHIFT)
	| ( (timestamp->tm_min  & 0xff) << MINUTESHIFT)
	| ( (timestamp->tm_sec  & 0xff) << SECONDSHIFT);  
    }
  }

  LCTime DaqTypeDataBlock::composeTimeStamp(){

    //composition of an intermediate LCTime object in order
    //to get everything up to seconds in nanosecs.
    LCTime init(  (_timestamptoint[0] >> YEARSHIFT) & 0xffff,
		  (_timestamptoint[0] >> MONTHSHIFT) & 0xff,     
		  (_timestamptoint[0] >> DAYSHIFT) & 0xff,     
		  (_timestamptoint[1] >> HOURSHIFT) & 0xff,     
		  (_timestamptoint[1] >> MINUTESHIFT) & 0xff,     
		  (_timestamptoint[1] >> SECONDSHIFT) & 0xff );
    LCTime finaltime(init.timeStamp());
    return finaltime;
  }

  // Convenient print method 
  void DaqTypeDataBlock::print(std::ostream& os) {
    //Check What we did, loop over the restored maps 
    os << "**************Inspecting the restored TimeStamp Maps********************" << std::endl;
    for(TimeStampMap_t::iterator timestamp_iter=_timeStampMap.begin(); timestamp_iter!=_timeStampMap.end(); timestamp_iter++) os << (*timestamp_iter).first << ": " << (*timestamp_iter).second.getDateString() << std::endl;
    os << "**************************** End *************************************" << std::endl;  


    os << "**************Inspecting the restored Int Maps********************" << std::endl;
    for ( DaqTypeDataIntMap_t::iterator slowmap_iter = _bufferOfInts.begin(); slowmap_iter != _bufferOfInts.end(); slowmap_iter++) {
      os << "Data type is: " << (*slowmap_iter).first << std::endl;
      for(std::vector<int>::const_iterator thevals_iter =
	    (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ )os << "Value: " << (*thevals_iter) << std::endl;
    }
    os << "**************************** End *************************************" << std::endl;  


    os << "**************Inspecting the restored Double Maps********************" << std::endl;
    for ( DaqTypeDataDblMap_t::iterator slowmap_iter = _bufferOfDbls.begin(); slowmap_iter != _bufferOfDbls.end(); slowmap_iter++) {
      os << "Data type is: " << (*slowmap_iter).first << std::endl;
      for(std::vector<double>::const_iterator thevals_iter =
	    (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ )os << "Value: " << (*thevals_iter) << std::endl;
    }
    os << "**************************** End *************************************" << std::endl;

    os << "**************Inspecting the restored Float Maps********************" << std::endl;
    for ( DaqTypeDataFloatMap_t::iterator slowmap_iter = _bufferOfFloats.begin(); slowmap_iter != _bufferOfFloats.end(); slowmap_iter++) {
      os << "Data type is: " << (*slowmap_iter).first << std::endl;
      for(std::vector<float>::const_iterator thevals_iter =
	    (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ )os << "Value: " << (*thevals_iter) << std::endl;
    }
    os << "**************************** End *************************************" << std::endl;

  }


}
