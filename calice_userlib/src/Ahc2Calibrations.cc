#include "Ahc2Calibrations.hh"

namespace CALICE {

  Ahc2Calibrations::Ahc2Calibrations(){
    setMIP(NULL);
    setGain(NULL);
    setPedestal(NULL);
    setInterCalibration(NULL);
    setPhysicsCalibIC(NULL);
    setTemperature(NULL);
    setSaturation(NULL);
    setTimeSlopes(NULL);
    setTimePedestal(NULL);
    setStatus(0);
    setCellID(0);
    setCellIDEncoding("");
  }

  Ahc2Calibrations::Ahc2Calibrations(LinearFitCompound* MIP,
                                     LinearFitCompound* gain,
				     SimpleValue* pedestal,
                                     SimpleValue* interCalibration,
				     SimpleValue* interCalibrationPhysicsCalib,
				     SimpleValue* temperature,
				     SaturationParameters* saturation,
				     SimpleValueVector *timeSlopesCalibration,
				     SimpleValueVector *timePedestalCalibration,
                                     const int status,
                                     const int cellID,
                                     const std::string& cellIDEncoding) {

    setMIP                 (MIP);
    setGain                (gain);
    setPedestal            (pedestal);
    setInterCalibration    (interCalibration);
    setPhysicsCalibIC      (interCalibrationPhysicsCalib);
    setTemperature         (temperature);
    setSaturation          (saturation);
    setTimeSlopes          (timeSlopesCalibration);
    setTimePedestal        (timePedestalCalibration);
    setStatus              (status);
    setCellID              (cellID);
    setCellIDEncoding      (cellIDEncoding);
  }

  Ahc2Calibrations::~Ahc2Calibrations()
  {
    delete _gain;
    delete _MIP;
    delete _pedestal;
    delete _interCalibration;
    delete _interCalibrationPhysicsCalib;
    delete _temperature;
    delete _saturation;
    delete _timeSlopesCalibration;
    delete _timePedestalCalibration;
  }

  void Ahc2Calibrations::setMIP(LinearFitCompound* MIP) {
    _MIP = MIP;
  }

  void Ahc2Calibrations::setGain(LinearFitCompound* gain) {
    _gain = gain;
  }

  void Ahc2Calibrations::setPedestal(SimpleValue* pedestal) {
    _pedestal = pedestal;
  }

  void Ahc2Calibrations::setInterCalibration(SimpleValue* interCalibration) {
    _interCalibration = interCalibration;
  }

  void Ahc2Calibrations::setPhysicsCalibIC(SimpleValue* interCalibrationPhysicsCalib) {
    _interCalibrationPhysicsCalib = interCalibrationPhysicsCalib;
  }

  void Ahc2Calibrations::setTemperature(SimpleValue* temperature) {
    _temperature = temperature;
  }

  void Ahc2Calibrations::setSaturation(SaturationParameters* param) {
    _saturation = param;
  }

  void Ahc2Calibrations::setTimeSlopes(SimpleValueVector* timeSlopes) {
    _timeSlopesCalibration = timeSlopes;
  }

  void Ahc2Calibrations::setTimePedestal(SimpleValueVector* timePedestal) {
    _timePedestalCalibration = timePedestal;
  }


  void Ahc2Calibrations::setStatus(const int status) {
    _status = status;
  }

  void Ahc2Calibrations::setCellID(const int cellID) {
    _cellID = cellID;
  }

  void Ahc2Calibrations::setCellIDEncoding(const std::string& encoding) {
    _cellIDEncoding = encoding;
  }

} // end namespace CALICE

