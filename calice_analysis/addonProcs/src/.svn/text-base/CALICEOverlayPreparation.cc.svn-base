#include <CALICEOverlayPreparation.hh>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace marlin;
using namespace CALICE;
using namespace lcio;
using namespace std;

//------------------------------------------------------------------
//           CALICE Overlay Preparation
//             version 2.02 
//              December 2010
// Uses CalorimeterHit collections data
// to prepare events for merging with selection
// depending on required particle type.
// Detector configuration for CERN 2007 runs
// (30-layer ECAL & 38-layer HCAL)
// 
//      @author M. V. Chadeeva
//=====================================================================

//#include "DrawGeometry.hh"
#include "StartAndTrack.hh"
#include "Select.hh"

//===================================================================
CALICEOverlayPreparation aCALICEOverlayPreparation;

//================constructor=======================================
CALICEOverlayPreparation::CALICEOverlayPreparation() : Processor("CALICEOverlayPreparation"){

   _description = "Preparation of collections for overlay";

   // beam and particle
   registerProcessorParameter("BeamEnergy", "Beam energy in GeV",
			      _beamEnergy, float (0.0) );
   registerProcessorParameter("BeamAngle", "Beam incident angle in degrees",
			      _beamAngle, float (0.0) );
   registerProcessorParameter("OutputParticleType", "Type of output particle",
			      _ptype, std::string ("proton") );
   registerProcessorParameter("CalorimeterHalfWidth", "Half of calorimeter width for beam window",
			      _calWidth, float (100.) );
   registerProcessorParameter("RadialBins", "Number of bins for 95-% shower radius calculation",
			      _rbin, int (50) );
   registerProcessorParameter("MCflag", "1/0 - MC/Data",
			      _mcflag, int (0) );
   registerProcessorParameter("MCPhysicsList", "GEANT physics list name",
			      _physlist, std::string ("LHEP") );

   // CALICE calibration constants
   registerProcessorParameter("MIP2GEVinECAL1", "MIP to GeV in 1st ECAL part",
			      _mip2gevECAL1, float (0.00376) );
   registerProcessorParameter("MIP2GEVinECAL2", "MIP to GeV in 2nd ECAL part",
			      _mip2gevECAL2, float (0.00752) );
   registerProcessorParameter("MIP2GEVinECAL3", "MIP to GeV in 3d ECAL part",
			      _mip2gevECAL3, float (0.01128) );
   registerProcessorParameter("MIP2GEVinHCAL1", "MIP to GeV in 1st HCAL part",
			      _mip2gevHCAL1, float (0.02364) );
   registerProcessorParameter("MIP2GEVinHCAL2", "MIP to GeV in 2nd HCAL part",
			      _mip2gevHCAL2, float (0.02364) );
   registerProcessorParameter("MIP2GEVinTCAL1", "MIP to GeV in 1st TCAL part",
			      _mip2gevTCAL1, float (0.02364) );
   registerProcessorParameter("MIP2GEVinTCAL2", "MIP to GeV in 2nd TCAL part",
			      _mip2gevTCAL2, float (0.11820) );
   // collections' names
   registerProcessorParameter("xTBTrackCollection", "Name of TBTrack X collection",
			      _xtbtcol, std::string ("TBTrackFEX") );
   registerProcessorParameter("yTBTrackCollection", "Name of TBTrack Y collection",
			      _ytbtcol, std::string ("TBTrackFEY") );
   registerProcessorParameter("ecalHitsCollection", "Name of ECAL hits collection",
			      _ecalcol, std::string ("EmcCalorimeter_Hits") );
   registerProcessorParameter("hcalHitsCollection", "Name of HCAL hits collection",
			      _hcalcol, std::string ("AhcCalorimeter_Hits") );
   registerProcessorParameter("tcmtHitsCollection", "Name of TCMT hits collection",
			      _tcmtcol, std::string ("TcmtCalorimeter_Hits") );

   // CED drawing
   registerProcessorParameter("DrawFlag", "Flag for CED drawing",
			      _fDraw, int (0) );
   registerProcessorParameter("CED_HostPort", "Host Port number for CED",
			      _cedhost, int (7557) );
   // ROOT output
   registerProcessorParameter("RootOutputFlag", "Flag for output to Root-file",
			      _fRootOut, int (1) );
   registerProcessorParameter("slcioOutputDir", "Output directory name",
			      _dirOutput, std::string ("./") );
   //noise threshold
   registerProcessorParameter("NoiseMIPLimit", "Noise threshould for hit energy in MIPs",
			      _noiseMIPLimit, float (0.5) );
   // track criteria
   registerProcessorParameter("SizeofGAP", "Maximum gap between track points",
			      _NGAP, int (6) );
   registerProcessorParameter("AbsTrackLengthLimit", "Length limit for saved tracks",
			      _absTrackLengthLimit, int (3) );
   registerProcessorParameter("RelTrackLengthLimit", "Minimum track length relative to shower start",
			      _relTrackLengthLimit, float (0.3) );

} // end constructor

//====================destructor========================================================
CALICEOverlayPreparation::~CALICEOverlayPreparation(){}

