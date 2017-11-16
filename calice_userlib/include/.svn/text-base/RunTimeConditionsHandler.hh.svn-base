#ifndef RunTimeConditionsHandler_h
#define RunTimeConditionsHandler_h

//#include "lccd.h"
#include "lccd/ConditionsHandlerBase.hh"

using namespace lcio ;
using namespace lccd ;

namespace CALICE {
  
  /** Implementation of ConditionsHandlerBase that can be used to propagate 
   *  conditions data that were generated at runtime. This is necessary
   *  if the output of a processor should trigger a ConditionsChangeListener
   *  The data will get an infinite validity time intervall assigned.<br>
   * 
   *  @author Benjamin.Lutz@desy.de
   *  @date Dec 2008
   *  @version 1.0
   */
  
  class RunTimeConditionsHandler : public ConditionsHandlerBase {
    
  public:
    
    /** Default constructor. You have to specify the following parameters:
     * 
     * @param name:  name of this conditions data set - used, e.g. as collection name 
     * in update(LCEvent* evt)
     * 
     */
    RunTimeConditionsHandler( const std::string& name) ;

    
    virtual ~RunTimeConditionsHandler() ;
    
    /** 
     *  
     */
    virtual void update( LCCDTimeStamp timestamp ) ;
    
    virtual void update( LCCollection *col);

    /** Register a pointer to a default collection which will be passed to the
     *  IConditionsChangeListener if no data can be found for a given time stamp
     */
    virtual void registerDefaultCollection( LCCollection* col);   


    /** The default collection registered with the handler 
     */
    virtual LCCollection* defaultCollection()
    {
      return _defaultCol;
    }

    /** The last valid collection held by the handler 
     */
    virtual LCCollection* lastValidCollection()
    {
      return _lastValidCollection;
    }

  protected :
    

    RunTimeConditionsHandler() {} 
    virtual void updateEvent( LCEvent* evt );

  private:
    LCEvent *_evt;
    bool _missedAddEvent;

    LCCollection *_defaultCol;/**<the default (empty) collection*/
    LCCollection *_lastValidCollection; /**<the last valid collection*/

  };
}  /*end namespace*/

#endif /* RunTimeConditionsHandler_h*/
