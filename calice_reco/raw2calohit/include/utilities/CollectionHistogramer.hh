#ifndef _CollectionHistogramer_hh_
#define _CollectionHistogramer_hh_

#include <RtypesSubSet.h>
#include <map>
#include <string>
#include <iostream>
#include <marlin/Processor.h>
#include <Average_t.hh>

namespace CALICE {
  /** Collect the names of all collections which appear in the LCIO stream.
   * Moreover some statistics are gathered about the number of elements 
   * of all the collections.
   */
  class CollectionHistogramer : public marlin::Processor
  {

  public:
    Processor*  newProcessor() { return new CollectionHistogramer ; }

    CollectionHistogramer();
    
    ~CollectionHistogramer() {};
    

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent 
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse. 
     */
    void processEvent( LCEvent * evtP );

    void end();

  protected:

    int _histogramParameters;                /**< Set to !=0 to also histogram event parameters.*/

    enum EParMaps {kCollectionMap,            /**< Normal collections */
		   kTransientCollectionMap,  /**< Transient collections. */
		   kIntParMap,               /**< int parameters. */
		   kFloatParMap,             /**< float parameters. */
		   kStringParMap,            /**< string parameters. */
		   kNParMaps};
    
    std::map<std::string,Average_t> _hist[kNParMaps];   /**< collection, parameter histograms.*/
    static const char *__histogramTypeName[kNParMaps];  /**< Names for the histrogams.*/
  };

}
#endif
