#include "EventChecker.hh"

#include "lcio.h"
#include "EVENT/LCCollection.h"


#include "LabviewBlock.hh"


namespace CALICE {

EventChecker aEventChecker;

  EventChecker::EventChecker() : Processor("EventChecker") {
    _description = "Processor for checking the event builder";

   registerProcessorParameter("InputCollectionName",
                               "Name of the input collection of Labview raw data to be checked",
                               _inputColName,
                               std::string("LabviewData"));

  }


  EventChecker::~EventChecker() {}

  void EventChecker::init() {}

  void EventChecker::processEvent(LCEvent* evt){

    try {
      //fetch Labview data raw collection
      LCCollection* col = evt->getCollection( _inputColName ) ;
      
      //count elements in ChipID 129:0, 130:1, ... 144:15
      //init to 0.
      int counter[16]; 
      for (int i = 0; i<16;i++){
	counter[i]=0;
      }
      
      //check all the ChipID, each one has 36 channels, 
      for (unsigned int ielm=0; ielm < static_cast<unsigned int>(col->getNumberOfElements()); ielm++) {

	//LCObject *obj = col->getElementAt(ielm);
	//LabviewBlock lBlock(obj);
	LabviewBlock lBlock( (col->getElementAt(ielm)) );
	
	
	//Change the ChipID [129,144] into CID [0,15]; 
	//i.e 129->0, 130->1, ... 144->15
	int CID = lBlock.GetChipID() - 129; 
	
	counter[CID]++;
	  
      }
      
      for (int i = 0; i<16;i++){
	if( counter[i] > 0 && counter[i] != 36 ){
	  std::cout<<"Event number: "<< evt->getEventNumber()
		   <<"     ChipID: "      << (i+129) //Print in ChipID number, i.e. 0->129, 1->130, ... 15->144
		   <<"     Entries: "     << counter[i]
		   <<std::endl;
	  return;
	}
      }
    



    } catch (  lcio::DataNotAvailableException &err ) {
      err.what();
      return;
    }
    

   }
  
  void EventChecker::end()
  {}
  
}
