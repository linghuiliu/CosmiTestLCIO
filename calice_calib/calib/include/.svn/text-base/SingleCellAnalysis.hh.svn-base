#ifndef SingleCellAnalysis_hh
#define SingleCellAnalysis_hh 1

// Marlin includes
#include "marlin/Processor.h"

// LCIO includes
#include "lcio.h"

// c++ includes
#include <string>

// CALICE includes
#include "AhcMapper.hh"
#include "MappedContainer.hh"

// ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"

// other includes
#include "SingleCellStatistics.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /**
   * Processor for analysing cellwise properties like mean signal, RMS, hit frequency, ...
   *
   * The processor creates histograms for each cell summarizing the hit information. In addition, the frequency of hits
   * with respect to the total number of events is determined for each cell.
   *
   * @par processor parameters
   * <table>
   * <tr><td> steering file parameter name </td><td> description </td></tr>
   * <tr><td><b><em> MappingProcessorName </em></b></td><td> Name of the MappingProcessor instance that provides the geometry of the detector. </td></tr>
   * <tr><td><b><em> Hist_ESum_xbins </em></b></td><td> Energy sum histogram: number of bins in x-axis <td></tr>
   * <tr><td><b><em> Hist_ESum_xmin </em></b></td><td> Energy sum histogram: lower limit on x-axis <td></tr>
   * <tr><td><b><em> Hist_ESum_xmax </em></b></td><td> Energy sum histogram: upper limit on x-axis <td></tr>
   * <tr><td><b><em> AHCALcollection </em></b></td><td> Name of the AHCAL Hit collection </td></tr>
   * <tr><td><b><em> OutputFile_txt </em></b></td><td> Name of the .txt output file </td></tr>
   * <tr><td><b><em> OutputFile_root </em></b></td><td> Name of the .root output file containing one histogram per cell </td></tr>
   * <tr><td><b><em>  </em></b></td><td>  </td></tr>
   * <tr><td><b><em>  </em></b></td><td>  </td></tr>
   * <tr><td><b><em>  </em></b></td><td>  </td></tr>
   * </table>
   *
   * @author Nils.Feege@desy.de
   * @date August 2010
   */
  class SingleCellAnalysis : public Processor {
  public:

    SingleCellAnalysis();
    ~SingleCellAnalysis();

    virtual Processor*  newProcessor() { return new SingleCellAnalysis; }

    /* Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /* Called for every event.
     */
    virtual void processEvent(LCEvent *evt) ;

    /* Called after data processing for clean up.
     */
    virtual void end() ;

  private:

    void updateMapping();

    unsigned _nEvents;

    MappedContainer<SingleCellStatistics>* _cellStat;
    MappedContainer<TH1F>* _cellHist;

    std::string _mappingProcessorName;
    std::string _calibLevel;
    std::string _ahcalColName;
    std::string _txtOutputName;
    std::string _rootOutputName;

    int _Hist_ESum_xbins;
    float _Hist_ESum_xmin;
    float _Hist_ESum_xmax;

    const AhcMapper* _mapper;
    unsigned int _mapperVersion;

  };


} // end namespace CALICE


#endif
