#ifndef __BOOJUM_COLOR_H
#define __BOOJUM_COLOR_H

/**
 *<br>
 *   Usage:       int col = CC_Green[j];                 <br> 
 *          see a list of possible colors below          <br>
 *                                                       <br>
 *           or   int col = CEDColor(j);                 <br>  
 *                                                       <br>
 *               0 <= j < infinity                       <br>
 *                                                       <br>
 *            List of colors:                            <br>
 *                                                       <br>
 *  Red,       Orange,     Plum,  Violet, Blue,          <br>
 *  LightBlue, Aquamarine, Green, Oliver, Yellow         <br>
 *                                                       <br>
 *                                                       <br>
 *    @authors V. L. Morgunov, A. Zhelezov (DESY/ITEP)   <br>
 */    


class _CEDColor {
 protected:
  static unsigned color[200];
  const unsigned *operator[](unsigned i) const { return &color[i*20]; }
 public:
  unsigned operator()(unsigned i) const { return color[i%200]; }
};
#ifdef  _DEFINE_CED_COLOR_CLASSES
#define _DEFINE_CED_COLOR_CLASS(x) _CC_##x CC_##x;
#else
#define _DEFINE_CED_COLOR_CLASS(x) extern _CC_##x CC_##x;
#endif

#define _DEFINE_CED_COLOR(x,n)			\
 class _CC_##x : public _CEDColor {           \
  public:                                       \
   unsigned operator[](unsigned i) const { return color[n*20+(i%19)+1]; }	\
 };   \
_DEFINE_CED_COLOR_CLASS(x)

///////////////////////////////
extern _CEDColor CEDColor;

_DEFINE_CED_COLOR(Red,0)
_DEFINE_CED_COLOR(Orange,1)
_DEFINE_CED_COLOR(Plum,2)
_DEFINE_CED_COLOR(Violet,3)
_DEFINE_CED_COLOR(Blue,4)
_DEFINE_CED_COLOR(LightBlue,5)
_DEFINE_CED_COLOR(Aquamarine,6)
_DEFINE_CED_COLOR(Green,7)
_DEFINE_CED_COLOR(Oliver,8)
_DEFINE_CED_COLOR(Yellow,9)

#endif
