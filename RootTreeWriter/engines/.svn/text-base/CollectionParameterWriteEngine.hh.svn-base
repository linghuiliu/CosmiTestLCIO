#ifndef __COLLECTIONPARAMETERWRITEENGINE_HH__
#define __COLLECTIONPARAMETERWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"


#include <string>

#include "TTree.h"

namespace marlin
{

  class CollectionParameterWriteEngine: public RootWriteEngine
  {
  public:

    CollectionParameterWriteEngine( RootTreeWriter* host, std::string engineName = "CollectionParameterWriteEngine" ):
      RootWriteEngine(host), _engineName( engineName ) {}


    virtual const std::string& getEngineName()
    { return _engineName; }

    //    virtual RootWriteEngine* newEngine( Processor* proc )
    //    {
    //      mParentProcessor = proc;
    //      return new CollectionParameterWriteEngine ;
    //    }


    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );

    virtual void fillVariables( EVENT::LCEvent* );


    // tree variables

    std::vector<int>      _intValues;
    std::vector<float>    _floatValues;
    std::vector<IntVec>   _intVecValues;
    std::vector<FloatVec> _floatVecValues;
    std::vector<int>      _nIntValues;
    std::vector<int>      _nFloatValues;


  private:
    std::string _engineName;
    std::string _enginePrefix;

    std::string _collectionparameterInColName;

    StringVec _floatParameters;
    StringVec _intParameters;
    StringVec _floatVecParameters;
    StringVec _intVecParameters;

    std::vector<TBranch*> _floatVecBranches;
    std::vector<TBranch*> _intVecBranches;
  };

}//namespace marlin

#endif // __COLLECTIONPARAMETERWRITEENGINE_HH__
