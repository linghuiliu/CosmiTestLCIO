/*******************************************************************
*       Cluster finding and analysis                               *
*                                                                  *
*  Hadronic shower analysis using hit density, shower topology     *
*  and cluster shapes to find shower component of different        *
*  physical origin, such as pure tracks, electromagnetic           *
*  cascades inside hadronic shower; and hits that produced by      *
*  neutrons.                                                       *
*                                                                  *
*         Author    V.L. Morgunov                                  *
*         created  22-Jun-2003                                     *
*                                                                  *
*     Last update  V.L.Morgunov     22-Jun-2003                    *
*     Last update  V.L.Morgunov     05-Aug-2003                    *
*     Last update  V.L.Morgunov     13-Sep-2003                    *
*                                                                  *
*     Converted to C     A. Zhelezov      1.2007                   *
*     RS structure       A. Zhelezov      1/2.2007                 *
*                                                                  *
*     Last update  V.L.Morgunov     07-Jan-2007                    *
*     Last update  V.L.Morgunov     28-Feb-2007                    *
*     Last update  V.L.Morgunov     15-Mach-2007                   *
*     Last update  V.L.Morgunov     12-August-2007                 *
*     Last update  V.L.Morgunov     18-September-2007              *
*                                                                  *
*******************************************************************/
#ifndef DEEPANALYSIS_HH
#define DEEPANALYSIS_HH

#include "RecoUtil.hh"
#include "B_Util_DA.h"//renamed due to B_Util.h in MarlinUtil
#include "cernlib_utils.hh"
#include "HistogrammPT.h"

// #ifdef HAVE_CED
// #include <ced_cli.h>
// #endif
//#define DeepAnalysis_DEBUG

//-----------------------------------------------------------------------
/**  Cluster finding and analysis   
    
    Hadronic shower analysis using hit density, shower topology     
    and cluster shapes to find shower component of different        
    physical origin, such as pure tracks, electromagnetic           
    cascades inside hadronic shower; and hits that produced by      
    neutrons.                                                       
                                                                  
    @author  V.L. Morgunov                                  
    @date 18 September 2007
    */
class DeepAnalysis : public RS {
    //-----------------------------------------------------------------------
  public:
    
    /** Class for detector and algorithm parameters
	*/
    class Detector { 
      public:
	double sampling;        /**< thickness of one calorimeter layer [mm]*/
	double cell_size;       /**< cell size [mm]*/
	double ext_cell_size;   /**< used as intermediate cell size [mm]*/
	double neut_thresh;     /**< neutron threshold [MIP] (influents the neutron hits collection efficiency) */
	double normal_thresh;   /**< normal threshold [MIP]*/
	// Changing this value one changes the spherical shell curvature
	double zoom_dist;   /**< distance to look at the event [mm];
				by changing this value one changes the spherical shell curvature;
				should be recalculated using real distance to the first event hit.*/
	double shift;       /**< distance to move event, calculated  [mm]*/
	double a_delta;     /**< bin size for angular histogram [radians].
				Used for EM part of shower only to catch hits around em-shower core*/
	double elips_trans_to_catch;/**<coefficient to increase ellipsoid*/
	double elips_forw_to_catch; /**<coefficient to increase ellipsoid*/
	double elips_back_to_catch; /**<coefficient to increase ellipsoid*/
	double join_factor; /**< coefficient to calculated distance to join in 3-d */
	double sph_cut;     /**< coefficient to convert energy to angular size.
				Used in cluster joining function as cuts.*/
	double cut_3d_em;   /**< criteria for cluster to join  [mm]*/
	double cut_3d_had;  /**< criteria for cluster to join  [mm].
				/Used at the cluster ID definition.*/
	double ecc_trk_hadr_sel;    /**< eccentricity selection cut level*/

	/** Constructor. The parameters are set to as default for HCAL (it is not valid for ECAL):
	    - sampling            = 30.0;
	    - cell_size           = 30.0;
	    - ext_cell_size       = 30.0;
	    - neut_thresh         = 0.6;
	    - normal_thresh       = 0.5;
	    - elips_trans_to_catch= 1.5;
	    - elips_forw_to_catch = 1.5;
	    - elips_back_to_catch = 1.0;
	    - ecc_trk_hadr_sel    = 0.3;
	    - a_delta             = 0.01;
	    - sph_cut             = 0.05; // about 3 degree
	    - cut_3d_em           = 31.0;
	    - cut_3d_had          = 81.0;
	    - join_factor         = 1.0;
	    - zoom_dist           = 200.0.
	    */
	Detector() {
	  sampling            = 30.0;
	  cell_size           = 30.0;
	  ext_cell_size       = 30.0;
	  neut_thresh         = 0.6;
	  normal_thresh       = 0.5;
	  elips_trans_to_catch= 1.5;
	  elips_forw_to_catch = 1.5;
	  elips_back_to_catch = 1.0;
	  ecc_trk_hadr_sel    = 0.3;
	  a_delta             = 0.01;
	  sph_cut             = 0.05; // about 3 degree
	  // Here is should be the calculation of both distances in the adaptive way 
	  // taking into account - sampling distance and tile sizes
	  cut_3d_em           = 31.0;
	  cut_3d_had          = 81.0;
	  join_factor         = 1.0;
	  zoom_dist           = 200.0;
	}

	/** Initialize detector constants and parameters from that has direct input values
	    @param hit_dist
	    */
	void init(double hit_dist)
	{
	  a_delta     = cell_size/zoom_dist; // [radians]
	  shift       = zoom_dist - hit_dist;
	  join_factor = sqrt(cell_size) + 0.01;

#ifdef DeepAnalysis_DEBUG
	  cout<<"=========================================================="<<endl;
	  cout<<"        Detector was initialized with:"<<endl
	      <<"  Sampling = "<<sampling
	      <<", Cell_size = "<<cell_size
	      <<", Ext_cell_size = "<<ext_cell_size<<endl
	      <<"  Normal thresh = "<<normal_thresh
	      <<", Neutr thresh = "<<neut_thresh
	      <<endl;
	  cout<<"        DeepAnalysis was initialized with:"<<endl
	      <<"  Zoom dist. = "<<zoom_dist
	      <<"  Hit dist. "<<hit_dist
	      <<"  Shift = "<<shift<<endl
	      <<", Hist. bin size = "<<a_delta
	      <<", Sph cut. = "<<sph_cut<<"("<<sph_cut*RADDEG<<")"<<endl
	      <<"  3-D cluster to join = "<<join_factor
	      <<", EM cut to join = "<<cut_3d_em
	      <<", HAD cut to join = "<<cut_3d_had
	      <<endl;
	  cout<<"=========================================================="<<endl;
#endif
	}
    };
    Detector detector;

    /** An enumeration -- colors for RGB scheme for hit spectrum
        RED is most dense hit(calorimeter cell)
	BLUE is intermediate
	GREEN is MIP-like, or around the MIP density
	*/
    typedef enum {
      EM=0, /**< former 2 color red*/
      TRK,  /**< former 3 color green*/
      HAD,  /**< former 4 color blue*/
      KIND_COUNT,/**<counter for kinds (types)*/
      NEUTR /**<after count, since it is not KindGroup.*/
    } KIND;

    //-----------------------------------------------------------------------
    /** Class for hit: consists of:
	*/
    class Hit : public RSObj {
      public:
	static const unsigned RS_TYPE = 0;/**<type of RS object */

	unsigned idx;    /**<index in input hits*/
	Point3D  cartesianPoint;     /**< cartesian coordinates*/
	Sphere3D sphere3d;     /**< spherical coordinates*/
	double   amplGeV;     /**< amplitude [GeV] */
	double   ampl_mip; /**< amplitude in MIPs */
	double   thr;    /**< threshold in MIPs */
	unsigned lay;    /**< layer number */
	int addr;        /**< address in initial event array */

	/**Constructor*/
	Hit() : RSObj(RS_TYPE) {}

