#ifndef PRIMARY_TRACK_FINDER_HH
#define PRIMARY_TRACK_FINDER_HH 1

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
#include <marlin/Processor.h>
#include <EVENT/LCEvent.h>
#include <UTIL/LCTOOLS.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCIntVec.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/CalorimeterHit.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <EVENT/LCParameters.h>
#include <collection_names.hh>
#include <TriggerBits.hh>
#include <CellIndex.hh>
#include <HcalCellIndex.hh>
#include <IMPL/LCGenericObjectImpl.h>
#include <IMPL/LCFlagImpl.h>
//CED
#include "ced_cli.h"
#include "Mapper.hh"
#include "MappedContainer.hh"


//********************************
//  instead of include "layers.h"

#define L1  ( 1<<CED_LAYER_SHIFT)
#define L1S (11<<CED_LAYER_SHIFT)

#define L2  ( 2<<CED_LAYER_SHIFT)
#define L2S (12<<CED_LAYER_SHIFT)

#define L3  ( 3<<CED_LAYER_SHIFT)
#define L3S (13<<CED_LAYER_SHIFT)

#define L4  ( 4<<CED_LAYER_SHIFT)
#define L4S (14<<CED_LAYER_SHIFT)

#define L5  ( 5<<CED_LAYER_SHIFT)
#define L5S (15<<CED_LAYER_SHIFT)

#define L6  ( 6<<CED_LAYER_SHIFT)
#define L6S (16<<CED_LAYER_SHIFT)

#define L7  ( 7<<CED_LAYER_SHIFT)
#define L7S (17<<CED_LAYER_SHIFT)

#define L8  ( 8<<CED_LAYER_SHIFT)
#define L8S (18<<CED_LAYER_SHIFT)

#define L9  ( 9<<CED_LAYER_SHIFT)
#define L9S (19<<CED_LAYER_SHIFT)

#define L0  ( 0<<CED_LAYER_SHIFT)
#define L0S (10<<CED_LAYER_SHIFT)
//********************************
//=======================================================================
/** Detector parameters */
const unsigned ELAY1 = 10; /** Number of layers in 1st ECAL segment*/
const unsigned ELAY2 = 20; /** Number of layers in 1st+2nd ECAL segments*/
const unsigned ELAY  = 30; /** Number of layers in ECAL*/
const unsigned HLAY  = 38; /** Number of layers in HCAL*/
const unsigned EHLAY = 68; /** Number of layers in ECAL+HCAL*/
const unsigned HTAIL = 60; /** Number of first layer in HCAL with lower granularity*/
const unsigned TCAL1 = 9;  /** Number of layers in 1st TCMT segment*/
const unsigned TCAL  = 16; /** Number of layers in TCMT*/
/** Sampling weights*/
float mip2gevECAL1 = 0.00376;
float mip2gevECAL2 = 0.00376*2.; 
float mip2gevECAL3 = 0.00376*3.; 
float mip2gevHCAL1 = 0.02309;
float mip2gevHCAL2 = 0.02309; 
float mip2gevTCAL1 = 0.02309; 
float mip2gevTCAL2 = 0.02309*5.; 
/** Constant parameters for Track Finder*/
const float s1ECAL    = 10.0;  /** Cell size in ECAL*/
const float s3HCAL    = 30.0;  /** Cell size in central part of HCAL*/
const float s6HCAL    = 60.0;  /** Cell size outside the central part of HCAL*/
const float s12HCAL   = 120.0; /** Cell size on the HCAL edges*/
const float b3        = 150.0; /** The boundary of central part of HCAL*/
const float b3l       = 120.0; /** Edge cells in central part of HCAL*/
const float b6        = 330.0; /** The boundary of 6x6 part of HCAL*/
const float b6l       = 270.0; /** Edge cells in 6x6 part of HCAL*/
const float BEAMLIMIT = 20.; /**Beam energy limit for muon criteria application*/
/** Colors for CED drawing*/
const int RED    = 0xfc1d4d; 
const int YELLOW = 0xfcff23;
const int GREEN  = 0x54f354;
const int PINK   = 0xfc15d9;
const int BLUE   = 0x26f1f4;
const int WHITE  = 0xffffff;
const int BLACK  = 0x000000;
/** Helpers*/
const float SQ2         = sqrt(2.05);
const float SQ1         = SQ2*s1ECAL;
const float SQ3         = SQ2*s3HCAL;
const float SQ6         = SQ2*s6HCAL;
const float SQ12        = SQ2*s12HCAL;
const float ParaDistmin = 20.;  /** Parameter to mark events with parallel tracks*/
const float ParaDistmax = 110.; /** Parameter to mark events with parallel tracks*/
//-------------------------------------

