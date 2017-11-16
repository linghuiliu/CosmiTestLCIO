#include <algorithm>
#include <cmath>

#include <HistogrammPT.h>

//-----------------------------------------------------------------------
bool HistogrammPT::AxisRange::set(const MinMaxRange<double> &_lim,
				  double _step){
//-----------------------------------------------------------------------
  step = _step;
  n = 0;

  if(_lim.min > _lim.max){
    cout<<"Range Error "<<lim.min<<" "<<lim.max<<endl;
    return false;
  }

  lim.min = _lim.min - step;
  n = (unsigned)((_lim.max - lim.min)/step + 3);
  n = max(n, (unsigned)4);
  lim.max = lim.min + n * step;
  return true;
}
//-----------------------------------------------------------------------
bool HistogrammPT::Book(const MinMaxRange<double> &p_lim,
			const MinMaxRange<double> &t_lim,
			double _step) {
//-----------------------------------------------------------------------
  if( !p.set(p_lim, _step) || !t.set(t_lim, _step) )
    return false;
  c_count = p.n * t.n;
  c = new HCell[c_count];

  for(unsigned p_i = 0; p_i < p.n; p_i++)
    for(unsigned t_i = 0; t_i < t.n; t_i++){
      HCell &pc = *get_by_index(p_i, t_i);
      pc.p_i = p_i;
      pc.t_i = t_i;
      pc.val = 0.;
    }
  return true;
}
//-----------------------------------------------------------------------
HistogrammPT::HCell *HistogrammPT::get(const Sphere3D &sp){
//-----------------------------------------------------------------------
  unsigned idx_p = p.idx(sp.p), idx_t = t.idx(sp.t);
  if((idx_p >= p.n) || (idx_t >= t.n)){
    cerr<<"Histogram:Fill: Out of range!"<<endl;
    return 0;
  }
  return &(c[idx_p * t.n + idx_t]);
}
