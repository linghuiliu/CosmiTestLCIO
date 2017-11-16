#ifdef USE_LCCD
#ifndef MappingAndAlignment_hh
#define MappingAndAlignment_hh

#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <climits>

#include <ModuleConnection.hh>
#include <TcmtConnection.hh>
#include <Alignment.hh>
#include <SimpleArray_t.hh>
#include <CellIndex.hh>
//Come define statements as given in CellIndex Class
#define SHIFT_M 0
#define MASK_M (UInt_t) 0x00000007

#if ( __GNUC__ < 3 )
#  include <math.h>
#else
#  include <cmath>
#endif

using namespace std;

namespace CALICE {

/** Class to comunicate/handle the translation into unique layer/cell ids and 3D spacial coordinates.
 * The class has to be informed about changes of the detector configuration using the methods
 * \ref CALICE::MappingAndAlignment::moduleTypeChanged, 
 * \ref CALICE::MappingAndAlignment::moduleLocationChanged and
 * \ref CALICE::MappingAndAlignment::moduleConnectionChanged
 * This information is used to map the DAQ signals to cells and calculate 3D spacial coordinates
 * for hits.
 * 
 * The class performs range checks only if BOUNDARY_CHECK is defined.
 *
 */


class MappingAndAlignment : public Alignment
{
  friend class VRawADCValueProcessor;
  friend class ModuleIndexReverseLookup;
  friend class HcalModuleIndexReverseLookup;
public:
  /** default constructor. 
   * The default constructor configures the conditions data change delegators.
   */
  MappingAndAlignment();

  ~MappingAndAlignment() {
    clearTcmtConnections();
  }

  /**Does noting for the time beeing.
   */
  void init();

  /** Get the module indices connected to the right and left side of the given front-end.
   * @param crate_id the identifier of the crate.
   * @param slot_id the identifier of the slot of the vme crate (usually 0-22)
   * @param front_end_nr the identifier of the front-end (0-7, 8 has special meaning which is not supported).
   * @return pair containing the module indices of the modules connected to the left and the right side of the front-end.
   *
   * @throw std::runtime_error if the crate is not registered or the slot of the crate has no connected  front-ends.
   * @throw std::range_error if the BOUNDARY_CHECK is defined and the crate and slot numbers are 
   *                         valid but the front-end number is out of the considered range.
   * 
   * Each front-end has two sides. the left side can be connected with a full connector (12 lines) or both
   * sides can be connected with half connectors (6 lines). In case the left side is connected to a full
   * connector the left and the rigtht module index will be the same.
   */
   std::pair<UInt_t,UInt_t> getModuleIndex(UInt_t crate_id, UInt_t slot_id, UInt_t front_end_nr) const
    throw (runtime_error
#ifdef BOUNDARY_CHECK
	   ,range_error
#endif
	   )
  {

    for(CrateList_t::const_iterator crate_iter=_crateList.begin(); 
	crate_iter!=_crateList.end();
	crate_iter++) {
      if (crate_iter->first==crate_id) {

	//	if (slot_id>=crate_iter->second.size()) {
	//	  return make_pair<UInt_t,UInt_t>(UINT_MAX,UINT_MAX);
	//	}
	//#ifdef BOUNDARY_CHECK
	//	if (slot_id>=crate_iter->second.size()) {
	//	  throw std::range_error("MappingAndAlignment::getModuleIndex>Slot index out of range.");
	//	}
	//#endif	
	if (slot_id>=crate_iter->second.size() || crate_iter->second[slot_id].size()==0) {
	  // Slot is not configured i.e. AdcBlocks are not for me.
	  return std::make_pair<UInt_t,UInt_t>(UINT_MAX,UINT_MAX);
	}

	UInt_t front_end_index=front_end_nr*2;

#ifdef BOUNDARY_CHECK
	if (front_end_index>=(UInt_t) crate_iter->second[slot_id].size()) {
	  std::stringstream message;
	  message << "MappingAndAlignment::getModuleAndCellIndex> Front-end index " << front_end_nr
		  << " of slot " << slot_id
		  <<  " out of the allowed range (0-7).";
	      
	  throw std::range_error(message.str());
	}
#endif	

	UInt_t module_index_left=crate_iter->second[slot_id][front_end_index];
	UInt_t module_index_right=crate_iter->second[slot_id][front_end_index+1];

	return std::make_pair<UInt_t,UInt_t>( (isModule(module_index_left) ? module_index_left : UINT_MAX),
					      (isModule(module_index_right) ? module_index_right : UINT_MAX) );
      }
    }
    return std::make_pair<UInt_t,UInt_t>(UINT_MAX,UINT_MAX);
  }

