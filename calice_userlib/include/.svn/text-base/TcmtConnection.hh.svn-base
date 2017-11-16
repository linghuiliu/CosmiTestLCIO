//
// File: TcmtConnection.hh
// Package: calice_userlib
//
// Purpose: defines TcmtConnection objects stored in the conditions database
//
// 070720 - G.Lima  created
//
#ifndef TcmtConnection_hh
#define TcmtConnection_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <iomanip>

// using namespace lcio ;

namespace CALICE {

/**
 * @author Guilherme Lima (Northern Illinois University)
 * @date July 2007
 */

enum ETcmtConnectionFloatType {kNTcmtConnectionFloats};
enum ETcmtConnectionIntType {
  kTcmtConnectionIntCrate,
  kTcmtConnectionIntElecID,  // 0xAABBCCDD, where AA=slot, BB=fe, CC=chip, DD=channel
  kTcmtConnectionIntFastID,  // 0x0000MMSS, where MM=moduleID and SS=StripID
  kTcmtConnectionIntIndexOfLowerLeftCell,
  kTcmtConnectionIntModuleType,
  kNTcmtConnectionInts
};

  /** Defines the electronic connection of a TCMT strip.
   * A module location defined by @ref ModuleLocation may be connected
   * to a connector of a CERC board.  The module location is
   * identified by the index offset.
   */
class TcmtConnection
  : public LCFixedObject<kNTcmtConnectionInts,kNTcmtConnectionFloats,0>
{
public:
  TcmtConnection()
    : LCFixedObject<kNTcmtConnectionInts,kNTcmtConnectionFloats,0>() { };

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  TcmtConnection(LCObject* obj)
    : LCFixedObject<kNTcmtConnectionInts,kNTcmtConnectionFloats,0>(obj) { }

  TcmtConnection& setCrate(int crate_number) {
    obj()->setIntVal(kTcmtConnectionIntCrate, crate_number);
    return *this;
  };

  int getCrate() const {
    return getIntVal(kTcmtConnectionIntCrate);
  };

  TcmtConnection& setElecID(int elecID) {
    obj()->setIntVal(kTcmtConnectionIntElecID, elecID);
    return *this;
  };
  int getElecID() const {
    return getIntVal(kTcmtConnectionIntElecID);
  };

  TcmtConnection& setFastID(int fastID) {
    obj()->setIntVal(kTcmtConnectionIntFastID, fastID);
    return *this;
  };
  int getFastID() const {
    return getIntVal(kTcmtConnectionIntFastID);
  };

  TcmtConnection& setSlot(int slot) {
    assert( slot>=0 && slot<256);
    unsigned int newElecID = (this->getElecID() & 0x00FFFFFF) | (slot << 24);
    this->setElecID( newElecID );
    return *this;
  };
  int getSlot() const {
    return (this->getElecID() >> 24) & 0xff;
  };

  TcmtConnection& setFrontEnd(int fe) {
    assert( fe>=0 && fe<256);
    unsigned int newElecID = (this->getElecID() & 0xFF00FFFF) | (fe << 16);
    this->setElecID( newElecID );
    return *this;
  };
  int getFrontEnd() const {
    return (this->getElecID() >> 16) & 0xff;
  };

  TcmtConnection& setChip(int chip) {
    assert( chip>=0 && chip<256);
    unsigned int newElecID = (this->getElecID() & 0xFFFF00FF) | (chip << 8);
    this->setElecID( newElecID );
    return *this;
  };
  int getChip() const {
    return (this->getElecID() >> 8) & 0xff;
  };

  TcmtConnection& setChannel(int channel) {
    assert( channel>=0 && channel<256);
    unsigned int newElecID = (this->getElecID() & 0xFFFFFF00) | channel;
    this->setElecID( newElecID );
    return *this;
  };
  int getChannel() const {
    return this->getElecID() & 0xff;
  };

  /** Set the cell index (Mokka specification) of the cell in the lower left corner.
   * The index offset is used to uniquely define the module location.
   */
  TcmtConnection &  setIndexOfLowerLeftCell(unsigned int index_offset)
  {
    obj()->setIntVal(kTcmtConnectionIntIndexOfLowerLeftCell, index_offset);
    return *this;
  };

  /** Get the cell index (Mokka specification) of the cell in the lower left corner.
   * This index uniquely defines the module location.
   */
  int getIndexOfLowerLeftCell() const
  {
#ifdef RECO_DEBUG
    std::cout << "Index: " << getIntVal(kTcmtConnectionIntIndexOfLowerLeftCell) << std::endl;
#endif
    return getIntVal(kTcmtConnectionIntIndexOfLowerLeftCell);
  };


  /** Set the module type.
   * The module type is not needed but this redundant information allows
   * to verify the configuration correctness.
   * @todo remove this redundant information?
   */
  TcmtConnection&  setModuleType(unsigned char type)
  {
    obj()->setIntVal(kTcmtConnectionIntModuleType, (int) type);
    return *this;
  };

  /** Get the module type.
   * The module type is not needed but this redundant information allows
   * to verify the configuration correctness.
   * @todo remove this redundant information?
   */
  unsigned char getModuleType() const
  {
    return static_cast<unsigned char>(getIntVal(kTcmtConnectionIntModuleType));
  };

  /** Set the serial number of a module.
   * This number together with the module type is used to uniquely identify a module.
   * This unique identification is for example needed to get the correct calibration
   * constants.
   */
  TcmtConnection& setModuleID(int module_id)
  {
    assert( module_id>=0 && module_id<256);
    unsigned int newFastID = (this->getFastID() & 0xFFFF00FF) | (module_id << 8);
    this->setFastID( newFastID );
    return *this;
  };

  /** Get the serial number of a module.
   * @sa setModuleID
   */
  int getModuleID() const
  {
    return (this->getFastID() >> 8) & 0xff;
  };

  /** Set the strip ID inside module.  Strip ID runs from 0 to 19.
   */
  TcmtConnection& setStripID(int strip) {
    assert( strip>=0 && strip<=19);
    unsigned int newFastID = (this->getFastID() & 0xFFFFFF00) | strip;
    this->setFastID( newFastID );
    return *this;
  };
  /** Get the strip ID inside module.
   * @sa setStripID
   */
  int getStripID() const {
    return this->getFastID() & 0xff;
  };

  int getConnectorType() const {
    return 0;
  }

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class
   */
  const std::string getTypeName() const {
    return std::string("TcmtConnection");
  };

  /** Return a brief description of the data members
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "i[" << kNTcmtConnectionInts << "]: crate, elecID, fastID, module type, lowerLeftID";
    return message.str();
  };

}; // class

  inline std::ostream& operator<<( std::ostream& os, const TcmtConnection& me ) {
    os << me.getCrate()
       <<"/"<< me.getSlot()
       <<"/"<< me.getFrontEnd()
       <<"/"<< me.getChip()
       <<"/"<< me.getChannel()
       <<" ("<< std::hex << me.getElecID() << std::dec <<")"
       <<" -> ("<< std::hex << me.getFastID() << std::dec <<")"
       <<" "<< (me.getIndexOfLowerLeftCell() >> 24)
       <<"/"<< me.getModuleID()
       <<"/"<< me.getStripID()
       <<"/"<< (unsigned)me.getModuleType()
       <<"\t"<< me.getIndexOfLowerLeftCell();
    return os;
  }

} // namespace CALICE
#endif
