#ifndef _CollectionSelector_hh_
#define _CollectionSelector_hh_

#include <RtypesSubSet.h>
#include <string>
#include <marlin/Processor.h>

namespace CALICE {
  /** Mark matching collections as transient or remove transient mark.
   * The processor has two Parameters excludePattern, includePattern.
   * The transient flags are changed of all collections matching the 
   * excludePattern or includePattern. The pattern is just a
   * simple string. If it is contained in a collection name the collection
   * is marked transient(excludePattern) or the transient status is 
   * removed(includePattern)
   * @todo the simple string should be replaced by regular expressions.
   */
  class CollectionSelector : public marlin::Processor
  {
  public:
    Processor*  newProcessor() { return new CollectionSelector ; }

    CollectionSelector();
    
    ~CollectionSelector() {};
    

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

    /** Set or remove the transient Mark of a collection.
     * @param evtP pointer to the event in which the collection transient status is altered.
     * @param name name of the collection to be altered
     * @param transient if equals true the collection is marked transient otherwise the transient mark is removed
     */
    static void setTransient(lcio::LCEvent *evtP, const std::string &name, bool transient);

    StringVec _excludePattern;
    StringVec _includePattern;
    StringVec _selectionList;
    StringVec _selectionVariableName;
    IntVec    _selectionValue;
  };
}
#endif
