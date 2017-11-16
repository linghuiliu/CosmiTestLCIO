#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>

#include <B_Util_DA.h>

using namespace std;

//============================================================================
Point3D Projection(Point3D &p,Point3D &p0,Point3D &p1){
//============================================================================
  Vector3D v(p0,p1);
  Vector3D w(p0,p);
  Point3D pr=v.linear(w.dot(v)/v.dot(v),p0);
  return pr;
}
//============================================================================
bool Point3D::IsBetween(Point3D &p0,Point3D &p1){
//============================================================================
  if((x>max(p0.x,p1.x)) || (x<min(p0.x,p1.x))||
     (y>max(p0.y,p1.y)) || (y<min(p0.y,p1.y))||
     (z>max(p0.z,p1.z)) || (z<min(p0.z,p1.z)))
    return false;
  return true;
}
//============================================================================
double angle_dist(double t1,double p1,double t2,double p2){
//============================================================================
  double angle=sin(t1)*cos(p1)*sin(t2)*cos(p2)+
               sin(t1)*sin(p1)*sin(t2)*sin(p2)+
               cos(t1)*cos(t2);
  if(angle>1.0)
    return 0.0;
  return fabs(acos(angle));
}
//============================================================================
void Order_Max(int n,double *a,int m, double *b, int *ia){
//============================================================================
/*      Utility routine to find M smallest values                    
 from N values and thier addresses in the initial array
 Can be used if M << N in opposite case better use SORTZV (CERN-lib)                
    Author    V.L. Morgunov, V.A. Ivanova  created   04-Sep-2001
    rewitten to C++ at 30-Nov-2006
 INPUT  n -- initial array size                               
        a -- Array with tested numbers                        
        m -- Number of needed min numbers                     
 OUTPUT
        b -- Sorted array                                     
        ia -- addresses of sorted numbers in initial array    
*/
  for(int l = 0; l < m; l++)
    b[l] = 1.e10;

  int key = 0;
  int i,j,k;
  double aa;
  for(i = 0; i < n; i++){
    aa = a[i];
    for(j = 0; j < m; j++){
      if(aa > b[j]){
	for(k=key; k > j; k--){
	   b[k] =  b[k-1];
	  ia[k] = ia[k-1];
	}
	key = key + 1;
	if(key >= m-1) 
	  key = m-1;
	b[j] = aa;
	ia[j] = i;
	break;
      }
    }
  }
} 
  /*
//============================================================================
void FastOrder_Min(int n,double *a,int m, double *b, int *ia){
//============================================================================
      Utility routine to find M smallest values                    
 from N values and thier addresses in the initial array
 Can be used if M << N in opposite case better use SORTZV (CERN-lib)                
    Author    V.L. Morgunov, V.A. Ivanova  created   04-Sep-2001
    Modified A.V. Zhelezov, 30-Nov-2006
 INPUT  n -- initial array size                               
        a -- Array with tested numbers                        
        m -- Number of needed min numbers                     
 OUTPUT
        b -- Sorted array                                     
        ia -- addresses of sorted numbers in initial array    

  if(m>n)
    m=n;

  struct _fom {
    struct _fom *nxt;
    struct _fom *prv;
    int idx;
  } fom[m];
  struct _fom *head=0;
  int key_count=0;

  for(int i=0;i<n;i++){
    double aa=a[i];
    struct _fom *p;
    for(p=head;p;p=p->nxt)
      if(aa<a[p->idx])
	break;
    if(!p){
      if(key_count==m)
	continue;
      struct _fom *tail=fom+key_count;
      key_count++;
      tail->idx=i;
      tail->nxt=0;
      if(head){
	tail->prv=head->prv;
	head->prv->nxt=tail;
	head->prv=tail;
      } else
	tail->prv=head=tail;
    } else {
      if(key_count==m){
	struct _fom *crt=head->prv;
	if(crt!=p){
	  crt->prv->next=0;
	  head->prv=ctr->prv;
	  crt->nxt=p;
	  if(p==head){
	    crt->prv=head->prv;
	    head->prv=crt;
	    head=crt;
	  } else {
	    p->prv->nxt=crt;
	    crt->prv=p->prv;
	    p->prv=crt;
	  }
	}
	crt->idx=i;
      } else {
	...
      }
    }
  }
 }
*/    

//====================================================
void accumulate(Vector3D &centerOfGravity, Point3D &point3d, double ampl)
{
  centerOfGravity.x += ampl * point3d.x;
  centerOfGravity.y += ampl * point3d.y;
  centerOfGravity.z += ampl * point3d.z;
}
double dist2line(Point3D &point1, Point3D &point2, Vector3D &vec)
{
  Vector3D vecFromPoints(point1, point2);
  Vector3D vecDiff(vec.y * vecFromPoints.z - vec.z * vecFromPoints.y,
		   vec.z * vecFromPoints.x - vec.x * vecFromPoints.z,
		   vec.x * vecFromPoints.y - vec.y * vecFromPoints.x);
  double output = vecDiff.module()/vec.module();
  return output;
}
Vector3D directionCos(Vector3D &vec)
{
  double tmp = 1./vec.module();
  Vector3D vecOut(vec.x * tmp, vec.y * tmp, vec.z * tmp);
  return vecOut;
}
void initializeSphere(Sphere3D &sphere, double radius, double theta, double phi)
{
  sphere.r = radius;
  sphere.t = theta;
  sphere.p = phi;
}
