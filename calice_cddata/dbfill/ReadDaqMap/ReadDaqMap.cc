#include <ReadDaqMap.hh>

#include <ReadLine.hh>
#include <TLineIterator.hh>
#include <sstream>

#include "parseDateTime.hh"

ReadDaqMap::ReadDaqMap(const char *file_name) throw(runtime_error)
  : _crate(0),
    _since(0),
    _till(0)
{
  ReadLine input(80,file_name);
  try {
  while (input.ReadNextLine()) {
    TLineIterator line_iter(input.GetBuffer());
    if (line_iter.IsEmpty()) continue;  //skip empty lines
    if (line_iter.IsComment()) continue;  //skip comment lines
    if (line_iter.IsAlpha()) {
      std::string first_word=line_iter.GetWord();
      if (first_word=="validity:") { 
	_since=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
	_till=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
      }
      else if (first_word=="crate:") { 
	_crate=line_iter.GetUnsignedInteger();
      }
      continue;  //skip header
    }

    UInt_t slot_i=line_iter.GetUnsignedInteger();
    UInt_t fe_i=line_iter.GetUnsignedInteger();
    UInt_t connector_side_i=line_iter.GetUnsignedInteger();
    UInt_t pcb_stack_layer=line_iter.GetUnsignedInteger();
    UInt_t pcb_position_in_layer=line_iter.GetUnsignedInteger();
    
    string pcb_name=line_iter.GetWord(true);

    if (connector_side_i>2) {
      stringstream message;
      message << "Unhandled connector type : " <<  input.GetLineNumber() << ". The connector can be: 0=left or central; 1=right; 2=full connector (left).";
      throw runtime_error(message.str());
    }

    UInt_t pcb_id=0;
    FeConnectionDef_t::EModuleType pcb_type;
    {
      string::size_type pos=0;

      if (pcb_name.size()==0) {
	stringstream message;
	message  << "Missing PCB id in line " << input.GetLineNumber();
	throw runtime_error(message.str());
      }
      if (
#if ( __GNUC__ < 3)
          pcb_name.compare("PCB",0,3)==0
#else
          pcb_name.compare(0,3,"PCB")==0
#endif
      ) {
	pos=3;
	if (pcb_name[pos]=='_') pos++;
      }
      pcb_id=atoi(&(pcb_name.c_str()[pos]));
      if (pcb_id==0) {
	stringstream message;
	message  << "PCB id should not be zero in line " << input.GetLineNumber();
	throw runtime_error(message.str());
      }
      string::size_type ext_pos=pcb_name.find("_",pos);
      if (ext_pos==string::npos) {
	//if (pcb_name.size()==0) {
	//	  stringstream message;
	//	  message  << "Missing extension (_C,_D,_G) for PCB id in line " << input.GetLineNumber();
	//	  throw runtime_error(message.str());
	//	}
	pcb_type=FeConnectionDef_t::kCentral;
      }
      else {
	ext_pos++;
      
	cout << pcb_name << " -> " <<  string(pcb_name,ext_pos,pcb_name.size()-ext_pos) << " " << ext_pos << "+" <<pcb_name.size()-ext_pos << endl;
	if (
#if ( __GNUC__ < 3 )
	    pcb_name.compare("C",ext_pos,pcb_name.size()-ext_pos)==0
#else
	    pcb_name.compare(ext_pos,pcb_name.size()-ext_pos,"C")==0
#endif
	    ) {
	  pcb_type=FeConnectionDef_t::kCentral;
	}
	else if (
#if ( __GNUC__ < 3 ) 
		 pcb_name.compare("D",ext_pos,pcb_name.size()-ext_pos)==0 || pcb_name.compare("R",ext_pos,pcb_name.size()-ext_pos)==0
#else
		 pcb_name.compare(ext_pos,pcb_name.size()-ext_pos,"D")==0 || pcb_name.compare(ext_pos,pcb_name.size()-ext_pos,"R")==0
#endif
		 ) {
	  pcb_type=FeConnectionDef_t::kLowRight;
	}
	else if (
#if ( __GNUC__ < 3 ) 
		 pcb_name.compare("G",ext_pos,pcb_name.size()-ext_pos)==0 || pcb_name.compare("L",ext_pos,pcb_name.size()-ext_pos)==0
#else
		 pcb_name.compare(ext_pos,pcb_name.size()-ext_pos,"G")==0 || pcb_name.compare(ext_pos,pcb_name.size()-ext_pos,"L")==0
#endif
		 ) {
	  pcb_type=FeConnectionDef_t::kLowLeft;
	}
	else {
	  stringstream message;
	  message  << "Extension of PCB id is not one of: central:\"_C\", low left: \"_G\",\"_L\", low right:\"_D\", \"_R\" in line" << input.GetLineNumber();
	  throw runtime_error(message.str());
	}
      }
      
      if (   (pcb_type==FeConnectionDef_t::kCentral && pcb_position_in_layer!=1)
          || (pcb_type!=FeConnectionDef_t::kCentral && pcb_position_in_layer==1)) {
	stringstream message;
	message  << "Module type suggested by the extension of PCB id does not agree with specifier (lower/centre) in line" << input.GetLineNumber();
	throw runtime_error(message.str());
      }
    }

    FeConnectionDef_t::EModuleType module_type=FeConnectionDef_t::kCentral;
    if (connector_side_i==0) module_type=FeConnectionDef_t::kLowLeft;
    else if (connector_side_i==1) module_type=FeConnectionDef_t::kLowRight;
    _mapping[slot_i][fe_i*2+connector_side_i%2]=FeConnectionDef_t(slot_i, fe_i,module_type ,pcb_stack_layer, pcb_type, pcb_id, pcb_name);
  }
  ShowChannelMap();
  }
  catch (std::runtime_error &err) {
    std::stringstream message;
    message << "ReadDaqMap> Error in " << file_name << ":" << input.GetLineNumber() << " :: " << err.what();
    throw std::runtime_error(message.str());
  }
}

