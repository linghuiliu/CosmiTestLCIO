#include "DhcRawChipContent.hh"


std::ostream& DhcRawChipContent::print(std::ostream &ostrm){
     ostrm << " DhcRawChipContent: "  << std::endl;
     ostrm << " Crate number: " << std::hex << getCrateNumber() << std::endl; 
     ostrm << " VME address: " << std::hex << getVmeAddress() << std::endl; 
     ostrm << " Chip address: " << getDcalAddress() << std::endl; 
     ostrm << " Data Concentrator Address: " <<  getDconAddress() << std::endl;
     ostrm << " Data Collector Address: " <<  getDcolAddress() << std::endl;
     ostrm << " Hits Hi: " <<  getHitsHi() << std::endl;
     ostrm << " Hits Lo: " <<  getHitsLo() << std::endl;
     ostrm << " Dhc Timestamp: " << std::dec << getDhcTimeStamp() << std::endl;
     ostrm << " Trigger info: " <<  getTrgInfo() << std::endl;
     ostrm << " Dbt info: " <<  getDbtInfo() << std::endl;
     ostrm << " Err info: " <<  static_cast<unsigned int> (getErrInfo()) << std::endl;
     ostrm << " Checksum from DAQ: " << std::hex << static_cast<unsigned int> (getChkSum()) << std::dec << std::endl;
     ostrm << " Versum   from DAQ: " << std::hex << static_cast<unsigned int> (getVerSum()) << std::dec << std::endl;
     return ostrm;
}
