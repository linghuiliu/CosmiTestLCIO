#ifndef BeamMomentum_hh
#define BeamMomentum_hh 1

#include "BmlSlowRunDataBlock.hh"
#include <sstream>
#include <string>

namespace CALICE {

  /**
   * @brief Class to claculate the beam momentum and momentum spread from beamline slow readout data
   *
   * The beam momentum is linearly approximated from the magnet currents.
   * A Gaussian distribution of the beam momentum spread is assumed.
   *
   * @par CERN
   * The calculations are based on the informations found in the @b "Short Introduction to the use of the H6 beam", Version 3.0, 2 May 2000.
   *
   * @par
   * There are no references found for the current to momentum translation for most of the magnets.
   * The @b Introduction tells only that B(end)5 should be at 566.4A for 120 GeV.
   * Therefore, the slope was fitted on a few runs. For B5 the fit gives the same result as the @b Introduction.
   *
   * @par
   * The @b full @b width beam momentum spread is calculated by the formula
   * @f[ \frac{\Delta p}{p} = \frac{\sqrt{C_{3}^2+C_{8}^2}}{19.4} [\%] @f]
   * where @f$ C_{i} @f$ is the @b full @b width opening of collimator @f$i@f$.
   * This is valid for tertiary beams.
   *
   * @par
   * For secondary beams this might lead to an overestimation of the beam spread, as @f$C_{8}@f$
   * might be further open than the beam size. The @b Introduction suggest to use @f$ C_{8} = @f$ 4mm as upper limit for high energy secondary beams.
   * This class provides limited and unlimited results, as it is not clear to the author when a beam can be considered "high energy"
   * and the destinction between secondary and tertiary beam by target position fails in 2007. The beam is considered secondary, if the beam momentum
   * in the secondary and the tertiary section differ for less than 10 @%. In this case, a warning message is issued.
   *
   * @par
   * The Gaussian width is derived by @f[ \sigma = \frac{\Delta p}{2\sqrt{2\ln{2}}} \approx \frac{\Delta p}{2.35482} @f]
   *
   * @attention The implementation supports only CERN so far
   *
   * @todo implement a constructor for FNAL slow readout data
   *
   * @author Benjamin.Lutz@desy.de
   * @version 1.0
   * @date October 2008
   */
  class BeamMomentum {
  public:
    /**
     * Construcor using CERN data as input
     * @throw BeamMomentumException when number of magnet currents or collimators does not corresond to the CERN beamline
     */
    BeamMomentum( const CALICE::BmlSlowRunDataBlock& cernBeamData );

    /**
     * @return the beam momentum in GeV
     */
    double getMomentum() const;

    /**
     * @return the beam momentum before the secondary target
     */
    double getSecondaryMomentum() const;

    /**
     * @return the beam momentum after the secondary target
     */
    double getTertiaryMomentum() const;

    /**
     * @return relative beam momentum spread
     */
    double getRelativeSpread() const;

    /**
     * @return beam momentum spread in GeV
     */
    double getMomentumSpread() const;

    /**
     * @brief this returns a limited spread for secondary beams, please read the class description
     * @return relative beam momentum spread
     */
    double getLimitedRelativeSpread() const;

    /**
     * @brief this returns a limited spread for secondary beams, please read the class description
     * @return beam momentum spread in GeV
     */
    double getLimitedMomentumSpread() const;

    /**
     * @return warnings that occured during interpretation of beam settings
     */
    const std::string getWarnings() const;

  private:
    double _momentum;
    double _secondaryMomentum;
    double _tertiaryMomentum;
    double _relativeSpread;
    double _limitedRelativeSpread;
    std::stringstream _warningText;

  };


  /**
   * @brief excpetion during beam momentum calculation
   */
  class BeamMomentumException {
  public:
    /**
     * @param what Reason of the exception.
     */
    BeamMomentumException(std::string what) : _type("BeamMomentumException"), _what(what) {
    }

    /**
     * tell what caused the exception
     */
    const std::string what() const {
      return _type+": "+_what;
    }
  protected:
    std::string _type;
    std::string _what;
  };



}
#endif
