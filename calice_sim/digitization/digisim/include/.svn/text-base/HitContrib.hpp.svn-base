#ifndef _HITCONTRIB_HPP_
#define _HITCONTRIB_HPP_ 1

namespace digisim {

class HitContrib {
public:
  //Constructor
  HitContrib(double energy, double time);

  //method to return contributing energy
  double energy() const;

  //method to return contribution timing
  double time() const;

  //method to scale energy contribution; used to apply factors to the energy values of individual contributions
  void scale(double factor);

  void increment(double energy, double time);

private:
  // private default constructor
  HitContrib() {};

private:
  //--Data Members--

  //energy contribution from simHit to tempHit
  double _energy;

  //timing from this contribution
  double _time;
};

} // end ns digisim

#endif // _HITCONTRIB_HPP_
