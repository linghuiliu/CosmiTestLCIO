#ifndef ConvertAmpl2CaloHitProcessor_h
#define ConvertAmpl2CaloHitProcessor_h 1

#include "lcio.h"
#include "marlin/Processor.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "UTIL/LCRelationNavigator.h"

#include "AhcAmplitude.hh"


namespace CALICE
{

  /** Processor which converts an AhcAmplitudeRelation collection to a CalorimeterHits collection.
   *
   * @author nils.feege@desy.de
   * @date January 2011
   */
  class ConvertAmpl2CaloHitProcessor: public marlin::Processor
  {
  public:
    /**Return a new instance of this processor
     */
    ConvertAmpl2CaloHitProcessor *newProcessor()
    {
      return new ConvertAmpl2CaloHitProcessor;
    }

    /**Default constructor
     */
    ConvertAmpl2CaloHitProcessor();

    /**Destructor
     */
    ~ConvertAmpl2CaloHitProcessor(){};

    /**Initialise variables, if needed
     */
    virtual void init();

    /**Process event (the working horse)
       @param evt event to be processed
    */
    virtual void processEvent(LCEvent *evt);

    /**Function called after all events have been processed, for cleanup
     */
    virtual void end();

  protected:
    std::string _ahcHitColName;         /**<name of the input AHC hit collection*/
    std::string _ahcAmplColName;        /**<name of the input AHC amplitude collection*/
    std::string _hitAmplRelationColName;      /**<name of the LCRelation collection between CalorimeterHit and AhcAmplitude*/

    std::string _ahcHitOutputV2ColName;         /**<name of the output AHC hit version 2 collection*/

    std::string _amplName;         /**<name of the new amplitude used for the output hit collection*/

  };/*end of class ConvertAmpl2CaloHitProcessor*/

}/*end of namespace CALICE*/
#endif
