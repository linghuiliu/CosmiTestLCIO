#ifndef __SCECALTEMPERATURE_HH__
#define __SCECALTEMPERATURE_HH__  1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE {

  class ScECALTemperature : public lcio::LCFixedObject<5,0,3> {

  public:

    /**  Empty constructor
     */

    ScECALTemperature(){}

    ScECALTemperature(lcio::LCObject* obj) : lcio::LCFixedObject<5,0,3>(obj){}

    ScECALTemperature(int year, int month, int day, int hour_minute, int second, 
					   double timestamp, double temp1, double temp2 ) {

      obj()->setIntVal(0, year);
      obj()->setIntVal(1, month);
      obj()->setIntVal(2, day);
      obj()->setIntVal(3, hour_minute);
      obj()->setIntVal(4, second);
      obj()->setDoubleVal(0, timestamp);
      obj()->setDoubleVal(1, temp1);
      obj()->setDoubleVal(2, temp2);
    }

    /**  Destructor
     */
    ~ScECALTemperature(){
    }

    /** Get identifiers.
     */
    int getyear() const { //

      return getIntVal(0);

    }

    int getmonth() const { //

      return getIntVal(1);

    }
    int getday() const { //

      return getIntVal(2);

    }
     int gethour_minute() const { //

      return getIntVal(3);

    }
     int getsecond() const { //

      return getIntVal(4);

    }
 
    /** Get timestamp.
     *
     */
    double gettimestamp() const {

      return getDoubleVal(0);

    }

    /** Get temperatures.
     */
    double gettemp1() const {

      return getDoubleVal(1);

    }

     double gettemp2() const {

      return getDoubleVal(2);

    }
   
    /** Sets  identifiers.
     */
    void setyear(const int year) {

      obj()->setIntVal(0, year);

    }
    
    void setmonth(const int month) {

      obj()->setIntVal(1, month);

    }
       
    void setday(const int day) {

      obj()->setIntVal(2, day);

    }
    
    void sethour_minute(const int hour_minute) {

      obj()->setIntVal(3, hour_minute);

    }
  
    void setsecond(const int second) {

      obj()->setIntVal(4, second);

    }
   
    /** Sets timestamp.
     */
    void settimestamp(const double timestamp) {

      obj()->setDoubleVal(0, timestamp);

    }


    /** Sets temperatures.
     */
    void settemp1(const double temp1) {

      obj()->setDoubleVal(1, temp1);

    }

    void settemp2(const double temp2) {

      obj()->setDoubleVal(2, temp2);

    }

   /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "ScECALTemperature";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:year,"
	     "i:month,"
	     "i:day,"
	     "i:hour_minute,"
	     "i:second,"
             "f:timestamp,"
             "f:temp1,"
             "f:temp2,";
    }

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::ScECALTemperature& lfr) {

    out << "year: " << lfr.getyear()
        << " month: " << lfr.getmonth()
        << " day: " << lfr.getday()
        << " hour_minute: " << lfr.gethour_minute()
        << " second: " << lfr.getsecond()
        << " timestamp: " << lfr.gettimestamp()
        << " temp1: " << lfr.gettemp1()
        << " temp2: " << lfr.gettemp2();

    return out;

  }

}


#endif