using namespace std;
using namespace CALICE;

//=======================================================================
/** Auxiliary class to keep hit data*/
class CHit {
public:
  /** Constructor with member initialization*/
  CHit() 
  { 
    x = y = z = e = r = c = 0.0; 
    l = 0; 
    cellID = 0;
  }
  
  /** Default destructor*/
  ~CHit() { }
  
  unsigned l; /** Layer number*/
  float x; /** x coordinate of hit*/
  float y; /** y coordinate of hit*/
  float z; /** z coordinate of hit*/
  float e; /** Hit energy in MIPs*/
  float r; /** Criteria for distance between neighbour hits*/
  float c; /** Cell size in mm*/
  int cellID; /** Cell ID0*/
};

//=======================================================================
/** Auxiliary class to keep layer data*/
class CLayer {
public:
  /** Constructor with member initialization*/
  CLayer() 
  { 
    nh = 0; 
    emip = 0.0; 
    vl.reserve(50);  
  }

  /** Destructor with vector cleaning*/ 
  ~CLayer() 
  {
    vl.clear(); 
  }

  std::vector<CHit*> vl;
  int nh;      /** number of hits per layer*/
  float emip;  /** energy sum per layer*/
};

//=======================================================================
/** @brief  Marlin processor that finds shower start and primary track

In this processor CalorimeterHit collections for ECAL and HCAL
are used to find shower starting layer and hits belonging to ingoing track,
these hits are then put into output collection of class CalorimeterHit.
In track output collection the following class members
are set:
Type - hit layer number,
Position - XYZ hit coordinates,
Energy - deposited energy in MIPs. 
For shower starting layer, LCGenericObject collection
has two integer members:
calorimeter type (0 - ECAL, 1 - first 30 HCAL layers, 2 - last coarse part of HCAL) 
shower starting layer number (in each calorimeter)
and two float members:
weighted average of x coordinate of track hits
weighted average of y coordinate of track hits

@version 4.01
@author M.V. Chadeeva
@date November 2010
*/
class PrimaryTrackFinder : public marlin::Processor {
public:
  /** Default constructor*/
    PrimaryTrackFinder();
  /** Default destructor*/
    virtual ~PrimaryTrackFinder();

    PrimaryTrackFinder* newProcessor(){ return new PrimaryTrackFinder(); }

  /** Initialization of class members,root-file opening and histogramm booking*/
    virtual void init();
  //-------------------------------------------------------------
  /** Processing of run header*/
    virtual void processRunHeader(LCRunHeader* );
  //-------------------------------------------------------------
  /** Base function to loop over events from which all other
      functions are called for every event*/
    virtual void processEvent(lcio::LCEvent*);
  //-------------------------------------------------------------
  /** Function to close root-file and print some statistic results*/
    virtual void end();
  //-------------------------------------------------------------
  /** Calculates and returns distance between two hits in XY-plane*/ 
    float distXY(CHit* , CHit* );
  //-------------------------------------------------------------
  /** Finds and returns the number of layer where shower started
      using the following criteria:
      1. Moving average sum of MIPs inside the  window = 10 layers  
      for two successive layers is greater than _avMIPxxxx value.
      2. Number of hits in two successive layers is greater than
      _hitLimxxx value. Both constraints are set in steering file.
      They are energy dependent and similar for ECAL and HCAL.*/
    unsigned getStartingLayer( CLayer* );
  //-------------------------------------------------------------
  /** Finds hits belonging to primary track taking in account
      the earlier found shower starting layer. The "nearest neighbour"
      criteria is used. Maximum distance parameters are defined taking
      in account the cell size.
      First parameter is the number of shower starting layer.
      Second parameter is an array with of objects CLayer
      that contain layer parameters. */
    bool FindTrack( unsigned, CLayer* );
  //-------------------------------------------------------------
  /** Indentifies event as muon-like according to criterium
      proposed by Vasiliy Morgunov and based on low amount of
      total deposited energy: 
      E1<first parameter<E2  &&  E3<second parameter<E4
      first parameter = Esum(ECAL)+Esum(HCAL)
      second parameter = Esum(TCMT)
      values E1, E2, E3 and E4 are taken from run data*/
    bool isMuon(float, float);

