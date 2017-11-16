#include "Ahc2TimeSmearingProcessor.hh"

#include <iostream>
#include <vector>
#include <TMath.h>
#include <cstdlib>

// -- CALICE Header
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "Ahc2Calibrations.hh"
#include "Ahc2CalibrationsProcessor.hh"
#include "CellIterator.hh"
#include "Ahc2CalibrationStatusBits.hh"

// ---- LCIO Headers
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCFlagImpl.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include "UTIL/BitField64.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"

#include <TMath.h>

using namespace lcio;
using namespace marlin;
using namespace std;

namespace CALICE
{
  double GaussConv(double *x, double *par);
  double DoubleGaussConv(double *x, double *par);

  Ahc2TimeSmearingProcessor::Ahc2TimeSmearingProcessor() : Processor("Ahc2TimeSmearingProcessor")
  {

    _description = "AHCAL EPT digitisation, time smeering";

    // register steering parameters: name, description, class-variable, default value

    registerProcessorParameter("Input_Collection",
    "Name of Calorimeter Hit input collections",
    _calorimInpCollection,
    std::string("hcal_smeared") );

    registerProcessorParameter("Output_Collection" ,
    "Name of the Calorimeter Hit output collection with time smeering"  ,
    _calorimOutCollection,
    std::string("hcal_smeared_time") ) ;

    registerProcessorParameter( "MappingProcessorName" ,
    "Name of the MappingProcessor instance that provides"
    " the geometry of the detector." ,
    _mappingProcessorName,
    std::string("MyMappingProcessor") ) ;

    registerProcessorParameter("RandomSeed", "Seed for random number generator",
    _randomSeed, int(0));

    registerProcessorParameter("doTimeSmearing",
    "Time Smearing of the hit",
    _doTimeSmearing,
    bool(true));

    registerProcessorParameter("doTDCShift",
    "Take into account TDC shift",
    _doTDCShift,
    bool(false));

    StringVec ParaExample;
    ParaExample.push_back("Value1");
    ParaExample.push_back("Value2");

    registerProcessorParameter("SigmaEffectParameters",
    "Value of sigma for each bin of nHits",
    _ParaVector,
    ParaExample,
    ParaExample.size()) ;

    StringVec KExample;
    KExample.push_back("N");
    KExample.push_back("mu");
    KExample.push_back("sigma");

    registerProcessorParameter("SmearingMethod",
    "Method for smearing: SimpleGaus, DoubleGaus",
    _methodSmearing,
    std::string("SimpleGaus")) ;

    registerProcessorParameter( "TimeSmearingParameters" ,
    "Parameters used for smearing. SG: mu, sigma; DG: N1, mu1, sigma1, N2, mu2, sigma2;",
    _KVector,
    KExample,
    KExample.size()) ;

  }

  /************************************************************************************/

