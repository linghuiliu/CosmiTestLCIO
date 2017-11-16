#ifdef USE_LCCD
#include <MappingAndAlignment.hh>
#include <DetectorTransformation.hh>
#include <ExperimentalSetup.hh>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <Exceptions.h>

#include "EVENT/LCCollection.h"

// #define RECO_DEBUG

namespace CALICE {

  int debug = 0;

  const UInt_t  MappingAndAlignment::__nLines    =12;
  const UInt_t  MappingAndAlignment::__nLinesHalf=6;
  const UInt_t  MappingAndAlignment::__nFrontEnds=8;


  MappingAndAlignment::MappingAndAlignment() 
    : _nCells(0),
      _viewConnectionTree(false),
      _maxNCells(0)

  {
  }

  void MappingAndAlignment::init()
  {
    Alignment::init();
  }


  string MappingAndAlignment::getModuleName(UInt_t module_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error,logic_error)
#else
    throw (logic_error)
#endif
  {
#ifdef BOUNDARY_CHECK
      if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
	std::stringstream message;
	message << "MappingAndAlignment::getModuleName> Module " << module_index << "is not defined.";
	throw range_error(message.str());
      }
#endif
      if (!isModuleConnected(module_index)) {
	std::stringstream message;
	message << "MappingAndAlignment::getModuleName> Module " << module_index << "is not connected.";
	throw logic_error(message.str());
      }
      std::stringstream name;
      name << "PCB" 
	   << _moduleLocationList[module_index].first
	   << "_" << _moduleTypeList[_moduleLocationList[module_index].second.getModuleType()].getModuleTypeName();
      return name.str();
  }


  void MappingAndAlignment::moduleTypeChanged(lcio::LCCollection* col)
  {
    if(debug>0) cout<<"MapAndAlign.moduleTypeChanged(): enter"<<endl;
    //cellid encoding for the cells in the modules which are descrvied in this collection
    _cellIDEncoding=col->getParameters().getStringVal(LCIO::CellIDEncoding); 
    Alignment::moduleTypeChanged(col);
    rebuildConnectionTree(); 
    correctFullHalfConnectorErrors();
    determineMaximumCellNumberPerFrontEnd();
    if(debug>0) cout<<"MapAndAlign.moduleTypeChanged(): exit"<<endl;
  }

  void MappingAndAlignment::moduleLocationChanged(lcio::LCCollection* col) {
    if(debug>0) cout<<"MapAndAlign.moduleLocationChanged(): enter"<<endl;
    Alignment::moduleLocationChanged(col);
    _moduleIndex.clear();
    for (UInt_t module_i=0; module_i<_moduleLocationList.size();module_i++) {
      const ModuleLocation &location(_moduleLocationList[module_i].second);
      _moduleIndex.insert(make_pair(location.getCellIndexOffset(),module_i));
    }

    rebuildConnectionTree(); 
    correctFullHalfConnectorErrors();
    determineMaximumCellNumberPerFrontEnd();
    if(debug>0) cout<<"MapAndAlign.moduleLocationChanged(): exit"<<endl;
  }

  void MappingAndAlignment::moduleConnectionChanged(lcio::LCCollection* col)
  {
    if(debug>0) cout<<"MapAndAlign.moduleConnectionChanged(): enter"<<endl;
#ifdef RECO_DEBUG  
    std::cout << "MappingAndAlignment::moduleConnectionChanged>" << std::endl;
#endif    
    if (!col) {
      throw std::runtime_error("MappingAndAlignment::moduleConnectionChanged> Missing ModuleConnection condditions data. Sorry: don't know the folder name neither the time stamp.");
    }

    //    std::cout << "MappingAndAlignment::moduleConnectionChanged>" << std::endl;
    _moduleConnectionList.clear();
    clearTcmtConnections();
    for (UInt_t element_i=0; element_i<(UInt_t) col->getNumberOfElements(); element_i++) {

      LCGenericObject* genObj = (LCGenericObjectImpl*)col->getElementAt(element_i);
      if(genObj->getNInt()==7) _moduleConnectionList.push_back(ModuleConnection(col->getElementAt(element_i)));

      // Tcmt connections
      if(genObj->getNInt()==5) {
	// fill connections map, keyed by elecID (slot,fe,chip,channel)
	TcmtConnection* connection = new TcmtConnection(col->getElementAt(element_i));
	_tcmtConnectionList.insert( make_pair(connection->getElecID(),connection) );
      }
#ifdef RECO_DEBUG
//       if(genObj->getNInt()==7) {
// 	ModuleConnection connection(col->getElementAt(element_i));
// 	connection.print(std::cout);
// 	std::cout << std::endl;
//       }
//       if(genObj->getNInt()==5) {
// 	TcmtConnection connection(col->getElementAt(element_i));
// 	connection.print(std::cout);
// 	std::cout << std::endl;
//       }
#endif
    }
    rebuildConnectionTree(); 
    correctFullHalfConnectorErrors();
    determineMaximumCellNumberPerFrontEnd();
    if(debug>0) cout<<"MapAndAlign.moduleConnectionChanged(): exit"<<endl;
  }

  void MappingAndAlignment::rebuildConnectionTree() {
    if(debug>0) cout<<"MapAndAlign.rebuildConnectionTree(): enter"<<endl;
    if(debug>0) cout << "MappingAndAlignment::rebuildConnectionTree> list" 
	      << ", _moduleConnectionList.size=" << _moduleConnectionList.size()
	      << ", _tcmtConnectionList.size=" << _tcmtConnectionList.size()
	      << ", _moduleTypeList.size=" << _moduleTypeList.size()
	      << ", _moduleLocationList.size=" << _moduleLocationList.size() << std::endl;
    
    _crateList.clear();
    if ( isModuleConditionsDataComplete() ) {
      if(debug>0) cout<<"ModuleConditionsData is complete"<<endl;

      for(ModuleList_t::iterator location_iter = _moduleLocationList.begin();
	  location_iter != _moduleLocationList.end();
	  location_iter++) {
	location_iter->first=static_cast<UInt_t>(-1);
      }

      if( _tcmtConnectionList.size()>0 ) {
	rebuildTcmtConnectionTree();
	if(debug>0) cout<<"MapAndAlign.rebuildConnectionTree(): exit"<<endl;
	return;
      }

      // here for non-Tcmt connection trees
      assert(_moduleConnectionList.size()>0);

      // loop over all connected front-ends
      // create a list of crates and slots and front-ends and connect the used front-ends of all the slots
      // to modules.
      for(ModuleConnectionList_t::const_iterator module_iter = _moduleConnectionList.begin();
	  module_iter != _moduleConnectionList.end();
	  module_iter++) {

	ModuleIndex_t::const_iterator module_location_index=_moduleIndex.find(module_iter->getIndexOfLowerLeftCell());
	if (module_location_index == _moduleIndex.end() ||
	    _moduleLocationList[module_location_index->second].second.getModuleType() >= _moduleTypeList.size()) {

	  std::stringstream message;
	  message << "MappingAndAlignment::rebuildConnectionTree> No location defined for module "
		  << " id=" << module_iter->getModuleID() << " type=" << (UInt_t) module_iter->getModuleType()
		  << " connected to crate/slot/FE/connector: "
		  <<        module_iter->getCrate()
		  << "/" << module_iter->getSlot()
		  << "/" << module_iter->getFrontEnd()
		  << "/" << module_iter->getConnectorTypeName();

	  throw runtime_error(message.str());
	}

	//A map which relates the mokka cell indices to the module index
#ifdef RECO_DEBUG
	//Test
	std::cout << "Module Index: " << module_location_index->second << std::endl;
	std::cout << "ILC Index: " << std::hex << module_iter->getIndexOfLowerLeftCell() << std::dec << std::endl;
#endif
	_mapCelltoModuleIndex.insert(std::make_pair(module_iter->getIndexOfLowerLeftCell(), module_location_index->second)  );

	// Search in the list of crates if a crate with this ID is already registered 
	CrateList_t::iterator crate_iter=_crateList.begin();
	while (crate_iter!=_crateList.end()) {
	  if (crate_iter->first==(UInt_t) module_iter->getCrate()) break;
	  crate_iter++;
	}

	SlotList_t *the_slot_list;
	if (crate_iter==_crateList.end()) {
	  // if this crate was not yet registered, then register the crate
	  _crateList.push_back(make_pair(module_iter->getCrate(),SlotList_t()));
	  the_slot_list=&(_crateList.back().second);
	}
	else {
	  the_slot_list=&(crate_iter->second);
	}

	// create a slot list for this crate which is big enough, such that the slot ID can be used as an
	// array index.
	// It is assumed that the slot ID is a small number and the range from 0 to the maximum slot ID
	// is more or less used. Otherwise this would be a huge waste of space!
	if (the_slot_list->size()<=(UInt_t) module_iter->getSlot()) the_slot_list->resize(module_iter->getSlot()+1);

	UInt_t front_end_id=module_iter->getFrontEnd()*2 + (module_iter->getConnectorType()%2);
	if (module_iter->getConnectorType()>=CALICE::kNConnectorTypes) {
	  std::stringstream message;
	  message << "MappingAndAlignment::rebuildConnectionTree>"
		  << "Unknown connector type for crate=" << crate_iter->first
		  << " slot=" << module_iter->getSlot() 
		  << " front-end=" << module_iter->getFrontEnd() 
		  << " type id=" << static_cast<UInt_t>(module_iter->getConnectorType())
		  << ".";
	  throw std::runtime_error(message.str());
	}

	// Verify that the front-end or front-end side does not exceed the considered front-end range 
	if (front_end_id>=__nFrontEnds*2) {
	  std::stringstream message;
	  message << "MappingAndAlignment::rebuildConnectionTree>"
		  << "Front-end ID(" << module_iter->getFrontEnd() 
		  <<") are out of range. Only " << __nFrontEnds << "front-ends and 2 sides are handled.";
	  throw std::logic_error(message.str());
	}

	// create a list of "__nFrontEnds * 2 sides" elements to store a module ID to which the front-end 
	// is connected. If the front-end is not connected _moduleLocationList.size() is stored instead.
	while ((*the_slot_list)[module_iter->getSlot()].size()<=2*__nFrontEnds) {
	  (*the_slot_list)[module_iter->getSlot()].push_back(getNModules());
	}

	// Finally connect the specified front-end to the given module 
	(*the_slot_list)[module_iter->getSlot()][front_end_id]=module_location_index->second;

	if (module_iter->getConnectorType()==kFullConnector) {
	  (*the_slot_list)[module_iter->getSlot()][front_end_id+1]=module_location_index->second;
	}
	_moduleLocationList[module_location_index->second].first=module_iter->getModuleID();
      }
#ifdef RECO_DEBUG
	std::cout << "**************************************************" << std::endl;
#endif

      _nCells=0;
      _nConnectedCells=0;
      for (ModuleList_t::const_iterator module_iter=_moduleLocationList.begin();
	   module_iter!=_moduleLocationList.end();
	   module_iter++) {
	UInt_t module_cells=_moduleTypeList[module_iter->second.getModuleType()].getNCells();
	_nCells+=module_cells;
	if (module_iter->first!=(UInt_t) -1) {
	  _nConnectedCells+=module_cells;
	}
      }

      // DEBUG:
      if (_viewConnectionTree) print(std::cout);
    }
    if(debug>0) cout<<"MapAndAlign.rebuildConnectionTree(): exit"<<endl;
  }

  void MappingAndAlignment::rebuildTcmtConnectionTree() {
    // only called when all conditions data is complete
    if(debug>0) cout<<"MapAndAlign.rebuildTcmtConnectionTree(): enter"<<endl;
    assert( isModuleConditionsDataComplete() );
    assert( _tcmtConnectionList.size()>0 );

    // loop over all connected strips and connect each strip to an
    // electronic channel
    for(TcmtConnectionList_t::const_iterator module_iter = _tcmtConnectionList.begin();
	module_iter != _tcmtConnectionList.end();
	module_iter++) {

      TcmtConnection* connection = module_iter->second;
      ModuleIndex_t::const_iterator module_location_index=_moduleIndex.find(connection->getIndexOfLowerLeftCell());
      if (module_location_index == _moduleIndex.end() ||
 	  _moduleLocationList[module_location_index->second].second.getModuleType() >= _moduleTypeList.size()) {

	std::stringstream message;
	message << "MappingAndAlignment::rebuildTcmtConnectionTree> No location defined for module "
		<< " id=" << connection->getModuleID() << " type=" << (UInt_t) connection->getModuleType()
		<< " connected to crate/slot/FE: "
		<<        connection->getCrate()
		<< "/" << connection->getSlot()
		<< "/" << connection->getFrontEnd();

	throw runtime_error(message.str());
      }

      //A map which relates the mokka cell indices to the module index
#ifdef RECO_DEBUG
      //Test
      int tmpLayer = (connection->getIndexOfLowerLeftCell() >> 24);
      std::cout << "Location Index: " << module_location_index->second <<", layer="<< tmpLayer << std::endl;
      std::cout << "ILC Index: " << std::hex << connection->getIndexOfLowerLeftCell() << std::dec << std::endl;
#endif
      _mapCelltoModuleIndex.insert(std::make_pair(connection->getIndexOfLowerLeftCell(), module_location_index->second)  );

      // Search in the list of crates if a crate with this ID is already registered 
      CrateList_t::iterator crate_iter=_crateList.begin();
      while (crate_iter!=_crateList.end()) {
 	if (crate_iter->first==(UInt_t) connection->getCrate()) break;
 	crate_iter++;
      }

      SlotList_t *the_slot_list;
      if (crate_iter==_crateList.end()) {
 	// if this crate was not yet registered, then register the crate
 	_crateList.push_back(make_pair(connection->getCrate(),SlotList_t()));
 	the_slot_list=&(_crateList.back().second);
      }
      else {
 	the_slot_list=&(crate_iter->second);
      }

      _moduleLocationList[module_location_index->second].first = connection->getModuleID();
    }
#ifdef RECO_DEBUG
    std::cout << "**************************************************" << std::endl;
#endif

    _nCells=0;
    _nConnectedCells=0;
    for (ModuleList_t::const_iterator module_iter=_moduleLocationList.begin();
	 module_iter!=_moduleLocationList.end();
	 module_iter++) {
      UInt_t module_cells=_moduleTypeList[module_iter->second.getModuleType()].getNCells();
      _nCells+=module_cells;
      if (module_iter->first!=(UInt_t) -1) {
	_nConnectedCells+=module_cells;
      }
    }

    // DEBUG:
    if (_viewConnectionTree) printTcmtConnections(std::cout);

    if(debug>0) cout<<"MapAndAlign.rebuildTcmtConnectionTree(): exit"<<endl;
  }

  Int_t MappingAndAlignment::getModuleIndexFromCellIndex(UInt_t cellindex) {
     //first maskout the relevant Mokka indices K (layer number) and M (for the SiW Ecal, Wafer Row)
    UInt_t lookupindex = ( ((cellindex & MASK_M) -1) | (cellindex & MASK_K) );
#ifdef RECO_DEBUG
    std::cout << "Lookup Index: " << std::hex << lookupindex << std::dec << std::endl;
#endif
    MapCelltoModuleIndex_t::iterator map_iter = _mapCelltoModuleIndex.find(lookupindex);
    //if we have found the module_index. fine - Return it  
    if ( map_iter != _mapCelltoModuleIndex.end()) return map_iter->second;

    //if not we might be still one or more rows above
    //the one containing the index of the lower left cell, so keep on reducing the
    //row index
    UInt_t isearch(0);
    UInt_t testindex(0); 
    //Loop until we find an index;
    while ( map_iter == _mapCelltoModuleIndex.end() ) {
      isearch++;
      Int_t m_index = static_cast<Int_t>(( (cellindex & MASK_M) >> SHIFT_M) - isearch - 1); 
      if(m_index < 0) {
        stringstream message;
	message << "MappingAndAlignment::getModuleIndexFromCellIndex:Index of lower left cell not found"; 
        message << "for mokka index " << cellindex << std::endl; 
	throw std::runtime_error(message.str());
      }
      testindex = (static_cast<UInt_t>(m_index) << SHIFT_M) | (cellindex & MASK_K) ;
#ifdef RECO_DEBUG
    std::cout << "Test Index: " << std::hex << testindex << std::dec << std::endl;
#endif
      map_iter = _mapCelltoModuleIndex.find(testindex);
      
    }
#ifdef RECO_DEBUG
    std::cout << "Before second return: " << std::endl;
#endif
    return map_iter->second;
}


  void MappingAndAlignment::print(std::ostream &os) {
    os << "Module types:" << std::endl;
    for (UInt_t module_i=0; module_i<_moduleTypeList.size(); module_i++) {
      if (_moduleTypeList[module_i].getModuleType() != UCHAR_MAX) { 
        if (!_moduleTypeList[module_i].isValid()) continue;
        os << module_i << ":"
	   << _moduleTypeList[module_i].getModuleTypeName()
	   << " type="  << _moduleTypeList[module_i].getModuleType()
	   << " width="  << _moduleTypeList[module_i].getWidth()
	   << " height="  << _moduleTypeList[module_i].getHeight()
	   << " n_cells=" << _moduleTypeList[module_i].getNCells();
	//	if (!_moduleTypeList[module_i].hasCellDimensionsPerCell()) {   
	//	os << " cell width="  << _moduleTypeList[module_i].getCellWidth()
	//	   << " cell height="  << _moduleTypeList[module_i].getCellHeight();
	//   
	os << std::endl;
        for (UInt_t cell_i=0; cell_i<_moduleTypeList[module_i].getNCells(); cell_i++) {
	  os << "\t" 
	     << std::setw(10) << _moduleTypeList[module_i].getCellXPos(cell_i) 
	     << std::setw(10) <<  _moduleTypeList[module_i].getCellYPos(cell_i)
	     << std::endl;
	}     
      }
    }
    os << std::endl; 
    os << "Module Locations:" << std::endl;
    for (UInt_t module_i=0; module_i<_moduleLocationList.size(); module_i++) {
      if (isModuleConnected(module_i)) {
	os << module_i << " id=" << _moduleLocationList[module_i].first << " ";
	_moduleLocationList[module_i].second.print(os);
      }
      else {
	os << module_i << " (not connected) ";
	_moduleLocationList[module_i].second.print(os);
      }
    }
    os << std::endl;
    os << "Connection Tree:" << std::endl;
    for (CrateList_t::const_iterator crate_iter=_crateList.begin();
	 crate_iter!=_crateList.end();
	 crate_iter++) {
      os << "Crate=" << crate_iter->first << std::endl;
      for (UInt_t slot_i=0; slot_i<crate_iter->second.size(); slot_i++) {
	if (crate_iter->second[slot_i].size()>0) {
	  os << "Slot=" << slot_i << std::endl;
	  for (UInt_t fe_i=0; fe_i<crate_iter->second[slot_i].size(); fe_i++) {
	    os << "fe=" << fe_i/2 << " side" << fe_i%2 << " " ;
	    if (crate_iter->second[slot_i][fe_i]<_moduleLocationList.size()) {
	      os << " module=" << crate_iter->second[slot_i][fe_i]
		 << " id="  << _moduleLocationList[crate_iter->second[slot_i][fe_i]].first;
	      if ( _moduleLocationList[crate_iter->second[slot_i][fe_i]].second.getModuleType()<=_moduleTypeList.size()) {
		os << " type=" << (int) _moduleLocationList[crate_iter->second[slot_i][fe_i]].second.getModuleType();
	      }
	      else {
		os << "** module type does not exist ** ";
	      }
	      os << std::endl;
	    }
	    else {
	      os << "Not connected." << std::endl;
	    }
	  }
	}
      }
    }
  }

  void MappingAndAlignment::determineMaximumCellNumberPerFrontEnd() 
  {
    if ( isModuleConditionsDataComplete() ) {
      _maxNCells=0;
      for (CrateList_t::const_iterator crate_iter=_crateList.begin();
	   crate_iter!=_crateList.end();
	   crate_iter++) {
	for (UInt_t slot_i=0; slot_i<crate_iter->second.size(); slot_i++) {
	  if (crate_iter->second[slot_i].size()>0) {

	    for (UInt_t fe_i=0; fe_i+1<crate_iter->second[slot_i].size(); fe_i+=2) {
	      UInt_t n_cells[2]={0,0};
	      UInt_t last_module_index=_moduleLocationList.size();
	      for (UInt_t side_i=0; side_i<2; side_i++) {
		UInt_t eff_fe_i=fe_i+side_i;
		if (crate_iter->second[slot_i][eff_fe_i]<_moduleLocationList.size()) {
		  UInt_t module_index=crate_iter->second[slot_i][eff_fe_i];
		  UInt_t module_type_i=_moduleLocationList[crate_iter->second[slot_i][eff_fe_i]].second.getModuleType();
		  if ( module_type_i < _moduleTypeList.size()) {
		    if (side_i==0 || module_index != last_module_index) {
		      n_cells[side_i]=_moduleTypeList[ module_type_i ].getNCells();
		    }
		  }
		  last_module_index=module_index;
		}
	      }
	      if (n_cells[1]>0) {
		// if there is a  left side
		// then the maximum number of cells is 
		// defined large enough such that half of the buffer which is set to the maximum cell size 
		// will be large enough to fit the cells of the right and the left side.
		if (n_cells[0]*2>_maxNCells) {
		  _maxNCells=n_cells[0]*2;
		}
		if (n_cells[1]*2>_maxNCells) {
		  _maxNCells=n_cells[0]*2;
		}
	      }
	      else {
		// if there is only the right side
		if (n_cells[0]>_maxNCells) {
		  _maxNCells=n_cells[0];
		}
	      }
	    }
	  }
	}
      }
    }
  }


  void MappingAndAlignment::correctFullHalfConnectorErrors() 
  {
    if (_moduleTypeList.size()==0) {
      // No modules type have been defined yet cannot guess the required connector type (full/half).
      return;
    }
    //    bool changed_something=false;
    for (CrateList_t::iterator crate_iter=_crateList.begin();
	 crate_iter!=_crateList.end();
	 crate_iter++) {
      for (UInt_t slot_i=0; slot_i<crate_iter->second.size(); slot_i++) {
	if (crate_iter->second[slot_i].size()>0) {
	  for (UInt_t fe_i=0; fe_i<crate_iter->second[slot_i].size(); fe_i+=2) {
	    if (isModule(crate_iter->second[slot_i][fe_i])) {
	      if (_moduleLocationList[crate_iter->second[slot_i][fe_i]].second.getModuleType()>=_moduleTypeList.size()) {
		std::cerr << "Unknown module type connected to crate " << crate_iter->first
			  << " slot=" << slot_i << " fe=" << fe_i << std::endl;
		continue;
	      }
	      const ModuleDescription &module_type=_moduleTypeList[_moduleLocationList[crate_iter->second[slot_i][fe_i]].second.getModuleType()];

	      bool module_probably_needs_full_connector=( ( module_type.getNCells() >108) ? true : false); 
	      if (isModule(crate_iter->second[slot_i][fe_i+1])) {
		if (module_probably_needs_full_connector && crate_iter->second[slot_i][fe_i+1] != crate_iter->second[slot_i][fe_i]) {
		  std::cerr << "ERROR::Module type \"" << module_type.getModuleTypeName() << "\" (" << module_type.getNCells()
			    << " cells), which is connected to crate " << crate_iter->first
			    << " slot=" << slot_i << " fe=" << fe_i/2
			    << ", probably has a full connector), But, a second module is connected to the right side of the front-end. " << std::endl;
		  // FIXME: throw exception?
		}

	      }
	      else {
		if (module_probably_needs_full_connector) {
		  std::cerr << "WARNING::Module type \"" << module_type.getModuleTypeName() << "\" (" << module_type.getNCells()
			    << " cells) is connected to the left side of crate " << crate_iter->first
			    << " slot=" << slot_i << " fe=" << fe_i/2
			    << ", but probably it has a full connector. ** Will assume that a full connector was meant!" << std::endl;
		  crate_iter->second[slot_i][fe_i+1]=crate_iter->second[slot_i][fe_i];
		  //		  changed_something=true;
		}
	      }
	    }
	  }
	}
      }
    }
    //    if (changed_something) {
    //	print(std::cout);
    //    }

  }

  void MappingAndAlignment::clearTcmtConnections() {
    if(_tcmtConnectionList.size()>0) {
      for(TcmtConnectionList_t::iterator iter = _tcmtConnectionList.begin(); iter!=_tcmtConnectionList.end(); ++iter) {
	if(iter->second) delete iter->second;
      }
    }
    _tcmtConnectionList.clear();
  }

  const TcmtConnection* MappingAndAlignment::getTcmtConnection(UInt_t crate, UInt_t slot, UInt_t fe, UInt_t chip, UInt_t channel) const {
    TcmtConnection tmp;
    tmp.setSlot(slot)
      .setFrontEnd(fe)
      .setChip(chip)
      .setChannel(channel);

    TcmtConnectionList_t::const_iterator iter = _tcmtConnectionList.find( tmp.getElecID() );
    if(iter!=_tcmtConnectionList.end()) {
      if((UInt_t)iter->second->getCrate() == crate) return iter->second;
      else {
	std::stringstream message("MappingAndAlignment::getTcmtConnection> multiple Tcmt crates: ");
	message << iter->second->getCrate() <<" and "<< crate <<".";
	throw std::runtime_error( message.str() );
      }
    }
    else return NULL;
  }

  void MappingAndAlignment::printTcmtConnections(std::ostream &os) {
    os<<"\n***** TCMT Connection tree: "<< _tcmtConnectionList.size() << " strips." << endl;
    os<<" crate/slot/fe/chip/chan  (elecID) -> (fastID) layer/cass/strip/type  cellIDoffset"
      << endl;
    for(TcmtConnectionList_t::const_iterator iconn=_tcmtConnectionList.begin(); iconn!=_tcmtConnectionList.end(); ++iconn) {
      os<< *(iconn->second) << endl;
    }
  }

  bool MappingAndAlignment::isValidCrate(UInt_t crate) const {
    // Search for this crate in the list of crates
    for( CrateList_t::const_iterator icrate=_crateList.begin(); icrate!=_crateList.end(); ++icrate) {
      if( icrate->first == crate )  return true;
    }
    return false;
  }
}
#endif