  /** get the cell index for the given module according to the current multiplex position and line index.
   * @param module_index a valid index of a conencted module (@ref getModuleIndex)
   * @param multiplex_position the multiplex sequence number (0-17).
   * @param line_i the number of the input line/ chip (0-11).
   * @return the cell index for the given multiplex position and line index.
   * The cell index can be savely increment for the remaining lines 
   *  (0 < line_i < getNLinesOfLeftIndex, getNLinesOfRightIndex).
   * @todo need more robust method to determine the number of lines per modules
   */
  UInt_t getCellIndex(UInt_t module_index, UInt_t multiplex_position, UInt_t line_i) const {
    return multiplex_position*(getNCellsPerModule(module_index) > 108 ?  __nLines : __nLinesHalf )+line_i;
  }

  /** Get the module and the cell index using the information of the electrical connection.
   * The ADC values are grouped in modules. In case of the Calice ECAL a module is one PCB.
   * The pads are labeled by the cell index. The number of cells per module may not be the 
   * same (In case of the Calice ECAL there are modules composed of 6 or 3 wafers with 36 
   * pads each).
   *
   * @param adc_block reference to the current AdcBlock
   * @param line_i the line/chip of the current adc value
   * @return pair continaing first the module index and second the cell index (e.g. pad )
   *
   * @throw std::runtime_error if the crate of the current adc_block is not registered or
   * @throw std::range_error if the BOUNDARY_CHECK is defined and the crate and slot numbers are 
   *                         valid but the front-end number of the current adc_block is out of the considered range.
   *                         (<b>Attention<b>: If the slot or fron-end numbers are within the
   *                         range, but not assigend to valid modules no exception is thrown
   *                         i.e. these errors pass unoticed!)
   */
  //  pair<UInt_t, UInt_t> getModuleAndCellIndex(const AdcBlock &adc_block, UInt_t line_i) const
  //    throw (runtime_error
  //#ifdef BOUNDARY_CHECK
  //	   ,range_error
  //#endif
  //	   );
    
  //  /** Get the layer number of the specified module. 
  //   * @param group_index the first component of the index returned by @ref getIndex
  //   * @param sub_group_index the second component of the index returned by @ref getIndex
  //   * @return the layer number (0 ... Number of Calorimeter layers - 1 )
  //   * @throw range_error if the module index is out of range and BOUNDARY_CHECK is defined
  //   */
  //  UInt_t getLayerIndex(UInt_t module_index,  UInt_t cell_index) const 
  //#ifdef BOUNDARY_CHECK
  //    throw (range_error)
  //#endif
  //  {
  //#ifdef BOUNDARY_CHECK
  //    if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType()>=_moduleTypeList.size()) {
  //	std::stringstream message;
  //	message << "MappingAndAlignment::getLayerIndex> Module " << module_index << "is not defined.";
  //	throw range_error(message.str());
  //    }
  //#endif
  //    return _moduleLocationList[module_index].second.getLayerNumber();
  //  };


  /** Get the ID of module which uniquely identifies a certain module of one module type.
   * Different modules of one type always have different IDs but modules of different types may have the same ID.
   * In case of the ECAL the module ID is the serial number of the PCBs.
   * @param module_index the index of the module within the detector (not necessarily the same as the module ID)
   * @return the ID of a module.
   * @throw range_error if the module specified by module_index is not defined but only if BOUNDARY_CHECK is defined.
   * @see getModuleType()
   */
  UInt_t getModuleID(UInt_t module_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size() || _moduleLocationList[module_index].second.getModuleType() >= _moduleTypeList.size()) {
	std::stringstream message;
	message << "MappingAndAlignment::getModuleID> Module " << module_index << "is not defined.";
	throw range_error(message.str());
    }
#endif
    return _moduleLocationList[module_index].first;
  }

  /** Get the name containing the type specifier and the module ID.
   * A string is formed from th ID of the specified module and the module type name.
   * @param module_index the index of the module within the detector (not necessarily the same as the module ID)
   * @return a string which uniquely identifies a module
   * @throw range_error if the module specified by module_index is not defined but only if BOUNDARY_CHECK is defined.
   * @see ModuleDescription::getModuleTypeName(), ModuleConnection::getModuleID()
   */
  string getModuleName(UInt_t module_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error,logic_error)
#else
    throw (logic_error)
#endif
    ;
  