//===================ini function=====================================================
void CALICEOverlayPreparation::init() {

  if (_fDraw) {
//------------------------------------------------------
//     Initialize CED
//------------------------------------------------------
    ced_client_init("localhost", _cedhost); // connection to CED
    ced_register_elements();
    ced_new_event();
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    std::cout << "  Connection to CED through port " << _cedhost << endl;
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
  }
  tba =  tan(_beamAngle*M_PI/180.0);
  dr = HCAL_WIDTH/float(_rbin);

  // criteria for starting layer finding
  miplim = 6.0 + 0.1*_beamEnergy;
  hitlim = int(3.77 + 1.44*std::log(_beamEnergy) + 0.5);
  win_mov_av = 10;

  // criteria for trash
  e_trash_max = _beamEnergy*0.84 - 2.4*sqrt(_beamEnergy); // 0.84 = e/pi ratio
  e_mult_min = _beamEnergy + 2.4*sqrt(_beamEnergy);

  // muon coefficients
  if ( _beamEnergy > ELIMIT ) {
    float x1 = 1.0;
    float y1 = 0.5;
    float x2 = 7.0;
    float y2 = 2.0;
    float x3 = 1.5;
    float y3 = 7.0;
    amu12 = (y1 - y2)/(x1 - x2);  // 0.25
    bmu12 = y1 - amu12*x1;        // 0.25
    amu23 = (y2 - y3)/(x2 - x3);  // -0.9090909
    bmu23 = y2 - amu23*x2;        // 8.364
    amu13 = (y1 - y3)/(x1 - x3);  // 13.
    bmu13 = y1 - amu13*x1;        // -12.5
  }
  else {
    float x1 = 1.0;
    float y1 = 0.8;
    float x2 = 4.0;
    float y2 = 1.0;
    float x3 = 1.5;
    float y3 = 6.0;
    amu12 = (y1 - y2)/(x1 - x2);  // 
    bmu12 = y1 - amu12*x1;        // 
    amu23 = (y2 - y3)/(x2 - x3);  // 
    bmu23 = y2 - amu23*x2;        // 
    amu13 = (y1 - y3)/(x1 - x3);  // 
    bmu13 = y1 - amu13*x1;        // 
  }

  lcwriter = NULL;

// global counters
  nevt = 0;
  nevt_sel = 0;

} //end init function
//====================== read run header and alignment =============================
void CALICEOverlayPreparation::processRunHeader(LCRunHeader* run) {

  if ( lcwriter ) return; // return if output files are already opened

  runnum = run->getRunNumber();
  int rnm = 0;
  for ( unsigned i = 0; i < NUM_RUNS; i++ ) {
    if ( runnum == ALIGN[i][0] ) {
      rnm = runnum;
      num_align = i;
      /*
      ecal_x_shift = ALIGN[i][3];
      ecal_y_shift = ALIGN[i][4];
      hcal_x_shift = ALIGN[i][5];
      hcal_y_shift = ALIGN[i][6];
      ecal_z_shift = ALIGN[i][7];
      */
    }
  }

  if ( runnum > 350000 ) calor_front = 30; // case of HCAL only
  else calor_front = 0;

  if ( rnm ) {
    cout << "ALIGNMENT IS APPLIED FOR RUN " << rnm << endl;

  // file name construction from particle ID and beam energy
    char s[80];
    char sr[80];
    if ( _mcflag ) {
      for ( int ii = 1; ii < 7; ii++ ) {    // due to shift during digitization
	ALIGN[num_align][ii] =  ALIGN_MC[num_align][ii];
      } 
      sprintf(s,"%s%6s%03d_%06d_%s.slcio", _dirOutput.c_str(), _ptype.c_str(), int(_beamEnergy), runnum, _physlist.c_str());
      sprintf(sr,"%s%6s%03d_%06d_%s.root", _dirOutput.c_str(), _ptype.c_str(), int(_beamEnergy), runnum, _physlist.c_str());
    }
    else {
      sprintf(s,"%s%6s%03d_%06d.slcio", _dirOutput.c_str(), _ptype.c_str(), int(_beamEnergy), runnum);
      sprintf(sr,"%s%6s%03d_%06d.root", _dirOutput.c_str(), _ptype.c_str(), int(_beamEnergy), runnum);
    }
    outputFileName = s;
    rootfilename = sr;

// root output
    if (_fRootOut){
      rootfile = new TFile(rootfilename.c_str(),"RECREATE");
      cout << "ROOT file " << rootfilename << " opened for root output" << endl;

      float binnum = 100.;
      float llim = _beamEnergy - 20.; if ( llim < 0. ) llim = 0.; 
      float rlim = _beamEnergy + 20.;
      float dbin = (rlim - llim)/binnum;
      rlim = llim + binnum*dbin;

      Cher_trig = new TH1I("Cher_trig", "Cherenkov", 2, 0, 2);
      Etrack_dist = new TH1F("Etrack_dist", "Track energy", 100, 0., 5.);
      Etrack_dist->GetXaxis()->SetTitle("E [GeV]");
      Esum_dist = new TH1F("Esum_dist", "Deposited energy: ECAL+HCAL+TCMT", int(binnum), llim, rlim);
      Esum_dist->GetXaxis()->SetTitle("E [GeV]");
      Esum95_dist = new TH1F("Esum95_dist", "Deposited energy: ECAL+HCAL(95% of total)", int(binnum), llim, rlim);
      Esum95_dist->GetXaxis()->SetTitle("E [GeV]");
      Eshower_dist = new TH1F("Eshower_dist", "Shower energy: ECAL+HCAL(95% of total)", int(binnum), llim, rlim);
      Eshower_dist->GetXaxis()->SetTitle("E [GeV]");
      Rshower_dist = new TH1F("Rshower_dist", "Shower radius (with 95% of deposited energy in ECAL+HCAL)", _rbin, 0., HCAL_WIDTH);
      Rshower_dist->GetXaxis()->SetTitle("R [mm]");
      TrackXY_E = new TH2F("TrackXY_E","Average track hit coordinates",200,-200.,200.,200,-200.,200.);
      TrackXY_E->GetXaxis()->SetTitle("X [mm]");
      TrackXY_E->GetYaxis()->SetTitle("Y [mm]");
      TrackDC = new TH2F("TrackDC","DC track coordinates on HCAL front",200,-200.,200.,200,-200.,200.);
      TrackDC->GetXaxis()->SetTitle("X [mm]");
      TrackDC->GetYaxis()->SetTitle("Y [mm]");
      StartLayer = new TH1I("StartPoint","Shower starting layer distribution", EHLAY, 0, int(EHLAY));
      StartLayer->GetXaxis()->SetTitle("Number of shower starting layer from ECAL front");
      xyCOG_ecal = new TH2F("xyCOG_ecal","Centers of gravity in ECAL", 200, -100., 100., 200, -100., 100.);
      xyCOG_ecal->GetXaxis()->SetTitle("X [mm]");
      xyCOG_ecal->GetYaxis()->SetTitle("Y [mm]");
      xyCOG_hcal = new TH2F("xyCOG_hcal","Centers of gravity in HCAL", 400, -200., 200., 400, -200., 200.);
      xyCOG_hcal->GetXaxis()->SetTitle("X [mm]");
      xyCOG_hcal->GetYaxis()->SetTitle("Y [mm]");
      xy_dc = new TH2F("xy_dc","XY positions in DC", 200, -100., 100., 200, -100., 100.);
      xy_dc->GetXaxis()->SetTitle("X [mm]");
      xy_dc->GetYaxis()->SetTitle("Y [mm]");
    }
// open output slcio file

    lcwriter = LCFactory::getInstance()->createLCWriter();
    if ( lcwriter ) cout << "LCWriter created" << endl;
    else cout << "LCWriter creation failed" << endl;

    lcwriter->open( outputFileName.c_str(), LCIO::WRITE_NEW );
    cout << "File " << outputFileName << " opened for slcio output" << endl;

    lcwriter->writeRunHeader(run);
  }
  else{
    cout<<"   Abort"<<endl; 
    abort();
  }
}
//=============================process event ==================================
void CALICEOverlayPreparation::processEvent(LCEvent* evt){ 	 
  if (_fDraw ) {
    //DrawGeometry();
    ced_new_event();
  } 
//------------------------------------------
// Clear sums and counters
  nhitE = 0;
  nhitH = 0;
  nhitT = 0;
  nDCx = 0;
  nDCy = 0;

  ncutE = 0;
  ncutH = 0;
  ncutT = 0;

  LCCollection *col_E = NULL;
  LCCollection *col_H = NULL;
  LCCollection *col_T = NULL;
  LCCollection *col_DCx = NULL;
  LCCollection *col_DCy = NULL;

// Beam coordinates
  xshift = -99999.;
  yshift = -99999.;

// Energy sums 
  float esumEm = 0.0;
  float esumHm = 0.0;
  float esumTm = 0.0;  // Energy sum for TCMT in mip
  float esumEg1 = 0.0;
  float esumEg2 = 0.0;
  float esumEg3 = 0.0;
  float esumHg = 0.0;
  float esumTg1 = 0.0;  // Energy sum for TCMT in gev
  float esumTg2 = 0.0;

  //DC
  float xdc = -99999.;
  float ydc = -99999.;
  float z_hcal_front = 10000;

  // Flags
  bool muonID = false;
  bool trashID = false;
  bool decision = false;
  bool track_found = false;
  bool dc_track = false;

  // Beam position
  float xtrack = 0.;
  float ytrack = 0.;
  int shift_type = 0;

  // Helpers
  unsigned cher = 0;
  float Etrack = 0.0;
  float Etrack_mip = 0.0;
  float Eshower = 0.0;
  float Rshower = 0.0;
  intrack.clear();
  paraID = false;
  float xcog_ecal = 0.0;
  float ycog_ecal = 0.0;
  float xcog_hcal = 0.0;
  float ycog_hcal = 0.0;
//----------------------------------------------- 
// Trigger check
  bool tflag = false;
  //bool pedestal = false;
  //bool beamtrig = false;
  TriggerBits evt_trg(((LCEvent *) evt)->getParameters().getIntVal(PAR_TRIGGER_EVENT));
  //if ( evt_trg.isPurePedestalTrigger() || evt_trg.isPedestalTrigger() ) pedestal = true;
  //if ( evt_trg.isPureBeamTrigger() || evt_trg.isBeamTrigger() ) beamtrig = true;
  //cout << "Pedestal: " << pedestal << " Beam: " << beamtrig << " Cher: " << cher << endl;

  if ( _mcflag ) {  // assign cher bit for MC
    tflag = true;
    nevt++;
    if ( (!std::strcmp(_ptype.data(),"electr")) ||
	 (!std::strcmp(_ptype.data(),"positr")) ||
	 (!std::strcmp(_ptype.data(),"pionpl")) )
      cher = 1;
    if ( (!std::strcmp(_ptype.data(),"proton")) ||
	 (!std::strcmp(_ptype.data(),"pionmi")) )
      cher = 0;
  }
  else {
    if(evt_trg.isPurePedestalTrigger() || evt_trg.isPedestalTrigger()) 
      return;
    if(evt_trg.isCalibTrigger() || evt_trg.isPureCalibTrigger() ) 
      return; 

    if(evt_trg.isPureBeamTrigger() || evt_trg.isBeamTrigger()) {
      if ( evt_trg.isCherenkovTrigger() )  cher = 1;

      nevt++; // number of events according to beam trigger

      // skip events with inconsistent cher bit
      if ( !cher && ( (!std::strcmp(_ptype.data(),"electr")) ||
		      (!std::strcmp(_ptype.data(),"positr")) ||
		      (!std::strcmp(_ptype.data(),"pionpl")) ) )
	return;
      if ( cher && ( (!std::strcmp(_ptype.data(),"proton")) ||
		     (!std::strcmp(_ptype.data(),"pionmi")) ) )
	return;
      tflag = true;
    }
    else
      tflag = false; 
  }
    //    cout << "Event " << nevt << endl; 
  if ( tflag ) {
// get collection names
    const std::vector<std::string>* strVec = evt->getCollectionNames();
// resolve collection names
    for( std::vector<std::string>::const_iterator name = strVec->begin();
	 name != strVec->end() ; name++){
      const std::string & tname = evt->getCollection( *name )->getTypeName();
      if(tname == LCIO::LCGENERICOBJECT ) {
	if( !std::strcmp((*name).data(), _xtbtcol.c_str()) ) {
	  col_DCx = evt->getCollection( *name );
	  nDCx  = col_DCx->getNumberOfElements(); // number of elements in DC collection
	} // TDC x collection

	if( !std::strcmp((*name).data(), _ytbtcol.c_str()) ){
	  col_DCy = evt->getCollection( *name );
	  nDCy  = col_DCy->getNumberOfElements(); // number of elements in DC collection
	} // TDC y collection 
      } // if LCGENERICOBJECT

      if(tname == LCIO::CALORIMETERHIT ){
	if( !std::strcmp((*name).data(), _ecalcol.c_str()) ) {
	  col_E = evt->getCollection( *name );
	  nhitE =  col_E->getNumberOfElements(); // number of hits in ECAL
	} // if EmcCalorimeter_Hits

	if( !std::strcmp((*name).data(), _hcalcol.c_str()) ) {
	  col_H = evt->getCollection( *name );
	  nhitH =  col_H->getNumberOfElements(); // number of hits in HCAL
	} // end if AhcCalorimeter_Hits

	if( !std::strcmp((*name).data(), _tcmtcol.c_str()) ) {
	  col_T = evt->getCollection( *name );
	  nhitT =  col_T->getNumberOfElements(); // number of hits in TCMT
	} // if TcmtCalorimeter_Hits

      } // if CALORIMETERHIT

    } // end for Collection name

    //cout << "  Ecal: " << nhitE << "; Hcal: " << nhitH << "; Tcmt: " << nhitT << endl;

// Adjacent arrays for track finder
     CLayer clayer[EHLAY+1];
     CHit chit[nhitE+nhitH+nhitT];
//-------------------------------------------------------------------------------------
// ------  Fill hit data in CHit ---------------------------------------------------
//-------------------------------------------------------------------------------------
// ECAL
//-------------------------------------------------------------------------------------
     for (unsigned i = 0; i < nhitE; i++) {
       CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col_E->getElementAt( i ) );
       CellIndex cid(hit->getCellID0());
       unsigned layer = unsigned(cid.getLayerIndex() - 1); // layer number
       chit[i].l = layer;
       chit[i].x = hit->getPosition()[0] + ALIGN[num_align][3];
       chit[i].y = hit->getPosition()[1] + ALIGN[num_align][4];
       chit[i].z = hit->getPosition()[2] + ECAL_Z_ALIGN + ALIGN[num_align][7];
       chit[i].hit_addr = hit;
       float e = hit->getEnergy();
       if ( e >= _noiseMIPLimit ) {
	 ncutE++;
	 if ( layer == (ELAY - 1) ) chit[i].r = SQ3;
	 else chit[i].r = SQ1;
	 chit[i].w = s1ECAL;
	 clayer[layer].vl.push_back(&chit[i]);
	 clayer[layer].nh++;
	 clayer[layer].emip += e;
	 esumEm += e;
// mips to gev without even-odd corrections !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 if( layer < ELAY1 ) {
	   esumEg1 += e*_mip2gevECAL1; 
	   xcog_ecal += hit->getPosition()[0]*e*_mip2gevECAL1;
	   ycog_ecal += hit->getPosition()[1]*e*_mip2gevECAL1;
	 }
	 else if( layer < ELAY2 ) {
	   esumEg2 += e*_mip2gevECAL2;
	   xcog_ecal += hit->getPosition()[0]*e*_mip2gevECAL2;
	   ycog_ecal += hit->getPosition()[1]*e*_mip2gevECAL2;
	 }
	 else {
	   esumEg3 += e*_mip2gevECAL3;
	   xcog_ecal += hit->getPosition()[0]*e*_mip2gevECAL3;
	   ycog_ecal += hit->getPosition()[1]*e*_mip2gevECAL3;
	 }
       }
     } // for hits ECAL
     float ee = esumEg1 + esumEg2 + esumEg3;
     if ( ee > 0. ) {
       xcog_ecal /= ee;
       ycog_ecal /= ee;
       xyCOG_ecal->Fill(xcog_ecal,ycog_ecal,1.0);
     }
