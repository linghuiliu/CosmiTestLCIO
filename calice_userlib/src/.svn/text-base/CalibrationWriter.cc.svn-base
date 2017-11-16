#include "CalibrationWriter.hh"

#include <cstdio>

#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>

using namespace lcio;

namespace CALICE {

CalibrationWriter::CalibrationWriter(std::string dbinit, std::string folder, std::string description):
  _dbinit(dbinit), _folder(folder), _description(description)
{

  _writeMap.clear();

}



CalibrationWriter::~CalibrationWriter() {

  for (CalibrationWriteMap::iterator _mapIter=_writeMap.begin(); _mapIter != _writeMap.end(); _mapIter++){
    delete _mapIter->second;
  }  
  _writeMap.clear();
}



void CalibrationWriter::putCalibration(unsigned moduleID, unsigned chip, unsigned channel, LCHcalCalibrationObject* Calibration) {

  putCalibration(moduleID, (chip << 8) + channel, Calibration);
}



void CalibrationWriter::putCalibration(unsigned moduleID, unsigned cellKey, LCHcalCalibrationObject* Calibration) {

  lcio::LCCollectionVec* _col;
//  std::cout << "putCalibration: " << moduleID << " " << cellKey << std::endl;
  CalibrationWriteMap::iterator _mapIter = _writeMap.find(moduleID);
  if (_mapIter == _writeMap.end()) {
//    std::cout << "new collection" << std::endl;
    _col = new lcio::LCCollectionVec(LCIO::LCGENERICOBJECT);
    _col->parameters().setValue("ModuleID",(int) moduleID);
    _writeMap.insert(std::make_pair(moduleID,_col));    
  } else {
//    std::cout << "old collection used" << std::endl;
    _col = _mapIter->second;
  }  
  _col->addElement(Calibration);

}


void CalibrationWriter::flushCalibration(lccd::LCCDTimeStamp from, lccd::LCCDTimeStamp till, bool _writeFile) const {
  
  flushCalibration(from, till, from, till, _writeFile);
}


void CalibrationWriter::flushCalibration(lccd::LCCDTimeStamp from, lccd::LCCDTimeStamp till, lccd::LCCDTimeStamp extractionTime, bool _writeFile) const {
  
  flushCalibration(from, till, extractionTime, extractionTime, _writeFile);
}
  
  
void CalibrationWriter::flushCalibration(lccd::LCCDTimeStamp from, lccd::LCCDTimeStamp till, lccd::LCCDTimeStamp extractionFrom, lccd::LCCDTimeStamp extractionTill, bool _writeFile) const {

  for (CalibrationWriteMap::const_iterator _mapIter = _writeMap.begin(); _mapIter != _writeMap.end(); _mapIter++) {
    unsigned _moduleID = _mapIter->first;
//     std::cout <<"CalibrationWriter: moduleID="<< std::hex << _moduleID << std::dec << std::endl;

    bool tcmtFlag = false;
    if( (_moduleID&0xFFFF)==0 ) tcmtFlag=true;

    lcio::LCCollectionVec* _col = _mapIter->second;
    _col->parameters().setValue("ExtractionTimeFromL",(int)(extractionFrom >> 32));
    _col->parameters().setValue("ExtractionTimeFromH",(int)(extractionFrom & 0x00000000FFFFFFFF));
    _col->parameters().setValue("ExtractionTimeTillL",(int)(extractionTill >> 32));
    _col->parameters().setValue("ExtractionTimeTillH",(int)(extractionTill & 0x00000000FFFFFFFF));
    lccd::DBInterface* _db = NULL;

    char _mod[10];
    sprintf(_mod,"%.2d",(_moduleID >> 8));
    std::string _moduleString(_mod);
    if(!tcmtFlag) {
      // here for Hcal
      int mtype = (_moduleID & 0xFF);
      std::string _moduleExt((mtype%2==0) ? "A" : "B");
      _db = new lccd::DBInterface(_dbinit, _folder+"/ID"+_moduleString+_moduleExt, true);
      _db->storeCollection(from, till, _col, _description+" for module "+_moduleString);
    }
    else {
      // here for TCMT
      _db = new lccd::DBInterface(_dbinit, _folder+_moduleString, true);
      _db->storeCollection(from, till, _col, _description);
    }

    if (_db && _writeFile) {
      _db->createDBFile();
      delete _db;
    }
  }

}
  
}
