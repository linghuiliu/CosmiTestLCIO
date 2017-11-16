#include <ErrorBits.hh>
#include <iomanip>


const char *CALICE::ErrorBits::__name[kNErrorBits]={
  "NoEventData",
  "MissingAdcBlock", 
  "MissingVLinkHeader",
  "InvalidTrigger", 
  "BadConfigData", 
  "WrongTriggerCounter",
  "CorruptEventRecord",
  "MissingTriggerRecord", 
  "CorruptAcquisition",
  "TDCOutOfSynch",
  "DirtyEvent",
  "LargeNegativeSignal",
  "CorruptBmlRecord",
  "CorruptDifTriggerCounter"
};


std::map<std::string, unsigned short> CALICE::ErrorBits::__nameToBit;

/** Translate an error name into a bit.
 */
unsigned int CALICE::ErrorBits::getErrorBit(const std::string &name) {
  if (CALICE::ErrorBits::__nameToBit.empty()) {
    for (unsigned int  bit_i=0; bit_i<kNErrorBits; bit_i++) {
      __nameToBit[__name[bit_i] ] = bit_i;
    }
  }
  std::map<std::string, unsigned short>::const_iterator error_bit= __nameToBit.find(name);
  if (error_bit != __nameToBit.end()) {
    return error_bit->second;
  }
  else {
    return kNErrorBits;
  }

}

unsigned int CALICE::ErrorBits::getErrorBitMask(const std::vector< std::string> &error_names) 
{
    // build error mask
    unsigned int error_mask = 0;
    for(std::vector<std::string>::const_iterator error_iter = error_names.begin();
	error_iter != error_names.end();
	error_iter++) {

      unsigned int error_bit = CALICE::ErrorBits::getErrorBit(*error_iter);

      assert (  error_bit < CALICE::ErrorBits::kNErrorBits  );

      error_mask |= (1<<error_bit);

    }
    return error_mask;
}


std::ostream &CALICE::ErrorBits::print(std::ostream &out) const
{
  if (_errorBits!=0) {
    
    unsigned int mask_i=1;
    out << "Errors: ";
    for (unsigned int  bit_i=0; bit_i<kNErrorBits; bit_i++) {
      if (_errorBits & mask_i) {
	out << getErrorName(bit_i) << " ";
      }
      mask_i <<= 1;
    }
  }
  return out;
}
