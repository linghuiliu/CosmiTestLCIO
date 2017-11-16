#ifndef __HITWRITEENGINE_HH__
#define __HITWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"
#include "TMath.h"

#include <string>

#include "TTree.h"

namespace marlin
{
  class HitWriteEngine: public RootWriteEngine
  {
  public:
    
    HitWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
					   _engineName("HitWriteEngine")
    {}
    
    
    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );
    
    virtual void fillVariables( EVENT::LCEvent* );
    
    /* tree variables*/
    const static unsigned int MAXCELLS  = 7609; /*should be big enough for all detectors!*/
    const static unsigned int MAXLAYERS = 39;   /*should be big enough for all detectors!*/
    struct
    {
      int   iEvt;
      int   nHits;
      int   nLayers;         /**< number of layers < MAXLAYERS !*/
      float energySum;
      float energyDensity;      
      float cogX;            /**< center of gravity weighted by energy on whole calo*/
      float cogY;
      float cogZ;
      float cogI;            /**<center of gravity with cell index I (column)*/
      float cogJ;            /**<center of gravity with cell index J (row)*/
      float cogIGeom;        /**<geometrical (not weighted) center of gravity with cell index I (column)*/
      float cogJGeom;        /**<geometrical (not weighted) center of gravity with cell index J (row)*/
      float radius;          /**<shower radius (in x,y plane) calculated w.r.t. the cog */
      float radiusEw;        /**<shower radius (in x,y plane) calculated w.r.t. the cog energy weighted*/
      
      int   cellID[MAXCELLS];
      int   hitI[MAXCELLS];
      int   hitJ[MAXCELLS];
      int   hitK[MAXCELLS];
      float hitEnergy[MAXCELLS];
      float hitTime[MAXCELLS];
      int   hitType[MAXCELLS];
      float hitRadius[MAXCELLS];        /**< hit distance to the cog  (ideally one wants to replace 
					   the cog with the DC track)*/
      float hitEnergyDensity[MAXCELLS]; /**< hit energy weighted by tile_size^2*hitRadius (dimention: E/mm^3)*/
      float hitPos[MAXCELLS][3];
      int   cellSize[MAXCELLS];
      float cellTemperature[MAXCELLS];
      float hitAmplRawADC[MAXCELLS];
      float hitAmplRawMinusPedestalADC[MAXCELLS];
      
      
      int   lNHits[MAXLAYERS];        /**< layer by layer number of hits*/
      float lEnergy[MAXLAYERS];       /**< layer by layer Energy */
      float lEnergy_err[MAXLAYERS];   /**< layer by layer Energy error (not yet filled in CalorimeterHit class)*/
      
      float lCogX[MAXLAYERS];         /**< center of gravity weighted by energy in one layer*/
      float lCogY[MAXLAYERS];
      float lCogI[MAXLAYERS];         /**< center of gravity in I,J weighted by energy in one layer*/
      float lCogJ[MAXLAYERS];
      float lCogIGeom[MAXLAYERS];     /**< geometrical (not weighted) center of gravity in I,J */
      float lCogJGeom[MAXLAYERS];
      float lRadius[MAXLAYERS];       /**< shower radius (in x,y plane) in one layer calculated w.r.t. the cog*/
      float lRadiusEw[MAXLAYERS];     /**< energy weighted shower radius (in x,y plane) in 
					 one layer calculated w.r.t. the cog*/
      float energySum5Layer;          /**< energy Sum for the first 5 layer outside 28 cm*/
      int nHits5Layer;                /**< number of Hits for the first 5 layer outside 28 cm*/
      
      float cogX5Layer;      /**< center of gravity weighted by energy over the first 5 layers*/
      float cogY5Layer;
      float cogZ5Layer;
      
      
    } _hitsFill;
    
    
  private:
    void resetHitsFill();

  
    std::string _engineName;
    std::string _enginePrefix;

    /* processor parameters*/
    std::string _prefix;
    std::string _hitColName;
    std::string _ahcAmplRelColName;     
    bool _useAhcAmplCol;

    int _level;
    float _samplingFraction;
    int _eventFiltering;
    int _cogIJenable;
    float _mipCut;

    int   ievt;
    int   nHits;
    float eSum;    
    float cogx;     
    float cogy;
    float cogz;
    float radius;   
    float radiusEw;   
    int   nhits[MAXLAYERS];
    float energy[MAXLAYERS];
    float energy_err[MAXLAYERS];
    float lcogx[MAXLAYERS];
    float lcogy[MAXLAYERS];
    float lcogI[MAXLAYERS];
    float lcogJ[MAXLAYERS];
    float lcogIGeom[MAXLAYERS];
    float lcogJGeom[MAXLAYERS];
    float lradius[MAXLAYERS];
    float lradiusEw[MAXLAYERS];
    float lEnergyDensity[MAXLAYERS];
  
    int   tilesize;

    float ampl;
    float time;
    int   type;
    float ampl_GeV;
    float esum_5Layer;  
    int   numHits5Layer;
    float cogIJeSum;
    float cogI;
    float cogJ;
    int   nIJHits;
    float cogIGeom;
    float cogJGeom;

    float esum_all5Layer; 
    float cogx5;
    float cogy5;
    float cogz5;  


  };

}/*namespace marlin*/

#endif /* __HITWRITEENGINE_HH__*/
