#ifndef __ExtractConfigurationAverageProcessor_hh__
#define __ExtractConfigurationAverageProcessor_hh__ 1

#include <vector>
#include <map>
#include <iostream>

#include <marlin/Processor.h>
#include <EVENT/LCEvent.h>
#include <lccd.h>

#include <AverageValue.hh>

namespace CALICE{

  /**  Processor to determine the channel-wise average over any type of configuration over an entire run
   *
   *   This processor selects events of a pre-defined type of configuration
   *   (e.g. pedestal or LED block) and determined the channel-wise average 
   *   over all these events utilizing the AverageValue class. The results get
   *   written to screen or stored to LCIO files/LCCD databases in form of
   *   SimpleValue objects. Cells are identified by the CellID of the input 
   *   hits without any further interpretation of the meaning of this number.
   *
   *   @author  Niels.Meyer@desy.de
   *   @date    January 2008
   */
  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  class ExtractConfigurationAverageProcessor : public marlin::Processor {
  
   public:
    
    //  Abbreviation for the map type used to keep the channel-wise information
    typedef std::map< KEY, AverageValue<VALUE> > AverageValueMap_t;

    typedef typename AverageValueMap_t::iterator AVM_iterator;
    typedef typename AverageValueMap_t::const_iterator AVM_const_iterator;

    typedef KEY (OBJECT_TO_AVERAGE::*PMF_KEY)() const;

    typedef VALUE (OBJECT_TO_AVERAGE::*PMF_VALUE)() const;
  
    ExtractConfigurationAverageProcessor(const std::string name, 
                                         PMF_KEY pmf_key,
                                         PMF_VALUE pmf_value);

    ~ExtractConfigurationAverageProcessor();
  
    /** @brief  Implements marlin::Processor::newProcessor()
     */
    ExtractConfigurationAverageProcessor* newProcessor(){ 
      return new ExtractConfigurationAverageProcessor(_name,
                                                      _pmf_key,
                                                      _pmf_value); 
    }
  
    /** @brief  Implements marlin::Processor::init()
     */
    virtual void init(); 

    /** @brief  Implements marlin::Processor::processEvent(LCEvent*)
     */
    virtual void processEvent( LCEvent* evt );
  
    /** @brief  Implements marlin::Processor::processRunHeader(LCRunHeader*)
     */
    virtual void processRunHeader( LCRunHeader* runHeader );

    /** @brief  Implements marlin::Processor::end()
     */
    virtual void end();

    /**  @brief  Print channel wise mean and RMS to output stream
     */
    void print( std::ostream& );

    /**  @brief  Store results in LCIO file to be read in by a SimpleFileHandler
     */
    void writeSimpleFile( LCCollection* );

    /**  @brief  Store results in LCCD database
     */
    void writeDBFolder( LCCollection* );
  
   protected:
    std::string _inColName;
    std::string _outColName;
    std::string _outFileName;
    std::string _dbInit;
    std::string _dbFolder;
    std::string _dbDescription;
    lccd::LCCDTimeStamp _from;
    lccd::LCCDTimeStamp _till;
    int _selectConf;
    AverageValueMap_t _averageObjects;
    const std::string _defaultString;

    LCCollection* getCollection();
  
   private:
  
    PMF_KEY   _pmf_key;
    PMF_VALUE _pmf_value;

    std::string _name;

  }; // class ExtractConfigurationAverageProcessor
  
}; // namespace CALICE

#endif

