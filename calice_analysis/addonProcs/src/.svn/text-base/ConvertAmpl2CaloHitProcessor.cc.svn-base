#include "ConvertAmpl2CaloHitProcessor.hh"

#include "marlin/Exceptions.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTOOLS.h"
#include "collection_names.hh"
#include "TriggerBits.hh"

using std::endl;

namespace CALICE
{
  ConvertAmpl2CaloHitProcessor aConvertAmpl2CaloHitProcessor;

  ConvertAmpl2CaloHitProcessor::ConvertAmpl2CaloHitProcessor():Processor("ConvertAmpl2CaloHitProcessor")
  {
    _description = "Convert amplitude of CalorimeterHit collection to a different ampitude provided by Amplitude collection";

    registerProcessorParameter("AhcHitCollectionName",
                               "Name of the input AHCal hit collection, of type CalorimeterHit",
                               _ahcHitColName,
                               std::string("AhcCalorimeter_Hits"));

    registerProcessorParameter("AhcAmplCollectionName",
                               "Name of the input AHCal amplitude collection, of type LCGenericObject",
                               _ahcAmplColName,
                               std::string("AhcAmplitude"));

    registerProcessorParameter("HitAmplRelationName",
                               "Name of the input LCRelation between CalorimeterHit and AhcAmplitude",
                               _hitAmplRelationColName,
                               std::string("AhcHitAmplitudeRelation"));

    registerProcessorParameter("V2AmplitudeName",
                               "Name of the amplitude for the output AHCal hit version 2 collection (choose RawADC, RawMinusPedestalADC, TemperatureCorrMIP, NOTTemperatureCorrMIP, GeV )",
                               _amplName,
                               std::string("NOTTemperatureCorrMIP"));

    registerProcessorParameter("OutputAhcHitV2CollectionName",
                               "Name of the output AHCal hit version 2 collection, of type CalorimeterHit",
                               _ahcHitOutputV2ColName,
                               std::string("AhcCalorimeter_HitsV2"));
  }

  void ConvertAmpl2CaloHitProcessor::init()
  {
    printParameters();
  }

  void ConvertAmpl2CaloHitProcessor::processEvent(LCEvent *evt)
  {

    LCCollection *inputAhcHitCol = NULL;
    try
      {
        inputAhcHitCol = evt->getCollection(_ahcHitColName);
      }
    catch (EVENT::DataNotAvailableException &e)
      {
        streamlog_out(WARNING)<< "missing collection "
                      <<_ahcHitColName<<endl<<e.what()<<endl;
        return;
      }

    LCCollection *inputAhcAmplCol = NULL;
    try
      {
        inputAhcAmplCol = evt->getCollection(_ahcAmplColName);
      }
    catch (EVENT::DataNotAvailableException &e)
      {
        streamlog_out(WARNING)<< "missing collection "
                      <<_ahcAmplColName<<endl<<e.what()<<endl;
        return;
      }

    LCCollection *inputHitAmplRelationCol = NULL;
    try
      {
        inputHitAmplRelationCol = evt->getCollection(_hitAmplRelationColName);
      }
    catch (EVENT::DataNotAvailableException &e)
      {
        streamlog_out(WARNING)<< "missing collection "
                      <<_hitAmplRelationColName<<endl<<e.what()<<endl;
        return;
      }

    /* start loop over input collection */
    int noElem = inputAhcHitCol->getNumberOfElements();
    if (noElem <= 0) return;

    streamlog_out(DEBUG)<<" \n\n=========================Start to process event "<<evt->getEventNumber()<<endl;
    streamlog_out(DEBUG)<<"ahcHitColName "<<_ahcHitColName<<" has "<<noElem<<" hits"<<endl;

    LCCollection *ahcHitOutputV2Col  = new LCCollectionVec(LCIO::CALORIMETERHIT);
    ahcHitOutputV2Col->setFlag(ahcHitOutputV2Col->getFlag() | 1 << LCIO::CHBIT_LONG );

    const std::string encodingString = inputAhcHitCol->getParameters().getStringVal(LCIO::CellIDEncoding);

    LCRelationNavigator relNav(inputHitAmplRelationCol);

    for (unsigned int i = 0; i < (unsigned int)noElem; ++i)
      {

        CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inputAhcHitCol->getElementAt(i));
        streamlog_out(DEBUG) <<" CellID0: "<<hit->getCellID0() <<"  HIT ENERGY: " << hit->getEnergy()
                     <<" position: "<<hit->getPosition()[0]<<" "<<hit->getPosition()[1]<<" " <<hit->getPosition()[2] <<endl;

        const LCObjectVec amplVec = relNav.getRelatedToObjects(hit);
        LCGenericObject *amplObj = dynamic_cast<LCGenericObject*>(amplVec[0]);

        AhcAmplitude *ahcAmpl = new AhcAmplitude(amplObj);

        float amplGeV = ahcAmpl->getAmplGeV();

        streamlog_out(DEBUG)<<" CellID   "<<ahcAmpl->getCellID()
                    <<" f:"<<ahcAmpl->getAmplRawADC()
                    <<" f:"<<ahcAmpl->getAmplRawMinusPedestalADC()
                    <<" f:"<<ahcAmpl->getAmplTemperatureCorrMIP()
                    <<" f:"<<ahcAmpl->getAmplNOTTemperatureCorrMIP()
                    <<" ampl[GeV] = "<<amplGeV<<endl;

        CalorimeterHitImpl *newHit = new CalorimeterHitImpl();

        /* set parameters of new hit */
        //int cellID = ahcAmpl->getCellID();
        //newHit->setCellID0(cellID);
        newHit->setCellID0( hit->getCellID0() );
        newHit->setCellID1( hit->getCellID1() );
        newHit->setTime( hit->getTime() );
        newHit->setPosition( hit->getPosition() );

        /* set amplitude of new hit */
        if ( _amplName == "RawADC" )
          newHit->setEnergy( ahcAmpl->getAmplRawADC() );
        else if ( _amplName == "RawMinusPedestalADC" )
          newHit->setEnergy( ahcAmpl->getAmplRawMinusPedestalADC() );
        else if ( _amplName == "TemperatureCorrMIP" )
          newHit->setEnergy( ahcAmpl->getAmplTemperatureCorrMIP() );
        else if ( _amplName == "NOTTemperatureCorrMIP" )
          newHit->setEnergy( ahcAmpl->getAmplNOTTemperatureCorrMIP() );
        else if ( _amplName == "GeV" )
          newHit->setEnergy( ahcAmpl->getAmplGeV() );
        else
          newHit->setEnergy( 0. );

        /* add new hit to output collection */
        ahcHitOutputV2Col->addElement(newHit);

        delete ahcAmpl;
      }

