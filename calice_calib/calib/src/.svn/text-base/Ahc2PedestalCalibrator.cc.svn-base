#include <cfloat>
#include <iostream>
#include <string>

#include "EVENT/CalorimeterHit.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCCollection.h>

#include "Ahc2PedestalCalibrator.hh"

#include "marlin/ConditionsProcessor.h"
#include <marlin/Exceptions.h>

#include "LabviewBlock2.hh"

#include <TSystem.h>
#include <TROOT.h>

using namespace std;
using namespace marlin;

namespace CALICE
{

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                    */
  /***************************************************************************************/

  Ahc2PedestalCalibrator aAhc2PedestalCalibrator;

  Ahc2PedestalCalibrator::Ahc2PedestalCalibrator():Processor("Ahc2PedestalCalibrator")
  {
    _description = "Extract Pedestal from data, fit them and provide a file with Pedestal value and memory offsets";
    
    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection",
			       _inputColName,
			       std::string(""));

    registerProcessorParameter("OutputRootfileName",
			       "Name of the rootfile output containing pedestals",
			       _rootfileName,
			       std::string(""));

    registerProcessorParameter("OutputPedestalFile",
			       "Name of the pedestal file output",
			       _pedestalfileName,
			       std::string(""));

  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                    */
  /***************************************************************************************/

