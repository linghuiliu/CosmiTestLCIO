#include "CellDescriptionGenerator.hh"

#include "EVENT/LCGenericObject.h"

#include "ModuleLocation.hh"
#include "ModuleDescription.hh"
#include "ModuleConnection.hh"

#include "DetectorTransformation.hh"

#include <iostream>
#include <cmath>

// to be able to use streamlog_out like in Processors
#include "marlin/Processor.h"

namespace CALICE {

  void CellDescriptionGenerator::generate(const lcio::LCCollection* modDescriptionCol, const lcio::LCCollection* modConnectionCol, const lcio::LCCollection* modLocationCol, const lcio::LCCollection* detTransformationCol, MappedContainer<CellDescription>* container ) const {

    typedef std::map<int,ModuleLocation*> locationMap_t;
    locationMap_t locationMap;
    typedef std::map<unsigned char,ModuleDescription*> descriptionMap_t;
    descriptionMap_t descriptionMap;

    float angle = 0;
    float xDetector = 0;
    float yDetector = 0;
    float zDetector = 0;

    const float pi = M_PI;

    container->clear();

    if (detTransformationCol->getNumberOfElements() == 1) {
      DetectorTransformation* transformation = new DetectorTransformation( dynamic_cast<lcio::LCGenericObject*>(detTransformationCol->getElementAt(0)) );

      angle =  -1*transformation->getDetectorAngleZX(); // wrong notation of rotation angle in the database
      xDetector = transformation->getDetectorX0();
      yDetector = transformation->getDetectorY0();//wrong meaning of y-detector in the data base
      zDetector = transformation->getDetectorZ0();

      std::cout << "detector transformation record angle: " << angle << "deg position: ("
                << xDetector <<"," << yDetector <<"," << zDetector <<")" << std::endl;
      delete transformation;
    }
    else std::cout << "ERROR: not exactly one detector transformation" << std::endl;

    for (int nDesc=0; nDesc < modDescriptionCol->getNumberOfElements(); ++nDesc) {
      ModuleDescription* description = new ModuleDescription( dynamic_cast<lcio::LCGenericObject*>(modDescriptionCol->getElementAt(nDesc)) );
      descriptionMap[description->getModuleType()] = description;
      std::cout << "module description record " << nDesc << " type: " << (int)(description->getModuleType()) << std::endl;
    }
    for (int nLoc=0; nLoc < modLocationCol->getNumberOfElements(); ++nLoc) {
      ModuleLocation* location = new ModuleLocation( dynamic_cast<lcio::LCGenericObject*>(modLocationCol->getElementAt(nLoc)) );
      locationMap[location->getCellIndexOffset()] = location;
      std::cout << "module location " << nLoc << std::hex <<" index: " << location->getCellIndexOffset() << std::dec << " type " << (int)location->getModuleType()  << std::endl;
      std::cout << "                position: (x,y,z) (" << location->getX() << "," <<  location->getY() << "," <<  location->getZ() << ")" << std::endl;
    }
    for (int nCon=0; nCon < modConnectionCol->getNumberOfElements(); ++nCon) {
      ModuleConnection* connection = new ModuleConnection( dynamic_cast<lcio::LCGenericObject*>(modConnectionCol->getElementAt(nCon)) );


      unsigned char type = connection->getModuleType()+4; //types have different numbering scheme in description and location
      int index = connection->getIndexOfLowerLeftCell();

      int module = connection->getModuleID();

      delete connection;

      float xLocation = locationMap[index]->getX();
      float yLocation = locationMap[index]->getY();
      float zLocation = locationMap[index]->getZ();

      for (unsigned int i=0;i<descriptionMap[type]->getNCells();++i) {

        unsigned int cellID = descriptionMap[type]->getGeometricalCellIndex(i);

        if (cellID != 0) {

          int combinedCellID = index + cellID;


          float xCellInModule=0;
          float yCellInModule=0;
          float zCellInModule=0;

          xCellInModule = descriptionMap[type]->getCellXPos(i);
          yCellInModule = descriptionMap[type]->getCellYPos(i);
          if (descriptionMap[type]->hasCellZPos()) zCellInModule = descriptionMap[type]->getCellZPos(i);
          else std::cout << " no z position information in CellDescription assuming 0 "<< std::endl;


          /*
           * now rotating cells in the module using rotation matrix:
           * @f{eqnarray}{
           * cos(\theta) & 0 & sin(\theta) \\
           * 0 & 1 & 0 \\
           * -sin(\theta) & 0 & cos(\theta)
           * @f}
           */
          float xCellInModuleRotated =      xCellInModule * cos(angle/180.*pi) +             0 + zCellInModule * sin(angle/180.*pi);
          float yCellInModuleRotated =                                      0  + yCellInModule + 0;
          float zCellInModuleRotated = -1 * xCellInModule * sin(angle/180.*pi) +             0 + zCellInModule * cos(angle/180.*pi);


          /*
           * the rotation also changes the coordinates of the locations
           * the z-position is shifted to larger values
           */
          float xLocationStretched = xLocation;
          float yLocationStretched = yLocation;
          float zLocationStretched = zLocation/cos(angle/180.*pi);

          /*
           * the local position is the rotated module at its location
           */
          float xLocal = xLocationStretched + xCellInModuleRotated;
          float yLocal = yLocationStretched + yCellInModuleRotated;
          float zLocal = zLocationStretched + zCellInModuleRotated; // rotation also shiftes the detector slot z position

          /*
           * Currently there is no definition of the displacement of the module in the layer.
           * It is assumed that the displacement is measured in the detector coordinate system and
           * includes alredy an eventual rotation of the module.
           * If the displacement should be measured in reference to the module coordinate system,
           * and additional rotation would be necessary here:
           */
          float x = xLocal + xDetector;
          float y = yLocal + yDetector;
          float z = zLocal + zDetector;

          float sizeX = 0;
          float sizeY = 0;

          if ( descriptionMap[type]->hasCellDimensionsPerCell() ) {
            sizeX = descriptionMap[type]->getIndividualCellWidth(i);
            sizeY = descriptionMap[type]->getIndividualCellHeight(i);
          }
          else {
            sizeX = descriptionMap[type]->getWidth();
            sizeY = descriptionMap[type]->getHeight();
          }


          streamlog_out(DEBUG) << "cellID " << std::hex << combinedCellID << std::dec
                       << " ( I = " << _mapper->getDecoder()->getIFromCellID(combinedCellID) << " , J = " << _mapper->getDecoder()->getJFromCellID(combinedCellID) << " , K = " << _mapper->getDecoder()->getKFromCellID(combinedCellID) << " )"
                       << " in module " << module << std::endl
                       << " position (x,y,z) : (" << x << "," << y << "," << z <<")" << std::endl
                       << " rotation angle   : " << angle << std::endl
                       << " calculated from: " << std::endl
                       << "  module type                 : " << (int)type << std::endl
                       << "   cell index                 : " << std::hex << cellID << std::dec
                       << " ( I = " << _mapper->getDecoder()->getIFromCellID(cellID) << " , J = " << _mapper->getDecoder()->getJFromCellID(cellID) << " , K = " << _mapper->getDecoder()->getKFromCellID(cellID) << " )" << std::endl
                       << "   position in module         : (" << xCellInModule << "," << yCellInModule << "," << zCellInModule <<")" << std::endl
                       << "   position in rotated module : (" << xCellInModuleRotated << "," << yCellInModuleRotated << "," << zCellInModuleRotated <<")" << std::endl
                       << "  module location : " << std::hex << index << std::dec
                       << " ( I = " << _mapper->getDecoder()->getIFromCellID(index) << " , J = " << _mapper->getDecoder()->getJFromCellID(index) << " , K = " << _mapper->getDecoder()->getKFromCellID(index) << " )" << std::endl
                       << "   position (0deg): (" << xLocation << "," << yLocation << "," << zLocation <<")" << std::endl
                       << "   position (now):  (" << xLocationStretched << "," << yLocationStretched << "," << zLocationStretched <<")" << std::endl
                       << " detector position: (" << xDetector << "," << yDetector << "," << zDetector <<")" << std::endl << std::endl;

          CellDescription *thisPosition = new CellDescription();
          thisPosition->setPosition(x,y,z);
          thisPosition->setSize(sizeX,sizeY);
          thisPosition->setAngle(angle);
          container->fillByCellID(combinedCellID,thisPosition);
        }
      }

    }

    for (locationMap_t::iterator iter = locationMap.begin(); iter != locationMap.end(); ++iter) delete iter->second;
    for (descriptionMap_t::iterator iter = descriptionMap.begin(); iter != descriptionMap.end(); ++iter) delete iter->second;
  }

} //end namespace CALICE
