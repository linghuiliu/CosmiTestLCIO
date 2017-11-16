#ifndef _SimpleTriggerConfiguration_hh_
#define _SimpleTriggerConfiguration_hh_

#include <TriggerConfiguration.hh>

namespace CALICE {
  /** Check for specific trigger configurations.
   * The class checks whether the trigger is configured for:
   * <ul>
   *   <li>beam trigger</li>
   *   <li>cosmics trigger</li>
   *   <li>pedestal i.e. random trigger</li>
   * </ul>
   * The class depends on the trigger assignement and configuration conditions data.
   * The configuration conditions data results from the raw-to-LCIO conversion.
   */

  class SimpleTriggerConfiguration : protected TriggerConfiguration
  {
  public:
    SimpleTriggerConfiguration() {};
    //  ~SimpleTriggerConfiguration() {}

    void init(const std::string &configuration_col_name, 
	      const std::string &assignment_col_name,
	      const std::string &beam_trigger_bit_name,
	      const std::string &cosmics_trigger_bit_name,
	      const std::string &pedestal_trigger_bit_name) {
    
      TriggerConfiguration::init(configuration_col_name,assignment_col_name);

      _beamTriggerID     = getTriggerTypeID(beam_trigger_bit_name);
      _cosmicsTriggerID  = getTriggerTypeID(cosmics_trigger_bit_name);
      _pedestalTriggerID = getTriggerTypeID(pedestal_trigger_bit_name);

      TriggerMask::init();
    };

    bool isBeamTrigger() const {
      return triggerConfMatches( _beamTriggerMask, _beamTriggerValue);
    };

    bool isCosmicsTrigger() const {
      return triggerConfMatches( _cosmicsTriggerMask, _cosmicsTriggerValue );
    };

    bool isPedestalTrigger() const {
      return triggerConfMatches( _pedestalTriggerMask, _pedestalTriggerValue );
    };

  
  protected:
    void configurationChanged() {

      // print(std::cout);

      _beamTriggerMask     = getMask( _beamTriggerID );
      _cosmicsTriggerMask  = getMask( _cosmicsTriggerID );
      _pedestalTriggerMask = getMask( _pedestalTriggerID );
      _beamTriggerValue     = getValue( _beamTriggerID );
      _cosmicsTriggerValue  = getValue( _cosmicsTriggerID );
      _pedestalTriggerValue = getValue( _pedestalTriggerID );

    };

  private:
    UInt_t _beamTriggerMask;
    UInt_t _cosmicsTriggerMask;
    UInt_t _pedestalTriggerMask;

    UInt_t _beamTriggerValue;
    UInt_t _cosmicsTriggerValue;
    UInt_t _pedestalTriggerValue;
  
    UInt_t _beamTriggerID;
    UInt_t _cosmicsTriggerID;
    UInt_t _pedestalTriggerID;
  };
}
#endif
