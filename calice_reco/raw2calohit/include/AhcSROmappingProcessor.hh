#ifndef AHCSROMAPPINGPROCESSOR_hh
#define AHCSROMAPPINGPROCESSOR_hh 1

#include "marlin/Processor.h"
#include "lcio.h"
#include "lccd/IConditionsChangeListener.hh"

#include "AhcSlowReadoutModMapper.hh"
#include "RunTimeConditionsHandler.hh"

using namespace marlin ;

/**  
 * @brief Processor to fix the mapping of a collection of AhcSlowReadoutModBlock
 * 
 * This processor retrieves the data by ConditionsChangeListener for the input- and 
 * mapping-collection. 
 *
 * The output is added to the event by a RunTimeConditionsHandler to
 * propagate the change to possible further ConditionsChangeListener.
 * 
 * @see lccd::IConditionsChangeListener
 * @see CALICE::RunTimeConditionsHandler
 * @see marlin::ConditionsProcessor
 *
 * @author Benjamin.Lutz@desy.de
 * @date Dec 2008
 * @version 1.0
 */
class AhcSROmappingProcessor : public Processor, public lccd::IConditionsChangeListener 
{
public:
  
  virtual Processor*  newProcessor() { return new AhcSROmappingProcessor ; }
  
  AhcSROmappingProcessor() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
    
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
    
  /** Called after data processing for clean up.
   */
  virtual void end(){} ;
  
  
protected:
  std::string _inColName;
  std::string _outColName;
  std::string _mappingColName;


private:
  CALICE::AhcSlowReadoutModMapper _mapper;
  
  void conditionsChanged(lcio::LCCollection *col);

  bool _mappingChanged;
  bool _inputChanged;
  lcio::LCCollection *_mappingCol;
  lcio::LCCollection *_inCol;

  CALICE::RunTimeConditionsHandler *_conditionsHandler;

  bool _mappingAvailable;

  lcio::LCCollection *_lastDataCol;

} ;

#endif

