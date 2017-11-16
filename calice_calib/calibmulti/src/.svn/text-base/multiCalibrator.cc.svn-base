#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "multiCalibrator.hh"

// LCIO includes
#include <EVENT/LCParameters.h>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCGenericObject.h>
#include <UTIL/LCTime.h>
#ifdef MULTI_DBINIT
// LCCD includes
#include <lccd/DBInterface.hh>
#endif

// Marlin includes
#include <marlin/ConditionsProcessor.h>
#include <marlin/Exceptions.h>

// CALICE includes
#include "collection_names.hh"
#include "AdcBlock.hh"
#include "TriggerBits.hh"

// root includes
#include "TFile.h"
#include "TF1.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

// include convolution class
#include "TConvolution.h"

// include connectionDataClass
#include "DAQconnection.hh"
#include "VetoFraction.hh"
#include "VetoThreshold.hh"

// include some c++ std
#include <ctime>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <limits>


namespace CALICE {
  multiCalibrator amultiCalibrator;

  multiCalibrator::multiCalibrator() :  marlin::Processor("multiCalibrator"),_connectionAvailable(false)  {

    _description = " This processor adds the multi amplitude as " PAR_MULTI " to the event and calibrates the thresholds."
#ifdef MULTI_DBINIT
      " The threshold results are written to the conditions database. ";
#else
    ;
#endif

    registerInputCollection( LCIO::LCGENERICOBJECT, "ADCCollectionName" ,
                             "The name of the adc collection (input) to be used" ,
                             _adcColName ,
                             std::string(COL_ADC) );

    registerInputCollection( LCIO::LCGENERICOBJECT, "connectionCollectionName" ,
                             "The name of the DAQconnection collection (input) to be used" ,
                             _connectionColName ,
                             std::string("multiConnection") );

    registerInputCollection( LCIO::LCGENERICOBJECT, "referenceThresholds" ,
                             "The name of the reference threshold collection to be used to calculate rejected fraction" ,
                             _referenceThresholdColName ,
                             std::string("") );

    registerProcessorParameter("TriggerEventName",
                               "Name of the event parameter name which contains the current trigger main word .",
                               _parNameTriggerEvent,
                               std::string(PAR_TRIGGER_EVENT));
#ifdef MULTI_DBINIT

    registerProcessorParameter("thresholdFolder",
                               "folder in the database to which the threshold results should be written",
                               _thresholdFolderName,
                               std::string(""));

    registerProcessorParameter("rejectedFractionFolder",
                               "folder in the database to which the fraction of rejected events for each threshold should be written",
                               _rejectedFractionFolderName,
                               std::string(""));

    registerProcessorParameter("rejectedFractionReferenceFolder",
                               "folder in the database to which the fraction of rejected events for the reference thresholds should be written",
                               _rejectedFractionReferenceFolderName,
                               std::string(""));
#endif
    registerProcessorParameter("OutputFilePath",
                               "path for the root file inlcuding the multi histograms",
                               _outputFilePath,
                               std::string("./"));


    _signalBinning.clear();
    _signalBinning.push_back(20000);
    _signalBinning.push_back(0);
    _signalBinning.push_back(20000);

    registerProcessorParameter( "SignalBinning" ,
                                "The binning for the multi counter amplitude  histograms." ,
                                _signalBinning ,
                                _signalBinning ,
                                _signalBinning.size() ) ;

    _purityList.clear();
    _purityList.push_back(99.9);
    _purityList.push_back(99.);
    _purityList.push_back(95.);
    _purityList.push_back(90.);

    registerProcessorParameter( "purities" ,
                                "List of purities for which the thresholds should be calculated. 99.9 means that at least 99.9% of the two mip events are rejected." ,
                                _purityList,
                                _purityList ,
                                _purityList.size() ) ;


    registerProcessorParameter( "gausRange",
                                "processing range of the gaus in the convolution (+- ADC counts)",
                                _gausRange,
                                (float)300);

    registerProcessorParameter( "rebin",
                                "rebin factor for the histogram before the fit gets applied\nthis heavily affects the CPU time needed for the fit\nshould be a power of 2 to reduce DNL of the CALICE CRC ADC",
                                _rebin,
                                (int)16);

    registerProcessorParameter( "granFac",
                                "factor for the granularity of the 1 & 2 MIP histograms generated from the fit results; reasonably the same as rebin",
                                _granFac,
                                (int)16);

    registerOptionalParameter( "skipCalibration",
                               "If this is \"true\", only the parameter is added to the event.",
                               _skipCalibration,
                               std::string("false"));
  }

