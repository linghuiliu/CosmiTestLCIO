#ifndef _HistogramOutput_hh_
#define _HistogramOutput_hh_

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include <marlin/Processor.h>
#include <LCIOSTLTypes.h>

#ifdef HAVE_PLOTMM
class Display;
#endif

// FIXME: Should go into another namespace (?)
namespace histmgr {

  /** Processor which writes histogram groups to files at the end of the processing.
   * The histogram groups and their destination files are specified by marlin processor
   * parameters. Each histogram group can be written to a seperate file.
   */
  class HistogramOutput : public marlin::Processor
  {

  public:
    Processor*  newProcessor() { return new HistogramOutput ; }

    HistogramOutput();
    
    ~HistogramOutput();
    

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent 
     *  histograms.
     */
    //  void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse. 
     */
    //  void processEvent( LCEvent * evtP );

    void end();

  protected:
    StringVec _fileFolderAssignment;
#ifdef HAVE_PLOTMM
  private:
    int _visualise;
    Display *_display;
#endif
  };


}
#endif