	//-----------------------------------------------------------------------
	/** Set hit parameters
	    @param _idx index in input hits
	    @param _x x-coordinate
	    @param _y y-coordinate
	    @param _z z-coordinate
	    @param _am amplitude (in GeV)
	    @param _ampl_mip amplitude (in MIPs)
	    @param _thr threshold in MIPs
	    @param _lay layer number
	    @param _addr address in initial event array
	    */
	void set(unsigned _idx,double _x,double _y,double _z,
		 double _am,double _ampl_mip,double _thr,unsigned _lay, int _addr){
	  //-----------------------------------------------------------------------
	  idx =_idx;
	  cartesianPoint.calc(_x,_y,_z);
	  sphere3d.calc(cartesianPoint);
	  amplGeV  = _am;
	  ampl_mip = _ampl_mip;
	  thr      = _thr;
	  lay      = _lay;
	  addr     = _addr;
 	}
	//-----------------------------------------------------------------------
	/** Draw hit
	    @param type hit type
	    @param size size of point/size of cross
	    @param color color n RGB form (e.g. 0xff0000 is RED)
	    */
	void draw(unsigned type, unsigned size, unsigned color){
// #ifdef HAVE_CED
// 	  ced_hit(cartesianPoint.x,
// 		  cartesianPoint.y,
// 		  cartesianPoint.z,
// 		  type, size, color);
// 	  ced_hit(cartesianPoint.x*10.,
// 		  cartesianPoint.y*10.,
// 		  cartesianPoint.z*10.,
// 		  type, size, color);
// #endif
	}
	KIND get_color(DeepAnalysis &rs);
    };
    //-----------------------------------------------------------------------
    /** Class for  general multipurpose grouping
	*/
    class Group : public RSObj { 
      public:
	static const unsigned RS_TYPE = 1;/**<type of RS object*/
	/**Constructor*/
	Group() : RSObj(RS_TYPE) {}
    };

    //-----------------------------------------------------------------------
    /** This class is not a cluster, but this is as it said in name -- Group of Hits,
        that has a boundaries in 3-D cartesian coordinate system and spherical coordinate system,
	center of gravity and whole energy.
	*/
    class HitGroup : public RSObj {
      public:
	static const unsigned RS_TYPE = 2;/**<type of RS object*/
	/**Constructor*/
	HitGroup() : RSObj(RS_TYPE) {}

	Vector3D centerOfGravity;     /**<center of gravity (X_clust..., xs...)*/
	double  energySum;           /**< sum of energy */
	MinMaxRange<int>      layer_g; /**< layer */
	MinMaxRange<double>   radius_g; /**< radius*/
	MinMaxRange<double>   theta_g; /**< theta */
	MinMaxRange<double>   phi_g; /**< phi */
	//-----------------------------------------------------------------------
	/** Calculate initial statistics of this Group of Hits.
	    */
	void calc_stat(){
	  centerOfGravity.calc(0.,0.,0.);
	  energySum = 0.;
	  layer_g.init();
	  radius_g.init();
	  theta_g.init();
	  phi_g.init();

	  RSIterator<Hit> hitIterator(this);
	  while(hitIterator.next()){
	    Hit &hit = *hitIterator;
	    energySum += hit.amplGeV;
	    accumulate(centerOfGravity, hit.cartesianPoint, hit.amplGeV);
	    layer_g.set(hit.lay);
	    radius_g.set(hit.sphere3d.r);
	    theta_g.set(hit.sphere3d.t);
	    phi_g.set(hit.sphere3d.p);
	  }

	  if(energySum!=0.) centerOfGravity.divide(energySum); 
	}
	//-----------------------------------------------------------------------
	/** Draw hits
	    @param type hit type
	    @param size size of point/size of cross
	    @param color color n RGB form (e.g. 0xff0000 is RED)
	    */
	void draw(unsigned type, unsigned size, unsigned color){
	  RSIterator<Hit> hitIterator(this);
	  while(hitIterator.next())
	    hitIterator->draw(type, size, color);
	}
    };

    //-----------------------------------------------------------------------
    /** This class has a represents of RGB types of Groups
	*/
    class KindGroup : public HitGroup {
      public:
	static const unsigned RS_TYPE = 3;/**<type of RS object*/
	/**Constructor */
	KindGroup() : HitGroup() { AddRSType(RS_TYPE); }
    };
    //-----------------------------------------------------------------------
    /** Neutrons group (specific for technical reasons)
	*/
    class NeutGroup : public HitGroup { 
      public:
	static const unsigned RS_TYPE = 4;/**<type of RS object*/
	/**Constructor */
	NeutGroup() : HitGroup() { AddRSType(RS_TYPE); }
    };
    //-----------------------------------------------------------------------
    /**Class for joining groups (juts a working intermediate class) (for @ref cluster_join method)*/
    class JoinGroup : public RSObj { 
      public:
	static const unsigned RS_TYPE = 5;/**<type of RS object*/
	/**Constructor */
	JoinGroup() : RSObj(RS_TYPE) {}
    };
    //-----------------------------------------------------------------------
    /** Cluster is the main object to work with and to keep the main results of the analisys 
	Cluster consists of its properties: like parameters of the inertia tensor, average density,
	whole volume, a few topological Chisq and pointers to first and last hits in cluster.
	*/
    class Cluster : public HitGroup {
      public:
	static const unsigned RS_TYPE = 6;/**<type of RS object*/

	/** An enumeration */
	enum {
	  EMPTY, /**< empty (no hit)*/
	  ONEHIT,/**<contains one hit*/
	  CL2D,  /**<2D cluster*/
	  CHAIN, /**<chain*/
	  JOINED,/**<joined*/
	  EXT    /**<external*/
	} type;

	Sphere3D sp2d; /**< temporary variable, for use in 2D clustering */
	double    e2d; /**< temporary variable, for use in 2D clustering */

	KIND col;        /**< type (M_cl_col)*/
	Vector3D clusterAxisVector;      /**< main principal axis (Cx_clust)*/
	double  clusterRadius;       /**< radius vector length (R_clust)*/
	double  clusterLongestRadius;      /**< longest radius (r1_clust) */
	double  forwRadius; /**< forward radius*/
	double  backwRadius; /**< backward radius*/
	double  r2;      /**< short one*/
	double  r3;      /**< short two*/
	double  ecc;     /**< eccentricity (ecc_clust)*/
	double  ellipAverageRadius;   /**< elipsoid average radius*/
	double  volume;  /**< elipsoid volume */
	double  density; /**< cluster density*/
	double  chi[3];  /**< Chisquare W - quadratic, W linear, W=1 - pure spatial */
	Hit *begn;       /**< address of first cluster point (L_cl_begn)*/
	Hit *last;       /**< address of last cluster point (L_cl_last)*/
	double v1[3],v2[3],v3[3]; /**< elipsoid axis*/

	/**Constructor */
	Cluster() : HitGroup(),type(EMPTY) { AddRSType(RS_TYPE); }
	//-----------------------------------------------------------------------
	/** Set the cluster color depending on the hit and cluster type and colors
	    */
	void calc_color(DeepAnalysis &rs){
	  //-----------------------------------------------------------------------
	  unsigned n[KIND_COUNT], n_neut = 0;
	  memset(n, 0, sizeof(n));

	  for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	    KindGroup *kg = hitIterator->getParent<KindGroup>();

	    for(unsigned i = 0; i<KIND_COUNT; i++)
	      if(kg == rs.kind[i]){
		n[i]++;
		break;
	      }
	    if(hitIterator->getParent<NeutGroup>())
	      n_neut++;
	  }
	  double nnn  = getNumberOf<Hit>();
	  double pmip = double(n[TRK])/nnn;
	  double pem  = double(n[EM])/nnn;
	  double phad = double(n[HAD])/nnn;

