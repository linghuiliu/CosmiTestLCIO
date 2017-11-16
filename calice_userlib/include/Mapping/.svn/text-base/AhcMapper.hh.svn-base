#ifndef AhcMapper_hh
#define AhcMapper_hh

#include <vector>
#include <map>
#include <sstream>
#include <limits>

#include "lcio.h"
#include "EVENT/LCCollection.h"

#include "Mapper.hh"
#include "CaliceException.hh"

namespace CALICE {

  /**
   * @defgroup Range Functions to get dimensions of detector geometry
   */

  /**
   * AHCAL implementation of Mapper class
   *
   * @par invalid cells and exception handling
   * Most functions throw a CALICE::BadDataException if an invalid combination or ID is queried.
   * Not all invalid combinations will be detected by all commands, depending on the
   * necessary coordinate transformations in the command. E.g.: When querying the module
   * from a Mokka cell ID, only K is extracted from the ID. If no module exists for this
   * K an exception is thrown. But there will be no exception thrown if K is valid but
   * I and J belong to a channel which does not exist in this module.
   *
   * @par
   * The protected getIndexFunctions do <b>not</b> throw exceptions to allow a fast
   * lookup of the index. Invalid channels are signalled with return value maxIndex+1.
   *
   * @par
   * Some functions have a second version that does a full check if the ID is valid
   * without throwing an exception.
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.3
   * @date January 2010
   */
  class AhcMapper : public Mapper {

  public:

    AhcMapper() {

      clear();

    }


    /**
     * generate (Mokka) cellID from module, chip, channel
     *
     * Translates module, chip, channel to (Mokka) CellID using the
     * current encoding string.
     *
     * @warning only I,J,K will be set, possible additional variables will be 0
     *
     * @param[in] module module number
     * @param[in] chip chip number
     * @param[in] channel channel number
     *
     * @returns (Mokka) cell ID
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    int getCellIDFromModChipChan(const unsigned int module, const unsigned int chip, const unsigned int channel) const {
      return getDecoder()->getCellID(getI(module,chip,channel), getJ(module,chip,channel), getK(module));
    }



    /**
     * module number from (Mokka) CellID
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns module number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getModuleFromCellID(const int cellID) const {
      return getModule(getDecoder()->getKFromCellID(cellID));
    }
    /**
     * module number from DAQ channel ID
     *
     * @param[in] DAQchannel DAQ channel ID
     *
     * @returns module number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup DAQgroup
     */
    unsigned int getModuleFromDAQID(const int DAQchannel) const {
      return getModule(getDecoder()->getCrateFromDAQID(DAQchannel),getDecoder()->getSlotFromDAQID(DAQchannel),getDecoder()->getFeFromDAQID(DAQchannel));
    }
    /**
     * module number from triple crate, slot, fe
     *
     * @param[in] crate crate number (should be 0xac)
     * @param[in] slot slot number
     * @param[in] fe fe number
     *
     * @returns module number
     *
     * @throws BadDataException if lookup fails
     */
    unsigned int getModule(const unsigned int crate, const unsigned int slot, const unsigned int fe) const {

      unsigned int index = getIndexModuleConnection(crate,slot,fe);

      if (! valid(index, _connectionAvailable) ) throw BadDataException("invalid DAQ connection value (crate,slot,fe)");

      return _moduleVconnection.at(index);
    }
    /**
     * module number from K
     *
     * @param[in] k K
     *
     * @throws BadDataException if lookup fails
     *
     * @returns module number
     */
    unsigned int getModule(const unsigned int k) const {
      if (! valid(k,_kAvailable) ) {
        std::ostringstream errorMessage;
        errorMessage << "invalid K value " << k << " using encoding string: " << getDecoder()->getCellIDEncoding();
        throw BadDataException(errorMessage.str());
      }
      return _moduleVk.at(k);
    }


    /**
     * get module (number) in which this cell is included from (Mokka) cellID
     *
     * Checks if the cell ID exists. No exception is thrown.
     *
     * @param[in] cellID (Mokka) cell ID
     * @param[out] valid false if a non valid cell is queried
     *
     * @returns module
     *
     * @ingroup CellIDgroup
     */
    unsigned int getModuleFromCellID(const int cellID, bool &valid) const {
      return getModuleFromIndex(getIndexByCellID(cellID),valid);
    }

    /**
     * get module (number) in which this (DAQ) channel is included
     *
     * Checks if the channel exists. No exception is thrown.
     *
     * @param[in] DAQchannel DAQ channel ID
     * @param[out] valid false if a non valid cell is queried
     *
     * @returns module
     *
     * @ingroup DAQgroup
     */
    unsigned int getModuleFromDAQID(const int DAQchannel, bool &valid) const {
      return getModuleFromIndex(getIndexByDAQID(DAQchannel),valid);
    }

    /**
     * get module (number) from this (module) channel ID
     *
     * Checks if the channel exists. No exception is thrown.
     *
     * @param[in] moduleID module channel ID
     * @param[out] valid false if a non valid cell is queried
     *
     * @returns module
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getModuleFromModuleID(const int moduleID, bool &valid) const {
      return getModuleFromIndex(getIndexByModuleID(moduleID),valid);
    }


    /**
     * chip number from (Mokka) CellID
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns chip number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getChipFromCellID(const int cellID) const {
      return getChip(getDecoder()->getIFromCellID(cellID),getDecoder()->getJFromCellID(cellID),getDecoder()->getKFromCellID(cellID));
    }
    /**
     * chip number from triple I, J, K
     *
     * @param[in] i I
     * @param[in] j J
     * @param[in] k K
     *
     * @throws BadDataException if lookup fails
     *
     * @returns chip number
     */
    unsigned int getChip(const unsigned int i, const unsigned int j, const unsigned int k) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(getModule(k));
      unsigned int index = getIndexIJ(i,j);

      if ( ! valid(index,_ijAvailable.at(moduleTypeIndex)) ) throw BadDataException("invalid i,j");

      return getChipFromChanIndex( _chanVij.at(moduleTypeIndex).at(index) );
    }


