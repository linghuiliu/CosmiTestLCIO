#include <CellAnalysisProcessor.hh>
#include <sstream>
#include <streamlog/streamlog.h>
#include <HcalTileIndex.hh>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/LCCollection.h>
#include <EVENT/Cluster.h>
#include "UTIL/LCRelationNavigator.h"
#include <marlin/Exceptions.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "UTIL/CellIDEncoder.h"
#include "UTIL/CellIDDecoder.h"

#include "MappingProcessor.hh"
#include "AhcAmplitude.hh"
#include "HcalCellIndex.hh"

#include "TDirectory.h"
#include "TMath.h"

using namespace marlin;
using namespace lcio;
using namespace CALICE;
using namespace UTIL;

using std::endl;
using std::cout;

CellAnalysisProcessor aCellAnalysisProcessor;

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
CellAnalysisProcessor::CellAnalysisProcessor() : Processor("CellAnalysisProcessor") 
{
  _description = "";
  
  registerInputCollection(LCIO::CALORIMETERHIT, "InputMuonHcalColName",
			  "Name of the muon HCAL collection", 
			  _inputMuonHcalColName, std::string("MuonHcalHits"));

  registerInputCollection(LCIO::CALORIMETERHIT, "InputMuonTcmtColName",
			  "Name of the muon TCMT collection", 
			  _inputMuonTcmtColName, std::string("MuonTcmtHits"));
  
  // The hit energy spectrum for each cell
  registerProcessorParameter("HitEnergyHistoBins", "HistoBins",
			     _hitenergyhistobins, int(40));
  
  registerProcessorParameter("HitEnergyHistoMin", "HistoMin",
			     _hitenergyhistomin, float(0.1));
  
  registerProcessorParameter("HitEnergyHistoMax", "HistoMax",
			     _hitenergyhistomax, float(5));
  
  registerProcessorParameter("RootFileName", "RootFileName", _rootfilename,
			     std::string("single-channel-histos.root"));

  registerProcessorParameter("calibrationSpectra", "calibrationSpectra",
			     _calibrationSpectra, int(0));
  
  registerProcessorParameter("UseTCMT", "Use TCMT input hits", _useTCMT,
			     bool(false));
            
  registerProcessorParameter("MappingProcessorName",
			     "Name of the MappingProcessor",
			     _mappingProcessorName, std::string("HcalMappingProcessor"));
 
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
CellAnalysisProcessor::~CellAnalysisProcessor() 
{
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void CellAnalysisProcessor::openInputCollections(LCEvent* evt) 
{
  try {
    _hcalInputCol = evt->getCollection(_inputMuonHcalColName);
    streamlog_out(DEBUG) << "HCAL collection: " << _inputMuonHcalColName<<", "
		 <<_hcalInputCol->getNumberOfElements()<<" elements" << endl;
  } 
  catch (DataNotAvailableException &e) 
    {
      streamlog_out(DEBUG)<<"Could not open "<<_inputMuonHcalColName<<" collection"<<endl;
      throw marlin::SkipEventException(this);
    }

  /*-------------------------------------------------------------------*/
  try {
    if (_useTCMT == true) 
      {
	_tcmtInputCol = evt->getCollection(_inputMuonTcmtColName);
	streamlog_out(DEBUG) << "TCMT collection: " << _inputMuonTcmtColName<<", "
		     <<_tcmtInputCol->getNumberOfElements()<<" elements" << endl;
	
      }
  } 
  catch (DataNotAvailableException &e) 
    {
      streamlog_out(DEBUG)<<"Could not open "<<_inputMuonTcmtColName<<" collection"<<endl;
      throw marlin::SkipEventException(this);
    }
  
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void CellAnalysisProcessor::init() 
{
  printParameters();
  
  std::stringstream message;
  bool error = false;

  _ahcMapper = dynamic_cast<const AhcMapper*> (MappingProcessor::getMapper(_mappingProcessorName));
  if (!_ahcMapper)
    {
      message << "MappingProcessor::getMapper(" << _mappingProcessorName
	      << ") did not return a valid mapper." << std::endl;
      error = true;      
    }
  if (error) 
    {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

  /*-------------------------------------------------------------------------------*/
  _rootout = new TFile(_rootfilename.c_str(), "RECREATE");

//   _tree = new TTree(_treeName.c_str(), _treeName.c_str());
//   _tree->Branch("hcalEsum", &_hcalEsum, "hcalEsum/F");
//   _tree->Branch("hcalHitEnergy", &_hcalHitEnergy, "hcalHitEnergy/F");
//   _tree->Branch("hcalNHits", &_hcalHitEnergy, "hcalHitEnergy/F");
  

  if (_useTCMT) 
    {
      TDirectory* tcmtDir = _rootout->mkdir("TCMT");

      // Create the hit energy spectrum histograms for ech tcmt strip
      for (unsigned int layer = 0; layer != MAXTCMTLAYERS; ++layer) 
	{	
	  tcmtDir->cd();
	  std::ostringstream directory_name_stream;
	  directory_name_stream << "layer " << layer + 1;
	  std::string directory_name = directory_name_stream.str();
	  tcmtDir->mkdir(directory_name.c_str())->cd();


	  std::ostringstream histoLayerNameStream;
	  histoLayerNameStream << "tcmtlayer: " << std::setw(2)
			       << std::setfill('0') << layer + 1;
	  
	  std::string histoLayerName = histoLayerNameStream.str();
	  _tcmtlayerenergyhistos[layer] = new TH1F(histoLayerName.c_str(),
						   histoLayerName.c_str(), _hitenergyhistobins,
						   _hitenergyhistomin, _hitenergyhistomax);
	  
	  for (unsigned int strip = 0; strip != MAXTCMTSTRIPS; ++strip) 
	    {
	      std::ostringstream histoNameStream;
	      histoNameStream << "tcmtlayer: " << std::setw(2)
			      << std::setfill('0') << layer + 1 << " strip: "
			      << std::setw(2) << std::setfill('0') << strip;
	      
	      std::string histoName = histoNameStream.str();
	      _tcmtenergyhistos[layer][strip] = new TH1F(histoName.c_str(),
							 histoName.c_str(), _hitenergyhistobins,
							 _hitenergyhistomin, _hitenergyhistomax);
	    }
	}
    }
  
  _rootout->cd();
  TDirectory* ahcalDir = _rootout->mkdir("AHCAL");
  _hMuonHcalESum = new TH1F("hMuonHcalESum", "hMuonHcalESum", 5000, 0, 200);

  // Create the hit energy spectrum histogram for each cell
  for (unsigned int module = 0; module != MAXNRMODULES; ++module) 
    {
      ahcalDir->cd();
      std::ostringstream directory_name_stream;
      directory_name_stream << "module " << module + 1;
      std::string directory_name = directory_name_stream.str();
      ahcalDir->mkdir(directory_name.c_str())->cd();


      std::ostringstream histo_nameMod_stream;
      histo_nameMod_stream << "module: " << std::setw(2) << std::setfill('0')
			   << module + 1;
      
      std::string histo_nameMod = histo_nameMod_stream.str();
      
      _hitenergyPerModulehistos[module] = new TH1F(histo_nameMod.c_str(),
						   histo_nameMod.c_str(), _hitenergyhistobins, _hitenergyhistomin,
						   _hitenergyhistomax);
      
      for (unsigned int chip = 0; chip != MAXNRCHIPS; ++chip) 
	{
	  for (unsigned int channel = 0; channel != MAXNRCHANNELS; ++channel) {
	    std::ostringstream histo_name_stream;
	    
	    histo_name_stream << "module: " << std::setw(2)
			      << std::setfill('0') << module + 1 << " chip: "
			      << std::setw(2) << std::setfill('0') << chip
			      << " channel: " << std::setw(2) << std::setfill('0')
			      << channel;
	    
	    std::string histo_name = histo_name_stream.str();
	    
	    streamlog_out(DEBUG0) << "Creating histogram: " << histo_name
				  << std::endl;
	    
	    _hitenergyhistos[module][chip][channel] = new TH1F(histo_name.c_str(), histo_name.c_str(),
							       _hitenergyhistobins, _hitenergyhistomin,
							       _hitenergyhistomax);
	    
	  } // end of loop over channel
	} // end of loop over chip
    }// end of loop over module
  
  

}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void CellAnalysisProcessor::processEvent(LCEvent* evt) 
{
  openInputCollections(evt);

  /*---------------------------------------------------------------------------
   TCMT*/
  
  
  CellIDDecoder<CalorimeterHit>* tcmtDecoder = 0;
  if (_useTCMT) 
    {
      LCTypedVector<CalorimeterHit> tcmtHitsVec(_tcmtInputCol);  
      streamlog_out(DEBUG0) << "TCMT hits " << tcmtHitsVec.size() << endl;

      tcmtDecoder = new CellIDDecoder<CalorimeterHit> (_tcmtInputCol);

      for (unsigned int i = 0; i < tcmtHitsVec.size(); i++) 
	{
	  CalorimeterHit *Hit = tcmtHitsVec[i];
	  const int I = (*tcmtDecoder)(Hit)["I"];
	  const int J = (*tcmtDecoder)(Hit)["J"];
	  const int K = (*tcmtDecoder)(Hit)["K-1"];
	  const int layer = K;
	  int strip = 0;
	  if (I != 0)
	    strip = I - 1;
	  if (J != 0)
	    strip = J - 1;
	  streamlog_out(DEBUG0) << "Found a TCMT hit. Layer: " << layer << ", "
			<< " Strip: " << strip << endl;
	  
	  float energy = Hit->getEnergy();
	  _tcmtlayerenergyhistos[layer-1]->Fill(energy);
	  _tcmtenergyhistos[layer-1][strip]->Fill(energy);
	  
	}/*end loop over i*/      
    }
  
  
  /*---------------------------------------------------------------------------
   HCAL*/
  LCCollection *relCol = evt->getCollection("AhcHitAmplitudeRelation");
  if (relCol == NULL) 
    {
      cout << " no relation collection, aborting" << endl;
      exit(1);
    }
  UTIL::LCRelationNavigator navigator(relCol);

  /*Angela Lucaci: I know, bad idea to hard-code the collection name.
    But I dislike more the idea of adding yet another steering parameter,
    so I keep it like this for the moment.*/
  LCCollection *allHcalHitsCol = evt->getCollection("AhcCalorimeter_Hits");

  LCTypedVector<CalorimeterHit> ahcalHitsVec(_hcalInputCol);  
  streamlog_out(DEBUG0) << "AHCAL hits " << ahcalHitsVec.size() << endl;

  unsigned int numHcalHits = ahcalHitsVec.size();

//   float esum = 0;

//   /*---------------------------------------------------------------------*/
//   unsigned int numHcalHits = ahcalHitsVec.size();
//   int maxHcalLayerNumber = 0;
  
//   float numHitsPerLayer[MAXNRMODULES] = {0};
//   float energyPerLayer [MAXNRMODULES] = {0};
//   CellIDDecoder<CalorimeterHit>* decoder = new CellIDDecoder<CalorimeterHit> (_hcalInputCol);
//   unsigned int countCentralTiles = 0;

//   for (unsigned int i = 0; i < numHcalHits; i++) 
//     {
//       CalorimeterHit *Hit = ahcalHitsVec[i];
//       const int K = (*decoder)(Hit)["K-1"] + 1;
//       if (maxHcalLayerNumber < K)
// 	maxHcalLayerNumber = K;
//       numHitsPerLayer[K-1]++;
//       energyPerLayer[K-1] += Hit->getEnergy();

//       const int I = (*decoder)(Hit)["I"];
//       const int J = (*decoder)(Hit)["J"];
      
//       bool isCentralTile = ( (I==40 || I==43 || I==46 || I==49) && (J==40 || J==43 || J==46 || J==49));
//       if (_useCentralTilesOnly == true && isCentralTile == true) countCentralTiles++;

//       //cout<<"     lntai I/J/K="<<I<<"/"<<J<<"/"<<K<<endl;  
//     }

//   if (_useCentralTilesOnly == true && countCentralTiles != numHcalHits)
//     {
//       streamlog_out(DEBUG)<<" non-central tiles, skip event"<<endl;
//       throw marlin::SkipEventException(this);
//     }

//   float median = TMath::Median(MAXNRMODULES, energyPerLayer);
//   bool isPunchThoughPion = false;
//   bool hasTooManyHitsPerLayer = false;
//   for (unsigned int i = 0; i < MAXNRMODULES; ++i)
//     {
//       if (energyPerLayer[i] > 3*median) isPunchThoughPion = true;
//       if (numHitsPerLayer[i] > _maxNumberOfHitsPerHcalLayer)
// 	{
// 	  hasTooManyHitsPerLayer = true;
// 	  break;
// 	}
//     }

//   streamlog_out(DEBUG)<<"\n Event "<<evt->getEventNumber()<<" numHcalHits: "<<numHcalHits
//       <<" minNumberOfHitsInHcal: "<<_minNumberOfHitsInHcal
//       <<" maxHcalLayerNumber: "<<maxHcalLayerNumber
//       <<" minHcalLayerNumber: "<<_minHcalLayerNumber<<endl;

//   if (!(numHcalHits >= (unsigned)_minNumberOfHitsInHcal && maxHcalLayerNumber>=_minHcalLayerNumber 
// 	&& (hasTooManyHitsPerLayer==false)
// 	&& (isPunchThoughPion==false) 
// 	))
//     {
//       streamlog_out(DEBUG)<<"Probably a pion, skipping"<<endl;
//       //return;
//       throw marlin::SkipEventException(this);
//     }


  /*---------------------------------------------------------------------*/
  for (unsigned int i = 0; i < numHcalHits; i++) 
    {
      CalorimeterHit *Hit = ahcalHitsVec[i];
      const int cellID = Hit->getCellID0();
      const unsigned int module  = _ahcMapper->getModuleFromCellID(cellID);
      const unsigned int chip    = _ahcMapper->getChipFromCellID(cellID);
      const unsigned int channel = _ahcMapper->getChanFromCellID(cellID);
      
      if (module == 0) continue;
      
      const float energy = Hit->getEnergy();
      
      float amplRawPedSubst  = 0.;
      float energyNoTNoSat   = 0.;
      float energyWithTNoSat = 0.;

      CalorimeterHit *originalHcalHit = NULL;

      for (int iAll = 0; iAll < allHcalHitsCol->getNumberOfElements(); ++iAll)
	{
	  CalorimeterHit *originalHit = dynamic_cast<CalorimeterHit*>(allHcalHitsCol->getElementAt(iAll));
	  if (originalHit->getCellID0() == cellID) 
	    {
	      originalHcalHit = originalHit;
	      break;
	    }
	}/*end loop over iAll*/

      if (originalHcalHit == NULL)
	{
	  cout<<"\n Sorry, something is wrong, no original HCAL hit found"<<endl;
	  exit(1);
	}

      const LCObjectVec &amplVec = navigator.getRelatedToObjects(originalHcalHit);
      //streamlog_out(DEBUG)<<"  amplVec.size="<<amplVec.size()<<endl;
      if (amplVec.size() > 0) 
	{
	  LCGenericObject *obj  = dynamic_cast<LCGenericObject*> (amplVec[0]);
	  AhcAmplitude *ahcAmpl = new AhcAmplitude(obj);
	  amplRawPedSubst = ahcAmpl->getAmplRawMinusPedestalADC();
	  energyNoTNoSat = ahcAmpl->getAmplNOTTemperatureCorrMIP();
	  energyWithTNoSat = ahcAmpl->getAmplTemperatureCorrMIP();
	  delete ahcAmpl;
	}
      
      // without temperature correction	      
      streamlog_out(DEBUG1) << "Filling histogram: "
			    << "module: " << std::setw(2) << std::setfill('0')
			    << module << " chip: " << std::setw(2)
			    << std::setfill('0') << chip << " channel: "
			    << std::setw(2) << std::setfill('0') << channel
			    << std::endl;

     
      //cout << "amplitude adc - pedestal: " << amplRawPedSubst << endl;
      
      
      if (_calibrationSpectra == 3) 
	{
	  _hitenergyhistos[module - 1][chip][channel]->Fill(energyWithTNoSat);
	  _hitenergyPerModulehistos[module - 1]->Fill(energyWithTNoSat);
// 	  esum += energyWithTNoSat;
	  //cout << energyNoTNoSat << " mod/chip/chan " << module-1 << " / " << chip << " / " << channel << endl;
	}
      
      else if (_calibrationSpectra == 2) 
	{
	  _hitenergyhistos[module - 1][chip][channel]->Fill(energyNoTNoSat);
	  _hitenergyPerModulehistos[module - 1]->Fill(energyNoTNoSat);
// 	  esum += energyNoTNoSat;
	  //cout << energyNoTNoSat << " mod/chip/chan " << module-1 << " / " << chip << " / " << channel << endl;
	}
      
      // store calibration spectra in MIP units
      else if (_calibrationSpectra == 0) 
	{
	  _hitenergyhistos[module - 1][chip][channel]->Fill(energy);
	  _hitenergyPerModulehistos[module - 1]->Fill(energy);
// 	  esum += energy;
	  //cout << energy << endl;
	}
      // store calibration spectra in ADC units
      else if (_calibrationSpectra == 1) 
	{
	  _hitenergyhistos[module - 1][chip][channel]->Fill(amplRawPedSubst);
	  _hitenergyPerModulehistos[module - 1]->Fill(amplRawPedSubst);
// 	  esum += amplRawPedSubst;
	}
    }/*end loop over input HCAL calorimeter hits
       ---------------------------------------------------------------------------------*/
  
//   streamlog_out(DEBUG)<<"esum: "<<esum<<endl;

//   _hMuonHcalESum->Fill(esum);

//   _hcalEsum = esum;
//   _tree->Fill();


  /* make sure the tree is saved once in a while*/
  if (evt->getEventNumber()%1000 == 0) 
    {
//       _tree->Write("", TObject::kOverwrite);
      _rootout->Write("", TObject::kOverwrite);
    }
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void CellAnalysisProcessor::end() 
{
//   _tree->Write("", TObject::kOverwrite);
  _rootout->Write("", TObject::kOverwrite);
  _rootout->Close();

}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void CellAnalysisProcessor::writeHistograms() {
  if (_useTCMT) 
    {
      for (unsigned int layer = 0; layer != MAXTCMTLAYERS; ++layer) 
	{
	  _tcmtlayerenergyhistos[layer]->Write("", TObject::kOverwrite);

	  for (unsigned int strip = 0; strip != MAXTCMTSTRIPS; ++strip) 
	    {
	      _tcmtenergyhistos[layer][strip]->Write("", TObject::kOverwrite);
	    }
	}
    }
  
  for (unsigned int module = 0; module != MAXNRMODULES; ++module) 
    {
     _hitenergyPerModulehistos[module]->Write("", TObject::kOverwrite);

      for (unsigned int chip = 0; chip != MAXNRCHIPS; ++chip) 
	{
	  for (unsigned int channel = 0; channel != MAXNRCHANNELS; ++channel) 
	    {
	      streamlog_out(DEBUG0) << "Writing histogram for " << "module: "
				    << std::setw(2) << std::setfill('0') << module + 1
				    << " chip: " << std::setw(2) << std::setfill('0')
				    << chip << " channel: " << std::setw(2)
				    << std::setfill('0') << channel << std::endl;
	      
	      _hitenergyhistos[module][chip][channel]->Write("", TObject::kOverwrite);
	      
	    }
	}
    }
}
