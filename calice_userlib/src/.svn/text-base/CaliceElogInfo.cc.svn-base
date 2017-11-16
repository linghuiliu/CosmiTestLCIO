#include <stdexcept>
#include "CaliceElogInfo.hh"

#include "IMPL/LCCollectionVec.h"
#include "EVENT/LCIO.h"

namespace CALICE
{
  /**Constructor from LCObject:
     use LCObject, and do the cast from the LCGenericObject;
     this way, the users do not have to do the cast anymore
  */
  CaliceElogInfo::CaliceElogInfo( EVENT::LCCollection* col )
  {

    if (col->getNumberOfElements() != 1) {
      throw std::invalid_argument("CaliceElogInfo: this collection does not contain exactly one element");
    }

    EVENT::LCGenericObject *genericObject = dynamic_cast<EVENT::LCGenericObject*>(col->getElementAt(0));
    if (genericObject == NULL)
      throw std::invalid_argument("CaliceElogInfo: this LCObject is not an LCGenericObject");

    if (genericObject->getNInt() != kNInts
        || genericObject->getNFloat() != kNFloats)
      throw std::invalid_argument("CaliceElogInfo: suspicious number of elements in this LCGenericObject");

    setRunNumber(         genericObject->getIntVal(kRunNumber));
    setPdgType(           genericObject->getIntVal(kPdgType));
    setQualityFlag(       genericObject->getIntVal(kQualityFlag));
    setTriggerType(       genericObject->getIntVal(kTriggerType));
    setTriggerSetting(    genericObject->getIntVal(kTriggerSetting));
    setEnergy(            genericObject->getFloatVal(kEnergy));
    setCherenkovPressure( genericObject->getFloatVal(kCherenkovPressure) );
    setCherenkov2Pressure(genericObject->getFloatVal(kCherenkov2Pressure) );
    setPositionX(         genericObject->getFloatVal(kPositionX));
    setPositionY(         genericObject->getFloatVal(kPositionY));
    setRotationAngle(     genericObject->getFloatVal(kRotationAngle));

    _runType = col->getParameters().getStringVal("runType");

  }


  /**constructor from elements
   */
  CaliceElogInfo::CaliceElogInfo(const int unsigned runNumber,
                                 const int pdgType,
                                 const float energy,
                                 const int qualityFlag,
                                 const int triggerType,
                                 const int triggerSetting,
                                 const float cherenkovPressure,
                                 const float cherenkov2Pressure,
                                 const float positionX,
                                 const float positionY,
                                 const float rotationAngle)
  {
    setRunNumber(runNumber);
    setPdgType(pdgType);
    setQualityFlag(qualityFlag);
    setTriggerType(triggerType);
    setTriggerSetting(triggerSetting);
    setEnergy(energy);
    setCherenkovPressure(cherenkovPressure);
    setCherenkov2Pressure(cherenkov2Pressure);
    setPositionX(positionX);
    setPositionY(positionY);
    setRotationAngle(rotationAngle);

  }

  void CaliceElogInfo::setRunNumber(const unsigned int runNumber)
  {
    setIntVal(kRunNumber, runNumber);
  }

  unsigned int CaliceElogInfo::getRunNumber()
  {
    int runNumber = getIntVal(kRunNumber);
    return runNumber;
  }


  void CaliceElogInfo::setPdgType(const int pdgType)
  {
    setIntVal(kPdgType, pdgType);
  }

  int CaliceElogInfo::getPdgType()
  {
    int pdgType = getIntVal(kPdgType);
    return pdgType;
  }


  void CaliceElogInfo::setQualityFlag(const int qualityFlag)
  {
    setIntVal(kQualityFlag, qualityFlag);
  }

  int CaliceElogInfo::getQualityFlag()
  {
    int qualityFlag = getIntVal(kQualityFlag);
    return qualityFlag;
  }


