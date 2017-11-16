#ifndef __ASIMCALORIMETERHITWRITEENGINE_HH__
#define __ASIMCALORIMETERHITWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "RootTreeWriter.hh"
#include "TTree.h"
#include "TH1F.h"
#include "marlin/Processor.h"

#include <vector>

using std::vector;


namespace marlin
{
  class ASimCalorimeterHitWriteEngine: public RootWriteEngine
  {
    //========================================================//
    //                                                        //
    //  Private members                                       //
    //                                                        //
    //========================================================//
  private:
    std::string _engineName;
    std::string _enginePrefix;
    //-----------------------
    // processor parameters
    std::string _prefix;
    std::string _calorimCollection;
      
    static const int _fMaxLayers = 38;  //maximum no of layers (should be enough for all calorimeters)

    int _nHitsPerEvent;
	  
    vector<int> *_IPtr;
    vector<int> *_JPtr;
    vector<int> *_KPtr;

    vector<float> *_xPtr;
    vector<float> *_yPtr;
    vector<float> *_zPtr;

    vector<float> *_hitEnergyPtr;
    vector<int> *_cellID0Ptr;
 	  		  
    double _xCog;
    double _yCog;
    double _zCog;

    double _energySumPerEvent; //energy sum per event
		
    //-----------------------------
    //Quantities per layer
    int    _nLayers;
    int    _nHitsPerLayer[_fMaxLayers];
    double _energySumPerLayer[_fMaxLayers];
		  
    double _xCogPerLayer[_fMaxLayers];
    double _yCogPerLayer[_fMaxLayers];
    double _zCogPerLayer[_fMaxLayers];


    TH1F *_histHcalLongProfile;

    //========================================================//
    //                                                        //
    //  Public  members                                       //
    //                                                        //
    //========================================================//
  public:
    ASimCalorimeterHitWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
							  _engineName("ASimCalorimeterHitWriteEngine")
    {
    }
    
    virtual const std::string& getEngineName()
    { return _engineName; }
    virtual void registerParameters();
    virtual void registerBranches( TTree* hostTree );
    virtual void fillVariables( EVENT::LCEvent* );

        

  
  };

}//namespace marlin

#endif

