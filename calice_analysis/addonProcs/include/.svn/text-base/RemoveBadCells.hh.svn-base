#ifndef REMOVE_BAD_CELLS_H
#define REMOVE_BAD_CELLS_H

// Marlin includes
#include "marlin/Processor.h"

// LCCD includes
#include "lccd/IConditionsChangeListener.hh"

// CALICE includes
#include "AhcMapper.hh"
#include "MappedContainer.hh"
#include "CellQuality.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /** Processor which drops all hits listed in collection of CellIDs for bad cells from hit collection.
   *
   * @author Nils.Feege@desy.de
   * @date Aug 2010
   *
   */

  class RemoveBadCells : public marlin::Processor, public lccd::IConditionsChangeListener  {
  public:
    Processor*  newProcessor() { return new RemoveBadCells ; }

    RemoveBadCells();

    ~RemoveBadCells();

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run);

    /** Called for every event - this is where the real action is taking place. */
    void processEvent( LCEvent * evt );

    /** Called after data processing for clean up.
     */
    void end();

    /* callback function for the condition changes
     */
    virtual void conditionsChanged(LCCollection *col);

  private:

    void updateMapping();
    void updateCellQuality();

    std::string _mappingProcessorName;
    std::string _colName_noCut;
    std::string _colName_rmBad;
    std::string _cellQualityColName;

    const AhcMapper* _mapper;
    unsigned int _mapperVersion;

    LCCollection *_cellQualityCol;
    bool _cellQualityChanged;
    bool _isTCMT;
    std::vector<int> _tcmtDeadVec;

    MappedContainer<CellQuality> *_cellQualityContainer;

  };

}

#endif
