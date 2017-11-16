#include "Ahc2ROCThresholdProcessor.hh"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/MCParticle.h>
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include <IMPL/LCFlagImpl.h>
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"

// -- CALICE Header
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "Ahc2Calibrations.hh"
#include "Ahc2CalibrationsProcessor.hh"
#include "CellIterator.hh"
#include "Ahc2CalibrationStatusBits.hh"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"

// ----- ROOT --------
#include "TMath.h"
#include "TROOT.h"

using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace CALICE
{

  Ahc2ROCThresholdProcessor aAhc2ROCThresholdProcessor ;


  Ahc2ROCThresholdProcessor::Ahc2ROCThresholdProcessor() : Processor("Ahc2ROCThresholdProcessor") {

    // register steering parameters: name, description, class-variable, default value

    registerInputCollection(LCIO::SIMCALORIMETERHIT,
      "ROCThresholdProcessor_simHitInputCollection",
      "Name of SimCalorimeterHit input collections",
      _calorimInpCollection,
      std::string("hcalSD") );


      registerOutputCollection( LCIO::CALORIMETERHIT,
        "ROCThresholdProcessor_calHitOutputCollection" ,
        "Name of the Calorimeter Hit output collection"  ,
        _calorimOutCollection,
        std::string("ahcal_ROCThr") ) ;

        registerProcessorParameter("ROCThresholdProcessor_tileEdgeX",
        "tile edge dimension in mm",
        _tileEdgeX,
        float(30.0));

        registerProcessorParameter("ROCThresholdProcessor_tileEdgeY",
        "tile edge dimension in mm",
        _tileEdgeY,
        float(30.0));

        registerProcessorParameter("ROCThresholdProcessor_deadSpace",
        "dead space between tiles in mm",
        _deadSpace,
        float(0.1));

        registerProcessorParameter("ROCThresholdProcessor_Threshold",
        "MIP threshold fraction",
        _MIPThr,
        float(0.5));

        registerProcessorParameter("ROCThresholdProcessor_tfast",
        "fast shaper time in ns",
        _tfast,
        float(15));

        registerProcessorParameter("ROCThresholdProcessor_tslow",
        "slow shaper time in ns",
        _tslow,
        float(50));

        registerProcessorParameter("NLayer",
        "Number of Layers in the detector",
        _NLayer,
        int(10));

        registerProcessorParameter("LayerPattern",
        "Layer arrangement of the detector (1 EBU Horizontal, 2 EBU Vertical, 3 SSF HBU, 4 2*2HBU)",
        _LayerPattern,
        std::string("1212121234"));

      }

      /************************************************************************************/

      void Ahc2ROCThresholdProcessor::init() {

        streamlog_out(DEBUG) << "   init called  " << std::endl ;
        energyThreshold = _MIPThr;
        halfdS = 0.5*_deadSpace;

        _nRun = 0 ;
        _nEvt = 0 ;

        //Pattern Layer
        if ( (int)_LayerPattern.length() != 0 )
        {
          if ( isValidLayerPattern(_LayerPattern) )
          {
            _LayerPatternVector = getLayerPatternVector(_LayerPattern);
          }
          else
          {
            streamlog_out(ERROR) << "'LayerPattern' parameter is not valid in steering file. ABORTING Marlin" << endl;
            exit(1);
          }
        }
        else
        {
          streamlog_out(ERROR) << "'LayerPattern' parameter is not defined in steering file. ABORTING Marlin" << endl;
          exit(1);
        }

        printParameters() ;

        //fOut = new TFile("CheckTimeROC.root", "RECREATE");
        //hFillTimeROC = new TH1F("hFillTimeROC", "hFillTimeROC", 400, -200, 200);
      }

      /************************************************************************************/
      void Ahc2ROCThresholdProcessor::processRunHeader( LCRunHeader* run) {

        _nRun++ ;
      }

      /************************************************************************************/

      void Ahc2ROCThresholdProcessor::processEvent( LCEvent * evt ) {

        LCCollectionVec* calOutVec = new LCCollectionVec( LCIO::CALORIMETERHIT) ;
        LCFlagImpl hitFlag(calOutVec->getFlag());
        hitFlag.setBit(LCIO::RCHBIT_TIME);
        hitFlag.setBit(LCIO::CHBIT_LONG);
        calOutVec->setFlag(hitFlag.getFlag());

        int evtNumber = evt->getEventNumber();

        if(evtNumber%1000 == 0)
        streamlog_out(MESSAGE) << " \n ---------> Event: " << evtNumber <<"!!! <-------------\n"<< endl;

        bool calorimCollectionFound = false;

        try {//check if the input collection exists
          std::vector< std::string >::const_iterator iter;
          const std::vector< std::string >* colNames = evt->getCollectionNames();

          for( iter = colNames->begin() ; iter != colNames->end() ; iter++) {
            if ( *iter == _calorimInpCollection ) calorimCollectionFound = true;
          }
        }
        catch(DataNotAvailableException &e){
          streamlog_out(WARNING) <<  "WARNING: List of collection names not available in event "<< evt->getEventNumber() << endl;
          return;
        };

        streamlog_out(DEBUG) << "Collection " << _calorimInpCollection << " found : " << calorimCollectionFound << endl;

        if (calorimCollectionFound){
          LCCollection *inputCalorimCollection = evt->getCollection(_calorimInpCollection);
          int noHits = inputCalorimCollection->getNumberOfElements();

          streamlog_out(DEBUG) << "Opened collection " << _calorimInpCollection << " contains " << noHits << " hits" << endl;
          _encoding = inputCalorimCollection->getParameters().getStringVal("CellIDEncoding");

          CellIDDecoder<SimCalorimeterHit> decoder(inputCalorimCollection);
          CellIDEncoder<CalorimeterHitImpl> encoder(_encoding.c_str(), calOutVec);

          bool hasKminus1 = false;
          if (_encoding.find("K-1") != std::string::npos)
          {
            hasKminus1 = true;
          }

          for (int i = 0; i < noHits; i++){
            SimCalorimeterHit *aSimCalorimHit = dynamic_cast<SimCalorimeterHit*>(inputCalorimCollection->getElementAt(i));

            int noSubHits = aSimCalorimHit->getNMCContributions();

            int layer = 0;
            if (hasKminus1) layer = decoder(aSimCalorimHit)["K-1"] + 1;
            else layer = decoder(aSimCalorimHit)["K"];

            int I = decoder(aSimCalorimHit)["I"];
            int J = decoder(aSimCalorimHit)["J"];

            //Check Type of EBU/HBU based on Layer Pattern!!!
            int type = _LayerPatternVector.at(layer-1);

            streamlog_out(DEBUG) << "layer " << layer << " type " << type << endl;

            /* Reject Hits outside box due to Mokka bug or TBSD driver wrong implementation?? */

            if(type == 1)
            {
              if((I > 4 || I < 1) || (J > 36 || J < 1))
              {
                streamlog_out(WARNING) << "Event " << evt->getEventNumber() <<  " Hit outside sensitive box --- Mokka bug??? -- reject hit" << endl;
                continue;
              }
            }
            else if(type == 2)
            {
              if((J > 4 || J < 1) || (I > 36 || I < 1))
              {
                streamlog_out(WARNING) << "Event " << evt->getEventNumber() << " Hit outside sensitive box --- Mokka bug??? -- reject hit" << endl;
                continue;
              }
            }
            else if(type == 3)
            {
              if((I > 18 || I < 7) || (J > 18 || J < 7))
              {
                streamlog_out(WARNING) << "Event " << evt->getEventNumber() << " Hit outside sensitive box --- Mokka bug??? -- reject hit" << endl;
                continue;
              }
            }
            else if(type == 4)
            {
              if((I > 24 || I < 1) || (J > 24 || J < 1))
              {
                streamlog_out(WARNING) << "Event " << evt->getEventNumber() << " Hit outside sensitive box --- Mokka bug??? -- reject hit" << endl;
                continue;
              }
            }

            /* ---------------------------------- */

            vector<float> subhitsTime;
            vector<float> subhitsEnergy;
            subhitsTime.resize(noSubHits);
            subhitsEnergy.resize(noSubHits);

            for(int j = 0; j < noSubHits; j++){//fill hit time and deposited energy per sub-hit

              const float *_hitStep = aSimCalorimHit->getStepPosition(j);
              Float_t _hitOntileX;
              Float_t _hitOntileY;

              if(type == 1)//EBU Horizontal
              {
                _hitOntileX =(Float_t)_hitStep[0]-(Float_t) std::floor(_hitStep[0]/_tileEdgeY)*_tileEdgeY;
                _hitOntileY =(Float_t)_hitStep[1]-(Float_t) std::floor(_hitStep[1]/_tileEdgeX)*_tileEdgeX;

                float _edepstep = (float)aSimCalorimHit->getEnergyCont(j);

                if(_hitOntileX < halfdS || _hitOntileX > (_tileEdgeY-halfdS) || _hitOntileY < halfdS || _hitOntileY > (_tileEdgeX-halfdS)) _edepstep = 0.0;

                subhitsTime[j] = (float)aSimCalorimHit->getTimeCont(j);
                subhitsEnergy[j] = _edepstep;

                streamlog_out(DEBUG) << "Layer " << layer << " type " << type << " edep " << _edepstep << endl;
              }
              else if(type == 2)//EBU Transverse
              {
                _hitOntileX =(Float_t)_hitStep[0]-(Float_t) std::floor(_hitStep[0]/_tileEdgeX)*_tileEdgeX;
                _hitOntileY =(Float_t)_hitStep[1]-(Float_t) std::floor(_hitStep[1]/_tileEdgeY)*_tileEdgeY;

                float _edepstep = (float)aSimCalorimHit->getEnergyCont(j);

                if(_hitOntileX < halfdS || _hitOntileX > (_tileEdgeY-halfdS) || _hitOntileY < halfdS || _hitOntileY > (_tileEdgeX-halfdS)) _edepstep = 0.0;

                subhitsTime[j] = (float)aSimCalorimHit->getTimeCont(j);
                subhitsEnergy[j] = _edepstep;

                streamlog_out(DEBUG) << "Layer " << layer << " type " << type << " edep " << _edepstep << endl;
              }
              else if(type == 3 || type == 4)//HBU and 2*2 HBU
              {
                _hitOntileX =(Float_t)_hitStep[0]-(Float_t) std::floor(_hitStep[0]/_tileEdgeX)*_tileEdgeX;
                _hitOntileY =(Float_t)_hitStep[1]-(Float_t) std::floor(_hitStep[1]/_tileEdgeY)*_tileEdgeY;


                float _edepstep = (float)aSimCalorimHit->getEnergyCont(j);

                if(_hitOntileX < halfdS || _hitOntileX > (_tileEdgeX-halfdS) || _hitOntileY < halfdS || _hitOntileY > (_tileEdgeY-halfdS)) _edepstep = 0.0;

                subhitsTime[j] = (float)aSimCalorimHit->getTimeCont(j);
                subhitsEnergy[j] = _edepstep;
              }

            }//end loop on subhits

            vector<int> lookup;
            lookup.resize(noSubHits);

            TMath::Sort(noSubHits,&subhitsTime[0],&lookup[0],kFALSE);//creating look-up table for time ordered subhits

            float energy(0.);//energy sum
            int ThrIndex(0);//index of the first hit in the sliding window
            float tH(0.);//time of passing threshold

            float Epar(0.);
            bool passThr(false);

            /*checking for the time of signal passing threshold:
            amplitude are added only until they are in a 50 ns window*/


            for(int k=0; k<noSubHits; k++){//loop on all subhits
              ThrIndex = k;
              Epar = 0.;
              for(int isub=k; isub< noSubHits; isub++){//loop on the remaining hits

                if((subhitsTime[lookup[isub]]-subhitsTime[lookup[ThrIndex]]) <_tfast){//check if the following hit is within tfast
                  Epar += subhitsEnergy[lookup[isub]];
                }else{break;}//endif //if time is > tfast exit loop on remaining hits

                if(Epar>energyThreshold){//check of energy sum vs threshold
                  tH = subhitsTime[lookup[isub]];//ste time hit = last hit time
                  passThr = true;
                  break;//exit loop on remaining hits
                } //endif

              }//end loop on remaining hits
              if(passThr==true) break; //if threshold had passed exit loop on all subhits
            }//end loop


            /*checking if the subhits happen within the risetime of the slow shaper*/
            if(passThr==true){
              for(int j=ThrIndex; j<noSubHits; j++){
                if(subhitsTime[lookup[j]] < (subhitsTime[lookup[ThrIndex]]+_tslow)){
                  energy += subhitsEnergy[lookup[j]];
                }
              }

              CalorimeterHitImpl * aCalorimHit =  new CalorimeterHitImpl();

              const float *hitpos = aSimCalorimHit->getPosition();

              aCalorimHit->setTime(tH);
              aCalorimHit->setEnergy(energy);
              aCalorimHit->setPosition(hitpos);
              aCalorimHit->setCellID0(aSimCalorimHit->getCellID0());

              calOutVec->addElement(aCalorimHit);

              //hFillTimeROC->Fill(tH);

              streamlog_out(DEBUG) << "Added hit to new collection " << _calorimOutCollection << endl;

            }
          }//end loop over SimCalorimeterHits

          //=======================================================================

        }//end if collection found

        LCParameters &theParam = calOutVec->parameters();
        theParam.setValue(LCIO::CellIDEncoding, _encoding);

        evt->addCollection( calOutVec, _calorimOutCollection ) ;

        //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

        streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
        << "   in run:  " << evt->getRunNumber() << std::endl ;

        _nEvt ++ ;
      }

      /************************************************************************************/

      void Ahc2ROCThresholdProcessor::check( LCEvent * evt ) {
        // nothing to check here - could be used to fill checkplots in reconstruction processor
      }

      /************************************************************************************/
      void Ahc2ROCThresholdProcessor::end(){

        std::cout << "ROCThresholdProcessor::end()  " << name()
        << " processed " << _nEvt << " events in " << _nRun << " runs "
        << std::endl ;

        //fOut->cd();
        //hFillTimeROC->Write();
        //fOut->Close();

      }


      /************************************************************************************/
      void Ahc2ROCThresholdProcessor::printParameters(){
        std::cerr<<"============= ROC Threshold Processor ================="<<std::endl;
        std::cerr<<" Simulating Spiroc2 timestamping and energy measurement"<<std::endl;
        std::cerr<<" Tile EdgeX: "<<_tileEdgeX <<" mm"<< " Tile EdgeY: "<<_tileEdgeY <<" mm" << std::endl;
        std::cerr<<" Dead space bet. tiles: "<<_deadSpace<<" mm"<<std::endl;
        std::cerr<<" disctiminator: "<<_MIPThr<<" MIP"<<std::endl;
        std::cerr<<" fast shaper time: "<<_tfast<<" ns"<<std::endl;
        std::cerr<<" slow shaper time: "<<_tslow<<" ns"<<std::endl;
        std::cerr<<"======================================================="<<std::endl;
        return;

      }

      /*================================================================================*/

      bool Ahc2ROCThresholdProcessor::isValidLayerPattern(string LayerPattern)
      {

        if ( (int)LayerPattern.length() != _NLayer ){
          cerr << "WRONG NUMBER OF ENTRIES IN THE LAYER PATTERN" <<endl;
          return false;
        }

        bool valid = true;

        for(int i = 0; i < _NLayer; i++)
        {
          if ((LayerPattern.c_str()[i] != '1') && (LayerPattern.c_str()[i] != '2') && (LayerPattern.c_str()[i] != '3') && (LayerPattern.c_str()[i] != '4'))
          {
            cerr << "WRONG CHARACTER IN THE LAYER PATTERN" << endl;
            valid = false;
            break;
          }
        }

        streamlog_out(MESSAGE) << "Layer pattern is valid " << LayerPattern << endl;

        return valid;

      }

      /*================================================================================*/

      vector<int> Ahc2ROCThresholdProcessor::getLayerPatternVector(string LayerPattern)
      {
        vector<int> patternVector;

        for(int i = 0; i < _NLayer; i++)
        {
          stringstream ss;
          string str;

          ss << LayerPattern.at(i);
          ss>> str;

          int digit = atoi(str.c_str());
          patternVector.push_back(digit);
        }

        return patternVector;
      }
    }
