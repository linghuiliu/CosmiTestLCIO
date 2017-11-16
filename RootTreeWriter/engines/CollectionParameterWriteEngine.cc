#include "CollectionParameterWriteEngine.hh"

#include <cfloat>

#include "EVENT/LCCollection.h"

using namespace std;

#define DDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{

  RTW_REGISTER_ENGINE( CollectionParameterWriteEngine );

  void CollectionParameterWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterInputCollection(LCIO::RECONSTRUCTEDPARTICLE,_engineName+"_InCol",
                                                "Name of input collection",
                                                _collectionparameterInColName, std::string("Unset")  );

    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_prefix",
                                                   "engine prefix for running multiple instances",
                                                   _enginePrefix, std::string("") );

    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_FloatParameters",
                                                   "names of float parameters",
                                                   _floatParameters, _floatParameters );

    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_IntParameters",
                                                   "names of int parameters",
                                                   _intParameters, _intParameters );

    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_FloatVecParameters",
                                                   "names of FloatVec parameters",
                                                   _floatVecParameters, _floatVecParameters );

    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_IntVecParameters",
                                                   "names of IntVec parameters",
                                                   _intVecParameters, _intVecParameters );

  }

  void CollectionParameterWriteEngine::registerBranches( TTree* hostTree )
  {
    _intValues.resize(_intParameters.size(),0);
    _floatValues.resize(_floatParameters.size(),0);
    _nIntValues.resize(_intVecParameters.size(),0);
    _nFloatValues.resize(_floatVecParameters.size(),0);
    _intVecValues.resize(_intVecParameters.size());
    _floatVecValues.resize(_floatVecParameters.size());


    for (unsigned int i = 0; i < _intParameters.size(); ++i)
      hostTree->Branch( (_enginePrefix+_intParameters[i]).c_str() ,&(_intValues[i]),(_enginePrefix+_intParameters[i]+"/I").c_str() );

    for (unsigned int i = 0; i < _floatParameters.size(); ++i)
      hostTree->Branch( (_enginePrefix+_floatParameters[i]).c_str() ,&(_floatValues[i]),(_enginePrefix+_floatParameters[i]+"/F").c_str() );


    for (unsigned int i = 0; i < _intVecParameters.size(); ++i) {

      std::string sizeName = _enginePrefix+"_N_"+_intVecParameters[i];

      hostTree->Branch( sizeName.c_str() ,&(_nIntValues[i]),( sizeName + "/i").c_str() );

      _intVecBranches.push_back(hostTree->Branch( (_enginePrefix+_intVecParameters[i]).c_str() ,&(_nIntValues[i]),(_enginePrefix+_intVecParameters[i]+"["+sizeName+"]/I").c_str() ) );
    }


    for (unsigned int i = 0; i < _floatVecParameters.size(); ++i) {

      std::string sizeName = _enginePrefix+"_N_"+_floatVecParameters[i];

      hostTree->Branch( sizeName.c_str() ,&(_nFloatValues[i]),( sizeName + "/i").c_str() );

      _floatVecBranches.push_back( hostTree->Branch( (_enginePrefix+_floatVecParameters[i]).c_str() ,&(_nFloatValues[i]),(_enginePrefix+_floatVecParameters[i]+"["+sizeName+"]/F").c_str() ) );
    }


  }


  void CollectionParameterWriteEngine::fillVariables( EVENT::LCEvent* evt ) {

    for (unsigned int i=0; i < _intVecValues.size(); ++i) _intVecValues[i].clear();
    for (unsigned int i=0; i < _floatVecValues.size(); ++i) _floatVecValues[i].clear();

    LCCollection* inCol;
    try {
      inCol = evt->getCollection( _collectionparameterInColName );

      for (unsigned int i=0; i < _intParameters.size(); ++i) _intValues[i] = inCol->getParameters().getIntVal(_intParameters[i]);
      for (unsigned int i=0; i < _floatParameters.size(); ++i) _floatValues[i] = inCol->getParameters().getFloatVal(_floatParameters[i]);

      for (unsigned int i=0; i < _intVecParameters.size(); ++i)  inCol->getParameters().getIntVals( _intVecParameters[i], _intVecValues[i] );
      for (unsigned int i=0; i < _floatVecParameters.size(); ++i)  inCol->getParameters().getFloatVals( _floatVecParameters[i], _floatVecValues[i] );

    }
    catch ( DataNotAvailableException err ) {
      streamlog_out(WARNING) <<  "WARNING: Collection "<< _collectionparameterInColName
                     << " not available in event "<< evt->getEventNumber() << endl;

    }


    for (unsigned int i=0; i < _intVecParameters.size(); ++i) {
      streamlog_out(DEBUG) << " integer vector " << _intVecParameters[i] << " has " << _intVecValues[i].size() << " elements" << endl;
      _nIntValues[i] = _intVecValues[i].size();
      _intVecBranches[i]->SetAddress( &(_intVecValues[i][0])  );
    }
    for (unsigned int i=0; i < _floatVecParameters.size(); ++i) {
      streamlog_out(DEBUG) << " float vector " << _floatVecParameters[i] << " has " << _floatVecValues[i].size() << " elements" << endl;
      _nFloatValues[i] = _floatVecValues[i].size();
      _floatVecBranches[i]->SetAddress( &(_floatVecValues[i][0])  );
    }


  }

}//namespace marlin
