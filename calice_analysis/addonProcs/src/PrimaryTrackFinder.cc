#include "PrimaryTrackFinder.hh"
#include <fstream>
#include <cstring>

#include "MappingProcessor.hh"
#include "marlin/Exceptions.h"

using namespace marlin;
using namespace lcio;
using namespace std;

/*----------------------------------------------------------------------------
  Primary Track Finder
  version 4.01 
  November 2010
  Uses CalorimeterHit collections data
  to find shower start and primary track in ECAL & HCAL.
  Detector configuration for CERN 2007 runs
  (30-layer ECAL & 38-layer HCAL)
  
  @author M. V. Chadeeva
  ----------------------------------------------------------------------------*/


/*===================================================================*/
PrimaryTrackFinder aPrimaryTrackFinder;

/*================ Constructor =======================================*/
PrimaryTrackFinder::PrimaryTrackFinder() : Processor("PrimaryTrackFinder")
{
  _description = "Primary Track Finder";

  /* run information*/
  registerProcessorParameter("BeamEnergy", "Beam energy in GeV",
			     _beamEnergy, float (0.0) );
  registerProcessorParameter("BeamAngle", "Beam incident angle in degrees",
			     _beamAngle, float (0.0) );
  registerProcessorParameter("XAlignment", "X alignment ECAL-HCAL in mm",
			     _x_align, float (0.0) );
  registerProcessorParameter("YAlignment", "Y alignment ECAL-HCAL in mm",
			      _y_align, float (0.0) );
  /* noise cut*/
  registerProcessorParameter("NoiseMIPLimit", "Noise threshold for hit energy in MIPs",
			     _MIPLimit, float (0.5) );

  /* CED drawing*/
  registerProcessorParameter("DrawFlag", "Flag for CED drawing",
			     _fDraw, bool (false) );
  registerProcessorParameter("CED_HostPort", "Host Port number for CED",
			     _cedhost, int (7557) );
  registerProcessorParameter("CEDPointSize", "Point size for CED drawing",
			     _pointSize, int (5) );
  registerProcessorParameter("CEDLineSize", "Line size for CED drawing",
			     _lineSize, int (2) );

  /* ROOT output*/
  registerProcessorParameter("RootOutputFlag", "Flag for output to Root-file",
			     _fRootOut, bool (false) );
  registerProcessorParameter("RootOutputFileDir", "Directory for output ROOT-file",
			     _rootfiledir, std::string ("./") );

  /*input collection names*/
  registerProcessorParameter("ecalHitsCollection", "Name of ECAL hits collection",
			     _ecalcol, std::string ("EmcCalorimeter_Hits") );
  registerProcessorParameter("hcalHitsCollection", "Name of HCAL hits collection",
			     _hcalcol, std::string ("AhcCalorimeter_Hits") );
  registerProcessorParameter("tcmtHitsCollection", "Name of TCMT hits collection",
			     _tcmtcol, std::string ("TcmtCalorimeter_Hits") );
 
  /* output collection names*/
  registerProcessorParameter("OutputTrackCollectionName", "Name of track hits collection",
			     _prtrackColName, std::string ("PrimaryTrackHits") );
  registerProcessorParameter("OutputStartCollectionName", "Name of shower start collection",
			     _startColName, std::string ("ShowerStartingLayer") );
 
  /* starting layer finder*/
  registerProcessorParameter("FixStartingLayer", "Skip starting layer finding procedure",
			     _fixStartingLayer, bool (false) );
  
  /* track finder*/
  registerProcessorParameter("SkipTrackFinder", "Skip primary track finding procedure",
			     _skipTrackFinder, bool (false) );
  registerProcessorParameter("SizeofGAP", "Maximum gap between track points",
			     _NGAP, int (6) );
  registerProcessorParameter("AbsTrackLengthLimit", "Length limit for saved tracks",
			     _absTrackLengthLimit, int (3) );
  registerProcessorParameter("RelTrackLengthLimit", "Minimum track length relative to shower start",
			     _relTrackLengthLimit, float (0.3) );
  
  registerProcessorParameter("IsMonteCarlo", "Is Monte Carlo file (i.e. no triggers will be used)",
			     _isMC, bool (false) );

  registerProcessorParameter("DoWriteSubset", "Do not write a new CalorimeterHit collection, but create"
			     " a subset of the original collection",
			     _doWriteSubset, bool (false) );

  registerProcessorParameter( "MappingProcessorName" ,
			      "Name of the MappingProcessor instance that provides"
			      " the geometry of the detector." ,
			      _mappingProcessorName,
			      std::string("MyMappingProcessor") );
}


/*====================destructor========================================================*/
PrimaryTrackFinder::~PrimaryTrackFinder()
{}

