#include "ShowerShapeEngine.hh"

#include "ClusterShapesTyped.hh"

using namespace lcio;
using namespace std;
using namespace CALICE;


namespace marlin
{
  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void ShowerShapeEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter("ShowerShapeEngine_inputCol",
						   "input collection of shower shapes",
						   _showerShapeColName,
						   std::string(""));

    _hostProcessor.relayRegisterProcessorParameter("ShowerShapeEngine_prefix",
                                                   "ShowerShapeEngine prefix to tree variables",
                                                   _prefix,
                                                   std::string("showerShapes"));

 
   }


  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
   void ShowerShapeEngine::registerBranches( TTree* hostTree )
  {
    if ( _prefix.size() > 0 )
      if ( _prefix[ _prefix.length()-1 ] != '_' )
	_prefix += "_";

    hostTree->Branch(string(_prefix+"nClusters").c_str(),    &_nClusters, 
                     string(_prefix+"nClusters/I").c_str());
  
    hostTree->Branch(string(_prefix+"cogX").c_str(), _cogX, 
		     string(_prefix+"cogX["+_prefix+"nClusters]/F").c_str());
 
    hostTree->Branch(string(_prefix+"cogY").c_str(), _cogY, 
		     string(_prefix+"cogY["+_prefix+"nClusters]/F").c_str());
   
    hostTree->Branch(string(_prefix+"cogZ").c_str(), _cogZ, 
		     string(_prefix+"cogZ["+_prefix+"nClusters]/F").c_str());

    hostTree->Branch(string(_prefix+"energy").c_str(), _energy, 
		     string(_prefix+"energy["+_prefix+"nClusters]/F").c_str());

    hostTree->Branch(string(_prefix+"nHits").c_str(), _nHits, 
		     string(_prefix+"nHits["+_prefix+"nClusters]/I").c_str());

 
  }

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void ShowerShapeEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    /*Initialize variables (otherwise variables may keep values from previous events)*/

    _nClusters = 0;
    for (unsigned int i = 0; i < MAX_CLUSTERS; ++i)
      {
	_cogX[i]   = 0;
	_cogY[i]   = 0;
	_cogZ[i]   = 0;
	_energy[i] = 0;
	_nHits[i] = 0;
      }
    

    /*-----------------------------------------------------------------------------*/
   std::vector<ClusterShapesTyped*> allClusters;

    StringVec v_colNames;
    evt->getParameters().getStringVals( _showerShapeColName, v_colNames );

    streamlog_out(DEBUG)<<"\n Start loop over "<<v_colNames.size()<<" collections named "<<_showerShapeColName<<std::endl;

    for ( unsigned i = 0; i < v_colNames.size(); i++ )
      {
	try 
	  {
	    LCCollection* incol_i = evt->getCollection( v_colNames[i] );

	    streamlog_out(DEBUG)<<"\n Collection "<<v_colNames[i]<<endl;

	    /*
	      for (int iHit = 0; iHit < incol_i->getNumberOfElements(); ++iHit)
	      {
	      CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(incol_i->getElementAt(i));
	      const float *position = hit->getPosition();
	      float energy   = hit->getEnergy();
	      
	      streamlog_out(DEBUG)<<" hit "<<iHit+1<<" x="<<position[0]<<", y="<<position[1]<<", z="<<position[2]
	      <<", energy="<<energy<<endl;
	      }
	    */

	    ClusterShapesTyped* shape_i = new ClusterShapesTyped;
	    shape_i->fill<CalorimeterHit>(incol_i);	    
	    
	    allClusters.push_back(shape_i);
	  
	  }
	catch ( DataNotAvailableException &err) 
	  {
	    streamlog_out(WARNING) << "collection " << v_colNames[i] << " not available" << std::endl;
	  }
      }/*end loop over input shower shapes collection*/
    
 
    unsigned int nClusters = allClusters.size();
    _nClusters = nClusters;
    
    for ( unsigned iCluster = 0; iCluster <  nClusters; iCluster++ ) 
      {
	float *pos_cog = (allClusters[iCluster])->getClusterShapesPointer()->getCentreOfGravity();
	float energy = (allClusters[iCluster])->getClusterShapesPointer()->getTotalAmplitude();
	int nHits = (allClusters[iCluster])->getClusterShapesPointer()->getNumberOfHits();

	_cogX[iCluster] = pos_cog[0];
	_cogY[iCluster] = pos_cog[1];
	_cogZ[iCluster] = pos_cog[2];

	_energy[iCluster] = energy;
	_nHits[iCluster]  = nHits;

	streamlog_out(DEBUG)<<"\n cluster "<<iCluster+1<<" pos_cog=("<<pos_cog[0]<<","<<pos_cog[1]<<","<<pos_cog[2]<<")"<<endl;
	streamlog_out(DEBUG)<<"  nhits ="<<nHits<<endl;
	streamlog_out(DEBUG)<<"  energy="<<energy<<endl;


      }/*end loop over clusters*/

  }/*fillVariables*/

}/*namespace marlin*/

