
#include "HitContrib.hpp"
using digisim::HitContrib;

//constructor
HitContrib::HitContrib(double energy, double time) {
  _energy = energy;
  _time = time;
}

//Returns contribution energy
double HitContrib::energy() const{
  return _energy;
}

//Returns contribution timing
double HitContrib::time() const{
  return _time;
}

//method to scale energy contribution; used to apply factors to the energy values of individual contributions
void HitContrib::scale(double factor) {
  _energy *= factor;
}

//Increment energy contribution
void HitContrib::increment(double energy, double time) {
  _energy += energy;
    if( time<_time ) _time =time;
}
