#include "MCParticleWriteEngine.hh"

#include "EVENT/LCCollection.h"

#include "IMPL/LCGenericObjectImpl.h"
#include "IMPL/MCParticleImpl.h"

#include <cfloat>
#include <climits>

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  void MCParticleWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter(
						   _engineName+"_prefix",
						   "MCParticleWriteEngine prefix to tree variables",
						   _enginePrefix,
						   std::string("mcp_") );

    _hostProcessor.relayRegisterProcessorParameter(
						   _engineName+"_inColNameMCParticle",
						   "name of MCParticle input collection",
						   _inColNameMCParticle,
						   std::string("MCParticle") );
  }


  void MCParticleWriteEngine::registerBranches( TTree* hostTree )
  {
    hostTree->Branch( (_enginePrefix+"startPos").c_str(),
		      & _startPos,
		      (_enginePrefix+"startPos[3]/D").c_str() );

    hostTree->Branch( (_enginePrefix+"endPos").c_str(),
		      & _endPos,
		      (_enginePrefix+"endPos[3]/D").c_str() );

    hostTree->Branch( (_enginePrefix+"PDG").c_str(),
		      & _PDG,
		      (_enginePrefix+"PDG/I").c_str() );
  }


  void MCParticleWriteEngine::fillVariables( LCEvent* evt )
  {
    LCCollection *inColMCParticle;

    try
      {
	inColMCParticle = evt->getCollection(_inColNameMCParticle);

	const int nElements = inColMCParticle->getNumberOfElements();
	if (nElements == 0) return;

	IMPL::MCParticleImpl* mcp = (IMPL::MCParticleImpl*)(inColMCParticle->getElementAt(0));

	const double *MCstart = mcp->getVertex();
	const double *MCend   = mcp->getEndpoint();

	for(int i=0; i!=3; ++i) {
	  _startPos[i] = MCstart[i];
	  _endPos[i] = MCend[i];
	}

	_PDG = mcp->getPDG();

      }
    catch ( DataNotAvailableException err )
      {
	std::cout <<  "WARNING: Collection " << _inColNameMCParticle << " not available in event " << evt->getEventNumber() << std::endl;
	for(int i=0; i!=3; ++i) {
	  _startPos[i] = INVALIDF;
	  _endPos[i] = INVALIDF;
	  _PDG=INVALIDI;
	}
      }
  }

} //namespace marlin
