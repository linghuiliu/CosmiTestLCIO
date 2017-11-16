#include "DEHEventDisplayProcessor.hh"
#include <iostream>
#include <iomanip>
#include <typeinfo>
/* ----- include for verbosity dependend logging ---------*/
#include "marlin/VerbosityLevels.h"

#include "marlin/Exceptions.h"

/* LCIO includes*/
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCTOOLS.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/Cluster.h"
#include "UTIL/CellIDDecoder.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCCollectionVec.h"

/* CED includes*/
#include "ced_cli.h"

/* CALICE includes*/
#include "MappingProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "Ahc2Calibrations.hh"
#include "Ahc2CalibrationsProcessor.hh"
#include "Ahc2CalibrationStatusBits.hh"
#include "CellIterator.hh"
#include "ClusterShapesTyped.hh"
#include "CellIndex.hh"

using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace CALICE {

  DEHEventDisplayProcessor aDEHEventDisplayProcessor;



  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  DEHEventDisplayProcessor::DEHEventDisplayProcessor() : Processor("DEHEventDisplayProcessor"),
  _mapper(0), _cellDescriptions(0)
  {
    /* modify processor description*/
    _description = "processor for displaying CALICE events in CED";

    registerInputCollection( LCIO::CALORIMETERHIT,
      "Collection_EBUCalorimeterHits" ,
      "Name of the EBU ECAL CalorimeterHit collection"  ,
      _colNameEBU ,
      std::string("EBUCalorimeter_Hits") ) ;

      registerInputCollection( LCIO::CALORIMETERHIT,
        "Collection_HBUCalorimeterHits" ,
        "Name of the HBU CalorimeterHit collection"  ,
        _colNameHBU ,
        std::string("HBUCalorimeter_Hits") ) ;


        registerProcessorParameter( "ProcessorName_Mapping" ,
        "name of Ahc2 MappingProcessor which takes care of the mapping",
        _mappingProcessorName,
        std::string("Ahc2MappingProcessor") ) ;

        registerProcessorParameter( "ProcessorName_CellNeighbours" ,
        "name of Ahcal CellNeighboursProcessor which takes care of the cell neighbours calculation",
        _cellNeighboursProcessorName,
        std::string("MyCellNeighboursProcessor") ) ;

        registerProcessorParameter( "ProcessorName_CellDescription" ,
        "name of Ahcal CellDescriptionProcessor which takes care of the cell description generation",
        _cellDescriptionProcessorName,
        std::string("Ahc2CellDescriptionProcessor") ) ;

        registerProcessorParameter( "Ahc2CalibrationsProcessorName" ,
        "Name of the Ahc2CalibrationsProcessor that provides"
        " the calibrations of the AHCal tiles." ,
        _calibProcessorName,
        std::string("MyAhc2CalibrationsProcessor") ) ;

        registerProcessorParameter( "NLayer_EBU" ,
        "Number of Layers (EBU) in detector" ,
        _nLayer_EBU,
        int(0) ) ;

        registerProcessorParameter( "NLayer_HBU" ,
        "Number of Layers (HBU single) in detector" ,
        _nLayer_HBU,
        int(7) ) ;

        registerProcessorParameter( "NLayer_HBU2x2" ,
        "Number of Layers (HBU 2x2) in detector" ,
        _nLayer_HBU2x2,
        int(4) ) ;

        registerProcessorParameter( "CED_WaitForKeyPressed" ,
        "0 disables to wait for key at end of event" ,
        _waitForKeyPressed,
        int(1) ) ;

        registerProcessorParameter( "CED_Draw" ,
        "0 disables CED drawing" ,
        _draw,
        int(1) ) ;

        registerProcessorParameter( "CED_AppendToExistingCED" ,
        "1 skips opening a new CED connection and appends output to existing CED event" ,
        _appendToExistingCED,
        int(0) ) ;

        registerProcessorParameter( "CED_Port" ,
        "Port to connect to CED" ,
        _portCED,
        int(5622) ) ;

        registerProcessorParameter( "T0_selection",
        "Do the event selection based on T0 information",
        _doT0selection,
        bool(false) );

        registerProcessorParameter( "T0_Number",
        "Number of T0s needed to select the event",
        _nT0s,
        int(0) );

        StringVec T0Example;
        T0Example.push_back("ICoordinate");
        T0Example.push_back("JCoordinate");
        T0Example.push_back("KCoordinate");

        registerOptionalParameter( "T0Handler" ,
        "List of T0s in the AHCAL"  ,
        _T0Vector ,
        T0Example ,
        T0Example.size()) ;


      }


      /**************************************************************************************/
      /*                                                                                    */
      /*                                                                                    */
      /*                                                                                    */
      /**************************************************************************************/
      void DEHEventDisplayProcessor::init()
      {
        streamlog_out(DEBUG) << "     DEHEventDisplayProcessor::init()  " << std::endl ;

        /* usually a good idea to*/
        printParameters() ;


        bool error = false;


        _mapper = dynamic_cast<const CALICE::Ahc2Mapper*> ( CALICE::MappingProcessor::getMapper(_mappingProcessorName) );

        if ( ! _mapper) {
          streamlog_out(ERROR) << "Cannot obtain Ahc2Mapper from MappingProcessor "<<_mappingProcessorName
          <<". Mapper not present or wrong type." << std::endl;
          error = true;
        }
        _mapperVersion = _mapper->getVersion();

        _cellNeighbours = CALICE::CellNeighboursProcessor::getNeighbours(_cellNeighboursProcessorName);
        if ( ! _cellNeighbours ) {
          streamlog_out(ERROR) << "Cannot obtain cell neighbours from CellNeighboursProcessor "<<_cellNeighboursProcessorName
          <<". Maybe, processor is not present" << std::endl;
          error = true;
        }

        _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
        if ( ! _cellDescriptions ) {
          streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor "<<_cellDescriptionProcessorName
          <<". Maybe, processor is not present" << std::endl;
          error = true;
        }

        _calibContainer = Ahc2CalibrationsProcessor::getCalibrations(_calibProcessorName);
        if ( ! _calibContainer )
        {
          streamlog_out(ERROR) << "init(): Ahc2CalibrationsProcessor::getCalibrations("<< _calibProcessorName
          << ") did not return a valid MappedContainer." <<std::endl;
          error = true;
        }

        if (error) throw StopProcessingException(this);


        _mapT0s.clear();

        //Process T0 (I, J, K) and put then into a map
        if( parameterSet( "T0Handler" ) )
        {
          unsigned index = 0 ;
          while( index < _T0Vector.size() )
          {

            string strI( _T0Vector[ index++ ] );
            string strJ( _T0Vector[ index++ ] );
            string strK( _T0Vector[ index++ ] );

            int I = atoi(strI.c_str());
            int J = atoi(strJ.c_str());
            int K = atoi(strK.c_str());

            if(_mapT0s.count(K) == 0)
            {
              //Create Table of IJK T0s
              vector<pair<int, int> > vec_pair;
              pair<int, int> IJpair;
              IJpair = make_pair(I, J);
              vec_pair.push_back(IJpair);
              _mapT0s.insert(make_pair(K,vec_pair));
            }
            else
            {
              map<int, vector<pair<int, int> > >::iterator it;
              for(it = _mapT0s.begin(); it != _mapT0s.end(); ++it)
              {
                vector<pair<int, int> > &vec_pair = it->second;
                pair<int, int> IJpair;
                IJpair = make_pair(I, J);
                vec_pair.push_back(IJpair);
              }
            }
          }

          streamlog_out(MESSAGE) << "Number of Layers with T0s: " << _mapT0s.size() << endl;

        }

        _nRun = 0 ;
        _nEvt = 0 ;

        /*CED part*/
        if (_draw && !_appendToExistingCED)
        {
          ced_client_init("localhost",_portCED);
          ced_register_elements();
        }


      }


      /**************************************************************************************/
      /*                                                                                    */
      /*                                                                                    */
      /*                                                                                    */
      /**************************************************************************************/
      void DEHEventDisplayProcessor::processRunHeader( LCRunHeader* run) {

        _nRun++ ;
      }


      /**************************************************************************************/
      /*                                                                                    */
      /*                                                                                    */
      /*                                                                                    */
      /**************************************************************************************/
      void DEHEventDisplayProcessor::drawCell(const CALICE::CellDescription* description,
        const unsigned int color, const unsigned int layer,
        const bool solid) const
        {
          if (_draw) {
            double cellSize[3]  = {description->getSizeX(),description->getSizeY(),5};
            double cellPos[3]   = {description->getX(),description->getY(),description->getZ()};
            double cellAngle[3] = {0.,description->getAngle(),0.};
            if (solid) ced_geobox_r_solid(cellSize,cellPos,cellAngle,color,layer << CED_LAYER_SHIFT);
            else ced_geobox_r(cellSize,cellPos,cellAngle,color,layer << CED_LAYER_SHIFT);
          }
        }

        /**************************************************************************************/
        /*                                                                                    */
        /*                                                                                    */
        /*                                                                                    */
        /**************************************************************************************/
        void DEHEventDisplayProcessor::drawCell2(const CALICE::CellDescription* description,
          const unsigned int color, const unsigned int layer,
          const bool solid) const
          {
            if (_draw) {
              double cellSize[3]  = {10,10,10};
              double cellPos[3]   = {description->getX(),description->getY(),description->getZ()};
              double cellAngle[3] = {0.,description->getAngle(),0.};
              if (solid) ced_geobox_r_solid(cellSize,cellPos,cellAngle,color,layer << CED_LAYER_SHIFT);
              else ced_geobox_r(cellSize,cellPos,cellAngle,color,layer << CED_LAYER_SHIFT);
            }
          }


          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          void DEHEventDisplayProcessor::drawHit(const CalorimeterHit* hit,const unsigned int color, const unsigned int layer) const
          {
            if (_draw) ced_hit(hit->getPosition()[0],hit->getPosition()[1],hit->getPosition()[2],layer << CED_LAYER_SHIFT,2,color);
          }


          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          void DEHEventDisplayProcessor::chooseColorAndLayer(const float energy, unsigned int& color, unsigned int& layer) const
          {
            if (energy < 0.5) {
              color = 0x444444 ;//grey
            }
            else if ( (energy >= 0.5) && (energy < 1.65) ) {
              color = 0x00aa00 ;//green
              layer += 1;
            }
            else if ( (energy >= 1.65) && (energy < 2.9) ) {
              color = 0xbbbb00;//kaki
              layer += 2;
            }
            else if ( (energy >= 2.9) && (energy < 5.4) ) {
              color = 0xdd8800;//gold
              layer += 3;
            }
            else if ( energy >= 5.4 ) {
              color = 0xaa0000 ;//red
              layer += 4;
            }

          }


          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          void DEHEventDisplayProcessor::drawEBU(LCEvent *evt)
          {
            try {
              LCCollection* col = evt->getCollection(  _colNameEBU );

              LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

              // make sure to use the right encoding for the collection just fetched
              _cellDescriptions->getDecoder()->setCellIDEncoding( col->getParameters().getStringVal("CellIDEncoding") );

              float esum = 0;
              float nhits = 0;

              for (LCTypedVector<CalorimeterHit>::iterator iter = calorimterHitsCol.begin();iter != calorimterHitsCol.end();++iter) {

                int cellID = (*iter)->getCellID0();
                float energy = (*iter)->getEnergy();

                //if (energy < 0.5) continue;
                CALICE::CellDescription* cellDescription = _cellDescriptions->getByCellID(cellID);

                esum += energy;
                nhits++;

                streamlog_out(DEBUG)<<setiosflags(ios::right)<< setiosflags(ios::fixed) << setprecision(2)
                <<"HCAL: x="<< setw(8)<<(*iter)->getPosition()[0]
                <<", y="<<setiosflags(ios::right)<<setw(8)<<(*iter)->getPosition()[1]
                <<", z="<<setiosflags(ios::right)<<setw(8)<<(*iter)->getPosition()[2]
                <<", I/J/K="<<setiosflags(ios::right)<<setw(8)<<_mapper->getDecoder()->getIFromCellID(cellID)
                <<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
                <<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
                <<" energy="<<setiosflags(ios::right)<<setw(6)<<energy
                <<std::endl;


                unsigned int color=0;
                unsigned int layer=1;

                chooseColorAndLayer(energy,color,layer);

                //drawHit((*iter),color,layer);
                //drawCell2(cellDescription,color,layer);
                //drawCell2(cellDescription,color,20,false);
                drawCell(cellDescription,color,layer);
                drawCell(cellDescription,color,20,false);

              }

              streamlog_out(DEBUG)<<"\n energy sum: "<<esum<<", "<<nhits<<" hits"<<std::endl;
            }
            catch ( EVENT::DataNotAvailableException &err) {
              streamlog_out(WARNING) << "AHCAL collection " << _colNameEBU << " not available" << std::endl;
            }
          }

          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          void DEHEventDisplayProcessor::drawHBU(LCEvent *evt)
          {
            try {
              LCCollection* col = evt->getCollection( _colNameHBU );

              LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

              // make sure to use the right encoding for the collection just fetched
              _cellDescriptions->getDecoder()->setCellIDEncoding( col->getParameters().getStringVal("CellIDEncoding") );

              float esum = 0;
              float nhits = 0;

              for (LCTypedVector<CalorimeterHit>::iterator iter = calorimterHitsCol.begin();iter != calorimterHitsCol.end();++iter) {

                int cellID = (*iter)->getCellID0();
                //int K = _mapper->getDecoder()->getKFromCellID(cellID);

                Ahc2Calibrations *calibration = _calibContainer->getByCellID(cellID);
                const Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( calibration->getStatus() );

                float energy = (*iter)->getEnergy();

                //if (energy < 0.5) continue;
                CALICE::CellDescription* cellDescription = _cellDescriptions->getByCellID(cellID);

                esum += energy;
                nhits++;

                streamlog_out(DEBUG)<<setiosflags(ios::right)<< setiosflags(ios::fixed) << setprecision(2)
                <<"HCAL: x="<< setw(8)<<(*iter)->getPosition()[0]
                <<", y="<<setiosflags(ios::right)<<setw(8)<<(*iter)->getPosition()[1]
                <<", z="<<setiosflags(ios::right)<<setw(8)<<(*iter)->getPosition()[2]
                <<", I/J/K="<<setiosflags(ios::right)<<setw(8)<<_mapper->getDecoder()->getIFromCellID(cellID)
                <<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
                <<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
                <<" energy="<<setiosflags(ios::right)<<setw(6)<<energy
                <<std::endl;


                unsigned int color=0;
                unsigned int layer=1;

                chooseColorAndLayer(energy,color,layer);

                //drawHit((*iter),color,layer);
                //drawCell2(cellDescription,color,layer);
                //drawCell2(cellDescription,color,21,false);
                drawCell(cellDescription,color,layer);
                drawCell(cellDescription,color,21,false);

              }

              streamlog_out(DEBUG)<<"\n energy sum: "<<esum<<", "<<nhits<<" hits"<<std::endl;
            }
            catch ( EVENT::DataNotAvailableException &err) {
              streamlog_out(WARNING) << "AHCAL collection " << _colNameHBU << " not available" << std::endl;
            }
          }

          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          /* @todo implement rotated/staggered detector...*/

          void  DEHEventDisplayProcessor::drawDetectorDEH()
          {

            for(int iE = 1; iE<= _nLayer_EBU; iE++){

              double EBUSize[3] = {180.,180.,3};
              CALICE::CellDescription *cell77 = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(1,1,iE)));
              double center_pos[3] = {0.,0.,cell77->getZ()};

              streamlog_out(DEBUG0)<<"Layer     : "<< iE << std::endl;
              streamlog_out(DEBUG0)<<"EBUSize   : "<< EBUSize[0] << std::endl;
              streamlog_out(DEBUG0)<<"center_pos: 0, 0, "<< center_pos[2]<< "\n" << std::endl;

              if (_draw) ced_geobox(EBUSize,center_pos,0x000000);
              if (_draw) ced_geobox(EBUSize,center_pos,0xa4a4a4);
            }

            for(int iH = _nLayer_EBU+1; iH<= _nLayer_EBU + _nLayer_HBU; iH++){

              double HBUSize[3] = {360.,360.,3};
              CALICE::CellDescription *cell77 = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(7,7,iH)));
              double center_pos[3] = {0.,0.,cell77->getZ()};

              streamlog_out(DEBUG0)<<"Layer     : "<< iH << std::endl;
              streamlog_out(DEBUG0)<<"HBUSize   : "<< HBUSize[0] << std::endl;
              streamlog_out(DEBUG0)<<"center_pos: 0, 0, "<< center_pos[2]<< "\n" << std::endl;

              if (_draw) ced_geobox(HBUSize,center_pos,0x000000);
              if (_draw) ced_geobox(HBUSize,center_pos,0xa4a4a4);
            }

            for(int jH = _nLayer_EBU + _nLayer_HBU + 1; jH<= _nLayer_EBU + _nLayer_HBU + _nLayer_HBU2x2; jH++){

              double HBU2by2Size[3] = {720.,720.,3};
              CALICE::CellDescription *cell77 = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(7,7,jH)));
              double center_pos[3] = {0.,0,cell77->getZ()};

              streamlog_out(DEBUG0)<<"Layer     : "<< jH << std::endl;
              streamlog_out(DEBUG0)<<"HBUSize   : "<< HBU2by2Size[0] << std::endl;
              streamlog_out(DEBUG0)<<"center_pos: 0, 0, "<< center_pos[2]<< "\n" << std::endl;

              if (_draw) ced_geobox(HBU2by2Size,center_pos,0x000000);
              if (_draw) ced_geobox(HBU2by2Size,center_pos,0xa4a4a4);

            }

          }

          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/

          bool DEHEventDisplayProcessor::isT0event( LCEvent * evt )
          {

            bool isT0event = false;
            int nT0 = 0;

            try {
              LCCollection* col = evt->getCollection( _colNameHBU );

              LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

              // make sure to use the right encoding for the collection just fetched
              _cellDescriptions->getDecoder()->setCellIDEncoding( col->getParameters().getStringVal("CellIDEncoding") );

              for (LCTypedVector<CalorimeterHit>::iterator iter = calorimterHitsCol.begin();iter != calorimterHitsCol.end();++iter) {

                int cellID = (*iter)->getCellID0();
                float energy = (*iter)->getEnergy();
                //CALICE::CellDescription* cellDescription = _cellDescriptions->getByCellID(cellID);

                int I = _mapper->getDecoder()->getIFromCellID(cellID);
                int J = _mapper->getDecoder()->getJFromCellID(cellID);
                int K = _mapper->getDecoder()->getKFromCellID(cellID);


                //Check in the T0s map and increment number of T0 in the event
                map<int, vector<pair<int, int> > >::iterator found = _mapT0s.find(K);
                if(found != _mapT0s.end())
                {
                  vector<pair<int, int> >::iterator it;
                  for(it = found->second.begin(); it != found->second.end(); ++it)
                  {
                    if(it->first == I && it->second == J && energy > 0.5)
                    {
                      nT0++;
                    }
                  }
                }
              }

              if(nT0 > _nT0s)
              {
                isT0event = true;
                cout << "Found T0 event" << endl;
              }
            }
            catch ( EVENT::DataNotAvailableException &err) {
              streamlog_out(WARNING) << "AHCAL collection " << _colNameHBU << " not available" << std::endl;
            }

            return isT0event;
          }

          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          void DEHEventDisplayProcessor::processEvent( LCEvent * evt )
          {
            streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
            << "   in run:  " << evt->getRunNumber()
            << std::endl ;

            //_mapper->print(std::cout);
            //_mapper->printStats(std::cout);

            bool evtempty = false;

            /* CED */
            if (_draw && !_appendToExistingCED)
            {
              ced_new_event();
            }

            //Check if event is empty
            try {
              LCCollection* col = evt->getCollection(  _colNameHBU );
              LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

              if(calorimterHitsCol.size() <= 0)
              {
                streamlog_out(WARNING) << "AHCAL collection " << _colNameHBU << " empty" << std::endl;
                evtempty = true;
              }
            }
            catch ( EVENT::DataNotAvailableException &err) {
              streamlog_out(WARNING) << "AHCAL collection " << _colNameHBU << " not available" << std::endl;
              evtempty = true;
            }

            if(evtempty)
            _nEvt++;
            else
            {
              if(_doT0selection)
              {
                bool _eventwithT0 = isT0event(evt);

                if(_eventwithT0)
                {
                  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
                  << "   in run:  " << evt->getRunNumber()
                  << " is T0 event "
                  << std::endl ;
                  if (_draw)
                  {
                    drawDetectorDEH();
                    /* draw EBU related part */
                    if( parameters()->isParameterSet("Collection_EBUCalorimeterHits") )
                    {
                      drawEBU(evt);
                    }

                    /* draw AHCAL related part */
                    if( parameters()->isParameterSet("Collection_HBUCalorimeterHits") )
                    {
                      drawHBU(evt);
                    }



                    /* last step: send event to CED */
                    ced_send_event();

                  }

                  if (_waitForKeyPressed) {
                    std::cout << " event " << evt->getEventNumber() << "       [ Press return for next event ] " << std::endl ;
                    getchar();
                  }
                }

                _nEvt++ ;

              }
              else
              {
                if (_draw)
                {
                  drawDetectorDEH();
                  /* draw EBU related part */
                  if( parameters()->isParameterSet("Collection_EBUCalorimeterHits") )
                  {
                    drawEBU(evt);
                  }

                  /* draw AHCAL related part */
                  if( parameters()->isParameterSet("Collection_HBUCalorimeterHits") )
                  {
                    drawHBU(evt);
                  }


                  /* last step: send event to CED */
                  ced_send_event();

                }

                if (_waitForKeyPressed) {
                  std::cout << " event " << evt->getEventNumber() << "       [ Press return for next event ] " << std::endl ;
                  getchar();
                }

                _nEvt++ ;

              }
            }
          }

          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          void DEHEventDisplayProcessor::check( LCEvent * evt )
          {

          }


          /**************************************************************************************/
          /*                                                                                    */
          /*                                                                                    */
          /*                                                                                    */
          /**************************************************************************************/
          void DEHEventDisplayProcessor::end()
          {

          }

        }
