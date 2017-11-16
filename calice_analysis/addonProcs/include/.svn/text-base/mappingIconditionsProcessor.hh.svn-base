#ifndef mappingIconditionsProcessor_h 
#define mappingIconditionsProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif
 
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"
#include "marlin/Processor.h"
#include "lccd/IConditionsChangeListener.hh"

#include "AhcVfeConfigurationBlock.hh"
#include "AhcMapper.hh"

namespace CALICE 
{ 
  
  /** A processor which collects and provides the conditions data for the AHCAL modules.
   *
   * @todo
   * - get rid of hardcoded range numbers  
   * - get rid of hardcoded module index generation
   * - use central mapping instead of re-implementing vrawadcprocessor
   *
   * @author B.Lutz
   * @version 2.0
   * @date 2010/06/15
   */
  class mappingIconditionsProcessor : public marlin::Processor, public lccd::IConditionsChangeListener
  {
  public: 
    /**Create a new instance of this processor
     */
    virtual Processor* newProcessor() { return new mappingIconditionsProcessor; }
    
    /**Default constructor
     */
    mappingIconditionsProcessor();
    
    /**Initialize useful variables
     */
    virtual void init();

    /**Process the run header
       @param run the run header to be processed
     */
    virtual void processRunHeader(LCRunHeader* run);
    
    /**Process the event
       @param evt the event to be processed
     */
    virtual void processEvent(LCEvent* evt);
    
    /**End of all events
     */
    virtual void end();
    
   
  private:
    const AhcMapper *_ahcMapper;
    unsigned int _mapperVersion;
    unsigned int _ahcMaxNumberOfModules;  /**<AHCAL maximum number of modules (39)*/
    unsigned int _ahcMaxNumberOfChips;    /**<AHCAL maximum number of chips*/
    unsigned int _ahcMaxNumberOfChannels; /**<AHCAL maximum number of channels*/

    std::string _outputColName;
    std::string _colAhcFeConf;
    std::string _colAhcVfeConf;
    std::string _mappingProcessorName;
    
    bool _ahcFeConfigurationChanged;
    bool _ahcVfeConfigurationChanged;

    bool _feDataAvailable;
    bool _vfeDataAvailable;

    LCCollection *_feCol;
    LCCollection *_vfeCol;

    bool *_foundFeData;
    bool *_foundVfeData;
    int *_calibStart;
    int *_calibWidth;
    bool *_calibEnable;
    int *_hold;
    int *_holdWidth;
    int *_multiplex;
    int *_vcalib;
    int *_verification;
    int **_sr;
    unsigned int *_moduleID;

    

    /**Check if the collection col has changed*/
    void conditionsChanged(LCCollection *col);

    /** The method which is called in case the Ahc FeConfiguration changes */
    void setAhcFeConfigCol(EVENT::LCCollection *feCol);
        
    /** The method which is called in case the Ahc FeConfiguration changes */
    void setAhcVfeConfigCol(EVENT::LCCollection *vfeCol);

   void processVfeForSlot(int crate, int slot, int fe, 
			   AhcVfeConfigurationBlock vfeConfBlock);

   void processVfe(int crate, int slot, int fe, 
		    AhcVfeConfigurationBlock vfeConfBlock);

    void updateMapper();
    
  

  };

}

#endif
