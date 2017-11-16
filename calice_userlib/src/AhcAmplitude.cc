#include <stdexcept>
#include "AhcAmplitude.hh"

namespace CALICE
{
   /**Constructor from LCObject:
     use LCObject, and do the cast from the LCGenericObject;
     this way, the users do not have to do the cast anymore
   */
  AhcAmplitude::AhcAmplitude(EVENT::LCGenericObject *genericObject)
  {
    /*
      Angela Lucaci-Timoce: the temperature was added in December 2010.
      In order to be compatible with files which were reconstructed before,
      don't check anymore the number of floats, and also
      set the temperature only if there is a temperature defined.
      (to be fixed)
    */

    if (genericObject->getNInt() != kNInts
        //|| genericObject->getNFloat() != kNFloats
	)
      throw std::invalid_argument("AhcAmplitude: suspicious number of elements in this LCGenericObject");
    
    setCellID(genericObject->getIntVal(kCellID));
    setAmplRawADC(genericObject->getFloatVal(kAmplRawADC));
    setAmplRawMinusPedestalADC(genericObject->getFloatVal(kAmplRawMinusPedestalADC));
    setAmplTemperatureCorrMIP(genericObject->getFloatVal(kAmplTemperatureCorrMIP));
    setAmplNOTTemperatureCorrMIP(genericObject->getFloatVal(kAmplNOTTemperatureCorrMIP));
    setAmplGeV(genericObject->getFloatVal(kAmplGeV));

    if (genericObject->getNFloat() == kNFloats)
      setTemperature(genericObject->getFloatVal(kTemperature));
  }

  /**empty constructor
   */
  AhcAmplitude::AhcAmplitude()
  {
    setCellID(0);
    setAmplRawADC(0.);
    setAmplRawMinusPedestalADC(0.);
    setAmplTemperatureCorrMIP(0.);
    setAmplNOTTemperatureCorrMIP(0.);
    setAmplGeV(0.);
    setTemperature(0);
  }

  /**constructor from elements
   */
  AhcAmplitude::AhcAmplitude(int cellID, 
			     float amplRawADC, 
			     float amplRawMinusPedestalADC,
			     float amplTemperatureCorrMIP, 
			     float amplNOTTemperatureCorrMIP, 
			     float amplGeV,
			     float temperature)
  {
    setCellID(cellID);
    setAmplRawADC(amplRawADC);
    setAmplRawMinusPedestalADC(amplRawMinusPedestalADC);
    setAmplTemperatureCorrMIP(amplTemperatureCorrMIP);
    setAmplNOTTemperatureCorrMIP(amplNOTTemperatureCorrMIP);
    setAmplGeV(amplGeV);
    setTemperature(temperature);
  }

  void AhcAmplitude::setCellID(const int cellID)
  {
    setIntVal(kCellID, cellID);
  }

  const int AhcAmplitude::getCellID()
  {
    int cellID = getIntVal(kCellID);
    return cellID;
  }

  /***********************************************/
  void AhcAmplitude::setAmplRawADC(const float amplRawADC)
  {
    setFloatVal(kAmplRawADC, amplRawADC);
  }
  
  const float AhcAmplitude::getAmplRawADC()
  {
    float amplRawADC = getFloatVal(kAmplRawADC);
    return amplRawADC;
  }
   /***********************************************/
  void AhcAmplitude::setAmplRawMinusPedestalADC(const float amplRawMinusPedestalADC)
  {
    setFloatVal(kAmplRawMinusPedestalADC, amplRawMinusPedestalADC);
  }
  
  const float AhcAmplitude::getAmplRawMinusPedestalADC()
  {
    float amplRawMinusPedestalADC = getFloatVal(kAmplRawMinusPedestalADC);
    return amplRawMinusPedestalADC;
  }

  /***********************************************/
  void AhcAmplitude::setAmplTemperatureCorrMIP(const float amplTemperatureCorrMIP)
  {
    setFloatVal(kAmplTemperatureCorrMIP, amplTemperatureCorrMIP);
  }
  
  const float AhcAmplitude::getAmplTemperatureCorrMIP()
  {
    float amplTemperatureCorrMIP = getFloatVal(kAmplTemperatureCorrMIP);
    return amplTemperatureCorrMIP;
  }

  /***********************************************/
  void AhcAmplitude::setAmplNOTTemperatureCorrMIP(const float amplNOTTemperatureCorrMIP)
  {
    setFloatVal(kAmplNOTTemperatureCorrMIP, amplNOTTemperatureCorrMIP);
  }
  
  const float AhcAmplitude::getAmplNOTTemperatureCorrMIP()
  {
    float amplNOTTemperatureCorrMIP = getFloatVal(kAmplNOTTemperatureCorrMIP);
    return amplNOTTemperatureCorrMIP;
  }

  /***********************************************/
  void AhcAmplitude::setAmplGeV(const float amplGeV)
  {
    setFloatVal(kAmplGeV, amplGeV);
  }
  
  const float AhcAmplitude::getAmplGeV()
  {
    float amplGeV = getFloatVal(kAmplGeV);
    return amplGeV;
  }

  /***********************************************/
  void AhcAmplitude::setTemperature(const float temperature)
  {
    setFloatVal(kTemperature, temperature);
  }
  
  const float AhcAmplitude::getTemperature()
  {
    float temperature = getFloatVal(kTemperature);
    return temperature;
  }

  /***********************************************/
  const std::string AhcAmplitude::getTypeName() const
  {
    return "AhcAmplitude";
  }

  const std::string AhcAmplitude::getDataDescription() const
  {
    std::string descriptionString = "i:cellID,f:amplRawADC,f:amplRawMinusPedestalADC,"
      "f:amplTemperatureCorrMIP,f:amplNOTTemperatureCorrMIP,f:amplGeV,f:temperature";
    return descriptionString;
  }



}/*end of namespace CALICE*/