//-------------------------------------------------------------------------------------
//HCAL
//-------------------------------------------------------------------------------------
    for(unsigned i = nhitE; i < nhitE + nhitH; i++) {
      CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col_H->getElementAt( i - nhitE) ); 
      HcalCellIndex cid(hit->getCellID0());
      unsigned layer = unsigned(cid.getLayerIndex() - 1); // layer number to index 
      chit[i].l = unsigned(layer) + ELAY; 
      chit[i].x = hit->getPosition()[0] + ALIGN[num_align][5];
      chit[i].y = hit->getPosition()[1] + ALIGN[num_align][6];
      chit[i].z = hit->getPosition()[2] + HCAL_Z_ALIGN;
      chit[i].hit_addr = hit;
      float e = hit->getEnergy();
      float xx = fabs(hit->getPosition()[0]);
      float yy = fabs(hit->getPosition()[1]);
      if ( hit->getPosition()[2] < z_hcal_front ) z_hcal_front =  hit->getPosition()[2];
      if ( e >= _noiseMIPLimit ) {
	ncutH++;
	if ( chit[i].l < HTAIL ) {
	  if((xx < b3) && (yy < b3)){
	    chit[i].w = s3HCAL;
	    if(chit[i].l == HTAIL-1) 
	      chit[i].r = SQ6;
	    else {
	      chit[i].r = SQ3;
	      if((xx > b3l) || (yy > b3l)) chit[i].b = true;
	    }
	  } else if ( (xx < b6) && (yy < b6)) {
	    chit[i].w = s6HCAL;
	    chit[i].r = SQ6;
	    if ( (xx > b6l) || (yy > b6l) ) chit[i].b = true;
	  } 
	  else {
	    chit[i].r = SQ12;
	    chit[i].w = s12HCAL;
	  }
	} 
	else {
	  if((xx < b6) && (yy < b6) ) {
	    chit[i].w = s6HCAL;
	    chit[i].r = SQ6;
	    if( (xx > b6l) || (yy > b6l) ) chit[i].b = true;
	  } 
	  else {
	    chit[i].r = SQ12;
	    chit[i].w = s12HCAL;
	  }
	}
	clayer[layer+ELAY].vl.push_back(&chit[i]);
	clayer[layer+ELAY].nh++;
	clayer[layer+ELAY].emip += e;
	esumHm += e;
	esumHg += e*_mip2gevHCAL1;
	xcog_hcal += hit->getPosition()[0]*e;
	ycog_hcal += hit->getPosition()[1]*e;
      }
    } // for hits HCAL
    float eeh = ee + esumHg;
    if ( esumHm > 0. ) {
      xcog_hcal /= esumHm;
      ycog_hcal /= esumHm;
      xyCOG_hcal->Fill(xcog_hcal,ycog_hcal,1.0);
    }
