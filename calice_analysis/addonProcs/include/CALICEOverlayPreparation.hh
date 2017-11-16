#ifndef CALICE_OVERLAY_PREPARATION_HH
#define CALICE_OVERLAY_PREPARATION_HH 1

/** @brief  Marlin processor for preparation of events for overlapping

@author M.V. Chadeeva
@ date December 2010
*/

//version 2.02

//ROOT
#include "TROOT.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
// STD
#include <cmath>
#include <vector>
//CALICE
#include <marlin/Global.h>
#include <marlin/Processor.h>
#include <EVENT/LCEvent.h>
#include <IO/LCWriter.h>
#include <UTIL/LCTOOLS.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCIntVec.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/CalorimeterHit.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <EVENT/LCParameters.h>
#include <collection_names.hh>
#include <TriggerBits.hh>
#include <CellIndex.hh>
#include <HcalCellIndex.hh>
#include <IMPL/LCGenericObjectImpl.h>
// TBTrack
#include <TBTrack/TBTrackAligner.hh>
#include <TBTrack/TBTrackBaseProcessor.hh>
#include <TBTrack/TBTrackDbHandler.hh>
#include <TBTrack/TBTrackMapper.hh>
#include <TBTrack/TBTrackMokkaCheck.hh>
#include <TBTrack/TBTrackProducerCheck.hh>
#include <TBTrack/TBTrackProducer.hh>
#include <TBTrack/TBTrackRawCheck.hh>
#include <TBTrack/TBTrackRemover.hh>
//#include <TBTrack/TBTrackScatter.hh>
#include <TBTrack/TBTrackTdcHitsCheck.hh>
//CED
#include <ced_cli.h>
#include <layers.h>
//#include "BoojumColor.h"
/*
//GEAR
#include <gear/GEAR.h>
#include <gear/TPCParameters.h>
#include <gearimpl/TPCParametersImpl.h>
#include <gearimpl/FixedPadSizeDiskLayout.h>
#include <gear/CalorimeterParameters.h>
#include <gear/LayerLayout.h>
#include <gear/BField.h>
*/
#include "Alignment.h"

//=======================================================================
/** Detector parameters */
const float ELIMIT = 20.;
const unsigned ELAY1 = 10; /** Number of layers in 1st ECAL segment*/
const unsigned ELAY2 = 20; /** Number of layers in 1st+2nd ECAL segments*/
const unsigned ELAY = 30;  /** Number of layers in ECAL*/
const unsigned HLAY = 38;  /** Number of layers in HCAL*/
const unsigned EHLAY = 68; /** Number of layers in ECAL+HCAL*/
const unsigned HTAIL = 60; /** Number of first layer in HCAL with lower granularity*/
const unsigned TCAL1 = 9;  /** Number of layers in 1st TCMT segment*/
const unsigned TCAL = 16;  /** Number of layers in TCMT*/
const float ECAL_Z_ALIGN = -1289.4375;
const float HCAL_Z_ALIGN = -1468.0;
const float TCAL_Z_ALIGN = -1590.38;
const float HCAL_WIDTH = 500.; // mm
/** Constant parameters for Track Finder*/
const float s1ECAL = 10.0; /** Cell size in ECAL*/
const float s3HCAL = 30.0; /** Cell size in central part of HCAL*/
const float s6HCAL = 60.0; /** Cell size outside the central part of HCAL*/
const float s12HCAL = 120.0; /** Cell size on the HCAL edges*/
const float b3 = 150.0; /** The boundary of central part of HCAL*/
const float b3l = 120.0; /** Edge cells in central part of HCAL*/
const float b6 = 330.0; /** The boundary of 6x6 part of HCAL*/
const float b6l = 270.0; /** Edge cells in 6x6 part of HCAL*/
/** Colors for CED drawing*/
const int RED = 0xfc1d4d; 
const int YELLOW = 0xfcff23;
const int GREEN = 0x54f354;
const int PINK = 0xfc15d9;
const int BLUE = 0x26f1f4;
const int WHITE = 0xffffff;
const int BLACK = 0x000000;
/** Output collections*/
const std::string COL_ECAL  = "ECAL";
const std::string COL_HCAL  = "HCAL";
const std::string COL_TCAL  = "TCAL";
const std::string COL_TRACK = "PTRK";
const std::string COL_ADD   = "ADDS";
/** Helpers*/
const float SQ2  = sqrt(2.05);
const float SQ1  = SQ2*s1ECAL;
const float SQ3  = SQ2*s3HCAL;
const float SQ6  = SQ2*s6HCAL;
const float SQ12 = SQ2*s12HCAL;
const float SQADD = SQ2*0.5*s3HCAL;
const float ParaDistmin = 20.;
const float ParaDistmax = 110.;
//-------------------------------------

