#ifndef CalibrationWriter_h
#define CalibrationWriter_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>

#include <lcio.h>
#include <lccd.h>

#include <LCHcalCalibrationObject.hh>
//#include <ConditionsChangeDelegator.hh>
#include <CaliceHit.hh>
#include <IMPL/LCCollectionVec.h>

using namespace lcio;

namespace CALICE {

typedef std::map<unsigned,lcio::LCCollectionVec*> CalibrationWriteMap;
    
/**
  * Class which handles the module wise storage of conditions data and the access to the 
  * lccd framework (i.e. to the data base)
  * @author S.Schmidt DESY
  * @date Jun 16 2006
  */   
class CalibrationWriter {

public:

 /** Constructor has parameters:
   * @param dbinit access string for the mysql database
   * @param folder path in the data base where the calibration collections should be stored
   * @param description text describing the type of calibration provided by the stored constants
   */ 
  CalibrationWriter(std::string dbinit, std::string folder, std::string description);
  
  ~CalibrationWriter();
  
 /** fills the "memory" with a calibration object for a certain moduleID and cellKey
   * @param moduleID ID of the module "printed" on it
   * @param cellKey number describing a certain cell of a module, calculated from the chip and the channel number 
   * @param Calibration object used to store the calibration data in the data base
   */
  void putCalibration(unsigned moduleID, unsigned cellKey, LCHcalCalibrationObject* Calibration);

 /** fills the "memory" with a calibration object for a certain moduleID and cellKey
   * @param moduleID ID of the module "printed" on it
   * @param chip chip number of the cell
   * @param channel channel number of the cell
   * @param Calibration object used to store the calibration data in the data base
   */
  void putCalibration(unsigned moduleID, unsigned chip, unsigned channel, LCHcalCalibrationObject* Calibration); 
  
 /** writes the "memory" to the given data base and the given folder, extraction time range is set equal
   *   to the validity time range 
   * @param from start timestamp of the validity range of the calibration
   * @param till end timestamp of the validity range of the calibration
   * @param _writeFile write lcio file after filling the data base 
   */
  void flushCalibration(lccd::LCCDTimeStamp from, lccd::LCCDTimeStamp till, bool _writeFile) const;
  
 /** writes the "memory" to the given data base and the given folder
   * @param from start timestamp of the validity range of the calibration
   * @param till end timestamp of the validity range of the calibration
   * @param extractionTime timestamp describing the time range of the sample used to gather the calibration
   * @param _writeFile write lcio file after filling the data base
   */
  void flushCalibration(lccd::LCCDTimeStamp from, lccd::LCCDTimeStamp till, lccd::LCCDTimeStamp extractionTime, bool _writeFile) const;

 /** writes the "memory" to the given data base and the given folder
   * @param from start timestamp of the validity range of the calibration
   * @param till stop timestamp of the validity range of the calibration
   * @param extractionFrom start timestamp of the sample used to gather the calibration
   * @param extractionTill end timestamp of the sample used to gather the calibration
   * @param _writeFile write lcio file after filling the data base
   */
  void flushCalibration(lccd::LCCDTimeStamp from, lccd::LCCDTimeStamp till, lccd::LCCDTimeStamp extractionFrom, lccd::LCCDTimeStamp extractionTill, bool _writeFile) const;


protected:

 /** access string for the mysql database
   */
  std::string _dbinit;
  
 /** folder path in the data base where the calibration collections should be stored
   */   
  std::string _folder;
  
 /** description text describing the type of calibration provided by the stored constants
   */
  std::string _description;
  
private:
  CalibrationWriteMap _writeMap;

};

}

#endif
