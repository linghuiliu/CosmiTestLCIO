//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// date   : 050608
// author : G.Mavromanolakis
//
// code   : define an LCGenericObjectdriver to store event header info 
//          i.e. event# in run, event# in configuration, event# in spill
//        
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef EventHeader_HH
#define EventHeader_HH 1


#include <iostream>

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"

using namespace std;
using namespace lcio;



//class EventHeader : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {

namespace CALICE {
class EventHeader;


/**Class to store event header info, 
 * i.e. event# in run, event# in configuration, event# in spill 
 * 
 * @author G.Mavromanolakis
 * @date 8 Jun 2005
 */

class EventHeader : public LCGenericObjectImplExt
{
  

   public: 
  
   /**The costructor
    */
  EventHeader(int eventNumberInRun, 
               int eventNumberInConfiguration,
               int eventNumberInAcquisition) 
   {
     //obj()->setIntVal(0, eventNumberInRun);
     //obj()->setIntVal(1, eventNumberInConfiguration);
     //obj()->setIntVal(2, eventNumberInAcquisition);  
     setEventNumberInRun(eventNumberInRun); 
     setEventNumberInConfiguration(eventNumberInConfiguration);
     setEventNumberInAcquisition(eventNumberInAcquisition);
      }

   /**A copy constructor
    */
   EventHeader(LCObject* obj) : LCGenericObjectImplExt(obj) 
   { 
   } 

   /**The destructor 
    */
   virtual ~EventHeader() 
   { 
   }

   /**sets the event number in run  
    */
   void setEventNumberInRun(int evtnumberrun) 
   {
       obj()->setIntVal(0, evtnumberrun);
   }


   /**returns the event number in run  
    */
   int getEventNumberInRun() 
   {
      return obj()->getIntVal(0);
   }

   /**set the event number in configuration
    */
   void setEventNumberInConfiguration( int evtnumberconfig) 
   {
       obj()->setIntVal(1, evtnumberconfig);
   }

   /**returns the event number in configuration
    */
   int getEventNumberInConfiguration() 
   {
      return obj()->getIntVal(1);
   }

   /**returns the event number in spill
    */
   //int getEventNumberInSpill() 
   //{
   //   return obj()->getIntVal(2);
   //}

   /**sets the event number in Acquisition
    */
   void setEventNumberInAcquisition(int evtnumberacq) 
    {
      obj()->setIntVal(2, evtnumberacq);
   }

   /**returns the event number in Acquisition
    */
   int getEventNumberInAcquisition() 
   {
      return obj()->getIntVal(2);
   }

   /**returns the the type name
    */
   const std::string getTypeName() const 
   { 
      return "EventHeader" ;
   } 

   /**returns a brief description of the data stored 
    */
   const std::string getDataDescription() const 
   {
      return "int:eventNumberInRun,int:eventNumberInConfiguration,int:eventNumberInAcquisition" ;
   }

   /**dumps data content 
    */
   std::ostream& print(std::ostream &ostrm)  
   {
      ostrm 
      << "EventHeader::print() Event numbers in run " << getEventNumberInRun() 
      << ", in configuration "                        << getEventNumberInConfiguration() 
      << ", in acquisition "                                << getEventNumberInAcquisition() 
      << std::endl;
      
      return ostrm;
   }

}; //class EventHeader

}
#endif 