  void Ahc2PedestalCalibrator::init()
  {
    gSystem->Load("libSpectrum"); //Load TSpectrum
    printParameters();

    output_file = new TFile(_rootfileName.c_str(),"RECREATE"); //Create output rootfile

    hWidthRatio = new TH1F("hWidthRatio",";RMS_all / RMS_singles;entries",100,0,5);
    hOffset = new TH1F("hOffsets",";Offsets;entries",100,-20,20);

    histos.clear(); //clear map
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2PedestalCalibrator::processRunHeader(LCEvent *evt) {
    
    streamlog_out(MESSAGE) << "Start to process Run: " << evt->getRunNumber() << endl;
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2PedestalCalibrator::processEvent(LCEvent *evt)
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

	for (unsigned int i = 0; i < (unsigned int)noElem; i++)
	  {
	    const LabviewBlock2 ldata(inputCol->getElementAt(i));

	    int HitBit = ldata.GetHitBit();
	    int GainBit = ldata.GetGainBit();
	    int EvtNr = ldata.GetEvtNr();

	    int ChipID = ldata.GetChipID();
	    int Chn = ldata.GetChannel();

	    int ADC = ldata.GetADC();

	    //#########
	    streamlog_out(DEBUG)<< "ChipID: " << ldata.GetChipID()<< "  Channel: "<< ldata.GetChannel() << endl;

	    if(EvtNr == 0) continue;//Reject first Memory cell (0) in the Chip

	    //generate new histos if necessary:
	    if (histos.count(ChipID) == 0)
	      {
		histos[ChipID] = vector<PedestalSpectrum*>(36);
		for (int i=0; i<36; i++)
		  {
		    histos[ChipID][i] = new PedestalSpectrum(ChipID, i);
		  }
	    
		streamlog_out(MESSAGE) << "creating ChipID " << ChipID << flush << endl;
	      }

	    //Fill Histo
	    if (ADC > 0 && GainBit == 1)
	      {
		histos[ChipID][Chn]->Fill(ADC, EvtNr);//Fill Histogram (HG only)
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

  void Ahc2PedestalCalibrator::check( LCEvent * evt )
  {
    
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  void Ahc2PedestalCalibrator::end()
  {
    
    /*----------------------------------------------------------------------*/
    /*                                                                      */
    /*       Fit Pedestals                                                  */
    /*                                                                      */
    /*----------------------------------------------------------------------*/

    ofstream myfile;//Create output tsv file
    myfile.open (_pedestalfileName.c_str());
    myfile <<"#pedestal positions & memory cell dependent offsets from file" << endl;
    myfile <<"#Chip \t Chn \t pedposall \t pedwidthall \t pedposcell1 \t pedposcell2 \t pedposcell3 \t ... \t pedposcell16" << endl;

    int cntChn;
    //Loop over histo map chip
    for(map<int, vector<PedestalSpectrum*> >::iterator itChips = histos.begin(); itChips!=histos.end(); itChips++)
      {
	cntChn = 0;
	//Loop over each channels (36)
	for(vector<PedestalSpectrum*>::iterator it = itChips->second.begin(); it!=itChips->second.end(); it++)
	  {
	    streamlog_out(MESSAGE) << "Chip " << itChips->first << " chn " << cntChn << endl;

	    (*it)->FitPedestals();//Fit Pedestal

	    //Write to output file "Chip Chn PedAll PedAllWidth"
	    myfile << itChips->first << "\t" << cntChn << "\t" << (*it)->pedestalPositionAll << "\t" << (*it)->pedestalWidthAll;

	    //Loop over all memory cells
	    for(int i=0; i<16; i++)
	      {
		double thisWidthRatio;
		thisWidthRatio = (*it)->pedestalWidthAll / (*it)->pedestalWidths.at(i);
		hWidthRatio->Fill(thisWidthRatio);//Fill Ratio histogram

		hOffset->Fill((*it)->pedestalOffsets.at(i));//Fill offset histogram

		streamlog_out(DEBUG) << "cell " << i+1 << " thisWidthRatio " << thisWidthRatio << " offset " << (*it)->pedestalOffsets.at(i) << endl;

		//Write to output file "Pedestal for memcell0 ... memcell15"
		myfile << "\t" << (*it)->pedestalOffsets.at(i);

		if (fabs((*it)->pedestalOffsets.at(i))>200) 
		  streamlog_out(DEBUG) << "Strange shift : Chip " << itChips->first << " chan " << cntChn << endl;
	      }

	    myfile << endl;
	    cntChn += 1;
	  }//end loop channel
      }//end loop chip

    myfile.close();//Close output file
    output_file->Write();
    output_file->Close();//Write and Close output rootfile

  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                Pedestal Spectrum Class implementation                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  Ahc2PedestalCalibrator::PedestalSpectrum::PedestalSpectrum(int chipID, int chn)
  {
    histoAll = new TH1F(TString::Format("z_PEDESTAL_chip%d_chn%d_all",chipID, chn),TString::Format("Pedestal Spectrum Chip %d, Channel %d, all MemCells",chipID, chn),4096,-0.5,4095.5);//Create histo for each Chip / Chn

    _histos.resize(16);//Resize vector histo Memcell

    for (int i=0; i<16; i++)
      {
	_histos.at(i) = new TH1F(TString::Format("z_PEDESTAL_chip%d_chn%d_cell%d",chipID, chn, i),TString::Format("Pedestal Spectrum Chip %d, Channel %d, MemCell %d",chipID, chn, i),4096,-0.5,4095.5);//Create histo for each Chip / Chn / MemCell
      }
  
    pedestalPositions.resize(16,0);//Resize vector Ped position for each memcell
    pedestalOffsets.resize(16,0);//Resize vector Ped offset for each memcell
    pedestalWidths.resize(16,0);//Resize vector Ped width for each memcell
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  Ahc2PedestalCalibrator::PedestalSpectrum::~PedestalSpectrum()
  {
    delete histoAll; //delete histo for each Chip / Chn
    for (int i=0; i<16; i++)
      {
	delete _histos.at(i);//delete histo for each Chip / Chn / Memcell
      }
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  int Ahc2PedestalCalibrator::PedestalSpectrum::Fill(double val, int memCell)
  {
    //Filling values
    histoAll->Fill(val);
    _histos.at(memCell)->Fill(val);
  }

  /***************************************************************************************/
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /*                                                                                                                                                     */
  /***************************************************************************************/

  int Ahc2PedestalCalibrator::PedestalSpectrum::FitPedestals()
  {
    TSpectrum *s = new TSpectrum();
    for (int i=0; i<=16; i++)
      {
	TH1F* thisHisto;
	double *thisPedestalWidth, *thisPedestalPosition;
	if (i<16)
	  {
	    thisHisto = _histos.at(i);
	    thisPedestalWidth = &(pedestalWidths.at(i));
	    thisPedestalPosition = &(pedestalPositions.at(i));
	  } 
	else 
	  {
	    thisHisto = histoAll;
	    thisPedestalWidth = &pedestalWidthAll;
	    thisPedestalPosition = &pedestalPositionAll;
	  }
      
	s->Search(thisHisto, 4, "", 0.5);//Search for pedestal peak

	double *PeakPosX, *PeakPosY;
	double PeakX, PeakY;
	int NPeaks;
      
	NPeaks = s->GetNPeaks();//Number of peaks
	PeakPosX = (double*)s->GetPositionX();//X position of peaks
	PeakPosY = (double*)s->GetPositionY();//Y position of peaks
      
	if (NPeaks>0)
	  {
	    thisHisto->Fit("gaus","","QM", PeakPosX[0]-7, PeakPosX[0]+7);//Fit histogram with gaussian function between 1st peak and +- 7 ADC
	    TF1* fit = thisHisto->GetFunction("gaus");
	    if (fit != 0)
	      {
		*thisPedestalPosition = fit->GetParameter(1);
		*thisPedestalWidth = fit->GetParameter(2);
	      } else 
	      {
		*thisPedestalPosition = 0;
		*thisPedestalWidth = 0;
	      }
	  }
      }

    for (int i=0; i<16; i++)
      {
	pedestalOffsets.at(i) = pedestalPositions.at(1) - pedestalPositions.at(i);//Calculate offset between memory cell i and memory cell 1

	 streamlog_out(DEBUG) << "cell " << i << " pedestalPositions.at(1) " << pedestalPositions.at(1) 
	     << " pedestalPositions.at(i) " << pedestalPositions.at(i) << endl;
      }
  }

}