using namespace std;

/** Auxiliary class to keep hit data*/
class CHit {
public:
  /** Constructor with member initialization*/
  CHit() { x = y = z = r = w = 0.0; 
    l = 0; hit_addr = NULL; tr = false; b = false;}
  /** Default destructor*/
  ~CHit() { }
  bool tr; /** true if belongs to track*/
  bool b;  /** true if boundary cell*/
  unsigned l; /** Layer number*/
  float x; /** x coordinate of hit*/
  float y; /** y coordinate of hit*/
  float z; /** z coordinate of hit*/
  float w; /** Weight for cell*/ 
  float r; /** Criteria for distance between neighbour hits*/
  // helper for copy to a new collection
  CalorimeterHit* hit_addr;
};

/** Auxiliary class to keep layer data*/
class CLayer {
public:
  /** Constructor with member initialization*/
    CLayer() { nh = 0; emip = 0.0; vl.reserve(50);  }
  /** Destructor with vector cleaning*/ 
    ~CLayer() {vl.clear(); }
    std::vector<CHit*> vl;
    int nh;
    float emip;
};

/**  CALICEOverlayPreparation processor class*/
class CALICEOverlayPreparation : public marlin::Processor {
public:
  /** Default constructor*/
    CALICEOverlayPreparation();
  /** Default destructor*/
    virtual ~CALICEOverlayPreparation();

    CALICEOverlayPreparation* newProcessor(){ return new CALICEOverlayPreparation(); }

  /** Initialization of class members,root-file opening and histogramm booking*/
    virtual void init();
  /** Get run information */
  virtual void processRunHeader(LCRunHeader* run);
  /** Mail function to loop over events from which all other
      functions are called for every event*/
    virtual void processEvent(lcio::LCEvent*);
  /** Function to close root-file and print some statistic results*/
    virtual void end();
  /** Finds and returns the number of layer where shower started
      using the following criteria:
      1. Moving average sum of MIPs inside the  window = 10 layers  
      for two successive layers is greater than _avMIPxxxx value.
      2. Number of hits in two consequent layers is greater than
      _hitLimxxx value. Both constraints are energy dependent.*/
  unsigned getStartingLayer( CLayer* );
  /** Finds hits belonging to primary track taking in account
      the earlier found shower starting point. The "nearest neighbour"
      criteria is used. Maximum distance parameters are defined taking
      in account the cell size.
      First parameter is the number of shower starting layer.
      Second parameter is an array with objects CLayer
      that contain layer parameters. */
  bool FindTrack( unsigned, CLayer* );
  /** Finds muon-like events */
  bool isMuon(float, float);
  /** Selects trash events (including multiparticle) */
  bool isTrash(unsigned, float);
  /** Calculates and returns distance between two hits in XY-plane*/ 
  float distXY(CHit* , CHit* );
 
 private:
  LCWriter* lcwriter;
 
  int runnum;
  unsigned calor_front;
  unsigned num_align;
  float e_trash_max;
  float e_mult_min;

