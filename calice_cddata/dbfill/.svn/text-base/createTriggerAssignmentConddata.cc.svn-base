#include "createTriggerAssignmentConddata.hh"

#include <string>
#include <stdexcept>
#include <cassert>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <UTIL/LCTime.h>
#include <EVENT/LCIO.h>
#include <lcio.h>
#include <LCIOSTLTypes.h>

#include <ReadDaqMap/parseDateTime.hh>
#include <ReadDaqMap/ReadLine.hh>
#include <ReadDaqMap/TLineIterator.hh>

#include <collection_names.hh>

EVENT::LCCollection *createTriggerAssignmentConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &input_file) 
{
    if (input_file.empty()) {
      throw std::runtime_error("ERROR: input file is empty");
    }

    EVENT::StringVec trigger_names;
    EVENT::IntVec    trigger_mask;
    EVENT::IntVec    trigger_value;
    EVENT::IntVec    trigger_delay;
    EVENT::IntVec    trigger_jitter;
    ReadLine in(200,input_file.c_str());
    Bool_t new_line;
    std::vector<UTIL::LCTime> since_times;
    UInt_t mode=0;
    while ((new_line=in.ReadNextLine())) {
      TLineIterator line_iter(in.GetBuffer());
      if (line_iter.IsEmpty()) continue;    //skip empty ln the ines
      if (line_iter.IsComment()) continue;  //skip header
      if (mode==0) {
	for (UInt_t i=0; i<2; i++) {
	  std::string a_date_string=line_iter.GetWord(true);
	  if (a_date_string=="validity:") { 
	    a_date_string=line_iter.GetWord(true);
	  }
	  if (a_date_string.size()>32) {
	    throw std::runtime_error("ERROR:expting date string in the format 2005/02/28 or 28.02.2005 (<==10 characters).");
	  }
	  std::string a_time_string=line_iter.GetWord(true);
	  if (a_time_string.size()>32) {
	    throw std::runtime_error("ERROR:expting time stirng in the format 00:00:00 (==8 characters) or 00:00:00.0 (>9 characters).");
	  }
	  std::cout << a_date_string << " " << a_time_string << std::endl;
	  since_times.push_back(parseDateTime(a_date_string,a_time_string));
	}
	mode=1;
      }
      else {
	
	UInt_t a_bit=(line_iter.IsBinary() ?  line_iter.GetBinary() : line_iter.GetUnsignedInteger());
	UInt_t a_trigger_mask=a_bit;
	UInt_t a_trigger_value=0;
	Int_t  a_trigger_delay=0;
	Int_t  a_trigger_jitter=1;

	if (line_iter.IsBinary()) {
	  a_trigger_value=line_iter.GetBinary();
	}
	else if (line_iter.IsInteger(false)) {
	  a_trigger_value=line_iter.GetUnsignedInteger();
	}
	else {
	  if (a_bit>31) {
	    throw std::runtime_error("ERROR:trigger bit out of range (0-31).");
	  }
	  a_trigger_mask=1<<a_bit;
	  a_trigger_value=a_trigger_mask;
	}

	std::string a_trigger_name=line_iter.GetWord();
	if (a_trigger_name.size()>32) {
	  throw std::runtime_error("ERROR:trigger name suspiciously long (<32 characters).");
	}

	if (line_iter.IsInteger(true)) {
	  a_trigger_delay=line_iter.GetInteger();

	  if (line_iter.IsInteger(true)) {
	    a_trigger_jitter=line_iter.GetInteger();
	  }
	}

	std::cout << a_trigger_name << " mask=" << a_trigger_mask << " value=" << a_trigger_value << " delay=" << a_trigger_delay << " jitter=" << a_trigger_jitter << std::endl;

	trigger_names.push_back(a_trigger_name);
	trigger_mask.push_back(a_trigger_mask);
	//	trigger_value.push_back(a_trigger_value);
	trigger_delay.push_back(a_trigger_delay);
	trigger_jitter.push_back(a_trigger_jitter);
      }
    }
    if (trigger_names.empty()) {
      throw std::runtime_error("ERROR: input file does not contain trigger assignements i.e. lines with: [bit] [name]");
    }
    IMPL::LCCollectionVec* trigger_assignment_col = new IMPL::LCCollectionVec( EVENT::LCIO::LCGENERICOBJECT )  ;

    trigger_assignment_col->parameters().setValues(PAR_TRIGGER_TYPE_NAMES, trigger_names);
    trigger_assignment_col->parameters().setValues(PAR_TRIGGER_MASK,   trigger_mask);
    //    trigger_assignment_col->parameters().setValues(PAR_TRIGGER_VALUE,  trigger_value);
    trigger_assignment_col->parameters().setValues(PAR_TRIGGER_DELAY,  trigger_delay);
    trigger_assignment_col->parameters().setValues(PAR_TRIGGER_JITTER, trigger_jitter);
    

    trigger_assignment_col->addElement(new IMPL::LCGenericObjectImpl(0,0,0));
    if (since_times.size()>0) {
      since=since_times[0];
    }
    if (since_times.size()>1) {
      till=since_times[1];
    }
    assert ( since_times.size()==2 );
    return trigger_assignment_col;
}
