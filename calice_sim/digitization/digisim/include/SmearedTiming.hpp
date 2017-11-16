
#ifndef _SMEAREDTIMING_HPP_
#define _SMEAREDTIMING_HPP_

#include "FunctionModifier.hpp"
#include <vector>
#include <string>

namespace digisim {

class SmearedTiming : public CalHitModifier {

public:
  //----------methods-------------------
  /** Returns a reference to a new instance */
  SmearedTiming* newInstance(const std::string& modName) {
    return new SmearedTiming(modName);
    }

  //bool registerMe();

  /** default constructor is only called for global instance */
  SmearedTiming();

  /** initialize parameters */
  void init(std::vector<float>& floats);

  /*
   * This is where real work is performed.
   * @param hitmap map with transient hits.  Both input and output.
   */
  void processHits(std::map<long long, TempCalHit>& hitmap);

  /** Smeared linear transformations on time */
  double transformTime(const TempCalHit& hit);

  /** debugging printout */
  void print() const;

private:
  //-------------methods-----------------
  // constructor
  SmearedTiming(const std::string& modName) { }

  // copy constructor, not to be defined
  SmearedTiming(const SmearedTiming& rhs);

  //-----------data members-------------

  /** Parameters: gain and threshold */
  std::vector<float> _par;

  /** Global instance, used only for registration */
  //SmearedTiming _me = new SmearedTiming();

  };
 }  //end namespace digisim
#endif 
