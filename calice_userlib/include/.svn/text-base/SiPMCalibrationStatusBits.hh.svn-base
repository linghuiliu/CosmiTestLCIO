#ifndef SiPMCalibrationStatusBits_hh
#define SiPMCalibrationStatusBits_hh 1

#include "BitSet.hh"

namespace CALICE {


  /**
   * Bit set to describe the SiPM Calibration Status
   *
   * @author Benjamin.Lutz@desy.de
   * @date February 2010
   * @version 1.0
   */
  class SiPMCalibrationStatusBits : public BitSet {
  protected:
    enum ESiPMCalibrationStatusBitNo { kIsDead,                        /*!< this cell is flagged dead */
                                       kNoPedestal,                    /*!< no pedestal value available */
                                       kNoTemperature,                 /*!< no temperature value available */

                                       // bits to show that defaults were used
                                       kMIPConstantIsDefault,          /*!< the default MIP constant was used */
                                       kMIPSlopeIsDefault,             /*!< the default MIP slope was used */
                                       kGainConstantIsDefault,         /*!< the default gain constant was used */
                                       kGainSlopeIsDefault,            /*!< the default gain slope was used */
                                       kInterCalibrationIsDefault,     /*!< the default inter-calibration constant was used */
                                       kSaturationCorrectionIsDefault, /*!< the default saturation correction was used */
                                       kPixelScaleIsDefault,           /*!< the default pixel-scale factor was used */

                                       // bits to show that a value was scaled (e.g. for systematic studies)
                                       kMIPConstantIsScaled,          /*!< the MIP constant was scaled */
                                       kMIPSlopeIsScaled,             /*!< the MIP slope was scaled */
                                       kGainConstantIsScaled,         /*!< the gain constant was scaled */
                                       kGainSlopeIsScaled,            /*!< the gain slope was scaled */
                                       kInterCalibrationIsScaled,     /*!< the inter-calibration constant was scaled */
                                       kSaturationCorrectionIsScaled, /*!< the saturation correction was scaled */
                                       kPixelScaleIsScaled,           /*!< the pixel-scale factor was scaled */

                                       kNumberOfBits}; // just to count the number of bits

  public:
    /**
     * standard constructor
     *
     * all bits are off
     */
    SiPMCalibrationStatusBits() : BitSet(kNumberOfBits) {}

    /**
     * constructor with initialisaton of the the bits
     *
     * @param[in] value bits to be set
     */
    SiPMCalibrationStatusBits(const int value) : BitSet(kNumberOfBits, value) {}


    bool isDead()     const { return getBit(kIsDead    ); }
    bool noPedestal() const { return getBit(kNoPedestal); }
    bool noTemperature() const { return getBit(kNoTemperature); }

    bool MIPConstantIsDefault()          const { return getBit(kMIPConstantIsDefault          ); }
    bool MIPSlopeIsDefault()             const { return getBit(kMIPSlopeIsDefault             ); }
    bool gainConstantIsDefault()         const { return getBit(kGainConstantIsDefault         ); }
    bool gainSlopeIsDefault()            const { return getBit(kGainSlopeIsDefault            ); }
    bool interCalibrationIsDefault()     const { return getBit(kInterCalibrationIsDefault     ); }
    bool saturationCorrectionIsDefault() const { return getBit(kSaturationCorrectionIsDefault ); }
    bool pixelScaleIsDefault()           const { return getBit(kPixelScaleIsDefault           ); }

    bool hasDefault() const { return MIPConstantIsDefault() ||
                                MIPSlopeIsDefault() ||
                                gainConstantIsDefault() ||
                                gainSlopeIsDefault() ||
                                interCalibrationIsDefault() ||
                                saturationCorrectionIsDefault() ||
                                pixelScaleIsDefault(); }


    bool MIPConstantIsScaled()          const { return getBit(kMIPConstantIsScaled          ); }
    bool MIPSlopeIsScaled()             const { return getBit(kMIPSlopeIsScaled             ); }
    bool gainConstantIsScaled()         const { return getBit(kGainConstantIsScaled         ); }
    bool gainSlopeIsScaled()            const { return getBit(kGainSlopeIsScaled            ); }
    bool interCalibrationIsScaled()     const { return getBit(kInterCalibrationIsScaled     ); }
    bool saturationCorrectionIsScaled() const { return getBit(kSaturationCorrectionIsScaled ); }
    bool pixelScaleIsScaled()           const { return getBit(kPixelScaleIsScaled           ); }

    bool hasScaled() const { return MIPConstantIsScaled() ||
                               MIPSlopeIsScaled() ||
                               gainConstantIsScaled() ||
                               gainSlopeIsScaled() ||
                               interCalibrationIsScaled() ||
                               saturationCorrectionIsScaled() ||
                               pixelScaleIsScaled(); }


    void setDead( const bool state = true )                       { setBit(kIsDead    , state); }
    void setNoPedestal( const bool state = true )                 { setBit(kNoPedestal, state); }
    void setNoTemperature( const bool state = true )              { setBit(kNoTemperature, state); }

    void setMIPConstantDefault( const bool state = true )          { setBit(kMIPConstantIsDefault          , state); }
    void setMIPSlopeDefault( const bool state = true )             { setBit(kMIPSlopeIsDefault             , state); }
    void setGainConstantDefault( const bool state = true )         { setBit(kGainConstantIsDefault         , state); }
    void setGainSlopeDefault( const bool state = true )            { setBit(kGainSlopeIsDefault            , state); }
    void setInterCalibrationDefault( const bool state = true )     { setBit(kInterCalibrationIsDefault     , state); }
    void setSaturationCorrectionDefault( const bool state = true ) { setBit(kSaturationCorrectionIsDefault , state); }
    void setPixelScaleDefault( const bool state = true )           { setBit(kPixelScaleIsDefault           , state); }

    void setMIPConstantScaled( const bool state = true )          { setBit(kMIPConstantIsScaled          , state); }
    void setMIPSlopeScaled( const bool state = true )             { setBit(kMIPSlopeIsScaled             , state); }
    void setGainConstantScaled( const bool state = true )         { setBit(kGainConstantIsScaled         , state); }
    void setGainSlopeScaled( const bool state = true )            { setBit(kGainSlopeIsScaled            , state); }
    void setInterCalibrationScaled( const bool state = true )     { setBit(kInterCalibrationIsScaled     , state); }
    void setSaturationCorrectionScaled( const bool state = true ) { setBit(kSaturationCorrectionIsScaled , state); }
    void setPixelScaleScaled( const bool state = true )           { setBit(kPixelScaleIsScaled           , state); }

  };

}
#endif
