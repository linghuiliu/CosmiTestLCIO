#include "FeStatList_t.hh"
#include <iostream>
//#include <iomanip>

FeStatListBase_t::~FeStatListBase_t()
{
  for(std::map<unsigned int, std::vector< std::vector< StatObjPtr > > >::iterator crate_iter=_feInfo.begin();
      crate_iter != _feInfo.end();
      crate_iter++) {
    for(std::vector< std::vector< StatObjPtr > >::iterator slot_iter=crate_iter->second.begin();
	slot_iter != crate_iter->second.end();
	slot_iter++) {
      for(std::vector< StatObjPtr >::iterator fe_iter=slot_iter->begin();
	  fe_iter != slot_iter->end();
	  fe_iter++) {
	delete *fe_iter;
	*fe_iter=0;
      }
    }
  }
}


void FeStatListBase_t::show(const char *label)
{
    bool first=true;
    for(std::map<unsigned int, std::vector< std::vector< StatObjPtr > > >::iterator crate_iter=_feInfo.begin();
	crate_iter != _feInfo.end();
	crate_iter++) {
      unsigned int slot_id=0;
      for(std::vector< std::vector< StatObjPtr > >::iterator slot_iter=crate_iter->second.begin();
	  slot_iter != crate_iter->second.end();
	  slot_iter++, slot_id++) {
	unsigned int fe_id=0;
	for(std::vector< StatObjPtr >::iterator fe_iter=slot_iter->begin();
	    fe_iter != slot_iter->end();
	    fe_iter++, fe_id++) {
	  if (*fe_iter) {
	    (*fe_iter)->finish();
	    if ((*fe_iter)->hasInfo()) {
	      if (first) {
		if (label) {
		  std::cout << label << std::endl;
		}
		std::cout << std::setw(6) << "crate" << std::setw(6) << "slot" << std::setw(3) << "FE" << " ";
		(*fe_iter)->printTableHeader(std::cout);
		first=false;
	      }
	      std::cout << std::dec << std::setw(6) << crate_iter->first << std::setw(6) << slot_id << std::setw(3) << fe_id << " ";
	      (*fe_iter)->print(std::cout);
	    }
	  }
	}
	
      }
    }

}
