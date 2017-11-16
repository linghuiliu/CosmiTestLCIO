#include <cassert>
#include <iostream>
#include <string>

#include "lcio.h"
#include "IO/LCWriter.h"
#include "EVENT/LCGenericObject.h"
#include "EVENT/LCIntVec.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/LCTime.h"
#include "LCIOSTLTypes.h"

#include "TBTrackProducer.hh"


using namespace lcio;
using namespace marlin;
using namespace TBTrack;

using std::cout;
using std::endl;

TBTrackProducer aTBTrackProducer;


/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
TBTrackProducer::TBTrackProducer() : TBTrackBaseProcessor("TBTrackProducer") 
{
  _description="TBTrackProducer" ;
   
  registerProcessorParameter( "TDCHitCollectionName" , 
			      "The name of the output collection which will contain the selected TDCHit." ,
			      _TDCHitColName ,
			      std::string("TBTrackTdcHits"));
}


/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackProducer::ProcessEvent(LCEvent *evt) 
{

  /* Check there are some constants*/
  if(!_alnConstantsValid || !_fitConstantsValid) 
    {
      if(!_alnConstantsValid) cout << "No valid AlnConstants" << endl;
      if(!_fitConstantsValid) cout << "No valid FitConstants" << endl;
      assert(false);   
    }
  
  /* If they have changed, update the track finder*/
  if(_alnConstantsUpdated) 
    {
      _finder.alnConstants(_alnConstants);

      if(printLevel(1)) 
	{
	  cout<< "Updating TrackFinder with AlnConstants" << endl;
	  _alnConstants.print(std::cout,name()+": ") << endl;
	}
    }
  
  if(_fitConstantsUpdated) 
    { 
      _finder.fitConstants(_fitConstants);
      
      if(printLevel(1)) 
	{
	  cout << "Updating TrackFinder with FitConstants" << endl;
	  _fitConstants.print(std::cout,name()+": ") << endl;
	}
    }
  

  streamlog_out(DEBUG)<<"\n Will try to get collection "<<_TDCHitColName<<" of type "<<LCIO::LCINTVEC<<endl;

  const LCCollection* col = getCollection(evt, _TDCHitColName, LCIO::LCINTVEC, false);
  if (col == NULL)
    {
      streamlog_out(DEBUG) << "TBTrackProducer::ProcessEvent() Track Collection not in event." << endl;
    }
  else
    {
      std::vector<int> vHits[2][4];
      
      if (col->getNumberOfElements() < 1)
	streamlog_out(DEBUG)<<"\n WARNING collection "<<_TDCHitColName<<" is empty, no tracks will be found"<<endl<<endl;
      
      for(int i(0); i < col->getNumberOfElements(); i++) 
	{
	  const LCIntVec *q = dynamic_cast<const LCIntVec*>(col->getElementAt(i));
	  
	  for(unsigned j(0); j < q->size();j++) 
	    {
	      streamlog_out(DEBUG) << "LCIntVec " << i << "[" << j << "] = " << (*q)[j] << endl;      
	    }
	  
	  const LCIntVec &v(*q);
	  streamlog_out(DEBUG) << "Element " << i << " has " << v.size()-1 << " hits " << endl;
	  
	  if(v[0] < 8) 
	    {
	      unsigned xy(v[0]&1);
	      unsigned layer(v[0]>>1);
	      
	      for(unsigned j(1); j < v.size(); j++) 
		{
		  streamlog_out(DEBUG) << "X/Y " << xy << ", layer " << layer
			       << ", hit " << j-1 << " = " << v[j] << endl;
		  
		  vHits[xy][layer].push_back(v[j]);
		}
	    }
	}/*end loop over i*/
      
      std::vector<TrackProjection> vTrks[2][2][2];
      for(unsigned fb(0); fb < 2; fb++) 
	{
	  for(unsigned eh(0); eh < 2; eh++) 
	    {
	      for(unsigned xy(0); xy < 2; xy++) 
		{
		  vTrks[fb][eh][xy] = (_finder.find(fb, eh, xy, 
						    vHits[xy][0], vHits[xy][1], vHits[xy][2], vHits[xy][3]));
		  
		  streamlog_out(DEBUG) << "F/B " << fb << ", E/H " << eh << ", X/Y " << xy << " found "
			       << vTrks[fb][eh][xy].size() << " tracks" << endl;
		  if(printLevel(3)) 
		    {
		      for(unsigned i(0); i < vTrks[fb][eh][xy].size(); i++) 
			{
			  vTrks[fb][eh][xy][i].print(std::cout,"TBTrackProducer: ");
			}/*end loop over i*/
		    }
		  
		}
	    }
	}/*end loop over fb*/
      
      /* Make LCTrackPayload<TBTrackProjection> objects and put into output collection(s)*/
      
      /*create output collections (one X, one Y) if input coll exist.*/
      
      LCCollectionVec* outdcCol;
      
      for(unsigned fb(0); fb < 2; fb++) 
	{
	  for(unsigned eh(0); eh < 2; eh++) 
	    {
	      for(unsigned xy(0); xy < 2; xy++) 
		{
		  outdcCol = new LCCollectionVec( LCIO::LCGENERICOBJECT );
		  
		  if(fb==0 && eh==0 && xy==0) evt->addCollection(outdcCol, "TBTrackFEX");
		  if(fb==0 && eh==0 && xy==1) evt->addCollection(outdcCol, "TBTrackFEY");
		  if(fb==0 && eh==1 && xy==0) evt->addCollection(outdcCol, "TBTrackFHX");
		  if(fb==0 && eh==1 && xy==1) evt->addCollection(outdcCol, "TBTrackFHY");
		  if(fb==1 && eh==0 && xy==0) evt->addCollection(outdcCol, "TBTrackBEX");
		  if(fb==1 && eh==0 && xy==1) evt->addCollection(outdcCol, "TBTrackBEY");
		  if(fb==1 && eh==1 && xy==0) evt->addCollection(outdcCol, "TBTrackBHX");
		  if(fb==1 && eh==1 && xy==1) evt->addCollection(outdcCol, "TBTrackBHY");
		  
		  for(unsigned i(0); i < vTrks[fb][eh][xy].size(); i++) 
		    {
		      //LCPayload<TrackProjection> *p=new LCPayload<TrackProjection>(vTrks[fb][eh][xy][i]);
		      //outdcCol->addElement(dynamic_cast<LCGenericObject*>(p)) ;
		      outdcCol->addElement(vTrks[fb][eh][xy][i].get()) ;
		    }
		}
	    }
	}
    }//col in event

}//process event

