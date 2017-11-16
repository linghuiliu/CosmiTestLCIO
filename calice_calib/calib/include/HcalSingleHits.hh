#ifndef HCALSINGLEHITS_HH_
#define HCALSINGLEHITS_HH_

struct HcalSingleHit
{
  int I;
  int J;
  int K;
  float x;
  float y;
  float z;
  float energy;
  float hittime;
  int hittype;
  int CellID0;
};

#endif /*HCALSINGLEHITS_HH_*/
