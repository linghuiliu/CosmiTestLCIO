#ifndef Ahc2CalibrationStatusBits_hh
#define Ahc2CalibrationStatusBits_hh 1

#include "BitSet.hh"

namespace CALICE {


  /**
   * Bit set to describe the Ahc2 Calibration Status
   *
   * @author E.Brianne
   * @date Novemeber 2015
   * @version 1.0
   */
  class Ahc2CalibrationStatusBits : public BitSet {
  protected:
    enum EAhc2CalibrationStatusBitNo { kIsDead,                        /*!< this cell is flagged dead */
                                       kNoPedestal,                    /*!< no pedestal value available */
                                       kNoTemperature,                 /*!< no temperature value available */
				       kisNewITEP,                     /*!< is a new ITEP Tile */

                                       // bits to show that defaults were used
                                       kMIPConstantIsDefault,          /*!< the default MIP constant was used */
                                       kMIPSlopeIsDefault,             /*!< the default MIP slope was used */
                                       kGainConstantIsDefault,         /*!< the default gain constant was used */
                                       kGainSlopeIsDefault,            /*!< the default gain slope was used */
                                       kInterCalibrationIsDefault,     /*!< the default inter-calibration constant was used */
				       kPhysicsCalibICIsDefault,       /*!< the default inter-calibration physics/calib constant was used */
                                       kSaturationParametersIsDefault, /*!< the default saturation parameters was used */
				       kTimeSlopesParametersIsDefault, /*!< the default time slopes parameters was used */
				       kTimePedestalParametersIsDefault, /*!< the default time pedestal parameters was used */

                                       // bits to show that a value was scaled (e.g. for systematic studies)
                                       kMIPConstantIsScaled,          /*!< the MIP constant was scaled */
                                       kMIPSlopeIsScaled,             /*!< the MIP slope was scaled */
                                       kGainConstantIsScaled,         /*!< the gain constant was scaled */
                                       kGainSlopeIsScaled,            /*!< the gain slope was scaled */
                                       kInterCalibrationIsScaled,     /*!< the inter-calibration constant was scaled */
				       kPhysicsCalibICIsScaled,       /*!< the inter-calibration physics/calib constant was scaled */

                                       kNumberOfBits}; // just to count the number of bits

  public:
    /**
     * standard constructor
     *
     * all bits are off
     */
    Ahc2CalibrationStatusBits() : BitSet(kNumberOfBits) {}

    /**
     * constructor with initialisaton of the the bits
     *
     * @param[in] value bits to be set
     */
    Ahc2CalibrationStatusBits(const int value) : BitSet(kNumberOfBits, value) {}


    bool isDead()     const { return getBit(kIsDead    ); }
    bool noPedestal() const { return getBit(kNoPedestal); }
    bool noTemperature() const { return getBit(kNoTemperature); }
    bool isNewITEP() const { return getBit(kisNewITEP); }

    bool MIPConstantIsDefault()          const { return getBit(kMIPConstantIsDefault          ); }
    bool MIPSlopeIsDefault()             const { return getBit(kMIPSlopeIsDefault             ); }
    bool gainConstantIsDefault()         const { return getBit(kGainConstantIsDefault         ); }
    bool gainSlopeIsDefault()            const { return getBit(kGainSlopeIsDefault            ); }
    bool interCalibrationIsDefault()     const { return getBit(kInterCalibrationIsDefault     ); }
    bool PhysicsCalibICIsDefault()     const { return getBit(kPhysicsCalibICIsDefault     ); }

    bool saturationParametersIsDefault() const { return getBit(kSaturationParametersIsDefault ); }
    bool TimeSlopesParametersIsDefault() const { return getBit(kTimeSlopesParametersIsDefault ); }
    bool TimePedestalParametersIsDefault() const { return getBit(kTimePedestalParametersIsDefault ); }

    bool hasDefault() const { return MIPConstantIsDefault() ||
                                MIPSlopeIsDefault() ||
                                gainConstantIsDefault() ||
                                gainSlopeIsDefault() ||
                                interCalibrationIsDefault() ||
	                        PhysicsCalibICIsDefault() ||
                                saturationParametersIsDefault() ||
	                        TimeSlopesParametersIsDefault() ||
	                        TimePedestalParametersIsDefault(); }


    bool MIPConstantIsScaled()          const { return getBit(kMIPConstantIsScaled          ); }
    bool MIPSlopeIsScaled()             const { return getBit(kMIPSlopeIsScaled             ); }
    bool gainConstantIsScaled()         const { return getBit(kGainConstantIsScaled         ); }
    bool gainSlopeIsScaled()            const { return getBit(kGainSlopeIsScaled            ); }
    bool interCalibrationIsScaled()     const { return getBit(kInterCalibrationIsScaled     ); }
    bool PhysicsCalibICIsScaled()     const { return getBit(kPhysicsCalibICIsScaled     ); }

    bool hasScaled() const { return MIPConstantIsScaled() ||
                               MIPSlopeIsScaled() ||
                               gainConstantIsScaled() ||
                               gainSlopeIsScaled() ||
	                       interCalibrationIsScaled() ||
	                       PhysicsCalibICIsScaled(); }


    void setDead( const bool state = true )                       { setBit(kIsDead    , state); }
    void setNoPedestal( const bool state = true )                 { setBit(kNoPedestal, state); }
    void setNoTemperature( const bool state = true )              { setBit(kNoTemperature, state); }
    void setisNewITEP(const bool state = true )                   { setBit(kisNewITEP, state); }

    void setMIPConstantDefault( const bool state = true )          { setBit(kMIPConstantIsDefault          , state); }
    void setMIPSlopeDefault( const bool state = true )             { setBit(kMIPSlopeIsDefault             , state); }
    void setGainConstantDefault( const bool state = true )         { setBit(kGainConstantIsDefault         , state); }
    void setGainSlopeDefault( const bool state = true )            { setBit(kGainSlopeIsDefault            , state); }
    void setInterCalibrationDefault( const bool state = true )     { setBit(kInterCalibrationIsDefault     , state); }
    void setPhysicsCalibICDefault( const bool state = true )       { setBit(kPhysicsCalibICIsDefault       , state); }
    void setSaturationParametersDefault( const bool state = true ) { setBit(kSaturationParametersIsDefault , state); }
    void setTimeSlopesParametersDefault( const bool state = true ) { setBit(kTimeSlopesParametersIsDefault , state); }
    void setTimePedestalParametersDefault( const bool state = true ) { setBit(kTimePedestalParametersIsDefault , state); }

    void setMIPConstantScaled( const bool state = true )          { setBit(kMIPConstantIsScaled          , state); }
    void setMIPSlopeScaled( const bool state = true )             { setBit(kMIPSlopeIsScaled             , state); }
    void setGainConstantScaled( const bool state = true )         { setBit(kGainConstantIsScaled         , state); }
    void setGainSlopeScaled( const bool state = true )            { setBit(kGainSlopeIsScaled            , state); }
    void setInterCalibrationScaled( const bool state = true )     { setBit(kInterCalibrationIsScaled     , state); }
    void setPhysicsCalibICScaled( const bool state = true )       { setBit(kPhysicsCalibICIsScaled       , state); }

  };

}
#endif
