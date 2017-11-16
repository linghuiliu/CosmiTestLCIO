#ifndef __DRIFTCHAMBERWRITEENGINE_HH__
#define __DRIFTCHAMBERWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"


#include <string>

#include "TTree.h"

namespace marlin
{

  ///\ingroup calice_engines
  class DriftChamberWriteEngine: public RootWriteEngine
  {
  public:

    DriftChamberWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
					    _engineName("DriftChamberWriteEngine")
    {}
    

    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );

    virtual void fillVariables( EVENT::LCEvent* );

    //This program is using Michele Faucci Giannelli <giannell@PP.RHUL.AC.UK>
    //processor to create tracks from DC info. See:
    //http://www.pp.rhul.ac.uk/~calice/giannell/TGZ/calice/DriftChambertoTrack/v1.3/

    // tree variables
    const static unsigned int MAXHITS = 7608;
    struct
    {
      float EcalXYZ[3];
      float HcalXYZ[3];
      float Phi;     //Phi is the angle on the x-z plane 
      float Lambda;  //Lambda is the angle on the y-z plane
      float Chi2;    // chi2 = chi2x + chi2y


    } _hitsFill;
    
        

  private:
    std::string _engineName;
    std::string _enginePrefix;
    // processor parameters
    std::string _driftChamberColName;
    std::string _model;

  };

}//namespace marlin

#endif // __DRIFTCHAMBERWRITEENGINE_HH__