//-------------------------------------------------------------------------------------
//TCMT
//-------------------------------------------------------------------------------------
    for(unsigned i = nhitE + nhitH; i < nhitE + nhitH + nhitT; i++) {
      CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col_T->getElementAt( i - nhitE - nhitH) );
	CellIndex cid(hit->getCellID0());
	unsigned layer = unsigned(cid.getLayerIndex() - 1); // layer number
// energy and coordinates
	chit[i].l = unsigned(layer) + EHLAY;
	chit[i].x = hit->getPosition()[0] + ALIGN[num_align][5];
	chit[i].y = hit->getPosition()[1] + ALIGN[num_align][6];
	chit[i].z = hit->getPosition()[2] + TCAL_Z_ALIGN;
	chit[i].hit_addr = hit;
	float e = hit->getEnergy();
	if ( e >= _noiseMIPLimit ) {
	  ncutT++;
	  esumTm += e;
	  if ( layer < TCAL1 ) esumTg1 += e*_mip2gevTCAL1;
	  else esumTg2 += e*_mip2gevTCAL2;
	}
    }
    float et = esumTg1 + esumTg2;
    float e_all = eeh + et;
//-------------------------------------------------------------------------------------
    //TBTrack collections
//-------------------------------------------------------------------------------------
    if ( nDCx ) {
      EVENT::LCGenericObject* elx = dynamic_cast<LCGenericObject*>( col_DCx->getElementAt( 0 ) );
      TBTrack::TrackProjection projX( elx );
      xdc = projX.intercept( z_hcal_front, 0.0 );
    }
    if ( nDCy ) {
      EVENT::LCGenericObject* ely = dynamic_cast<LCGenericObject*>( col_DCy->getElementAt( 0 ) );
      TBTrack::TrackProjection projY( ely );
      ydc = projY.intercept( z_hcal_front, 0.0 );
    }
    if ( nDCx && nDCy ) xy_dc->Fill(xdc,ydc,1.0);

     xdc += ALIGN[num_align][1];
     ydc += ALIGN[num_align][2];

     if ( fabs(xdc) < _calWidth && fabs(ydc) < _calWidth )
       dc_track = true;
     else
       dc_track = false;
