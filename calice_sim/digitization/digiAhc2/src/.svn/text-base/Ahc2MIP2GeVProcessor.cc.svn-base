#include "Ahc2MIP2GeVProcessor.hh"

#include <iostream>
#include <cstdlib>

#define MAX_K 40

// ---- LCIO Headers
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/MCParticle.h>
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include <IMPL/LCFlagImpl.h>
#include "UTIL/LCTypedVector.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include "UTIL/BitField64.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
//#include <AIDA/IHistogram1D.h>
#endif // MARLIN_USE_AIDA

//ROOT
#include <TH1.h>
#include <TString.h>
#include <TFile.h>

using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace CALICE
{

  Ahc2MIP2GeVProcessor aAhc2MIP2GeVProcessor;

  Ahc2MIP2GeVProcessor::Ahc2MIP2GeVProcessor() : Processor("Ahc2MIP2GeVProcessor")
  {

    // register steering parameters: name, description, class-variable, default value

    registerInputCollection(LCIO::SIMCALORIMETERHIT,
      "Input_Collection",
      "Name of SimCalorimeterHit input collections",
      _calorimInpCollection,
      std::string("hcalSD") );

      registerOutputCollection( LCIO::SIMCALORIMETERHIT,
        "Output_Collection" ,
        "Name of the Sim Calorimeter Hit output collection converted to MIP"  ,
        _calorimOutCollection,
        std::string("hcalSD_MIP") ) ;

        registerProcessorParameter("MIP2GeV",
        "MIP value in GeV",
        _MIPvalue,
        float(0.00047714));

        registerProcessorParameter("WriteRootfile",
        "Flag to write rootfile",
        _debug,
        bool(false));

        registerProcessorParameter("RootfileName",
        "Name of output rootfile",
        _rootfilename,
        std::string("MIP2GeV.root"));

        StringVec KExample;
        KExample.push_back("SimKCoordinate");
        KExample.push_back("KCoordinate");

        registerOptionalParameter( "KHandler" ,
        "Map for changing K from Simulation to K in database (example in Sim K = 31 -> K = 14)"  ,
        _KVector ,
        KExample ,
        KExample.size()) ;

      }

      /************************************************************************************/

      void Ahc2MIP2GeVProcessor::init()
      {
        streamlog_out(DEBUG) << "init called" << std::endl ;

        if(_debug)
        {
          rootfile = new TFile(_rootfilename.c_str(), "RECREATE");
          for(int K = 0; K < MAX_K; K++)
          {
            m_hSpectra[K] = new TH1F(TString::Format("hSpectra_Layer%i", K), TString::Format("hSpectra_Layer%i", K), 1500, 0, 1500);
            m_hSpectra[K]->GetXaxis()->SetTitle("Deposited Energy [keV]");
            m_hSpectra[K]->GetYaxis()->SetTitle("Normalized");
          }
        }

        //K Map
        _mapKs.clear();

        //Process Ks and put then into a map
        if( parameterSet( "KHandler" ) )
        {
          unsigned index = 0 ;
          while( index < _KVector.size() )
          {

            string strSimK( _KVector[ index++ ] );
            string strK( _KVector[ index++ ] );

            int SimK = atoi(strSimK.c_str());
            int K = atoi(strK.c_str());

            if(_mapKs.count(SimK) == 0)
            {
              //Create Table of SimK and K
              _mapKs.insert(make_pair(SimK,K));
            }
          }

          streamlog_out(MESSAGE) << "Number of Layers with changing K: " << _mapKs.size() << std::endl;

        }

        printParameters();

        _nRun = 0 ;
        _nEvt = 0 ;
      }

      /************************************************************************************/
      void Ahc2MIP2GeVProcessor::processRunHeader( LCRunHeader* run)
      {
        _nRun++ ;
      }

      void Ahc2MIP2GeVProcessor::processEvent( LCEvent * evt )
      {

        LCCollectionVec* calOutVec = new LCCollectionVec( LCIO::SIMCALORIMETERHIT) ;

        LCFlagImpl hitFlag(calOutVec->getFlag());
        hitFlag.setBit(LCIO::RCHBIT_TIME);
        hitFlag.setBit(LCIO::CHBIT_STEP);
        hitFlag.setBit(LCIO::CHBIT_LONG);
        calOutVec->setFlag(hitFlag.getFlag());

        int evtNumber = evt->getEventNumber();
        if ((evtNumber % 1000) == 0)
        streamlog_out(DEBUG) << " \n ---------> Event: "<<evtNumber<<"!!! <-------------\n" << std::endl;

        bool calorimCollectionFound = false;

        try {
          //check if the input collection exists
          std::vector< std::string >::const_iterator iter;
          const std::vector< std::string >* colNames = evt->getCollectionNames();

          for( iter = colNames->begin() ; iter != colNames->end() ; iter++) {
            if ( *iter == _calorimInpCollection ) calorimCollectionFound = true;
          }
        }
        catch(DataNotAvailableException &e){
          std::cout <<  "WARNING: List of collection names not available in event "<< evt->getEventNumber() << std::endl;
          return;
        };

        if (calorimCollectionFound)
        {
          LCCollection *inputCalorimCollection = evt->getCollection(_calorimInpCollection);
          int noHits = inputCalorimCollection->getNumberOfElements();
          _encoding = inputCalorimCollection->getParameters().getStringVal("CellIDEncoding");

          CellIDDecoder<SimCalorimeterHit> decoder(inputCalorimCollection);
          CellIDEncoder<SimCalorimeterHitImpl> encoder(_encoding.c_str(), calOutVec);

          bool hasKminus1 = false;
          if (_encoding.find("K-1") != std::string::npos)
          {
            hasKminus1 = true;
          }

          for (int i = 0; i < noHits; i++)
          {
            SimCalorimeterHit *aSimCalorimHit = dynamic_cast<SimCalorimeterHit*>(inputCalorimCollection->getElementAt(i));
            //New SimHit converted to MIP
            SimCalorimeterHitImpl *aSimCalorimHit_MIP =  new SimCalorimeterHitImpl();

            int noSubHits = aSimCalorimHit->getNMCContributions();
            for(int j = 0; j < noSubHits; j++)
            {
              EVENT::MCParticle *mcp_subhit = (EVENT::MCParticle*)aSimCalorimHit->getParticleCont(j);
              float *step_subhit = (float*)aSimCalorimHit->getStepPosition(j);
              int pdg_subhit = (int)aSimCalorimHit->getPDGCont(j);
              float time_subhit = (float)aSimCalorimHit->getTimeCont(j);
              float edep_subhit = (float)aSimCalorimHit->getEnergyCont(j);

              //Adding contribution to new hit
              aSimCalorimHit_MIP->addMCParticleContribution(mcp_subhit, edep_subhit/_MIPvalue, time_subhit, pdg_subhit, step_subhit);
            }

            /* Correct K in Sim for matching data */
            int I =  decoder(aSimCalorimHit)["I"];
            int J = decoder(aSimCalorimHit)["J"];
            int K = 0;

            if (hasKminus1) K = decoder(aSimCalorimHit)["K-1"] + 1;
            else K = decoder(aSimCalorimHit)["K"];

            //Correct K Simulation from map of Ks
            map<int, int>::iterator found = _mapKs.find(K);
            if(found != _mapKs.end())
            {
              K = found->second;
              streamlog_out(DEBUG) << "Changing Simulation K to " << K << std::endl;
            }

            //Check Comversion plot
            if(_debug)
            {
              m_hSpectra[K]->Fill(aSimCalorimHit->getEnergy()*1000000);
            }

            //Setting new CellID0
            encoder["I"] = I;
            encoder["J"] = J;
            if(hasKminus1) encoder["K-1"] = K-1;
            else encoder["K"] = K;

            encoder.setCellID(aSimCalorimHit_MIP);

            //Setting new Hit
            //aSimCalorimHit_MIP->setCellID0(aSimCalorimHit->getCellID0());
            aSimCalorimHit_MIP->setEnergy(aSimCalorimHit->getEnergy()/_MIPvalue);
            aSimCalorimHit_MIP->setPosition(aSimCalorimHit->getPosition());

            calOutVec->addElement(aSimCalorimHit_MIP);

          }//end loop over SimCalorimeterHits

        }//end if collection found

        LCParameters &theParam = calOutVec->parameters();
        theParam.setValue(LCIO::CellIDEncoding, _encoding);

        if(calOutVec->getNumberOfElements() > 0)
        evt->addCollection( calOutVec, _calorimOutCollection ) ;

        //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

        streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
        << "   in run:  " << evt->getRunNumber() << std::endl ;

        _nEvt ++ ;
      }

      /************************************************************************************/

      void Ahc2MIP2GeVProcessor::check( LCEvent * evt ) {
        // nothing to check here - could be used to fill checkplots in reconstruction processor
      }

      /************************************************************************************/
      void Ahc2MIP2GeVProcessor::end(){

        if(_debug)
        {
          rootfile->cd();
          for(std::map<int, TH1F*>::iterator it = m_hSpectra.begin(); it != m_hSpectra.end(); ++it)
          {
            it->second->Scale(1./it->second->Integral("width"));
            if(it->second->GetEntries() > 0)
            it->second->Write();
          }
          rootfile->Close();
        }

        std::cout << "MIP2GeVProcessor::end()  " << name()
        << " processed " << _nEvt << " events in " << _nRun << " runs "
        << std::endl ;

      }


      /************************************************************************************/
      void Ahc2MIP2GeVProcessor::printParameters(){
        std::cerr << "============= MIP2GeV Processor =================" <<std::endl;
        std::cerr << "Converting Simulation Hits from GeV to MIP" <<std::endl;
        std::cerr << "Input Collection name : " << _calorimInpCollection << std::endl;
        std::cerr << "Output Collection name : " << _calorimOutCollection << std::endl;
        std::cerr << "MIP: " << _MIPvalue << " GeV" <<std::endl;
        std::cerr << "Write rootfile : " << _debug << std::endl;
        std::cerr << "Number of Layers with changing K: " << _mapKs.size() << std::endl;
        std::cerr << "=======================================================" <<std::endl;
        return;

      }
    }
