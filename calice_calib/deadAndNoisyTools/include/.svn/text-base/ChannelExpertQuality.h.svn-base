#ifndef CHANNEL_EXPERT_QUALITY_H
#define CHANNEL_EXPERT_EXPERT_H

#include <Rtypes.h>

/** A class holing the constants with define the detailed (expert) channel quality flags.
 *  These flags are for detailed analysis in the deadAndNoisyChannels expert package.
 *  They are to be interpreted as a states of a bit field.
 *  Usage:
 *  \code
 *  // compose a quality word, use bitwise or
 *  UInt_t thisChannelQuality = ChannelExpertQuality::highRMS | ChannelExpertQuality::multiPeakNoise;
 *  // thisChannelQuality now is 0xA, bits 1 and 3 set
 *  
 *  // check if a specific bit is set, use bitwise and
 *  if ( thisChannelQuality & ChannelExpertQuality::dead ) {continue;} // skip if dead bit is set
 *  \endcode
 * 
 *  \attention These are not the flags written to the data base! These are defined in CellQuality object
 *  of the userlib, which is the LCGenericObject written to the data base.
 *
 * Explanation of the meaning/ current definitions (as of 12. Dec. 2010)
 * \li \c dead: rms in the PMNoise spectrum is below a certain value
 * \li \c highRMS: rms in the PMNoise spectrum is above a certain value
 * \li \c highNoiseRate: (not implemented yet) channel is above MIP/noise cut in more than 10 % of all events in an noise run
 * \li \c multiPeakNoise: (not implemented yet) PMNoise spectrum has more than one peak
 * \li \c zombie: (not implemented yet) Channel is dead in a certain period and sees signals again later
 */
class ChannelExpertQuality
{
 public:
  static const UInt_t dead           = 0x1; ///< the dead bit: 0x1
  static const UInt_t highRMS        = 0x2; ///< the righ RMS bit: 0x2
  static const UInt_t highNoiseRate  = 0x4; ///< the high noise rate bit: 0x4
  static const UInt_t multiPeakNoise = 0x8; ///< the multi peak noise bit: 0x8
  static const UInt_t zombie         = 0x10;///< the zombie bit: 0x10;
};

#endif //CHANNEL_EXPERT_QUALITY_H