/*===================ini function=====================================================*/
void PrimaryTrackFinder::init() 
{
  if (_doWriteSubset == true)
    {
      _originalHitsContainer = NULL;
      _mapper = MappingProcessor::getMapper(_mappingProcessorName);
      
      std::stringstream message;
      bool error = false;
      
      if ( ! _mapper )
	{
	  message << "MappingProcessor::getMapper("<< _mappingProcessorName 
		  << ") did not return a valid mapper." << std::endl;
	  error = true;
	}
      if (error) 
	{
	  streamlog_out(ERROR) << message.str();
	  throw marlin::StopProcessingException(this);
	}  
    }
  
  /*------------------------------------------------------
    Initialize CED
    ------------------------------------------------------*/
  
  if (_fDraw) {
    ced_client_init("localhost", _cedhost); // connection to CED
    ced_register_elements();
    ced_new_event();
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    std::cout << "  Connection to CED through port " << _cedhost << endl;
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
  }

  /* tangent of beam angle*/
  _tangentBeamAngle = tan(_beamAngle*M_PI/180.0);
  streamlog_out(DEBUG) << "Angle = " << _beamAngle << "; tan = " << _tangentBeamAngle << endl;

  /* starting layer finding criteria for given beam energy*/
  _minMIPfromBeamEnergy = 6.0 + 0.1*_beamEnergy;
  _minHitsFromBeamEnergy = int(3.77 + 1.44*std::log(_beamEnergy) + 0.5);
  _movingAverageWindow = 10;

  /* calorimeter front: 0 with ECAL, 30 for HCAL only*/
  if ( runnum > 350000 ) calor_front = 30;
  else calor_front = 0;

  /* muon coefs*/
  if ( _beamEnergy > BEAMLIMIT ) 
    {
      double x1 = 1.0;
      double y1 = 0.5;
      double x2 = 7.0;
      double y2 = 2.0;
      double x3 = 1.5;
      double y3 = 7.0;
      amu12 = (y1 - y2)/(x1 - x2); // 0.25
      bmu12 = y1 - amu12*x1;         // 0.25
      amu23 = (y2 - y3)/(x2 - x3); // -0.9090909
      bmu23 = y2 - amu23*x2;         // 8.364
      amu13 = (y1 - y3)/(x1 - x3); // 13.
      bmu13 = y1 - amu13*x1;         // -12.5
    } 
  else 
    {
      double x1 = 1.0;
      double y1 = 0.8;
      double x2 = 4.0;
      double y2 = 1.0;
      double x3 = 1.5;
      double y3 = 6.0;
      amu12 = (y1 - y2)/(x1 - x2);  
      bmu12 = y1 - amu12*x1;       
      amu23 = (y2 - y3)/(x2 - x3); 
      bmu23 = y2 - amu23*x2;       
      amu13 = (y1 - y3)/(x1 - x3); 
      bmu13 = y1 - amu13*x1;       
    }

  /* global run counters*/
  nevt = 0;
  n_muon_like = 0;
  n_short = 0;
  n_short_muon = 0;
  n_para = 0;
  n_para_muon = 0;
  n_muon_track = 0;
  n_hadr_track = 0;
  n_early_shower = 0;
}


/*============================= read run header ==================================*/
void PrimaryTrackFinder::processRunHeader(LCRunHeader* run) 
{
  runnum = run->getRunNumber();
  
  if ( runnum > 350000 ) calor_front = ELAY; // case of HCAL only
  else calor_front = 0;
  
  if (_fRootOut)
    {
      char s[128];
      sprintf(s,"%s/primarytrack_%06d.root", _rootfiledir.c_str(), runnum);
      /* open ROOT file*/
      rootfile = new TFile( s, "RECREATE");
      // hists booking
      TrackHitMIP = new TH1F("TrackHitMIP","Energy distribution for primary track hits (MIP)",50,0.0,5.0);
      TrackHitMIP->GetXaxis()->SetTitle("Energy in track hits [MIP]");
      TrackXY_E = new TH2F("TrackXY_E","Average track coordinates in XY-plane in ECAL",100,-100.0,100.0,100,-100.0,100.0);
      TrackXY_E->GetXaxis()->SetTitle("X [mm]");
      TrackXY_E->GetYaxis()->SetTitle("Y [mm]");
      TrackXY_H = new TH2F("TrackXY_H","Average track coordinates in XY-plane in HCAL",100,-200.0,200.0,100,-200.0,200.0);
      TrackXY_H->GetXaxis()->SetTitle("X [mm]");
      TrackXY_H->GetYaxis()->SetTitle("Y [mm]");
      StartLayer = new TH1I("StartLayer","Shower starting layer distribution", EHLAY+1, 0, int(EHLAY+1));
      StartLayer->GetXaxis()->SetTitle("Shower starting layer from ECAL front");
      StartLayer_HCAL1 = new TH1I("StartLayer_HCAL1","Shower starting layer distribution in fine HCAL", 30, 0, 30);
      StartLayer_HCAL1->GetXaxis()->SetTitle("Shower starting layer from HCAL front");
      StartLayer_HCAL2 = new TH1I("StartLayer_HCAL2","Shower starting layer distribution in coarse HCAL", 9, 0, 9);
      StartLayer_HCAL2->GetXaxis()->SetTitle("Shower starting layer from 30th HCAL layer");
    }
}

