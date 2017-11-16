#ifndef BeTrgPollData_HH
#define BeTrgPollData_HH 1


#include <iostream>

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
#include "collection_names.hh"
#include "UTIL/LCTime.h"
#include <sys/time.h>


using namespace std;
using namespace lcio;
using namespace CALICE;


namespace CALICE {

/** The indices of all the stored values.*/

  enum EBeTrgPollIntValues {kBeTrgPollBoardID, kBeTrgPollRecordLabel, 
                            kBeTrgPollstartTimeymd, kBeTrgPollstartTimehms, kBeTrgPollstartTimemus,
                            kBeTrgPollendTimeymd, kBeTrgPollendTimehms, kBeTrgPollendTimemus,
			    kBeTrgPollmaxTimesec, kBeTrgPollmaxTimemus,  
                            kBetrgPollnumberOfPolls,
			    kBeTrgPollTimeout, kBeTrgPollIntValues};


/**Class to store info on trigger polling, 
 * This is useful to detect a softtrigger among
 * e.g. beam trigger if e.g. the beam has disappeared
 *  @author R. Pöschl (LAL Orsay)
 *  @date Jul 02 2006
 */

class BeTrgPollData : public LCGenericObjectImplExt 
{
  

   public: 
  

  /** Default Constructor */
      BeTrgPollData() {

	obj()->setIntVal(kBeTrgPollstartTimeymd, 0);
	obj()->setIntVal(kBeTrgPollstartTimehms, 0);
	obj()->setIntVal(kBeTrgPollstartTimemus, 0);
	obj()->setIntVal(kBeTrgPollendTimeymd, 0);
	obj()->setIntVal(kBeTrgPollendTimehms, 0);
	obj()->setIntVal(kBeTrgPollendTimehms, 0);
	obj()->setIntVal(kBeTrgPollmaxTimesec, 0);  
	obj()->setIntVal(kBeTrgPollmaxTimemus, 0);  
	obj()->setIntVal(kBetrgPollnumberOfPolls, 0);  
	obj()->setIntVal(kBeTrgPollTimeout, 0);  
	
      }; 

//............................................................................
   /**A copy constructor
    */
   BeTrgPollData(LCObject* obj) : LCGenericObjectImplExt(obj) 
   { 
   } 
//............................................................................
   /**The destructor 
    */
   virtual ~BeTrgPollData() 
   { 
   }
//............................................................................  

    /** set the packed board id.
     * @sa BoardID
     */
    BeTrgPollData &setBoardID(int boardID) {
       obj()->setIntVal(kBeTrgPollBoardID, boardID); 
       return *this;
    };

    /**get the board id */
    int getBoardID() const {return getIntVal(kBeTrgPollBoardID);};

    /** Set the Record Label*/
    BeTrgPollData &setRecordLabel(int label) {
       obj()->setIntVal(kBeTrgPollRecordLabel,label);
       return *this;
    };


    /** Return the Record Label */
    short getRecordLabel() const {
      return (short) ( getIntVal(kBeTrgPollRecordLabel) & 0xff);
    };



   /**set the start time  
    * We store Year, Month, Day and Hour, Minute, Second
    * and microsceconds seperately
    * We store UTC!!!!
    */
   BeTrgPollData& setStartTime(struct tm* startTime, int startTimemus) 
   {

     //Starttime year month day
     int startTimeymd(0);
     startTimeymd = startTimeymd | ( ( (startTime->tm_year + TM_YEARSTART) & 0xffff) << YEARSHIFT) 
                                 | ( ( (startTime->tm_mon + 1) & 0xff) << MONTHSHIFT)
                                 | ( (  startTime->tm_mday & 0xff) << DAYSHIFT);  


     //Starttime hour minute second
     int startTimehms(0);
     startTimehms = startTimehms | ( (startTime->tm_hour & 0xff) << HOURSHIFT)
                                 | ( (startTime->tm_min  & 0xff) << MINUTESHIFT)
                                 | ( (startTime->tm_sec  & 0xff) << SECONDSHIFT);  


     obj()->setIntVal(kBeTrgPollstartTimeymd, startTimeymd);
     obj()->setIntVal(kBeTrgPollstartTimehms, startTimehms);
     //... and the microseconds
     obj()->setIntVal(kBeTrgPollstartTimemus, startTimemus);
     return *this;
   }

   /**returns the start time  
    */
   LCTime getStartTime() 
   {

     int starttimeymd(obj()->getIntVal(kBeTrgPollstartTimeymd));
     int starttimehms(obj()->getIntVal(kBeTrgPollstartTimehms));

     //composition of an intermediate LCTime object in order
     //to get everything up to seconds in nanosecs.
     LCTime init(  (starttimeymd >> YEARSHIFT) & 0xffff,
                   (starttimeymd >> MONTHSHIFT) & 0xff,     
                   (starttimeymd >> DAYSHIFT) & 0xff,     
                   (starttimehms >> HOURSHIFT) & 0xff,     
                   (starttimehms >> MINUTESHIFT) & 0xff,     
                   (starttimehms >> SECONDSHIFT) & 0xff );
     //Add the microseconds
     long64 starttimemus = static_cast<long64>(obj()->getIntVal(kBeTrgPollstartTimemus));        
     LCTime finaltime(init.timeStamp() + starttimemus*1000);
     return finaltime;

   }

