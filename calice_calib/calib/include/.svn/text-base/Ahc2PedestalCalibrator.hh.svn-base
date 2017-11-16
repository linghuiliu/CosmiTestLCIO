#ifndef AHC2PEDESTALCALIBRATOR_HH_
#define AHC2PEDESTALCALIBRATOR_HH_

/** @brief External Pedestal Calibrator Processor for EPT Prototype.

    Detailed description follows here.
    @author E. Brianne, DESY
    @date Nov 2015
*/

#include "marlin/Processor.h"

#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>

#include <TMath.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TString.h>

using namespace std;
using namespace lcio;
using namespace marlin;

namespace CALICE
{
  class Ahc2PedestalCalibrator : public Processor
  {
  
  public :

    virtual Processor*  newProcessor() { return new Ahc2PedestalCalibrator; }

    /** Default constructor.
     */
    Ahc2PedestalCalibrator();

    /** Marlin init() function.
     */
    virtual void init();

    /** Marlin processRunHeader() function.
     */
    virtual void processRunHeader(LCEvent *evt);

    /** Marlin processEvent() function.
     */
    virtual void processEvent(LCEvent *evt);

    /** Marlin check() function.
     */
    virtual void check( LCEvent * evt );

    /** Marlin end() function.
     */
    virtual void end();

    /** Object containing Pedestal histograms for each memory cells
     */
    class PedestalSpectrum;

  protected :

    string _inputColName; /**<Input LCIO Collection name*/

    string _rootfileName; /**<Output Rootfile name*/

    string _pedestalfileName; /**<Output tsv file name*/

    TFile *output_file; /**<Pointer to output rootfile*/

    TH1F *hWidthRatio; /**<Pointer to histogram ratio pedestal width all / memory cell i*/

    TH1F *hOffset; /**<Pointer to histogram memory cell offset distribution*/

    map<int, vector<PedestalSpectrum*> > histos; /**<map for each chip containing 36 histograms*/

  };

  class Ahc2PedestalCalibrator::PedestalSpectrum 
  {
  public:
    
    /** Constructor.
	@param chipID - chipID Chip number
        @param chn - chn Channel number
    */
    PedestalSpectrum(int chipID, int chn);
    
    /** Default destructor.
     */
    virtual ~PedestalSpectrum();
     
    /** Fill function
	@param val - val value to fill in histogram
	@param memCell - memCell corresponding Memory cell
    */
    int Fill(double val, int memCell);

     /** Fit Pedestal function. Fit the pedestal using TSpectrum and using a gauss function for fitting
    */
    int FitPedestals();

    std::vector<double> pedestalOffsets; /**<vector containing the offset of each memory cells compared to the first memory cell*/
    std::vector<double> pedestalPositions; /**<vector containing the position of pedestal for each memory cells*/
    std::vector<double> pedestalWidths; /**<vector containing the width of pedestal for each memory cells */
    double pedestalPositionAll; /**<Pedestal position for all memory cells */
    double pedestalWidthAll; /**<Pedestal width for all memory cells*/

  private:
    TH1F* histoAll; /**<Histogram with pedestal with all memory cells */
    std::vector< TH1F* > _histos; /**<vector of histograms for each memory cells */
  };

}

#endif
