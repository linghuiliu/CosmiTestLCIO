#ifndef APPENDMCPARTICLEINFORMATION_hh
#define APPENDMCPARTICLEINFORMATION_hh 1

// LCIO includes
#include "lcio.h"
#include "EVENT/CalorimeterHit.h"

// Marlin includes
#include "marlin/Processor.h"

// CALICE includes
#include "Mapper.hh"

#include "MappedContainer.hh"

#include "CellDescription.hh"

using namespace lcio ;
using namespace marlin ;

namespace CALICE {

  /**  @brief Processor for appending information about the MC-Particle to the event as event parameter
   *
   * @author N. Feege, University of Hamburg
   * @date Dec 2010
   *
   */
  class AppendMCParticleInformation : public Processor {

  public:

    virtual Processor*  newProcessor() { return new AppendMCParticleInformation ; }


    AppendMCParticleInformation() ;

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() ;

    /** Called for every run.
     */
    virtual void processRunHeader( LCRunHeader* run ) ;

    /** Called for every event - the working horse.
     */
    virtual void processEvent( LCEvent * evt ) ;

    virtual void check( LCEvent * evt ) ;

    /** Called after data processing for clean up.
     */
    virtual void end() ;


  protected:
    std::string _colNameIn;

    std::string _cellIDEncoding;

    std::string _mappingProcessorName;
    std::string _cellDescriptionProcessorName;

    int _nRun ;
    int _nEvt ;

    const CALICE::Mapper *_mapper;
    unsigned int _mapperVersion;

    CALICE::MappedContainer<CALICE::CellDescription>* _cellDescriptions;

  } ;

}

#endif