/*=============================process event ==================================*/
void PrimaryTrackFinder::processEvent(LCEvent* evt)
{ 	 
  /*------------------------------------------
    CalorimeterHit collections*/
  nhitE = 0;
  nhitH = 0;
  nhitT = 0;
  nhitE_cut = 0;
  nhitH_cut = 0;
  unsigned nhit = 0; /*for track*/
  LCCollection *col_E = NULL;
  LCCollection *col_H = NULL;
  LCCollection *col_T = NULL;
  LCCollection *col   = NULL; /* for track*/
  LCCollection *col_S = NULL; /* for start*/

  /* Average track coordinates*/
  float x_av_E = 0.0;
  float y_av_E = 0.0;
  float x_av_H = 0.0;
  float y_av_H = 0.0;
  float xtrack = 0.0;
  float ytrack = 0.0;

  /* Energy sums*/
  float energySumEcalHcal = 0.0; //*Energy sum for ECAL+HCAL*/
  float energySumTcmt = 0.0;  /* Energy sum for TCMT*/
  bool track_found = false;
  bool muon = false;
  unsigned sh_st = EHLAY;
  std::vector<CHit*> finalTrack;
  intrack.clear();
  paraID = false;
  shortID = false;

  /*----------------------------------------------- 
    Trigger check*/
  TriggerBits evt_trg(((LCEvent *) evt)->getParameters().getIntVal(PAR_TRIGGER_EVENT));
  bool isBeamTrigger = evt_trg.isPureBeamTrigger() || evt_trg.isBeamTrigger();
  
  streamlog_out(DEBUG)<<"\n\n ------------------------------------------------------------------"<<endl;
  streamlog_out(DEBUG)<<"EVENT "<<evt->getEventNumber()<<" isMC: "<<_isMC<<" isBeamTrigger: "<<isBeamTrigger<<endl;

  if (_isMC == false)
    {
      if (evt_trg.isPurePedestalTrigger() || evt_trg.isPedestalTrigger()) return;
      if (evt_trg.isCalibTrigger()) return;       
      if (evt_trg.isPureCalibTrigger()) return; 
    }
      
  if (!(isBeamTrigger || (_isMC == true)) ) return;
 
  nevt++;

  /* get collection names*/
  const std::vector<std::string>* strVec = evt->getCollectionNames();
  
  if (_doWriteSubset)
    {
      /*clean up the container at the beginning of the event*/
      delete _originalHitsContainer;
      _originalHitsContainer = 0;
    }


  /* resolve collection names*/
  for( std::vector<std::string>::const_iterator name = strVec->begin(); name != strVec->end() ; name++)
    {
      const std::string & tname = evt->getCollection( *name )->getTypeName();
      
      if(tname == LCIO::CALORIMETERHIT )
	{
	  /* if EmcCalorimeter_Hits*/
	  if(!std::strcmp((*name).data(), _ecalcol.c_str()))
	    {	  
	      col_E = evt->getCollection( *name );
	      nhitE =  col_E->getNumberOfElements(); /*number of hits in ECAL*/
	    }

	  /* end if AhcCalorimeter_Hits*/
	  if(!std::strcmp((*name).data(), _hcalcol.c_str()))
	    {
	      col_H = evt->getCollection( *name );
	      nhitH =  col_H->getNumberOfElements(); /*number of hits in HCAL*/
	      streamlog_out(DEBUG)<<"Hits in HCAL: "<<nhitH<<endl;

	      if (_doWriteSubset)
		{
		  this->fillOriginalHitsContainer(col_H);
		}
	    }
	  
	  /* if TcmtCalorimeter_Hits*/
	  if(!std::strcmp((*name).data(), _tcmtcol.c_str()))
	    {
	      col_T = evt->getCollection( *name );
	      nhitT =  col_T->getNumberOfElements(); /*number of hits in TCMT*/
	    } 

	} /* if CALORIMETERHIT*/
    } /* end for Collection names*/
 
  streamlog_out(DEBUG) <<"\n Event "<<evt->getEventNumber()<< " Ecal: " << nhitE << "; Hcal: " << nhitH << "; Tcmt: " << nhitT << endl;

  /* Adjacent arrays for track finder*/
  CLayer clayer[EHLAY+1];
  CHit chit[nhitE+nhitH];
  
  /* ------  fill hit data in CHit --------------------------------------------------------*/
  /* ECAL*/
     for ( unsigned i = 0; i < nhitE; i++ ) 
       {
	 CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col_E->getElementAt( i ) );
	 
	 CellIndex cellIndex(hit->getCellID0());
	 unsigned layer = unsigned(cellIndex.getLayerIndex() - 1); // layer number
	 chit[i].l = layer;
	 chit[i].x = hit->getPosition()[0] + _x_align;
	 chit[i].y = hit->getPosition()[1] + _y_align;
	 chit[i].z = hit->getPosition()[2];
	 chit[i].e = hit->getEnergy();
	 chit[i].c = s1ECAL;
	 
	 if ( chit[i].e >= _MIPLimit ) 
	   {
	     nhitE_cut++;
	     if ( layer == (ELAY - 1) ) 
	       chit[i].r = SQ3;
	     else 
	       chit[i].r = SQ1; // or 1.85 ?????? due to dead areas in ECAL;

	     clayer[layer].vl.push_back(&chit[i]);
	     clayer[layer].nh++;
	     clayer[layer].emip += chit[i].e;

	     float eg;
	     if(layer < ELAY1) 
	       eg = chit[i].e*mip2gevECAL1;
	     else if(layer < ELAY2) 
	       eg = chit[i].e*mip2gevECAL2;
	     else 
	       eg = chit[i].e*mip2gevECAL3;
	     energySumEcalHcal += eg;
	   }
       } /* for hits ECAL*/

     streamlog_out(DEBUG)<<" \n\n Loop over HCAL "<<endl;
     
     /*HCAL*/
     for ( unsigned i = 0; i < nhitH; i++ ) 
       {
	 unsigned ihit = i + nhitE;
	 
	 CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col_H->getElementAt( i ) );
	 
	 HcalCellIndex cellIndex(hit->getCellID0());
	 unsigned layer = unsigned(cellIndex.getLayerIndex() - 1); // layer number to index 
	 chit[ihit].l = unsigned(layer) + ELAY; 
	 chit[ihit].x = hit->getPosition()[0];
	 chit[ihit].y = hit->getPosition()[1];
	 chit[ihit].z = hit->getPosition()[2];
	 chit[ihit].e = hit->getEnergy();
	 chit[ihit].cellID = hit->getCellID0();
	 
	 if (  chit[ihit].e < _MIPLimit )  continue;
	   
	 nhitH_cut++;


	 //cout<<"   x="<<hit->getPosition()[0]<<" y="<<hit->getPosition()[1]<<" z="<<hit->getPosition()[2]<<endl;

	 /*----------------------------------------------------------------
	  fine layers*/
	 if ( chit[ihit].l < HTAIL ) 
	   {
	     if ( (fabs(chit[ihit].x) < b3) && (fabs(chit[ihit].y) < b3) ) 
	       {
		 chit[ihit].c = s3HCAL;

		 if ( chit[ihit].l == HTAIL-1 )  
		   {
		     chit[ihit].r = SQ6;
		   }
		 else 
		   {
		     if ( (fabs(chit[ihit].x) < b3l) || (fabs(chit[ihit].y) < b3l) ) 
		       chit[ihit].r = SQ3;
		     else 
		       chit[ihit].r = SQ3*1.5; //0.5*(s3HCAL + s6HCAL);
		   }
	       }
	     else if ( (fabs(chit[ihit].x) < b6) && (fabs(chit[ihit].y) < b6) ) 
	       {
		 chit[ihit].c = s6HCAL;
		 if ( (fabs(chit[ihit].x) < b6l) || (fabs(chit[ihit].y) < b6l) ) 
		   chit[ihit].r = SQ6;
		 else chit[ihit].r = SQ6*1.5; //0.5*(s6HCAL + s12HCAL);
	       }
	     else 
	       {
		 chit[ihit].r = SQ12;
		 chit[ihit].c = s12HCAL;
	       }
	   }
	 /*----------------------------------------------------------------
	  coarse layers*/
	 else 
	   {
	     if ( (fabs(chit[ihit].x) < b6) && (fabs(chit[ihit].y) < b6) ) 
	       {
		 chit[ihit].c = s6HCAL;
		 if ( (fabs(chit[ihit].x) < b6l) || (fabs(chit[ihit].y) < b6l) ) 
		   chit[ihit].r = SQ6;
		     else chit[ihit].r = SQ6*1.5; //0.5*(s6HCAL + s12HCAL);
	       }
	     else 
	       {
		 chit[ihit].r = SQ12;
		 chit[ihit].c = s12HCAL;
	       }
	   }
	 clayer[layer+ELAY].vl.push_back(&chit[ihit]);
	 clayer[layer+ELAY].nh++;
	 clayer[layer+ELAY].emip += chit[ihit].e;
	 
	 energySumEcalHcal +=  chit[ihit].e*mip2gevHCAL1;

	 streamlog_out(DEBUG)<<" ihit: "<<ihit<<" layer: "<<layer<<" energy: "<<hit->getEnergy()<<" r: "<<chit[ihit].r
		     <<" size: "<<chit[ihit].c
		     <<" x="<<chit[ihit].x
		     <<" y="<<chit[ihit].y
		     <<" z="<<chit[ihit].z
		     <<endl;

	 
       } /* for hits HCAL*/
     
     /*TCMT*/
     for ( unsigned i = 0; i < nhitT; i++ ) 
       {
	 CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col_T->getElementAt( i ) );
	 CellIndex cellIndex(hit->getCellID0());
	 unsigned layer = unsigned(cellIndex.getLayerIndex() - 1);
	 float e =  hit->getEnergy();
	 if ( e >= _MIPLimit ) 
	   {
	     if ( layer < TCAL1 ) 
	       energySumTcmt += e*mip2gevTCAL1;
	     else 
	       energySumTcmt += e*mip2gevTCAL2;
	 }
    }
     
     /*------ Calculate  moving average of mips in layer  -------------- */
     mov_av[0] = clayer[0].emip;
     for ( unsigned i = 1; i < _movingAverageWindow; i++ ) 
       {
	 mov_av[i] = mov_av[i-1] + clayer[i].emip;
       }

     for ( unsigned i = _movingAverageWindow; i < EHLAY; i++ ) 
       {
	 mov_av[i] = mov_av[i-1] - clayer[i - _movingAverageWindow].emip + clayer[i].emip;
       }

     for ( unsigned i = 1; i < _movingAverageWindow; i++ ) 
       {
	 mov_av[i] /= i + 1;
       }

     for ( unsigned i = _movingAverageWindow; i < EHLAY; i++ ) 
       {
	 mov_av[i] /= _movingAverageWindow;
       }
     
     /*---- Identify muons and exclude trash and multiparticle events-------------------*/
     
     /* for muon runs*/
     if ( _fixStartingLayer ) 
       {  
	 muon = true;
	 n_muon_like++;
       }
     else  
       {
	 muon = isMuon(energySumEcalHcal, energySumTcmt);
	 if ( muon ) 
	   n_muon_like++;
	 else 
	   sh_st = getStartingLayer(clayer);
       }
     
     streamlog_out(DEBUG)<<" muon(HCAL, TCMT)="<<muon<<endl;
     streamlog_out(DEBUG)<<" Number of events with muon candidates: "<<n_muon_like<<endl;
     streamlog_out(DEBUG)<<" fixStartingLayer="<<_fixStartingLayer<<endl;
     streamlog_out(DEBUG)<<" Shower starts in layer: "<<sh_st<<endl;

     if ( _fRootOut ) StartLayer->Fill(sh_st);
     
     if  ( (int(sh_st) - int(calor_front)) <= _absTrackLengthLimit ) 
       {
	 n_early_shower++;
	 streamlog_out(DEBUG)<<" Number of events showering too early (i.e. track smaller than given absTrackLengthLimit): "<<n_early_shower<<endl;
       }
     else if ( !_skipTrackFinder ) 
       {
	 track_found = FindTrack(sh_st, clayer);
	 streamlog_out(DEBUG)<<" track_found="<<track_found<<endl;
       }

     
     if ( muon ) 
       {
       if ( shortID ) 
	 {
	   n_short_muon++;
	   track_found = false;
	 }
       if ( paraID ) 
	 {
	   n_para_muon++;
	 }
      if ( track_found ) 
	n_muon_track++;
       } 
     else 
       {
	 if ( shortID ) 
	   {
	     n_short++;
	     track_found = false;
	   }
	 if ( paraID ) 
	   {
	     n_para++;
	     track_found = false;
	   }
	 if ( track_found ) n_hadr_track++;
       }
