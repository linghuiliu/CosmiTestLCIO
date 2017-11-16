#ifndef SiPMCalibrations_hh
#define SiPMCalibrations_hh

/* CALICE includes*/
#include "LinearFitCompound.hh"
#include "SimpleValue.hh"
#include "SatCorrItep.hh"
#include "CellDescription.hh"

namespace CALICE {

  /**
   * storing all informations necessary to calibrate a SiPM read channel
   *
   * This class is meant as central holder of all necessary informations to calibrate a SiPM.
   * Currently, following informations are stored:
   * \li \c MIP calibration in form of a \b LinearFitResult
   * \li \c gain calibration in form of a \b LinearFitResult
   * \li \c pedestal calibration in form of a \b SimpleValue
   * \li \c inter \c calibration in form of a \b SimpleValue
   * \li \c saturation \c correction \c function in form of a \b SatCorrItep
   * \li \c status in form of an \b integer
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date October 2009
   */
  class SiPMCalibrations {

  public:

    SiPMCalibrations();

    SiPMCalibrations(LinearFitCompound* MIP,
                     LinearFitCompound* gain,
                     SimpleValue* pedestal,
                     SimpleValue* interCalibration,
                     SimpleValue* temperature,
                     SatCorrFunction* saturationCorrectionFunction,
                     const int status,
                     const int cellID,
                     const std::string& cellIDEncoding);
    ~SiPMCalibrations();

    LinearFitCompound*   getMIP()                  const {return _MIP;}
    LinearFitCompound*   getGain()                 const {return _gain;}

    SimpleValue*       getPedestal()             const {return _pedestal;}
    SimpleValue*       getInterCalibration()     const {return _interCalibration;}
    SimpleValue*       getTemperature()          const {return _temperature;}

    SatCorrFunction*   getSaturationCorrection() const {return _saturationCorrectionFunction;}

    int                getStatus()               const {return _status;}

    int                getCellID()               const {return _cellID;}

    const std::string& getCellIDEncoding()       const {return _cellIDEncoding;}

    void setMIP                 (LinearFitCompound* MIP );
    void setGain                (LinearFitCompound* gain);

    void setPedestal            (SimpleValue*     pedestal);
    void setInterCalibration    (SimpleValue*     interCalibration);
    void setTemperature         (SimpleValue*     temperature);

    void setSaturationCorrection(SatCorrFunction* function);
    void setStatus              (const int status);

    void setCellID              (const int cellID);
    void setCellIDEncoding      (const std::string& encoding);

  private:
    LinearFitCompound *_gain;
    LinearFitCompound *_MIP;

    SimpleValue     *_pedestal;
    SimpleValue     *_interCalibration;
    SimpleValue     *_temperature;

    SatCorrFunction *_saturationCorrectionFunction;

    int              _status;

    int              _cellID;
    std::string      _cellIDEncoding;
  };

} // end namespace CALICE

#endif