  /** Verify  whether the module location is connected to an active front-end.
   * @param module_index the index of the module location.
   * @return true if the module location is connected to a front-end otherwise false.
   */
  bool isModuleConnected(UInt_t module_index) const
#ifdef BOUNDARY_CHECK
    throw (range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    if (module_index>=_moduleLocationList.size()) {
	std::stringstream message;
	message << "MappingAndAlignment::isModuleConnected> Module " << module_index << "is not defined.";
	throw range_error(message.str());
    }
#endif
    return _moduleLocationList[module_index].first!=(UInt_t) -1;
  };
 
  /** Return the module index for a given (*Mokka*) Cellindex 
    * All this assumes that the ModuleLocation class has got
    * set properly the index of the lower left cell 
    * e.g. CALICE::CellIndex(0,1,0,0,layer_i+1)
    * ,    CALICE::CellIndex(a,1,0,0,layer_i+1)
    * where the first argument is the wafer row (in case of SiW Ecal).
    * @todo Extend function to handle situation in which a layer k does not exist in data but is simulated
    */
  Int_t getModuleIndexFromCellIndex(UInt_t);


  /** Return the total number of defined cells.
   * The total number of cells includes cells of all modules also modules which are not connected to the DAQ but which are defined.
   */
  UInt_t getNCellsTotal() const {return _nCells;};

  /** Return the number of cells connected to the DAQ.
   */
  UInt_t getNConnectedCells() const {return _nConnectedCells;};

  /** Verify whether the given cell is dead.
   * @param module_index the index of the module location
   * @param cell_index the index of the cell on the module.
   * @return true if the cell is dead.
   */
  Bool_t isCellDead(UInt_t module_index, UInt_t cell_index) 
    {
      // TODO: add conditions data  which contains the dead cells per module 
      return !isModuleConnected(module_index);
    };

  /**Inform the object about changes of the module types.
   * @param col collection of  @ref ModuleDescription objects
   * This class must be called before using any(most) of the other methods.
   * The function call will cause a rebuild of the module DAQ front-end connection tree.
   */
  void moduleTypeChanged(lcio::LCCollection* col);

  /**Inform the object about changes of the module locations.
   * @param col collection of  @ref ModuleLocation objects
   * This class must be called before using any(most) of the other methods.
   * The function call will cause a rebuild of the module DAQ front-end connection tree.
   */
  void moduleLocationChanged(lcio::LCCollection* col);

  /**Inform the object about changes of the module connection.
   * @param col collection of  @ref ModuleConnection objects
   * This class must be called before using any(most) of the other methods.
   * The function call will cause a rebuild of the module DAQ front-end connection tree.
   */
  void moduleConnectionChanged(lcio::LCCollection* col);

  /** To debug.
   */
  void print(std::ostream &os);
  void printTcmtConnections(std::ostream &os);

  /** Validates a crate number based on mapping.  It can be used to skip processing of
   *  unused crates as early as possible.
   *  @param crate Crate number to be validated
   */
  bool isValidCrate(UInt_t crate) const;

  /** Toggle printing of the connecting tree after each rebuild (conditions data change: ModuleConnection, ModuleLocation).
   * Set to true if the connection tree should be displayed after each call to rebuild the connection tree. The latter
   * is initiated after module location or module connection changes.
   */
  void setViewConnectionTree(bool view_connection=true) {_viewConnectionTree=view_connection;};

  /** Return true if the front-end has a right side connected.
   * @return true if the right side is connected and is not conencted to the same module as the left side.
   * If the left and the right side is connected to the same module, then it is considered that a full connector
   * was connected to the left side and the right side was not directly connected.
   */
  bool hasRightSide(const std::pair<UInt_t,UInt_t> &module_indices) const {
    return module_indices.second<UINT_MAX && module_indices.second != module_indices.first;
  };
  
  /** Return the number above the last line which should be used for the left or the right index.
   * @param module_indices the left and the right module indec.
   * @param right_instead_of_left if true the right index is used instead of the left.
   * @return the line after the last line which may be use for the specified module index.
   */
  UInt_t getNumberOfLines(const std::pair<UInt_t,UInt_t> &module_indices, bool right_instead_of_left) const {
    if (right_instead_of_left) {
      return __nLines;
    }
    return (module_indices.first == module_indices.second ? __nLines : __nLinesHalf);
  };

