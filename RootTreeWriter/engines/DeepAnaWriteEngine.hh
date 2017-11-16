#ifndef __DEEPANAWRITEENGINE_HH__
#define __DEEPANAWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"


#include <string>

#include "TTree.h"

namespace marlin
{

  class DeepAnaWriteEngine: public RootWriteEngine
  {
    public:

      DeepAnaWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
						 _engineName("DeepAnaWriteEngine")
      {}
    

      virtual const std::string& getEngineName()
      { return _engineName; }

      virtual void registerParameters();

      virtual void registerBranches( TTree* hostTree );

      virtual void fillVariables( EVENT::LCEvent* );


      //!! place the member variables to hold the ROOT TTree branches here !!;
      unsigned int   _elmNoClusters;//_hE_n_cl;
      unsigned int   _elmNoHits;    //_hE_n_hit; // EM-like 
      double         _elmEngySum;           //_hE_e_cl;
      unsigned int   _trkNoClusters;         //_hT_n_cl;
      unsigned int   _trkNoHits;             //_hT_n_hit; // TRK-like
      double         _trkEngySum;            //_hT_e_cl;
      unsigned int   _hadNoClusters; //_hH_n_cl;
      unsigned int   _hadNoHits;     //_hH_n_hit; // HAD-like
      double         _hadEngySum;    //_hH_e_cl;
      unsigned int   _neutrNoHits;
      double         _neutrEngySum;   //_hN_e_cl

      double _elmX; //x position of elm hit
      double _elmY;
      double _elmZ;

      double _trkX;
      double _trkY;
      double _trkZ;
      
      double _hadX;
      double _hadY;
      double _hadZ;

      double _X;
      double _Y;
      double _Z;
 
    private:
      std::string _engineName;
      std::string _enginePrefix;
      // processor parameters
      std::string _deepanaInColName;
      std::string _deepanaInNeutrColName;

    
  };

}//namespace marlin

#endif // __DEEPANAWRITEENGINE_HH__