//-------------------------------------------------------------------------------------
//------ Calculate  moving average of mips in layer  -------------- 
//-------------------------------------------------------------------------------------
    mov_av[0] = clayer[0].emip;
    for ( unsigned i = 1; i < win_mov_av; i++ ) {
      mov_av[i] = mov_av[i-1] + clayer[i].emip;
    }
    for ( unsigned i = win_mov_av; i < EHLAY; i++ ) {
      mov_av[i] = mov_av[i-1] - clayer[i - win_mov_av].emip + clayer[i].emip;
    }
    for ( unsigned i = 1; i < win_mov_av; i++ ) {
      mov_av[i] /= i + 1;
    }
    for ( unsigned i = win_mov_av; i < EHLAY; i++ ) {
      mov_av[i] /= win_mov_av;
    }
//-------------------------------------------------------------------------------------
//          Muons, trash,  and shower start 
//-------------------------------------------------------------------------------------
// identify muon
     muonID = isMuon(eeh, et);
    if ( muonID ) {
      if ( !std::strcmp(_ptype.data(),"muon__") ) {
	sh_st = EHLAY;
	decision = true;
      } else 
	return;
    } else {
      sh_st = getStartingLayer(clayer);
    }

// identify trash
    if ( !muonID ) trashID = isTrash(sh_st, e_all);
    if ( trashID ) return;

//-----------------------------------------------------------------------------------------------
//     Find track
//-----------------------------------------------------------------------------------------------
// find track up to found shower start
    if ( (int(sh_st) - int(calor_front)) <= _absTrackLengthLimit )  
      track_found = false;
    else 
      track_found = FindTrack(sh_st, clayer);
//-----------------------------------------------------------------------------------------------
// Additional trash selection for low energies
//-----------------------------------------------------------------------------------------------    
    if ( (_beamEnergy <= ELIMIT) && !muonID && !trashID && !track_found
	 && (sh_st > unsigned(_absTrackLengthLimit+1)) && (sh_st < ELAY) ) return;
//-----------------------------------------------------------------------------------------------
// Exclude additional multiparticle events with parallel tracks
//-----------------------------------------------------------------------------------------------
    if ( paraID ) return; // drop events with parallel tracks
//-----------------------------------------------------------------------------------------------
//     Make decision
//-----------------------------------------------------------------------------------------------
    if ( !muonID ) {
      if ( cher ) {
	if ( (!std::strcmp(_ptype.data(),"pionpl")) )
	  decision = true;
	if ( (!std::strcmp(_ptype.data(),"electr")) ||
	     (!std::strcmp(_ptype.data(),"positr")) ) {
	  if ( calor_front > 0 ) 
	    decision = true;
	  else {
	    if ( (ee/e_all > 0.95) && (int(sh_st) < _absTrackLengthLimit) )
	      decision = true;
	  }
	} 
      }	else {
	if ( (!std::strcmp(_ptype.data(),"proton")) ||
	     (!std::strcmp(_ptype.data(),"pionmi")) ) 
	  decision = true;
      }
    }
    if ( !decision ) return; //  drop event if no decision
//-------------------------------------------------------------------------------------
//  Calculate shifts for x and y
//-------------------------------------------------------------------------------------
    if ( track_found ) {
      float sigma = 0.0;
      for (std::vector<CHit*>::iterator p = intrack.begin();
	   p != intrack.end(); p++) {
	float wi = 4./((*p)->w)/((*p)->w);
	(*p)->tr = true;
	sigma += wi;
	xtrack += ((*p)->x)*wi;
	ytrack += ((*p)->y)*wi;
      }
      if (sigma > 0.0) {
	xtrack /= sigma;
	ytrack /= sigma;
      }
      xshift = xtrack;
      yshift = ytrack;
      shift_type = 1;
    } else if ( dc_track ) {
	xshift = xdc;
	yshift = ydc;
	shift_type = 2;
    }   //  End of   if ( track_found ) 

//  Drop event if shift greater than calorimeter width
    if ( (fabs(xshift) > _calWidth) ||(fabs(yshift) > _calWidth) ) return;
//-------------------------------------------------------------------------------------
    nevt_sel++;
