#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include "EVENT/LCIntVec.h"
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"

#include "marlin/ConditionsProcessor.h"

#include "BmlEventData.hh"

#include "TBTrackMapper.hh"


//namespace CALICE {
  
TBTrackMapper a_TBTrackMapper_instance;

// -- The DriftChamber to TDCHit Processor 

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
TBTrackMapper::TBTrackMapper() : TBTrackBaseProcessor("TBTrackMapper") 
{
    registerProcessorParameter( "TDCHitCollectionName" , 
			      "The name of the output collection which will contain the selected TDCHit." ,
			      _TDCHitColName ,
			      std::string("TBTrackTdcHits"));
  
  registerProcessorParameter( "MappingCollectionName",
			      "Name of the mapping collection with TdcConnection objects",
			      _mappingColName,
			      std::string( "TBTrackTdcMap" ) );
  
  registerProcessorParameter( "UpperLimit",
			      "Upper accepted value in TDC counts." ,
			      _TDC_Upper_Limit,
			      3000);
  
  //34 for backwards compatibility, but should be adapted to current TDC and trigger scheme
  registerProcessorParameter( "LowerLimit",
			      "Lower accepted value in TDC counts (depends on the trigger)." ,
			      _TDC_Lower_Limit,
			      34);

}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackMapper::Init() 
{
  /*  the conditions data with TDC-to-chamber mapping*/
  _mapping = new cMap_t( &CALICE::TdcConnection::getDCIndex );
  marlin::ConditionsProcessor::registerChangeListener( _mapping , _mappingColName );
  _TDC_CAEN1290 = false;
  //  needed eventually
  _firstEvent = true;
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackMapper::ProcessRunHeader( LCRunHeader *run ) 
{
  /* no op */
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackMapper::ProcessEvent( LCEvent * evtP ) 
{
  mMap_t mMap = _mapping->map();
  
  /*  Need to check for negative chamber indices, which indicates that 
      there are two lines to be differentiated per chamber. Have to do 
      that in the event, because the cond data are not yet available 
      during run header processing. Could be done by changeListener,
      but the cable map never changes during a run.*/
  if ( _firstEvent )
    {
      _twoLines = false;
    
      for ( mMap_t::iterator chIter = mMap.begin();  chIter != mMap.end(); ++chIter )   // loop over all chambers
	{
	  int dc = (*chIter).second.getDCIndex();
	  if ( dc < 0 ) _twoLines = true;
	}
      _firstEvent = false;
    }
  
  /*  the main part*/
  try{
    /*  attempt to read collection - this will throw an exception 
	for pedestal and calib events */
    LCCollection* tdcCol = evtP->getCollection( _tdcRawDataCollection ) ;
    
    /* reset private maps and vectors*/
    reset();
   
    /*  find the edges from the TDC collection*/
    findEdges(evtP, tdcCol );

    /*  find hits from the edges*/
    findHits();

    /*  append the hits to the event*/
    evtP->addCollection( outputCollection( _hits ),
			 _TDCHitColName.c_str() );

    /*  append the edges to the event (debug only)*/
    evtP->addCollection( outputCollection( _edges ),
			 "TBTrackMapper_edges" );
  } catch (...)
    {
      /*  catch any exception, meaning do nothing for missing TDC collection*/
  }
  
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackMapper::findEdges(   LCEvent * evtP, EVENT::LCCollection* tdcCol )
{
  /*  This method attempts to find signals from the input TDC collection.
      It will fill the private _edges map.
      
      Up to now, there were two different TDC setups (hardware plus DAQ)
      in operation, which use different data formats and need different
      interpretation of the signals. Addressing of a TDC input and the
      chamber output is universal. The map of all connections defines the
      entity of channels to be processed. So the first step is to get
      the std::map out of the ConditionsMap for looping: */
  
  mMap_t mMap = _mapping->map();

  /*  Then, we need to find out the TDC storage format. The 'old' way, 
      used e.g. at DESY in 2006, stores the raw data in LCIntVec format - 
      so, do a dynamic cast and see whether it succeeds */

  EVENT::LCIntVec* intVec = dynamic_cast<EVENT::LCIntVec*>(tdcCol->getElementAt(0));

  /*old TDC read-out
    In the old setup, each chamber hit results in one TDC signal. The
    'edge' therefore is just a no-intelligence copying. So, just start
    a loop over the channel map: */

  if ( intVec ) 
    {      
      for ( mMap_t::iterator iter = mMap.begin(); iter != mMap.end(); ++iter )
	{
	  //  ... extract the indices ...
	  unsigned int tdc = (*iter).second.getTdcIndex();
	  int dc = (*iter).second.getDCIndex();

	  //  ... make sure that the index of the TDC line is not out of range 
	  if ( (int)tdc >= tdcCol->getNumberOfElements() ) continue;
	  
	  //  ... and copy the data one-to-one
	  intVec = dynamic_cast<EVENT::LCIntVec*>( tdcCol->getElementAt( tdc ));
	  
	  for ( LCIntVec::const_iterator jter = intVec->begin(); jter != intVec->end(); ++jter )
	    _edges[dc].push_back( *jter );
	}
    } 
  else 
    {  
      /*  CAEN read-out
	  ok, apparently, the TDC collection does not contain LCIntVec objects.
	  Then it can be assumed that the data comes in BmlEventData objects.
	  Unfortunately, these are LCGenericObjects, so a dynamic cast for
	  cross check won't work. NEEDS TO BE CHANGED ONCE WE GET A THIRD TDC
	  READOUT BASED ON LCGENERICOBJECTS AGAIN!!!
	  
	  Also in this case, we start a loop over all entries in the
	  channel map: */

      for ( mMap_t::iterator iter = mMap.begin(); iter != mMap.end(); ++iter )
	{
	  /*  ... and extract the indices*/
	  unsigned int tdc = (*iter).second.getTdcIndex();
	  int dc = (*iter).second.getDCIndex();
	  
	  /*  We need to de-compose the TDC module and channel using the
	      appropriate TdcIndex class from the calice_userlib */
	  CALICE::TdcIndex tid( tdc );
	  unsigned int tMod = tid.tdcModule();
	  unsigned int tChn = tid.tdcChannel();
	  
	  /*  Raw data provides one object per TDC module, and we should
	      make sure that the index is within range */
	  if ( (int)tMod >= tdcCol->getNumberOfElements() ) continue;
	  
	  BmlEventData bmlobj = tdcCol->getElementAt(tMod);
	  bmlobj.addSupplementaryInformation(evtP,tMod);
	  
	  if(bmlobj.getTDCType() == "CAEN_1290")
	    _TDC_CAEN1290 = true;
	  
	  /*  The one object per TDC module provides a container with
	      the channel-to-channel data. Also here, we have to make sure 
	      that the channel in the map exists in data. */
	  TDCChannelContainer_t tdcChnls = bmlobj.getTDCChannelContainer();
	  if ( tdcChnls.find( tChn ) == tdcChnls.end() ) continue;

	  /*  The channel-to-channel data comes in a vector of pairs, where
	      the pair contains a bool (to be ignored) and a TDC value.
	      The value is either a rising (positive) or falling (negative) 
	      edge from a pulse on the drift chamber line. We ignore falling
	      edges, and require the rising edges to be inside the expected
	      range. The value of 34 is from the old version of this
	      code and should eventually be changed (-> user parameter instead: _TDC_Lower_Limit). */

	  std::vector< std::pair< bool, int > > tdcVals = tdcChnls[tChn];
	  
	  for ( std::vector< std::pair< bool, int > >::const_iterator jter = tdcVals.begin(); jter != tdcVals.end(); ++jter ) 
	    {
	      int tEdge = (*jter).second;

	      /*CERN CAEN 1290 TDC: data still in counts but limits in ns. 
		Conversion from counts to time done in findHits() */

	      if(_TDC_CAEN1290)
		{
		  if ( tEdge > _TDC_Lower_Limit*40 && tEdge < _TDC_Upper_Limit*40 )
		    _edges[dc].push_back( tEdge );
		}
	      else
		{
		  if ( tEdge > _TDC_Lower_Limit && tEdge < _TDC_Upper_Limit )
		    _edges[dc].push_back( tEdge );
		} 
	    }
	}
    }
  
}


/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackMapper::findHits(){
  /*  This method interpretes the values stored in the _edge map,
      find hits, and stores them in the _hits map. The procedure 
      depends on the drift chambers in use - there are three setups:
      1. drift chambers with one read-out line (e.g. DESY 2006)
      2. drift chambers with two lines to be differentiated (e.g. CERN)
      3. drift chambers similar top one, but with an additional
      TDC signal as reference (so also a differential signal).
      The behaviour is steered the conditions data for the TDC-to-chamber
      mapping, which defines the entity of all chambers in the system.
      Chamber ID's in the map are interpreted with the DCIndex class
      from the calice_userlib. 
      In case differentiating is required, the map should contain two
      entries per chamber with the same index, one being positive and the 
      other negative. The sign defines which line is subtracted from the 
      other.
      In case no negative indices are present, the values from the _edges
      map are copied to the _hits map one-to-one. */
  
  /*  So, once again, we need to get the channel map and start a loop*/
  mMap_t mMap = _mapping->map();  

  for ( mMap_t::iterator chIter = mMap.begin(); chIter != mMap.end(); ++chIter )
    {
      /*  ... extract the drift chamber index and omitt all negative ones
	  (these will come later) */
      int dc = (*chIter).second.getDCIndex();
      if ( dc < 0 ) 
	{
	  continue; 
	}

      /* ... there is nothing to be done if there where no signals found 
	 by the findEdges() method */
      if ( _edges.find( dc ) == _edges.end() ) 
	{
	  continue;
	}

      /* Otherwise, remember the vector of signals found*/
      value_t pVec = (*_edges.find( dc )).second;
      if ( pVec.size() < 1 ) continue;

      /* In case there are two lines (this is evaluated from the
	 channel map in the ProcessEvent(LCEvent*) method), get also
	 that vector from the same chamber id with negative sign. 
	 Be careful with ID = 0 (-0 is minInf), and do nothing in case
	 no signals are present */
      if ( _twoLines )
	{
	  int mdc = -dc;
	  //  todo: change to minimum int (maximum neg. int)
	  if ( dc == 0 ) mdc = (0x80000000);
	  if ( _edges.find( mdc ) == _edges.end() ) 
	    {
	      continue;
	    }
	  value_t nVec = (*_edges.find( mdc )).second;

	  /*  A hit signal is the difference of the two TDC lines. In case 
	      there are more than one signal on one of the lines, we need to
	      store any combination as possible hit. This is the place where
	      a hit sanity cut could be placed. */
     
	  //CERN CAEN 1290 TDC
	  if(_TDC_CAEN1290)
	    {
	      for ( value_t::const_iterator pIter = pVec.begin(); pIter != pVec.end(); ++pIter )
		{
		  for ( value_t::const_iterator nIter = nVec.begin(); nIter != nVec.end(); ++nIter )
		    {
		      /* _TDC_Upper_Limit-_TDC_Lower_Limit should be in the range between  250 and 300 ns 
			 where 250 ns is the maximal delay time of the chamber. */
		      streamlog_out(DEBUG)<<"Drift chamber index: "<<dc<<" "<<(*pIter)<<" "<<(*nIter)<<" "<<((*nIter) - (*pIter))<<endl;

		      if((*nIter) - (*pIter)< _TDC_Upper_Limit*40-_TDC_Lower_Limit*40)
			{
			  _hits[dc].push_back( (*nIter) - (*pIter) );
			}
		    }
		}
	    }
	  else
	    {
	      for ( value_t::const_iterator pIter = pVec.begin(); pIter != pVec.end(); ++pIter )
		{
		  for ( value_t::const_iterator nIter = nVec.begin(); nIter != nVec.end(); ++nIter )
		    {
		      //  todo: some possible hits cuts here
		      _hits[dc].push_back( (*pIter) - (*nIter) );
		    }
		}
	    }
	} 
      else 
	{ 
	  /*  If there are not two lines, the game is simple again: Just copy*/
	  for ( value_t::const_iterator iter = pVec.begin(); iter != pVec.end(); ++iter )
	    {
	      _hits[dc].push_back(*iter);
	    }
	}
    }
}


/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
EVENT::LCCollection* TBTrackMapper::outputCollection( const vMap_t& data ) 
{
  /*  This is a simple routine to store the TDC values (of either edges or
      hits) in a collection of LCIntVec objects, one per chamber. The first
      element is the chamber id (according to the mapping conditions data). */
  
  IMPL::LCCollectionVec* col = new IMPL::LCCollectionVec( LCIO::LCINTVEC );

  for ( vMap_t::const_iterator chIter = data.begin(); chIter != data.end(); ++chIter )
    {
      EVENT::LCIntVec *vec = new EVENT::LCIntVec();
      vec->push_back( (*chIter).first );
      value_t dVec = (*chIter).second;

      for ( value_t::const_iterator iter = dVec.begin(); iter != dVec.end(); ++iter )
	{
	  vec->push_back( (*iter) );
	}
      col->addElement( vec );
    }
  return col;
}

  
/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackMapper::End() {
  /* no op */
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackMapper::reset() {
  //  clear the private maps for edges and hits
  _edges.clear();
  _hits.clear();
}