  void CaliceElogInfo::setTriggerType(const int triggerType)
  {
    setIntVal(kTriggerType, triggerType);
  }

  int CaliceElogInfo::getTriggerType()
  {
    int triggerType = getIntVal(kTriggerType);
    return triggerType;
  }

  void CaliceElogInfo::setTriggerSetting(const int triggerSetting)
  {
    setIntVal(kTriggerSetting, triggerSetting);
  }

  int CaliceElogInfo::getTriggerSetting()
  {
    int triggerSetting = getIntVal(kTriggerSetting);
    return triggerSetting;
  }




  void CaliceElogInfo::setEnergy(const float energy)
  {
    setFloatVal(kEnergy, energy);
  }

  float CaliceElogInfo::getEnergy()
  {
    float energy = getFloatVal(kEnergy);
    return energy;
  }


  void CaliceElogInfo::setCherenkovPressure(const float cherenkovPressure)
  {
    setFloatVal(kCherenkovPressure, cherenkovPressure);
  }

  float CaliceElogInfo::getCherenkovPressure()
  {
    float cherenkovPressure = getFloatVal(kCherenkovPressure);
    return cherenkovPressure;
  }

  void CaliceElogInfo::setCherenkov2Pressure(const float cherenkov2Pressure)
  {
    setFloatVal(kCherenkov2Pressure, cherenkov2Pressure);
  }

  float CaliceElogInfo::getCherenkov2Pressure()
  {
    float cherenkov2Pressure = getFloatVal(kCherenkov2Pressure);
    return cherenkov2Pressure;
  }


  void CaliceElogInfo::setPositionX(const float positionX)
  {
    setFloatVal(kPositionX, positionX);
  }

  float CaliceElogInfo::getPositionX()
  {
    float positionX = getFloatVal(kPositionX);
    return positionX;
  }

  void CaliceElogInfo::setPositionY(const float positionY)
  {
    setFloatVal(kPositionY, positionY);
  }

  float CaliceElogInfo::getPositionY()
  {
    float positionY = getFloatVal(kPositionY);
    return positionY;
  }


  void CaliceElogInfo::setRotationAngle(const float rotationAngle)
  {
    setFloatVal(kRotationAngle, rotationAngle);
  }

  float CaliceElogInfo::getRotationAngle()
  {
    float rotationAngle = getFloatVal(kRotationAngle);
    return rotationAngle;
  }


  const std::string CaliceElogInfo::getTypeName() const
  {
    return "CaliceElogInfo";
  }

  const std::string CaliceElogInfo::getDataDescription() const
  {
    std::string descriptionString = "i:runNumber,i:pdgType,i:qualityFlag,"
      "i:triggerType,i:triggerSetting,"
      "f:energy,f:cherenkovPressure,f:cherenkov2Pressure,f:positionX,f:positionY,f:rotationAngle";

    return descriptionString;

  }

  /**************************************************************/
  void CaliceElogInfo::setRunType(const std::string runType)
  {
    _runType = runType;
  }
  const std::string CaliceElogInfo::getRunType()
  {
    return _runType;
  }

  const std::string CaliceElogInfo::getTargetString(const EVENT::LCParameters &params)
  {
    return params.getStringVal("target");
  }

  const std::string CaliceElogInfo::getAbsorberString(const EVENT::LCParameters &params)
  {
    return params.getStringVal("absorber");
  }

  const std::string CaliceElogInfo::getComment(const EVENT::LCParameters &params)
  {
    return params.getStringVal("comment");;
  }


  /**************************************************************/
  EVENT::LCCollection *CaliceElogInfo::getCollection() const
  {
    EVENT::LCCollection *col = new IMPL::LCCollectionVec(EVENT::LCIO::LCGENERICOBJECT);

    col->addElement(const_cast<CaliceElogInfo*>(this));

    EVENT::LCParameters &parameters = col->parameters();
    parameters.setValue("runType", _runType);

    return col;
  }

}/*end of namespace CALICE*/