//-------------------------------------------------------------------------------------
    finalTrack.clear();
    float crit = 0.0;
    float sigd = 0.0;
    float avd  = 0.0;
    int tsize  = intrack.size();

    if( track_found ) 
      {
	float wt = 0.0;
	float xtr = 0.0;
	float ytr = 0.0;
	float wi[tsize];
	
	for ( int ip = 0; ip < tsize; ip++ ) 
	  {
	    wi[ip] = 4./(intrack[ip]->c)/(intrack[ip]->c);
	    wt += wi[ip];
	    xtr += (intrack[ip]->x)*wi[ip];
	    ytr += (intrack[ip]->y)*wi[ip];
	  }

	if ( wt > 0. ) 
	  {
	    xtr /= wt;
	    ytr /= wt;
	  }

	// cleaning of outliers
	float wd = 0.0;
	float di[tsize];
	for ( int ip = 0; ip < tsize; ip++ ) 
	  {
	    float wdi = wi[ip]*wt/(wi[ip] + wt);
	    wd += wdi;
	    di[ip] = sqrt( ((intrack[ip]->x) - xtr)*((intrack[ip]->x) - xtr) 
			   + ((intrack[ip]->y) - ytr)*((intrack[ip]->y) - ytr) );
	    avd += di[ip]*wdi;
	  }

	if ( wd > 0. ) avd /= wd;
	for ( int ip = 0; ip < tsize; ip++ ) 
	  {
	    sigd += (di[ip] - avd)*(di[ip] - avd);
	  }

	if ( (tsize-1) > 1 ) sigd = sqrt(sigd/(tsize-1));
	crit = 3.*sigd;

	for ( int ip = 0; ip < tsize; ip++ ) 
	  {
	    float sdd = fabs(di[ip] - avd);
	    if ( sdd > crit ) continue; // outlier that exceeds 3 sigma
	    finalTrack.push_back(intrack[ip]);
	  }
	float wfinal = 0.0;
	for (std::vector<CHit*>::iterator p = finalTrack.begin(); p != finalTrack.end(); p++) 
	  {
	    float wfi = 4./((*p)->c)/((*p)->c);
	    wfinal += wfi;
	    xtrack += ((*p)->x)*wfi;
	    ytrack += ((*p)->y)*wfi;
	  }
	if ( wfinal > 0. ) 
	  {
	    xtrack /= wfinal;
	    ytrack /= wfinal;
	  }
      }
    else 
      {
	xtrack = 99999.;
	ytrack = 99999.;
      }