 private:
  bool _doWriteSubset;
  MappedContainer<CalorimeterHitImpl> *_originalHitsContainer; 
  void fillOriginalHitsContainer(LCCollection *inputCol);
  std::string _mappingProcessorName;        /**<name of the mapping processor name*/
  const Mapper *_mapper;       /**<pointer to the used mapped*/

//===============================================================
// Run information
//---------------------------------------------------------------
  int runnum;
  unsigned calor_front;
  unsigned nhitE;
  unsigned nhitH;
  unsigned nhitT;
//===============================================================
// For steering file
//---------------------------------------------------------------
  bool _fDraw;              /** Flag for CED drawing Y/N=>1/0*/
  int _cedhost;             /** Open CED-viewer port number*/
  int _pointSize;           /** Size of point icon to draw hit*/
  int _lineSize;            /** Line thickness of drawn track*/ 
  bool _fRootOut;           /** Flag for output to root-file Y/N=>1/0*/
  std::string _rootfiledir; /** Directory for output root-file*/

  bool _isMC; /**In case of MC, there are usually no triggers*/
//------------------------------------
// collection names
  std::string _ecalcol;       /** Input CalorimeterHit ECAL collection*/
  std::string _hcalcol;       /** Input CalorimeterHit HCAL collection*/
  std::string _tcmtcol;       /** Input CalorimeterHit TCMT collection*/
  std::string _prtrackColName;/** Output collection name for track hits*/
  std::string _startColName;  /** Output collection name for shower starting layer number*/
//-------------------------------------
// input parameters from steering file
// run information
  float _beamEnergy;      /** Beam energy in GeV*/
  float _beamAngle;       /** Beam incident angle in degrees*/
  float _x_align;         /** ECAL alignment to HCAL along x direction*/ 
  float _y_align;         /** ECAL alignment to HCAL along y direction*/
// Noise cut
  float _MIPLimit;        /** Noise threshold value for hit energy in MIPs*/
// starting layer finder
  bool _fixStartingLayer; /** Skip shower starting layer finding and assume it is after HCAL (for muon runs)*/
// track finder 
  bool _skipTrackFinder;      /** Flag to skip track finding procedure*/
  int _NGAP;                  /** Maximum gap allowed between track points*/
  int _absTrackLengthLimit;   /** Length limit for saved tracks*/
  float _relTrackLengthLimit; /** Minimum track length relative to shower start*/
//===============================================================
// For histogramming
//---------------------------------------------------------------
  TFile* rootfile;        /** Output root-file*/
  TH1F* TrackHitMIP;      /** Histogram with hit energy distribution*/
  TH2F* TrackXY_E;        /** 2D hist with averaged hit coordinate in ECAL*/
  TH2F* TrackXY_H;        /** 2D hist with averaged hit coordinate in HCAL*/
  TH1I* StartLayer;       /** Starting layer number distribution from ECAL front*/
  TH1I* StartLayer_HCAL1; /** Starting layer number distribution from HCAL front*/
  TH1I* StartLayer_HCAL2; /** Starting layer number distribution from 30th HCAL layer*/
//===============================================================
// Helpers
//---------------------------------------------------------------
  float _tangentBeamAngle;                  /** Tangent of beam incident angle*/
  std::vector<CHit*> intrack; /** Vector with pointers to primary track hits*/
  int nhitE_cut;              /** Number of hit in ECAL after _MIPlimit cut*/
  int nhitH_cut;              /** Number of hit in HCAL after _MIPlimit cut*/
  bool paraID;
  bool shortID;
  float _minMIPfromBeamEnergy; /** MIP criteria for starting layer calculated from beam energy*/
  int _minHitsFromBeamEnergy;                  /** Hit criteria for starting layer calculated from beam energy*/
  unsigned _movingAverageWindow;         /** Moving average window*/
  float mov_av[EHLAY];         /** Array of calculated moving averages*/
  // coefficients for muon triangle
  float amu12;
  float bmu12;
  float amu23;
  float bmu23;
  float amu13;
  float bmu13;
//===============================================================
// Global counters
//---------------------------------------------------------------
  int nevt;          /** Number of events processed*/
  int n_muon_like;   /** Number of events identified as muon-like*/
  int n_muon_track;  /** Number of tracks found for muon-like events*/
  int n_short;       /** Number of found but short tracks*/
  int n_short_muon;
  int n_para;        /** Number of parallel tracks*/
  int n_para_muon;   /** Number of parallel muon tracks*/
  int n_hadr_track;  /** Number of tracks found for non-trash and non-muon-like events*/
  int n_early_shower;/** Number of events for which shower starting layer lays before the minimum value defined 
			   in the variable _TrackLengthLimit*/
};
#endif 
