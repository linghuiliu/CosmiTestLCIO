#include <cassert>
#include <iomanip>

#include "MapConstants.hh"

namespace TBTrack {

MapConstants::MapConstants(int p) :
  _period(p) {

  assert(sizeof(MapConstants)==
         sizeof(int)*numberOfInts+
         sizeof(float)*numberOfFloats+
         sizeof(double)*numberOfDoubles);
}

std::ostream& MapConstants::print(std::ostream &o, const std::string &s) const {
  o << s << "MapConstants::print()" << std::endl;

  o << s << " Period = " << _period << std::endl;

  return o;
}

const int* MapConstants::intData() const {
  return &_period;
}

int* MapConstants::intData() {
  return &_period;
}

const float* MapConstants::floatData() const {
  return 0;
}

float* MapConstants::floatData() {
  return 0;
}

const double* MapConstants::doubleData() const {
  return 0;
}

double* MapConstants::doubleData() {
  return 0;
}

}