  unsigned nhitE;
  unsigned nhitH;
  unsigned nhitT;
  unsigned nDCx;
  unsigned nDCy;
  unsigned ncutE;
  unsigned ncutH;
  unsigned ncutT;

  unsigned sh_st;
  float xshift;
  float yshift;
//=====================================================================
// input parameters from steering file
//-------------------------------------
// debug flags and parameters from steering file
    int _fDraw;      /** Flag for CED drawing Y/N=>1/0*/
    int _cedhost;    /** Open CED-viewer port number*/
    int _fRootOut;   /** Flag for output to root-file Y/N=>1/0*/
//-------------------------------------
  // output slcio
    std::string outputFileName; /** Name of output slcio-file*/
  // output root
    std::string rootfilename;   /** Name of output root-file*/
    std::string _dirOutput;     /** Output directory*/
//-------------------------------------
// for shower starting layer
  float _noiseMIPLimit;        /** Noise threshould value for hit energy in MIPs*/
  float miplim;                /** MIP criteria for starting layer calculated from beam energy*/
  int hitlim;                  /** Hit criteria for starting layer calculated from beam energy*/
  unsigned win_mov_av;         /** Moving average window*/
  float mov_av[EHLAY];         /** Array of calculated moving averages*/
// for track 
  int _NGAP;                  /** Maximum gap allowed between track points*/
  int _absTrackLengthLimit;   /** Length limit for saved tracks*/
  float _relTrackLengthLimit; /** Minimum track length relative to shower start*/
// beam data
  float _beamAngle;           /** Beam incident angle in degrees*/
  float _beamEnergy;          /** Beam energy in GeV*/
  float _calWidth;            /** Half of calorimeter width for beam window*/
  std::string _ptype;         /** Type of output particle*/
  int _rbin;                  /** Number of bins for shower radius calculation*/
  int _mcflag;                /**  1/0 - MC/Data*/
  std::string _physlist;      /** Physics list for MC*/
//-------------------------------------
// calibration constants
  float _mip2gevECAL1; 
  float _mip2gevECAL2; 
  float _mip2gevECAL3; 
  float _mip2gevHCAL1;
  float _mip2gevHCAL2; 
  float _mip2gevTCAL1; 
  float _mip2gevTCAL2; 
//--------------------------------------
// collection names
  std::string _xtbtcol;  /** Name of TBTrack collection for x coordinate*/
  std::string _ytbtcol;  /** Name of TBTrack collection for y coordinate*/
  std::string _ecalcol;  /** Name of ECAL CalorimeterHits collection*/
  std::string _hcalcol;  /** Name of HCAL CalorimeterHits collection*/
  std::string _tcmtcol;  /** Name of TCMT CalorimeterHits collection*/
//==========================================================================
// histogramming
  TFile* rootfile; /** Pointer to debug root-file*/
  TH1I* Cher_trig;
  TH1F* Etrack_dist;
  TH1F* Esum_dist;
  TH1F* Esum95_dist;
  TH1F* Eshower_dist;
  TH1F* Rshower_dist;
  TH2F* TrackXY_E;   /** Pointer to 2D hist with averaged track hit coordinates*/
  TH2F* TrackDC;     /** Pointer to 2D hist with DC coordinates on HCAL front*/
  TH1I* StartLayer;  /** Pointer to hist with starting layer number distribution*/
  TH2F* xyCOG_ecal;
  TH2F* xyCOG_hcal;
  TH2F* xy_dc;
// helpers
  std::vector<CHit*> intrack; /** Vector with pointers to primary track hits*/
  float tba;            /** Beam slope w.r.t. z axis (tangent)*/
  bool paraID;
  float dr;
  float amu12;
  float bmu12;
  float amu13;
  float bmu13;
  float amu23;
  float bmu23;
//counters
  int nevt; /** Number of events processed*/
  int nevt_sel; /** Number of selected events*/
};
#endif 
