#ifndef __AHCHcalEntry_hh__
#define __AHCHcalEntry_hh__ 1

#include "AHCEntry.hh"
#include "HcalImplicits.hh"
#include "ModuleConnection.hh"
#include "ModuleLocation.hh"

/** @brief Class for an HCAL entry in the CALICE DAQ configuration file
 *
 *  This class implements the AHCEntry interface in order to read
 *  config entries with HCAL and HCAL8 tags.
 *
 *  @author Niels.Meyer@desy.de
 *  @date   May 2008
 */
class AHCHcalEntry : public AHCEntry {
public:
  AHCHcalEntry( const std::string& line );
  ~AHCHcalEntry();

  /** @brief Implements AHCEntry::init
   *  This routine knows about the input format and needs to be adjusted 
   *  upon changes inside the online software
   */
  virtual void init( const std::string& line );

  /** @brief Implements AHCEntry::clone
   *  Create a clone object and return pointer to the clone - unintended 
   *  change of this object is excluded
   */
  virtual AHCHcalEntry* clone() const { return new AHCHcalEntry( *this ); }

  const unsigned int slot() const { return _slot; }
  const unsigned int frontEnd() const { return _fe; }
  const unsigned int moduleID() const { return _modId; }
  const unsigned int layer() const { return _layer; }
  const bool isFine() const { return ( _type == "AHCAL" ); }
  const bool isCoarse() const { return ( _type == "AHCAL8" ); }
  void setAbsorberType(const std::string _absorberType) {  _absorberFeW = _absorberType; }
  const std::string getAbsorberType() const { return _absorberFeW; }

  const CALICE::ModuleConnection getConnection( const bool upper ) const;
  const CALICE::ModuleLocation getLocation( const bool upper ) const;

protected:
  unsigned int _slot;
  unsigned int _fe;
  unsigned int _modId;
  unsigned int _layer;
  unsigned int _cmbId;
  unsigned int _cmbCanAdr;
  unsigned int _cmbPinId; 
  unsigned int _holdExt;
  unsigned int _holdLedCm;
  unsigned int _holdLedPm;
  unsigned int _vcalibCm;
  unsigned int _vcalibPm;
  std::string _absorberFeW;
};  // class AHCHcalEntry

#endif

