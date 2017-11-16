#include <SquareFinder.hh>
#include <vector>
#include <EVENT/LCIO.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <EVENT/CalorimeterHit.h>
#include <IMPL/ClusterImpl.h>
#include <IMPL/LCCollectionVec.h>
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif
#include <cassert>
#include <collection_names.hh>
#include <CellIndex.hh>
#include <ErrorBits.hh>

#ifdef HAVE_CALICEGUI
#  include <GuiThread.hh>
#endif 

#ifdef WITH_CONTROL_HISTOGRAMS
#include <histmgr/HistMgr.hh>
#include <CellIndex.hh>
#include <histmgr/FloatHistogram1D.hh>
#include <ModuleLocation.hh>
#endif

#define CAN_CAST_CALORIMETERHIT_GETPOSITION

namespace CALICE {

  inline Double_t sqr(const Double_t a) {return a*a;}

  // create instance to make this Processor known to Marlin
  SquareFinder a_SquareFinder_instance;
  
  /** Select Mips .
   * Mip tracks are search within the calorimeter hits. 
   * Developped for cosmics.
   */
  SquareFinder::SquareFinder() 
    : Processor("SquareFinder")
#ifdef WITH_CONTROL_HISTOGRAMS
    ,_histGroupKey(type()),
      _padIndexHistKey("PadIndex"),
      _waferIndexHistKey("WaferIndex"),
      _nSquareHistKey("NSquares"),
      _borderSignalHistKey("BorderSignal"),
      _borderTotalSignalHistKey("BorderTotalSignal")
#endif
  {
    _description = "Select mips and histogram the signal" ;

    _nHitsMin=static_cast<unsigned int>((12+8)*.6);
    registerProcessorParameter( "NHitsMin" , 
			       "The minimum number of hits per track for accepted tracks." ,
			       _nHitsMin ,
			       _nHitsMin);

    _nInsidedHitsMax=static_cast<unsigned int>((4*4)*.5);
    registerProcessorParameter( "NInsideHitsMax" , 
				"The maximum number of hits inside the wafer" ,
			       _nInsidedHitsMax ,
			       _nInsidedHitsMax);

    registerProcessorParameter( "HitCollectionName" , 
    				"Name of the Calorimeter hit collection"  ,
				_colName ,
    				std::string(COL_HITS) ) ;


    registerProcessorParameter( "ClusterCollectionName" , 
    				"Name of the (Square-) Cluster collection"  ,
				_clusterColName ,
    				std::string("SquareClusters") ) ;

    registerProcessorParameter( "SquareEventFlagName" , 
    				"Name of the event parameter which flags square events."  ,
				_squareFlagName ,
    				std::string("SquareEvent") ) ;




#ifdef WITH_CONTROL_HISTOGRAMS
    registerProcessorParameter( "HistogramGroupName" , 
    				"The name of the histogram group under which the control histograms will be registered."  ,
				_histGroupKey.nameStorage() ,
    				_histGroupKey.name() );

    _signalHistPar.clear();
    _signalHistPar.push_back(10000);
    _signalHistPar.push_back(0);
    _signalHistPar.push_back(500);
    _signalHistPar.push_back(5000);
    _signalHistPar.push_back(0);
    _signalHistPar.push_back(1000);
    
    registerOptionalParameter( "SignalBinning" , 
			       "The binning for the control histograms: border signal (3 values), optionally total signal (+3 values)." ,
			       _signalHistPar ,
			       _signalHistPar ,
			       _signalHistPar.size() ) ;

#endif

  }

