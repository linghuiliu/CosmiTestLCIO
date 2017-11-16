#include "createBeamParameterConddata.hh"

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

#include <BeamParameterException.hh>


#include <RtypesSubSet.h>

using namespace CALICE;

EVENT::LCCollection *createBeamParameterConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &input_file)
{
  // ATTENTION: Internal length units are cm length. The Units used for LCObjects are mm

    Bool_t new_line;

    unsigned int energy;

    ReadLine in(200,input_file.c_str());
    while ((new_line=in.ReadNextLine())) {
      TLineIterator line_iter(in.GetBuffer());
      if (line_iter.IsEmpty()) continue;    //skip empty ln the ines
      if (line_iter.IsComment()) continue;  //skip header

      if (line_iter.IsAlpha()) {
	std::string first_word=line_iter.GetWord();
	if (first_word=="RunNumber:") { 
	  since=static_cast<long64>(line_iter.GetUnsignedInteger()); 
          till=since.timeStamp()+1LL;
	}
	else if (first_word=="Energy:") { 
	  energy=line_iter.GetUnsignedInteger();
	  std::cout << "energy: " << energy << " MeV" << std::endl;
	}
	continue;  //skip header
      }
      
    }

    //if (since.timeStamp()>=till.timeStamp()) {
    //  std::runtime_error("ERROR:createBeamParameterConddata> No valid validity range in input file.");
    //}
  

    LCCollectionVec* beam_parameter_col = new LCCollectionVec( LCIO::LCGENERICOBJECT );
    CALICE::BeamParameterException *a_beam_parameter=new CALICE::BeamParameterException;
    (*a_beam_parameter).setBeamEnergy(energy);


        

	a_beam_parameter->print(std::cout);
	beam_parameter_col->addElement(a_beam_parameter);
      
      return beam_parameter_col;
}