//--------------------------------------------------------------------
//------  Drawing ------------- --------------------------------------
//--------------------------------------------------------------------
    if ( _fDraw ) {
    //if ( _fDraw && !track_found ) {
    //if ( _fDraw && ( trash || mult || paraID || shortID ) ) {
      ced_new_event();
      cout << "*****  Event " << nevt << " *****" 
	   << "    Muon: " << muon << "   Parallel: " << paraID << "   Short: " << shortID << endl
	   << "   Start in " << sh_st << " layer" << endl;

      // ECAL hits
     for ( unsigned i = 0; i < nhitE; i++ ) {
       if ( chit[i].e >= _MIPLimit ) {
	 ced_hit( chit[i].x, chit[i].y, chit[i].z, L1|3, _pointSize-1, WHITE);
       }
     }
      // HCAL hits
     for ( unsigned i = nhitE; i < nhitE+nhitH; i++ ) {
       if ( chit[i].e >= _MIPLimit ) {
	 ced_hit( chit[i].x, chit[i].y, chit[i].z, L2|3, _pointSize, GREEN);
       }
     }
      // starting layer
      for(std::vector<CHit*>::iterator p = clayer[sh_st].vl.begin();
	  p != clayer[sh_st].vl.end(); p++) {
	ced_hit((*p)->x,(*p)->y,(*p)->z, L3|3, _pointSize+2, RED);
      }

      // track
      if ( track_found ) {
	for (std::vector<CHit*>::iterator p = intrack.begin();
	     p != intrack.end(); p++) {
	  ced_hit( (*p)->x, (*p)->y, (*p)->z, L4|0, _pointSize, YELLOW);
	}
	int inp = finalTrack.size();
	for (std::vector<CHit*>::iterator p = finalTrack.begin();
	     p != finalTrack.end(); p++) {
	  ced_hit( (*p)->x, (*p)->y, (*p)->z, L5|0, _pointSize, BLUE);
	  //cout << "  x = " << (*p)->x << "  y = " << (*p)->y << "  z = " << (*p)->z << endl;
	}
	float zl1 = finalTrack[0]->z;
	float zl2 = finalTrack[inp-1]->z;
	if (_beamAngle) {
	  float xl1 = finalTrack[0]->x;
	  float xl2 = finalTrack[inp-1]->x;
	  float yl1 = finalTrack[0]->y;
	  float yl2 = finalTrack[inp-1]->y;

	  ced_line(xl1, yl1, zl1, xl2, yl2, zl2, 1, _lineSize, YELLOW);
	}
	else ced_line(xtrack, ytrack, zl1, xtrack, ytrack, zl2, 1, _lineSize, YELLOW);

	cout << " Track length = " << inp << " points" << " (" << tsize - inp << " cleaned)" << endl;
      }
      else {
	 cout << "Track not found (" << finalTrack.size() << " points)"<< endl;
      }

	ced_send_event();
	getchar();
    }
//--------------------------------------------------------------------
//------  Collection preparing --------------------------------------
//-------------------------------------------------------------------
//------  Collection name = _startColName  = "ShowerStartingLayer"
//       Collection member:
//       (int) calorimeter type: 0(ECAL) or 1(HCAL)
//       (int) start_layer = number of shower starting layer
//       (float) weighted average x of track
//       (float) weighted average y of track
//-----------------------------------------------------------------
    EVENT::LCCollection* ShowerStart  =
	    new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT);
	IMPL::LCGenericObjectImpl* ShowerStartLayerData = 
	         new IMPL::LCGenericObjectImpl;
	int cal_type = int(sh_st/ELAY);
	int st_lay = int(sh_st) - cal_type*int(ELAY);

	streamlog_out(DEBUG)<<"\n ptfShowerStart: "<<st_lay<<" sh_st="<<sh_st<<" ELAY="<<ELAY<<endl;

	ShowerStartLayerData->setIntVal(0, cal_type );
	ShowerStartLayerData->setIntVal(1, st_lay );
	ShowerStartLayerData->setFloatVal(2, xtrack );
	ShowerStartLayerData->setFloatVal(3, ytrack );
	ShowerStart->addElement( ShowerStartLayerData );
	evt->addCollection( ShowerStart, _startColName ); 