  void SquareFinder::init() {

    printParameters();

    if (_clusterColName.size()>=2 && _clusterColName[0]=='\"' && _clusterColName[_clusterColName.size()-1]=='\"') {
      _clusterColName.erase(_clusterColName.size()-1,1);
      _clusterColName.erase(0,1);
    }
    if (_clusterColName.size()>=2 && _clusterColName[0]=='\\') {
      _clusterColName.erase(0,1);
    }
    if (_clusterColName.size()>=2 && _clusterColName[_clusterColName.size()-2]=='\\') {
      _clusterColName.erase(_clusterColName.size()-2,1);
      _clusterColName.erase(0,1);
    }

    if (_clusterColName.empty()) {
      std::cout << "INFO: SquareFinder::init> No cluster collection name given. Will only mark events with square clusters but will not create square cluster collection."  << std::endl;
    }

    _nSquareEvents=0;
    _nEvents=0;

#ifdef WITH_CONTROL_HISTOGRAMS
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();

    {
      std::vector<std::string > names;
      names.push_back(std::string("PadsOtherEvents"));
      names.push_back(std::string("PadsSquareEvents"));
      names.push_back(std::string("MaxPadSquareEvents"));
      histogramList->create2DHistograms(_histGroupKey,_padIndexHistKey,names,3,HistPar(7,-.5,7-.5),HistPar(7,-.5,7-.5));
    }

    histogramList->create2DHistograms(_histGroupKey,_waferIndexHistKey,31,HistPar(7,-.5,7-.5),HistPar(7,-.5,7-.5));
    histogramList->createHistograms(_histGroupKey,_nSquareHistKey,1,HistPar(31,-.5,31-.5));

    assert (_signalHistPar.size() == 3 ||  _signalHistPar.size() == 6);

    {
      std::vector<std::string > names;
      names.push_back(std::string("BorderPadSignal"));
      names.push_back(std::string("MaxPadSignal"));
      histogramList->createHistograms(_histGroupKey,_borderSignalHistKey,names,2,
				      HistPar(static_cast<UInt_t>(_signalHistPar[0]), _signalHistPar[1], _signalHistPar[2]),
				      true);
    }
    
    UInt_t total_i=(_signalHistPar.size()==6 ? 3 : 0);
    histogramList->createHistograms(_histGroupKey,_borderTotalSignalHistKey,2,
				    HistPar(static_cast<UInt_t>(_signalHistPar[total_i+0]), _signalHistPar[total_i+1], _signalHistPar[total_i+2]),
				    true);

#endif

  }

  class WaferInfo_t {
  public:
    WaferInfo_t() : _nBorderHits(0), _nInnerHits(0), _maxRow(0),_maxCol(0),_maxSignal(0.),_totalSignal(0.) {}

    void addBorderHit() {_nBorderHits++;}
    void addInnerHit() {_nInnerHits++;}

    void addHitSignal(Float_t energy, unsigned char col_i, unsigned char row_i) {
      if (energy > _maxSignal) {
	_maxSignal=energy;
	_maxCol=col_i;
	_maxRow=row_i;
      }
      _totalSignal+=energy;
    }
    
    unsigned char nBorderHits() const {return _nBorderHits;}
    unsigned char nInnerHits() const {return _nInnerHits;}

    unsigned char maxRow() const {return _maxRow;}
    unsigned char maxCol() const {return _maxCol;}

    Float_t maxSignal() const {return _maxSignal;};
    Float_t totalSignal() const {return _totalSignal;};

    

    unsigned char _nBorderHits;
    unsigned char _nInnerHits;
    unsigned char _maxRow;
    unsigned char _maxCol;
    Float_t _maxSignal;
    Float_t _totalSignal;
  };


