#include "createDetectorTransformationConddata.hh"

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

#include <DetectorTransformation.hh>

#include <RtypesSubSet.h>


EVENT::LCCollection *createDetectorTransformationConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &geometry_file)
{
  // ATTENTION: Internal length units are cm length. The Units used for LCObjects are mm
  const Float_t length_unit=1e-2/1e-3; // comversion from cm to mm

    Bool_t new_line;
    Double_t rotation_zx=0;
    Double_t rotation_x0=0;
    Double_t rotation_z0=0;
    Double_t position_x=0;
    Double_t position_y=0;
    Double_t position_z=0;

    ReadLine in(200,geometry_file.c_str());
    while ((new_line=in.ReadNextLine())) {
      TLineIterator line_iter(in.GetBuffer());
      if (line_iter.IsEmpty()) continue;    //skip empty ln the ines
      if (line_iter.IsComment()) continue;  //skip header

      if (line_iter.IsAlpha()) {
	std::string first_word=line_iter.GetWord();
	if (first_word=="validity:") { 
	  if (since.timeStamp()<till.timeStamp()) {
	    continue;
	  }
	  else {
	    since=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
	    till=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
	  }
	}
	else if (first_word=="rotationAngleZX:") { 
	  rotation_zx=static_cast<Float_t>(line_iter.GetDouble());
	}
	else if (first_word=="rotationX0:") { 
	  rotation_x0=static_cast<Float_t>(line_iter.GetDouble());
	}
	else if (first_word=="rotationZ0:") { 
	  rotation_z0=static_cast<Float_t>(line_iter.GetDouble());
	}
	else if (first_word=="detectorPostionX:") { 
	  position_x=static_cast<Float_t>(line_iter.GetDouble());
	}
	else if (first_word=="detectorPostionY:") { 
	  position_y=static_cast<Float_t>(line_iter.GetDouble());
	}
	else if (first_word=="detectorPostionZ:") { 
	  position_z=static_cast<Float_t>(line_iter.GetDouble());
	}
	else {
	  std::stringstream message;
	  message << "ERROR:createDetectorTransformationConddata> " << "Invalid key word " << first_word << " in line " << in.GetLineNumber() << ".";
	  throw std::runtime_error(message.str());
	}
	continue;  //skip header
      }
      else {
	std::stringstream message;
	message << "ERROR:createDetectorTransformationConddata> " << "Error in line " << in.GetLineNumber() << ".";
	throw std::runtime_error(message.str());
      }
    }

    if (since.timeStamp()>=till.timeStamp()) {
      std::runtime_error("ERROR:createEcalModuleLocationConddata> No valid validity range in input file.");
    }

    LCCollectionVec* detector_postition_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;
    CALICE::DetectorTransformation *transform= new CALICE::DetectorTransformation;
    (*transform).setDetectorAngleZX(rotation_zx)
      .setDetectorRotationX0(rotation_x0*length_unit)
      .setDetectorRotationZ0(rotation_z0*length_unit)
      .setDetectorTransformation(position_x*length_unit, position_y*length_unit, position_z*length_unit);

    detector_postition_col->addElement(transform);
    return detector_postition_col;
}