    /**
     * channel number from (Mokka) CellID
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns channel number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getChanFromCellID(const int cellID) const {
      return getChan(getDecoder()->getIFromCellID(cellID),getDecoder()->getJFromCellID(cellID),getDecoder()->getKFromCellID(cellID));
    }
    /**
     * chip number from triple I, J, K
     *
     * @param[in] i I
     * @param[in] j J
     * @param[in] k K
     *
     * @throws BadDataException if invalid channel
     *
     * @returns channel number
     */
    unsigned int getChan(const unsigned int i, const unsigned int j, const unsigned int k) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(getModule(k));
      unsigned int index = getIndexIJ(i,j);

      if ( ! valid(index,_ijAvailable.at(moduleTypeIndex)) ) throw BadDataException("invalid i,j");

      return getChanFromChanIndex( _chanVij.at(moduleTypeIndex).at(index) );
    }


    /**
     * crate number from (Mokka) CellID
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns crate number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getCrateFromCellID(const int cellID) const {
      return getCrate(getModuleFromCellID(cellID));
    }
    /**
     * crate number from Module cell ID
     *
     * @param[in] moduleID module cell ID
     *
     * @returns crate number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getCrateFromModuleID(const int moduleID) const {
      return getCrate(getDecoder()->getModuleFromModuleID(moduleID));
    }
    /**
     * crate number from module number
     *
     * @param[in] module module number
     *
     * @throws BadDataException if lookup fails
     *
     * @returns crate number
     */
    unsigned int getCrate(const unsigned int module) const {
      if (! valid(module,_moduleAvailable) ) throw BadDataException("invalid module");
      return getCrateFromModuleConnectionIndex(_connectionVmodule.at(module));
    }


    /**
     * slot number from (Mokka) CellID
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns slot number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getSlotFromCellID(const int cellID) const {
      return getSlot(getModuleFromCellID(cellID));
    }
    /**
     * slot number from Module cell ID
     *
     * @param[in] moduleID module cell ID
     *
     * @returns slot number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getSlotFromModuleID(const int moduleID) const {
      return getSlot(getDecoder()->getModuleFromModuleID(moduleID));
    }
    /**
     * slot number from module number
     *
     * @param[in] module module number
     *
     * @throws BadDataException if lookup fails
     *
     * @returns slot number
     */
    unsigned int getSlot(const unsigned int module) const {
      if ( ! valid(module,_moduleAvailable) ) throw BadDataException("invalid module");
      return getSlotFromModuleConnectionIndex(_connectionVmodule.at(module));
    }



    /**
     * front end number from (Mokka) CellID
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns front end number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getFeFromCellID(const int cellID) const {
      return getFe(getModuleFromCellID(cellID));
    }
    /**
     * front end number from Module cell ID
     *
     * @param[in] moduleID module cell ID
     *
     * @returns front end number
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getFeFromModuleID(const int moduleID) const {
      return getFe(getDecoder()->getModuleFromModuleID(moduleID));
    }
    /**
     * front end number from module number
     *
     * @param[in] module module number
     *
     * @throws BadDataException if lookup fails
     *
     * @returns front end number
     */
    unsigned int getFe(const unsigned int module) const {
      if ( ! valid(module,_moduleAvailable) ) throw BadDataException("invalid module");
      return getFeFromModuleConnectionIndex(_connectionVmodule.at(module));
    }


    /**
     * I of (Mokka) cellID
     *
     * @param[in] moduleID  module cell ID
     *
     * @returns I
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getIFromModuleID(const int moduleID) const {
      return getI(getDecoder()->getModuleFromModuleID(moduleID),getDecoder()->getChipFromModuleID(moduleID),getDecoder()->getChannelFromModuleID(moduleID));
    }
    /**
     * I of (Mokka) cellID
     *
     * @param[in] DAQchannel  DAQ channel ID
     *
     * @returns I
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup DAQgroup
     */
    unsigned int getIFromDAQID(const int DAQchannel) const {
      return getI(getModuleFromDAQID(DAQchannel),getDecoder()->getChipFromDAQID(DAQchannel),getDecoder()->getChannelFromDAQID(DAQchannel));
    }
    /**
     * I of (Mokka) cellID
     *
     * @param[in] module module number
     * @param[in] chip chip number
     * @param[in] chan channel number
     *
     * @throws BadDataException if lookup fails
     *
     * @returns I
     */
    unsigned int getI(const unsigned int module, const unsigned int chip, const unsigned int chan) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(module);
      unsigned int index = getIndexChan(chip,chan);

      if ( ! valid(index,_chipChanAvailable.at(moduleTypeIndex)) ) throw BadDataException("invalid chip, channel");

      return getIFromIJIndex( _ijVchan.at(moduleTypeIndex).at(index) );
    }

    /**
     * J of (Mokka) cellID
     *
     * @param[in] moduleID  module cell ID
     *
     * @returns J
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getJFromModuleID(const int moduleID) const {
      return getJ(getDecoder()->getModuleFromModuleID(moduleID),getDecoder()->getChipFromModuleID(moduleID),getDecoder()->getChannelFromModuleID(moduleID));
    }
    /**
     * J of (Mokka) cellID
     *
     * @param[in] DAQchannel DAQ channel ID
     *
     * @returns J
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup DAQgroup
     */
    unsigned int getJFromDAQID(const int DAQchannel) const {
      return getJ(getModuleFromDAQID(DAQchannel),getDecoder()->getChipFromDAQID(DAQchannel),getDecoder()->getChannelFromDAQID(DAQchannel));
    }
    /**
     * J of (Mokka) cellID
     *
     * @param[in] module module number
     * @param[in] chip chip number
     * @param[in] chan channel number
     *
     * @throws BadDataException if lookup fails
     *
     * @returns J
     */
    unsigned int getJ(const unsigned int module, const unsigned chip, const unsigned chan) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(module);
      unsigned int index = getIndexChan(chip,chan);

      if ( ! valid(index,_chipChanAvailable.at(moduleTypeIndex)) ) throw BadDataException("invalid chip, channel");

      return getJFromIJIndex( _ijVchan.at(moduleTypeIndex).at(index) );
    }

    /**
     * K of (Mokka) cellID
     *
     * @param[in] moduleID  module cell ID
     *
     * @returns K
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getKFromModuleID(const int moduleID) const {
      return getK(getDecoder()->getModuleFromModuleID(moduleID));
    }
    /**
     * K of (Mokka) cellID
     *
     * @param[in] DAQchannel  DAQ channel ID
     *
     * @returns K
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup DAQgroup
     */
    unsigned int getKFromDAQID(const int DAQchannel) const {
      return getK(getModuleFromDAQID(DAQchannel));
    }
    /**
     * K of (Mokka) cellID
     *
     * @param[in] module module number
     *
     * @returns K
     *
     * @throws BadDataException if lookup fails
     */
    unsigned int getK(const unsigned int module) const {
      if (! valid(module,_moduleAvailable) ) throw BadDataException("invalid module");
      return _kVmodule.at(module);
    }



    /**
     * get cell size in I direction of (Mokka) CellID coordinate
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns size in I
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getISizeFromCellID(const int cellID) const {
      return getISizeFromIJK(getDecoder()->getIFromCellID(cellID),getDecoder()->getJFromCellID(cellID),getDecoder()->getKFromCellID(cellID));
    }
    /**
     * get cell size in I direction of (Mokka) CellID coordinate
     *
     * @param[in] moduleID  module cell ID
     *
     * @returns size in I
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getISizeFromModuleID(const int moduleID) const {
      return getISize(getDecoder()->getModuleFromModuleID(moduleID),getDecoder()->getChipFromModuleID(moduleID),getDecoder()->getChannelFromModuleID(moduleID));
    }
    /**
     * get cell size in I direction of (Mokka) CellID coordinate
     *
     * @param[in] DAQchannel  DAQ channel ID
     *
     * @returns size in I
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup DAQgroup
     */
    unsigned int getISizeFromDAQID(const int DAQchannel) const {
      return getISize(getModuleFromDAQID(DAQchannel),getDecoder()->getChipFromDAQID(DAQchannel),getDecoder()->getChannelFromDAQID(DAQchannel));
    }
    /**
     * get cell size in I direction of (Mokka) CellID coordinate
     *
     * @param[in] i I
     * @param[in] j J
     * @param[in] k K
     *
     * @returns size in I
     *
     * @throws BadDataException if lookup fails
     */
    unsigned int getISizeFromIJK(const unsigned int i, const unsigned int j, const unsigned int k) const {
      return getISize(getModule(k),getChip(i,j,k),getChan(i,j,k));
    }
    /**
     * get cell size in I direction of (Mokka) CellID coordinate
     *
     * @param[in] module module number
     * @param[in] chip chip number
     * @param[in] chan channel number
     *
     * @returns size in I
     *
     * @throws BadDataException if lookup fails
     */
    unsigned int getISize(const unsigned int module, const unsigned int chip, const unsigned int chan) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(module);
      unsigned int index = getIndexChan(chip,chan);

      if ( ! valid(index,_chipChanAvailable.at(moduleTypeIndex)) ) throw BadDataException("invalid chip, channel");

      return _sizeIVchan.at(moduleTypeIndex).at(index);
    }


    /**
     * get cell size in J direction of (Mokka) CellID coordinate
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns size in J
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup CellIDgroup
     */
    unsigned int getJSizeFromCellID(const int cellID) const {
      return getJSizeFromIJK(getDecoder()->getIFromCellID(cellID),getDecoder()->getJFromCellID(cellID),getDecoder()->getKFromCellID(cellID));
    }
    /**
     * get cell size in J direction of (Mokka) CellID coordinate
     *
     * @param[in] moduleID  module cell ID
     *
     * @returns size in J
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup ModuleIDgroup
     */
    unsigned int getJSizeFromModuleID(const int moduleID) const {
      return getJSize(getDecoder()->getModuleFromModuleID(moduleID),getDecoder()->getChipFromModuleID(moduleID),getDecoder()->getChannelFromModuleID(moduleID));
    }
    /**
     * get cell size in J direction of (Mokka) CellID coordinate
     *
     * @param[in] DAQchannel  DAQ channel ID
     *
     * @returns size in J
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup DAQgroup
     */
    unsigned int getJSizeFromDAQID(const int DAQchannel) const {
      return getJSize(getModuleFromDAQID(DAQchannel),getDecoder()->getChipFromDAQID(DAQchannel),getDecoder()->getChannelFromDAQID(DAQchannel));
    }
    /**
     * get cell size in J direction of (Mokka) CellID coordinate
     *
     * @param[in] i I
     * @param[in] j J
     * @param[in] k K
     *
     * @returns size in J
     *
     * @throws BadDataException if lookup fails
     */
    unsigned int getJSizeFromIJK(const unsigned int i, const unsigned int j, const unsigned int k) const {
      return getJSize(getModule(k),getChip(i,j,k),getChan(i,j,k));
    }
    /**
     * get cell size in J direction of (Mokka) CellID coordinate
     *
     * @param[in] module module number
     * @param[in] chip chip number
     * @param[in] chan channel number
     *
     * @returns size in J
     *
     * @throws BadDataException if lookup fails
     */
    unsigned int getJSize(const unsigned int module, const unsigned int chip, const unsigned int chan) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(module);
      unsigned int index = getIndexChan(chip,chan);

      if ( ! valid(index,_chipChanAvailable.at(moduleTypeIndex)) ) throw BadDataException("invalid chip, channel");

      return _sizeJVchan.at(moduleTypeIndex).at(index);
    }




    /**
     * return identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * Mokka CellID which is used to uniquely identify it.
     *
     * @param [in] virtualCellIndex (Mokka) cellID of (virtual) cell
     *
     * @returns index of full cell
     *
     * @ingroup VirtualCellID
     */
    int getTrueCellID(const int virtualCellIndex) const {
      return getDecoder()->getCellID(getTrueIFromCellID(virtualCellIndex), getTrueJFromCellID(virtualCellIndex), getDecoder()->getKFromCellID(virtualCellIndex));
    }

    /**
     * return identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * Mokka CellID which is used to uniquely identify it.
     *
     * @param[in] i I of virtual cell
     * @param[in] j J of virtual cell
     * @param[in] k K of virtual cell
     *
     * @returns index of full cell
     *
     * @ingroup VirtualCellID
     */
    int getTrueCellID(const unsigned int i, const unsigned int j, const unsigned int k) const {
      return getTrueCellID(getDecoder()->getCellID(i,j,k));
    }

    /**
     * return I of identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * I of the Mokka CellID which is used to uniquely identify it.
     *
     * @param [in] virtualCellID (Mokka) cellID of (virtual) cell
     *
     * @returns I of CellID of full cell
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup VirtualCellID
     */
    unsigned int getTrueIFromCellID(const int virtualCellID) const {
      return getTrueIFromIJK(getDecoder()->getIFromCellID(virtualCellID),getDecoder()->getJFromCellID(virtualCellID),getDecoder()->getKFromCellID(virtualCellID));
    }
    /**
     * return I of identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * I of the Mokka CellID which is used to uniquely identify it.
     *
     * @param[in] i I
     * @param[in] j J
     * @param[in] k K
     *
     * @returns I of CellID of full cell
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup VirtualCellID
     */
    unsigned int getTrueIFromIJK(const unsigned int i, const unsigned int j, const unsigned int k) const {
      return getTrueI(getModule(k),i,j);
    }
    /**
     * return I of identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * I of the Mokka CellID which is used to uniquely identify it.
     *
     * @param[in] module module number
     * @param[in] i I
     * @param[in] j J
     *
     * @returns I of CellID of full cell
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup VirtualCellID
     */
    unsigned int getTrueI(const unsigned int module, const unsigned int i, const unsigned int j) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(module);
      unsigned int index = getIndexIJ(i,j);

      if ( ! ( valid(index,_ijSecondaryAvailable.at(moduleTypeIndex)) || valid(index,_ijAvailable.at(moduleTypeIndex))) ) throw BadDataException("invalid i,j");

      return getIFromIJIndex( _ijVchan.at(moduleTypeIndex).at( _chanVij.at(moduleTypeIndex).at(index) ) );
    }

    /**
     * return J of identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * I of the Mokka CellID which is used to uniquely identify it.
     *
     * @param [in] virtualCellID (Mokka) cellID of (virtual) cell
     *
     * @returns J of CellID of full cell
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup VirtualCellID
     */
    unsigned int getTrueJFromCellID(const int virtualCellID) const {
      return getTrueJFromIJK(getDecoder()->getIFromCellID(virtualCellID),getDecoder()->getJFromCellID(virtualCellID),getDecoder()->getKFromCellID(virtualCellID));
    }
    /**
     * return J of identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * I of the Mokka CellID which is used to uniquely identify it.
     *
     * @param[in] i I
     * @param[in] j J
     * @param[in] k K
     *
     * @returns J of CellID of full cell
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup VirtualCellID
     */
    unsigned int getTrueJFromIJK(const unsigned int i, const unsigned int j, const unsigned int k) const {
      return getTrueJ(getModule(k),i,j);
    }
    /**
     * return J of identifying index of cell
     *
     * Cells of different size span different areas in Mokka CellID space.
     * This functions returns for all coordinates of the cell the
     * I of the Mokka CellID which is used to uniquely identify it.
     *
     * @param[in] module module number
     * @param[in] i I
     * @param[in] j J
     *
     * @returns J of CellID of full cell
     *
     * @throws BadDataException if lookup fails
     *
     * @ingroup VirtualCellID
     */
    unsigned int getTrueJ(const unsigned int module, const unsigned int i, const unsigned int j) const {

      unsigned int moduleTypeIndex = getModuleTypeIndex(module);
      unsigned int index = getIndexIJ(i,j);

      if ( ! ( valid(index,_ijSecondaryAvailable.at(moduleTypeIndex)) || valid(index,_ijAvailable.at(moduleTypeIndex))) ) throw BadDataException("invalid i,j");

      return getJFromIJIndex( _ijVchan.at(moduleTypeIndex).at( _chanVij.at(moduleTypeIndex).at(index) ) );
    }

    /**
     * return maximum module number
     *
     * @returns largest module number
     *
     * @ingroup Range
     */
    unsigned int getMaxModule() const {
      return _nMod;
    }

    /**
     * return maximum chip number
     *
     * @returns largest chip number
     *
     * @ingroup Range
     */
    unsigned int getMaxChip() const {
      return _nChip;
    }

    /**
     * return maximum channel number
     *
     * @returns largest channel number
     *
     * @ingroup Range
     */
    unsigned int getMaxChannel() const {
      return _nChan;
    }

    /**
     * return maximum I index
     *
     * @returns largest I index
     *
     * @ingroup Range
     */
    unsigned int getMaxI() const {
      return _nI;
    }
    /**
     * return maximum J index
     *
     * @returns largest J index
     *
     * @ingroup Range
     */
    unsigned int getMaxJ() const {
      return _nJ;
    }
    /**
     * return maximum K index
     *
     * @returns largest K index
     *
     * @ingroup Range
     */
    unsigned int getMaxK() const {
      return _nK;
    }

    /**
     * fills all necessary mapping information
     *
     * Module layout types are set from moduleDescription (chip,channel to I,J,K & size)
     * Module connections are set from moduleConnection (crate, slot, fe to module)
     *
     * Old settings are cleared before new are set.
     *
     * @param[in] moduleDescriptionCol LCGenericObject collection of ModuleDescription type
     * @param[in] moduleConnectionCol LCGenericObject collection of ModuleConnection type
     */
    void fill(const lcio::LCCollection* moduleDescriptionCol, const lcio::LCCollection* moduleConnectionCol) {
      fillModuleDescription(moduleDescriptionCol);
      fillModuleConnection(moduleConnectionCol);
    }

    /**
     * updates the mapping information in the class (possible after first fill only)
     *
     * The internal mapping of modules to type and electronics connection is updated.
     *
     * @param[in] moduleConnectionCol LCGenericObject collection of ModuleConnection type
     */
    void updateConnections(const lcio::LCCollection* moduleConnectionCol) {
      fillModuleConnection(moduleConnectionCol);
    }

    /**
     * print current data contents
     *
     * A detailed printout of the current layout is sent to the given stream.
     *
     * @param[out] ostream stream for the output
     */
    void print(std::ostream &ostream) const;

    /**
     * print summary of current data contents
     *
     * A summary of the current number of module types, modules, etc... is printed.
     *
     * @param[out] ostream stream for the output
     */
    void printStats(std::ostream &ostream) const;


  protected:


    unsigned int getIndexByCellID(const int cellID) const {
      DecoderSet *decoder = getDecoder();
      // check if k exists
      unsigned int k = decoder->getKFromCellID(cellID);
      if ( ! valid(k,_kAvailable) ) return getMaxIndex()+1;

      /* check if channel (i/j) lays inside this module
       * NOTE: to speed up, we assume at this place, that everything was filled right
       *       we assume that when K is available, also a module
       *       with a valid type belongs to this K
       */
      unsigned int ijIndex = getIndexIJ(decoder->getIFromCellID(cellID), decoder->getJFromCellID(cellID));
      unsigned int module = _moduleVk[k];
      unsigned int moduleType = _typeVmodule[module];
      if ( ! valid( ijIndex, _ijAvailable[moduleType]) ) return getMaxIndex()+1;

      // everything seems to be alright; let's return the compact index
      unsigned int chanIndex = _chanVij[moduleType][ijIndex];
      return getCompactIndex(module,getChipFromChanIndex(chanIndex),getChanFromChanIndex(chanIndex));

      //     return getCompactIndex(getModuleFromCellID(cellID),_chanVij.at(getModuleTypeIndex(getModuleFromCellID(cellID))).at(getIndexIJ(getDecoder()->getIFromCellID(cellID),getDecoder()->getJFromCellID(cellID))));
    }

    unsigned int getIndexByDAQID(const int ID) const {
      DecoderSet *decoder = getDecoder();

      // check if crate exists
      unsigned int crate = decoder->getCrateFromDAQID(ID);
      if ( ! valid(crate,_crateIndex) ) return getMaxIndex()+1;

      // check if slot exists
      unsigned int slot = decoder->getSlotFromDAQID(ID);
      if ( ! valid(slot,_slotAvailable) ) return getMaxIndex()+1;

      // check if there is a module for this crate/slot/fe
      unsigned int connectionIndex = getIndexModuleConnection(crate,slot,decoder->getFeFromDAQID(ID));
      if ( ! valid(connectionIndex,_connectionAvailable) ) return getMaxIndex()+1;

      /*
       * NOTE: To speed up, we assume at this place, that everything was filled right.
       *       We assume, that when the connection is available, also a module
       *       with a valid type belongs to this connection.
       */
      unsigned int module  = _moduleVconnection[connectionIndex];
      unsigned int chip    = decoder->getChipFromDAQID(ID);
      unsigned int channel = decoder->getChannelFromDAQID(ID);

      // check if channel (chip/channel) lays inside this module
      if ( ! valid( getIndexChan(chip,channel),
                    _chipChanAvailable[_typeVmodule[module]])
           ) return getMaxIndex()+1;

      // everything seems to be alright; let's return the compact index
      return getCompactIndex(module,chip,channel);

      //      return getCompactIndex(getModule(getDecoder()->getCrateFromDAQID(ID),getDecoder()->getSlotFromDAQID(ID),getDecoder()->getFeFromDAQID(ID)),getDecoder()->getChipFromDAQID(ID),getDecoder()->getChannelFromDAQID(ID));
    }

    unsigned int getIndexByModuleID(const int ID) const {
      DecoderSet *decoder = getDecoder();

      // check if module is valid
      unsigned int module = decoder->getModuleFromModuleID(ID);
      if ( ! valid(module,_moduleAvailable) ) return getMaxIndex()+1 ;

      /* check if channel (chip/channel) lays inside this module
       * NOTE: To speed up, we assume at this place, that everything was filled right.
       *       We assume that an available module implies that it has a valid type.
       */
      unsigned int chip = decoder->getChipFromModuleID(ID);
      unsigned int channel = decoder->getChannelFromModuleID(ID);
      if ( ! valid( getIndexChan(chip, channel),
                    _chipChanAvailable[_typeVmodule[module]])
           ) return getMaxIndex()+1;


      // everything seems to be alright; let's return the compact index
      return getCompactIndex(module,chip,channel);

      //      return getCompactIndex(getDecoder()->getModuleFromModuleID(ID),getDecoder()->getChipFromModuleID(ID),getDecoder()->getChannelFromModuleID(ID));
    }

    unsigned int getMaxIndex() const {
      return _nMod * _nChip * _nChan;
    }

    int getCellIDOfIndex(unsigned int index) const {
      return getDecoder()->getCellID( getI(getModuleFromCompactIndex(index),getChipFromCompactIndex(index),getChanFromCompactIndex(index)),
                                      getJ(getModuleFromCompactIndex(index),getChipFromCompactIndex(index),getChanFromCompactIndex(index)),
                                      getK(getModuleFromCompactIndex(index)) );
    }

  private:

    typedef std::map<unsigned int,unsigned int> indexMap_t ;

    unsigned int _nChan;
    unsigned int _nChip;
    unsigned int _nFe;
    unsigned int _nSlot;
    unsigned int _maxSlot;
    unsigned int _nCrate;

    unsigned int _nMod;
    unsigned int _nModType;

    unsigned int _nI;
    unsigned int _nJ;
    unsigned int _nK;

    bool valid(const unsigned int index, const std::vector<bool> &availableVec) const {
      if ( ! (index < availableVec.size()) || ! availableVec[index] ) return false;
      return true;
    }

    bool valid(const unsigned int index, const indexMap_t &indexMap) const {
      indexMap_t::const_iterator indexIter = indexMap.find(index);
      if (indexIter == indexMap.end()) return false;
      else return true;
    }

    unsigned int getModuleTypeIndex(const unsigned int module) const {

      if ( ! valid(module,_moduleAvailable) ) throw BadDataException("module number does not exist");

      return _typeVmodule.at(module);
    }

    unsigned int getIndexModuleConnection(const unsigned int crate, const unsigned int slot, const unsigned int fe) const {
      indexMap_t::const_iterator crateIndexIter = _crateIndex.find(crate);
      if (crateIndexIter == _crateIndex.end()) throw BadDataException("crate does not exist");
      return
        crateIndexIter->second*_nSlot*_nFe +
        _slotIndex[slot]*_nFe +
        fe;
    }
    unsigned int getCrateFromModuleConnectionIndex(const unsigned int index) const {
      return _crateNumber[index/(_nSlot*_nFe)];
    }
    unsigned int getSlotFromModuleConnectionIndex(const unsigned int index) const {
      return _slotNumber[(index%(_nSlot*_nFe))/_nFe];
    }
    unsigned int getFeFromModuleConnectionIndex(const unsigned int index) const {
      return index%_nFe;
    }

    unsigned int getCompactIndex(const unsigned int module, const unsigned int chip, const unsigned int chan) const {
      return module*_nChip*_nChan + getIndexChan(chip,chan);
    }
    unsigned int getCompactIndex(const unsigned int module, const unsigned int chanIndex) const {
      return module*_nChip*_nChan + chanIndex;
    }
    unsigned int getModuleFromCompactIndex(const unsigned int index) const {
      return index/(_nChip*_nChan);
    }
    unsigned int getChipFromCompactIndex(const unsigned int index) const {
      return getChipFromChanIndex( index%(_nChip*_nChan) );
    }
    unsigned int getChanFromCompactIndex(const unsigned int index) const {
      return getChanFromChanIndex( index%(_nChip*_nChan) );
    }
    unsigned int getModuleFromIndex(const unsigned int index, bool &valid) const {
      if ( index > getMaxIndex() ) {
        valid = false;
        return std::numeric_limits<unsigned int>::max();
      }
      else {
        valid = true;
        return getModuleFromCompactIndex(index);
      }
    }


    unsigned int getIndexChan(const unsigned int chip, const unsigned int chan) const {return chip*_nChan+chan;}
    unsigned int getChipFromChanIndex(const unsigned int index) const {return index/_nChan;}
    unsigned int getChanFromChanIndex(const unsigned int index) const {return index%_nChan;}

    unsigned int getIndexIJ(const unsigned int i, const unsigned int j) const {return j*_nI+i;}
    unsigned int getIFromIJIndex(const unsigned int index) const {return index%_nI;}
    unsigned int getJFromIJIndex(const unsigned int index) const {return index/_nI;}

    //memory managment functions

    void clearCrate() {

      _crateIndex.clear();
      _crateNumber.clear();

      _nCrate = 0;
    }


    void clearSlot() {
      _slotAvailable.clear();
      _slotIndex.clear();
      _slotNumber.clear();
      _nSlot = 0;
      _maxSlot = 0;
    }

    void clearModuleType() {
      _moduleTypeName.clear();
      _moduleTypeIndex.clear();
      _moduleTypeAvailable.clear();
      _chipChanAvailable.clear();
      _ijAvailable.clear();
      _nModType = 0;
    }

    template <class T>
    void initToSize( std::vector<T> &vec, const unsigned int size, const T initValue) {
      vec.clear();
      vec.resize(size,initValue);
    }

    void initToSize( std::vector<bool> &vec, const unsigned int size ) {
      initToSize(vec,size,false);
    }

    void initToSize( std::vector<unsigned int> &dataVec, std::vector<bool> &availableVec, const unsigned int size) {
      initToSize(dataVec,size,0u);
      initToSize(availableVec,size);
    }

    void initFe(const unsigned int maxFe = 0) {
      _nFe = maxFe+1;

      initToSize(_moduleVconnection,_connectionAvailable,_nCrate*_nSlot*_nFe);
    }

    void initModule(const unsigned int maxMod = 0) {
      _nMod = maxMod+1;

      initToSize(_typeVmodule,_moduleAvailable,_nMod);

      initToSize(_connectionVmodule,_connectionVmoduleAvailable,_nMod);

      initToSize(_kVmodule,_kVmoduleAvailable,_nMod);
    }

    void initK(const unsigned int maxK = 0) {
      _nK = maxK+1;

      initToSize(_moduleVk,_kAvailable,_nK);
    }

    void initIJChipChannel( const unsigned int maxI =0, const unsigned int maxJ=0, const unsigned int maxChip =0, const unsigned int maxChan =0  ) {

      _nI = maxI+1;
      _nJ = maxJ+1;
      _nChip = maxChip+1;
      _nChan = maxChan+1;


      _ijVchan.clear();
      _ijVchan.resize(_nModType);

      _chanVij.clear();
      _chanVij.resize(_nModType);

      _sizeIVchan.clear();
      _sizeIVchan.resize(_nModType);

      _sizeJVchan.clear();
      _sizeJVchan.resize(_nModType);

      for (unsigned int modType=0;modType<_nModType;++modType) {

        initToSize( _chanVij.at(modType), _ijAvailable.at(modType), _nI*_nJ);
        initToSize( _ijSecondaryAvailable.at(modType), _nI*_nJ);

        initToSize( _ijVchan.at(modType), _chipChanAvailable.at(modType), _nChip*_nChan);

        initToSize( _sizeIVchan.at(modType),_nChip*_nChan,(unsigned int)0);
        initToSize( _sizeJVchan.at(modType),_nChip*_nChan,(unsigned int)0);

      }
    }


    void clear() {

      clearCrate();

      clearSlot();

      clearModuleType();

      initModule();

      initFe();

      initK();

      initIJChipChannel();

    }

    void registerCrate(const unsigned int crate) {
      if (! crateAvailable(crate) ) {
        _crateIndex[crate] = _crateNumber.size();
        _crateNumber.push_back(crate);
        _nCrate++;
      }
    }

    bool crateAvailable(const unsigned int crate) const {
      indexMap_t::const_iterator element = _crateIndex.find(crate);
      if (element == _crateIndex.end()) return false;
      else return true;
    }

    void registerSlot(const unsigned int slot) {
      if (! slotAvailable(slot) ) {
        if ( slot >= _slotAvailable.size() ) {
          _maxSlot = slot;
          _slotAvailable.resize(_maxSlot+1,false);
          _slotIndex.resize(_maxSlot+1,0);
        }
        _slotIndex[slot] = _slotNumber.size();
        _slotNumber.push_back(slot);
        _slotAvailable[slot] = true;
        _nSlot = _slotNumber.size();
      }
    }

    bool slotAvailable(const unsigned int slot) const {
      if ( slot >= _slotAvailable.size() ) return false;
      else return _slotAvailable[slot];
    }

    void registerModuleType(const unsigned int moduleTypeName) {

      //      std::cout << "registering module type name: " << moduleTypeName << std::endl;

      if (! moduleTypeAvailable(moduleTypeName) ) {
        if (moduleTypeName >= _moduleTypeAvailable.size() ) {
          _moduleTypeAvailable.resize(moduleTypeName+1,false);
          _moduleTypeIndex.resize(moduleTypeName+1,0);
        }
        _moduleTypeIndex[moduleTypeName] = _moduleTypeName.size();
        _moduleTypeName.push_back(moduleTypeName);
        _moduleTypeAvailable[moduleTypeName] = true;

        std::vector<bool> ijVec;
        std::vector<bool> ijVecSec;
        std::vector<bool> chipChanVec;

        _ijAvailable.push_back(ijVec);
        _ijSecondaryAvailable.push_back(ijVecSec);
        _chipChanAvailable.push_back(chipChanVec);

        _nModType=_moduleTypeName.size();

      }
    }

    bool moduleTypeAvailable(const unsigned int moduleTypeName) const {
      if (moduleTypeName >= _moduleTypeAvailable.size()) return false;
      else return _moduleTypeAvailable[moduleTypeName];
    }

    unsigned int countAvailable(const std::vector<bool> &vec) const ;


    // set functions
    void setModuleCrateSlotFe(const unsigned int module, const unsigned int crate, const unsigned int slot, const unsigned int fe) {

      if ( ! _connectionVmoduleAvailable.at(module)  && ! _connectionAvailable.at(getIndexModuleConnection(crate, slot, fe)) ) {
        _moduleVconnection.at(getIndexModuleConnection(crate, slot, fe)) = module;
        _connectionVmodule.at(module) = getIndexModuleConnection(crate, slot, fe);
        _connectionVmoduleAvailable.at(module) = true;
        _connectionAvailable.at(getIndexModuleConnection(crate, slot, fe)) = true;
      }
      else if (    _connectionVmoduleAvailable.at(module) && _connectionAvailable.at(getIndexModuleConnection(crate, slot, fe))
                   && ( _moduleVconnection.at(getIndexModuleConnection(crate, slot, fe)) == module )
                   && ( _connectionVmodule.at(module) == getIndexModuleConnection(crate, slot, fe) ) ) {
        std::cout  << "WARNING: double setting of module " << module << " connected to crate " << crate << " slot " << slot << " FE " << fe << std::endl;
      }
      else {
        std::cout << "ERROR: setting of module " << module << " connected to crate " << crate << " slot " << slot << " FE " << fe << std::endl;
        std::cout << "       is in conflict with: " << std::endl;
        if ( _connectionVmoduleAvailable.at(module) ) {
          unsigned int connectionIndex = _connectionVmodule.at(module);
          std::cout  << "setting of module " << module << " connected to crate " << getCrateFromModuleConnectionIndex(connectionIndex) << " slot " << getSlotFromModuleConnectionIndex(connectionIndex) << " FE " << getFeFromModuleConnectionIndex(connectionIndex) << std::endl;
        }
        if ( _connectionAvailable.at(getIndexModuleConnection(crate,slot, fe)) ) {
          unsigned int existingModule = _moduleVconnection.at(getIndexModuleConnection(crate,slot, fe));
          std::cout  << "setting of module " << existingModule << " connected to crate " << crate << " slot " << slot << " FE " << fe << std::endl;
        }
      }
    }

    void setModuleType(const unsigned int module, const unsigned int typeName) {
      if ( ! _moduleAvailable.at(module) ) {
        _typeVmodule.at(module) = _moduleTypeIndex.at(typeName);
        _moduleAvailable.at(module) = true;
      }
      else if ( _moduleAvailable.at(module) && ( _typeVmodule.at(module) == _moduleTypeIndex.at(typeName) ) ) {
        std::cout  << "WARNING: double setting of module type name " << typeName << " for " << module << std::endl;
      }
      else {
        std::cout  << "ERROR: setting of module type name " << typeName << " for " << module << std::endl
                   << "       conflicts with previous setting of type name" << _moduleTypeName.at(_typeVmodule.at(module)) << std::endl;
      }

    }


    void setModuleK(const unsigned int module, const unsigned int K) {
      if ( ! _kVmoduleAvailable.at(module) && ! _kAvailable.at(K) ) {

        _kVmodule.at(module) = K;
        _kVmoduleAvailable.at(module) = true;

        _moduleVk.at(K) = module;
        _kAvailable.at(K) = true;

      }
      else if (    _kVmoduleAvailable.at(module) && _kAvailable.at(K)
                   && _kVmodule.at(module) == K     && _moduleVk.at(K) == module ) {
        std::cout  << "WARNING: double setting of module " << module << " and K " << K << std::endl;
      }
      else {
        std::cout  << "ERROR: setting of module " << module << " and K " << K << std::endl
                   << "       conflicts with: " << std::endl;
        if ( _kAvailable.at(K) ) std::cout << "           module " << _moduleVk.at(K) << " K " << K <<  std::endl;
        if ( _kVmoduleAvailable.at(module) ) std::cout << "           module " << module << " K " << _kVmodule.at(module) << std::endl;

      }
    }

    void setModuleTypeIJChipChannel(const unsigned int moduleTypeName, const unsigned int I, const unsigned int J, const unsigned int sizeI, const unsigned int sizeJ, const unsigned int chip, const unsigned int channel) {
      //      std::cout << "setting moduleTyp " << moduleType << " I " << I << " J " << J << " sizeI " << sizeI << " sizeJ " << sizeJ << " chip " << chip << " chan " << channel << std::endl;

      if (! moduleTypeAvailable(moduleTypeName))
        throw BadDataException("setModuleTypeIJChipChannel: moduleTypeName is unknown");

      unsigned int moduleType = _moduleTypeIndex.at(moduleTypeName);

      if (    ! _chipChanAvailable.at(moduleType).at(getIndexChan(chip,channel))
              && ! _ijAvailable.at(moduleType).at(getIndexIJ(I,J)) ) {

        _ijVchan.at(moduleType).at(getIndexChan(chip,channel)) = getIndexIJ(I,J);
        _chipChanAvailable.at(moduleType).at(getIndexChan(chip,channel)) = true;

        _chanVij.at(moduleType).at(getIndexIJ(I,J)) = getIndexChan(chip,channel);
        _ijAvailable.at(moduleType).at(getIndexIJ(I,J)) = true;

        _sizeIVchan.at(moduleType).at(getIndexChan(chip,channel)) = sizeI;
        _sizeJVchan.at(moduleType).at(getIndexChan(chip,channel)) = sizeJ;

        for (unsigned int i=I;i<I+sizeI;++i)
          for (unsigned int j=J;j<J+sizeJ;++j)
            if (!(i==I && j==J) ) {
              if (_ijAvailable.at(moduleType).at(getIndexIJ(i,j)) || _ijSecondaryAvailable.at(moduleType).at(getIndexIJ(i,j))) {
                unsigned int chanindex = _chanVij.at(moduleType).at(getIndexIJ(i,j));

                std::cout  << "ERROR: setting of moduleTyp " << moduleTypeName << " I " << I << " J " << J << " sizeI " << sizeI << " sizeJ " << sizeJ << " chip " << chip << " chan " << channel << std::endl
                           << "       overlaps with:"
                           << " I " << getIFromIJIndex(_ijVchan.at(moduleType).at(chanindex))
                           << " J " << getJFromIJIndex(_ijVchan.at(moduleType).at(chanindex))
                           << " sizeI " << _sizeIVchan.at(moduleType).at(chanindex)
                           << " sizeJ " << _sizeIVchan.at(moduleType).at(chanindex)
                           << " chip " << getChipFromChanIndex(chanindex) << " chan " << getChanFromChanIndex(chanindex) << std::endl
                           << " at I " << i << " J " << j << std::endl;

              }

              _chanVij.at(moduleType).at(getIndexIJ(i,j)) = getIndexChan(chip,channel);
              _ijSecondaryAvailable.at(moduleType).at(getIndexIJ(i,j)) = true;
            }

      }
      else if (    _chipChanAvailable.at(moduleType).at(getIndexChan(chip,channel))
                   && _ijAvailable.at(moduleType).at(getIndexIJ(I,J))
                   && ( _ijVchan.at(moduleType).at(getIndexChan(chip,channel)) == getIndexIJ(I,J) )
                   && ( _chanVij.at(moduleType).at(getIndexIJ(I,J)) == getIndexChan(chip,channel) )
                   && ( _sizeIVchan.at(moduleType).at(getIndexChan(chip,channel)) == sizeI )
                   && ( _sizeJVchan.at(moduleType).at(getIndexChan(chip,channel)) == sizeJ ) ) {

        std::cout  << "WARNING: double setting of moduleTyp " << moduleTypeName << " I " << I << " J " << J << " sizeI " << sizeI << " sizeJ " << sizeJ << " chip " << chip << " chan " << channel << std::endl;
      }
      else {
        std::cout  << "ERROR: setting of moduleTyp " << moduleTypeName << " I " << I << " J " << J << " sizeI " << sizeI << " sizeJ " << sizeJ << " chip " << chip << " chan " << channel << std::endl
                   << "       is in conflict with " << std::endl;
        if (_chipChanAvailable.at(moduleType).at(getIndexChan(chip,channel))) {
          unsigned int ijindex = _ijVchan.at(moduleType).at(getIndexChan(chip,channel));
          std::cout << "moduleType " << moduleTypeName << " I " << getIFromIJIndex(ijindex) << " J " << getJFromIJIndex(ijindex) << " sizeI " << _sizeIVchan.at(moduleType).at(getIndexChan(chip,channel)) << " sizeJ " << _sizeJVchan.at(moduleType).at(getIndexChan(chip,channel)) << " chip " << chip << " chan " << channel << std::endl;
        }
        if (_ijAvailable.at(moduleType).at(getIndexIJ(I,J))) {
          unsigned int chanindex = _chanVij.at(moduleType).at(getIndexIJ(I,J));
          std::cout << "moduleType " << moduleTypeName << " I " << I << " J " << J  << " sizeI " << _sizeIVchan.at(moduleType).at(chanindex) << " sizeJ " << _sizeJVchan.at(moduleType).at(chanindex) << " chip " << getChipFromChanIndex(chanindex) << " chip " << getChanFromChanIndex(chanindex) << std::endl;
        }
      }
    }

    //fill functions

    /**
     * sets the mapping information in the class, call after fillModuleDescription
     *
     * The internal mapping of modules to type and electronics connection is set.
     *
     * @param[in] col LCGenericObject collection of ModuleConnection type
     */
    void fillModuleConnection(const lcio::LCCollection* col);
    /**
     * sets the module layout information in the class, module connection information will be invalidated
     *
     * The internal layout of modules, like channel to I,J,K and cell sizes are set.
     *
     * @warning  module connection information will be invalidated, fillModuleConnection has to be called aftwards
     *
     * @param[in] col LCGenericObject collection of ModuleDescription type
     */
    void fillModuleDescription(const lcio::LCCollection* col);


    // helper functions
    void getChipChannelForModuleDescriptionID(const unsigned int id, const unsigned int moduleType, int &chip, int &channel) const ;
    unsigned int getCombinedModuleType(const unsigned int moduleType) const ;
    int correctCellIndex(const unsigned int moduleType, const int cellIndex) const;
    const std::string printIfAvailable(bool available, unsigned int value) const;

    //private variables


    indexMap_t                _crateIndex;
    std::vector<unsigned int> _crateNumber;

    std::vector<unsigned int> _slotIndex;
    std::vector<unsigned int> _slotNumber;
    std::vector<bool>         _slotAvailable;

    std::vector<bool>         _connectionAvailable;

    std::vector<bool>         _moduleAvailable;

    std::vector<unsigned int> _typeVmodule;

    std::vector<unsigned int> _connectionVmodule;
    std::vector<bool>         _connectionVmoduleAvailable;
    std::vector<unsigned int> _moduleVconnection;

    std::vector<unsigned int> _kVmodule;
    std::vector<bool>         _kVmoduleAvailable;

    std::vector<unsigned int> _moduleVk;
    std::vector<bool>         _kAvailable;





    std::vector<unsigned int>               _moduleTypeName;
    std::vector<unsigned int>               _moduleTypeIndex;
    std::vector<bool        >               _moduleTypeAvailable;

    std::vector<std::vector<unsigned int> > _ijVchan;
    std::vector<std::vector<bool> >         _chipChanAvailable;

    std::vector<std::vector<unsigned int> > _chanVij;
    std::vector<std::vector<bool> >         _ijAvailable;
    std::vector<std::vector<bool> >         _ijSecondaryAvailable;

    std::vector<std::vector<unsigned int> > _sizeIVchan;
    std::vector<std::vector<unsigned int> > _sizeJVchan;



  };
}





#endif //AhcMapper_hh
