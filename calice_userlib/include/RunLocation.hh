#ifndef RunLocation_hh
#define RunLocation_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include <sstream>
#include <string>
#include <iostream>


#include <RtypesSubSet.h>
#include <stringFromToInts.hh>

using namespace lcio ;
using namespace std;
namespace CALICE {
//namespace CALICE {



enum ERunLocationIntType {kStartIndexRunLocationInt,
                          kLengthIndexRunLocationInt,
                          kStartIndexRunTypeInt,
                          kLengthIndexRunTypeInt,
                          kStartIndexMonthInfo,
                          kLengthIndexMonthInfo,
			  kNRunLocationInts};

/** 
 * run and the generic run type i.e. for cases where more than one calorimenter was present
 * In addition a generic month info is stored which indicates the main or start (of a) running period 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Oct  2006
 */

class RunLocation : public LCGenericObjectImplExt {

public: 
  RunLocation(){};

  /** 'Copy constructor', create the RunLocation interface for the generic objects read from file/database.
   * The objects stored in the file or database are of type LCGenericObject. To generate the interface of the
   * object RunLocation:
   * @code
   * RunLocation a_module_location(lc_collection->getElementAt(index));
   * @endcode
   * @todo Currenlty, there is no serious type checking. Only the number of integers, floats and doubles is used
   * @todo       to verify whether the LCGenericObject is of the type RunLocation
   */

  RunLocation(LCObject* obj) : LCGenericObjectImplExt(obj) {}; 
  
  /** Set the location, generic Runtype and the monthinfo, these strings are decomposed and stored 
    * as a sequence of integer values
    */
  RunLocation &  setRunLocationParameters(std::string location, std::string runtype, std::string monthinfo) 
  {
    //obj()->setIntVal(kRunLocationIntLocation, atoi(location.c_str());
    obj()->setIntVal(kStartIndexRunLocationInt, kNRunLocationInts);
    UInt_t nextindex = convertStringToInts(obj(), location, kNRunLocationInts, kLengthIndexRunLocationInt);
    obj()->setIntVal(kStartIndexRunTypeInt, nextindex);
    nextindex = convertStringToInts(obj(), runtype, nextindex, kLengthIndexRunTypeInt);    
    obj()->setIntVal(kStartIndexMonthInfo, nextindex);
    nextindex = convertStringToInts(obj(), monthinfo, nextindex, kLengthIndexMonthInfo);    
    _obj=obj();
    return *this;
  };

    /** Return the location */
  std::string getLocation() const
  {
    return getStringFromInts(_obj, getIntVal(kStartIndexRunLocationInt), kLengthIndexRunLocationInt);
  };


  /** Return the generic type of the run */
  std::string getGenericRunType() const
  {
    return getStringFromInts(_obj, getIntVal(kStartIndexRunTypeInt), kLengthIndexRunTypeInt);
  };


  /** Return the month info for the run */
  std::string getMonthInfo() const
  {
    return getStringFromInts(_obj, getIntVal(kStartIndexMonthInfo), kLengthIndexMonthInfo);
  };



  std::string getConditionsFolder() const {
    std::string folder = "/cd_calice_";
    if ( getLocation() == "Desy" ) folder += "beam";
    else if ( getLocation() == "Cern" ) folder += "cernbeam";
    else if ( getLocation() == "Fnal" ) folder += "fnalbeam";
    else if ( getLocation() == "DesyTent" ) folder += "tent";
    else if ( getLocation() == "DesyLab" ) folder += "desylab";
    return folder;
  }


  std::string getConverterFolder( const std::string convVersion ) const {
    std::string folder = "/cd_calice_v";
    folder += convVersion;
    folder += "_";
    folder += getGenericRunType();
    return folder;
  }



  void print(  std::ostream& os );
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("RunLocation");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "string: runlocation, generic_type, monthinfo";
    return message.str();
  };

}; // class

//}
}
#endif 