//-------------------------------------------------------------------------------------
//        Calculate r of shower and e of shower 
//-------------------------------------------------------------------------------------
    for ( unsigned i = 0; i < sh_st; i++ ) {
      Etrack_mip += clayer[i].emip;
      if ( i < ELAY1 )
	Etrack += clayer[i].emip*_mip2gevECAL1;
      else if ( i < ELAY2 )
	Etrack += clayer[i].emip*_mip2gevECAL2;
      else if ( i < ELAY )
	Etrack += clayer[i].emip*_mip2gevECAL3;
      else 
	Etrack += clayer[i].emip*_mip2gevHCAL1;
    }
    if ( !muonID ) {
      for ( unsigned i = sh_st; i < EHLAY; i++ ) {
	if ( i < ELAY1 )
	  Eshower += clayer[i].emip*_mip2gevECAL1;
	else if ( i < ELAY2 )
	  Eshower += clayer[i].emip*_mip2gevECAL2;
	else if ( i < ELAY )
	  Eshower += clayer[i].emip*_mip2gevECAL3;
	else 
	  Eshower += clayer[i].emip*_mip2gevHCAL1;
      }
      float esh95 = 0.95*(Eshower + et);

      if ( Eshower >= esh95 ) {
	float e95 = 0.95*Eshower;
	float eer[_rbin];
	for ( int ii = 0; ii < _rbin; ii++ ) { eer[ii] = 0.0; }
	for ( unsigned i = 0; i < nhitE+nhitH; i++ ) {
	  float eem = chit[i].hit_addr->getEnergy();
	  if ( (chit[i].l < sh_st) || ( eem < _noiseMIPLimit ) ) continue;
	  float x = chit[i].x - xshift;
	  float y = chit[i].y - yshift;
	  int j = int(sqrt(x*x + y*y)/dr);
	  if ( j < _rbin ) {
	    if ( chit[i].l < ELAY1 )
	      eer[j] += eem*_mip2gevECAL1;
	    else if ( chit[i].l < ELAY2 )
	      eer[j] += eem*_mip2gevECAL2;
	    else if ( chit[i].l < ELAY )
	      eer[j] += eem*_mip2gevECAL3;
	    else 
	      eer[j] += eem*_mip2gevHCAL1;
	  }
	}
	int k = 0;
	float ersum = eer[k];
	while ( k < _rbin-1 && ersum < e95 ) {
	  k++;
	  ersum += eer[k];
	}
	Rshower = float(k)*dr;
      } else {
	Rshower = 0.0;
      }
    }
//----------------------------------------------------------------------------
//  Draw and print if flag on 
//----------------------------------------------------------------------------
    if ( _fDraw ) {
      cout << "     Event # " << nevt-1 << "  FIP at " << sh_st << "  Decision: " << decision << endl;
      cout << " shift type =  " << shift_type << ":  ";
      if(shift_type == 1)
	cout << "xtrack " << xtrack << ", ytrack " << ytrack << endl;
      else if(shift_type == 2)
	cout << "xdc    " << xdc    << ", ydc    " << ydc << endl;
      else
	cout << " ++++++++++++ error shift type =  "<< shift_type << endl;

      cout<<" ECAL: "<<ncutE<<"  E = "<<esumEm<<" MIPs  or "<< ee <<" GeV"<<endl;
      cout<<" HCAL: "<<ncutH<<"  E = "<<esumHm<<" MIPs  or "<< esumHg <<" GeV"<<endl;
      cout<<" TCAL: "<<ncutT<<"  E = "<<esumTm<<" MIPs  or "<< et <<" GeV"<<endl;
      cout<<" Track: "<<intrack.size()<<"  E = "<< Etrack_mip <<" MIPs  or "<< Etrack <<" GeV"<<endl;

      if ( track_found ) {
	int inp = intrack.size();
	float zl1 = intrack[0]->z;
	float zl2 = intrack[inp-1]->z;
	if (_beamAngle) {
	  float xl1 = intrack[0]->x;
	  float xl2 = intrack[inp-1]->x;
	  float yl1 = intrack[0]->y;
	  float yl2 = intrack[inp-1]->y;
	  ced_line(xl1-xshift, yl1-yshift, zl1, xl2-xshift, yl2-yshift, zl2, L3|1,3, YELLOW);
	} else 
	  ced_line(xtrack-xshift, ytrack-yshift, zl1, xtrack-xshift, ytrack-yshift, zl2, L3|1,3, YELLOW);
	for (std::vector<CHit*>::iterator p = intrack.begin();
	     p != intrack.end(); p++) {
	  ced_hit( (*p)->x, (*p)->y, (*p)->z,L1,3, PINK);
	}
      } else {
	cout << "   Track was not found" << endl;
      }
      //  ECAL
      for ( unsigned i = 0; i < nhitE; i++ )
	if(chit[i].hit_addr->getEnergy() > 0.5)
	  ced_hit(chit[i].x-xshift,chit[i].y-yshift,chit[i].z,L2|2,3, RED );

      //  HCAL
      for ( unsigned i = nhitE; i < nhitE+nhitH; i++ )
	if(chit[i].hit_addr->getEnergy() > 0.5)
	  ced_hit(chit[i].x-xshift,chit[i].y-yshift,chit[i].z,L2|2,3, GREEN );

      // starting layer
      for(std::vector<CHit*>::iterator p = clayer[sh_st].vl.begin();
	  p != clayer[sh_st].vl.end(); p++) 
	ced_hit((*p)->x-xshift,(*p)->y-yshift,(*p)->z, L3|3, 5, WHITE);

      ced_send_event();
      getchar();
      ced_new_event();
    }
//--------------------------------------------------------------------------------------
//  Count events and fill hists
//--------------------------------------------------------------------------------------
    if ( _fRootOut ) {
      Cher_trig->Fill(cher, 1.0);
      StartLayer->Fill(sh_st);
      if (shift_type == 1) TrackXY_E->Fill(xtrack, ytrack, 1.0);
      if (shift_type == 2) TrackDC->Fill(xdc, ydc, 1.0);
      Etrack_dist->Fill(Etrack, 1.0);
      Esum_dist->Fill(e_all, 1.0);
      if ( Eshower >= 0.95*(Eshower+et) ) {
	Esum95_dist->Fill(eeh, 1.0);
        Eshower_dist->Fill(Eshower, 1.0);
      }
      if ( Rshower > 0.0 ) Rshower_dist->Fill(Rshower, 1.0);
    }