//-----------------------------------------------------------------
//------  Collection name = _prtrackColName = "PrimaryTrackHits" 
//     Collection members:
//     (int) type = layer number from CellID0 continuous from 0 to 67
//     (float*) position  = x, y, z world coordinates from CalorimeterHit
//     (float) energy = E deposited in MIPs (from CalorimeterHit)
//-----------------------------------------------------------------
    if ( track_found ) {
	EVENT::LCCollection* PrimaryTrackCol = new IMPL::LCCollectionVec( LCIO::CALORIMETERHIT );

	LCFlagImpl hitFlag( PrimaryTrackCol->getFlag() );
	hitFlag.setBit( LCIO::CHBIT_LONG );
	if (_doWriteSubset == true) 
	  {
	    hitFlag.setBit(LCCollection::BITSubset);
	  }
	PrimaryTrackCol->setFlag(  hitFlag.getFlag() );

	float pos[3];
	for (std::vector<CHit*>::iterator p = finalTrack.begin(); p != finalTrack.end(); p++) 
	  {
	    const int cellID0 = (*p)->cellID;

	    /* Add element to the collection*/
	    if (_doWriteSubset == true)
	      {
		CalorimeterHit *originalHit = _originalHitsContainer->getByCellID(cellID0);
		PrimaryTrackCol->addElement( originalHit );
	      }
	    else
	      {
		IMPL::CalorimeterHitImpl* PrimaryTrackHitsData = new IMPL::CalorimeterHitImpl;
		pos[0] = (*p)->x;
		pos[1] = (*p)->y;
		pos[2] = (*p)->z;
		
		/* Add data to the collection element*/
		int type = int((*p)->l);
		PrimaryTrackHitsData->setType(type);
		PrimaryTrackHitsData->setPosition(pos);
		PrimaryTrackHitsData->setEnergy((*p)->e);
		PrimaryTrackHitsData->setCellID0(cellID0);
		PrimaryTrackCol->addElement( PrimaryTrackHitsData );	
	      }   
	  }
	
	/* Add collection to the event*/
	if (PrimaryTrackCol->getNumberOfElements() > 0) 
	  {
	    LCParameters &param = PrimaryTrackCol->parameters();
	    std::string outputEncodingString = "";
	    if (col_H != NULL)
	      outputEncodingString = col_H->getParameters().getStringVal(LCIO::CellIDEncoding);
	    param.setValue(LCIO::CellIDEncoding, outputEncodingString);
	    
	    evt->addCollection( PrimaryTrackCol, _prtrackColName );
	  }
	else
	  {
	    delete PrimaryTrackCol;
	  }
    }
//---------------------------------------------------------------------
//         Check collection 
//     Output to debug root-file
//    and an example of getting data from this collection
//---------------------------------------------------------------------
    if (_fRootOut) {
	int nE = 0;
	int nH = 0;
	const std::vector<std::string>* sVec = evt->getCollectionNames();

	for( std::vector<std::string>::const_iterator name = sVec->begin();
	     name != sVec->end() ; name++){
	    const std::string & tname = evt->getCollection( *name )->getTypeName();
	    if(tname == LCIO::LCGENERICOBJECT ) {
	      if(!std::strcmp((*name).data(), _startColName.c_str())) {
		col_S = evt->getCollection( *name );
		if ( col_S->getNumberOfElements() > 0 ) {
		  IMPL::LCGenericObjectImpl* st_col_check = dynamic_cast<LCGenericObjectImpl*>( col_S->getElementAt( 0 ));
		  if ( st_col_check->getNInt() > 1 ) {
		    int cal_type_check = st_col_check->getIntVal( 0 );
		    int st_check = st_col_check->getIntVal( 1 );
		    if ( cal_type_check == 1 ) StartLayer_HCAL1->Fill(st_check, 1.0);
		    if ( cal_type_check == 2 ) StartLayer_HCAL2->Fill(st_check, 1.0);
		  }
		}
	      }
	    }
	    if(tname == LCIO::CALORIMETERHIT ) {
		if(!std::strcmp((*name).data(),"PrimaryTrackHits")) {
		    col = evt->getCollection( *name );
		    nhit =  col->getNumberOfElements(); // number of track hits
		}
	    }
	}
	for(unsigned i = 0; i < nhit; i++) {
	    CalorimeterHit* hit = dynamic_cast<CalorimeterHit*>( col->getElementAt( i ) );
	    unsigned layer = unsigned(hit->getType());
	    if ( layer < ELAY ) {
		x_av_E += hit->getPosition()[0];
		y_av_E += hit->getPosition()[1];
		TrackHitMIP->Fill(hit->getEnergy());
		nE++;
	    }
	    else {
		x_av_H += hit->getPosition()[0];
		y_av_H += hit->getPosition()[1];
		TrackHitMIP->Fill(hit->getEnergy());
		nH++;
	    }
	}
	if (nE) { 
	    x_av_E /= nE; 
	    y_av_E /= nE;
	    TrackXY_E->Fill(x_av_E,y_av_E);
	}
	if (nH) { 
	    x_av_H /= nH; 
	    y_av_H /= nH;
	    TrackXY_H->Fill(x_av_H,y_av_H);
	}
    }
  
} // processEvent


/*=============================end======================================*/
void PrimaryTrackFinder::end()
{
  float percent_hadr = 0.0;
  int n_rem_hadr =  nevt - n_early_shower - n_muon_like - n_short;
  if ( n_rem_hadr ) percent_hadr = 100.0*float(n_hadr_track)/float(n_rem_hadr);
  float percent_muon = 0.0;
  int n_rem_muon = n_muon_like - n_short_muon; // - n_mult_muon;
  if ( n_rem_muon ) percent_muon = 100.0*float(n_muon_track)/float(n_rem_muon);

  cout << "Total number of events = " << nevt << endl
       << "    including:" << endl
       << n_early_shower << " events with shower start before " << _absTrackLengthLimit+1 
       << " layer from calorimeter front" << endl
       << n_short << " too short track fragments (< than " << _absTrackLengthLimit+1 << " points)" << endl
       << n_muon_like << " muon-like events (including " 
       << n_para_muon << " with parallel candidates and " << n_short_muon << " with possible short fragments)"  << endl;

  cout << "For " << n_rem_muon << " muon-like events " << n_muon_track << " tracks found (" 
       <<  percent_muon << "%)" << endl;
  cout << "For " << n_rem_hadr << " hadron-like events " << n_hadr_track << " tracks found (" 
       << percent_hadr << "%)" << endl;

  if (_fRootOut) {
    rootfile->Write();
    rootfile->Close();
  }  



} 
 
