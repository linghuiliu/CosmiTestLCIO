#include "CalHitModifier.hpp"
#include <vector>
#include <map>
#include <set>

#ifndef _ABSVALUEDISCRIMINATION_HPP_
#define _ABSVALUEDISCRIMINATION_HPP_

namespace digisim {

 class AbsValueDiscrimination : public CalHitModifier {

  public:
   //----------methods-------------------
     /** Returns a reference to a new instance */
  AbsValueDiscrimination* newInstance(const std::string& modName) {
    return new AbsValueDiscrimination(modName);
    }

   /** default constructor is only called for global instance */
   AbsValueDiscrimination();

   /** initialize parameters */
   void init(std::vector<float>& floats);

   /*
    * This is where real work is performed.
    * @param hitmap map with transient hits.  Both input and output.
    */
   void processHits(std::map<long long, TempCalHit>& hitmap);

   /** debugging printout */
   void print() const;


  private:

   // constructor
   AbsValueDiscrimination(const std::string& modName) { }

   // copy constructor, not to be defined
   AbsValueDiscrimination(const AbsValueDiscrimination& rhs);

   //-------------methods-----------------
   /** transformation functions: discrimination */
   bool isBetweenThresholds(double energy);


   //-----------data members-------------

   /** Parameters: gain and threshold */
   std::vector<float> _par;

   };
  } //end namespace digisim
#endif