//--------------------------------------------------------------------------------------
//   Prepare collections
//--------------------------------------------------------------------------------------
    EVENT::LCCollection* ShowerStart = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT);
    IMPL::LCGenericObjectImpl* ShowerStartLayerData = new IMPL::LCGenericObjectImpl;
    int cal_type = int(sh_st/ELAY);
    int st_lay = int(sh_st) - cal_type*int(ELAY);
    ShowerStartLayerData->setIntVal(   0, cal_type );
    ShowerStartLayerData->setIntVal(   1, st_lay );
    ShowerStartLayerData->setIntVal(   2, shift_type );
    ShowerStartLayerData->setFloatVal( 3, Eshower ); // shower energy in ECAL+HCAL [GeV]
    ShowerStartLayerData->setFloatVal( 4, Rshower ); // 95% of shower energy inside r [mm]
    ShowerStartLayerData->setFloatVal( 5, esumEg1 ); // for hits with e > _noiseMIPLimit
    ShowerStartLayerData->setFloatVal( 6, esumEg2 ); // for hits with e > _noiseMIPLimit
    ShowerStartLayerData->setFloatVal( 7, esumEg3 ); // for hits with e > _noiseMIPLimit
    ShowerStartLayerData->setFloatVal( 8, esumHg  ); // for hits with e > _noiseMIPLimit
    ShowerStartLayerData->setFloatVal( 9, esumTg1 ); // for hits with e > _noiseMIPLimit
    ShowerStartLayerData->setFloatVal(10, esumTg2 ); // for hits with e > _noiseMIPLimit
    ShowerStart->addElement( ShowerStartLayerData );
    evt->addCollection( ShowerStart, COL_ADD );
    //    cout << "Add collection ADDS" << endl;
//-----------------------------------------------------------------
    float pos[3];
// collection for ECAL
    EVENT::LCCollection* EcalHits = new IMPL::LCCollectionVec( LCIO::CALORIMETERHIT );
    for ( unsigned i = 0; i < nhitE; i++ ) {
      IMPL::CalorimeterHitImpl* EcalHitsData = new IMPL::CalorimeterHitImpl;
      pos[0] = chit[i].x - xshift;
      pos[1] = chit[i].y - yshift;
      pos[2] = chit[i].z;
      //if ( chit[i].hit_addr->getEnergy() > 0.5 )
      //cout << "x = " << pos[0]  << " y = " << pos[1]  <<  " z = " << pos[2] << " e = " << chit[i].hit_addr->getEnergy() << endl;
    
// Add data to the collection element
      EcalHitsData->setCellID0(chit[i].hit_addr->getCellID0());
      EcalHitsData->setCellID1(chit[i].hit_addr->getCellID1());
      EcalHitsData->setTime(chit[i].hit_addr->getTime());
      EcalHitsData->setPosition(pos);
      EcalHitsData->setType(int(chit[i].l));
      EcalHitsData->setEnergy(chit[i].hit_addr->getEnergy());
// Add element to the collection
      EcalHits->addElement( EcalHitsData );
    }
// Add collection to the event
    evt->addCollection( EcalHits, COL_ECAL );
    //    cout << "Add collection ECAL" << endl;
// collection for HCAL
//    cout << "HCAL" << endl;

    EVENT::LCCollection* HcalHits = new IMPL::LCCollectionVec( LCIO::CALORIMETERHIT );
    for ( unsigned i = nhitE; i < nhitE+nhitH; i++ ) {
      IMPL::CalorimeterHitImpl* HcalHitsData = new IMPL::CalorimeterHitImpl;
      pos[0] = chit[i].x - xshift;
      pos[1] = chit[i].y - yshift;
      pos[2] = chit[i].z;
      //cout << "x  " << pos[0]  << " y  " << pos[1]  <<  " z  " << pos[2] <<  " e " << chit[i].hit_addr->getEnergy() << endl;	    
// Add data to the collection element
      HcalHitsData->setCellID0(chit[i].hit_addr->getCellID0());
      HcalHitsData->setCellID1(chit[i].hit_addr->getCellID1());
      HcalHitsData->setTime(chit[i].hit_addr->getTime());
      HcalHitsData->setPosition(pos);
      HcalHitsData->setType(int(chit[i].l));
      HcalHitsData->setEnergy(chit[i].hit_addr->getEnergy());
// Add element to the collection
      HcalHits->addElement( HcalHitsData );
    }
// Add collection to the event
     evt->addCollection( HcalHits, COL_HCAL );
    //    cout << "Add collection HCAL" << endl;
// collection for TCAL
    EVENT::LCCollection* TcalHits = new IMPL::LCCollectionVec( LCIO::CALORIMETERHIT );
    for ( unsigned i = nhitE+nhitH; i < nhitE+nhitH+nhitT; i++ ) {
      IMPL::CalorimeterHitImpl* TcalHitsData = new IMPL::CalorimeterHitImpl;
      pos[0] = chit[i].x - xshift;
      pos[1] = chit[i].y - yshift;
      pos[2] = chit[i].z;	    
// Add data to the collection element
      TcalHitsData->setCellID0(chit[i].hit_addr->getCellID0());
      TcalHitsData->setCellID1(chit[i].hit_addr->getCellID1());
      TcalHitsData->setTime(chit[i].hit_addr->getTime());
      TcalHitsData->setPosition(pos);
      TcalHitsData->setType(int(chit[i].l));
      TcalHitsData->setEnergy(chit[i].hit_addr->getEnergy());
// Add element to the collection
      TcalHits->addElement( TcalHitsData );
    }
// Add collection to the event
    evt->addCollection( TcalHits, COL_TCAL );
    //    cout << "Add collection TCAL" << endl;

    // collection for track
    if ( track_found ) {
      EVENT::LCCollection* PrimaryTrack = new IMPL::LCCollectionVec( LCIO::CALORIMETERHIT );
      for (std::vector<CHit*>::iterator p = intrack.begin();
	   p != intrack.end(); p++) {
	IMPL::CalorimeterHitImpl* PrimaryTrackHitsData = new IMPL::CalorimeterHitImpl;
	pos[0] = (*p)->x - xshift;
	pos[1] = (*p)->y - yshift;
	pos[2] = (*p)->z;
// Add data to the collection element
//	       PrimaryTrackHitsData->id() = (*p)->hit_addr->id();
	PrimaryTrackHitsData->setCellID0((*p)->hit_addr->getCellID0());
	PrimaryTrackHitsData->setCellID1((*p)->hit_addr->getCellID1());
	PrimaryTrackHitsData->setTime((*p)->hit_addr->getTime());
	PrimaryTrackHitsData->setPosition(pos);
	PrimaryTrackHitsData->setType(int((*p)->l));
	PrimaryTrackHitsData->setEnergy((*p)->hit_addr->getEnergy());
// Add element to the collection
	PrimaryTrack->addElement( PrimaryTrackHitsData );
      }
// Add collection to the event
       evt->addCollection( PrimaryTrack, COL_TRACK );
      //      cout << "Add collection PTRK" << endl;
    }