  void Ahc2TimeSmearingProcessor::init()
  {
    std::cout << "init Ahc2TimeSmearing called" << std::endl ;
    bool error = false;
    std::stringstream message;

    /*initialize the random generator*/
    _randomGenerator = new TRandom3(_randomSeed);

    _nRun = 0 ;
    _nEvt = 0 ;

    Max_nHits = 0;

    _SigmaVector.clear();
    m_function.clear();

    std::cout << "Ahc2TimeSmearing TDC shift parameters called" << std::endl ;

    //Process Parameters for TDC shift
    if( parameterSet( "SigmaEffectParameters" ) )
    {
      unsigned index = 0 ;
      while( index < _ParaVector.size() )
      {
        string strValue( _ParaVector[ index++ ] );
        float _value = atof(strValue.c_str());
        _SigmaVector.push_back(_value);
      }
    }

    //Create function with the sigma value of the bins in nHits
    _ShiftFcn = new TGraph();
    for(unsigned int i = 0; i < _SigmaVector.size(); i++)
    _ShiftFcn->SetPoint(i, i+1, _SigmaVector.at(i));

    std::cout << "Ahc2TimeSmearing smearing parameters called" << std::endl ;

    float xmin = -4000;
    float xmax = 4000;
    float step = 1;
    int npx = (xmax-xmin)/step;

    //Process layers and put then into a map
    if( parameterSet( "TimeSmearingParameters" ) )
    {
      unsigned index = 0 ;

      //Simple gaussian case
      if(strcmp(_methodSmearing.c_str(), "SimpleGaus") == 0)
      {
        cout << "Simple Gaussian Case" << endl;

        while( index < _KVector.size() )
        {
          string strMu( _KVector[ index++ ] );
          string strReso( _KVector[ index++ ] );

          _mu = atof(strMu.c_str());
          _reso = atof(strReso.c_str());

          for(unsigned int nHits = 0; nHits < 36; nHits++)
          {
            int bin = nHits + 1;
            if(!_doTDCShift && bin > 1) continue;

            if(m_function.count(bin) == 0)
            {
              TString fcn_name = TString::Format("Function_SimpleGaus_nHits%02i", bin);
              m_function[bin] = new TF1(fcn_name, GaussConv, xmin, xmax, 4);
              m_function[bin]->SetParNames("Norm", "mu", "sigma", "sigma_e");
              if(!_doTDCShift)
              {
                m_function[bin]->FixParameter(0., 1.);
                m_function[bin]->FixParameter(3., 0.);
              }
              else
              {
                m_function[bin]->FixParameter(0., 1.);
                m_function[bin]->FixParameter(3., _ShiftFcn->Eval(bin));
              }
              m_function[bin]->FixParameter(1., _mu);
              m_function[bin]->FixParameter(2., _reso);

              m_function[bin]->SetNpx(npx);
            }
          }
        }//end loop
      }//end case

      //Double gaussian case
      if(strcmp(_methodSmearing.c_str(), "DoubleGaus") == 0)
      {
        cout << "Double Gaussian Case" << endl;

        while( index < _KVector.size() )
        {
          string strNorm1( _KVector[ index++ ] );
          string strMu1( _KVector[ index++ ] );
          string strReso1( _KVector[ index++ ] );
          string strNorm2( _KVector[ index++ ] );
          string strMu2( _KVector[ index++ ] );
          string strReso2( _KVector[ index++ ] );

          _norm1 = atof(strNorm1.c_str());
          _mu1 = atof(strMu1.c_str());
          _reso1 = atof(strReso1.c_str());
          _norm2 = atof(strNorm2.c_str());
          _mu2 = atof(strMu2.c_str());
          _reso2 = atof(strReso2.c_str());

          for(unsigned int nHits = 0; nHits < 36; nHits++)
          {
            int bin = nHits + 1;
            if(!_doTDCShift && bin > 1) continue;

            if(m_function.count(bin) == 0)
            {
              TString fcn_name = TString::Format("Function_DoubleGaus_nHits%02i", bin);
              m_function[bin] = new TF1(fcn_name, DoubleGaussConv, xmin, xmax, 7);
              m_function[bin]->SetParNames("Norm1", "mu1", "sigma1", "Norm2", "mu2", "sigma2", "sigma_e");

              if(!_doTDCShift)
              {
                m_function[bin]->FixParameter(0., _norm1);
                m_function[bin]->FixParameter(3., _norm2);
                m_function[bin]->FixParameter(6., 0.);
              }
              else
              {
                m_function[bin]->FixParameter(0., _norm1);
                m_function[bin]->FixParameter(3., _norm2);
                m_function[bin]->FixParameter(6., _ShiftFcn->Eval(bin));
              }
              m_function[bin]->FixParameter(1., _mu1);
              m_function[bin]->FixParameter(2., _reso1);
              m_function[bin]->FixParameter(4., _mu2);
              m_function[bin]->FixParameter(5., _reso2);

              m_function[bin]->SetNpx(npx);
            }
          }
        }//end while loop
      }//end case
    }//end parameters

    _mapper = dynamic_cast<const Ahc2Mapper*>(MappingProcessor::getMapper(_mappingProcessorName));

    if ( !_mapper )
    {
      message << "MappingProcessor::getMapper("<< _mappingProcessorName
      << ") did not return a valid mapper." << endl;
      error = true;
    }

    printParameters();

    //fOut = new TFile("CheckTimeSmearing.root", "RECREATE");
    //hFillTimeSmeared = new TH1F("hFillTimeSmeared", "hFillTimeSmeared", 400, -200, 200);

    if (error)
    {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

    std::cout << "Init finished" << std::endl;
  }

  /************************************************************************************/
  void Ahc2TimeSmearingProcessor::processRunHeader( LCRunHeader* run)
  {
    _nRun++ ;
  }

  /************************************************************************************/

  void Ahc2TimeSmearingProcessor::processEvent( LCEvent * evt )
  {
    int evtNumber = evt->getEventNumber();
    streamlog_out(DEBUG) << " \n ---------> Event: " << evtNumber << "!!! <-------------\n" << std::endl;

    LCCollection *inputCalorimCollection = NULL;
    try{
      inputCalorimCollection = evt->getCollection( _calorimInpCollection );
    }
    catch ( EVENT::DataNotAvailableException &e )
    {
      streamlog_out(WARNING) << "Collection " << _calorimInpCollection << " not available, skip this event" <<std::endl;
      //throw marlin::SkipEventException(this);
      return; // allow lcio event go to next processors.
    }

    /*the Mokka encoding string*/
    _mokkaEncodingString = inputCalorimCollection->getParameters().getStringVal(LCIO::CellIDEncoding);
    streamlog_out(DEBUG) << " Encoding string " << _mokkaEncodingString << std::endl;

    _mapper->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

    bool hasKminus1 = false;
    if (_mokkaEncodingString.find("K-1") != std::string::npos)
    {
      hasKminus1 = true;
    }

    LCCollectionVec *outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    LCFlagImpl hitFlag(outputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_TIME);
    hitFlag.setBit(LCIO::CHBIT_LONG);
    outputCol->setFlag(hitFlag.getFlag());

    CellIDDecoder<CalorimeterHit> decoder(inputCalorimCollection);

    /*get number of hits per chip event parameters*/
    nHitsPerChipAboveThr.clear();
    m_ChipTrigger.clear();

    evt->getParameters().getIntVals("nHitsPerChipAboveThr", nHitsPerChipAboveThr);

    for(unsigned int v = 0; v < nHitsPerChipAboveThr.size(); v++)
    {
      int value = nHitsPerChipAboveThr.at(v);
      int index = value/100;
      int nHits = value%100;

      //cout << "Ahc2TimeSmearingProcessor, index: " << index << " nHits " << nHits << endl;
      m_ChipTrigger.insert(make_pair(index, nHits));//make new map
    }

    for (int iHit = 0; iHit < inputCalorimCollection->getNumberOfElements(); ++iHit)
    {
      CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inputCalorimCollection->getElementAt(iHit));

      int cellID = hit->getCellID0();

      int layer = 0;
      if (hasKminus1) layer = decoder(hit)["K-1"] + 1;
      else layer = decoder(hit)["K"];

      /*---------------------- simulate electronics timing resolution ---------------------------*/
      //Time Smearing with a gaussian of a certain resolution defined by the user
      float time = hit->getTime();
      float timeSmeared = 0.;

      if(_doTimeSmearing)
      {
        if(_doTDCShift)
        {
          //TDC shift parametrization
          int Chip = _mapper->getChipFromCellID(hit->getCellID0());
          int index = layer*100+Chip;

          if(m_ChipTrigger.find(index) == m_ChipTrigger.end())
          {
            streamlog_out(WARNING) << "\n---------------" << std::endl;
            streamlog_out(WARNING) << " Skipping hit (no chip trigger)" << std::endl;
            streamlog_out(WARNING) << " currentHit with cell ID " << cellID <<", energyInMIPs "<< hit->getEnergy()
            << ", time " << hit->getTime() << ", I/J/K " << _mapper->getDecoder()->getIFromCellID(cellID)
            << "/" << _mapper->getDecoder()->getJFromCellID(cellID)
            << "/" << _mapper->getDecoder()->getKFromCellID(cellID)
            << std::endl;
            continue;
          }
          else{
            int nHits = m_ChipTrigger[index];

            if(nHits > Max_nHits)
            Max_nHits = nHits;

            if(m_function.find(nHits) != m_function.end())
            timeSmeared = time + m_function[nHits]->GetRandom();
            else{
              streamlog_out(WARNING) << "\n---------------" << std::endl;
              streamlog_out(WARNING) << " Skipping hit (no time bin)" << std::endl;
              streamlog_out(WARNING) << " currentHit with cell ID " << cellID <<", energyInMIPs "<< hit->getEnergy()
              << ", time " << hit->getTime() << ", I/J/K " << _mapper->getDecoder()->getIFromCellID(cellID)
              << "/" << _mapper->getDecoder()->getJFromCellID(cellID)
              << "/" << _mapper->getDecoder()->getKFromCellID(cellID)
              << std::endl;
              continue;
            }
          }
        }
        else
        timeSmeared = time + m_function[1]->GetRandom();
      }
      else
      timeSmeared = time;

      CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
      newHit->setCellID0(hit->getCellID0());
      newHit->setEnergy(hit->getEnergy());
      newHit->setPosition(hit->getPosition());
      newHit->setTime(timeSmeared);

      outputCol->addElement(newHit);

      //hFillTimeSmeared->Fill(timeSmeared);
    }

    LCParameters &theParam = outputCol->parameters();
    theParam.setValue(LCIO::CellIDEncoding, _mokkaEncodingString);

    if (outputCol->getNumberOfElements() > 0)
    evt->addCollection(outputCol, _calorimOutCollection);

    _nEvt++;
  }