	  col = NEUTR;
	  if(pmip >= pem + phad)      col = TRK;
	  if(pem > pmip + phad)       col = EM;
	  if(phad > pmip + pem)       col = HAD;
	  if(nnn <= 4 && n_neut > 0)  col = NEUTR;
	  if(n[EM] >= 3)              col = EM;
	}
	//-----------------------------------------------------------------------
	/** Calculate cluster  tensor of inertia and its principal axes 
	    */
	void inertia_3d(float *wr){ 
	  float a[3][3], zr[3][3], work[20];
	  memset(wr, 0, sizeof(wr));
	  memset(a, 0, sizeof(a));
	  memset(zr, 0, sizeof(zr));
	  float weig = 0.0;

	  // Calculate tensor of inertia
	  for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	    Hit &hit = *hitIterator;
	    //Point3D p=h.cartesianPoint - s;
	    Point3D p(hit.cartesianPoint.x - centerOfGravity.x,
		      hit.cartesianPoint.y - centerOfGravity.y,
		      hit.cartesianPoint.z - centerOfGravity.z);

	    a[0][0] += hit.amplGeV * (p.y * p.y + p.z * p.z);
	    a[1][1] += hit.amplGeV * (p.x * p.x + p.z * p.z);
	    a[2][2] += hit.amplGeV * (p.x * p.x + p.y * p.y);
	    a[0][1] -= hit.amplGeV * (p.x * p.y);
	    a[0][2] -= hit.amplGeV * (p.x * p.z);
	    a[1][2] -= hit.amplGeV * (p.y * p.z);
	    weig += hit.amplGeV;
	  }
	  a[1][0] = a[0][1];
	  a[2][0] = a[0][2];
	  a[2][1] = a[1][2];

	  int ierror = 0;
	  // Eigenvalue problem solve
	  EISRS1(3, 3,(float *)a, wr, (float *)zr, ierror, work);
	  if(ierror) cerr<<" ERROR in INERTIA"<<endl;

	  clusterAxisVector.calc(zr[0][0], zr[1][0], zr[2][0]);
	  double cdir = 1.; 
	  // Correct direction --- should go from the center of coordinates
	  Vector3D vend = centerOfGravity + clusterAxisVector;
	  if(vend.module() - clusterRadius < 0.){
	    clusterAxisVector.calc(-clusterAxisVector.x, -clusterAxisVector.y, -clusterAxisVector.z);
	    cdir = -1.;
	  }

	  for(int i = 0; i < 3 ; i++){
	    v1[i] = cdir * zr[i][0];
	    v2[i] = cdir * zr[i][1];
	    v3[i] = cdir * zr[i][2];
	  }

	  double DIS1 = 0., DIS2 = 0., DIS3 = 0., weig2 = 0.;

	  for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	    Hit &hit = *hitIterator;
	    weig2 += hit.amplGeV * hit.amplGeV;
	    double tmp = dist2line(hit.cartesianPoint, centerOfGravity, clusterAxisVector);

