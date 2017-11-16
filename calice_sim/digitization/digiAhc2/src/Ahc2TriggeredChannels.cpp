#include "Ahc2TriggeredChannels.hh"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/MCParticle.h>
#include "IMPL/CalorimeterHitImpl.h"
#include <IMPL/LCFlagImpl.h>
#include "EVENT/CalorimeterHit.h"
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

  Ahc2TriggeredChannels aAhc2TriggeredChannels ;

  Ahc2TriggeredChannels::Ahc2TriggeredChannels() : Processor("Ahc2TriggeredChannels") {

    // register steering parameters: name, description, class-variable, default value

    registerInputCollection(LCIO::CALORIMETERHIT,
      "InputCollection",
      "Name of CalorimeterHit input collection",
      _calorimInpCollection,
      std::string("hcalSD") );

      registerOutputCollection( LCIO::CALORIMETERHIT,
        "OutputCollection" ,
        "Name of the Calorimeter Hit output collection"  ,
        _calorimOutCollection,
        std::string("ahcal") ) ;

        registerProcessorParameter( "MappingProcessorName" ,
        "Name of the MappingProcessor instance that provides"
        " the geometry of the detector." ,
        _mappingProcessorName,
        std::string("MyMappingProcessor") ) ;
      }

      /************************************************************************************/

      void Ahc2TriggeredChannels::init() {

        streamlog_out(DEBUG) << "   init called  " << std::endl ;

        bool error = false;
        std::stringstream message;

        _nRun = 0 ;
        _nEvt = 0 ;

        _mapper = dynamic_cast<const Ahc2Mapper*>(MappingProcessor::getMapper(_mappingProcessorName));
        if ( !_mapper )
        {
          message << "MappingProcessor::getMapper("<< _mappingProcessorName
          << ") did not return a valid mapper." << endl;
          error = true;
        }

        if (error)
        {
          streamlog_out(ERROR) << message.str();
          throw marlin::StopProcessingException(this);
        }

      }

      /************************************************************************************/
      void Ahc2TriggeredChannels::processRunHeader( LCRunHeader* run) {

        _nRun++ ;
      }

      /************************************************************************************/
      void Ahc2TriggeredChannels::processEvent( LCEvent * evt )
      {
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

          for( iter = colNames->begin() ; iter != colNames->end() ; iter++)
          if ( *iter == _calorimInpCollection ) calorimCollectionFound = true;
        }
        catch(DataNotAvailableException &e)
        {
          streamlog_out(WARNING) <<  "WARNING: List of collection names not available in event "<< evt->getEventNumber() << endl;
          return;
        };

        streamlog_out(DEBUG) << "Collection " << _calorimInpCollection << " found : " << calorimCollectionFound << endl;

        if (calorimCollectionFound)
        {
          LCCollection *inputCalorimCollection = evt->getCollection(_calorimInpCollection);
          int noHits = inputCalorimCollection->getNumberOfElements();

          streamlog_out(DEBUG) << "Opened collection " << _calorimInpCollection << " contains " << noHits << " hits" << endl;

          _encoding = inputCalorimCollection->getParameters().getStringVal("CellIDEncoding");
          _mapper->getDecoder()->setCellIDEncoding(_encoding);

          streamlog_out(DEBUG) << "Encoding used " << _encoding << endl;

          CellIDDecoder<CalorimeterHit> decoder(inputCalorimCollection);
          CellIDEncoder<CalorimeterHitImpl> encoder(_encoding.c_str(), calOutVec);

          bool hasKminus1 = false;
          if (_encoding.find("K-1") != std::string::npos)
          hasKminus1 = true;

          for (int i = 0; i < noHits; i++)
          {
            CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inputCalorimCollection->getElementAt(i));

            int layer = 0;
            if (hasKminus1) layer = decoder(hit)["K-1"] + 1;
            else layer = decoder(hit)["K"];

            int cellID = hit->getCellID0();
            float energy = hit->getEnergy();

            int Chip = _mapper->getChipFromCellID(cellID);

            int index = layer*100+Chip;

            if(energy > 0.5)
            m_ChipTrigger[index]++;

            CalorimeterHitImpl * aCalorimHit =  new CalorimeterHitImpl();

            aCalorimHit->setTime(hit->getTime());
            aCalorimHit->setEnergy(hit->getEnergy());
            aCalorimHit->setPosition(hit->getPosition());
            aCalorimHit->setCellID0(hit->getCellID0());

            calOutVec->addElement(aCalorimHit);

            streamlog_out(DEBUG) << "Added hit to new collection " << _calorimOutCollection << endl;
          }//end loop calo hits

          //=======================================================================
        }//end if collection found

        nHitsPerChipAboveThr.clear();

        for(map<int, int>::iterator it = m_ChipTrigger.begin(); it != m_ChipTrigger.end(); ++it)
        {
          //cout << "Ahc2TriggeredChannels, index: " << it->first << " nHits " << it->second << endl;
          nHitsPerChipAboveThr.push_back(it->first*100+it->second);//push (layer*100+Chip)*100+nTrigger
        }

        m_ChipTrigger.clear();

        LCParameters &theParam = calOutVec->parameters();
        theParam.setValue(LCIO::CellIDEncoding, _encoding);

        //Event parameter number of hits per Chip over threshold
        evt->parameters().setValues("nHitsPerChipAboveThr", nHitsPerChipAboveThr);
        streamlog_out(DEBUG0)<<"HitPerChips AboveThr 0.5 MIP : "<< nHitsPerChipAboveThr.size() << endl;

        evt->addCollection( calOutVec, _calorimOutCollection ) ;

        //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

        streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
        << "   in run:  " << evt->getRunNumber() << std::endl ;

        _nEvt ++ ;
      }

      /************************************************************************************/

      void Ahc2TriggeredChannels::check( LCEvent * evt ) {
        // nothing to check here - could be used to fill checkplots in reconstruction processor
      }

      /************************************************************************************/
      void Ahc2TriggeredChannels::end(){

        std::cout << "Ahc2TriggeredChannels::end()  " << name()
        << " processed " << _nEvt << " events in " << _nRun << " runs "
        << std::endl ;
      }

    }//end namespace
