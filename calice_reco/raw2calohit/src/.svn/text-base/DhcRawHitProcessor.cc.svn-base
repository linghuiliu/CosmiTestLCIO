#include "DhcRawHitProcessor.hh"

#include <iostream>
#include <iomanip>
#include <sstream>




namespace marlin {

DhcRawHitProcessor aDhcRawHitProcessor;

  DhcRawHitProcessor::DhcRawHitProcessor() : Processor("DhcRawHitProcessor") {
    _description = "Processor for dhc raw data";
  }


  // DhcRawHitProcessor::~DhcRawHitProcessor() {}

  void DhcRawHitProcessor::init() {}

  void DhcRawHitProcessor::processEvent(LCEvent* evt){

    try {
      //fetch dhc raw collection
      LCCollection* col_dhc = evt->getCollection( COL_DHCRAW ) ;
      for (unsigned int ielm=0; ielm < static_cast<unsigned int>(col_dhc->getNumberOfElements()); ielm++) {
	DhcRawChipContent dhcRawChipContent(col_dhc->getElementAt(ielm));
        unsigned vmeaddress(dhcRawChipContent.getVmeAddress());
        unsigned dcaladdress(dhcRawChipContent.getDcalAddress());
        unsigned dcoladdress(dhcRawChipContent.getDcolAddress());
        unsigned dconaddress(dhcRawChipContent.getDconAddress());
        unsigned hitshi(dhcRawChipContent.getHitsHi());
        unsigned hitslo(dhcRawChipContent.getHitsLo());
        unsigned timestamp(dhcRawChipContent.getHitsLo());
        bool trginfo(dhcRawChipContent.getTrgInfo());
        bool dbtinfo(dhcRawChipContent.getDbtInfo());
        unsigned char errInfo(dhcRawChipContent.getErrInfo());
        unsigned char chkSum(dhcRawChipContent.getChkSum());
        //Print method for cross checks
	//dhcRawChipContent.print(std::cout);
      }
    } catch (  lcio::DataNotAvailableException &err ) {
      err.what();
      return;
    }



   }
  
  void DhcRawHitProcessor::end()
  {}
  
}
