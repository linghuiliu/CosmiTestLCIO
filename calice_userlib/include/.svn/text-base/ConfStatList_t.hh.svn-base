#ifndef _ConfStatList_t_hh_
#define _ConfStatList_t_hh_

#include <FeStatList_t.hh>

/** Statistics about configuration record errors.
 * The object is meant to be used together with @ref FeStatList_t.
 * This object collects statistics about bad configuration data.
 * The following errors are counted: the data which is read back
 * does not agree with the data which was written;
 * configuration data was written twice but never read back.
 * @author Goetz Gaycken, LLR - Ecole polytechnique
 * @date feb, 2006
 */
class ConfStat_t : public StatObj_t
{
public:
  ConfStat_t() : _n(0), _feBad(0),_consecutiveWrites(0),_nTotal(0), _feBadTotal(0),_consecutiveWritesTotal(0) {};

  /** Add statistics of the last 10 records.
   */
  void finish() {unCheckedCheckPoint();};

  /** Print the table header.
   */
  void printTableHeader(std::ostream &out) const ;

  /** Print the statistics of one object (one table row).*/
  void print(std::ostream &out) ;

  /** Return true if at least one error was detected for the corresponding front-end.*/
  bool hasInfo() const {return error();};

  /** The configuration data of the corresponding front-end is okay.*/
  void addGood() { checkPoint(); _n++;};

  /** The configuration data of the corresponding front-end is broken.*/
  void addBad()  { checkPoint(); _n++; _feBad++;};

  /** The configuration data of the corresponding front-end could bot be verified.*/
  void addConsecutiveWrites() { checkPoint(); _n++;_consecutiveWrites++;};
  
  /** Add the statistics gathered from the last 10 records to the total statistic and start a fresh sample.
   */
  void checkPoint() {
    if (_n > 10) {
      unCheckedCheckPoint();
    }
  }

  /** Add the statistics of the last sample to the total statistic.
   */
  void unCheckedCheckPoint() {
      _nTotal+=_n;
      _feBadTotal+=_feBad;
      _consecutiveWritesTotal+=_consecutiveWrites;
      _n=0;
      _feBad=0;
      _consecutiveWrites=0;;
  };
  
  /** The size of the last sample.
   */
  unsigned int n() const {return _n;};

  /** The number of bad configuration records for this front-end in the last sample.
   */
  unsigned int bad() const {return _feBad;};

  /** The number of unverified configuration records for this front-end in the last sample.
   */
  unsigned int consecutiveWrites() const {return _consecutiveWrites;};

  /** The number of configuration errors for this front-end in the last sample.
   */
  unsigned int error() const {return _feBadTotal+_consecutiveWritesTotal;};
  
  /** The total sample size.
   */
  unsigned int nTotal() const {return _nTotal;};

  /** The total number of bad configuration records for this front-end.
   */
  unsigned int badTotal() const {return _feBadTotal;};

  /** The total number of unverified configuration records for this front-end.
   */
  unsigned int consecutiveWritesTotal() const {return _consecutiveWritesTotal;};
  
private:
  unsigned int _n;
  unsigned int _feBad;
  unsigned int _consecutiveWrites;
  
  unsigned int _nTotal;
  unsigned int _feBadTotal;
  unsigned int _consecutiveWritesTotal;
};

/** the specific list which will provide for each existing front-end a statistics object.
 */
typedef FeStatList_t<ConfStat_t> ConfStatList_t;
  
#endif
