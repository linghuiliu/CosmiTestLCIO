#ifndef __CaliceElogInfo_HH__
#define __CaliceElogInfo_HH__

#include "EVENT/LCGenericObject.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "EVENT/LCParameters.h"
#include "EVENT/LCCollection.h"

namespace CALICE{

  /**
   * Implementation of the e-log information
   *
   * @author lucaci@mail.desy.de
   * @date June 2010
   */

  class CaliceElogInfo : public IMPL::LCGenericObjectImpl
  {
  public:
    /**constructor from LCCollection (this way, the user does not need to cast anymore)
     */
    CaliceElogInfo(EVENT::LCCollection* col);

    /**constructor from elements
       @param runNumber run number
       @param pdgType PDG ID of the beam particle (e.g. -211 for pi-)
       @param energy energy of the run, in GeV (according to elog information)
       @param qualityFlag quality Flag: 0 = good, 1 = junk or garbage
       @param triggerType trigger type: 10 for the '10x10' trigger, 20 for the '20x20' trigger,
       100 for the '100x100' trigger
       @param triggerSetting trigger setting (available only for FNAL data, see elog)
       @param cherenkovPressure pressure in cherenkov counter A, in psia
       @param cherenkov2Pressure pressure in cherenkov counter B, in psia
       @param positionX x position of the beam, in mm
       @param positionY y position of the beam, in mm
       @param rotationAngle rotation angle (in degrees)
    */
    CaliceElogInfo(const unsigned int runNumber,
                   const int pdgType,
                   const float energy,
                   const int qualityFlag,
                   const int triggerType,
                   const int triggerSetting,
                   const float cherenkovPressure,
                   const float cherenkov2Pressure,
                   const float positionX,
                   const float positionY,
                   const float rotationAngle);

    /**Set the run number
       @param runNumber the run number to be set
    */
    void setRunNumber(const unsigned int runNumber);
    /**Get the run number
       @return the run number
    */
    unsigned int getRunNumber();

    /**Set the PDG ID
       @param pdgType PDG ID to be set
    */
    void setPdgType(const int pdgType);
    /**Get the PDG ID
       @return the PDG ID
    */
    int getPdgType();

    /**Set the trigger type
       @param triggerType the trigger type:  10 for the '10x10' trigger,
       20 for the '20x20' trigger,
       100 for the '100x100' trigger
    */
    void setTriggerType(const int triggerType);
    /**Get the trigger type
       @return the trigger type (10 for the '10x10' trigger, 20 for the '20x20' trigger,
       100 for the '100x100' trigger)
    */
    int getTriggerType();

    /**Set the trigger setting
       @param triggerSetting
    */
    void setTriggerSetting(const int triggerSetting);
    /**Get the trigger setting
       @return the trigger setting
    */
    int getTriggerSetting();

    /**Set the quality flag (0=good, 1=bad). Please note that this is a general quality flag, based on the
       comments done by the people on shift. If they mentioned that the run was 'bad', 'junk'
       or 'garbage', the quality flag for this run is set to 1 (i.e. bad). This is different from
       the data quality flag set by the DQ bits (for more information, please see here:
       http://www-flc.desy.de/flc/flcwiki/RunLogDataQuality
       @param qualityFlag the quality flag to be set
    */
    void setQualityFlag(const int qualityFlag);
    /**Get the quality flag
       @return get the quality flag
    */
    int getQualityFlag();

    /**Set the energy of this run
       @param energy the energy to be set
    */
    void setEnergy(const float energy);
    /**Get the energy of this run
       @return the energy
    */
    float getEnergy();

    /**Set the pressure in Cherenkov counter A
       @param cherenkovPressure the Cherenkov pressure to be set
    */
    void setCherenkovPressure(const float cherenkovPressure);
    /**Get the pressure in Cherenkov counter A
       @return the Cherenkov pressure
    */
    float getCherenkovPressure();

    /**Set the pressure in Cherenkov counter B
       @param cherenkov2Pressure the Cherenkov pressure to be set
    */
    void setCherenkov2Pressure(const float cherenkov2Pressure);
    /**Get the pressure in Cherenkov counter B
       @return the Cherenkov pressure
    */
    float getCherenkov2Pressure();


    /**Set the x-position of the stage
       @param positionX the x-position of the stage
    */
    void setPositionX(const float positionX);
    /**Get the x-position of the stage
       @return the x-position of the stage
    */
    float getPositionX();

    /**Set the y-position of the stage
       @param positionY the y-position of the stage
    */
    void setPositionY(const float positionY);
    /**Get the y-position of the stage
       @return the y-position of the stage
    */
    float getPositionY();

    /**Set the rotation angle of the stage
       @param rotationAngle the rotation angle
    */
    void setRotationAngle(const float rotationAngle);
    /**Get the rotation angle of the stage
       @return the rotation angle
    */
    float getRotationAngle();

    /**Set the run type (beamData, calibration etc)
       @param runType
    */
    void setRunType(const std::string runType);
    /**Get the run type (beamData, calibration etc)
     */
    const std::string getRunType();

    /**Set the target string
       @param targetString the target string to be set
    */
    void setTargetString(const std::string targetString);
    /**Get the target string
     */
    const std::string getTargetString(const EVENT::LCParameters &params);

    /**Set the absorber string
       @param absorberString the target string to be set
    */
    void setAbsorberString(const std::string absorberString);
    /**Get the absorber string
     */
    const std::string getAbsorberString(const EVENT::LCParameters &params);

    /**Set the comment
       @param comment the comment to be set
    */
    void setComment(const std::string comment);
    /**Get the comment from the e-log
       @param params the LCParameters (since the comment is saved as a parameter
       of the collection of CaliceElogInfo objects)
    */
    const std::string getComment(const EVENT::LCParameters &params);

    /**Implementation of the virtual method from LCGenericObject
       @return type name, i.e. name of the class
    */
    const std::string getTypeName() const;

    /**Implementation of the virtual method from LCGenericObject
       @return data description, i.e description of the LCObject elements:
       "i:runNumber,i:pdgType,i:energy,i:qualityFlag,i:triggerType,i:triggetSetting,"
       "f:cherenkovPressure,f:positionX,f:positionY,f:rotationAngle";
    */
    const std::string getDataDescription() const;

    /**Get the LCCollection
     */
    EVENT::LCCollection* getCollection() const;

  private:
    enum Ints { kRunNumber, kPdgType, kQualityFlag, kTriggerType, kTriggerSetting, kNInts };
    enum Floats { kEnergy, kCherenkovPressure, kCherenkov2Pressure, kPositionX, kPositionY, kRotationAngle, kNFloats };
    enum Doubless { kNDoubles };

    std::string _runType;        /**<run type, e.g. "beamData", noise, etc*/
    std::string _targetString;   /**<target string, e.g. "air"*/
    std::string _absorberString; /**<absorber string, e.g. "6 mm Pb"*/
    std::string _comment;        /**<e-log comment, e.g. "10 deg, 100k"*/

  };


} /*end namespace CALICE*/
#endif
