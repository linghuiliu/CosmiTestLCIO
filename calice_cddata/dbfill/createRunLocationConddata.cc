#include "createRunLocationConddata.hh"

#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <cassert>

#include <IMPL/LCCollectionVec.h>
#include <UTIL/LCTime.h>

#include <ReadDaqMap/parseDateTime.hh>
#include <ReadDaqMap/ReadLine.hh>
#include <ReadDaqMap/TLineIterator.hh>

#include <RunLocation.hh>


#include <RtypesSubSet.h>



EVENT::LCCollection *createRunLocationConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &input_file)
{
  // ATTENTION: Internal length units are cm length. The Units used for LCObjects are mm

    Bool_t new_line;

    std::string location;
    std::string generic_type;
    std::string month_info;

    ReadLine in(200,input_file.c_str());
    while ((new_line=in.ReadNextLine())) {
      TLineIterator line_iter(in.GetBuffer());
      if (line_iter.IsEmpty()) continue;    //skip empty ln the ines
      if (line_iter.IsComment()) continue;  //skip header

      if (line_iter.IsAlpha()) {
	std::string first_word=line_iter.GetWord();
	if (first_word=="RunRange:") { 
	  since=static_cast<long64>(line_iter.GetUnsignedInteger()); 
	  till=static_cast<long64>(line_iter.GetUnsignedInteger()); 
	}
	else if (first_word=="Location:") { 
	  location=line_iter.GetWord();
	  std::cout << "location: " << location << std::endl;
	}
	else if (first_word=="generic_Type:") { 
	  generic_type=line_iter.GetWord();
	  std::cout << "generic Type: " << generic_type << std::endl;
	}
	else if (first_word=="month_Info:") { 
	  month_info=line_iter.GetWord();
	  std::cout << "Month Info: " << month_info << std::endl;
        }
	continue;  //skip header
      }
      
    }

    if (since.timeStamp()>=till.timeStamp()) {
      std::runtime_error("ERROR:createEcalModuleLocationConddata> No valid validity range in input file.");
    }
  

    LCCollectionVec* run_location_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;
    CALICE::RunLocation *a_run_location=new CALICE::RunLocation;
    (*a_run_location).setRunLocationParameters(location, generic_type, month_info);


        

	a_run_location->print(std::cout);
	run_location_col->addElement(a_run_location);
      
      return run_location_col;
}
