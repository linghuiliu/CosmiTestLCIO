#ifndef Mapper_hh
#define Mapper_hh

// c++ includes
#include <string>

// CALICE includes
#include "DecoderSet.hh"

namespace CALICE {

  class CellIterator;

  /** Mapping for CALICE detectors (currently: HCAL support only)
   * @author Benjamin.Lutz@desy.de
   * @version 0.4
   * @date January 2010
   */
  class Mapper {

  public:
    /**
     * constructor (default encoding strings)
     */
    Mapper();

    /**
     * constructor that takes encoding strings
     *
     * @param[in] cellIDencoding encoding string for (Mokka) cell IDs
     * @param[in] moduleEncoding encoding string for module channel ID
     * @param[in] DAQencoding encoding string for DAQ channel ID
     */
    Mapper(const std::string& cellIDencoding, const std::string& moduleEncoding, const std::string& DAQencoding );

    virtual ~Mapper() {
      if (_decoderSet) delete _decoderSet;
    }

    /**
     * get class which handles the bit en/decoding of the different IDs
     *
     * The encoding strings can be set there.
     */
    DecoderSet* getDecoder() const {
      return _decoderSet;
    }


    /**
     * get module (number) in which this cell is included from (Mokka) cellID
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns module
     *
     * @throws BadDataException if some error is detected, but no full verification is performed
     * @see getModuleFromCellID(const int cellID, bool &valid)
     *
     * @ingroup CellIDgroup
     */
    virtual unsigned int getModuleFromCellID(const int cellID) const = 0;

    /**
     * get module (number) in which this (DAQ) channel is included
     *
     * @param[in] DAQchannel DAQ channel ID
     *
     * @returns module
     *
     * @throws BadDataException if some error is detected, but no full verification is performed
     * @see getModuleFromDAQID(const int DAQchannel, bool &valid)
     *
     * @ingroup DAQgroup
     */
    virtual unsigned int getModuleFromDAQID(const int DAQchannel) const = 0;


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
    virtual unsigned int getModuleFromCellID(const int cellID, bool &valid) const = 0;

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
    virtual unsigned int getModuleFromDAQID(const int DAQchannel, bool &valid) const = 0;

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
    virtual unsigned int getModuleFromModuleID(const int moduleID, bool &valid) const = 0;


    /**
     * get cell size in I direction of (Mokka) CellID coordinate
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns size in I
     *
     * @throws BadDataException if cell does not exist
     *
     * @ingroup CellIDgroup
     */
    virtual unsigned int getISizeFromCellID(const int cellID) const = 0;

    /**
     * get cell size in J direction of (Mokka) CellID coordinate
     *
     * @param[in] cellID (Mokka) cell ID
     *
     * @returns size in J
     *
     * @throws BadDataException if cell does not exist
     *
     * @ingroup CellIDgroup
     */
    virtual unsigned int getJSizeFromCellID(const int cellID) const = 0;

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
     * @throws BadDataException if cell does not exist
     *
     * @ingroup VirtualCellID
     */
    virtual int getTrueCellID(const int virtualCellIndex) const = 0;

    /**
     * get an iterator to the first (Mokka) cell ID
     *
     * @returns iterator
     *
     * @ingroup CellIDgroup
     */
    CellIterator begin() const;

    /**
     * get an iterator one after the last (Mokka) cell ID
     *
     * @returns iterator
     *
     * @ingroup CellIDgroup
     */
    CellIterator end() const;


    /**
     * @returns a version number that increases with changes in the mapping
     */
    unsigned int getVersion() const { return _version;}


    template <class T>  friend class MappedContainer;
    friend class CellIterator;


  protected:

    /**
     * If this function is called the version will be increased
     * to indicate a change in the mapping.
     */
    void mappingModified() {_version++;}

    /**
     * get a compact index for storing data in vectors
     *
     * @param [in] cellID (Mokka) cell ID
     * @returns compact index
     * @ingroup CellIDgroup
     */
    virtual unsigned int getIndexByCellID(const int cellID) const = 0;

    /**
     * get a compact index for storing data in vectors
     *
     * @param [in] ID DAQ channel ID
     * @returns compact index
     * @ingroup DAQgroup
     */
    virtual unsigned int getIndexByDAQID(const int ID) const = 0;

    /**
     * get a compact index for storing data in vectors
     *
     * @param [in] ID Module channel ID
     * @returns compact index
     * @ingroup ModuleIDgroup
     */
    virtual unsigned int getIndexByModuleID(const int ID) const = 0;

    /**
     * get the maximum value of the compact index for storing data in vectors
     *
     * @returns maximum compact index
     */
    virtual unsigned int getMaxIndex() const = 0;

    /**
     * get the cell ID of a certain compact index
     *
     * @warning This cell ID will only contain information about I, J and K.
     *          If the encoding string contains additional fields this will have value 0.
     *
     * @see setCellIDencoding
     *
     * @returns (Mokka) cell ID
     * @ingroup CellIDgroup
     */
    virtual int getCellIDOfIndex(unsigned int index) const = 0;

  private:

    unsigned int _version;

    DecoderSet *_decoderSet;
  };





} // namespace CALICE

#endif