	    DIS1 += (tmp * hit.amplGeV) * (tmp * hit.amplGeV);
	    DIS2 += fabs(tmp) * hit.amplGeV;
	    DIS3 += fabs(tmp);
	  }
	  chi[0] = sqrt(DIS1/weig2);
	  chi[1] = DIS2/weig;
	  chi[2] = DIS3/getNumberOf<Hit>();
	}
	//-----------------------------------------------------------------------
	/** Calculate properties of Cluster
	    */
	void calc_prop(DeepAnalysis &rs, bool for_em=false){
	  calc_stat();
	  calc_color(rs);
	  clusterRadius = centerOfGravity.module();
	  clusterAxisVector.calc(0.,0.,0.);
	  clusterLongestRadius = r2 = r3 = forwRadius = backwRadius = ecc = chi[0] = chi[1] = chi[2] = 0.;
	  begn = last = 0;

	  unsigned nnn = getNumberOf<Hit>();
	  if(nnn > 2){ // cluster could be treated as inertia tensor
	    float wr[3]; //  Inertia tensor EigenValues
	    inertia_3d(wr);

	    //Ellipsoid parameter calculations
	    for(unsigned i = 0; i < 3; i++) {
	      wr[i] = sqrt(wr[i]/energySum);//Normalize inertia tensor eigen values
	    }

	    double chisq = chi[0];
	    clusterLongestRadius = sqrt(wr[1]*wr[2]);
	    r2 = sqrt(wr[0] * wr[2]); // spatial axis length
	    r3 = sqrt(wr[0] * wr[1]); 
	    r2 = r3 = max(max(r2, r3), 0.5 * rs.detector.cell_size);
	    double r_hit_max = -10000.;
	    double d_begn    = 100000.;
	    double d_last    = -100000.;

	    for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	      //Vector3D d=hi->cartesianPoint-s;
	      Vector3D d(hitIterator->cartesianPoint.x - centerOfGravity.x,
			 hitIterator->cartesianPoint.y - centerOfGravity.y,
			 hitIterator->cartesianPoint.z - centerOfGravity.z);

	      double r_max = d.module();
	      if(r_max > r_hit_max) r_hit_max = r_max;

	      float proj = d.dot(clusterAxisVector);
	      if(proj < d_begn){
		d_begn = proj;
		begn   = &hitIterator;
	      }
	      if(proj > d_last){
		d_last = proj;
		last   = &hitIterator;
	      }
	    }
	    forwRadius  = fabs(d_last);
	    backwRadius = fabs(d_begn);
	    //  Calculate other parameters of ellipsoid 
	    volume = 4.* M_PI * clusterLongestRadius * r2 * r3/3.; // ellipsoid volume
	    ellipAverageRadius = pow(volume,0.333333); // average radius
	    density = energySum/volume;  // density [GeV]/[cm]

	    float r_min_ave;
	    r_min_ave = sqrt(r2 * r3/2.0);
	    if(r_min_ave < 0.001) r_min_ave = chisq;
	    if(r_min_ave < 0.001) r_min_ave = 0.5*rs.detector.cell_size;

	    clusterLongestRadius = max(max(forwRadius, backwRadius), clusterLongestRadius);
	    ecc = r_min_ave/clusterLongestRadius; // Cluster Eccentricity

	    if(ecc > 1.0)
	      cout<<" BUG ::: Eccentricity > 1 ; Chisq = "<<chi[0]
		  <<" : "<<clusterLongestRadius
		  <<" : "<<r_min_ave
		  <<" : "<<ecc
		  <<endl;

	  } else if(nnn == 2){ // 2-pt cluster parameters    line in space
	    RSIterator<Hit> hitIterator(this);
	    hitIterator.next();
	    Hit &first = *hitIterator;
	    hitIterator.next();
	    Hit &second = *hitIterator;

	    Vector3D vect(second.cartesianPoint.x - first.cartesianPoint.x,
			  second.cartesianPoint.y - first.cartesianPoint.y,
			  second.cartesianPoint.z - first.cartesianPoint.z);

	    clusterAxisVector.calc(directionCos(vect));


	    Vector3D vend = centerOfGravity + clusterAxisVector; 
	    // Correct direction --- should go from the center of coordinates
	    if(vend.module() - clusterRadius < 0.)
	      clusterAxisVector.calc(-clusterAxisVector.x, -clusterAxisVector.y, -clusterAxisVector.z);
	    volume  = sqrt(vect.length2()) * rs.detector.cell_size * rs.detector.sampling;
	    density = energySum/volume;  // density [GeV]/[cm]

	    // Find first and last points of cluster
	    Vector3D diffVec_first(first.cartesianPoint.x - centerOfGravity.x,
				   first.cartesianPoint.y - centerOfGravity.y,
				   first.cartesianPoint.z - centerOfGravity.z);

	    backwRadius = clusterAxisVector.dot(diffVec_first);

	    Vector3D diffVec_second(second.cartesianPoint.x - centerOfGravity.x,
				    second.cartesianPoint.y - centerOfGravity.y,
				    second.cartesianPoint.z - centerOfGravity.z);

	    backwRadius = clusterAxisVector.dot(diffVec_second);
	
	    begn = &first;
	    last = &second;

	    if(backwRadius > forwRadius){
	      double tmp  = backwRadius;
	      backwRadius = forwRadius;
	      forwRadius  = tmp;
	      last = &first;
	      begn = &second;
	      last = &first;
	    }

	    chi[0] = chi[1] = chi[2] = 0.5 * rs.detector.cell_size;

	  } else if(nnn == 1){ // 1-pt cluster parameters  point in space
	    //  vol = length * tile_area * sampling (latter to do)
	    begn = last = getChild<Hit>();
	  } else
	    cerr<<"Empty cluster?"<<endl;
	}
	//-----------------------------------------------------------------------
	/** Finalize: mainly works with the rest of the hits that do not belongs to any cluster
	    and also recolored some of the clusters.
	    */
	void finalize(DeepAnalysis &rs){
	  unsigned nnn = getNumberOf<Hit>();

	  if(nnn > 2){
	    if(col != EM)
	      // Cluster color (Green or Blue) is take from cluster eccentricity
	      col = (ecc < rs.detector.ecc_trk_hadr_sel)? TRK : HAD;

	    if(nnn < 4){
	      for(RSIterator<Hit> hitIterator(this); hitIterator.next();)
		if(hitIterator->getParent<NeutGroup>()){
		  col = NEUTR;
		  break;
		}
	    }

	    for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	      Hit &hit =*hitIterator;
	      KindGroup *hcol = hit.getParent<KindGroup>();

	      if(col == NEUTR){
		rs.unlink(&hit, hcol);
		if(!hit.getParent<NeutGroup>())
		  rs.append(&hit, rs.neutrons);
	      } else {
		rs.unlink(&hit, rs.neutrons);
		if(hcol != rs.kind[col]){
		  rs.unlink(&hit, hcol);
		  rs.append(&hit, rs.kind[col]);
		}
	      }
	    }

	  } else { // destroy cluster with number of hits less than 3
	    for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	      Hit &hit = *hitIterator;
	      rs.unlink(&hit, hit.getParent<KindGroup>());
	      if(!hit.getParent<NeutGroup>())
		rs.append(&hit, rs.neutrons);
	    }
	    rs.unlink(this); // forget it
	  }
	}
	//-----------------------------------------------------------------------
	/**Rotate 3D point
	    @param point 3D point where to rotate
	    */
	void rotate_point(Point3D &point){
	  float rx = point.x * v1[0] + point.y * v2[0] + point.z * v3[0] + centerOfGravity.x;
	  float ry = point.x * v1[1] + point.y * v2[1] + point.z * v3[1] + centerOfGravity.y;
	  float rz = point.x * v1[2] + point.y * v2[2] + point.z * v3[2] + centerOfGravity.z;
	  point.x = rx;
	  point.y = ry;
	  point.z = rz;
	}
	//-----------------------------------------------------------------------
	/**Print out info
	    */
	void print() {
	  static const char *tname[] = { "EMPTY","ONEHIT","2D","CHAIN","JOINED","EXT" };
	  static const char *cname[] = { "EM","TRK","HAD","","NEUT" };
	  cout<<" Cluster ("<<tname[type]<<") : ";
	  cout<<cname[col]<<" "<<getNumberOf<Hit>()<<" E = "<<energySum;
	  cout<<endl;
	}
	//-----------------------------------------------------------------------
	/** Draw line between 2 points
	    */
	void line(const Point3D &p0, const Point3D &p1,
		  unsigned type, unsigned size, unsigned color){
// #ifdef HAVE_CED
// 	  ced_line(p0.x, p0.y, p0.z,
// 		   p1.x, p1.y, p1.z,
// 		   type, size, color);
// #endif
	}
	//-----------------------------------------------------------------------
	/** Draw star 
	    */
	void draw_star(unsigned type, unsigned color){
// #ifdef HAVE_CED
// 	  ced_hit(centerOfGravity.x, centerOfGravity.y, centerOfGravity.z, 
// 		  1<<CED_LAYER_SHIFT, 4, 0xd1cd04);
// #endif
	  for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	    Hit &hit = *hitIterator;
	    line(hit.cartesianPoint, centerOfGravity, type, 1, color);
	  }
	}
	//-----------------------------------------------------------------------
	/** Draw tree...
	    */
	void draw_tree(unsigned type,unsigned color){
// #ifdef HAVE_CED
// 	  ced_hit(begn->cartesianPoint.x, begn->cartesianPoint.y, begn->cartesianPoint.z,
// 		  1<<CED_LAYER_SHIFT, 4, 0xd1cd04);
// #endif
	  for(RSIterator<Hit> hitIterator(this); hitIterator.next();){
	    Hit &hit = *hitIterator;
	    line(hit.cartesianPoint, begn->cartesianPoint, type, 1, color);
	  }
	}
	//-----------------------------------------------------------------------
	/** Draw ellipsoid
	    */
	void draw_ellipse(unsigned type, unsigned color){
	  static bool init_done = false;
	  static float GmSIN[41], GmCOS[41];

	  if(!init_done){
	    init_done  = true;
	    float DPHI = M_PI/20.;
	    float PHI  = 0.;
	    for(int i = 0; i < 40; i++, PHI+=DPHI){
	      GmSIN[i] = sin(PHI);
	      GmCOS[i] = cos(PHI);
	    }
	    GmSIN[40] = GmSIN[0];
	    GmCOS[40] = GmCOS[0];
	  }

	  if( hypot3(forwRadius, r2, r3) < 0.001)
	    return; // AZ: too small
	  // Draw v1is
	  Point3D p0(centerOfGravity.x + forwRadius * v1[0],
		     centerOfGravity.y + forwRadius * v1[1],
		     centerOfGravity.z + forwRadius * v1[2]);
	  line(centerOfGravity, p0, type, 1, color);

	  Point3D p1(centerOfGravity.x - backwRadius * v1[0],
		     centerOfGravity.y - backwRadius * v1[1],
		     centerOfGravity.z - backwRadius * v1[2]);
	  line(centerOfGravity, p1, type, 1, color);

	  Point3D p2(centerOfGravity.x + r2 * v2[0],
		     centerOfGravity.y + r2 * v2[1],
		     centerOfGravity.z + r2 * v2[2]);

	  Point3D p3(centerOfGravity.x - r2 * v2[0],
		     centerOfGravity.y - r2 * v2[1],
		     centerOfGravity.z - r2 * v2[2]);
	  line(p2, p3, type, 1, color);

	  Point3D p4(centerOfGravity.x + r3 * v3[0],
		     centerOfGravity.y + r3 * v3[1],
		     centerOfGravity.z + r3 * v3[2]);

	  Point3D p5(centerOfGravity.x - r3 * v3[0],
		     centerOfGravity.y - r3 * v3[1],
		     centerOfGravity.z - r3 * v3[2]);
	  line(p4, p5, type, 2, color);

	  Point3D a[41];
	  for(int i = 0; i < 41; i++){
	    a[i].calc(0., r2 * GmCOS[i], r3 * GmSIN[i]);
	    rotate_point(a[i]);
	  }

	  for(int i = 0; i < 40; i++)
	    line(a[i], a[i+1], type, 1, color);
      
	  for(int i = 0; i < 41; i++){
	    a[i].calc((GmCOS[i]>0.?forwRadius:backwRadius) * GmCOS[i], 0., r3 * GmSIN[i]);
	    rotate_point(a[i]);
	  }

	  for(int i = 0; i < 40; i++)
	    line(a[i], a[i+1], type, 1, color);
      
	  for(int i = 0;i < 41; i++){
	    a[i].calc((GmCOS[i]>0.?forwRadius:backwRadius) * GmCOS[i], r2 * GmSIN[i], 0.);
	    rotate_point(a[i]);
	  }

	  for(int i = 0; i < 40; i++)
	    line(a[i], a[i+1], type, 1, color);
	}
    }; //end of class Cluster


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //    Create a main structure for reconstruction
    //-----------------------------------------------------------------------
    /**     All main pointer to structures */
    //-----------------------------------------------------------------------
    HitGroup  *all_hits;/**<hit group*/
    Group     *clusters;/**<clusters*/
    Group     *joingroups;/**<joined groups*/
    KindGroup *kind[KIND_COUNT];/**<hit type*/
    NeutGroup *neutrons;/**<neutrons group*/

    unsigned  all_hits_count;/**<counter for all hits*/

    //-----------------------------------------------------------------------
    /** Constructor
	*/
    DeepAnalysis() : RS() {
      all_hits_count = 0;
      all_hits       = alloc<HitGroup>();
      clusters       = alloc<Group>();
      joingroups     = alloc<Group>();

      for(unsigned i = 0; i < KIND_COUNT; i++){
	kind[i] = alloc<KindGroup>();
      }
      neutrons = alloc<NeutGroup>();
    }
    //-----------------------------------------------------------------------
    /** Create clusters from single hits
	*/
    void cluster_from_singles(KindGroup &gr, bool mark_neutrons){
      RSIterator<Hit> hitIterator(&gr);

      while(hitIterator.next()){
	if(hitIterator->getParent<Cluster>()) continue;

	Cluster *cl = alloc<Cluster>();
	cl->type = Cluster::ONEHIT;
	append(cl, clusters);
	append(&hitIterator, cl);

	if(mark_neutrons) append(&hitIterator, neutrons);
      }	
    }
    //-----------------------------------------------------------------------
    /** Distribute hits at the spherical shell onto found 2D cluster seeds
	*/
    void cluster_finder_2d_split(Hit &hit, unsigned InClust_count, Cluster **InClust)
    {
      float xx = rndm(0);
      float rr = 0;

      for(unsigned i = 0; i < InClust_count; i++){
	rr += InClust[i]->e2d;
      }

      float pos = 0;
      for(unsigned i = 0; i < InClust_count; i++){
	pos += InClust[i]->e2d;

	if(xx <= pos/rr){
	  append(&hit, InClust[i]);
	  break;
	}
      }
    }
    //-----------------------------------------------------------------------
    /** Special 2D histograms
	*/
    class SpecialHist2D : public HistogrammPT {
      public:
	/**Book histogram*/
	virtual bool Book(const MinMaxRange<double> &p_lim,
			  const MinMaxRange<double> &t_lim,
			  double _step=0.006){
	  // AZ: need it to change _step idea, for backward compat...
	  int np = int(p_lim.range()/_step);
	  Put2Range(np, 5, 360);

	  int nt = int(t_lim.range()/_step);
	  Put2Range(nt, 5, 180);

	  if( !p.set(p_lim, p_lim.range()/np) || !t.set(t_lim, t_lim.range()/nt) )
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
    };
    //-----------------------------------------------------------------------
    /** 2D clustering procedure to find clusters by histogramming technique  
       at the one spherical shell in Theta-Phi.
	*/
    void cluster_finder_2d(HitGroup &hitGroup, Group &ring)
    {
      unsigned nht = hitGroup.getNumberOf<Hit>();
      if(!nht) return;   // no hits at all

      if(nht == 1){ // one hit - one cluster
	Cluster *cl = alloc<Cluster>();
	append(hitGroup.getChild<Hit>(), cl);
	cl->calc_stat();
	cl->type=Cluster::ONEHIT;
	append(cl,&ring);
	return;
      }

      hitGroup.calc_stat();
      hitGroup.phi_g.extend(0.0001); //  To be sure that all hits are in interval
      hitGroup.theta_g.extend(0.0001);
      SpecialHist2D hst;
      hst.Book(hitGroup.phi_g, hitGroup.theta_g, detector.a_delta); // in radians

#ifdef DeepAnalysis_DEBUG
      cout<<"  Cl_2d : "<<nht<<" hits"
	  <<", Phi "<<hitGroup.phi_g.min<<" : "<<hitGroup.phi_g.max<<"/"<<hst.p.n
	  <<", Theta "<<hitGroup.theta_g.min<<" : "<<hitGroup.theta_g.max<<"/"<<hst.t.n
	  <<", delta = "<<detector.a_delta<<"("<<detector.a_delta*RADDEG<<")"
	  <<endl;
#endif
      double min_r_max = max(hst.p.step, hst.t.step)/sqrt(2.) + 0.00001;

      for(RSIterator<Hit> hitIterator(&hitGroup); hitIterator.next();)
	hst.Fill(hitIterator->sphere3d, hitIterator->amplGeV);
      /*
	  c======================================================================
	  c         Most tricky part of the clustering algorithm;
	  c  calculation of angular energy [MeV] radius for cluster center candidate
	  c  it takes into account the exponential shower shape in radius
	  c  This is equivalent to some cone parameters in usual cone cluster finder
	  c         Should be tuned very carefully
	  c======================================================================
	  c      DATA    sph_cutE_sup /3.0/, coeff_radE_sup /1.0/
	  c======================================================================
	  */
      double sph_cut1 = 1.1 * detector.sph_cut;
      double summ_log = log(1000. * hitGroup.energySum);// Logarithm of whole energy in MeV
      int nmax = min(nht, hst.c_count);
      unsigned adr[nmax]; // indexes of sorted cells


      // Sort histogram by bin amplitudes
      BiggestMfromN(hst.c_count, hst.c, nmax,(HistogrammPT::HCell *)0, adr);

      for(int i = 0; i < nmax; i++){
	HistogrammPT::HCell &c = hst.c[adr[i]];

	if(c.val == 0.) break; // because it was sorted...

	// calculation of angular energy [MeV] radius for cluster center candidate
	double r_max = detector.sph_cut * log(1000. * c.val)/summ_log;
	if(r_max <= min_r_max) r_max=min_r_max;

	// Find the unique cluster centers taking into account shower model, 
	//  if distance to the next amplitude  >  cut parameter
	bool found = false;
	for(RSIterator<Cluster> clusterIterator(&ring); clusterIterator.next();){
	  Cluster &cl = *clusterIterator;

	  if( (fabs(hst.get_t(c) - cl.sp2d.t) < sph_cut1) && 
	      (angle_dist(hst.get_t(c), hst.get_p(c), cl.sp2d.t, cl.sp2d.p) 
	       <= cl.sp2d.r)){
	    found = true;
	    break;
	  }
	}
	if(found) continue;

	Cluster *cl = alloc<Cluster>();
	initializeSphere(cl->sp2d, r_max, hst.get_t(c), hst.get_p(c));

	cl->e2d = c.val;
	append(cl, &ring);
      }

      //  Energy of cluster center is at the cl.e2d;  Theta and Phi are at cl.sp
      //  Cluster predicted raius is at the cl.sp2d.r;  Theta and Phi are at cl.sp
      Cluster *InClust[ring.getNumberOf<Cluster>()];

      //  Assign  hits to cluster with weight = amplitude of MAX
      for(RSIterator<Hit> hitIterator(&hitGroup); hitIterator.next();){ 
	Hit &hit = *hitIterator;

	unsigned InClust_count=0;
	for(RSIterator<Cluster> clusterIterator(&ring); clusterIterator.next();){
	  Cluster &cl = *clusterIterator;
	  if((fabs(hit.sphere3d.t - cl.sp2d.t) < sph_cut1) && 
	     (angle_dist(hit.sphere3d.t, hit.sphere3d.p, cl.sp2d.t, cl.sp2d.p) <= cl.sp2d.r))
	    InClust[InClust_count++] = &clusterIterator;
	} 
	if(!InClust_count) continue;     // not in cluster  
    
	if(InClust_count==1)  append(&hitIterator,InClust[0]);
	else // several candidates
	  cluster_finder_2d_split(hit, InClust_count, InClust);
      } //  End loop along hits

      // remove empty clusters and calculate stats
      for(RSIterator<Cluster> clusterIterator(&ring); clusterIterator.next();){ 
	unsigned nht = clusterIterator->getNumberOf<Hit>();
	if(!nht) unlink(&clusterIterator);
	else {
	  clusterIterator->calc_stat();
	  if(nht == 1)
	    clusterIterator->type = Cluster::ONEHIT;
	  else
	    clusterIterator->type=Cluster::CL2D;
	}
      }

      // there is one special case ...
      for(RSIterator<Hit> hitIterator(&hitGroup); hitIterator.next();){ 
	if(hitIterator->getParent<Cluster>()) continue;

	Cluster *cl = alloc<Cluster>();
	append(cl, &ring);

	cl->type = Cluster::ONEHIT;
	append(&hitIterator, cl);
	cl->calc_stat();
      }

    }
    //-----------------------------------------------------------------------
    /** Find 3D clusters. Method is: to create spherilal shells in the event volume,
	make 2D clustering at each shell, then connect near by 2D clusters into 3D trees.
	*/
    void cluster_finder_3d(KindGroup &gr, bool mark_neutrons){
      //-----------------------------------------------------------------------
      if(gr.getNumberOf<Hit>() < 3){
	cluster_from_singles(gr, mark_neutrons);
	return;
      }

      unsigned color = 0xf91111;
      if(&gr == kind[TRK]) color = 0x029102;
      if(&gr == kind[HAD]) color = 0x03abc1;

      gr.calc_stat(); // Calculate spherical divisions
      unsigned nradius_g = gr.layer_g.range(); // For the simplest case without overlaping
      if(!nradius_g)
	nradius_g = 1; // AZ: protection against 0!
      // Estimation of sampling step
      double a_sampl = gr.radius_g.range()/nradius_g; 
      nradius_g++;
      if(nradius_g < 2)
	nradius_g = 2;
      //  R-step should be close to the sampling step
      //    double dr = detector.sampling+(gr.radius_g.range() + a_sampl)/nradius_g; 
      double dr = (gr.radius_g.range() + a_sampl)/nradius_g; 
      if(dr < detector.sampling)
	dr = detector.sampling;
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //    R_parameter is very critical for the cluster joining
      //      It depends on the calorimeter sampling structure
      //              as well as on the cell sizes
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      float R_join = detector.join_factor*dr;
      if(R_join < detector.join_factor*detector.ext_cell_size)
	R_join = detector.join_factor*detector.ext_cell_size;
      Range<double> rr(gr.radius_g.min - 0.5*a_sampl);
      rr.max += dr;
      rr.extend(1.e-5);
 
      Group *rings = new Group[nradius_g];
      HitGroup hg;

      // Loop along radial shells
      for(unsigned j = 0; j < nradius_g; j++, rr.min += dr, rr.max += dr){ 
	RSIterator<Hit> hitIterator(&gr);
	while(hitIterator.next()){
	  if(rr.inrange(hitIterator->sphere3d.r) && !hitIterator->getParent<Cluster>())
	    append(&hitIterator, &hg);
	}

	cluster_finder_2d(hg, rings[j]);
	unlink(&hg); // AZ: cleanup ring and prepare for next iteration
      }// End loop along radial shells

      // AZ: Original algo minimized distances first. If A->B and C->B,
      //     than A->B wins. And while C will start new chain, it will
      //     be alone since B is already taken.
      //     Note: C will be marked as a chain (while still alone)
      // !!!! Bug in original code generate another result....

      for(unsigned j = 0; j < nradius_g - 1; j++){ // AZ: Prepare for sort...
	unsigned max_cand_count=
	  rings[j].getNumberOf<Cluster>()*rings[j+1].getNumberOf<Cluster>();
	unsigned cand_count = 0;
	Cluster *cand[max_cand_count][2];
	double dist[max_cand_count];

	for(RSIterator<Cluster> baseRing(rings+j); baseRing.next();) // base ring
	  for(RSIterator<Cluster> nextRing(rings+j+1); nextRing.next();){ // next ring
	    double d = baseRing->centerOfGravity.dist(nextRing->centerOfGravity);

	    if( d < R_join){
	      dist[cand_count]      = d;
	      cand[cand_count][0]   = &baseRing; // collect possible candidates
	      cand[cand_count++][1] = &nextRing;
	    }
	  } //next ring  as well as a base ring also finished here

	unsigned scand[cand_count];
	SmallestMfromN(cand_count, dist, cand_count, (double *)0, scand);

	for(unsigned i = 0; i < cand_count; i++){
	  Cluster *baseRing = cand[scand[i]][0];
	  Cluster *nextRing = cand[scand[i]][1];

// #ifdef HAVE_CED
// 	  //draw lines (change coordinate system back before drawing)
// 	  double x1 = baseRing->centerOfGravity.z;
// 	  double y1 = -baseRing->centerOfGravity.y;
// 	  double z1 = baseRing->centerOfGravity.x + detector.shift;
// 	  double x2 = nextRing->centerOfGravity.z;
// 	  double y2 = -nextRing->centerOfGravity.y;
// 	  double z2 = nextRing->centerOfGravity.x + detector.shift;
// #endif
	
	  // Some cluster is already in some chain
	  if(baseRing->type == Cluster::EMPTY || nextRing->type == Cluster::CHAIN){ 
// #ifdef HAVE_CED
// 	    ced_line(x1, y1, z1, x2, y2, z2, 16<<CED_LAYER_SHIFT, 1, 0x565501); //draw lines 
// #endif
	    continue;
	  }
// #ifdef HAVE_CED
// 	  ced_line(x1, y1, z1, x2, y2, z2, 6<<CED_LAYER_SHIFT, 2, color); //draw lines 
// 	  ced_hit(x1, y1, z1, 6<<CED_LAYER_SHIFT, 1, 0xf7f43b); //draw lines 
// #endif

	  // Chain clusters by moving hits, !!! not recalc stats !!!
	  baseRing->type = Cluster::EMPTY; 
	  nextRing->type = Cluster::CHAIN;

	  for(RSIterator<Hit> hitIterator(baseRing); hitIterator.next();){
	    append(&hitIterator, nextRing);
	  }
	  unlink(baseRing); // forget it
	} // loop along candidates
      }   // loop alog all spherical shells

      int n_one  = 0;
      int n_many = 0;
      int n_hits = 0;

      for(unsigned j = 0; j < nradius_g; j++){ // collect results
	for(RSIterator<Cluster> clusterIterator(rings+j); clusterIterator.next();){
	  if(clusterIterator->type != Cluster::CHAIN && mark_neutrons){
	    for(RSIterator<Hit> hitIterator(&clusterIterator); hitIterator.next();){
	      Cluster *newcl = alloc<Cluster>();
	      append(&hitIterator, newcl);
	      newcl->type = Cluster::ONEHIT;
	      append(newcl, clusters);
	      newcl->calc_prop(*this, false);
	      n_one++;
	    }
	    unlink(&clusterIterator);

	  } else {
	    n_many++;
	    n_hits += clusterIterator->getNumberOf<Hit>();
	    append(&clusterIterator, clusters);
	    clusterIterator->calc_prop(*this, !mark_neutrons);
	  }
	}
	unlink(rings+j);
      }
      delete [] rings;
    }
    //-----------------------------------------------------------------------
    /** Technical function used in cluster_join
	*/
    void join_group(Cluster *cl,JoinGroup *jg){
      JoinGroup *cjg = cl->getParent<JoinGroup>();

      if(!cjg){
	append(cl, jg);
	return;
      } else if(cjg == jg)
	return;
      for(RSIterator<Cluster> clusterIterator(cjg); clusterIterator.next();) // move cjg to jg
	append(&clusterIterator,jg);
      unlink(cjg); // forget it
    }
    //-----------------------------------------------------------------------
    /** Technical function used in cluster_join
	*/
    void make_join_group(Cluster *cl0,Cluster *cl1,Cluster *cl2=0){
      JoinGroup *jg = cl0->getParent<JoinGroup>();

      if(!jg) 
	jg = cl1->getParent<JoinGroup>();
      if(!jg && cl2)
	jg = cl2->getParent<JoinGroup>();
      if(!jg){
	jg = alloc<JoinGroup>();
	append(jg, joingroups);
      }
      join_group(cl0, jg);
      join_group(cl1, jg);

      if(cl2)
	join_group(cl2, jg);
    }
    //-----------------------------------------------------------------------
    /** Join clusters together
	*/
    void cluster_join(bool for_em){
      // Collect a few clusters together
      double reason_dist = detector.cut_3d_em;
      if(!for_em){
	reason_dist = detector.cut_3d_had;
      }

      unsigned color = 0xf91111;

      // AZ:!!! Method is rewritten. I hope logic is still the same.
      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){
	if(clusterIterator->type == Cluster::ONEHIT || 
	   clusterIterator->ecc >= (for_em?0.35:0.45))//  Parameter ???
	  continue;

	// Can be compared only em with em or had and mip both together
	if(!for_em && clusterIterator->col != TRK && clusterIterator->col != HAD) // for track-like only
	  continue;

	for(RSIterator<Cluster> clusterIterator2(clusters); clusterIterator2.next();){
	  if(&clusterIterator == &clusterIterator2)
	    continue;

	  if(clusterIterator2->type == Cluster::ONEHIT || 
	     clusterIterator2->ecc >= (for_em?0.35:0.45))//  Parameter ???
	    continue;

	  if(!for_em && clusterIterator2->col != TRK && clusterIterator2->col != HAD) // for track-like only
	    continue;

	  if(!clusterIterator->begn || !clusterIterator2->last){
	    cerr<<"BUG: properties are not calculated... found in cluster join"
		<<endl;
	    continue;
	  }

	  Hit &firstHit = *clusterIterator->begn;
	  Hit &lastHit = *clusterIterator2->last;

	  Vector3D vect(lastHit.cartesianPoint.x - firstHit.cartesianPoint.x,
			lastHit.cartesianPoint.y - firstHit.cartesianPoint.y,
			lastHit.cartesianPoint.z - firstHit.cartesianPoint.z);

// #ifdef HAVE_CED
// 	  //draw lines (change coordinate system back before drawing)
// 	  double x1 =  firstHit.cartesianPoint.z;
// 	  double y1 = -firstHit.cartesianPoint.y;
// 	  double z1 =  firstHit.cartesianPoint.x + detector.shift;
// 	  double x2 =  lastHit.cartesianPoint.z;
// 	  double y2 = -lastHit.cartesianPoint.y;
// 	  double z2 =  lastHit.cartesianPoint.x + detector.shift;

// 	  ced_line(x1, y1, z1, x2, y2, z2, 17<<CED_LAYER_SHIFT, 1, 0x565501);
// 	  ced_hit(x1, y1, z1, 6<<CED_LAYER_SHIFT, 1, 0xf7f43b);
// #endif

	  Vector3D dirco = directionCos(vect);
	  double dcheck = vect.module();

	  if(dcheck >= reason_dist)
	    continue; 	

	  //draw lines 
	  if(clusterIterator->col == TRK && clusterIterator2->col == TRK ) color = 0x029102;
	  if(clusterIterator->col == HAD && clusterIterator2->col == HAD ) color = 0x029102;
	  if(clusterIterator->col == HAD && clusterIterator2->col == TRK ) color = 0x0404ef;
	  if(clusterIterator->col == TRK && clusterIterator2->col == HAD ) color = 0x0404ef;

// #ifdef HAVE_CED
// 	  ced_line(x1, y1, z1, x2, y2, z2, 7<<CED_LAYER_SHIFT, 2, color);
// #endif
	  double rsp = 0.5 * dcheck;
	  Vector3D sp = firstHit.cartesianPoint + lastHit.cartesianPoint;
	  sp.mult(0.5);

	  // Find clusters joined without intermediate hit
	  //  Parameter ???
	  if(dcheck < (for_em?2.0:2.5) * detector.ext_cell_size || 
	     dcheck < (for_em?2.0:2.5) * detector.sampling)
	    make_join_group(&clusterIterator, &clusterIterator2);  // These clusters should be joined

	  // Find one-hit cluster that is in between of two others
	  for(RSIterator<Cluster> clusterIterator3(clusters); clusterIterator3.next();){
	    if(clusterIterator3->type != Cluster::ONEHIT)
	      continue; // One hit cluster will be joined  ONLY

	    double dhit = dist2line(clusterIterator3->centerOfGravity, sp, dirco);
	    Vector3D rv(sp, clusterIterator3->centerOfGravity);
	    float shit = rv.module();

	    //  Parameter ???
	    if(dhit < 2.5 * detector.ext_cell_size && shit < rsp)
	      //  All these clusters should be joined
	      make_join_group(&clusterIterator, &clusterIterator2, &clusterIterator3); 
	  }
	}
      }

      // Now it's time to really join clusters...
      for(RSIterator<JoinGroup> ji(joingroups); ji.next();){
	Cluster *bc = 0; // cluster to join into

	for(RSIterator<Cluster> clusterIterator(&ji); clusterIterator.next();){
	  if(!bc)
	    bc = &clusterIterator;
	  else {
	    for(RSIterator<Hit> hitIterator(&clusterIterator); hitIterator.next();)
	      append(&hitIterator, bc);
	    unlink(&clusterIterator); // forget about cluster (so hits will be also unlinked)
	  }
	}
	bc->type = Cluster::JOINED;
	bc->calc_prop(*this);
	unlink(&ji);
      }
      unlink(joingroups);
    }

    //-----------------------------------------------------------------------
    /** Find electromagnetic shower: it collects additional (not RED) hits around the EM cluster core.
	*/
    void em_shower_find(){
      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){
	if(clusterIterator->getNumberOf<Hit>() < 3) //skip small clusters
	  continue;

	unsigned found = 0;
	for(RSIterator<Hit> hitIterator(all_hits); hitIterator.next();){
	  Cluster *pcl = hitIterator->getParent<Cluster>();

	  if(pcl && (pcl == &clusterIterator || pcl->getNumberOf<Hit>() > 4))// was 2
	    continue; // skip hits from this cluster and skip "big" clusters
	  // AZ:??? For 2 hits clusters one hit can left out...

	  Vector3D d(hitIterator->cartesianPoint.x - clusterIterator->centerOfGravity.x,
		     hitIterator->cartesianPoint.y - clusterIterator->centerOfGravity.y,
		     hitIterator->cartesianPoint.z - clusterIterator->centerOfGravity.z);

	  float rx =  d.x * clusterIterator->v1[0] + d.y * clusterIterator->v1[1] + d.z * clusterIterator->v1[2];
	  float ry =  d.x * clusterIterator->v2[0] + d.y * clusterIterator->v2[1] + d.z * clusterIterator->v2[2];
	  float rz =  d.x * clusterIterator->v3[0] + d.y * clusterIterator->v3[1] + d.z * clusterIterator->v3[2];

	  float proj= d.dot(clusterIterator->clusterAxisVector);
	  float rr = (proj >=0.)?
	    clusterIterator->forwRadius*detector.elips_forw_to_catch : 
	    clusterIterator->backwRadius*detector.elips_back_to_catch;

	  if(clusterIterator->r2 < 0.00000001 || clusterIterator->r3 < 0.000000001)
	    continue;

	  float rt = max(clusterIterator->r2, clusterIterator->r3) * detector.elips_trans_to_catch;

	  float rrr =  (rx/rr) * (rx/rr) + (ry/rt) * (ry/rt) + (rz/rt) * (rz/rt);
	  if(rrr >= 1.)
	    continue; // not inside

	  found++;
	  append(&hitIterator, &clusterIterator);

	  if(pcl){
	    unlink(&hitIterator, pcl);
	    unsigned n = pcl->getNumberOf<Hit>();
	    if(!n){
	      // unlink(pcl); // can't do here...
	      pcl->type = Cluster::EMPTY;
	    } else { // n==1
	      pcl->type = Cluster::ONEHIT;
	      pcl->calc_prop(*this);
	    }
	  }

	  if(clusterIterator->col == NEUTR){ // hit must change the color
	    unlink(&hitIterator, hitIterator->getParent<KindGroup>());
	    append(&hitIterator, neutrons);
	  } else {
	    unlink(&hitIterator, neutrons);
	    if(hitIterator->getParent<KindGroup>() != kind[clusterIterator->col]){
	      unlink(&hitIterator, hitIterator->getParent<KindGroup>());
	      append(&hitIterator, kind[clusterIterator->col]);
	    }
	  } 
	}

	if(found){
	  clusterIterator->type = Cluster::EXT;
	  clusterIterator->calc_prop(*this);
	}
      }  // along all clusters

      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){
	if(clusterIterator->type == Cluster::EMPTY)
	  unlink(&clusterIterator);
      }
    }
    //-----------------------------------------------------------------------
    /** Reassign small clusters
	*/
    void reassignment_small_clusters(KIND k)
    {
      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){ 
	if(clusterIterator->getNumberOf<Hit>() > 2)
	  continue;

	for(RSIterator<Hit> hitIterator(&clusterIterator); hitIterator.next();){
	  unlink(&hitIterator, neutrons);
	  unlink(&hitIterator, hitIterator->getParent<KindGroup>());
	  append(&hitIterator, kind[k]);
	}
	unlink(&clusterIterator); // forget it
      }
    }
    //-----------------------------------------------------------------------
    /** Clean low energy clusters
	*/
    void low_e_clean()
    {
      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){
	bool found = false;
	
	for(RSIterator<Hit> hitIterator(&clusterIterator); hitIterator.next();){
	  double dthresh = 0.0;

	  if(clusterIterator->col == NEUTR)
	    dthresh = 0.10;

	  if(hitIterator->ampl_mip < hitIterator->thr + dthresh){
	    found = true;
	    unlink(&hitIterator);
	  }

	  if(found){
	    if(!clusterIterator->getChild<Hit>())
	      unlink(&clusterIterator);
	    else
	      clusterIterator->calc_prop(*this);
	  }
	}
      }
    }
    //-----------------------------------------------------------------------
    /** Create Hit object
	*/
    Hit *add_hit(double _x,double _y,double _z,
		 double _am,double _ampl_mip,double _thr,
		 unsigned _lay, KIND type,int _addr)
    {
      if(type >= KIND_COUNT || type < 0){
	cerr<<"BUG: Wrong type in add_hit"<<endl;
	return 0;
      }

      Hit *hit = alloc<Hit>();
      hit->set(all_hits_count++, _x, _y, _z, _am, _ampl_mip, _thr, _lay, _addr);
      append(hit, all_hits);
      append(hit, kind[type]);
      return hit;
    }
    //-----------------------------------------------------------------------
    /** Draw ??
	*/
    void draw()
    {
// #ifdef HAVE_CED
//       static const unsigned kcolor[]={ 
// 	0xff0000, // EM    -> red
// 	0x00ff00, // TRK   -> green
// 	0x00beff, // HAD   -> light blue
// 	0x000000, // unused
// 	0xffffff  // NEUTR -> white
//       };
//       //-----------------------------------------------------------------------
//       //     Cluster and hits drawing and
//       //  just to show how to extract the results
//       //-----------------------------------------------------------------------
// #define ORIGINAL_LAYER (1<<CED_LAYER_SHIFT)

//       for(RSIterator<Hit> hitIterator(all_hits); hitIterator.next();)
// 	hitIterator->draw(ORIGINAL_LAYER, 5, kcolor[hitIterator->get_color(*this)]);  
  
//       for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();)
// 	clusterIterator->draw_ellipse(3<<CED_LAYER_SHIFT, kcolor[clusterIterator->col]);
// #endif
    }
    //-----------------------------------------------------------------------
    /** Draw ??
	*/
    void draw_to_layer(unsigned layer)
    {
// #ifdef HAVE_CED
//       static const unsigned kcolor[]={ 
// 	0xff0000, // EM    -> red
// 	0x00ff00, // TRK   -> green
// 	0x00beff, // HAD   -> light blue
// 	0x000000, // unused
// 	0xffffff  // NEUTR -> white
//       };

//       for(RSIterator<Hit> hitIterator(all_hits); hitIterator.next();)
// 	hitIterator->draw(layer<<CED_LAYER_SHIFT, 3, kcolor[hitIterator->get_color(*this)]);

//       for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){
// 	clusterIterator->draw_ellipse(3<<CED_LAYER_SHIFT, kcolor[clusterIterator->col]);
// 	clusterIterator->draw_star(13<<CED_LAYER_SHIFT, kcolor[clusterIterator->col]);
// 	clusterIterator->draw_tree(4<<CED_LAYER_SHIFT, kcolor[clusterIterator->col]);
//       }
// #endif
    }
    //-----------------------------------------------------------------------
    /** Calculate cluster statistics
	@param k type of cluster
	@param ncl number of clusters
	@param n_sum  total number of hits
	@param e_sum total energy sum
	*/
    void color_clusters_stat(KIND k, unsigned &ncl, unsigned &n_sum, double &e_sum){
      ncl = n_sum = 0;
      e_sum = 0.;

      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();)
	if(clusterIterator->col == k){
	  n_sum += clusterIterator->getNumberOf<DeepAnalysis::Hit>();
	  e_sum += clusterIterator->energySum;
	  ncl++;
	}
    }
    //-----------------------------------------------------------------------
    /** Print clusters statistics.
	@param k kind of cluster
	*/
    void color_clusters_print_stat(KIND k){
      const char *s_start[]={"41m  EM","42m TRK","46m HAD"};
      unsigned n_sum,ncl;
      double e_sum;

      color_clusters_stat(k, ncl, n_sum, e_sum);

      if(!n_sum || (unsigned)k >= sizeof(s_start)/sizeof(*s_start))
	return;

      cout<<"\x1b[30;"<<s_start[k]<<"-Like  \x1b[0m\x1b[30m    energy sum = "
	  <<e_sum<<" in "<<n_sum<<" hits in "<<ncl<<" clusters"<<endl;
    }

    //-----------------------------------------------------------------------
    /** Check result
	*/
    void result_check(){
      for(RSIterator<Hit> hitIterator(all_hits); hitIterator.next();){
	if(hitIterator->getParent<KindGroup>() && !hitIterator->getParent<Cluster>()){
	  cout<<"Logical bug: colored hit ("<<hitIterator->idx<<") without cluster! "
	      <<hitIterator->cartesianPoint.x<<" "
	      <<hitIterator->cartesianPoint.y<<" "
	      <<hitIterator->cartesianPoint.z
	      <<endl;
	}
      }

      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){
	RSIterator<Hit> hitIterator(&clusterIterator);
	hitIterator.next();

	Point3D last(hitIterator->cartesianPoint);
	while(hitIterator.next()){
// #ifdef HAVE_CED
// 	   ced_line(last.x, last.y, last.z,
// 		    hitIterator->cartesianPoint.x,
// 		    hitIterator->cartesianPoint.y,
// 		    hitIterator->cartesianPoint.z,
// 		    19<<CED_LAYER_SHIFT, 1, 0xffffff);
// #endif
	  last = hitIterator->cartesianPoint;
	}
      }
    }
    //-----------------------------------------------------------------------
    /** Move event from a pole to equator of spherical coordinate system.
	*/
    void event_move(){
      double tmp; 

      for(RSIterator<Hit> hitIterator(all_hits); hitIterator.next();){
	tmp = hitIterator->cartesianPoint.x;

	hitIterator->cartesianPoint.x = hitIterator->cartesianPoint.z - detector.shift;// was minus
	hitIterator->cartesianPoint.z = tmp; 
	hitIterator->cartesianPoint.y = -hitIterator->cartesianPoint.y;
	hitIterator->sphere3d.calc(hitIterator->cartesianPoint);
      }
    }
    //-----------------------------------------------------------------------
    /**Move back event to the pole of Sp_CS
	*/
    void event_move_back(){
      double tmp; 
      for(RSIterator<Hit> hitIterator(all_hits); hitIterator.next();){
	tmp = hitIterator->cartesianPoint.x + detector.shift;

	hitIterator->cartesianPoint.x = hitIterator->cartesianPoint.z;
	hitIterator->cartesianPoint.z = tmp;
	hitIterator->cartesianPoint.y = -hitIterator->cartesianPoint.y;
	hitIterator->sphere3d.calc(hitIterator->cartesianPoint);
      }

      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();){
	clusterIterator->calc_prop(*this,false);
      }
    }
    //-----------------------------------------------------------------------
    /** Main routine to split event into four different types of clusters:
	EM - electromagnetic piece of hadronic shower
	TRK - tracks in hadronic shower
	HAD - a few tracks in  hadronic shower that cannot be splitted into individual tracks.
	NEUT - neutron component of hadronic shower
	*/
    void reconstruction(){ 
      //-----------------------------------------------------------------------    
      //    cout<<"        NO CLUSTER JOINING             "<<endl;
      all_hits->calc_stat();
      detector.init(all_hits->radius_g.min);
      event_move();

      //    cout<<"  -----------------  EM like  -------------------------"<<endl;
      cluster_finder_3d(*(kind[EM]), false);
      cluster_join(true);
      em_shower_find();
      reassignment_small_clusters(TRK);

      //    cout<<"  ----------------- TRK like  -------------------------"<<endl;
      cluster_finder_3d(*(kind[TRK]), false);// was true
      //    cluster_join(false);
      reassignment_small_clusters(HAD);

      //    cout<<"  ----------------- HAD like  -------------------------"<<endl;
      cluster_finder_3d(*(kind[HAD]), true);
      //    cluster_join(false);
      //    low_e_clean();
      event_move_back();
      for(RSIterator<Cluster> clusterIterator(clusters); clusterIterator.next();)
	clusterIterator->finalize(*this);
      //    result_check();
      //Reconstruction is finished here
    }

};
//-----------------------------------------------------------------------
/** Get hit color
    @param deepAnalysis
    */
inline DeepAnalysis::KIND DeepAnalysis::Hit::get_color(DeepAnalysis &deepAnalysis){
  if(getParent<NeutGroup>()) 
    return NEUTR;
  if(getParent<KindGroup>() == deepAnalysis.kind[EM])
    return EM;
  if(getParent<KindGroup>() == deepAnalysis.kind[TRK])
    return TRK;

  return HAD;
}


#endif  // __DEEPANALYSIS_HH
