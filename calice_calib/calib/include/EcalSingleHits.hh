#ifndef ECALSINGLEHITS_HH_
#define ECALSINGLEHITS_HH_

struct EcalSingleHit
{
  int I;
  int J;
  int K;
  float x;
  float y;
  float z;
  float energy;
  float hittime;
  int CellID0;
  int hittype;
  int multiplicity;
  bool Overlap_strip;
};

#endif /*ECALSINGLEHITS_HH_*/