  /** Return the first line associated to the module connected to the left side of the front-end.
   */
  UInt_t getLineIndexOfLeftSide() const {
    return 0;
  };

  /** Return the first line associated to the module connected to the right side of the front-end.
   */
  UInt_t getLineIndexOfRightSide() const {
    return __nLinesHalf;
  };

  /** Basic completeness test of conditions data.
   */
  Bool_t isModuleConditionsDataComplete() const {
    return (( _moduleConnectionList.size()>0 || _tcmtConnectionList.size()>0 ) &&_moduleTypeList.size()>0 && _moduleLocationList.size()>0);
  }

  UInt_t getMaxCellsPerFrontEnd() const {
    return _maxNCells;
  }

  /** Get the connection object associated to an electronic channel.
   * @param crate Crate number
   * @param slot  Slot number
   * @param fe    Front end number
   * @param chip  Chip number
   * @param chan  Channel number
   */
  const TcmtConnection* getTcmtConnection(UInt_t crate, UInt_t slot, UInt_t fe, UInt_t chip, UInt_t chan) const;

  /** Builds Ecal/Hcal connection mappings.
   */
  void rebuildConnectionTree();

  /** Returns the cell id encoding for the given detector */
  const std::string getCellIDEncoding(){return _cellIDEncoding;}  

protected:
  /** Builds TCMT connection mappings.
   */
  void rebuildTcmtConnectionTree();

  /** Reset TCMT connection mappings. Automatically called before these connection mappings are (re)built. */
  void clearTcmtConnections();

private:
  /** Cludge to correct configuration errors.
   * Initially there was only support for full connectors
   */
  void correctFullHalfConnectorErrors() ;
  //  typedef SimpleArray_t<UInt_t> ModuleIndexList_t; 
  //  ModuleList_t _moduleList;

  /** Determine the maximum number of cells per front end.
   * The maxmimum number of cells per front-end is defined such that 
   * this number is equal or larger than the number of cells per front
   * end of all front ends which only have left sides and half this number is equal or large than
   * the maximum number of cells of all right sides and all left sides respectively.
   */
  void determineMaximumCellNumberPerFrontEnd();

  typedef SimpleArray_t<UInt_t> FrontEndList_t;                 /**< Array which contains the module ID of the front-end to which it is connected.
								     The frond-end ID*2+ front-end sid (0/1) is used as an array index */
  typedef SimpleArray_t<FrontEndList_t> SlotList_t;             /**< List of slots. It is assumed that the slot ID is small and 
								     can be used as an array index (without requiring very large unused arrays) */
  typedef SimpleArray_t<pair<UInt_t,SlotList_t > > CrateList_t; /**< list of crates: first argument crate ID.*/

  typedef SimpleArray_t<ModuleConnection> ModuleConnectionList_t; /**< List which stores the module connection information provided by the 
								        conditions data handler (TODO: just store a pointer to the collection?)*/

  ModuleConnectionList_t _moduleConnectionList;

  /** map of TCMT connections, keyed by elecID (slot,fe,chip,chan) */
  typedef map<int,TcmtConnection*> TcmtConnectionList_t;
  TcmtConnectionList_t _tcmtConnectionList;

  typedef map<UInt_t , UInt_t > ModuleIndex_t; /**< The first component is a uniqe module index (the index offset) , the second component the array index 
						  in _moduleLocationList. */
  ModuleIndex_t _moduleIndex;

private:
  
  CrateList_t _crateList;

private:

  UInt_t  _nCells;                  /**< total number of cells; */
  UInt_t  _nConnectedCells;         /**< total number of cells which are connected to the DAQ; */
  
  Bool_t _viewConnectionTree;

  /** A map which relates indices (M,K) of a given cell to a module */ 
  typedef std::map<UInt_t, UInt_t> MapCelltoModuleIndex_t;
  MapCelltoModuleIndex_t _mapCelltoModuleIndex;

  /** A string which constains the cell id encoding 
      */
  std::string _cellIDEncoding;

  /** some constants*/
  static const UInt_t  __nLines    ;
  static const UInt_t  __nLinesHalf;
  static const UInt_t  __nFrontEnds;
  UInt_t _maxNCells;
};
}
#endif
#endif