void ReadDaqMap::ShowChannelMap() const 
{
  map<UInt_t, map< UInt_t, FeConnectionDef_t>::const_iterator > layer_order;
  for (map< UInt_t, map< UInt_t, FeConnectionDef_t> >::const_iterator slot_iter=_mapping.begin();
       slot_iter!=_mapping.end();
       slot_iter++) {
    for (map< UInt_t, FeConnectionDef_t>::const_iterator fe_iter=slot_iter->second.begin();
	 fe_iter!=slot_iter->second.end();
	 fe_iter++) {
      UInt_t layer_number=fe_iter->second.getLayerNumber()*3+(UInt_t) fe_iter->second.getModuleType();
      if (layer_order.find(layer_number)!=layer_order.end()) {
	cout << "Multiple modules located in layer " << fe_iter->second.getLayerNumber() << endl;
      }
      layer_order[layer_number]=fe_iter;
    }
  }

  for (map<UInt_t, map< UInt_t, FeConnectionDef_t>::const_iterator >::const_iterator layer_iter=layer_order.begin();
       layer_iter!=layer_order.end();
       layer_iter++) {
    map< UInt_t, FeConnectionDef_t>::const_iterator fe_iter=layer_iter->second;

    cout << (fe_iter->second.getLayerNumber()) << ":"
	 << (fe_iter->second.getModuleName())
	 << " id=" << (fe_iter->second.getModuleID())
	 << " type=" 
	 << (fe_iter->second.getModuleType()==FeConnectionDef_t::kCentral ? 
	     "central" 
	     : (fe_iter->second.getModuleType()==FeConnectionDef_t::kLowLeft ? "low left" : "low right"))
	 << " slot=" << (fe_iter->second.getSlot())
	 << " FE=" << (fe_iter->second.getFrontEnd())
	 << " connector side=" << (fe_iter->second.getFrontEndSide())
	 << endl;
  }
}

