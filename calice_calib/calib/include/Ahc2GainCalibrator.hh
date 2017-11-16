#ifndef AHC2GAINCALIBRATOR_HH_
#define AHC2GAINCALIBRATOR_HH_

/** @brief External Gain Calibrator Processor for EPT Prototype.

    Detailed description follows here.
    @author S. Krause, Mainz
    @date Jan 2016
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

#include <TSystemDirectory.h>
#include <TList.h>
#include <TSystem.h>

#include <TH1I.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TVirtualFFT.h>

#include <TApplication.h>
#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>
#include <TVector.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TLegend.h>
#include <TText.h>
#include <TPaveText.h>
#include <unistd.h>

#include <TStyle.h>
#include <TRint.h>
#include <cmath>

using namespace std;
using namespace lcio;
using namespace marlin;

namespace CALICE
{
  double MultiGaus(double *x, double *par);

  class Ahc2GainCalibrator : public Processor
  {
  
  public :

    virtual Processor*  newProcessor() { return new Ahc2GainCalibrator; }

    /** Default constructor.
     */
    Ahc2GainCalibrator();

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

    bool CheckSpectrum(TSpectrum *spec);

    /** Object containing Gain histograms for each memory cells
     */
    class CalibSpectrum;
    class PedestalCalibrator;

  protected :

    PedestalCalibrator *pedestal;

    std::string _inputColName; /**<Input LCIO Collection name*/

    std::string _rootfileName; /**<Output Rootfile name*/

    std::string _gainfileName; /**<Output tsv file name*/

    std::string _inputMemOffsetName; /**<Path to the Memcell offset values*/

    bool _isHGLG; /**<Files run with ADC HG-LG?*/

    bool _ToCheckSpectrum;
		
    bool _ToCheckMean;

    bool _takePedestal; /**<take pedestal memory cell offset*/

    std::string _vCalib; /**<LED calib voltage in mV*/

    float b_chi2, b_chip, b_chan, b_Vcalib;

    TFile *output_file; /**<Pointer to output rootfile*/

    TFile *fTest;
    TTree *tTest;
    TFile *fHighSignal;

    TH1D *hChiSquare;	/**<Pointer to histogram ChiSquare all */
    TH1D *hFitOverFFT;
    TH2D *hFit2FFT;

    map<int, std::vector<CalibSpectrum*> > histos; /**<map for each chip containing 36 histograms*/

  };
  
  class Ahc2GainCalibrator::CalibSpectrum 
  {
  public:
    CalibSpectrum(int chipID, int chn, int vCalib = 0);

    virtual ~CalibSpectrum();

    void Fill(double val);
	
    double FitGainMultiGauss();

    std::pair<double,double> FitGain();

    double FFTGain();

    float* GainFromFFT(TH1* hist);

    void FindPeak(TH1* hist, Float_t *res);

    TF1* MultiPFit(TH1 *peaks, int nOfpeaks, float res, float sigmapeak, float threshold);

    double GetMean() {return histoAll->GetMean();}

    TH1D* GetHisto() {return histoAll;}

    double GetChi2() {return ChiSquareNDF;}

  private:
    double ChiSquareNDF;
    TH1D* histoAll;
  };


  class Ahc2GainCalibrator::PedestalCalibrator 
  {
  public:
    PedestalCalibrator();

    virtual ~PedestalCalibrator();

    void FillPedestal();

    void setPath(std::string PATH);

    double getOffset(int chip, int chn, int memCell);

    double getWidthTotal(int chip, int chn);

    double getPositionTotal(int chip, int chn);

  private:
    std::string path;

    std::map<int, std::vector< std::vector<double> > > pedOffsets;
    std::map<int, std::vector<double> > pedPossAll;
    std::map<int, std::vector<double> > pedWidthsAll;
  };

}

#endif
