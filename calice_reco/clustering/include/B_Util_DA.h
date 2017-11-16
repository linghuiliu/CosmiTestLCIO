/*
    * Several types to make reconstruction easy read/write.
    *
    * A. Zhelezov, 2006/2007.
    */

#ifndef __B_UTIL_DA_H
#define __B_UTIL_DA_H

#include <cmath>
#include <iostream>
#include "B_Util.h"

using namespace std;

#define   RADDEG ((double)57.2957795130823209)
#define   DEGRAD ((double)0.0174532925199432958)

/** Hypot for 3 parameters
    */
template <class _T> _T hypot3(_T x,_T y,_T z){
  return ::sqrt(x * x + y * y + z * z);
}

/** Correct the value if it outside of specified range
    */
template<class _T>
  void Put2Range(_T &value, const _T min, const _T max){
  if(value < min)
    value = min;
  if(value > max)
    value = max;
}

/*
    * Some vector manipulations...
    */
// class Point2D {
//  public:
//   double x;  // Cartesian coordinate 
//   double y;  // Cartesian coordinate

//   double rz;
//   double rz2;
  
//   void calc() { rz2=x*x+y*y; rz=::sqrt(rz2); }
//   void calc(double _x,double _y) { x=_x; y=_y; rz2=x*x+y*y; rz=::sqrt(rz2); }
//   void calc(Point2D const &a) { x=a.x; y=a.y; rz2=a.rz2; rz=a.rz; }
//   double dist2(Point2D &a) { return (x-a.x)*(x-a.x)+(y-a.y)*(y-a.y); }
//   double dist(Point2D &a) { return ::sqrt(dist2(a)); }
  
//   Point2D() : x(0.),y(0.),rz(0.),rz2(0.) { }
//   Point2D(double _x,double _y) { calc(_x,_y); }
//   Point2D(Point2D const & a) { calc(a); }
  
//   Point2D& operator=(const Point2D &a) { calc(a); return *this; }
// };

// class Vector3D;

// class Point3D : public Point2D {
//  public:
//   double z;  // Cartesian coordinate
//   double r;  // Radius-vector length

//   void calc() { Point2D::calc(); r=::sqrt(rz2+z*z); }
//   void calc(double _x,double _y,double _z) { 
//     Point2D::calc(_x,_y); z=_z; r=::sqrt(rz2+z*z); 
//   }
//   void calc(Point3D const &_a) { Point2D::calc(_a); z=_a.z; r=_a.r; }
//   Point3D& operator=(Point3D &_a) { calc(_a); return *this; }
  
//   double dist2(Point3D &a) { return Point2D::dist2(a)+(z-a.z)*(z-a.z); }
//   double dist(Point3D &a) { return ::sqrt(dist2(a)); }

//   //  Point3D operator=(Point3D &a) { return a; }
  
//   Point3D operator+(Point3D &a){
//     Point3D t(x+a.x,y+a.y,z+a.z);
//     return t;
//   }
  
//   Point3D() : Point2D(),z(0.),r(0.) {}
//   Point3D(double _x,double _y,double _z) : Point2D(_x,_y) { 
//     z=_z; r=::sqrt(rz2+z*z); 
//   }
//   Point3D(Point3D const &a) : Point2D(a) { z=a.z; r=a.r; }

//   double rdist(Point3D &a) { return ::fabs(a.r-r); }

//   bool IsBetween(Point3D &p0,Point3D &p1);
//   bool IsNear(float px,float py,float pz,float tolerance=5){
//     if(::fabs(px-x)+::fabs(py-y)+::fabs(pz-z)<tolerance)
//       return true;
//     return false;
//   }
//   Point3D& operator=(const Point3D &_a) { calc(_a); return *this; }

//   // lp-point on the like, c-direction vector
//   double dist2line(Point3D &lp,Vector3D &c); 
// };


// class Vector3D : public Point3D {
//  public:
      
