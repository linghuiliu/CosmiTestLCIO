#ifndef TBTrack_MapConstants_HH
#define TBTrack_MapConstants_HH

#include <string>
#include <iostream>

//Forces the compiler to aling this class on a 32 bit boundary
//Needed to protect implemented type 'safety' check
#pragma pack (4)
namespace TBTrack {

class MapConstants {

public:
  MapConstants(int p=0);

  std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;

  enum {numberOfInts=1};
  const int* intData() const;
  int* intData();

  enum {numberOfFloats=0};
  const float* floatData() const;
  float* floatData();

  enum {numberOfDoubles=0};
  const double* doubleData() const;
  double* doubleData();

private:
  int _period;
};

}
//Reset the alignment
#pragma pack()
#endif
