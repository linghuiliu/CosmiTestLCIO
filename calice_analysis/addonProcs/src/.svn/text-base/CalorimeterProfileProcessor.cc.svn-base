#include "CalorimeterProfileProcessor.hh"

// Marlin includes
#include "marlin/Exceptions.h"

// lcio includes
#include "lcio.h"
#include "UTIL/LCTypedVector.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/LCCollection.h"

// c++ includes

#include <iostream>
#include <cmath>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// CALICE includes
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "VirtualCellsProcessor.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  CalorimeterProfileProcessor::CalorimeterProfileProcessor() : Processor("CalorimeterProfileProcessor") {

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "AhcCollection" ,
                             "Name of the AHCAL CalorimeterHit collection"  ,
                             _caloColName,
                             std::string("Calorimeter_Hits") ) ;

    registerInputCollection( LCIO::SIMCALORIMETERHIT,
                             "AhcSimCollection" ,
                             "Name of the AHCAL SimCalorimeterHit collection"  ,
                             _simCaloColName,
                             std::string("hcalSD") ) ;

    registerProcessorParameter( "MappingProcessorName" ,
                                "name of MappingProcessor which takes care of the mapping",
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "name of CellDescriptionProcessor which takes care of the cell description generation",
                                _cellDescriptionProcessorName,
                                std::string("MyCellDescriptionProcessor") ) ;

    registerProcessorParameter( "VirtualCellsProcessorName" ,
                                "name of VirtualCellsProcessor",
                                _virtualCellsProcessorName,
                                std::string("MyVirtualCellsProcessor") ) ;

    registerProcessorParameter( "ShowerStartProcessorName" ,
                                "name of ShowerStartProcessor",
                                _showerStartProcessorName,
                                std::string("MyShowerStartProcessor") ) ;

    registerProcessorParameter( "CenterOfGravityParameterName" ,
                                "name of FloatVec parameter with center of gravity information",
                                _cogXYParameterName,
                                std::string("cogXY") ) ;

    _middleCellIndex.push_back(45);
    _middleCellIndex.push_back(45);
    _middleCellIndex.push_back(1);

    _middleCellIndex.push_back(45);
    _middleCellIndex.push_back(46);
    _middleCellIndex.push_back(1);

    _middleCellIndex.push_back(46);
    _middleCellIndex.push_back(45);
    _middleCellIndex.push_back(1);

    _middleCellIndex.push_back(46);
    _middleCellIndex.push_back(46);
    _middleCellIndex.push_back(1);

    registerProcessorParameter( "MiddleCellIndex" ,
                                std::string("I,J,K of the front layer middle cell to use as origin and cut range. ")+
                                "If there is no exact middle cell, give all surounding (virtual) cells. The average will be used then.",
                                _middleCellIndex,
                                _middleCellIndex ) ;


    _middleCellIndexBack.push_back(45);
    _middleCellIndexBack.push_back(45);
    _middleCellIndexBack.push_back(38);
    registerProcessorParameter( "MiddleCellIndexBack" ,
                                std::string("I,J,K of the back layer middle cell to use as cut range. ")+
                                "If there is no exact middle cell, give all surounding (virtual) cells. The average will be used then.",
                                _middleCellIndexBack,
                                _middleCellIndexBack ) ;

    _zRange.push_back(-10);
    _zRange.push_back(50);
    registerProcessorParameter( "ZRange" ,
                                std::string("Range of Z binning (start layer,end layer). The bin width is fixed to the thickness of a layer. ")+
                                "Negative values are allowed to extend to the front of the detector.",
                                _zRange,
                                _zRange ) ;

    _rBinning.push_back(10.);
    _rBinning.push_back(450.);
    registerProcessorParameter( "RBinning" ,
                                "Width of r segmentation and upper range (width, maxR) in mm",
                                _rBinning,
                                _rBinning ) ;

    registerProcessorParameter( "Threshold" ,
                                "Threshold to consider hit in mip",
                                _threshold,
                                (float)0.5 ) ;

    _zOffset.push_back(0.);
    registerProcessorParameter( "ZOffset" ,
                                std::string("z-offset [mm] for which the shower start position should be corrected. ") +
                                "If 3 numbers are given, two histograms for systematic error with given additional shift are filled.",
                                _zOffset,
                                _zOffset ) ;


    registerOptionalParameter( "ScaleSimEnergy" ,
                               "scale factor for the simulation energy",
                               _simEnergyScaleFactor,
                               (float)1. ) ;

    registerProcessorParameter( "SuppressOutsideFirstAndLastLayerCenter" ,
                                "ignore all hits that have z coordinate smaller than first layer center bin and larger than last layer center bin",
                                _suppressOutsideFirstAndLastLayerCenter,
                                (int)1 ) ;


  }

  void CalorimeterProfileProcessor::init() {

    printParameters();

    bool error = false;

    _mapper =  MappingProcessor::getMapper(_mappingProcessorName);
    if ( ! _mapper) {
      streamlog_out(ERROR) << "Cannot obtain Mapper from MappingProcessor "<<_mappingProcessorName<<". Mapper not present or wrong type." << std::endl;
      error = true;
    }

    _cellDescriptions = CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions ) {
      streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor " << _cellDescriptionProcessorName << ". Maybe, processor is not present" << std::endl;
      error = true;
    }

    _virtualCells = VirtualCellsProcessor::getVirtualCells(_virtualCellsProcessorName);
    if ( ! _virtualCells ) {
      streamlog_out(ERROR) << "Cannot obtain virtual cell positions from VirtualCellsProcessor " << _virtualCellsProcessorName << ". Maybe, processor is not present" << std::endl;
      error = true;
    }

    if ( ! ( _zOffset.size() == 1 | _zOffset.size() ==3 ) ) {
      streamlog_out(ERROR) << "ZOffset should have either 1 or 3 elements. You specified " <<_zOffset.size() << std::endl;
      error = true;
    }

    if (error) throw StopProcessingException(this);

    _virtualCellsVersion = _virtualCells->getVersion();
    _firstEvent = true;

  }


  unsigned int CalorimeterProfileProcessor::getIndexForValue(const std::map<float, unsigned int> &lookup, const float value) const {

    std::map<float, unsigned int>::const_iterator cIter = lookup.lower_bound(value);

    if (cIter == lookup.end() ) streamlog_out(ERROR) << "getIndexForValue(): no lower bound found " << value << std::endl;

    return cIter->second;
  }


  void CalorimeterProfileProcessor::initGlobalPosition() {

    float x = 0;
    float y = 0;
    float z = 0;
    float xBack = 0;
    float yBack = 0;
    float zBack = 0;

    int nCells = 0;

    float deltaZ = 0;

    for (unsigned int i = 0; i < _middleCellIndex.size(); i+=3) {
      int cellID = _mapper->getTrueCellID(_mapper->getDecoder()->getCellID(_middleCellIndex[i],_middleCellIndex[i+1],_middleCellIndex[i+2]));
      int cellID2 = _mapper->getTrueCellID(_mapper->getDecoder()->getCellID(_middleCellIndex[i],_middleCellIndex[i+1],_middleCellIndex[i+2]+1));
      ++nCells;

      x      += _cellDescriptions->getByCellID(cellID)->getX();
      y      += _cellDescriptions->getByCellID(cellID)->getY();
      z      += _cellDescriptions->getByCellID(cellID)->getZ();
      deltaZ += _cellDescriptions->getByCellID(cellID2)->getZ() - _cellDescriptions->getByCellID(cellID)->getZ();
    }

    x      /= (float) nCells;
    y      /= (float) nCells;
    z      /= (float) nCells;
    deltaZ /= (float) nCells;

    nCells = 0;

    for (unsigned int i = 0; i < _middleCellIndexBack.size(); i+=3) {
      int cellID = _mapper->getTrueCellID(_mapper->getDecoder()->getCellID(_middleCellIndexBack[i],_middleCellIndexBack[i+1],_middleCellIndexBack[i+2]));
      ++nCells;
      xBack += _cellDescriptions->getByCellID(cellID)->getX();
      yBack += _cellDescriptions->getByCellID(cellID)->getY();
      zBack += _cellDescriptions->getByCellID(cellID)->getZ();
    }

    xBack /= (float) nCells;
    yBack /= (float) nCells;
    zBack /= (float) nCells;

    _nZBins = _zRange.at(1) - _zRange.at(0) + 1;
    _zMin = z + (_zRange.at(0) - 0.5 )*deltaZ;
    _zMax = z + (_zRange.at(1) + 0.5)*deltaZ;
    _zMinStart = (_zRange.at(0) - 0.5 )*deltaZ;
    _zMaxStart = (_zRange.at(1) + 0.5 )*deltaZ;

    _zMinDetectorAcceptance = z - 0.5 * deltaZ;
    _zMaxDetectorAcceptance = zBack + 0.5 * deltaZ;

    _nRBins = (unsigned int)(_rBinning.at(1)/_rBinning.at(0));
    _rMax = _rBinning.at(1);


    streamlog_out(DEBUG) << "initGlobalPosition:" << std::endl
                 << " position (" << x << "," << y << "," << "," << z << ") " << std::endl
                 << " layer distance " << deltaZ << std::endl
                 << " nuber of bins " << _nZBins << std::endl
                 << " minimal zPosition " << _zMin << std::endl
                 << " maximal zPosition " << _zMax<< std::endl;

  }

  void CalorimeterProfileProcessor::calculateNorm( BinnedVector<float, BinnedVector<float, float> > &norm, BinnedVector<float, BinnedVector<float, float> > &normLowBound, BinnedVector<float, BinnedVector<float, float> > &normHighBound, BinnedVector<float, float> &rNorm, BinnedVector<float, float> &lNorm, const float xOffset, const float yOffset, const float zOffset, const float lowBoundOffset, const float highBoundOffset, const bool calculateBounds) {
    for (unsigned int iCell = 0; iCell < _allVirtualCells.size(); ++iCell)
      for (unsigned int iVCell = 0; iVCell < _allVirtualCells[iCell]->getNumberOfVirtualCells(); ++iVCell) {

        const float *position = _allVirtualCells[iCell]->getVirtualCellPosition(iVCell);

        if (_suppressOutsideFirstAndLastLayerCenter && ( position[2] < _zMinDetectorAcceptance || position[2] > _zMaxDetectorAcceptance ) ) continue;

        float x = position[0] - xOffset;
        float y = position[1] - yOffset;
        float r = sqrt( x*x + y*y);
        float z = position[2] - zOffset;

        ++rNorm[r];
        ++lNorm[z];
        ++norm[z][r];

        if (calculateBounds) {
          float zLow = position[2] - zOffset + lowBoundOffset;
          float zHigh = position[2] - zOffset + highBoundOffset;
          ++normLowBound[zLow][r];
          ++normHighBound[zHigh][r];
          streamlog_out(DEBUG2) << " z low bound " << zLow << " z high bound " << zHigh << std::endl;
        }
      }
  }

  void CalorimeterProfileProcessor::fillLinearFloatVec( const BinnedVector<float, BinnedVector<float,float> >& input, FloatVec& output) const {
    output.clear();
    for (unsigned int iZ = 0; iZ < input.getVector().size(); ++iZ)
      for (unsigned int iR = 0; iR < input.getVector()[iZ].getVector().size(); ++iR)
        output.push_back(input.getVector()[iZ].getVector()[iR]);
  }

  void CalorimeterProfileProcessor::fillLinearCenterFloatVec( const BinnedVector<float, BinnedVector<float,float> >& input, FloatVec& output_center1, FloatVec& output_center2 ) const {
    output_center1.clear();
    output_center2.clear();
    for (unsigned int iZ = 0; iZ < input.getVector().size(); ++iZ) {
      for (unsigned int iR = 0; iR < input.getVector()[iZ].getVector().size(); ++iR) {
        output_center1.push_back(input.getBinCenters()[iZ]);
        output_center2.push_back(input.getVector()[iZ].getBinCenters()[iR]);
      }
    }
  }

  void CalorimeterProfileProcessor::processEvent( LCEvent *evt ) {

    /* First event: create global normalization
     */
    if (_firstEvent || _virtualCells->getVersion() != _virtualCellsVersion ) {

      initGlobalPosition();

      _virtualCellsVersion = _virtualCells->getVersion();
      _allVirtualCells = _virtualCells->getAllElements();

      BinnedVector<float,float> rGlobalNorm(_nRBins,0.,_rMax,0.);
      BinnedVector<float,float> lGlobalNorm(_nZBins,_zMin,_zMax,0.);
      BinnedVector<float,float> lStartNorm(_nZBins,_zMinStart,_zMaxStart,0.);

      BinnedVector<float, BinnedVector<float,float> > globalNorm(_nZBins,_zMin,_zMax, rGlobalNorm );
      BinnedVector<float, BinnedVector<float,float> > dummy(0,0,0,BinnedVector<float,float>(0,0,0,0));

      calculateNorm(globalNorm, dummy, dummy, rGlobalNorm, lGlobalNorm);

      _lGlobalNorm = lGlobalNorm.getVector();
      _lPos        = lGlobalNorm.getBinCenters();
      _lStartPos   = lStartNorm.getBinCenters();
      _rGlobalNorm = rGlobalNorm.getVector();
      _rPos        = rGlobalNorm.getBinCenters();

      fillLinearFloatVec(globalNorm, _globalNorm);

      /* fill vector with z-r position map
       */
      fillLinearCenterFloatVec(globalNorm, _2DlPos, _2DrPos);

    }

    LCCollection* inCol;

    /* check for shower start parameters */
    streamlog_out(DEBUG) << "shower start processor name " << _showerStartProcessorName+std::string("_showerStartPos") << std::endl;
    FloatVec showerStartPos;
    evt->getParameters().getFloatVals(_showerStartProcessorName+std::string("_showerStartPos"),showerStartPos);
    bool goodShowerStart = (showerStartPos.size() > 0 && showerStartPos[2] < std::numeric_limits<float>::max());

    /* check for center of gravity parameters */
    streamlog_out(DEBUG) << "center of gravity (XY) parameter name " << _cogXYParameterName << std::endl;
    FloatVec cogPos;
    evt->getParameters().getFloatVals(_cogXYParameterName,cogPos);
    bool goodCogPosition = (cogPos.size() > 0 && cogPos[2] < std::numeric_limits<float>::max());

    /* DATA or DIGITIZED SIMULATION
     */
    if ( _caloColName != "" )
      try {
        inCol = evt->getCollection( _caloColName );

        _virtualCells->getDecoder()->setCellIDEncoding(inCol->getParameters().getStringVal("CellIDEncoding"));

        lcio::LCTypedVector<lcio::CalorimeterHit> col(inCol);

        /* create binned vectors to collect energy depositions from this event
         */
        BinnedVector<float,float> rGlobalSum(_nRBins,0.,_rMax,0.);
        BinnedVector<float,float> lGlobalSum(_nZBins,_zMin,_zMax,0.);
        BinnedVector<float,float> rStartSum(_nRBins,0.,_rMax,0.);
        BinnedVector<float,float> lStartSum(_nZBins,_zMinStart,_zMaxStart,0.);
        BinnedVector<float,float> rCogSum(_nRBins,0.,_rMax,0.);
        BinnedVector<float,float> lCogSum(_nZBins,_zMin,_zMax,0.);
        BinnedVector<float, BinnedVector<float,float> > globalSum(_nZBins,_zMin,_zMax, rGlobalSum );
        BinnedVector<float, BinnedVector<float,float> > startSum(_nZBins,_zMinStart,_zMaxStart, rStartSum );
        BinnedVector<float, BinnedVector<float,float> > cogSum(_nZBins,_zMin,_zMax, rGlobalSum );

        BinnedVector<float, BinnedVector<float,float> > startSumLowBound(_nZBins,_zMinStart,_zMaxStart, rStartSum );
        BinnedVector<float, BinnedVector<float,float> > startSumHighBound(_nZBins,_zMinStart,_zMaxStart, rStartSum );

        /* loop over all hits
         */
        for (lcio::LCTypedVector<lcio::CalorimeterHit>::iterator iter=col.begin(); iter != col.end(); ++iter)
          if ((*iter)->getEnergy() >= _threshold) {

            int cellID   = (*iter)->getCellID0();
            float energy = (*iter)->getEnergy();

            VirtualCells *virtualCells = _virtualCells->getByCellID( cellID );

            /* estimate number of MIP-like particles contributing to energy deposition in cell and distribute
             * cell energy equally on these particles
             */
            unsigned int nParticlesInCell = (unsigned int) ((energy + 0.74)/1.225);
            float energyPerParticle = energy/(float)nParticlesInCell;

            /* distribute energy from each particle on virtual cell grid
             */
            unsigned int nVirtualCells = virtualCells->getNumberOfVirtualCells();

            std::vector<float> energyInVirtualCell;
            energyInVirtualCell.resize(nVirtualCells,0.);

            for (unsigned int particle = 0; particle < nParticlesInCell; ++particle) {
              unsigned int iVCell = (rand() % nVirtualCells); // get random cell
              energyInVirtualCell[iVCell] += energyPerParticle;
              //                std::cout << "random cell " << iVCell << " out of " << virtualCellX.size() << " possible" << std::endl;
            }

            /* assign energy from virtual cells ro radial, longitudinal, and 2D bins
             */
            for (unsigned int iVCell = 0; iVCell < nVirtualCells; ++iVCell)
              if (energyInVirtualCell[iVCell] > 0) {

                /* global scale, calorimeter reference
                 */
                const float *position = virtualCells->getVirtualCellPosition(iVCell);

                if (_suppressOutsideFirstAndLastLayerCenter && ( position[2] < _zMinDetectorAcceptance || position[2] > _zMaxDetectorAcceptance ) ) continue;

                float virtualX        = position[0];
                float virtualY        = position[1];
                float virtualZ        = position[2];

                float virtualR        = sqrt( virtualX*virtualX + virtualY*virtualY );
                float virtualEnergy   = energyInVirtualCell[iVCell];

                streamlog_out(DEBUG) << "r " << virtualR << " z " << virtualZ << std::endl;

                rGlobalSum[virtualR]           += virtualEnergy;
                lGlobalSum[virtualZ]           += virtualEnergy;
                globalSum[virtualZ][virtualR]  += virtualEnergy;

                /* shift by shower start, shower start reference
                 */
                if ( goodShowerStart ) {

                  float virtualXStart   = virtualX - showerStartPos[0];
                  float virtualYStart   = virtualY - showerStartPos[1];
                  float virtualZStart   = virtualZ - showerStartPos[2] - _zOffset[0];

                  float virtualRStart   = sqrt( virtualXStart*virtualXStart + virtualYStart*virtualYStart );

                  streamlog_out(DEBUG) << " rStart " << virtualRStart << " zStart " << virtualZStart << std::endl;

                  rStartSum[virtualRStart]               += virtualEnergy;
                  lStartSum[virtualZStart]               += virtualEnergy;
                  startSum[virtualZStart][virtualRStart] += virtualEnergy;

                  if (_zOffset.size() == 3) { // fill systematic error histograms
                    startSumLowBound[virtualZStart + _zOffset[1]][virtualRStart] += virtualEnergy;
                    startSumHighBound[virtualZStart + _zOffset[2]][virtualRStart] += virtualEnergy;
                  }
                }

                /* shift by center of gravity, center of gravity reference
                 */
                if ( goodCogPosition ) {

                  float virtualXCog   = virtualX - cogPos[0];
                  float virtualYCog   = virtualY - cogPos[1];
                  float virtualZCog   = virtualZ; // - cogPos[2]; // no shift in z!

                  float virtualRCog   = sqrt( virtualXCog*virtualXCog + virtualYCog*virtualYCog );

                  streamlog_out(DEBUG) << " rCog " << virtualRCog << " zCog " << virtualZCog << std::endl;

                  rCogSum[virtualRCog]               += virtualEnergy;
                  lCogSum[virtualZCog]               += virtualEnergy;
                  cogSum[virtualZCog][virtualRCog]   += virtualEnergy;

                }

              }

          }

        /* calculate normalization for shifted event: shower start reference
         */
        BinnedVector<float,float> rStartNorm(_nRBins,0.,_rMax,0.);
        BinnedVector<float,float> lStartNorm(_nZBins,_zMinStart,_zMaxStart,0.);
        BinnedVector<float, BinnedVector<float,float> > startNorm(_nZBins,_zMinStart,_zMaxStart, rStartNorm );
        BinnedVector<float, BinnedVector<float,float> > startNormLowBound(_nZBins,_zMinStart,_zMaxStart, rStartNorm );
        BinnedVector<float, BinnedVector<float,float> > startNormHighBound(_nZBins,_zMinStart,_zMaxStart, rStartNorm );

	if ( _firstEvent || _virtualCells->getVersion() != _virtualCellsVersion ) {
	  fillLinearCenterFloatVec(startNorm, _2DlStartPos, _2DrStartPos);
	}

	if (goodShowerStart)
          calculateNorm(startNorm, startNormLowBound, startNormHighBound, rStartNorm, lStartNorm, showerStartPos[0], showerStartPos[1], showerStartPos[2] - _zOffset[0], _zOffset[1], _zOffset[2], (_zOffset.size() == 3) );

        /* calculate normalization for shifted event: shower start reference
         */
        BinnedVector<float,float> rCogNorm(_nRBins,0.,_rMax,0.);
        BinnedVector<float,float> lCogNorm(_nZBins,_zMin,_zMax,0.);
        BinnedVector<float, BinnedVector<float,float> > cogNorm(_nZBins,_zMin,_zMax, rCogNorm );

        BinnedVector<float, BinnedVector<float,float> > dummy2(0,0,0,BinnedVector<float,float>(0,0,0,0));

	if ( _firstEvent || _virtualCells->getVersion() != _virtualCellsVersion ) {
	  fillLinearCenterFloatVec(cogNorm, _2DlCogPos, _2DrCogPos);
	}

        if (goodCogPosition)
          calculateNorm(cogNorm, dummy2, dummy2, rCogNorm, lCogNorm, cogPos[0], cogPos[1], 0);


        /* Append global result vectors to .slcio collection.
         * To avoid problems with the RootTreeWriter, all event parameters
         * are written even when these are empty.
         */
        FloatVec rGlobalSum_ = rGlobalSum.getVector();
        FloatVec lGlobalSum_ = lGlobalSum.getVector();
        FloatVec globalSum_;

        fillLinearFloatVec(globalSum, globalSum_);

        inCol->parameters().setValues(name()+"_radialEnergy",rGlobalSum_);
        inCol->parameters().setValues(name()+"_radialNorm",_rGlobalNorm);
        inCol->parameters().setValues(name()+"_radialPosition",_rPos);

        inCol->parameters().setValues(name()+"_longitudinalEnergy",lGlobalSum_);
        inCol->parameters().setValues(name()+"_longitudinalNorm",_lGlobalNorm);
        inCol->parameters().setValues(name()+"_longitudinalPosition",_lPos);

        inCol->parameters().setValues(name()+"_2DEnergy",globalSum_);
        inCol->parameters().setValues(name()+"_2DNorm",_globalNorm);
        inCol->parameters().setValues(name()+"_2DradialPosition",_2DrPos);
        inCol->parameters().setValues(name()+"_2DlongitudinalPosition",_2DlPos);


        /* Append shower start result and position vectors to .slcio collection.
         * To avoid problems with the RootTreeWriter, all event parameters
         * are written even when these are empty.
         */
        FloatVec rStartSum_  = rStartSum.getVector();
        FloatVec lStartSum_  = lStartSum.getVector();
        FloatVec rStartNorm_ = rStartNorm.getVector();
        FloatVec lStartNorm_ = lStartNorm.getVector();

        FloatVec startSum_;
        FloatVec startNorm_;
        fillLinearFloatVec(startSum,  startSum_);
        fillLinearFloatVec(startNorm, startNorm_);

        inCol->parameters().setValues(name()+"_radialEnergyStart",rStartSum_);
        inCol->parameters().setValues(name()+"_radialNormStart",rStartNorm_);
        inCol->parameters().setValues(name()+"_radialPositionStart",_rPos);

        inCol->parameters().setValues(name()+"_longitudinalEnergyStart",lStartSum_);
        inCol->parameters().setValues(name()+"_longitudinalNormStart",lStartNorm_);
        inCol->parameters().setValues(name()+"_longitudinalPositionStart",_lStartPos);

        inCol->parameters().setValues(name()+"_2DEnergyStart",startSum_);
        inCol->parameters().setValues(name()+"_2DNormStart",startNorm_);
        inCol->parameters().setValues(name()+"_2DradialPositionStart",_2DrPos);
        inCol->parameters().setValues(name()+"_2DlongitudinalPositionStart",_2DlStartPos);

        if (_zOffset.size() == 3) {
          FloatVec startSumLowBound_;
          FloatVec startSumHighBound_;
          FloatVec startNormLowBound_;
          FloatVec startNormHighBound_;
          fillLinearFloatVec(startSumLowBound,  startSumLowBound_);
          fillLinearFloatVec(startSumHighBound, startSumHighBound_);
          fillLinearFloatVec(startNormLowBound, startNormLowBound_);
          fillLinearFloatVec(startNormHighBound, startNormHighBound_);
          inCol->parameters().setValues(name()+"_2DEnergyStartLowBound",startSumLowBound_);
          inCol->parameters().setValues(name()+"_2DEnergyStartHighBound",startSumHighBound_);
          inCol->parameters().setValues(name()+"_2DNormStartLowBound",startNormLowBound_);
          inCol->parameters().setValues(name()+"_2DNormStartHighBound",startNormHighBound_);
        }


        /* Append center of gravity result and position vectors to .slcio collection.
         * To avoid problems with the RootTreeWriter, all event parameters
         * are written even when these are empty.
         */
        FloatVec rCogSum_  = rCogSum.getVector();
        FloatVec lCogSum_  = lCogSum.getVector();
        FloatVec rCogNorm_ = rCogNorm.getVector();
        FloatVec lCogNorm_ = lCogNorm.getVector();

        FloatVec cogSum_;
        FloatVec cogNorm_;
        fillLinearFloatVec(cogSum,  cogSum_);
        fillLinearFloatVec(cogNorm, cogNorm_);

        inCol->parameters().setValues(name()+"_radialEnergyCog",rCogSum_);
        inCol->parameters().setValues(name()+"_radialNormCog",rCogNorm_);
        inCol->parameters().setValues(name()+"_radialPositionCog",_rPos);

        inCol->parameters().setValues(name()+"_longitudinalEnergyCog",lCogSum_);
        inCol->parameters().setValues(name()+"_longitudinalNormCog",lCogNorm_);
        inCol->parameters().setValues(name()+"_longitudinalPositionCog",_lPos);

        inCol->parameters().setValues(name()+"_2DEnergyCog",cogSum_);
        inCol->parameters().setValues(name()+"_2DNormCog",cogNorm_);
        inCol->parameters().setValues(name()+"_2DradialPositionCog",_2DrPos);
        inCol->parameters().setValues(name()+"_2DlongitudinalPositionCog",_2DlPos);

      }
      catch ( DataNotAvailableException err ) {
        streamlog_out(WARNING) <<  "WARNING: Collection "<< _caloColName
                       << " not available in event "<< evt->getEventNumber() << std::endl;
      }


    /* RAW SIMULATION
     */
    if ( parameterSet("AhcSimCollection") && _simCaloColName != "" )
      try {
        inCol = evt->getCollection( _simCaloColName );

        lcio::LCTypedVector<lcio::SimCalorimeterHit> col(inCol);

        /* create binned vectors to collect energy depositions from this event
         */
        BinnedVector<float,float> rGlobalSum(_nRBins,0.,_rMax,0.);
        BinnedVector<float,float> lGlobalSum(_nZBins,_zMin,_zMax,0.);
        BinnedVector<float,float> rStartSum(_nRBins,0.,_rMax,0.);
        BinnedVector<float,float> lStartSum(_nZBins,_zMinStart,_zMaxStart,0.);
        BinnedVector<float, BinnedVector<float,float> > globalSum(_nZBins,_zMin,_zMax, rGlobalSum );
        BinnedVector<float, BinnedVector<float,float> > startSum(_nZBins,_zMinStart,_zMaxStart, rStartSum );

        /* loop over all hits
         */
        for (lcio::LCTypedVector<lcio::SimCalorimeterHit>::iterator iter=col.begin(); iter != col.end(); ++iter) {

          float energy = (*iter)->getEnergy() * _simEnergyScaleFactor;
          if (energy >= _threshold) {

            const float* position = (*iter)->getPosition();

            if (_suppressOutsideFirstAndLastLayerCenter && ( position[2] < _zMinDetectorAcceptance || position[2] > _zMaxDetectorAcceptance ) ) continue;

            float X        = position[0];
            float Y        = position[1];
            float Z        = position[2];

            float R        = sqrt( X*X + Y*Y );

            streamlog_out(DEBUG) << "simulation: r " << R << " z " << Z << std::endl;

            rGlobalSum[R]   += energy;
            lGlobalSum[Z]   += energy;
            globalSum[Z][R] += energy;

            if (goodShowerStart) {

              float XStart   = X - showerStartPos[0];
              float YStart   = Y - showerStartPos[1];
              float ZStart   = Z - showerStartPos[2];
              float RStart   = sqrt( XStart*XStart + YStart*YStart );

              streamlog_out(DEBUG) << "simulation rStart " << RStart << " zStart " << ZStart << std::endl;

              rStartSum[RStart]        += energy;
              lStartSum[ZStart]        += energy;
              startSum[ZStart][RStart] += energy;

            }

          }
        }

        FloatVec rGlobalSum_ = rGlobalSum.getVector();
        FloatVec lGlobalSum_ = lGlobalSum.getVector();
        FloatVec globalSum_;
        fillLinearFloatVec(globalSum, globalSum_);

        inCol->parameters().setValues(name()+"_radialEnergy",rGlobalSum_);
        inCol->parameters().setValues(name()+"_longitudinalEnergy",lGlobalSum_);
        inCol->parameters().setValues(name()+"_2DEnergy",globalSum_);

        FloatVec rStartSum_  = rStartSum.getVector();
        FloatVec lStartSum_  = lStartSum.getVector();
        FloatVec startSum_;
        fillLinearFloatVec(startSum,  startSum_);

        inCol->parameters().setValues(name()+"_radialEnergyStart",rStartSum_);
        inCol->parameters().setValues(name()+"_longitudinalEnergyStart",lStartSum_);
        inCol->parameters().setValues(name()+"_2DEnergyStart",startSum_);

      }
      catch ( DataNotAvailableException err ) {
        streamlog_out(WARNING) <<  "WARNING: Collection "<< _simCaloColName
                       << " not available in event "<< evt->getEventNumber() << std::endl;
      }

    /* end of event */
    _firstEvent = false;

  }

  void CalorimeterProfileProcessor::end() {

  }

  /* create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   */
  CalorimeterProfileProcessor aCalorimeterProfileProcessor;

} // end namespace CALICE
