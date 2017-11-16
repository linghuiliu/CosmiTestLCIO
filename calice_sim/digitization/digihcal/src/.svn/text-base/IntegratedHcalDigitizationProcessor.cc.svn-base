#include "IntegratedHcalDigitizationProcessor.hh"

#include "FastCaliceHit.hh"
#include "HcalTileIndex.hh"

#include "streamlog/streamlog.h"
#include "marlin/Exceptions.h"

#include "EVENT/LCEvent.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCFlagImpl.h"
#include "UTIL/CellIDEncoder.h"

#include <string>
#include <map>
#include <cmath>

using namespace CALICE::AHCAL::Digitization;

IntegratedHcalDigitizationProcessor aIntegratedHcalDigitizationProcessor;

IntegratedHcalDigitizationProcessor::IntegratedHcalDigitizationProcessor() :

 IntegratedHcalProcessor("IntegratedHcalDigitizationProcessor")

{

  _description = "AHcal: Converts the energy deposition per cell to "
                 "hardware units while simulating various "
                 "detector effects like: optical crosstalk, "
                 "pixel statistic, saturation and noise.";

  registerProcessorParameter("InputCollectionName", "Name of the input collection",
                             _inputColName, std::string("CaliceHitsLevel1"));
			    
  registerProcessorParameter("OutputCollectionName", "Name of the output collection",
                             _outputColName, std::string("CaliceHitsLevel2"));
  
  registerProcessorParameter("NoiseCollectionName", "Name of the noise collection", 
                             _noiseColName, std::string("NoiseHits"));

  registerProcessorParameter("RandomSeed", "Seed for random number generator",
			     _randomSeed, int(0));

  registerProcessorParameter("MipPerGeV", "MPV of raw energy depositied by a "
                             "minimum ionizing particle",
			     _mipGeV, float(.000861));

  registerProcessorParameter("LightLeakage", "Fraction of "
                             "optical crosstalk/leakage out of cells",
			     _leakage, float(.1));

  registerProcessorParameter("DoStat", 
                             "Toggle the smearing of the number of firing "
                             "pixel with a binomial statistic",
			     _doStat,
                             bool(true));

}


IntegratedHcalDigitizationProcessor::~IntegratedHcalDigitizationProcessor() {

}


void IntegratedHcalDigitizationProcessor::init() {

  streamlog_out(DEBUG) << "init() called\n";

  //  printParameters();

  /* Never forget to init your base processor!
   */
  IntegratedHcalProcessor::init();

  initRandomGenerator();

  streamlog_out(DEBUG) << "init() finished" << std::endl;
}



void IntegratedHcalDigitizationProcessor::initRandomGenerator() {

  _pRandomGenerator = new TRandom3(_randomSeed);

}



void IntegratedHcalDigitizationProcessor::processEvent(lcio::LCEvent* evt) {

  /* ATTENTION: DO _NOT_ uncomment any of these functions. This is
     just a way of splitting code. If you want to turn off optical
     crosstalk for example turn the steering parameter to zero.

     The reason for this is that I wanted to save loops so in the
     simulateOpticalCrosstalk function a copy of all hits to an
     internal map is done which would be missing if you comment this function.

     I'm sorry for this.
   */

  streamlog_out(DEBUG) << "processEvent() started\n";

  openInputCollections(evt);

  simulateOpticalCrosstalk();

  simulateSiPMBehaviour();

  mergeHitsAndNoise();
  
  createOutputCollections(evt);
  
  tidyUp();

  streamlog_out(DEBUG) << "processEvent() finished" << std::endl;

}



void IntegratedHcalDigitizationProcessor::openInputCollections(lcio::LCEvent* evt) {

  streamlog_out(DEBUG) << "openInputCollections() called\n";

  try{

    _gangedMCCol = evt->getCollection(_inputColName);

    _pIncomingCellIDDecoder = 
      new lcio::CellIDDecoder<lcio::CalorimeterHit>(_gangedMCCol);

  } catch (lcio::DataNotAvailableException& e) {

    //streamlog_out(WARNING) << e.what() << std::endl;

    streamlog_out(DEBUG4) << "Missing input collection. Event will only contain noise." << std::endl;

    //    throw marlin::SkipEventException(this);

  }

  if(parameterSet("NoiseCollectionName") == true) {

    try{

      _noiseCol = evt->getCollection(_noiseColName);

    }
    catch (lcio::DataNotAvailableException &e) {
      
      streamlog_out(MESSAGE) << e.what() << '\n'
                             << "no noise data available in this event\n";

      streamlog_out(WARNING) << "This event will be skipped.\n";

      tidyUp();

      throw marlin::SkipEventException(this);

    }

  }

  streamlog_out(DEBUG) << "openInputCollections() finished" << std::endl;


}