   /**set the end time  
    * (see set of start time)
    */
   BeTrgPollData& setEndTime(struct tm* endTime, int endTimemus) 
   {

     int endTimeymd(0);
     endTimeymd = endTimeymd | ( ( (endTime->tm_year + TM_YEARSTART ) & 0xffff) << YEARSHIFT)
                             | ( ( (endTime->tm_mon +1) & 0xff) << MONTHSHIFT)
                             | ( (endTime->tm_mday & 0xff) << DAYSHIFT);  

     int endTimehms(0);
     endTimehms = endTimehms | ( (endTime->tm_hour & 0xff) << HOURSHIFT)
                             | ( (endTime->tm_min  & 0xff) << MINUTESHIFT)
                             | ( (endTime->tm_sec  & 0xff) << SECONDSHIFT);  


     obj()->setIntVal(kBeTrgPollendTimeymd, endTimeymd);
     obj()->setIntVal(kBeTrgPollendTimehms, endTimehms);
     obj()->setIntVal(kBeTrgPollendTimemus, endTimemus);

     return *this;
   }

   /**returns the end time  
    * (see start time)
    */
   LCTime getEndTime() 
   {

     //composition of an intermediate LCTime object in order
     //to get everything up to seconds in nanosecs.
     int endtimeymd(obj()->getIntVal(kBeTrgPollendTimeymd));
     int endtimehms(obj()->getIntVal(kBeTrgPollendTimehms));

     LCTime init(  (endtimeymd >> YEARSHIFT) & 0xffff,
                   (endtimeymd >> MONTHSHIFT) & 0xff,     
                   (endtimeymd >> DAYSHIFT) & 0xff,     
                   (endtimehms >> HOURSHIFT) & 0xff,     
                   (endtimehms >> MINUTESHIFT) & 0xff,     
                   (endtimehms >> SECONDSHIFT) & 0xff );

     //Add the microseconds
     long64 endtimemus = static_cast<long64>(obj()->getIntVal(kBeTrgPollendTimemus));        
     LCTime finaltime(init.timeStamp() + endtimemus*1000);

     return finaltime;

   }

   /**set the maximum time  
    */
   BeTrgPollData& setMaximumTime(int maxTimesec, int maxTimemus) 
   {

     //Set secs and musecs seperately
     obj()->setIntVal(kBeTrgPollmaxTimesec, maxTimesec);
     obj()->setIntVal(kBeTrgPollmaxTimemus, maxTimemus);
     return *this;
   }

   /**returns the maximum time  sec. part
    */
   
   int getMaximumTimeSec() 
   {
     
      return obj()->getIntVal(kBeTrgPollmaxTimesec);
   }

   /**returns the maximum time  musec. part
    */

   int getMaximumTimeMus() 
   {
     
      return obj()->getIntVal(kBeTrgPollmaxTimemus);
   }


   /**set the number of polls  
    */
   BeTrgPollData& setNumberOfPolls(int numPolls) 
   {
     obj()->setIntVal(kBetrgPollnumberOfPolls, numPolls);
     return *this;
   }

   /**returns the number of polls time  
    */
   int getNumberOfPolls() 
   {
      return obj()->getIntVal(kBetrgPollnumberOfPolls);
   }


   /**set the info on timeout  
    */
   BeTrgPollData& setTimeout(int timeout) 
   {
     obj()->setIntVal(kBeTrgPollTimeout, timeout);
     return *this;
   }

  /**timeout set? */
   bool isTimeout() 
   {
      return static_cast<bool>(obj()->getIntVal(kBeTrgPollTimeout));
   }



   /**returns the the type name
    */
   const std::string getTypeName() const 
   { 
      return "BeTrgPollData" ;
   } 
//............................................................................      
   /**returns a brief description of the data stored 
    */
   const std::string getDataDescription() const 
   {
      return "int:BoardID, int:RecordLabel, int:startTimeymd, int:startTimehms, int:startTimemus, int:endTimeymd,int:endTimehms, int:endTimemus, int:maximumTimesec, int:maximumTimemus, int:numberOfPolls, int:timeout" ;
   }
//............................................................................      
   /**dumps data content 
    */
   std::ostream& print(std::ostream &ostrm)  
   {
      
     ostrm << " BeTrgPollData: "  << std::endl;
     ostrm << " Board: " << BoardID(getBoardID()) << std::endl;
     ostrm << " Record Label: " << getRecordLabel() << std::endl;
     ostrm << " Poll Starttime (UTC): " << getStartTime().getDateString() << std::endl; 
     ostrm << " Poll Endtime (UTC): " << getEndTime().getDateString() << std::endl;
     ostrm << " Poll Maximumtime: " << getMaximumTimeSec() << "." << getMaximumTimeMus()<< std::endl; 
     ostrm << " Number of Polls: " << getNumberOfPolls() << std::endl;
     ostrm << " is Timeout? " << isTimeout() << std::endl;
     
     return ostrm;
   }
//............................................................................      


}; //class AcquisitionData

}
#endif 
//............................................................................