/*==============================================================================
  Identification of muon-like events
  ------------------------------------------------------------------------------*/
bool PrimaryTrackFinder::isMuon(float e_eh, float e_t) 
{
  /*  Numbers are extracted from 2D histogram
      this is the corner triangle at histogram: E(ecal+hcal) vs E(tcmt) 
      all muons are at this corner 
      different triangle vertexes are set for E_beam < 20 GeV and E_beam > 20 GeV */
  
  if ( ( (nhitE_cut > 16) || (nhitH_cut > 18) ) && 
       ( (e_t > (amu12*e_eh + bmu12)) && (e_t < (amu23*e_eh + bmu23)) && (e_t < (amu13*e_eh + bmu13)) ) )
    return true;
  else 
    return false;
}

/*========================================================================
  Function distXY calculates distance between two hits in XY-plane
  ------------------------------------------------------------------------*/
float PrimaryTrackFinder::distXY(CHit* h1, CHit* h2) 
{
  return (sqrt((h1->x - h2->x)*(h1->x - h2->x) + (h1->y - h2->y)*(h1->y - h2->y)));
}

/*========================================================================
  Function getStartingLayer finds shower starting layer 
  ------------------------------------------------------------------------*/
unsigned PrimaryTrackFinder::getStartingLayer(CLayer* lr) 
{
  unsigned start = EHLAY-1;
  bool shower = false;
  unsigned i = 0;
  while (!lr[i].nh && i < EHLAY-1) 
    { 
      i++; 
    }
  
  while ( (i < EHLAY-1) && !shower ) 
    {
      if ( (mov_av[i] + mov_av[i+1]) > _minMIPfromBeamEnergy
	   && (lr[i].nh + lr[i+1].nh) > _minHitsFromBeamEnergy )
	shower = true;
      i++;
    }

  if ( shower ) 
    {
      if ( ( (i < EHLAY - 3) &&
	     ( (lr[i+2].emip < lr[i].emip) && (lr[i+3].emip < lr[i].emip) 
	       && ( (lr[i+3].emip + lr[i+2].emip) < (lr[i+1].emip + lr[i].emip) ) ) )
	   || ( i == ELAY ) )
	start = i;
      else start = i - 1;
    }
  return start;
}

/*===========================================================================
  Function FindTrack finds track in ECAL & HCAL
  using "nearest neighbour" criteria.
  --------------------------------------------------------------------------*/