void IntegratedHcalDigitizationProcessor::simulateOpticalCrosstalk() {

  if(_gangedMCCol != 0) {

  // loop over all ganged hits
  for(int hitnr = 0; hitnr != _gangedMCCol->getNumberOfElements(); ++hitnr) {
  
    lcio::CalorimeterHit* pCurrentHit = 
      dynamic_cast<lcio::CalorimeterHit*>(_gangedMCCol->getElementAt(hitnr));

    const unsigned int module =  (*_pIncomingCellIDDecoder)(pCurrentHit)["module"];
    const unsigned int chip =    (*_pIncomingCellIDDecoder)(pCurrentHit)["chip"];
    const unsigned int channel = (*_pIncomingCellIDDecoder)(pCurrentHit)["channel"];

    const unsigned int I = (*_pIncomingCellIDDecoder)(pCurrentHit)["I"];
    const unsigned int J = (*_pIncomingCellIDDecoder)(pCurrentHit)["J"];
    const unsigned int K = (*_pIncomingCellIDDecoder)(pCurrentHit)["K-1"] + 1;

    const HcalTileIndex hcalTileIndex(module, chip, channel);

    const double tilesize = getCellSize(module,chip,channel);

    const float energy = pCurrentHit->getEnergy();

    streamlog_out(DEBUG) << "new hit: \n"
                         << "module: " << module
                         << " chip: " << chip
                         << " channel: " << channel << '\n'
                         << "I: " << I
                         << " J: " << J
                         << " K: " << K << '\n'
                         << "tilesize: " << tilesize << '\n'
                         << "energy [GeV]: " << energy << std::endl;

    std::vector<std::pair<unsigned int, unsigned int> > neighbours;

    // find neighbors
    if(hcalTileIndex.getModuleType() == 0 || 
       hcalTileIndex.getModuleType() == 1) {

      neighbours = _aFineModule.getNeighbours(I,J);

    } else if(hcalTileIndex.getModuleType() == 2 || 
              hcalTileIndex.getModuleType() == 3) {

      neighbours = _aCoarseModule.getNeighbours(I,J);

    }

    // add primary hit to internal map. Store the energy in MIP.
    //
    // Although the energy is distributed to the neighbors this step is correct.
    // The optical crosstalk changes the mip scale in GeV.
    _hcalTileIndexToAmplitude[hcalTileIndex.getIndex()] += energy / _mipGeV;

    streamlog_out(DEBUG) << "this hit has neighbours: \n";
    
    for(unsigned int neighbournr = 0; neighbournr != neighbours.size();
        ++neighbournr) {
    
      const unsigned int neighbour_I = neighbours[neighbournr].first;
    
      const unsigned int neighbour_J = neighbours[neighbournr].second;
    
      // we are distribute only within a layer
      const unsigned int neighbour_K = K;
    
      const HcalTileIndex neighbour_hcalTileIndex (reverseLookup(neighbour_I, 
                                                                 neighbour_J, 
                                                                 neighbour_K)
                                                   );
      
      const double neighbour_tilesize =  getCellSize(neighbour_hcalTileIndex.getModule(),
                                                    neighbour_hcalTileIndex.getChip(),
                                                    neighbour_hcalTileIndex.getChannel());
        
      // simplification because of the regular layout of the hcal cells
      double fraction(1.0/4.0); 

      if(neighbour_tilesize > tilesize) {

        fraction *= 1;

      } else {

        fraction *= neighbour_tilesize / tilesize;

      }

      streamlog_out(DEBUG1) << "neighbour_I: " << neighbour_I << '\n'
                            << "neighbour_J: " << neighbour_J << '\n'
                            << "neighbour_K: " << neighbour_K << '\n'
                            << "neighbour tilesize: " << neighbour_tilesize 
                            << '\n'
                            << "fraction: " << fraction << '\n'
                            << std::endl;

      // neighbors get a certain fraction of energy in MIP according
      // to the optical crosstalk/leakage factor and the side length
      // they have in common with the primary hit
      _hcalTileIndexToAmplitude[neighbour_hcalTileIndex.getIndex()] 
        += _leakage * fraction * energy / (_mipGeV * (1-_leakage));
      
    }
    
  }

  }

}