    if (ahcHitOutputV2Col->getNumberOfElements() > 0)
      {
        //const string encodingString = calibration->getCellIDEncoding();
        LCParameters &param = ahcHitOutputV2Col->parameters();
        param.setValue(LCIO::CellIDEncoding, encodingString);

        //set flag and additional parameters (int, float, string)
        ahcHitOutputV2Col->setFlag( inputAhcHitCol->getFlag() );

        StringVec parameters_intKeys;
        inputAhcHitCol->parameters().getIntKeys( parameters_intKeys );
        for ( unsigned int i = 0; i < parameters_intKeys.size(); i++) {
          std::vector<int> tempVec_int;
          inputAhcHitCol->parameters().getIntVals( parameters_intKeys[i], tempVec_int );
          ahcHitOutputV2Col->parameters().setValues( parameters_intKeys[i], tempVec_int );
        }

        StringVec parameters_floatKeys;
        inputAhcHitCol->parameters().getFloatKeys( parameters_floatKeys );
        for ( unsigned int i = 0; i < parameters_intKeys.size(); i++) {
          std::vector<float> tempVec_float;
          inputAhcHitCol->parameters().getFloatVals( parameters_floatKeys[i], tempVec_float );
          ahcHitOutputV2Col->parameters().setValues( parameters_floatKeys[i], tempVec_float );
        }

        StringVec parameters_stringKeys;
        inputAhcHitCol->parameters().getStringKeys( parameters_stringKeys );
        for ( unsigned int i = 0; i < parameters_stringKeys.size(); i++) {
          std::vector<std::string> tempVec_string;
          inputAhcHitCol->parameters().getStringVals( parameters_stringKeys[i], tempVec_string );
          ahcHitOutputV2Col->parameters().setValues( parameters_stringKeys[i], tempVec_string );
        }

        evt->addCollection(ahcHitOutputV2Col, _ahcHitOutputV2ColName.c_str());

      } else {delete ahcHitOutputV2Col;}

  }

  void ConvertAmpl2CaloHitProcessor::end()
  {
    streamlog_out(MESSAGE)<<"End of ConvertAmpl2CaloHitProcessor" <<endl;
  }

}//namespace CALICE
