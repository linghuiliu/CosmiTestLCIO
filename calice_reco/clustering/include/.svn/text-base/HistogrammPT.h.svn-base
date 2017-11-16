#ifndef __HISTOGRAMMPT_H
#define __HISTOGRAMMPT_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <B_Util_DA.h>

using namespace std;

/** Histogram class
    */
class HistogrammPT {
  public:
    struct HCell {
	double val;
	unsigned p_i;
	unsigned t_i;
    };
  
    HCell *c;
    unsigned c_count;
  
    //-----------------------------------------------------------------------
    /** Class for axis range
	*/
    class AxisRange {
      public:
	MinMaxRange<double> lim;
	double step;
	unsigned n;
    
	bool set(const MinMaxRange<double> &_lim, double _step);
    
	unsigned idx(double val){
	  return (unsigned)((val - lim.min)/step);
	}
    };

    AxisRange p;
    AxisRange t;
  
    /** Constructor */
    HistogrammPT(): c(0),c_count(0) {}
 
    /** Destructor */
    virtual ~HistogrammPT() { if(c) delete[] c; }
  
    /** Book histogram
	*/
    virtual bool Book(const MinMaxRange<double> &p_lim,
		      const MinMaxRange<double> &t_lim,
		      double _step=0.006);
    /** Fill histogram
	*/
    bool Fill(const Sphere3D  &sp,double e){
      HCell *pc = get(sp);
      if(!pc)
	return false;
      pc->val += e;
      return true;
    }
  
    /** Get ?
	*/
    HCell *get(const Sphere3D &sp);
  
    /** Get by index ?
	*/
    HCell *get_by_index(unsigned p_i, unsigned t_i){
      return &(c[p_i * t.n + t_i]);
    }

    /** Get t ?
	*/
    double get_t(const HCell &c){
      return t.lim.min + t.step * (.5 + c.t_i);
    }

    /** Get p ?
	*/
    double get_p(const HCell &c){
      return p.lim.min + p.step*(.5 + c.p_i);
    }
};

/** Operator '<'
    */
inline bool operator<(const HistogrammPT::HCell &a,
		      const HistogrammPT::HCell &b){
  return a.val < b.val;
}
/** Operator '>'
    */
inline bool operator>(const HistogrammPT::HCell &a,
		      const HistogrammPT::HCell &b){
  return a.val > b.val;
}

#endif
