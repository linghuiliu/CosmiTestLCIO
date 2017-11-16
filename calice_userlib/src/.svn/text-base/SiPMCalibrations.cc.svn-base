#include "SiPMCalibrations.hh"

namespace CALICE {

  SiPMCalibrations::SiPMCalibrations(){
    setMIP(NULL);
    setGain(NULL);
    setPedestal(NULL);
    setInterCalibration(NULL);
    setTemperature(NULL);
    setSaturationCorrection(NULL);
    setStatus(0);
    setCellID(0);
    setCellIDEncoding("");
  }

  SiPMCalibrations::SiPMCalibrations(LinearFitCompound* MIP,
                                     LinearFitCompound* gain,
                                     SimpleValue* pedestal,
                                     SimpleValue* interCalibration,
				     SimpleValue* temperature,
                                     SatCorrFunction* saturationCorrectionFunction,
                                     const int status,
                                     const int cellID,
                                     const std::string& cellIDEncoding) {

    setMIP                 (MIP);
    setGain                (gain);
    setPedestal            (pedestal);
    setInterCalibration    (interCalibration);
    setTemperature         (temperature);
    setSaturationCorrection(saturationCorrectionFunction);
    setStatus              (status);
    setCellID              (cellID);
    setCellIDEncoding      (cellIDEncoding);
  }

  SiPMCalibrations::~SiPMCalibrations()
  {
    delete _gain;
    delete _MIP;
    delete _pedestal;
    delete _interCalibration;
    delete _temperature;
    delete _saturationCorrectionFunction;

  }

  void  SiPMCalibrations::setMIP(LinearFitCompound* MIP) {
    _MIP = MIP;
  }

  void SiPMCalibrations::setGain(LinearFitCompound* gain) {
    _gain = gain;
  }

  void SiPMCalibrations::setPedestal(SimpleValue* pedestal) {
    _pedestal = pedestal;
  }

  void SiPMCalibrations::setInterCalibration(SimpleValue* interCalibration) {
    _interCalibration = interCalibration;
  }

  void SiPMCalibrations::setTemperature(SimpleValue* temperature) {
    _temperature = temperature;
  }

  void SiPMCalibrations::setSaturationCorrection(SatCorrFunction* saturationCorrectionFunction) {
    _saturationCorrectionFunction = saturationCorrectionFunction;
  }

  void SiPMCalibrations::setStatus(const int status) {
    _status = status;
  }

  void SiPMCalibrations::setCellID(const int cellID) {
    _cellID = cellID;
  }

  void SiPMCalibrations::setCellIDEncoding(const std::string& encoding) {
    _cellIDEncoding = encoding;
  }

} // end namespace CALICE

