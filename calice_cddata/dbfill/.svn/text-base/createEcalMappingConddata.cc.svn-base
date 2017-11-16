#include "createEcalMappingConddata.hh"

#include <string>
#include <stdexcept>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <UTIL/LCTime.h>
#include <lcio.h>

#include <ReadDaqMap.hh>
#include <FeConnectionDef_t.hh>

#include <CellIndex.hh>
#include <ModuleConnection.hh>

EVENT::LCCollection *createEcalMappingConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &map_file, unsigned int crate_nr)
{
  if (map_file.empty()) return 0;

    ReadDaqMap daq_map(map_file.c_str());

    if (since.timeStamp()>=till.timeStamp()) {
      since=daq_map.since();
      till=daq_map.till();
      if (since.timeStamp()>=till.timeStamp()) {
	throw std::runtime_error("ERROR:: createEcalMappingConddata> There is neither a validity range inside the map file nor are since/till times given via the command line.");
      }
    }

    const ReadDaqMap::SlotMap_t &mapping=daq_map.getMapping();
    if (crate_nr==static_cast<UInt_t>(-1)) {
      if (daq_map.crate()==0) {
	throw std::runtime_error("ERROR:: createEcalMappingConddata> No crate number given.");
      }
      else {
	crate_nr=daq_map.crate();
      }
    }

    LCCollectionVec* mapping_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;

    for(ReadDaqMap::SlotMap_t::const_iterator slot_iter=mapping.begin();
	slot_iter!=mapping.end();
	slot_iter++) {
      for (ReadDaqMap::FeMap_t::const_iterator fe_iter=slot_iter->second.begin();
	   fe_iter!=slot_iter->second.end();
	   fe_iter++) {
	CALICE::ModuleConnection *a_connection=new CALICE::ModuleConnection;

	// changed layer numbering from 1 to 30

	std::cout << "Before Cell Index: " << std::endl;
	CALICE::CellIndex cell_index((fe_iter->second.getModuleType()==FeConnectionDef_t::kCentral ? 1 : 0), //offset on wafer row
			     1,
			     0,
			     0,
			     fe_iter->second.getLayerNumber() +1 ); 
	
	CALICE::EModuleConnectorType connector_type=CALICE::kFullConnector;
	if (fe_iter->second.getFrontEndSide()==1) {
	  connector_type=CALICE::kRightConnector;
	}
	else if (fe_iter->second.getModuleType()!=FeConnectionDef_t::kCentral) {
	  connector_type=CALICE::kLeftConnector;
	}

	(*a_connection)
	  .setCrate(crate_nr)
	  .setSlot(slot_iter->first)
	  .setFrontEnd(fe_iter->second.getFrontEnd())
	  .setConnectorType( connector_type )
	  .setIndexOfLowerLeftCell(cell_index.getCellIndex())
	  .setModuleType(fe_iter->second.getModuleType())
	  .setModuleID(fe_iter->second.getModuleID());
	  
	  mapping_col->addElement(a_connection);
      }
    }
    return mapping_col;
}