//   Vector3D(Point3D &a,Point3D &b) : Point3D(b.x-a.x,b.y-a.y,b.z-a.z) { }
//   Vector3D(Point3D const &a) : Point3D(a) { }
//   Vector3D() : Point3D() { }
//   Vector3D(double _x,double _y,double _z) : Point3D(_x,_y,_z) { }
//   double dot(const Vector3D &a) { return x*a.x+y*a.y+z*a.z; }
//   double module() { return ::hypot3(x,y,z); }
//   void divide(double a) { x/=a; y/=a; z/=a; }
//   void mult(double a) { x*=a; y*=a; z*=a; }
  
//   double length2() const { return x*x+y*y+z*z; }
  
//   Point3D linear(double a,Point3D &b) {
//     Point3D p(a*x+b.x,a*y+b.y,a*z+b.z);
//     return p;
//   }

//   void accomulate(Point3D &b,double a=1.) {
//     x+=a*b.x;
//     y+=a*b.y;
//     z+=a*b.z;
//   }

//   Point3D add(const Point3D &_a){
//     Point3D p(x+_a.x,y+_a.y,z+_a.z);
//     return p;
//   }

//   Point3D rotate(const Point3D &v1,const Point3D &v2,const Point3D &v3){
//     Point3D p(x*v1.x+y*v2.x+z*v3.x,
// 	      x*v1.y+y*v2.y+z*v3.y,
// 	      x*v1.z+y*v2.z+z*v3.z);
//     return p;
//   }

//   Vector3D dirco(){ //Direction COSines 
//     double aa=1./module();
//     Vector3D p(x*aa,y*aa,z*aa);
//     return p;
//   }
// };

// inline double Point3D::dist2line(Point3D &lp,Vector3D &c){
//   Vector3D w(*this,lp);
//   Vector3D _p(c.y*w.z-c.z*w.y,c.z*w.x-c.x*w.z,c.x*w.y-c.y*w.x);
//   return _p.module()/c.module();
// }

// inline Vector3D operator-(const Point3D &a,const Point3D &b){
//   Vector3D t(a.x-b.x,a.y-b.y,a.z-b.z);
//   return t;
// }
  
// Point3D Projection(Point3D &p,Point3D &p0,Point3D &p1);

// class Sphere3D {
//  public:
//   double r;  // Radius
//   double t;  // Theta  in radians
//   double p;  // Phi    in radians
//   Sphere3D() : r(0.),t(0.),p(0.) {}
//   void calc(Point3D &a) {
// //    Fill Spherical coordinate system
//     double rsq = ::hypot(a.x,a.y);
//     r = ::hypot3(a.x,a.y,a.z);
//     t = atan2(rsq,a.z);
//     p = atan2(a.y,a.x);
//     if (p < 0.0) 
//       p = 2*M_PI + p;
//   }
//   void calc(double _r,double _t,double _p){
//     r=_r; t=_t; p=_p;
//   }
// };

// class SimpleLinearFit {
// public:
//   double sum_x,sum_y,sum_x2,sum_y2,sum_xy;
//   unsigned n;
  
//   double a;
//   double b;
//   double md; // mean displacement

//   void init(){
//     sum_x=sum_y=sum_x2=sum_y2=sum_xy=0.;
//     a=b=md=0.;
//     n=0;
//   }
//   void add(double x,double y){
//     sum_x+=x;
//     sum_x2+=x*x;
//     sum_y+=y;
//     sum_xy+=x*y;
//     n++;
//   }
//   void calc(){
//     if(n>1){
//       a=(n*sum_xy-sum_x*sum_y)/(n*sum_x2-sum_x*sum_x);
//       b=(sum_y-a*sum_x)/n;
//       md=(sum_y2+a*a*sum_x2+n*b*b+2*a*b*sum_x-2*a*sum_xy-2*b*sum_y)/n;
//     }
//   }
// };

/*
    * Simple range
    */
template<class _T>
  class Range {
    public:
      _T max;/**<maximum value*/
      _T min;/**<minimum value*/
      Range(_T _min, _T _max) : max(_max), min(_min) {}
      Range(_T _minmax) : max(_minmax), min(_minmax) {}

      void extend(_T diff) { 
	min -= diff; 
	max += diff; 
      }
      _T range() const { 
	return max-min; 
      }
      bool inrange(const _T value) const { 
	return (value>=min)&&(value<=max); 
      }
      bool valid() const { 
	return max>min; 
      }
  };

