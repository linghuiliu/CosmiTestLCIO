#ifndef _BoardID_hh_
#define _BoardID_hh_

#include <iostream>
#include <iomanip>
#ifdef VALUE_CHECK
#    include <cassert>
#endif

namespace CALICE {
/** Create a packed board id of crate, slot, component numbers and the label.
 * Class is cheap and can be used to generate a packed board id:
 *  unsigned int packed_id = BoardID(short crateID, short slotID, short boardComponentNumber, short boardLabel).id();
 * or
 *  unsigned int packed_id = BoardID(short crateID, short slotID, short boardComponentNumber, short boardLabel);
 */
class BoardID
{
public:

  /** Create the packed id.
   */
  /*  BoardID(const short crateID, const short slotID, const short boardComponentNumber, const short boardLabel)
      : _id(((((((crateID & 0xff)<<8) | (slotID & 0xff))<<8) | (boardComponentNumber & 0xff) ) << 8) | (boardLabel &0xff))*/

  /** default constructor.
   * The default constructor produces an illegal board id.
   */
  BoardID() :_id(0) {
  }

  /**Label does not belong to the Device ID since it ony indicates
   *whether data were written or read back*/
  BoardID(const short crateID, const short slotID, const short boardComponentNumber)
    : _id(   ((((crateID & 0xff)<< 8) | (slotID & 0xff))<< 8) | (boardComponentNumber & 0xff))
  {
#if VALUE_CHECK
    assert((crateID&0xffffff00)==0);
    assert((slotID&0xffffff00)==0);
    assert((boardComponentNumber&0xffffff00)==0);
    //assert((boardLabel&0xffffff00)==0);
#endif
  };

  /** Wrap interface around packed id.
   * Cheap method to decode the packed components. 
   * slot_id=BoardID(packed_id).getSlotID();
   */
  BoardID(const unsigned int packed_id) 
    : _id(packed_id)
  {
  };

  /** Unpack the crate id.
   */
  short getCrateID() const {
    return (_id >> 16);
  };

  /** Unpack the slot id.
   */
  short getSlotID() const {
    return (_id >> 8) & 0xff;
  };

  /** Unpack the board component number
   */
  short getBoardComponentNumber() const {
    return _id   & 0xff;
  };

  /** Unpack the board label
   */
  //short getBoardLabel() const {
  //  return  _id        & 0xff;
  //};

  /** Get the packed id.
   */
  unsigned int id() const {return _id;};

  /** Get the packed id.
   */
  operator unsigned int() {return _id;};


  /** Compiler complained about missing <, > operators when BoardID was
    * directly used in a map, therefore we overload this operators
    */
   friend bool const operator <( const BoardID & b1, const BoardID& b2);

  /** See above ...*/
   friend bool const operator >( const BoardID & b1, const BoardID& b2);

private:
  unsigned int _id;
};


  /** Implemetations of overloaded operators */
inline bool const operator>(const BoardID &boardId1 , const BoardID &boardId2) {

  if ( boardId1.id() > boardId2.id() ) return true;
  else return false;
}

inline bool const operator<(const BoardID &boardId1 , const BoardID &boardId2) {

  if ( boardId1.id() < boardId2.id() ) return true;
  else return false;
}

inline std::ostream &operator <<(std::ostream &os, const BoardID &a) {
  os << "crate="      << std::setw(3)  << a.getCrateID()
     << " slot="      << std::setw(2)  << a.getSlotID() 
     << " component=" << std::setw(2)  << a.getBoardComponentNumber() 
    ;
  //     << " label="     << std::setw(2)  << a.getBoardLabel() 
  //  ;
  return os;
}


}
#endif