  /************************************************************************************/
  void Ahc2TimeSmearingProcessor::check( LCEvent * evt ) {
    // nothing to check here - could be used to fill checkplots in reconstruction processor
  }

  /************************************************************************************/
  void Ahc2TimeSmearingProcessor::end(){

    std::cout << "Ahc2TimeSmearingProcessor::end()  " << name()
    << " processed " << _nEvt << " events in " << _nRun << " runs "
    << std::endl ;

    cout << "Ahc2TimeSmearingProcessor: Maximum Number of hits in a chip " << Max_nHits << endl;

    //fOut->cd();
    //hFillTimeSmeared->Write();
    //fOut->Close();
  }

  /************************************************************************************/
  void Ahc2TimeSmearingProcessor::printParameters(){
    std::cerr << "============= Time Smearing Processor =================" <<std::endl;
    std::cerr << "Smearing simulation time by the electronics resolution" <<std::endl;
    std::cerr << "Input Collection name : " << _calorimInpCollection << std::endl;
    std::cerr << "Output Collection name : " << _calorimOutCollection << std::endl;
    std::cerr << "Time Smearing : " << _doTimeSmearing << std::endl;
    std::cerr << "Smearing Method : " << _methodSmearing << std::endl;
    if(strcmp(_methodSmearing.c_str(), "SimpleGaus") == 0)
    std::cerr << "Smearing parameters : " << _mu << " " << _reso << std::endl;
    if(strcmp(_methodSmearing.c_str(), "DoubleGaus") == 0)
    std::cerr << "Smearing parameters : " << _norm1 << " " << _mu1 << " " << _reso1 << " " << _norm2 << " " << _mu2 << " " << _reso2 << std::endl;
    std::cerr << "Effect TDC Shift : " << _doTDCShift << std::endl;
    std::cerr << "Parameters Effect TDC Shift : ";
    for(unsigned int i = 1; i <= 36; i++)
    std::cerr << _ShiftFcn->Eval(i) << " ";
    std::cerr << std::endl;
    return;
  }

