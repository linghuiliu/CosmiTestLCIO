#ifndef T_CHANNEL_SPECTRUM_PROPERTIES_H
#define T_CHANNEL_SPECTRUM_PROPERTIES_H

#include <TObject.h>

/**
 * The TChannelSpectrumProperties class is used to store the channel information 
 * (slot, frontEnd, module, chip) together with the spectrum properties 
 * (mean, rms) in a TTree.
 */
class TChannelSpectrumProperties: public TObject
{

 public:
  Int_t _module; ///< The readout module
  Int_t _chip; ///< The chip ID on this module
  Int_t _channel; ///< The channel ID within the chip
  Double_t _mean; ///< The mean value of the spectrum
  Double_t _rms; ///< The rms of the spectrum
  
  /// The constructor
  TChannelSpectrumProperties( Int_t module=-1, Int_t chip=-1,   Int_t channel=-1,
			      Double_t mean=0, Double_t rms=0);

  /// Convenience function to set all member variables at once
  void Set(  Int_t module, Int_t chip,   Int_t channel,
	     Double_t mean, Double_t rms);

  // define the class in ROOT
  ClassDef(TChannelSpectrumProperties,1);
};

#endif //T_CHANNEL_SPECTRUM_PROPERTIES_H