  void multiCalibrator::conditionsChanged(lcio::LCCollection* col) {
    std::string condName = col->getParameters().getStringVal("CollectionName");
    streamlog_out(MESSAGE) << "update for conditions collection " << condName << std::endl;

    if (condName == _connectionColName) {
      if (col->getNumberOfElements() != 1) {
        streamlog_out(WARNING1) << "unnatural connection collection size: " << col->getNumberOfElements() << std::endl;
      }
      for (unsigned int elm_i=0;elm_i< (unsigned int)col->getNumberOfElements() ; elm_i++) {
        DAQconnection connection(col->getElementAt(elm_i));
        _crate   = connection.getCrate();
        _slot    = connection.getSlot();
        _fe      = connection.getFe();
        _chip    = connection.getChip();
        _channel = connection.getChannel();
        _connectionAvailable = true;
        streamlog_out(MESSAGE) << "found multi connection info: " << _crate << "," << _slot << "," << _fe << "," << _chip << "," << _channel << std::endl;
      }
    }
    else if (condName == _referenceThresholdColName) {
      for (unsigned int elm_i=0;elm_i< (unsigned int)col->getNumberOfElements() ; elm_i++) {
        VetoThreshold threshold(col->getElementAt(elm_i));
        _referenceThresholds[threshold.getPurity()] = threshold.getThreshold();
      }
    }
  }

  void multiCalibrator::init() {

    printParameters();

    _runNumber = std::numeric_limits<unsigned int>::infinity();
    _startTime = 0;
    _endTime = 0;

    marlin::ConditionsProcessor::registerChangeListener( this,_connectionColName);
    marlin::ConditionsProcessor::registerChangeListener( this,_referenceThresholdColName);
    if (_skipCalibration != "true") {
      _sigHist = new TH1F("MultiSignal","Multi counter signal", (int)_signalBinning[0], (double)_signalBinning[1],(double)_signalBinning[2]);
      _pedHist = new TH1F("MultiPedestal","Multi counter pedestal", (int)_signalBinning[0], (double)_signalBinning[1],(double)_signalBinning[2]);
    }

  }

  void multiCalibrator::processRunHeader( LCRunHeader* run) {
    if (_runNumber != std::numeric_limits<unsigned int>::infinity()) {
      streamlog_out(ERROR) << "Cannot process more than one run!!!!! " << std::endl;
      throw marlin::StopProcessingException(&amultiCalibrator);
    }

    _runNumber = run->getRunNumber();
  }

  void multiCalibrator::fillHist( TH1F* hist, LCEvent* evt) {
    try {
      LCCollection* col_adc = evt->getCollection( _adcColName ) ;

      for (unsigned int elm_i=0;elm_i<(unsigned int)(col_adc->getNumberOfElements()); elm_i++) {
        AdcBlock adc_block(col_adc->getElementAt(elm_i));

        if ((unsigned int)(adc_block.getCrateID()) == _crate
            && (unsigned int)(adc_block.getSlotID()) == _slot
            && (unsigned int)(adc_block.getBoardFrontEnd()) == _fe
            && (unsigned int)(adc_block.getMultiplexPosition()) == _channel) {
          if (_skipCalibration != "true")  hist->Fill(adc_block.getAdcVal(_chip));
          evt->parameters().setValue(PAR_MULTI,(float)(adc_block.getAdcVal(_chip)));

          // there is only one multi counter
          break;
        }
      }
    }
    catch (  DataNotAvailableException &err ) {
    }

  }

