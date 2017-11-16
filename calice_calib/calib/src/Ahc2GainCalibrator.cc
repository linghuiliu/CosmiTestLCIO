#include <cfloat>
#include <iostream>
#include <string>
#include <stdlib.h>

#include "EVENT/CalorimeterHit.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCCollection.h>

#include "Ahc2GainCalibrator.hh"

#include "marlin/ConditionsProcessor.h"
#include <marlin/Exceptions.h>

#include "LabviewBlock2.hh"

#include <TSystem.h>
#include <TROOT.h>

using namespace std;
using namespace marlin;
using namespace lcio;

namespace CALICE
{


  double MultiGaus(double *x, double *par)
  {
    //multi peak gaussian function
    //sum of npeaks gaussians
    //peak distance is a fit parameter
    double xx = x[0];
    double npeaks = par[0];//number of peaks
    double gain = par[1];//gain
    double p0 = par[2];//first peak position

    double mg=0;
    for(int i=0; i<npeaks; i++)
      {
	mg += par[2*i+3]*TMath::Gaus(xx,gain*i+p0,par[2*i+4]);
      }
    return mg;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                    */
  /***************************************************************************************/

  Ahc2GainCalibrator aAhc2GainCalibrator;

  Ahc2GainCalibrator::Ahc2GainCalibrator():Processor("Ahc2GainCalibrator")
  {
    _description = "Extract Gain from data, fit them and provide a file with Gain value and memory offsets";
    
    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection",
			       _inputColName,
			       std::string(""));

    registerProcessorParameter("OutputRootfileName",
			       "Output path for the rootfiles including gain and spectra",
			       _rootfileName,
			       std::string(""));

    registerProcessorParameter("InputMemcellOffsetValues",
			       "Path to the Memcell offset values",
			       _inputMemOffsetName,
			       std::string(""));

    registerProcessorParameter("isHGLG",
			       "Files run with ADC HG-LG?",
			       _isHGLG,
			       bool(false));

    registerProcessorParameter("vCalib",
			       "LED calib voltage in mV",
			       _vCalib,
			       std::string(""));

    registerProcessorParameter("ToCheckSpectrum",
			       "check spectrum before fitting",
			       _ToCheckSpectrum,
			       bool(true));

    registerProcessorParameter("ToCheckMean",
			       "check mean of spectrum before fitting",
			       _ToCheckMean,
			       bool(true));

    registerProcessorParameter("takePedestal",
			       "take pedestal memory cell offset",
			       _takePedestal,
			       bool(false));
	
	
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                    */
  /***************************************************************************************/

  void Ahc2GainCalibrator::init()
  {
    gSystem->Load("libSpectrum"); //Load TSpectrum
    printParameters();

    output_file = new TFile((_rootfileName+"Plots_comp_"+_vCalib+".root").c_str(),"RECREATE");
	
    fTest = new TFile((_rootfileName+"Chi2info_"+_vCalib+".root").c_str(),"RECREATE");
    tTest = new TTree("Chi2info","Chi2info");
    b_chi2 = 0; 
    b_chip = 0; 
    b_chan = 0; 
    b_Vcalib = 0;
    tTest->Branch("chi2",   &b_chi2);
    tTest->Branch("chip",   &b_chip);
    tTest->Branch("chan",   &b_chan);
    tTest->Branch("Vcalib", &b_Vcalib);

    fHighSignal = new TFile((_rootfileName+"HighSignal_"+_vCalib+".root").c_str(),"RECREATE");

    hChiSquare = new TH1D("hChiSquare",";Chisquare over ndf of SPS fit;entries",100,0,10);
    hFitOverFFT = new TH1D("hFitoverFFT",";Fit gain over FFT gain;entries",200,0.5,1.5);
    hFit2FFT = new TH2D("hFit2FFT",";Fit gain;FFT gain",500,0,50,500,0,50);

    histos.clear(); //clear map

    pedestal = new PedestalCalibrator();

    pedestal->setPath(_inputMemOffsetName);
    pedestal->FillPedestal();
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::processRunHeader(LCEvent *evt) {
    
    streamlog_out(MESSAGE) << "Start to process Run: " << evt->getRunNumber() << endl;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::processEvent(LCEvent *evt)
  {

    /* start loop over input collection */
    streamlog_out(MESSAGE)<<" \n\n=========================Start to process event "<< evt->getEventNumber() <<endl;

    LCCollection *inputCol = NULL;
    try
      {
	inputCol = dynamic_cast<LCCollection*>(evt->getCollection(_inputColName));

	/*----------------------------------------------------------------------*/
	/*                                                                      */
	/*       start loop over input collection                               */
	/*                                                                      */
	/*----------------------------------------------------------------------*/
	int noElem = inputCol->getNumberOfElements();
	streamlog_out(MESSAGE) <<"inputcol " << _inputColName << " has " << noElem << " hits" << endl;

	output_file->cd();

	for (unsigned int i = 0; i < (unsigned int)noElem; i++)
	  {
	    const LabviewBlock2 ldata(inputCol->getElementAt(i));

	    int HitBit = ldata.GetHitBit();
	    int GainBit = ldata.GetGainBit();
	    int EvtNr = ldata.GetEvtNr(); //memcell

	    int ChipID = ldata.GetChipID();
	    int Chn = ldata.GetChannel();

	    int ADC = ldata.GetADC();

	    //new Sascha 12.01.16
	    int TDC = ldata.GetTDC();

	    //#########
	    streamlog_out(DEBUG)<< "ChipID: " << ldata.GetChipID()<< "  Channel: "<< ldata.GetChannel() << endl;
		
	    if (ChipID < 100 || ChipID > 300) continue;

	    if(EvtNr == 0) continue;//Reject first Memory cell (0) in the Chip

	    

	    //generate new histos if necessary:
	    if (histos.count(ChipID) == 0)
	      {
		histos[ChipID] = vector<CalibSpectrum*>(36);

		for (int i=0; i<36; i++)
		  {
		    histos[ChipID][i] = new CalibSpectrum(ChipID, i, atoi(_vCalib.c_str()));
		  }
	    
		streamlog_out(MESSAGE) << "creating ChipID " << ChipID << flush << endl;
	      }
	       
	    int VarToFill = ADC;
	    if (_isHGLG) VarToFill = TDC;

	    if (VarToFill > 0 && GainBit == 1 && HitBit == 1)
	      {
		if (_takePedestal)
		  {
		    histos[ChipID][Chn]->Fill(VarToFill+pedestal->getOffset(ChipID, Chn, EvtNr));//change this because of HGLG files
		  }
		else
		  histos[ChipID][Chn]->Fill(VarToFill);
	      }
	  }
      }
    catch (EVENT::DataNotAvailableException &e)//Case missing collection
      {
	streamlog_out(WARNING)<< "missing collection "
		      << _inputColName <<endl << e.what() <<endl;
	return;
      }
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::check( LCEvent * evt )
  {
    
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::end()
  {
    /*----------------------------------------------------------------------*/
    /*                                                                      */
    /*       Get Gains                                                  */
    /*                                                                      */
    /*----------------------------------------------------------------------*/
    int cntTotal(0), cntFitted(0), cntChn(0);
    //output_file->cd();

    fstream file_out;
    file_out.open(TString::Format("%sgainfits_%s.tsv",_rootfileName.c_str(), _vCalib.c_str()).Data(),ios::out);
    file_out << "#chip" << "\t" << "chn" << "\t" << "gainFit" << "\t" << "gainFFT" << "\t" << "gainErr" << endl;
    
    for (map<int, vector<CalibSpectrum*> >::iterator itChips = histos.begin(); itChips != histos.end(); itChips++)
      {
	cntChn = -1;
	for (vector<CalibSpectrum*>::iterator it = itChips->second.begin(); it!=itChips->second.end(); it++)
	  {
	//if(itChips->first <233 || itChips->first >236){
	//	cout << "chip " << itChips->first << " skipped." << endl;
	//	cntChn++;
	//	continue;
	//}
	    streamlog_out(DEBUG) << "Lan print-out: chip " << itChips->first << " chan " << cntChn << " Vcalib " << _vCalib << endl;
	    streamlog_out(DEBUG) << "Mean(1) of histo " << (*it)->GetMean()
			 << " pedestal.getPositionTotal(itChips->first, cntChn) (has to add 100 too) " 
			 << pedestal->getPositionTotal(itChips->first, cntChn)
			 << endl;

	    cntChn++;//Increment channel

	    //Reject weird chipID
	    if ( (itChips->first < 100) || (itChips->first > 300) )
	 	continue;
	    if((*it)->GetMean() < 10){
		//cntChn++;
	    	continue;
	    }
	    double gainFit(-1), gainErr(-1), gainFFT(0);
	    
	    gainFFT = (*it)->FFTGain();
	    //To check the distance between the peaks, it is better to just look at small number of peaks around the mean
	    TSpectrum * spectrum = new TSpectrum(10,1);//searching for peaks
	    spectrum->Search((*it)->GetHisto(), 5, "", 0.15);
	    Int_t nPeaks = spectrum->GetNPeaks();
	    streamlog_out(DEBUG) << "nPeaks CheckSpectrum = " << nPeaks << endl;
		    
	    //Check if the initial spectrum is okay
	    bool SpectrumIsOK = CheckSpectrum(spectrum);
	    if ( _ToCheckSpectrum && (!SpectrumIsOK) ) 
	      {
		//fHighSignal->cd();
		//((*it)->GetHisto())->Write();
		//cntChn++;
		//delete spectrum;
		
		continue;
	      }
	    //delete spectrum;
	    //To check mean value condition, it is better to see if it is the case of having many peaks or not
	    TSpectrum * spectrumToCheckMean = new TSpectrum(10,1);//searching for peaks
	    spectrumToCheckMean->Search((*it)->GetHisto(), 5, "", 0.15);
	    Int_t nPeaksToCheckMean = spectrumToCheckMean->GetNPeaks();
	    //Int_t nPeaksToCheckMean = nPeaks;
	    //delete spectrumToCheckMean;
	    streamlog_out(DEBUG) << "nPeaksToCheckMean = " << nPeaksToCheckMean << endl;
		    
	    if ( _ToCheckMean && nPeaksToCheckMean<8 )
	      {//if the number of peaks < 8: ask for the mean condition
		if ( (*it)->GetMean() < (pedestal->getPositionTotal(itChips->first, cntChn)+5*20) )
		  {
		    streamlog_out(DEBUG) << "nPeaks < 10: Condition satisfied" << endl;
		    std::pair<double,double> gainInfo = (*it)->FitGain();
		    gainFit = gainInfo.first;
		    gainErr = gainInfo.second;
		    hChiSquare->Fill((*it)->GetChi2());
		    b_chi2 = (*it)->GetChi2();
		    b_chip = itChips->first;
		    b_chan = cntChn;
		    b_Vcalib = atoi(_vCalib.c_str());
		    tTest->Fill();
		  } 
		else 
		  {

		    streamlog_out(DEBUG) << "nPeaks < 10: Mean condition not satisfied ==> No FitGain" << endl;

		    //fHighSignal->cd();
		    //((*it)->GetHisto())->Write();
		  }
	      } 
	    else 
	      {//if the number of peaks >= 10: drop the mean condition
		streamlog_out(DEBUG) << "nPeaks >= 10: No need to compare mean values" << endl;

		std::pair<double,double> gainInfo = (*it)->FitGain();
		gainFit = gainInfo.first;
		gainErr = gainInfo.second;
		hChiSquare->Fill((*it)->GetChi2());
		b_chi2 = (*it)->GetChi2();
		b_chip = itChips->first;
		b_chan = cntChn;
		b_Vcalib = atoi(_vCalib.c_str());
		tTest->Fill();
	      }
		    
	    streamlog_out(DEBUG) << "Result: gainFit = " << gainFit << " gainFFT = " << gainFFT << endl;
		    
	    if (gainFit <= 0)
	      {
		//delete *it;
	      } 
	    else 
	      {
		file_out << itChips->first << "\t" << cntChn << "\t" << gainFit << "\t" << gainFFT << "\t" << gainErr << endl;
		cntFitted += 1;
		
		streamlog_out(DEBUG) << "cntFitted = " << cntFitted << endl;
		
		hFit2FFT->Fill(gainFit, gainFFT);
		hFitOverFFT->Fill(gainFit/gainFFT);
	      }

	    //cntChn+=1;//maybe has to be deleted?
	    cntTotal += 1;
	  }
	streamlog_out(DEBUG) << "end of chip loop" << endl;
      }
    streamlog_out(DEBUG) << "vcalib: " << _vCalib << " total: " << cntTotal << " fitted: " << cntFitted << endl;
    file_out.close();
    
    output_file->Write();
    output_file->Close();
    fTest->cd();
    tTest->Write();
    fTest->Close();

    fHighSignal->Close();
    
    //delete pedestal;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  bool Ahc2GainCalibrator::CheckSpectrum(TSpectrum *spec)
  {
    bool spectrumIsOK = true;
  
    int nPeaks = spec->GetNPeaks();
  

    if (nPeaks<=2) return false;
  
    double *x     = new double[nPeaks];
    float *diffX  = new float[nPeaks];
    float *sigmaX = new float[nPeaks];
  
    x = (double*)spec->GetPositionX();//getting peak positions

    int lookup[nPeaks];//creating a lookup table to order peaks
    float dummyx[nPeaks];
    float max = 20000;

    for(int i=0; i<nPeaks; i++)
      {
	dummyx[i] = x[i];
      }
    for(int j=0; j<nPeaks; j++)
      {
	//ordering peaks
	for(int i=0; i<nPeaks; i++)
	  {
	    if(dummyx[i]<max)
	      {
		max = dummyx[i];
		lookup[j]=i;
	      }
	  }
	max = 20000;
	dummyx[lookup[j]]=30000;
      }

    for (int ix = 0; ix < nPeaks; ix++)
      {
	streamlog_out(DEBUG) << "peak " << ix << " x " << x[lookup[ix]] << endl;
      }

    if(lookup[0]>3000)
      {
	spectrumIsOK = false;
	//delete x;
	//delete diffX;
	//delete sigmaX;

	return spectrumIsOK;
      }

    diffX[0]  = 0;
    sigmaX[0] = 0;

    for (int ix = 1; ix < nPeaks; ix++)
      {
	streamlog_out(DEBUG) << "x[" << ix <<"] = " << x[lookup[ix]] << " x[" << ix-1 <<"] = " << x[lookup[ix-1]] << endl;
	diffX[ix]  = x[lookup[ix]] - x[lookup[ix-1]];
	sigmaX[ix] = sqrt(diffX[ix]);

	streamlog_out(DEBUG) << "ix " << ix 
		     << " diffX " << diffX[ix] << " sigmaX " << sigmaX[ix]
		     << " diffX-1: " << diffX[ix-1] << " sigmaX-1 : " << sigmaX[ix-1] << endl;
    
	if (ix<2)
	  {
	    streamlog_out(DEBUG) << "first peak: verification not started yet" << endl;
	    continue;
	  }
	if ( (diffX[ix] < diffX[ix-1]-2*sigmaX[ix-1]) || (diffX[ix] > diffX[ix-1]+2*sigmaX[ix-1]) )
	  {
	    streamlog_out(DEBUG) << "distance  between peak " << ix << " and peak " << ix-1
			 << " is too different from the distance between peak " << ix-1 << " and peak " << ix-2 <<endl;
	    spectrumIsOK = false;
	  }
      }

    //delete x;
    //delete diffX;
    //delete sigmaX;
    
    return spectrumIsOK;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                Calib Spectrum Class implementation                                                                                                                                       */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  Ahc2GainCalibrator::CalibSpectrum::CalibSpectrum(int chipID, int chn, int vCalib) 
  {
    histoAll = new TH1D(TString::Format("z_ADC_chip%d_chn%d_%dmV",chipID, chn, vCalib),TString::Format("ADC Spectrum Chip %d, Channel %d, V#_{calib} %dmV",chipID, chn, vCalib),4096,-0.5,4095.5);

    ChiSquareNDF = 0;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  Ahc2GainCalibrator::CalibSpectrum::~CalibSpectrum() 
  {
    delete histoAll;
  }
  
  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::CalibSpectrum::Fill(double val)
  {
    histoAll->Fill(val);
  }
  
  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  std::pair<double,double> Ahc2GainCalibrator::CalibSpectrum::FitGain()
  {
    TF1 *fitAll;
    double gainAll, gainErr;

    //Lan change the maximum number of peaks to find
    fitAll = MultiPFit(histoAll, 10, 1, 5, 0.15);

    if (fitAll != 0){
      gainAll = fitAll->GetParameter(1);
      gainErr = fitAll->GetParError(1);
      double fitrangeLower, fitRangeUpper;
		
      fitAll->GetRange(fitrangeLower, fitRangeUpper);
      ChiSquareNDF = fitAll->GetChisquare()/(histoAll->FindBin(fitRangeUpper)-histoAll->FindBin(fitrangeLower)-fitAll->GetNumberFreeParameters());
		
    }

    if (fitAll != 0)
      {
	gainAll = fitAll->GetParameter(1);
	gainErr = fitAll->GetParError(1);
	double fitrangeLower, fitRangeUpper;
		
	fitAll->GetRange(fitrangeLower, fitRangeUpper);
	ChiSquareNDF = fitAll->GetChisquare()/(histoAll->FindBin(fitRangeUpper)-histoAll->FindBin(fitrangeLower)-fitAll->GetNumberFreeParameters());
      }

    if (fitAll != 0)
      {
	//delete fitAll;
	return std::make_pair(gainAll,gainErr);
      }
	
    //delete fitAll;
    return std::make_pair(0,0);
  }
  
  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  double Ahc2GainCalibrator::CalibSpectrum::FFTGain()
  {
    return GainFromFFT(histoAll)[0];
  }
  
  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                      added FFT Method here.                                                                                                                                */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  float* Ahc2GainCalibrator::CalibSpectrum::GainFromFFT(TH1* hist)
  {
    TH1* hm=NULL;

    TVirtualFFT::SetTransform(0);
    hm = hist->FFT(hm,"MAG");      //root FFT gives same result as fftw3 package
    TString str = hist->GetName();
    str+="_fft";
	
    float *fitresult = new float[2];
    FindPeak(hm,fitresult);

    float norm = (fitresult[0]*hist->GetBinWidth(1));
    float normerr = (fitresult[0]+fitresult[1])*hist->GetBinWidth(1);
    float gain = hist->GetNbinsX()/norm;
    float errgain = hist->GetNbinsX()/normerr;
    float err = TMath::Abs(errgain-gain);
    delete fitresult;

    float *retv = new float[2];
    retv[0] = gain;
    retv[1] = err; 
    delete hm;
    return retv;
  }
  
  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::CalibSpectrum::FindPeak(TH1* hist, Float_t *res)
  {
    int startbin = 3;
    int endbin = 0.1*hist->GetNbinsX();

    for(int i=startbin;i<endbin;i++) 
      {
	if(    hist->GetBinContent(i)<hist->GetBinContent(i+1)
	       && hist->GetBinContent(i)<hist->GetBinContent(i+2) 
	       && hist->GetBinContent(i)<hist->GetBinContent(i+3) ) 
	  {
	    startbin=i;
	    break;
	  }
      }
    hist->SetAxisRange(startbin,endbin);
    int maxbin = hist->GetMaximumBin();
  
    TF1* f1 = new TF1("f1","gaus",maxbin-4,maxbin+4);
    f1->SetParLimits(1,maxbin-2,maxbin+2);
    hist->Fit(f1,"RQ");

    res[0] = f1->GetParameter(1);
    res[1] = f1->GetParError(1);

    //delete f1;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  TF1* Ahc2GainCalibrator::CalibSpectrum::MultiPFit(TH1 *peaks, int nOfpeaks, float res, float sigmapeak, float threshold)
  {
    //nOfpeaks -  sets the max n. of peaks stored
    //res -  resolution between peaks (min. value 1)
    //sigmapeak - sigma
    // threshold - peaks with amplitude less than threshold*highest_peak are discarded
  
    /*************finding peaks*****************/
    TSpectrum *spectrum = new TSpectrum(nOfpeaks,res);//searching for peaks
    spectrum->Search(peaks,sigmapeak,"",threshold);

    int nPeaks = spectrum->GetNPeaks();

    if (nPeaks > 2) 
      {
	double *x = new double[nPeaks];
	double *y = new double[nPeaks];

	x = (double*)spectrum->GetPositionX();//getting peak positions
	y = (double*)spectrum->GetPositionY();//getting amplitude

	int lookup[nPeaks];//creating a lookup table to order peaks
	float dummyx[nPeaks];
	float max = 20000;

	for(int i=0; i<nPeaks; i++)
	  {
	    dummyx[i] = x[i];
	  }

	for(int j=0; j<nPeaks; j++)
	  {
	    //ordering peaks
	    for(int i=0; i<nPeaks; i++)
	      {
		if(dummyx[i]<max)
		  {
		    max = dummyx[i];
		    lookup[j]=i;
		  }
	      }
	    max = 20000;
	    dummyx[lookup[j]]=30000;
	  }

	TF1 *mpeak = new TF1("mpeak",MultiGaus,0,4000,2*nPeaks+3);
	mpeak->SetNpx(1000);
	mpeak->SetLineColor(kRed);

	/******setting initial parameters*********/

	float gain = x[lookup[1]]-x[lookup[0]]; //gain distance between first 2 peaks
	float szero = gain/5; //initial sigma

	mpeak->FixParameter(0,nPeaks);//number of peaks FIXED

	mpeak->SetParameter(1,gain);//gain
	mpeak->SetParLimits(1,0.5*gain,1.5*gain);

	mpeak->SetParameter(2,x[lookup[0]]);//pede
	mpeak->SetParLimits(2,x[lookup[0]]-szero/2,x[lookup[0]]+szero/2);

	for(int i=0; i<nPeaks; i++)
	  {
	    mpeak->SetParameter(2*i+3,y[lookup[i]]);
	    mpeak->SetParLimits(2*i+3,0.8*y[lookup[i]],1.5*y[lookup[i]]);//height


	    //mpeak->SetParameter(2*i+4,TMath::Sqrt(i+1)*szero);
	    mpeak->SetParameter(2*i+4,szero);
	    mpeak->SetParLimits(2*i+4,szero/10,3*szero);//sigma

	  }

	peaks->Fit(mpeak,"R","",x[lookup[0]]-szero,x[lookup[nPeaks-1]]+1.5*szero);
	mpeak->SetRange(x[lookup[0]]-szero,x[lookup[nPeaks-1]]+1.5*szero);

	//delete x;
	//delete y;
	//delete spectrum;

	return mpeak;
      } 
    else 
      {
	//delete spectrum;

	return NULL;
      }
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  double Ahc2GainCalibrator::CalibSpectrum::FitGainMultiGauss()
  {
    TF1 *fitAll;
    double gainAll;

    fitAll = MultiPFit(histoAll, 6, 1, 2, 0.25);
    
    if (fitAll != 0)
      {
	gainAll = fitAll->GetParameter(1);
	double fitrangeLower, fitRangeUpper;
	fitAll->GetRange(fitrangeLower, fitRangeUpper);
	ChiSquareNDF = fitAll->GetChisquare()/(histoAll->FindBin(fitRangeUpper)-histoAll->FindBin(fitrangeLower)-fitAll->GetNumberFreeParameters());
      }

    //delete fitAll;

    if (fitAll != 0)
      {
	return gainAll;
      }

    return 0;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                Pedestal Calibrator Class implementation                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  Ahc2GainCalibrator::PedestalCalibrator::PedestalCalibrator()
  {
    path = "";
    pedOffsets.clear();
    pedPossAll.clear();
    pedWidthsAll.clear();
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::PedestalCalibrator::FillPedestal() 
  {
    std::ifstream file;
    file.open(path.c_str(),std::ifstream::in);

    std::string line;
    int chip, channel, cnt(0);
    double pedWidthAll, pedPosAll;
    double pedPosMemCell[16];

    for (int imcell = 0; imcell < 16; imcell++)
      {
	pedPosMemCell[imcell] = 0;
      }

    if(file.is_open())
      {
	while(getline(file, line))
	  {
	    if (!line.length()) continue;
	    if (line[0]=='#') continue;

	    sscanf(line.c_str(), "%d\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf",
		   &chip, &channel, &pedPosAll, &pedWidthAll,
		   pedPosMemCell+0, pedPosMemCell+1, pedPosMemCell+2, pedPosMemCell+3, pedPosMemCell+4, pedPosMemCell+5, pedPosMemCell+6, pedPosMemCell+7,
		   pedPosMemCell+8, pedPosMemCell+9, pedPosMemCell+10, pedPosMemCell+11, pedPosMemCell+12, pedPosMemCell+13, pedPosMemCell+14, pedPosMemCell+15);

	    cnt += 1;
	    //fill pedestal offsets:
	    if (pedOffsets.count(chip) == 0)
	      {
		pedOffsets[chip]; //this makes sure that the key chip has been default-constucted in the data map
		pedOffsets[chip].resize(36); //this makes sure the corresponding vector is long enough.
	      }

	    pedOffsets[chip][channel].resize(16);

	    for (int i=0; i<16; i++)
	      {
		pedOffsets[chip][channel][i] = pedPosMemCell[i];
	      }

	    //fill combined pedestal position:
	    if (pedPossAll.count(chip) == 0)
	      {
		pedPossAll[chip]; //this makes sure that the key chip has been default-constucted in the data map
		pedPossAll[chip].resize(36); //this makes sure the corresponding vector is long enough.
	      }

	    pedPossAll[chip][channel] = pedPosAll;

	    //fill combined pedestal width:
	    if (pedWidthsAll.count(chip) == 0)
	      {
		pedWidthsAll[chip]; //this makes sure that the key chip has been default-constucted in the data map
		pedWidthsAll[chip].resize(36); //this makes sure the corresponding vector is long enough.
	      }

	    pedWidthsAll[chip][channel] = pedWidthAll;
	  }
      }

    file.close();
    streamlog_out(DEBUG) << "PedestalCalibrator: read file \"" << path << "\", found " << cnt << " entries." << std::endl;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  Ahc2GainCalibrator::PedestalCalibrator::~PedestalCalibrator() 
  {

  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2GainCalibrator::PedestalCalibrator::setPath(std::string PATH)
  {
    path = PATH;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  double Ahc2GainCalibrator::PedestalCalibrator::getOffset(int chip, int chn, int memCell)
  {
    if (pedOffsets.count(chip) != 0)
      {
	return pedOffsets[chip][chn][memCell];
      } 
    else 
      {
	return 0;
      }
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  double Ahc2GainCalibrator::PedestalCalibrator::getPositionTotal(int chip, int chn)
  {
    if (pedOffsets.count(chip) != 0)
      {
	return pedPossAll[chip][chn];
      } 
    else 
      {
	return 0;
      }

  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  double Ahc2GainCalibrator::PedestalCalibrator::getWidthTotal(int chip, int chn)
  {
    if (pedOffsets.count(chip) != 0)
      {
	return pedWidthsAll[chip][chn];
      } 
    else 
      {
	return 0;
      }
  }

}//end of CALICE namespace
