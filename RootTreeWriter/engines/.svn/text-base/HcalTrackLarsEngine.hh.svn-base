#ifndef __HCALTRACKLARSENGINE_HH__
#define __HCALTRACKLARSENGINE_HH__

#include "RootWriteEngine.hh"
#include <vector>

namespace marlin
{
  class HcalTrackLarsEngine: public RootWriteEngine
  {
  public:
    
    HcalTrackLarsEngine( RootTreeWriter* host ):RootWriteEngine(host),
					   _engineName("HcalTrackLarsEngine")
    {}
    
    
    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );
    
    virtual void fillVariables( EVENT::LCEvent* );
  
    
  private:
    std::string _engineName;
 
   /* processor parameters*/
    std::string _ahcHitsColName;
    std::string _tcmHitsColName;
    std::string _trackColName;

  protected:
    double getSumOfCaloHits(EVENT::LCEvent* evt, std::string collectionName);
    
    /*tree variables*/
    struct EventTracks
    {
      int	NTracks;
      
      double    EmcEnergy;
      double    AhcEnergy;
      double    TcmtEnergy;

      std::vector< int >	t_NHits;
      std::vector< int >	t_NLength;
      std::vector< int >	t_StartLayer;
      std::vector< int >	t_StopLayer;
      std::vector< double >	t_CosPhi;
      
      /* The following vectors contain actually hit information for each track
	 I had to use a flat structure, as ROOT does not support a 2 dim (i.e. vector < vector < double > >)
	 without having to create a dictionary, which i want do avoid.
	 Hence: The t_h_xxx variables contain the hit information for all tracks stripped together,
	 and the t_trackHitIndex saves the position of the first hit of each track within these t_h_xxx vars */
      std::vector< int >	t_trackHitIndex;
      std::vector< double >	t_h_Energy;
      std::vector< int >	t_h_I;
      std::vector< int >	t_h_J;
      std::vector< int >	t_h_K;
      
    } _event;
    
    
  };

}/*namespace marlin*/

#endif /* __HCALTRACKLARSENGINE_HH__*/
