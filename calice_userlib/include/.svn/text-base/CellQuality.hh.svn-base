#ifndef __CellQuality_hh__
#define __CellQuality_hh__

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"

namespace CALICE {

/**
 * LCIO contions data class to describe the cell quality. The cell 
 * can be dead or noisy (or both, for instance if a cell is noisy and 
 * then dies during a run period).
 *
 * Note: Internally the status is treated as a bit pattern, which allows 
 * setting several flags simulaneously and can easily be extended.
 * Thanks to the user interface the user never has to fiddle around with the
 * bits. The definition is protected inside the class and queried with
 * isDead() or isNoisy().
 *
 * @author Niels Meyer, Desy
 * @author Martin Killenberg, CERN
 */
class CellQuality : public UTIL::LCFixedObject< 2, 0, 0 > {
 public:
  CellQuality();
  /** Initialise the CellQuality with the CellID. When the status is already know it can
   *  also be given to the constructor (optional).
   */
  CellQuality( const int id, const int status = 0);
  CellQuality( EVENT::LCObject* );
  virtual ~CellQuality();

  const int getCellID() const; ///< Get the cell ID
  const int getStatus() const; ///< Get the status

  bool isDead() const; ///< Check if the cell has been marked as dead
  bool isNoisy() const; ///< Check if the cell has been marked as noisy 

  /// Set the dead flag. By using \c setDead(false) you can unset the dead status.
  void setDead( bool dead = true );

  /// Set the noisy flag. By using \c setNoisy(false) you can unset the noisy status.
  void setNoisy( bool noisy = true );


  virtual const std::string getTypeName() const;
  virtual const std::string getDataDescription() const;

protected:
  static const int _dead  = 0x1; ///< the dead bit: 0x1
  static const int _noisy = 0x2; ///< the noisy bit: 0x2

private:
  /** The setting of the status is private. It is only allowed using the conctructor
   *  or the set() functions.
   */
  void setStatus(int status);
 };  // class CellQuality

} // namespace CALICE

#endif //  __CellQuality_hh__