/*
    * Minimum/maximum
    */
template<class _T>  
  class MinMaxRange : public Range<_T> {
    public:
      MinMaxRange() : Range<_T>(100000,-100000) {}

      /** Add  value
	  */
      void set(_T value){
	if(value < this->min) this->min=value;
	if(value > this->max) this->max=value;
      }

      void init() {  this->min=100000; this->max=-100000; }
  };

// #define ARSIZE(x) (sizeof(x)/sizeof(x[0]))

/*
    * Angular distance, calculated in radians.
    * To use with degrees, use deg_angle_dist().
    */
double angle_dist(double t1,double p1,double t2,double p2);
#define deg_angle_dist(t1,p1,t2,p2)					\
  (angle_dist(DEGRAD*(t1),DEGRAD*(p1),DEGRAD*(t2),DEGRAD*(p2))*RADDEG)

/* Find M smallest/biggest values (ordered) in array with size N.
    * Works faster than qsort in case N<<M.
    *
    * b[M] is filled with sordered values
    * ia[M] is filled with indexes (in a) of this values.
    * both arrays are optional (but must provide required space when used)
    *
    * Inspirated by: V.L. Morgunov
    * Implemented by: A. Zhelezov
    */
template<typename _t,typename _func>
  void SortMfromN(unsigned n,const _t *a,unsigned m,_t *b=0,unsigned *ia=0){
  if(m>n || !a){
    std::cerr<<"Incorrect Min_amp call: "<<n<<" "<<m<<std::endl;
    return;
  }
  typedef struct _item {
      struct _item *nxt;
      struct _item *prv;
      unsigned idx;
  } item;
  item x[m];
  unsigned x_used=0;
  item *head=0,*last=0,*p;
  if(m>1){
    for(unsigned i=0;i<n;i++){
      p=head;
      for(;p;p=p->nxt){
	if(_func::comp(a[i],a[p->idx]))
	  continue;
	item *newp;
	if(x_used<m)
	  newp=x+x_used++;
	else if(p == last){
	  last->idx=i;
	  break;
	} else {
	  newp=last;
	  last=last->prv;
	  last->nxt=0;
	}
	newp->nxt=p;
	if((newp->prv=p->prv))
	  p->prv->nxt=newp;
	else
	  head=newp;
	p->prv=newp;
	newp->idx=i;
	break;
      }
      if(!p && x_used<m){
	item *newp=x+x_used++;
	if((newp->prv=last))
	  last->nxt=newp;
	else
	  head=newp;
	last=newp;
	newp->nxt=0;
	newp->idx=i;
      }
    }
  } else if(m==1){ // make result collection work in this case
    head=x;
    x[0].idx=0;
    x[0].nxt=0;
  }
  if(b){
    p=head;
    for(unsigned i=0;i<m;i++,p=p->nxt)
      b[i]=a[p->idx];
  }
  if(ia){
    p=head;
    for(unsigned i=0;i<m;i++,p=p->nxt)
      ia[i]=p->idx;
  }
}

template<typename _t>
  class _comp_lt {
    public:
      inline static bool comp(const _t &x,const _t &y) { return x<y; }
  };
template<typename _t>
  class _comp_gt {
    public:
      inline static bool comp(const _t &x,const _t &y) { return x>y; }
  };


template<typename _t>
  void SmallestMfromN(unsigned n,const _t *a,unsigned m,_t *b=0,unsigned *ia=0){
  SortMfromN<_t,_comp_gt<_t> >(n,a,m,b,ia);
}

template<typename _t>
  void BiggestMfromN(unsigned n,const _t *a,unsigned m,_t *b=0,unsigned *ia=0){
  SortMfromN<_t,_comp_lt<_t> >(n,a,m,b,ia);
}

//=========================================================
void accumulate(Vector3D &centerOfGravity, Point3D &b, double ampl=1);
double dist2line(Point3D &point1, Point3D &point2, Vector3D &vec);
Vector3D directionCos(Vector3D &vec);
void initializeSphere(Sphere3D &sphere, double radius, double theta, double phi);

#endif /* __B_UTIL_H */
