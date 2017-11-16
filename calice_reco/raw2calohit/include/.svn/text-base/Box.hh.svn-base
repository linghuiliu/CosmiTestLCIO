#ifndef _Box_hh_
#define _Box_hh_

#include <RtypesSubSet.h>
#include <NVector_t.hh>
#define CAN_CAST_CALORIMETERHIT_GETPOSITION

namespace CALICE {

  /** Caclulate mean and RMS vaslue of a set of input values.
   */
  class Stat_t {
  public:
    friend std::ostream &operator <<(std::ostream &os, Stat_t &a);
    
    Stat_t() : _weight(0.),_sum(0.),_sum2(0.) {};
    void reset() {_weight=0.;_sum=0.; _sum2=0.;}
    void add(Float_t value, Float_t weight=1.) {_weight+=weight; _sum+=value*weight; _sum2+=value*value*weight; };
    void calculate() {
      if (_weight>1) {
	Double_t mean=_sum/_weight;
	Double_t sigma=sqrt((_sum2-mean*_sum)/(_weight-1));
	_mean=mean;
	_sigma=sigma;
      }
    };
    Double_t mean() const {return _mean;};
    Double_t sigma() const {return _sigma;};
    
  protected:
    Double_t  _weight;
    Double_t _sum;
    Double_t _mean;
    Double_t _sum2;
    Double_t _sigma;
  };

  /** Helper class to calculate an envelop cuboid around hit collections.
   * This class can be used to quickly reject hits which are far away from
   * the hit collection contained in this box.
   */
  class Box
  {
  public:
    Box() {reset();};

    /** Remove all hits from the box.
     */
    void reset() {_hits.clear();for(UInt_t i=0; i<3;i++) {_low[i]=FLT_MAX; _high[i]=-FLT_MAX;_mean[i].reset();};_merged=false;};

    /** Add a new hit to the box.
     * The envelop is not calculated automatically.
     * @sa calculateSize.
     */
    void add(const EVENT::CalorimeterHit *a_hit) {
#ifdef CAN_CAST_CALORIMETERHIT_GETPOSITION
      const ThreeVector_t *pointP=reinterpret_cast<const ThreeVector_t *>(a_hit->getPosition());
      for (UInt_t i=0; i<3; i++) {
	if ((*pointP)[i]<_low[i]) {_low[i]=(*pointP)[i];};
	if ((*pointP)[i]>_high[i]) {_high[i]=(*pointP)[i];};
	_mean[i].add((*pointP)[i]);
      }
#else
      for (UInt_t i=0; i<3; i++) {
	if ((a_hit->getPosition())[i]<_low[i]) {_low[i]=(a_hit->getPosition())[i];};
	if ((a_hit->getPosition())[i]>_high[i]) {_high[i]=(a_hit->getPosition())[i];};
	_mean[i].add((a_hit->getPosition())[i]);
      }
#endif
      _hits.push_back(a_hit);
    };

    /** Add all hits from the given box and recalculate the envelop.
     * The envelop is not calculated automatically.
     * @sa calculateSize.
     */
    void add(const Box &a) {
      for( std::vector<const EVENT::CalorimeterHit*>::const_iterator hit_iter=a._hits.begin();
	   hit_iter!=a._hits.end();
	   hit_iter++) {
	add(*hit_iter);
      }
    };

    /** Return true if the given position is closer to the envelop than the specified distance.
     */
    Bool_t closerToEnvelopThan(Float_t distance, const ThreeVector_t *a) const {
      for (UInt_t i=0; i<3;i++) {
	if (_low[i]-(*a)[i]>distance || (*a)[i]-_high[i]>distance) return false;
      }
      return true;
    };

    /** Return true if the given box is closer to this box than the specified distance.
     */
    Bool_t closerToEnvelopThan(Float_t distance, const Box &a_cluster) const {
      for (UInt_t i=0; i<3;i++) {
	if (_low[i]-a_cluster._high[i]>distance || a_cluster._low[i]-_high[i]>distance) return false;
      }
      return true;
    };

    /** Return the minimum distance of the given hits from the hits contained in this box.
     */
    Double_t minDistanceToPoints(const EVENT::CalorimeterHit *a_hit) const {

#ifdef CAN_CAST_CALORIMETERHIT_GETPOSITION	
      const ThreeVector_t *hit_point=reinterpret_cast<const ThreeVector_t *>(a_hit->getPosition());
#else
      ThreeVector_t a_hit_point;
      a_hit_point.set(a_hit->getPosition());
      const ThreeVector_t *hit_point=&a_hit_point;
#endif
      return minDistanceToPoints(hit_point);
    };

    /** Return the minimum distance of the given position from the hits contained in this box.
     */
    Double_t minDistanceToPoints(const ThreeVector_t *hit_point) const {
      Double_t min_distance2=FLT_MAX;
      for( std::vector<const EVENT::CalorimeterHit*>::const_iterator hit_iter=_hits.begin();
	   hit_iter!=_hits.end();
	   hit_iter++) {
#ifdef CAN_CAST_CALORIMETERHIT_GETPOSITION
	const ThreeVector_t *pointP=reinterpret_cast<const ThreeVector_t *>((*hit_iter)->getPosition());
#else
	ThreeVector_t temp;
	temp.set((*hit_iter)->getPosition());
	const ThreeVector_t *pointP=&temp;
#endif
	Double_t temp_scalar=dotOfDifference(*hit_point,*pointP);
	if (temp_scalar<min_distance2) {min_distance2=temp_scalar;};
      }
      return sqrt(min_distance2);
    };

    /** Print out the positiosn of all contained hits.
     */
    void show() {
      for (UInt_t i=0; i<3; i++) {
	_mean[i].calculate();
	std::cout << _mean[i].sigma();
	if (i<2) {
	  std::cout << ", ";
	}
      }
      std::cout << std::endl;
      for( std::vector<const EVENT::CalorimeterHit *>::const_iterator hit_iter=_hits.begin();
	   hit_iter!=_hits.end();
	   hit_iter++) {
	std::cout << "\t";
	for (UInt_t i=0; i<3; i++) {
	  std::cout << (*hit_iter)->getPosition()[i];
	  if (i<2) {
	    std::cout << ", ";
	  }
	}
	std::cout << std::endl;
      }
    };
    
    /** Calculate the envelop.
     */
    void calculateSize() {
      for (UInt_t i=0; i<3; i++) {
	_mean[i].calculate();
      }
    };

    /** Get the dimensions of one axis.
     * @param i axis (0-2).
     */
    Double_t getSize(UInt_t i) const {return _mean[i].sigma();};

    /** Return true if this box was merged with another one.
     * If a box is merged all hits are added to the box with which it is merged.
     * Thus, the same hits are stored in two boxes.
     */
    Bool_t isMerged() const {return _merged;};

    /** Set the status to "merged".
     */
    void setMerged() {_merged=true;};

    ThreeVector_t _low;
    ThreeVector_t _high;
    //    std::vector<const ThreeVector_t *> _pointsP;
    std::vector<const EVENT::CalorimeterHit*> _hits; /**< the hit collection contained in this box.*/
    Stat_t _mean[3];                                 /**< the position of the box centre and its dimesnion (all three axis).*/  
    Bool_t _merged;                                  /**< true if this box is merged with another one.*/
  };

  inline std::ostream &operator <<(std::ostream &os, Stat_t &a) {
    os << a._weight << ":" << a._mean << "+-" << a._sigma;
    return os;
  }

}
#endif