void IntegratedHcalDigitizationProcessor::simulateSiPMBehaviour() {

  // loop over the internal map where optical crosstalk has already
  // been simulated -> the maps holds energy deposited in MIP
  for(std::map<int,double>::iterator it = _hcalTileIndexToAmplitude.begin();
      it != _hcalTileIndexToAmplitude.end();
      ++it) {

    int cellID = (*it).first;

    const float gain  =  
      IntegratedHcalProcessor::getGain( cellID );
    const float inter =  
      IntegratedHcalProcessor::getIC( cellID );
    float mip         =  
      IntegratedHcalProcessor::getMip( cellID );

    /* Niels.Meyer@desy.de : 
     *  I don't really know why this cut is necessary, but it seems
     *  not unreasonable to exclude wrong MIP values - so let's leave
     *  it in for the moment...
     */
    // stupid fall back
    if(mip > 1000) {

      mip = 0;

    }

    const double lightYield = mip/gain * inter;
    


    double energyInMIP = (*it).second;

    const double linearPixel = lightYield * energyInMIP;

    double saturatedPixel = 
      IntegratedHcalProcessor::getSaturatedAmplitude( cellID, 
						      linearPixel );

    streamlog_out(DEBUG2) << "--------------------------------------\n"
                          << "energyInMIP:           "<< energyInMIP           
                          << '\n'
                          << "lightYield:            "<< lightYield            
                          << '\n'
                          << "linearPixel:           "<< linearPixel           
                          << '\n'
                          << "saturatedPixel:        "<< saturatedPixel
			  << std::endl;


    // smear the number of pixel with a binomial statistic
    if( _doStat == true ) {

      /* Niels.meyer@desy.de
       *  the total number of pixel is only needed for the Poisson
       *  (statistical) spread of the resulting amplitude. Querying the
       *  response curve for a high linear amplitude is just fine
       */
      int ntot = 
	static_cast<int>( IntegratedHcalProcessor::getSaturatedAmplitude( cellID,
								       10000 )
			  );
      
      double p = saturatedPixel / ntot;
      
      //ROOT returns 0 if probability is bigger than 1
      if(p > 1) {
	
	p = 1;
	
      }
      
      saturatedPixel = _pRandomGenerator->Binomial( ntot, p );

      streamlog_out( DEBUG2 ) << "total number of pixel: "<< ntot
			      << '\n'
			      << "probability:           "<< p
			      << '\n'
			      << "randomSaturatedPixels: "<< saturatedPixel
			      << std::endl;
    }

    double ADC_Counts(0);
    
    if(lightYield > 0) {
      
      ADC_Counts = ( saturatedPixel / lightYield ) * mip;
      
    }
    
    // conservative maximum number of ADC channels the hardware provides
    const unsigned int MAXADC = 33000;
    
    if(ADC_Counts > MAXADC) {

      streamlog_out(WARNING) << "Maximum number of ADC reached: " << ADC_Counts 
                             << '\n'
                             << "Limiting to: " << MAXADC << std::endl;

      ADC_Counts = MAXADC;

    }


    streamlog_out( DEBUG2 ) << "ADC_Counts:            "<< ADC_Counts
			    << std::endl;

    // now we store the ADC channels per channel
    _hcalTileIndexToAmplitude[ cellID ] = ADC_Counts;

  }
}



void IntegratedHcalDigitizationProcessor::mergeHitsAndNoise() {

  streamlog_out(DEBUG) << "mergeHitsAndNoise() called\n";

  if(_noiseCol != 0) {

    // the collection should always hold 7608 channels for a complete hcal
    for (int i = 0; i != _noiseCol->getNumberOfElements(); ++i) {
    
      CALICE::FastCaliceHit* pNoise = 
        dynamic_cast<CALICE::FastCaliceHit*>(_noiseCol->getElementAt(i));

      const double noise_ADCs = pNoise->getEnergyValue();

      int hti_index = pNoise->getCellID();
      
      // for every noise hit we add the digitized hit on top
      _hcalTileIndexToAmplitude[hti_index] += noise_ADCs;

    }

  }

  streamlog_out(DEBUG) << "mergeHitsAndNoise() finished" << std::endl;

}



void IntegratedHcalDigitizationProcessor::createOutputCollections(lcio::LCEvent* evt) {

  LCCollectionVec* pOutputCol = new LCCollectionVec(LCIO::RAWCALORIMETERHIT);

  std::string encodingString(CALICE::HcalTileIndex::getEncodingString(0));

  lcio::CellIDEncoder<lcio::RawCalorimeterHitImpl> 
                                       outgoingCellIDEncoder(encodingString, 
                                                             pOutputCol);
  lcio::LCFlagImpl colFlag(pOutputCol->getFlag());

  colFlag.unsetBit(LCIO::CHBIT_ID1);

  pOutputCol->setFlag(colFlag.getFlag());

  for(std::map<int,double>::const_iterator it = 
            _hcalTileIndexToAmplitude.begin();
      it != _hcalTileIndexToAmplitude.end();
      ++it) {

    int hti = (*it).first;
    float energy =  (*it).second;

    FastCaliceHit* pNewHit = new FastCaliceHit(hti, energy, 0, 0);
    
    if(energy != 0) {
      pOutputCol->addElement(pNewHit);
    }

  }

  evt->addCollection(pOutputCol, _outputColName);

}




void IntegratedHcalDigitizationProcessor::tidyUp() {

  streamlog_out(DEBUG) << "tidyUp() called\n";

  if(_pIncomingCellIDDecoder != 0) {

    delete _pIncomingCellIDDecoder;

  }

  if(_pOutgoingCellIDEncoder != 0) {

    delete _pOutgoingCellIDEncoder;

  }
     
  _hcalTileIndexToAmplitude.clear();
  
  _gangedMCCol = 0;
  _noiseCol = 0;

  _pIncomingCellIDDecoder = 0;

  _pOutgoingCellIDEncoder = 0;

  streamlog_out(DEBUG) << "tidyUp() finished" << std::endl;

}



void IntegratedHcalDigitizationProcessor::end() {

  streamlog_out(DEBUG) << "end() called\n";

  if(_pRandomGenerator != 0) {

    delete _pRandomGenerator;

  }

  streamlog_out(DEBUG) << "end() finished" << std::endl;

}