bool PrimaryTrackFinder::FindTrack(unsigned start, CLayer* lr) 
{
  int len2sh = int(start) - int(calor_front) - _absTrackLengthLimit + 1;

  streamlog_out(DEBUG)<<" \n FindTrack len2sh="<<len2sh<<" start="<<start<<" calor_front="<<calor_front
	      <<" absTrackLengthLimit="<<_absTrackLengthLimit
	      <<endl;
  
  bool isTrack = false;
  int ntrcand = 0;
  float xtr[20];
  float ytr[20];
  bool shorttemp = false;
  bool longtemp = false;

  /* ------- Finding Primary track ----------------------------*/
  CHit* pcur = NULL;
  CHit* pc1 = NULL;
  float rtrack;
  bool isGap;
  std::vector<CHit*> temp;

  int i = 0; /* first layer*/

  /*Loop layer by layer to search track beginning*/
  while ( i < len2sh && !isTrack)                
    {
      /*skip empty layers*/
      while (!lr[i].nh && i < len2sh-1) 
	{
	  i++; 
	  streamlog_out(DEBUG)<<" layer "<<i<<" nhits: "<<lr[i].nh<<endl;
	}   

      streamlog_out(DEBUG)<<" \n Look for track in layer "<<i<<" with " <<lr[i].nh<<" hits"<<endl;

      if ( (i == len2sh-1) && !lr[i].nh ) 
	{
	  streamlog_out(DEBUG)<<"    -> empty layer, will break"<<endl;
	  break;
	}
      
      float rtmin = SQ12;
      int jgap = 0;

      /*loop over  hits in layer of track beginning*/
      for(std::vector<CHit*>::iterator p0 = lr[i].vl.begin(); p0 != lr[i].vl.end(); p0++) 
	{             
	  temp.clear();
	  pcur = *p0;
	  isGap = false;
	  
	  unsigned j = unsigned(i);
	  
	  /*loop over layers and fill temporary candidate vector with hits*/
	  while ( (!isGap) && (j < start) )
	    {
	      jgap = 0;
	      pc1 = NULL;
	      
	      /* push current hit in candidate track*/
	      temp.push_back(pcur);     
	      /* set max allowable radial distance to the next hit*/
	      float r01max = pcur->r;          

	      streamlog_out(DEBUG)<<"   \nCurrent hit: x="<<pcur->x<<" y="<<pcur->y<<" z="<<pcur->z<<" r="<<pcur->r<<" layer="<<pcur->l<<endl;
	      
	      /* loop over next to find nearest*/
	      while ((j < start) && !pc1 && (jgap <= _NGAP)) 
		{ 
		  if ((pcur->l < ELAY) && (j > ELAY-1)) r01max = SQ3;
		  
		  streamlog_out(DEBUG)<<"   Max allowable radial distance to the next hit: "<<r01max<<endl;

		  /* loop over the hits in the next layer*/
		  for(std::vector<CHit*>::iterator p1 = lr[j+1].vl.begin(); p1 != lr[j+1].vl.end(); p1++) 
		    {                            
		      float r01 = 0.0;
		      
		      /* loop over previously filled candidate hits*/
		      for(std::vector<CHit*>::iterator pt = temp.begin(); pt != temp.end(); pt++)  
			{                           
			  r01 += fabs(distXY(*pt, *p1) - ((*p1)->z - (*pt)->z)*_tangentBeamAngle);
			}
		      
		      r01 /= temp.size();
		      
		      if (r01 < r01max) 
			{
			  r01max = r01;
			  
			  /* select the nearest to all previous*/
			  pc1 = *p1; 
			}
		    }
		  j++;
		  jgap++;
		}  /* end of search for next nearest*/
	      
	      if (pc1 && ( j < start )) pcur = pc1;
	      if ( jgap > _NGAP ) isGap = true;

	      streamlog_out(DEBUG)<<"   jgap="<<jgap<<" NGAP="<<_NGAP<<" isGap="<<isGap<<endl;
	    }/* end of loop over layers up to FIP*/
	  
	  /* temp track size for slopes comparison*/
	  int tsize = temp.size();     
  
	  streamlog_out(DEBUG)<<"   tsize="<<tsize<<" absTrackLengthLimit="<<_absTrackLengthLimit
		      <<" relTrackLengthLimit="<<_relTrackLengthLimit
		      <<" start="<<start<<" calor_front="<<calor_front
		      <<endl;
	  streamlog_out(DEBUG)<<"  (start-calor_front)*relTrackLengthLimit="
		      <<((float(start) - float(calor_front))*_relTrackLengthLimit )<<endl;
	  streamlog_out(DEBUG)<<"   ntrcand: "<<ntrcand<<endl;

	  
	  if ( (tsize >= _absTrackLengthLimit) &&
	       ( float(tsize) >= (float(start) - float(calor_front))*_relTrackLengthLimit ) ) 
	    {
	      rtrack = fabs(distXY(*p0, pcur) - (pcur->z - (*p0)->z)*_tangentBeamAngle);
	      streamlog_out(DEBUG)<<"   rtrack: "<<rtrack<<" currentR: "<<pcur->r<<endl;
	       
	      if ( rtrack < pcur->r && ntrcand < 20 ) 
		{
		  longtemp = true;
		  xtr[ntrcand] = 0.;
		  ytr[ntrcand] = 0.;
		  
		  /* loop over previously filled candidate hits*/
		  for(std::vector<CHit*>::iterator pt = temp.begin(); pt != temp.end(); pt++) 
		    {      
		      xtr[ntrcand] += (*pt)->x;
		      ytr[ntrcand] += (*pt)->y;
		    }
		  
		  xtr[ntrcand] /= tsize;
		  ytr[ntrcand] /= tsize;
		  
		  if (rtrack < rtmin) 
		    {
		      isTrack = true;
		      rtmin = rtrack;
		      intrack.clear();
		      
		      for (unsigned jj = 0; jj < temp.size(); jj++) 
			{
			  /* put currently best in the final vector*/
			  intrack.push_back(temp[jj]); 
			}
		    }/* end if closer to ideal slope*/
		  
		  ntrcand++;
		}/* end if candidate is inside slope limit*/
	    }/*end if track is long enough*/
	  else
	    {
	      streamlog_out(DEBUG)<<"   Track is not long enough"<<endl;
	      shorttemp = true;
	    }
	}/* end of loop over hits in first track layer*/
      i++;
    }/* end of loop layer by layer*/

  
  if ( shorttemp && !longtemp ) 
    shortID = true;
  else 
    shortID = false;
  
  if ( ntrcand > 19 ) 
    { 
      isTrack = false; paraID = true; 
    }
  else 
    {
      float t2tmax = 0.;
      
      for ( int k1 = 0; k1 < ntrcand-1; k1++ ) 
	{
	  for ( int k2 = k1+1; k2 < ntrcand; k2++ ) 
	    {
	      float dtrack = sqrt((xtr[k1] - xtr[k2])*(xtr[k1] - xtr[k2]) +
				  (ytr[k1] - ytr[k2])*(ytr[k1] - ytr[k2]) );
	      if ( dtrack > t2tmax ) t2tmax = dtrack;
	    }
	}
      
      /* mark event with parallel tracks*/
      if ( (t2tmax > ParaDistmax)
	   || ( ( start >= ELAY ) && ( t2tmax > ParaDistmin ) ) ) 
	{  
	  isTrack = false;
	  paraID = true;
	}
    }

  streamlog_out(DEBUG)<<" ntrcand: "<<ntrcand<<" isTrack: "<<isTrack<<endl;
  
  return isTrack;
}
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void PrimaryTrackFinder::fillOriginalHitsContainer(LCCollection *inputCol)
  {
    streamlog_out(DEBUG0)<<"\nStart to fillOriginalHitsContainer "<<std::endl;
 
    /*create a new map container. Note the 'false' flag, which tells the container
      that he is not responsible for the deletion of the contained elements.
      We use this because at the end of the event we have all the elements from
      the container into an LCCollection, and Marlin deletes them at the end of the event.*/
    _originalHitsContainer = new MappedContainer<CalorimeterHitImpl>(_mapper, false);
    std::string mokkaEncodingString = inputCol->getParameters().getStringVal(LCIO::CellIDEncoding);
    _originalHitsContainer->getDecoder()->setCellIDEncoding(mokkaEncodingString);
   
   /*Copy the input collection into the container, which we'll modify afterwards*/
     for (int iHit = 0; iHit < inputCol->getNumberOfElements(); ++iHit)
       {
	 CalorimeterHitImpl *hit = dynamic_cast<CalorimeterHitImpl*>(inputCol->getElementAt(iHit));
	 int trueCellID = hit->getCellID0();
	 
	 _originalHitsContainer->fillByCellID(trueCellID, hit);	 
       }
  
     streamlog_out(DEBUG0)<<"fillOriginalHitsContainer: Container has "
		  <<_originalHitsContainer->getAllElements().size()
		  <<" elements"<<endl;
     
  }
  
