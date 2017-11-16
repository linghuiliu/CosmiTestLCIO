#ifndef DIGITIZATION_PROCESSOR_HH
#define DIGITIZATION_PROCESSOR_HH

#include "TRandom3.h"

#include <map>

#include "marlin/Processor.h"
#include "lcio.h"
#include "lccd/ConditionsMap.hh"

#include "UTIL/CellIDDecoder.h"
#include "EVENT/CalorimeterHit.h"

#include "UTIL/CellIDEncoder.h"
#include "IMPL/CalorimeterHitImpl.h"

#include "NeighbourRelations/CoarseModule.hh"
#include "NeighbourRelations/FineModule.hh"

#include "IntegratedHcalProcessor.hh"

// forward declarations
namespace EVENT {
   
  class LCEvent;
  class LCCollection;  

}

/** The CALICE namespace
 */
namespace CALICE {
  /** namespace for AHcal
   */
  namespace AHCAL {
    /** namespace for Digitization
     */
    namespace Digitization {

      /**  
       * @brief Processor to digitized Monte Carlo hits.
       *
       * This DigitizationProcessor converts the energy deposition per
       * cell to hardware units while simulating various detector effects like:
       * optical crosstalk, pixel statistic, saturation and noise.
       *
       * 
       * @authors Sebastian Richter
       * @version 1.0
       */

      class IntegratedHcalDigitizationProcessor : public CALICE::IntegratedHcalProcessor 

      {

      public:

        IntegratedHcalDigitizationProcessor();
        ~IntegratedHcalDigitizationProcessor();

        IntegratedHcalDigitizationProcessor* newProcessor() { 
          return new IntegratedHcalDigitizationProcessor; 
        }

        /** Everything which has to be initialized once before the
            processing of the events can start goes here.

            The init of the base class processor
            (IntegratedHcalProcessor) is called (never forget it!).
           
            A random generator is set up: initRandomGenerator()
            
         */
        void init();

        /** This function is called for every event if the
            DigitizationProcessor is active.

            It starts a couple of functions like:

            @li openInputCollections()
            @li simulateOpticalCrosstalk()
            @li simulateSiPMBehaviour()
            @li mergeHitsAndNoise()
            @li createOutputCollections()

            and at the very end to clear all non-temperory maps etc.:

            @li tidyUp()

            If you want to know what's going on I suggest to go through
            all the functions in the given order.

        */
        void processEvent(lcio::LCEvent* evt);

        /** At the very end of Marlin this function is called.

            Think of it as the reverse of init(). Delete newed objects.
         */
        void end();

      private:

        /** Initialization of the random generator for pixel statistic
         *  simulation.
         */
        void initRandomGenerator();

        /** Open the input collections: incoming SimCalorimeterHits and noise
         *
         */
        void openInputCollections(lcio::LCEvent* evt);

        /** Hits are copied to a temporary map and optical crosstalk
         *  is simulated.
         */ 
        void simulateOpticalCrosstalk();

        /** In this function saturation and pixel statistic are simulated.
         *
         */
        void simulateSiPMBehaviour();

        /** The digitized event is add on top of the noise event.
         *
         */
        void mergeHitsAndNoise();

        /** Create the digitized + noise collection.
         *
         */
        void createOutputCollections(lcio::LCEvent* evt);

        /** Resets everything which is event specific.
         */
        void tidyUp();


//        /** from IntegratedHcalProcessor, SiPMPropertiesProcessor
//         */
//        double getSaturatedNumberOfPixel(double linearPixel,
//                                         int module, int chip, int channel) {
//
//          return linearPixel * getInverseSiPmSaturationCorrection(module,
//                                                                  chip,
//                                                                  channel,
//                                                                  linearPixel);
//
//        }

        /** from IntegratedHcalProcessor, SiPMPropertiesProcessor
         */
        double getCellSize(int module, int chip, int channel) {

          return getTileSize(module,chip,channel);

        }

        /** Name of the ganged collection which will be digitized.
         */
        std::string _inputColName;

        /** Name of the noise collection which will be overlayed.
         */
        std::string _noiseColName;
        
        /** Name of the digitized collection. Serves as an Input for
            the IntegratedHcalCalibrationProcessor. 
        */
        std::string _outputColName;

        /** Instance which can be asked about neighbour relations of a fine module.
         */
        NeighbourRelations::FineModule _aFineModule;

        /** Instance which can be asked about neighbour relations of a fine module.
         */
        NeighbourRelations::CoarseModule _aCoarseModule;

        /** To have he CellIDDecoder available in every member
            function we hold a pointer of it.
         */
        lcio::CellIDDecoder<lcio::CalorimeterHit>* _pIncomingCellIDDecoder;

        /** To have he CellIDEncoder available in every member
            function we hold a pointer of it.
         */
        lcio::CellIDEncoder<lcio::CalorimeterHitImpl>* _pOutgoingCellIDEncoder;

        /** Pointer to the incoming collection.
         */
        lcio::LCCollection* _gangedMCCol;    

        /** Pointer to the noise collection.
         */
        lcio::LCCollection* _noiseCol;

        /** Factor of optical crosstalk/leakage. For example 10%. Steerable.
         */
        float _leakage;

        /** Energy per MIP. Approx. 816 keV. Steerable.
         */
        float _mipGeV;

        /** Pointer to ROOTs random generator for pixel statistics
         */
        TRandom3* _pRandomGenerator;

        /** The random seed for the random generator. Steerable.
         */
        int _randomSeed;

        /** The main map. This holds the current amplitude. For
            performance its unit change during each step of
            digitization.
         */
        std::map<int,double> _hcalTileIndexToAmplitude;

        /** Toggle to simulate pixel statistics.
         */
        bool _doStat;

      };

    }

  }

}
#endif