//--------------------------------------------------------------
// Drop collections except needed
    const std::vector<std::string>* colNames = evt->getCollectionNames();
    for( StringVec::const_iterator name = colNames->begin();
	 name != colNames->end() ; name++ ){
      
      LCCollectionVec*  col =  dynamic_cast<LCCollectionVec*> (evt->getCollection( *name ) );

      if ( std::strcmp((*name).data(),"ECAL") 
	   && std::strcmp((*name).data(),"HCAL") 
	   && std::strcmp((*name).data(),"TCAL")
	   && std::strcmp((*name).data(),"PTRK")
	   && std::strcmp((*name).data(),"ADDS") ) {
	col->setTransient( true );
	//	cout << "Collection: " << (*name).c_str() << " dropped" << endl;
      }
      else {
	const std::string & tname = evt->getCollection( *name )->getTypeName();
	if (tname == LCIO::CALORIMETERHIT ) {
	  LCFlagImpl hitFlag( col->getFlag() );
	  hitFlag.setBit( LCIO::CHBIT_LONG );
	  col->setFlag(  hitFlag.getFlag() );
	}
	//	cout << "Collection: " << (*name).c_str() << " kept" << endl;
      }
    }
    /*
    if ( _fDraw ) {
      for (int i = 0; i < EcalHits->getNumberOfElements(); i++) {
	CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>(EcalHits->getElementAt(i));
	float x = hit->getPosition()[0];
	float y = hit->getPosition()[1];
	float z = hit->getPosition()[2];
	//	cout<<i<<" xyz "<<x<<" : "<<y<<" : "<<z<<endl;
	if(hit->getEnergy() > 0.5)
	  ced_hit(x,y,z,L1|0,3,CC_Orange[18]);
      }
      for ( int i = 0; i < HcalHits->getNumberOfElements(); i++) {
	CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>(HcalHits->getElementAt(i));
	float x = hit->getPosition()[0];
	float y = hit->getPosition()[1];
	float z = hit->getPosition()[2];
	//	cout<<i<<" xyz "<<x<<" : "<<y<<" : "<<z<<endl;
	if(hit->getEnergy() > 0.5)
	  ced_hit(x,y,z,L1|0,3,CC_LightBlue[18]);
      }

      ced_send_event();
      getchar();
      ced_new_event();

      return;
    }
    */


    //  ced_send_event();
    //UTIL::LCTOOLS::dumpEventDetailed(evt);
    //  getchar();

    lcwriter->writeEvent(evt);
    //cout << "Event " << nevt << endl; 
//---------------------------------------------------------------------
//         Check collection 
//     Output to debug root-file
//    and an example of getting data from this collection
//---------------------------------------------------------------------
//    if ( !(nevt_sel%1000) ) {
//      cout << "Event: " << nevt << "(" << nevt_sel << ")" << endl; 
//      UTIL::LCTOOLS::dumpEventDetailed(evt);
//      getchar();
      //    }

    //    if (_fRootOut) {
    //	int nE = 0;
    //	int nH = 0;
    /*
    LCCollection* col = NULL;
    unsigned nhit = 0;
	const std::vector<std::string>* sVec = evt->getCollectionNames();

	for( std::vector<std::string>::const_iterator name = sVec->begin();
	     name != sVec->end() ; name++){
	    const std::string & tname = evt->getCollection( *name )->getTypeName();
      
	    if(tname == LCIO::CALORIMETERHIT ) {
		if(!std::strcmp((*name).data(),"HCAL")) {
		    col = evt->getCollection( *name );
		    nhit =  col->getNumberOfElements(); // number of track hits
		}
	    }
	}
	for(unsigned i = 0; i < nhit; i++) {
	    CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col->getElementAt( i ) );
	    cout << "l = " << hit->getType() << ", x= " << hit->getPosition()[0] << ", y= " << hit->getPosition()[1] << ", z= " << hit->getPosition()[2] << endl; 
	}
	getchar();
    */

  } // if tflag true
} // processEvent
//=============================end======================================
void CALICEOverlayPreparation::end(){

  //  lcwriter->close();

  cout << "DC X: " << xy_dc->GetMean(1) << " - " << -ALIGN[num_align][1] << " = " << xy_dc->GetMean(1)+ALIGN[num_align][1] << endl;
  cout << "DC Y: " << xy_dc->GetMean(2) << " - " << -ALIGN[num_align][2] << " = " << xy_dc->GetMean(2)+ALIGN[num_align][2] << endl;
  cout << "ECAL X: " << xyCOG_ecal->GetMean(1) << " - " << -ALIGN[num_align][3] << " = " << xyCOG_ecal->GetMean(1)+ALIGN[num_align][3] << endl;
  cout << "ECAL Y: " << xyCOG_ecal->GetMean(2) << " - " << -ALIGN[num_align][4] << " = " << xyCOG_ecal->GetMean(2)+ALIGN[num_align][4] << endl;
  cout << "HCAL X: " << xyCOG_hcal->GetMean(1) << " - " << -ALIGN[num_align][5] << " = " << xyCOG_hcal->GetMean(1)+ALIGN[num_align][5] << endl;
  cout << "HCAL Y: " << xyCOG_hcal->GetMean(2) << " - " << -ALIGN[num_align][6] << " = " << xyCOG_hcal->GetMean(2)+ALIGN[num_align][6] << endl;

  cout << "From " << nevt << " events " << nevt_sel << " events selected" << endl;
  cout << "SLCIO output into: " << outputFileName << endl;

    if (_fRootOut) {
      rootfile->Write();
      rootfile->Close();
      cout << "ROOT output file: " << rootfilename << endl;
    }  
} //end 