  void SquareFinder::processEvent( LCEvent * evtP ) {
    if (evtP) {
      try {
	LCCollection* col_hits = evtP->getCollection( _colName ) ;
	//	_isolatedHits.clear();


	WaferInfo_t wafer_info[31][4][4];
	
	bool necessary_condition=false;
	// --- first step group nearby hits
	for(UInt_t element_i=0; element_i < (UInt_t) col_hits->getNumberOfElements(); element_i++) {
	  EVENT::CalorimeterHit *a_hit=dynamic_cast<EVENT::CalorimeterHit *>(col_hits->getElementAt(element_i));

	  CALICE::CellIndex index(a_hit->getCellID0());
#       ifdef BOUNDARY_CHECK
	  assert ( index.getLayerIndex() <= 30 );
	  assert ( index.getWaferColumn() <= 3 );
	  assert ( index.getWaferRow() <= 3 );
#       endif
	  if (index.getPadColumn()==1 || index.getPadColumn()==6 || index.getPadRow()==1 || index.getPadRow()==6) {
	    wafer_info[index.getLayerIndex()][index.getWaferColumn()][index.getWaferRow()].addBorderHit();
	    if (wafer_info[index.getLayerIndex()][index.getWaferColumn()][index.getWaferRow()].nBorderHits()>=static_cast<UInt_t>(_nHitsMin)) {
	      necessary_condition=true;
	    }
	  }
	  else {
	    wafer_info[index.getLayerIndex()][index.getWaferColumn()][index.getWaferRow()].addInnerHit();
	  }
	}

	IMPL::ClusterImpl *cluster[31][4][4];
	IMPL::ClusterImpl *other_hits = NULL;
	EVENT::LCCollection* col_cluster = NULL;

	if (!_clusterColName.empty()) {
	  other_hits=new IMPL::ClusterImpl;
	  col_cluster=new IMPL::LCCollectionVec( EVENT::LCIO::CLUSTER );
	  col_cluster->addElement(other_hits);
	}

	bool mark=false;

	UInt_t n_squares=0;
#     ifdef WITH_CONTROL_HISTOGRAMS
	histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
#     endif

	if (necessary_condition) {
#       ifdef WITH_CONTROL_HISTOGRAMS
	  histmgr::Histogram2DCollection_t & wafer_index_hist=histogramList->getHistogram2DCollection(_histGroupKey,_waferIndexHistKey);
#       endif

	  for (UInt_t layer_i=0; layer_i<=30; layer_i++) {
	    for (UInt_t col_i=0; col_i<=3; col_i++) {
	      for (UInt_t row_i=0; row_i<=3; row_i++) {
		
		if (   wafer_info[layer_i][col_i][row_i].nBorderHits()>=static_cast<UInt_t>(_nHitsMin) 
		    && wafer_info[layer_i][col_i][row_i].nInnerHits() <=static_cast<UInt_t>(_nInsidedHitsMax)) {

		  //		  std::cout << "SquareFinder::processEvent> Square event candidate : run= " << evtP->getRunNumber() << " event= " 
		  //			    << evtP->getEventNumber()
		  //			    << std::endl;
#               ifdef WITH_CONTROL_HISTOGRAMS
		  wafer_index_hist.histogram(layer_i)->fill(col_i,row_i);
#               endif
		  n_squares++;
		  mark=true;
		  if (!_clusterColName.empty()) {
		    cluster[layer_i][col_i][row_i]=new IMPL::ClusterImpl;
		    col_cluster->addElement(cluster[layer_i][col_i][row_i]);
		  }
		  
		}
#             ifdef BOUNDARY_CHECK
		else {
		  cluster[layer_i][col_i][row_i]=0;
		}
#             endif
	      }
	    }
	  }
	}
#     ifdef WITH_CONTROL_HISTOGRAMS
	histmgr::HistogramCollection_t & n_square_hist=histogramList->getHistogramCollection(_histGroupKey,_nSquareHistKey);
	n_square_hist.histogram(0)->fill(n_squares);
#     endif


#     ifdef WITH_CONTROL_HISTOGRAMS
	histmgr::Histogram2DCollection_t & pad_index_hist=histogramList->getHistogram2DCollection(_histGroupKey,_padIndexHistKey);
	histmgr::HistogramCollection_t & border_signal_hist=histogramList->getHistogramCollection(_histGroupKey,_borderSignalHistKey);
#     endif

	for(UInt_t element_i=0; element_i < (UInt_t) col_hits->getNumberOfElements(); element_i++) {
	  EVENT::CalorimeterHit *a_hit=dynamic_cast<EVENT::CalorimeterHit *>(col_hits->getElementAt(element_i));
	  
	  CALICE::CellIndex index(a_hit->getCellID0());
	  
	  UInt_t layer_i=index.getLayerIndex();
	  UInt_t col_i=index.getWaferColumn();
	  UInt_t row_i=index.getWaferRow();
	  if (   wafer_info[layer_i][col_i][row_i].nBorderHits()>=static_cast<UInt_t>(_nHitsMin) 
	      && wafer_info[layer_i][col_i][row_i].nInnerHits() <=static_cast<UInt_t>(_nInsidedHitsMax)) {
#         ifdef BOUNDARY_CHECK
	    assert (necessary_condition);
#         endif	    
	    if (!_clusterColName.empty()) {
	      cluster[layer_i][col_i][row_i]->addHit(a_hit,1.);
	    }
	    Float_t energy=a_hit->getEnergy();
	    border_signal_hist.histogram(0)->fill(energy);

	    wafer_info[layer_i][col_i][row_i].addHitSignal(energy,index.getPadColumn(),index.getPadRow());
	    border_signal_hist.histogram(0)->fill(energy);
#         ifdef WITH_CONTROL_HISTOGRAMS
	    pad_index_hist.histogram(0)->fill(index.getPadColumn(),index.getPadRow());
#         endif
	  }
	  else if (!_clusterColName.empty() ) {
	    other_hits->addHit(a_hit,1.);
#         ifdef WITH_CONTROL_HISTOGRAMS
	    pad_index_hist.histogram(1)->fill(index.getPadColumn(),index.getPadRow());
#         endif
	  }
	}

	if (mark) {
	  histmgr::HistogramCollection_t & total_signal_hist=histogramList->getHistogramCollection(_histGroupKey,_borderTotalSignalHistKey);
	  for (UInt_t layer_i=0; layer_i<=30; layer_i++) {
	    for (UInt_t col_i=0; col_i<=3; col_i++) {
	      for (UInt_t row_i=0; row_i<=3; row_i++) {
		
		if (   wafer_info[layer_i][col_i][row_i].nBorderHits()>=static_cast<UInt_t>(_nHitsMin) 
		       && wafer_info[layer_i][col_i][row_i].nInnerHits() <=static_cast<UInt_t>(_nInsidedHitsMax)) {
		  border_signal_hist.histogram(1)->fill(wafer_info[layer_i][col_i][row_i].maxSignal());
		  total_signal_hist.histogram(0)->fill(wafer_info[layer_i][col_i][row_i].totalSignal());
		}
	      }
	    }
	  }
	}
		
	if (!_clusterColName.empty()) {
	  evtP->addCollection(col_cluster, _clusterColName);
	}

	if (mark) {
	  _nSquareEvents++;
	  int val=n_squares;
	  evtP->parameters().setValue(_squareFlagName,val);
	  CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
	  error.setDirtyEvent();
	  evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
	  //	  std::cout << "EventViewProcessor::processEvent> square event:" <<  evtP->getRunNumber()  << " " << evtP->getEventNumber() << std::endl;
	}
	_nEvents++;
	  
#     ifdef HAVE_CALICEGUI
	if ( mark)  {
	  if (GLVIEW::GuiThread::isInstantiated()) {
	    GLVIEW::GuiThread *gui=GLVIEW::GuiThread::getInstance();
	    gui->markDisplayList( evtP->getRunNumber() , evtP->getEventNumber() );
	  }
	}
#     endif
      }
      catch (lcio::DataNotAvailableException err) {
      }
    }
  }

  void SquareFinder::end() {
    std::cout << "--- " << name() << " Report :" << std::endl
	      << std::setw(9) << _nSquareEvents << " Square event candidates" <<std::endl
	      << std::setw(9) << _nEvents << " n events" <<std::endl;
  }

}
