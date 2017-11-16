#include "TcmtMuonTracker.hh"

#include <iostream>
#include <iterator>

#include "UTIL/LCTypedVector.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"

#include "marlin/Exceptions.h"
#include "CaliceException.hh"

using std::cout;
using std::endl;

namespace CALICE {

  TcmtMuonTracker aTcmtMuonTracker;

  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  TcmtMuonTracker::TcmtMuonTracker() : marlin::Processor("TcmtMuonTracker") 
  {
    registerProcessorParameter( "MipThreshold" ,
                                "Hits with energies below this threshold are ignored" ,
                                _mipThreshold ,
                                float(0.5) );

    registerProcessorParameter( "MaxLayerK" ,
                                "maximum layer number taken into account - counting layers (k) from 1 to 16" ,
                                _kThreshold ,
                                int(16) );

    registerProcessorParameter( "TCMT_HitColName" ,
                                "The name of the TCMT hit input collection" ,
                                _tcmHitColName ,
                                std::string("TcmtCalorimeter_Hits") );

    registerProcessorParameter( "OutputMuonColName" ,
                                "The name of the muon hits output collection" ,
                                _outputColName,
                                std::string("TcmtMuonCalorimeter_Hits") );

    registerProcessorParameter( "TcmtStartVertical" ,
                                "The TCMT starts with vertical strips" ,
                                _tcmtStartVertical,
                                bool(false) ) ;

    _mu_nHits_isolated.push_back(5);
    registerProcessorParameter( "mu_nHits_isolated" ,
                                "The minimum number of isolated hits required for a tower"
				" (combined horizontal and vertical strips) for identifying a muon in the event" ,
                                _mu_nHits_isolated ,
                                _mu_nHits_isolated );

    _mu_eSum_isolated.push_back(0);
    registerProcessorParameter( "mu_eSum_isolated" ,
                                "The minimum energy sum (isolated hits) required for a tower "
				"(combined horizontal and vertical strips) for identifying a muon in the event" ,
                                _mu_eSum_isolated ,
                                _mu_eSum_isolated );
  }

  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void TcmtMuonTracker::init() 
  {

    printParameters();

    bool error = false;

    if ( _mu_nHits_isolated.size() != _mu_eSum_isolated.size() ) 
      {
	streamlog_out(ERROR) << "mu_nHits_isolated has  " << _mu_nHits_isolated.size() << " elements" << endl
		     << "mu_eSum_isolated has   " << _mu_eSum_isolated.size() << " elements" << endl;
	error = true;
      }
    
    /* initialize empty tower struct
     */
    _newTower.i             = 0;
    _newTower.j             = 0;
    _newTower.eSum          = 0;
    _newTower.eSum_front    = 0;
    _newTower.eSum_back     = 0;
    _newTower.nHits         = 0;
    _newTower.nHits_front   = 0;
    _newTower.nHits_back    = 0;
    _newTower.xstart        = 1000000;
    _newTower.xend          = -1000000;
    _newTower.ystart        = 1000000;
    _newTower.yend          = -1000000;
    _newTower.zstart        = 1000000;
    _newTower.zend          = -1000000;
    _newTower.trackID_nHits = 0;
    _newTower.trackID_eSum  = 0;
    _newTower.nNeighbours   = 0;
    _newTower.col_nHits     = 0;
    _newTower.col_eSum      = 0;

    if (error) throw marlin::StopProcessingException(this);

  }

  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void TcmtMuonTracker::processEvent( LCEvent * evt ) 
  {
    /* maps of TCMT hits
     */
    std::map<unsigned, CalorimeterHit*> hitMap_all;

    /* list of i and j values that have hits - defines coordinates of possible towers
     */
    std::list<unsigned> list_i_hit;
    std::list<unsigned> list_j_hit;

    /* map of TCMT towers
     */
    std::map<unsigned,tcmtTower> tcmtTowerMap_2d; // combined tower horizontal + vertical strips
    //std::map<unsigned,tcmtTower> tcmtTowerMap_2d_merged; // towers after merging procedure

    /* vectors with parameters of all towers
     */
    FloatVec allTower_x;
    FloatVec allTower_y;
    FloatVec allTower_eSum;
    IntVec allTower_nHits;
    FloatVec allTower_trackID_eSum;
    IntVec allTower_trackID_nHits;

    /* parameters of maximum tower */
    tcmtTower maxTower = _newTower;

    /* number of different configurations checked */
    int nConfigs = _mu_nHits_isolated.size();

    /* vector for storing the configuration number */
    IntVec configs;

    /* */
    IntVec tcmIsMuonEvent;

    for ( int config = 0; config < nConfigs; config++ )
      {
        // store number of configuration
        configs.push_back(config+1);
        tcmIsMuonEvent.push_back(0);
      }

    /* TCMT hit input collection
     */
    LCCollection* hitCol = NULL;

    try {
      hitCol = evt->getCollection( _tcmHitColName );
    }
    catch ( DataNotAvailableException err ) 
      {
	streamlog_out(DEBUG) <<  "TcmtMuonTracker WARNING: Collection "<< _tcmHitColName
		     << " not available in event "<< evt->getEventNumber() << endl;
	return;
      }
    
    streamlog_out(DEBUG)<<"\n EVENT "<<evt->getEventNumber()<<", TCMT has "<<hitCol->getNumberOfElements()<<" hits"<<endl;

    LCTypedVector<CalorimeterHit> tcmHits( hitCol );
    LCTypedVector<CalorimeterHit>::iterator tcmHitIt;
    
    /* get decoder for correct I, J, K values
     */
    DecoderSet *decoder = new DecoderSet( hitCol->getParameters().getStringVal("CellIDEncoding") , "", "" );
    
    /* loop over all hits in input collection
     */
    for ( tcmHitIt = tcmHits.begin(); tcmHitIt != tcmHits.end(); tcmHitIt++ ) 
      { 
	if  ( (*tcmHitIt)->getEnergy() >= _mipThreshold ) 
	  {
	    /* get i, j, k of hit from decoder
	     */
	    int i_pos = decoder->getIFromCellID( (*tcmHitIt)->getCellID0() );
	    int j_pos = decoder->getJFromCellID( (*tcmHitIt)->getCellID0() );
	    int k_pos = decoder->getKFromCellID( (*tcmHitIt)->getCellID0() );
	    
	    if ( k_pos <= _kThreshold ) 
	      {
		/* fill map with all TCMT hits
		 */
		hitMap_all.insert( std::make_pair( (*tcmHitIt)->getCellID0(), *tcmHitIt ) );
		
		/* check orientation of hit strip
		 */
		char orient = ' ';
		if (_tcmtStartVertical) orient = ( k_pos %2 ? 'H' :'V' );
		else                    orient = ( k_pos %2 ? 'V' :'H' );
		
		/* add i or j to list of hit coordinates
		 */
		if ( orient == 'V' ) // vertical strip
		  {
		    list_i_hit.push_back( i_pos );
		  }
		if ( orient == 'H' ) // horizontal strip
		  list_j_hit.push_back( j_pos );
		
// 		streamlog_out(DEBUG) <<
// 		  "*** new TCMT hit ***" << "\n" <<
// 		  "orientation: " << orient << "\n" <<
// 		  //"isolated   : " << isolated << "\n" <<
// 		  "index     i: " << i_pos << "\n" <<
// 		  "index     j: " << j_pos << "\n" <<
// 		  "index     k: " << k_pos << "\n" <<
// 		  "********************" << endl;		
	      }
	    
	  } // end single hit treatment
	
      } // end loop over all hits
    
    /* sort lists of used i,j va;ues, remove duplicates */
    list_i_hit.sort();
    list_i_hit.unique();
    
    list_j_hit.sort();
    list_j_hit.unique();

    streamlog_out(DEBUG)<<" list sizes i="<<list_i_hit.size()<<" j="<<list_j_hit.size()<<endl;
    
    streamlog_out(DEBUG) << "* size of tower map (before filling): " << tcmtTowerMap_2d.size() << endl;
    
    /* fill tower map using core algorithm */
    find2DTowers( hitMap_all, decoder, list_i_hit, list_j_hit, tcmtTowerMap_2d );
    //find2DCrosses( hitMap_all, decoder, list_i_hit, list_j_hit, tcmtTowerMap_2d );
    
    streamlog_out(DEBUG) << "* size of tower map (after filling) : " << tcmtTowerMap_2d.size() << endl;
    
    /* merge towers
     */
    //      merge2DTowers( tcmtTowerMap_2d , tcmtTowerMap_2d_merged );
    
    //      streamlog_out(DEBUG) << "* size of tower map (after merging) : " << tcmtTowerMap_2d_merged.size() << endl;
    
    /*-----------------------------------------------------------------------------------------
      find tower with maximum hits */
    std::map<unsigned,tcmtTower>::iterator towerItMax;
    bool firstTower = true;
    
    /* if first tower: choose as first maximum tower */
    for ( towerItMax = tcmtTowerMap_2d.begin(); towerItMax != tcmtTowerMap_2d.end(); towerItMax++ ) 
      {
	if ( firstTower ) 
	  {
	    maxTower = (towerItMax->second);
	    
	    firstTower=false;
	  }
	
        /* check if tower has more hits (more energy if equal number of hits) than previous maximum tower */
        if ( (towerItMax->second).trackID_nHits > maxTower.trackID_nHits 
	     || (towerItMax->second).trackID_nHits == maxTower.trackID_nHits && (towerItMax->second).trackID_eSum > maxTower.trackID_eSum ) 
	  {
	    maxTower = (towerItMax->second);
	  }
      }
    /* END find tower with maximum hits --------------------------------------------------------*/
    
    streamlog_out(DEBUG) << "* maximum tower: eSum  = " << maxTower.eSum << endl;
    streamlog_out(DEBUG) << "* maximum tower: nHits = " << maxTower.nHits << endl;
    streamlog_out(DEBUG) << "* maximum tower: x     = " << maxTower.xstart << endl;
    streamlog_out(DEBUG) << "* maximum tower: y     = " << maxTower.ystart << endl;
    streamlog_out(DEBUG) << "* maximum tower: trackID_eSum  = " << maxTower.trackID_eSum << endl;
    streamlog_out(DEBUG) << "* maximum tower: trackID_nHits = " << maxTower.trackID_nHits << endl;
    

    /* check for neighbours of maximum tower
     */
    findNeighbours( maxTower , tcmtTowerMap_2d );
    
    streamlog_out(DEBUG) << "* maximum tower: col_eSum  = " << maxTower.col_eSum << endl;
    streamlog_out(DEBUG) << "* maximum tower: col_nHits = " << maxTower.col_nHits << endl;
    
    
    /* check if requirements for muon ID are met
     *
     * loop over all configurations (= different threshold settings)
     *
     * @TODO study purity / efficiency at various beam energies for varying thresholds
     *
     */
    for ( int config = 0; config <= nConfigs; config++ ) 
      {
	if ( maxTower.nHits >= _mu_nHits_isolated[config] && maxTower.eSum >= _mu_eSum_isolated[config] )
          {
            tcmIsMuonEvent[config]=1;
            streamlog_out(DEBUG) << "***** EVENT " << evt->getEventNumber() << " - Configuration " 
			 << config << ": MUON IDENTIFIED!!! *****" << endl;
          }
        else
          streamlog_out(DEBUG) << "***** EVENT " << evt->getEventNumber() << " - Configuration " 
		       << config << ": no muon found. *****" << endl;
      }
    
    /*--------------------------------------------------------------------------------------
      loop over all towers, collect hits, energy, position from all towers to vectors */
    std::map<unsigned,tcmtTower>::iterator towerIt;
    
    for ( towerIt = tcmtTowerMap_2d.begin(); towerIt != tcmtTowerMap_2d.end(); towerIt++ ) 
      {
	allTower_eSum.push_back( (towerIt->second).eSum );
	allTower_nHits.push_back( (towerIt->second).nHits );
	
	allTower_x.push_back( (towerIt->second).xstart );
	allTower_y.push_back( (towerIt->second).ystart );
	
	allTower_trackID_eSum.push_back( (towerIt->second).trackID_eSum );
	allTower_trackID_nHits.push_back( (towerIt->second).trackID_nHits );
	
      } /* END loop over all towers, collect hits, energy, position from all towers to vectors
	   -----------------------------------------------------------------------------------*/
    
//     streamlog_out(DEBUG) << "* length of vector (all towers, eSum)         : " << allTower_eSum.size() << endl;
//     streamlog_out(DEBUG) << "* length of vector (all towers, nHits)        : " << allTower_nHits.size() << endl;
//     streamlog_out(DEBUG) << "* length of vector (all towers, x)            : " << allTower_x.size() << endl;
//     streamlog_out(DEBUG) << "* length of vector (all towers, y)            : " << allTower_y.size() << endl;
//     streamlog_out(DEBUG) << "* length of vector (all towers, trackID_eSum) : " << allTower_trackID_eSum.size() << endl;
//     streamlog_out(DEBUG) << "* length of vector (all towers, trackID_nHits): " << allTower_trackID_nHits.size() << endl;

    
    /* loop over all merged towers, collect hits, energy, position from all merged towers to vectors */
    //      for ( towerIt = tcmtTowerMap_2d_merged.begin(); towerIt != tcmtTowerMap_2d_merged.end(); towerIt++ ) {
    //
    //        mergedTower_eSum.push_back( (towerIt->second).eSum );
    //        mergedTower_nHits.push_back( (towerIt->second).nHits );
    //
    //        mergedTower_x.push_back( (towerIt->second).xstart );
    //        mergedTower_y.push_back( (towerIt->second).ystart );
    //
    //        mergedTower_trackID_eSum.push_back( (towerIt->second).trackID_eSum );
    //        mergedTower_trackID_nHits.push_back( (towerIt->second).trackID_nHits );
    //
    //      } // END loop over all merged towers, collect hits, energy, position from all merged towers to vectors
    //
    //      streamlog_out(DEBUG) << "* length of vector (merged towers, eSum) : " << mergedTower_eSum.size() << endl;
    //      streamlog_out(DEBUG) << "* length of vector (merged towers, nHits): " << mergedTower_nHits.size() << endl;
    //      streamlog_out(DEBUG) << "* length of vector (merged towers, x)    : " << mergedTower_x.size() << endl;
    //      streamlog_out(DEBUG) << "* length of vector (merged towers, y)    : " << mergedTower_y.size() << endl;
    //      streamlog_out(DEBUG) << "* length of vector (merged towers, trackID_eSum) : " << mergedTower_trackID_eSum.size() << endl;
    //      streamlog_out(DEBUG) << "* length of vector (merged towers, trackID_nHits): " << mergedTower_trackID_nHits.size() << endl;
    
    
    
    /* if names for parameters set, append number of muon position candidates and nHits and ESum for largest tower to event
     */
    // vectors with energy, hits, x-position, and y-position for ALL towers (up to 400 entries)
    evt->parameters().setValues( name()+"_allTower_eSum" , allTower_eSum );
    evt->parameters().setValues( name()+"_allTower_nHits" , allTower_nHits );
    evt->parameters().setValues( name()+"_allTower_x" , allTower_x );
    evt->parameters().setValues( name()+"_allTower_y" , allTower_y );
    evt->parameters().setValues( name()+"_allTower_trackID_eSum" , allTower_trackID_eSum );
    evt->parameters().setValues( name()+"_allTower_trackID_nHits" , allTower_trackID_nHits );
    
    //      // vectors with energy, hits, x-position, and y-position for MERGED towers (up to 400 entries)
    //      evt->parameters().setValues( name()+"_mergedTower_eSum" , mergedTower_eSum );
    //      evt->parameters().setValues( name()+"_mergedTower_nHits" , mergedTower_nHits );
    //      evt->parameters().setValues( name()+"_mergedTower_x" , mergedTower_x );
    //      evt->parameters().setValues( name()+"_mergedTower_y" , mergedTower_y );
    //      evt->parameters().setValues( name()+"_mergedTower_trackID_eSum" , mergedTower_trackID_eSum );
    //      evt->parameters().setValues( name()+"_mergedTower_trackID_nHits" , mergedTower_trackID_nHits );
    
    // valid for all configurations
    evt->parameters().setValue( name()+"_maxTower_eSum", maxTower.eSum );
    evt->parameters().setValue( name()+"_maxTower_nHits", maxTower.nHits );
    evt->parameters().setValue( name()+"_maxTower_x", maxTower.xstart );
    evt->parameters().setValue( name()+"_maxTower_y", maxTower.ystart );
    evt->parameters().setValue( name()+"_maxTower_trackID_eSum" , maxTower.trackID_eSum );
    evt->parameters().setValue( name()+"_maxTower_trackID_nHits" , maxTower.trackID_nHits );
    
    evt->parameters().setValue( name()+"_maxTower_nNeighbours" , maxTower.nNeighbours );
    evt->parameters().setValue( name()+"_maxTower_col_eSum" , maxTower.col_eSum );
    evt->parameters().setValue( name()+"_maxTower_col_nHits" , maxTower.col_nHits );

    
    // configuration dependent numbers
    evt->parameters().setValues( name()+"_MuonBit", tcmIsMuonEvent );
    evt->parameters().setValues( name()+"_config", configs );
    
    
    this->fillOutputCollection(evt, maxTower, hitCol, hitMap_all, decoder);

  } 
  
  
  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void TcmtMuonTracker::find2DTowers( std::map<unsigned, CalorimeterHit*> hitMap_all, 
				      DecoderSet* decoder, std::list<unsigned> list_i_hit, 
				      std::list<unsigned> list_j_hit, 
				      std::map<unsigned,tcmtTower> &tcmtTowerMap_2d ) 
  {
    /* loop over all possible combinations of i-j-hits ( create 2D-towers )
     */
    for ( std::list<unsigned>::iterator listit_i = list_i_hit.begin(); listit_i != list_i_hit.end(); listit_i++ ) 
      {
	for ( std::list<unsigned>::iterator listit_j = list_j_hit.begin(); listit_j != list_j_hit.end(); listit_j++ ) 
	  {
	    unsigned i_pos = (*listit_i);
	    unsigned j_pos = (*listit_j);

	    
	    /* define tower-id integer
	     */
	    unsigned towerid = getTowerIDfromIJ( i_pos, j_pos );
	    
	    /* check if struct for tower exists - add if not
	     */
	    if ( tcmtTowerMap_2d.find(towerid) == tcmtTowerMap_2d.end() ) 
	      {
		tcmtTowerMap_2d.insert( std::make_pair(towerid, _newTower) );
	      }
	    
	    /* loop over all layers and add hits belonging to tower
	     */
	    std::map<unsigned,tcmtTower>::iterator towerIt;
	    	    
	    towerIt = tcmtTowerMap_2d.find(towerid);
	    
	    (towerIt->second).i = i_pos;
	    (towerIt->second).j = j_pos;
	    
	    for ( unsigned k_pos = 0; k_pos <= MAX_TCMT_LAYERS; k_pos++ )
	      {
		unsigned i_pos_n = i_pos;
		unsigned j_pos_n = j_pos;
		
		/* check orientation of strip to check
		 */
		char orient = ' ';
		if (_tcmtStartVertical) orient = ( k_pos %2 ? 'H' :'V' );
		else                    orient = ( k_pos %2 ? 'V' :'H' );
		
		/* vertical strip */
		if ( orient == 'V' )
		  j_pos_n = 0;
		
		/* horizontal strip */
		if ( orient == 'H' )
		  i_pos_n = 0;
		
		/* if strip found: add info to tower */
		std::map<unsigned, CalorimeterHit*>::iterator hitMapIt;
		hitMapIt = ( hitMap_all.find( decoder->getCellID( i_pos_n, j_pos_n, k_pos ) ) );
		if ( hitMapIt != hitMap_all.end() ) 
		  {		  
		    /* check if hits in layers k+1 or k-1 contribute to current tower or if hit is isolated in z-direction */
		    bool isolated_z = true;
		    
		    if ( orient == 'V' ) 
		      {
			if ( hitMap_all.find( decoder->getCellID( 0, j_pos, k_pos+1 ) ) != hitMap_all.end() )
			  isolated_z = false;
			
			if ( hitMap_all.find( decoder->getCellID( 0, j_pos, k_pos-1 ) ) != hitMap_all.end() )
			  isolated_z = false;			
		      }

		    if ( orient == 'H' ) 
		      {
			if ( hitMap_all.find( decoder->getCellID( i_pos, 0, k_pos+1 ) ) != hitMap_all.end() )
			  isolated_z = false;
			
			if ( hitMap_all.find( decoder->getCellID( i_pos, 0, k_pos-1 ) ) != hitMap_all.end() )
			  isolated_z = false;			
		      }

		    /* only add hits to trackID part of tower that are not isolated in z */
		    if ( isolated_z == false ) 
		      {
			(towerIt->second).trackID_eSum += ( hitMapIt->second )->getEnergy();
			(towerIt->second).trackID_nHits++;			
		      }
		    
		    (towerIt->second).eSum += ( hitMapIt->second )->getEnergy();
		    (towerIt->second).nHits++;
		    
		    double x = (hitMapIt->second)->getPosition()[0];
		    double y = (hitMapIt->second)->getPosition()[1];
		    double z = (hitMapIt->second)->getPosition()[2];
		    
		    if ( k_pos > 8 ) 
		      {
			(towerIt->second).eSum_back += ( hitMapIt->second )->getEnergy();
			(towerIt->second).nHits_back++;
		      }
		    else 
		      {
			(towerIt->second).eSum_front += ( hitMapIt->second )->getEnergy();
			(towerIt->second).nHits_front++;
		      }
		    
		    /* separate orientations for position information
		     */
		    // case 1: no x-position set for tower
		    if ( orient == 'V' && ( (towerIt->second).xstart > 100000 ) ) 
		      {
			(towerIt->second).xstart = x;
			(towerIt->second).xend = x;
		      }
		    
		    // case 1: no y-position set for tower
		    if ( orient == 'H' && ( (towerIt->second).ystart > 100000 ) ) 
		      {
			(towerIt->second).ystart = y;
			(towerIt->second).yend = y;
		      }
		    
		    // case 3: new z position before earliest previous z-position
		    if ( z < (towerIt->second).zstart ) 
		      {
			(towerIt->second).zstart = z;
			
			if ( orient == 'V' )
			  (towerIt->second).xstart = x;
			
			if ( orient == 'H' )
			  (towerIt->second).ystart = y;			
		      }
		    
		    // case 4: new z position after latest previous z-position
		    if ( z > (towerIt->second).zend ) 
		      {
			(towerIt->second).zend = z;
			
			if ( orient == 'V' )
			  (towerIt->second).xend = x;
			
			if ( orient == 'H' )
			  (towerIt->second).yend = y;			
		      }
		    
		  } // END treat strip_i in tower
		
	      } // END loop over all layers and add hits belonging to tower
	    
	    /* check: if tower was added but is empty, remove tower */
	    if ( (towerIt->second).trackID_nHits == 0 )
	      tcmtTowerMap_2d.erase(towerIt);
	    
	  } // END loop over tower (j)
	
      } // END loop over all possible combinations of i-j-hits ( create 2D-towers )
    
    /* @TODO merge towers
     */
    //...
    
  } // END find2DTowers( ... )
  
  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void TcmtMuonTracker::findNeighbours( tcmtTower &tcmtTowerX , std::map<unsigned,tcmtTower> &tcmtTowerMap_2d ) 
  {
    /* initial energy and hits from central tower*/
    tcmtTowerX.col_nHits = tcmtTowerX.nHits;
    tcmtTowerX.col_eSum = tcmtTowerX.eSum;
    
    /* add data from neighbouring towers*/
    for ( int icount = -1; icount <= 1; icount++ ) 
      {
	for ( int jcount = -1; jcount <= 1; jcount++ ) 
	  {
	    /* skip center tower itself*/
	    if ( icount == 0 && jcount == 0 ) continue;
	    
	    unsigned itest = tcmtTowerX.i + icount;
	    unsigned jtest = tcmtTowerX.j + jcount;
	    unsigned idtest = getTowerIDfromIJ( itest , jtest );

	    /*check if tower exists*/
	    if ( tcmtTowerMap_2d.find(idtest) == tcmtTowerMap_2d.end() )
	      continue;

	    /* if tower exists: get pointer*/
	    std::map<unsigned,tcmtTower>::iterator towertest = tcmtTowerMap_2d.find(idtest);
	    
	    /* if tower has at least two connected hit, add neighbour and infomration*/
	    if ( (towertest->second).trackID_nHits > 0 ) 
	      {
		tcmtTowerX.nNeighbours++;
		tcmtTowerX.col_nHits += (towertest->second).nHits ;
		tcmtTowerX.col_eSum += (towertest->second).eSum ;		
	      }
	    
	  }
	
      }
    
    return;

  };
  
  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void TcmtMuonTracker::fillOutputCollection(LCEvent *evt, tcmtTower &muonTower, LCCollection *inputCol, 
					     std::map<unsigned, CalorimeterHit*> hitMap_all,
					     DecoderSet* decoder)
  {
    unsigned int I = muonTower.i;
    unsigned int J = muonTower.j;

    streamlog_out(DEBUG)<<"begin of fillOutputCollection: max I="<<I<<" J="<<J<<endl;

    LCCollection *outputCol  = new LCCollectionVec(LCIO::CALORIMETERHIT);
    /*we want to save the position, and this can only be done if the CHBIT_LONG bit is set*/
    outputCol->setFlag(outputCol->getFlag() | 1 << LCIO::CHBIT_LONG );

    /*start loop over TCMT layers*/
    for ( unsigned k = 0; k <= MAX_TCMT_LAYERS; k++ )
      {
	unsigned i_pos_n = I;
	unsigned j_pos_n = J;

	/* check orientation of strip to check
	 */
	char orient = ' ';
	if (_tcmtStartVertical) orient = ( k %2 ? 'H' :'V' );
	else                    orient = ( k %2 ? 'V' :'H' );
	
	/* vertical strip */
	if ( orient == 'V' )
	  j_pos_n = 0;
	
	/* horizontal strip */
	if ( orient == 'H' )
	  i_pos_n = 0;

	std::map<unsigned, CalorimeterHit*>::iterator hitMapIt;
	hitMapIt = ( hitMap_all.find( decoder->getCellID( i_pos_n, j_pos_n, k ) ) );
	if ( hitMapIt != hitMap_all.end() ) 
	  {		  
	    streamlog_out(DEBUG)<<" found hit with I/J/K="<<I<<"/"<<J<<"/"<<k<<endl;

	    CalorimeterHitImpl *muonHit = new CalorimeterHitImpl();
	    muonHit->setEnergy((hitMapIt->second)->getEnergy());
	    muonHit->setPosition((hitMapIt->second)->getPosition());
	    muonHit->setCellID0((hitMapIt->second)->getCellID0());

	    /*Angela Lucaci: it seems that the hits belong to the hitMap, so
	      I cannot just do:
	      outputCol->addElement(hitMapIt->second);
	      because Marlin then tries to delete hits which do not belong to it,
	      and I get a segmentation. Therefore I need to create a new hit.*/
	    outputCol->addElement(muonHit);
	  }

      }/*--------------------- end loop over TCMT layers ----------------------------*/


    if (outputCol->getNumberOfElements() > 0)
      {
	const std::string encodingString = inputCol->getParameters().getStringVal("CellIDEncoding");
	LCParameters &param = outputCol->parameters();
	param.setValue(LCIO::CellIDEncoding, encodingString);

	evt->addCollection(outputCol, _outputColName.c_str());

	streamlog_out(DEBUG)<<"\nFound "<<outputCol->getNumberOfElements()<<" muon hits, saved in col "
		    <<_outputColName
		    <<endl;
      }
    else
      {
	delete outputCol;
      }

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/

  //  void TcmtMuonTracker::find2DCrosses( std::map<unsigned, CalorimeterHit*> hitMap_all, DecoderSet* decoder, std::list<unsigned> list_i_hit, std::list<unsigned> list_j_hit, std::map<unsigned,tcmtTower> &tcmtTowerMap_2d ) {
  //
  //    /* loop over all possible combinations of i-j-hits ( create 2D-towers )
  //     */
  //    for ( std::list<unsigned>::iterator listit_i = list_i_hit.begin(); listit_i != list_i_hit.end(); listit_i++ ) {
  //
  //      for ( std::list<unsigned>::iterator listit_j = list_j_hit.begin(); listit_j != list_j_hit.end(); listit_j++ ) {
  //
  //        unsigned i_pos = (*listit_i);
  //        unsigned j_pos = (*listit_j);
  //
  //        /* define tower-id integer
  //         */
  //        unsigned towerid = i_pos * 100 + j_pos;
  //
  //        /* check if struct for tower exists - add if not
  //         */
  //        if ( tcmtTowerMap_2d.find(towerid) == tcmtTowerMap_2d.end() ) {
  //          tcmtTowerMap_2d.insert( std::make_pair(towerid, _newTower) );
  //        }
  //
  //        /* loop over all layers and add hits belonging to tower
  //         */
  //        std::map<unsigned,tcmtTower>::iterator towerIt;
  //
  //        unsigned kmax=17;
  //        towerIt = tcmtTowerMap_2d.find(towerid);
  //        for ( unsigned k_pos = 1; k_pos < kmax; k_pos++ )
  //          {
  //
  //            //      /* check only every 4th layer, i.e. the first of each super-layer type A */
  //            //            if ( k_pos % 4 != 1 )
  //            //              continue;
  //
  //            /* check all vertical strips */
  //            if ( k_pos % 2 != 1 )
  //              continue;
  //
  //            unsigned i_pos_n = i_pos;
  //            unsigned j_pos_n = 0;
  //
  //            /* iterator for accessing hits */
  //            std::map<unsigned, CalorimeterHit*>::iterator hitMapIt;
  //
  //            /* if cross found: add info to tower */
  //            bool hit_cross = false;
  //
  //            hitMapIt = ( hitMap_all.find( decoder->getCellID( i_pos_n, j_pos_n, k_pos ) ) );
  //            if ( hitMapIt != hitMap_all.end() ) {
  //
  //              /* check if hits in layers k+1 match for cross for current tower */
  //              if ( hitMap_all.find( decoder->getCellID( 0, j_pos, k_pos+1 ) ) != hitMap_all.end() )
  //                hit_cross = true;
  //
  //            }
  //
  //            /* only add hits to tower that are not isolated in z */
  //            if ( hit_cross ) {
  //
  //              (towerIt->second).nHits+=2;
  //              (towerIt->second).trackID_nHits+=2;
  //
  //              /* get information from vertical strip of cross */
  //              hitMapIt = hitMap_all.find( decoder->getCellID( i_pos, 0, k_pos ) );
  //              double x_v = (hitMapIt->second)->getPosition()[0];
  //              //double y_v = (hitMapIt->second)->getPosition()[1];
  //              double z_v = (hitMapIt->second)->getPosition()[2];
  //              double E_v = (hitMapIt->second)->getEnergy();
  //
  //              /* get information from horizontal strip of cross */
  //              hitMapIt = hitMap_all.find( decoder->getCellID( 0, j_pos, k_pos+1 ) );
  //              //double x_h = (hitMapIt->second)->getPosition()[0];
  //              double y_h = (hitMapIt->second)->getPosition()[1];
  //              double z_h = (hitMapIt->second)->getPosition()[2];
  //              double E_h = (hitMapIt->second)->getEnergy();
  //
  //              /* add strip energies to tower energy */
  //              (towerIt->second).eSum += ( E_v + E_h );
  //              (towerIt->second).trackID_eSum += ( E_v + E_h );
  //
  //              /* add front/back hits and energy */
  //              if ( k_pos >= 8 ) {
  //                (towerIt->second).eSum_back += (E_v + E_h);
  //                (towerIt->second).nHits_back+=2;
  //              }
  //              else {
  //                (towerIt->second).eSum_front += (E_v + E_h);
  //                (towerIt->second).nHits_front+=2;
  //              }
  //
  //              /* separate orientations for position information
  //               */
  //              // case 1: no position set for tower
  //              if ( (towerIt->second).xstart > 100000 || (towerIt->second).ystart > 100000 ) {
  //                (towerIt->second).xstart = x_v;
  //                (towerIt->second).xend = x_v;
  //                (towerIt->second).ystart = y_h;
  //                (towerIt->second).yend = y_h;
  //              }
  //
  //              // case 2: new z position before earliest previous z-position
  //              if ( z_v < (towerIt->second).zstart ) {
  //                (towerIt->second).zstart = z_v;
  //                (towerIt->second).xstart = x_v;
  //                (towerIt->second).ystart = y_h;
  //              }
  //
  //              // case 3: new z position after latest previous z-position
  //              if ( z_h > (towerIt->second).zend ) {
  //                (towerIt->second).zend = z_h;
  //                (towerIt->second).xend = x_v;
  //                (towerIt->second).yend = y_h;
  //              }
  //
  //            }
  //
  //          }
  //
  //        /* check: if tower was added but is empty, remove tower */
  //        if ( (towerIt->second).nHits == 0 )
  //          tcmtTowerMap_2d.erase(towerIt);
  //
  //      }
  //
  //    } // END loop over all possible combinations of i-j-hits ( create 2D-towers )
  //
  //    /* @TODO merge towers
  //     */
  //    //...
  //
  //
  //
  //  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void TcmtMuonTracker::merge2DTowers( std::map<unsigned,tcmtTower> &tcmtTowerMap_2d , 
				       std::map<unsigned,tcmtTower> &tcmtTowerMap_2d_merged ) 
  {
    tcmtTowerMap_2d_merged = tcmtTowerMap_2d;
  }



} // end namespace CALICE