  void multiCalibrator::processEvent( LCEvent * evt ) {
    if (_startTime == 0) _startTime = evt->getTimeStamp()-1;
    _endTime = evt->getTimeStamp()+1;

    if (!_connectionAvailable) {
      streamlog_out(WARNING1) << "No MULTI connection information available, cannot process multi" << std::endl;
      return;
    }

    TriggerBits triggerEvent = evt->getParameters().getIntVal(_parNameTriggerEvent);

    bool pedestal = triggerEvent.isPedestalTrigger();
    bool beam = triggerEvent.isBeamTrigger();

    if (beam) {
      fillHist(_sigHist, evt);
    }
    else if (pedestal) {
      fillHist(_pedHist, evt);
    }
    else streamlog_out(WARNING1) << "Event which is neither beam nor pedestal" << std::endl;
  }


  void multiCalibrator::end()  {

    if (_skipCalibration == "true") {
      streamlog_out(MESSAGE) << "calibration disabled -- nothing to do" << std::endl;
    }
    else {
      TH1* fitHist=0;
      TH1* pureMIPhist=0;
      TH1* oneMIPhist=0;
      TH1* twoMIPhist=0;
      TH1* fitResultHist=0;
      TH1* fitHistFine=0;
      TH1* subHist=0;
      TH1* subHistBiasRemoved=0;

      TF1* g=0;
      TF1* gg=0;
      TF1* l=0;
      TF1* ll=0;
      TF1* bias=0;

      TConvolution* convoluter=0;


      if (_pedHist && _sigHist) {
        const Int_t REBIN   = _rebin;   // to speed up the fit the histogram is rebinned
        const Int_t GRANFAC = _granFac; // for the estimation of threshold a better binning than for the fit is necessary

        std::time_t now = std::time(NULL);
        streamlog_out(MESSAGE) << "start of multi calibration " << std::asctime(std::localtime(&now));

        // get a working copy of the raw histogram
        fitHist = (TH1*) _sigHist->Clone("MultiCalibratorHistForFit");

        // rebin and remeber the dimensions of new histogram, this is necessary to properly add histograms later
        fitHist->Rebin(REBIN);
        const Int_t nBins = fitHist->GetNbinsX();
        const Double_t minRange = fitHist->GetBinLowEdge(1);
        const Double_t maxRange = fitHist->GetBinLowEdge(nBins) + fitHist->GetBinWidth(nBins);
        const Double_t binWidth =  fitHist->GetBinWidth(1);
        const Double_t entries =  fitHist->GetEntries();

        streamlog_out(MESSAGE2) << "Rebinned histogram has " << nBins << " bins in range from " << minRange << " to " << maxRange << std::endl;
        streamlog_out(MESSAGE2) << "BinWidth: " << binWidth << std::endl;


        // as the convoluted function has no start value apprixmation, we have to do it by hand
        now = std::time(NULL);
        streamlog_out(MESSAGE1) << "estimating start values for convoluted fit " << std::asctime(std::localtime(&now));

        g = new TF1("g","gaus",minRange,maxRange);
        l = new TF1("l","landau",minRange,maxRange);

        fitHist->Fit(l,"RQ0N");
        _pedHist->Fit(g,"RQ0N");

        Double_t pedestal = g->GetParameter(1);

        /* generate the functions for the convolution, we need the properly normalized versions
           also one function should have the mean/MPV fixed at the origin, for our case the gaussian */
        gg = new TF1("gg","TMath::Gaus(x,0,[0],1)",-1.*_gausRange,_gausRange);
        gg->SetParName(0,"sigma");
        ll = new TF1("ll","TMath::Landau(x,[0],[1],1)",minRange,maxRange);
        ll->SetParNames("MPV","sigma");

        /* initialize the functions for the convolution
           first approximation:
           gaus broadening is pure electronic noise (pedestal fit width)
           landau parameters are taken from landau fit of the signal */
        gg->SetParameter(0,g->GetParameter(2));  // Gaus sigma
        ll->SetParameter(0,l->GetParameter(1));  // Landau MPV
        ll->SetParameter(1,l->GetParameter(2));  // Landau sigma

        // get convoluted function
        convoluter = new TConvolution("convolution",gg,ll);
        TF1* conv = convoluter->GetConvolutedFunction();

        // normalize
        conv->SetParameter(3,binWidth*entries);

        // set fit range
        Double_t fitRangeMax = l->GetParameter(1)+6*l->GetParameter(2); // TO-DO, fit range should not be hardcoded
        Double_t fitRangeMin = l->GetParameter(1)-6*l->GetParameter(2);
        conv->SetRange(fitRangeMin,fitRangeMax);

        // fitting signal
        now = std::time(NULL);
        streamlog_out(MESSAGE1) << "fitting convoluted function " << std::asctime(std::localtime(&now));
        fitHist->Fit(conv,"Q0RN");

        Double_t MIPpos = conv->GetMaximumX();
        Double_t MIP = MIPpos - pedestal;
        streamlog_out(MESSAGE) << "pedestal position: " << pedestal << " MIP position: " << MIPpos << " MIP: " << MIP << std::endl;


        now = std::time(NULL);
        streamlog_out(MESSAGE2) << "generating histogram from fit function " << std::asctime(std::localtime(&now));

        /* we need something faster than a function generated by convolution for the later processing
           therefore a histogram is generated from the function
           also the histogram is readable without the ROOT extension when saved in the file */
        conv->SetRange(MIPpos-0.5*MIP,maxRange);  // extend range

        fitResultHist = new TH1F("fitResultHist","one MIP histogram",nBins,minRange,maxRange);
        fitResultHist->Add(conv);

        /* How good is the fit? How much double events exist?
           generate residual */
        now = std::time(NULL);
        streamlog_out(MESSAGE2) << "generating substracted histogram " << std::asctime(std::localtime(&now)) ;

        subHist = (TH1*) fitHist->Clone("substractedHist");
        subHist->GetListOfFunctions()->Clear();
        subHist->Sumw2();
        subHist->Add(fitResultHist,-1);


        /* We need to know the distribution of the two MIP events, for the calculation of the treshold.
           To save CPU the distribution is simulated from the single MIP distribution. */
        now = std::time(NULL);
        streamlog_out(MESSAGE2) << "generating distributions for threshold calculation" << std::asctime(std::localtime(&now)) ;

        /* Before playing with the parameters of the result: backup
           using vector, as some compiler options do not allow arrays to be initalized with a const generated within the function */
        const int NPAR = conv->GetNpar();
        std::vector<Double_t> parBackup;
        parBackup.reserve(NPAR);
        conv->GetParameters(&(parBackup[0]));

        // generate finer histogram of the fit result (for reference)
        fitHistFine = new TH1F("fitHistfine","one MIP histogram",nBins*GRANFAC,minRange,maxRange);
        conv->SetParameter(3,conv->GetParameter(3)/(float)GRANFAC); // renormalize to new binning
        fitHistFine->Add(conv);
        conv->SetParameters(&(parBackup[0]));

        /* We need the MIP distribution without pedestal & electronics noise broadening for the later calculation of the two MIP distribution */
        pureMIPhist = new TH1F("pureMIPhist","one MIP histogram without pedestal and electronic noise",nBins*GRANFAC,minRange,maxRange);
        conv->SetParameter(0,sqrt(pow(conv->GetParameter(0),2)-pow(g->GetParameter(2),2))); // quadratically substract the pedestal width (electronics noise)
        conv->SetParameter(1,conv->GetParameter(1)-pedestal); // shift for pedestal
        conv->SetRange(MIP-0.5*MIP,maxRange);
        pureMIPhist->Add(conv);
        conv->SetParameters(&(parBackup[0]));

        now = std::time(NULL);
        streamlog_out(MESSAGE2) << "generating MIP & two MIP histogram " << std::asctime(std::localtime(&now));

        oneMIPhist = new TH1F("oneMIPhist","one MIP histogram",nBins*GRANFAC,minRange,maxRange);
        for (unsigned i(0); i<1000000;i++) oneMIPhist->Fill(pureMIPhist->GetRandom() + g->GetRandom());

        twoMIPhist = new TH1F("twoMIPhist","two MIP histogram",nBins*GRANFAC,minRange,maxRange);
        for (unsigned i(0); i<1000000;i++) twoMIPhist->Fill(pureMIPhist->GetRandom() + pureMIPhist->GetRandom() + g->GetRandom());

        // now calculate threshold from distribution

        std::sort(_purityList.begin(), _purityList.end());  // order of values is important for an efficient threshold calculation;

        Int_t bin=0;
        Double_t rejectInt=0;
        LCCollection *thresholdCollection = new LCCollectionVec(LCIO::LCGENERICOBJECT);
        LCCollection *lossCollection = new LCCollectionVec(LCIO::LCGENERICOBJECT);

        LCCollection *referenceLossCollection = 0;
        if (_referenceThresholdColName!= "") referenceLossCollection = new LCCollectionVec(LCIO::LCGENERICOBJECT);

        for (int pos=(int)_purityList.size()-1; pos>=0 && bin<twoMIPhist->GetNbinsX();--pos) {

          float purity = _purityList[pos];
          float limit = (1. - purity/100.)*twoMIPhist->GetEntries();
          while ( rejectInt < limit && bin<twoMIPhist->GetNbinsX()) rejectInt += twoMIPhist->GetBinContent(bin++);

          float thresholdValue = twoMIPhist->GetBinLowEdge(bin-1);
          float thresholdValueErr = twoMIPhist->GetBinWidth(bin-1)/2.;
          float effectivePurity = (1. -(rejectInt - twoMIPhist->GetBinContent(bin-1))/twoMIPhist->GetEntries())*100.;
          VetoThreshold *threshold = new VetoThreshold(purity,effectivePurity,thresholdValue,thresholdValueErr);
          thresholdCollection->addElement(threshold);

          Int_t thresholdBinSignal = _sigHist->FindBin(thresholdValue);
          float lostEvents = _sigHist->Integral(thresholdBinSignal,_sigHist->GetNbinsX());
          float lossFraction = lostEvents/_sigHist->Integral(0,_sigHist->GetNbinsX());
          VetoFraction *loss = new VetoFraction(purity, lossFraction);
          lossCollection->addElement(loss);

        }

        if (referenceLossCollection)
          for (ThresholdMap_t::const_iterator iter = _referenceThresholds.begin(); iter != _referenceThresholds.end();++iter) {
            float purity = iter->first;
            float threshold = iter->second;

            Int_t thresholdBinSignal = _sigHist->FindBin(threshold);
            float lostEvents = _sigHist->Integral(thresholdBinSignal,_sigHist->GetNbinsX());
            float lossFraction = lostEvents/_sigHist->Integral(0,_sigHist->GetNbinsX());
            VetoFraction *loss = new VetoFraction(purity, lossFraction);
            referenceLossCollection->addElement(loss);
          }



        Double_t signalInt = _sigHist->Integral(1,_sigHist->GetNbinsX());

        // announce the result
        for (int i=0; i< thresholdCollection->getNumberOfElements(); ++i) {
          VetoThreshold *tr   = dynamic_cast<VetoThreshold*>( thresholdCollection->getElementAt(i));
          VetoFraction  *loss = dynamic_cast<VetoFraction*>( lossCollection->getElementAt(i));
          streamlog_out(MESSAGE) << " Threshold for rejection of " << tr->getPurity() <<"% ("<<tr->getEffectivePurity() <<") of double particles: " << tr->getThreshold() << "+-" << tr->getThresholdError() << " loss in total statistics: " << loss->getFraction()*100 << "%"<< std::endl;
        }
        for (int i=0; i< referenceLossCollection->getNumberOfElements(); ++i) {
          VetoFraction *loss = dynamic_cast<VetoFraction*>(referenceLossCollection->getElementAt(i));
          float purity = loss->getPurity();
          streamlog_out(MESSAGE) << " Loss for rejection of " << purity << "% of double particles with reference threshold " <<_referenceThresholds[purity] << ": " << loss->getFraction()*100 << "%"<< std::endl;

        }

        // now try to estimate number of double particles

        subHistBiasRemoved = (TH1*) subHist->Clone("subHistBiasRemoved");
        bias = new TF1("bias","pol1",pedestal+4*MIP,maxRange);
        subHist->Fit(bias,"RQ0N");
        bias->SetRange(minRange,maxRange);
        subHistBiasRemoved->Add(bias,-1);
        subHistBiasRemoved->GetXaxis()->SetRangeUser(dynamic_cast<VetoThreshold*>(thresholdCollection->getElementAt(0))->getThreshold(),maxRange);
        Double_t doubleParticles = subHistBiasRemoved->Integral();

        streamlog_out(MESSAGE) << " double particle events: " << doubleParticles << std::endl;
        streamlog_out(MESSAGE) << " estimated fraction of double particles: " << doubleParticles/signalInt << std::endl;
        now = std::time(NULL);
        streamlog_out(MESSAGE) << "done " << std::asctime(std::localtime(&now));

#ifdef MULTI_DBINIT
#ifndef USE_CONDDB
#error Cannot use -DMULTI_DBINIT without using -DUSE_CONDB
#endif
        if (_thresholdFolderName != "" || _rejectedFractionFolderName != "" || _rejectedFractionReferenceFolderName != "")
          if (_startTime != 0 && _endTime != 0) {  // do not write Runs without events

            streamlog_out(MESSAGE) << " writing threshold results to database fodler: " << _thresholdFolderName << std::endl;
            lcio::LCTime startTime(_startTime);
            lcio::LCTime endTime(_endTime);
            streamlog_out(MESSAGE) << " results are valid from  " << startTime.getDateString() << " to " <<endTime.getDateString() << std::endl;
            if (_thresholdFolderName != "") {
              lccd::DBInterface db( MULTI_DBINIT , _thresholdFolderName, true );
              db.storeCollection( _startTime, _endTime, thresholdCollection, "multi thresholds calculated by multiCalibrator-Processor" );
            }
            if (_rejectedFractionFolderName != "") {
              lccd::DBInterface db( MULTI_DBINIT , _rejectedFractionFolderName, true );
              db.storeCollection( _startTime, _endTime, lossCollection, "fraction off rejected beam events for a cetain purity threshold" );
            }

            if (_rejectedFractionReferenceFolderName != "") {
              lccd::DBInterface db( MULTI_DBINIT , _rejectedFractionReferenceFolderName, true );
              db.storeCollection( _startTime, _endTime, referenceLossCollection, "fraction off rejected beam events for a cetain purity threshold" );
            }
         }
#endif
      }
      else {
        streamlog_out(ERROR) << "Signal or pedestal histogram is missing. Cannot calibrate" << std::endl;
      }

      std::ostringstream fileName;

      fileName << _outputFilePath << "Run" << _runNumber << "_multiCalib.root";

      streamlog_out(MESSAGE) << "Writing multi histograms to " << fileName << std::endl;

      TFile *file = new TFile(fileName.str().c_str(),"RECREATE");

      if (_pedHist) _pedHist->Write();
      if (_sigHist) _sigHist->Write();
      if (pureMIPhist) pureMIPhist->Write();
      if (oneMIPhist) oneMIPhist->Write();
      if (twoMIPhist) twoMIPhist->Write();
      if (fitHist) fitHist->Write();
      if (fitResultHist) fitResultHist->Write();
      if (fitHistFine) fitHistFine->Write();
      if (subHist) subHist->Write();
      if (subHistBiasRemoved) subHistBiasRemoved->Write();

      if (convoluter) convoluter->Write();
      file->Write();
      file->Close();

      if (g) delete g;
      if (l) delete l;
      if (gg) delete gg;
      if (ll) delete ll;
      if (bias) delete bias;

      if (convoluter) {
        streamlog_out(MESSAGE)  << " convoluter deleted " << std::endl;
        delete convoluter;
      }

    }
  }


}