  /************************************************************************************/

  double GaussConv(double *x, double *par)
  {
    double Norm = par[0]/( TMath::Sqrt( 2 * TMath::Pi() * ( par[2]*par[2] + par[3]*par[3] ) ) );
    double arg = (x[0] - par[1])*(x[0] - par[1])/( 2*( par[2]*par[2] + par[3]*par[3] ) );

    return Norm * TMath::Exp( - arg ) ;
  }

  /************************************************************************************/

  double DoubleGaussConv(double *x, double *par)
  {
    double Norm1 = par[0]/( TMath::Sqrt( 2 * TMath::Pi() * ( par[2]*par[2] + par[6]*par[6] ) ) );
    double arg1 = (x[0] - par[1])*(x[0] - par[1])/( 2*( par[2]*par[2] + par[6]*par[6] ) );

    double Norm2 = par[3]/( TMath::Sqrt( 2 * TMath::Pi() * ( par[5]*par[5] + par[6]*par[6] ) ) );
    double arg2 = (x[0] - par[4])*(x[0] - par[4])/( 2*( par[5]*par[5] + par[6]*par[6] ) );

    return Norm1 * TMath::Exp( - arg1 ) + Norm2 * TMath::Exp( - arg2 ) ;
  }

  /***************************************************************************************
  * create instance to make processor known to Marlin
  * should be very last thing to do, to prevent order problems during
  * deletion of static objects.
  ***************************************************************************************/
  Ahc2TimeSmearingProcessor aAhc2TimeSmearingProcessor;

}/*end of namespace CALICE*/
