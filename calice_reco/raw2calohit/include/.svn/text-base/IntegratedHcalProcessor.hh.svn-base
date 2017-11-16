#ifndef IntegratedHcalProcessor_h
#define IntegratedHcalProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// FIXME: no magic numbers please
#define HCAL_N_MOD 39
#define HCAL_N_CELL 216

#include <cstdlib>
#include <map>

#include <lcio.h>
#include <lccd.h>

#include <SiPmPropertiesProcessor.hh>
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/ConditionsProcessor.h>
#include <ConditionsChangeDelegator.hh>
#include <FastCaliceHit.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>
#include <VRawADCValueProcessor.hh>
#include <collection_names.hh>
#include <HcalTempModel.hh>
#include <CalibrationSet.hh>
#include <GainConstants.hh>
#include <InterConstants.hh>
#include <MIPConstants.hh>
#include <MappingAndAlignment.hh>
#include <HcalModuleIndexReverseLookup.hh>

#include "AhcTempProvider.hh"
#include "AhcSimpleTempProvider.hh"

#include "lccd/ConditionsMap.hh"
#include "HcalTileIndex.hh"
#include "LinearFitConstant.hh"
#include "LinearFitSlope.hh"
#include "SimpleValue.hh"
#include "SatCorrItep.hh"

namespace CALICE {

  /** Base class for all processors which need a lot of AHcal specific
   *  data, like the CALICE::IntegratedHcalCalibrationProcessor and
   *  the CALICE::IntegratedHcalDigitizationProcessor.
   *
   * @author Sebastian Schmidt
   *
   */

class IntegratedHcalProcessor : public SiPmPropertiesProcessor {
 public:
  IntegratedHcalProcessor(const std::string processorName="IntegratedHcalProcessor");
  virtual ~IntegratedHcalProcessor();

  IntegratedHcalProcessor* newProcessor() {return new IntegratedHcalProcessor;}

  virtual void init();

 protected:
  bool _doMipTempCorr;
  bool _doGainTempCorr;

  /// Use this map to count how often the cell specified with the HcalTileIndex
  /// occurs during calibration or digitization in the whole run.
  std::map<unsigned,unsigned> _cellOccurenceCounter;
  
  /// Use this map to count how often the cell specified with the HcalTileIndex
  /// is calibrated or digitized with a temperature corrected Mip constant.
  std::map<unsigned,unsigned> _cellMipTempCorrCounter;

  /// Use this map to count how often the cell specified with the HcalTileIndex
  /// is calibrated or digitized using a Mip constant without temperature
  /// correction.
  std::map<unsigned,unsigned> _cellNoMipTempCorrCounter;

  /// Use this map to count how often the cell specified with the HcalTileIndex
  /// is calibrated or digitized with a temperature corrected Gain constant.
  std::map<unsigned,unsigned> _cellGainTempCorrCounter;
  
  /// Use this map to count how often the cell specified with the HcalTileIndex
  /// is calibrated or digitized using a Gain constant without temperature
  /// correction.
  std::map<unsigned,unsigned> _cellNoGainTempCorrCounter;

  /// This flags shows whether the current cell is calibrated or digitized
  /// with a temperature corrected mip constant or not. This should be set and 
  /// only set in \ref getMip. USE WITH CARE!
  bool _cellCalibUsedMipTempCorr;

  /// This flags shows whether the current cell is calibrated or digitized
  /// with a temperature corrected mip constant or not. This should be set and 
  /// only set in \ref getGain. USE WITH CARE!
  bool _cellCalibUsedGainTempCorr;


  /** \brief Is used to fill temperature correction statistic maps
    */
  void fillTempCountMaps( int cellID );
  void fillTempCountMaps(unsigned module, unsigned chip, unsigned channel);

  /** \brief Is used to print temperature correction statistic maps
    */
  void printTempCountMaps( std::ostream& out );

  std::string _gainCalColName;
  std::string _interCalColName;
  std::string _mipCalColName;
  std::string _ahcSroModDataColName;
  std::string _colNameModuleDescription;
  std::string _colNameModuleLocation;
  std::string _colNameModuleConnection;

  std::string _temperatureSensorCalibrationColName;

  AhcTempProvider* _tempProvider;

//  lccd::ConditionsMap<int, CALICE::LinearFitResult > _gainFitResultsMap;
//  lccd::ConditionsMap<int, CALICE::LinearFitResult > _MipFitResultsMap;

  lccd::ConditionsMap<int, CALICE::LinearFitConstant > _gainFitConstantsMap;
  lccd::ConditionsMap<int, CALICE::LinearFitSlope >    _gainFitSlopesMap;

  lccd::ConditionsMap<int, CALICE::LinearFitConstant > _MipFitConstantsMap;
  lccd::ConditionsMap<int, CALICE::LinearFitSlope >    _MipFitSlopesMap;  

  lccd::ConditionsMap<const int, CALICE::SatCorrItep > _SatCorrMap;
  

  lccd::ConditionsMap<int, CALICE::LinearFitConstant > _InterConstantsMap; 

  std::string _gainFitConstantsCollectionName;
  std::string _MipFitConstantsCollectionName;

  std::string _SatCorrCollectionName;

  std::string _gainFitSlopesCollectionName;
  std::string _MipFitSlopesCollectionName;

  std::string _InterConstantsCollectionName;

///  CalibrationSet<GainConstants>* _gainCalibSet;
///  CalibrationSet<InterConstants>* _interCalibSet;
///  CalibrationSet<MIPConstants>* _mipCalibSet;  
  IntVec _modules;
  float _gainScalingFactor;
  float _mipScalingFactor;
  
  HcalTempModel* _tempModel;
  MappingAndAlignment _mapping;
  HcalModuleIndexReverseLookup _indexLookup;

  int _viewConnectionTree;
///  int _fudgeNonExistingSaturationCorrections;
///
///  bool _correctSatLY;
///  
///  //  float _lightyield[HCAL_N_MOD+1][HCAL_N_CELL];
///  //  float _lightyieldError[HCAL_N_MOD+1][HCAL_N_CELL];

  typedef struct {
    unsigned short module;
    unsigned short cellkey;
    float weight;
  } NeighbourItem;

  typedef std::map<unsigned, std::vector<NeighbourItem>* > NeighbourMap;
  std::map<unsigned,unsigned> _inverseModuleMap;
  std::map<unsigned,unsigned> _cellMap;
  NeighbourMap _neighbourMap;
    
///  ConditionsChangeDelegator<IntegratedHcalProcessor> _gainCalibrationChange;
///  ConditionsChangeDelegator<IntegratedHcalProcessor> _interCalibrationChange;
///  ConditionsChangeDelegator<IntegratedHcalProcessor> _mipCalibrationChange;
  ConditionsChangeDelegator<IntegratedHcalProcessor> _ahcSroModDataChange;

  ConditionsChangeDelegator<IntegratedHcalProcessor> _temperatureSensorCalibrationChange;

  ConditionsChangeDelegator<IntegratedHcalProcessor> _moduleTypeChange;
  ConditionsChangeDelegator<IntegratedHcalProcessor> _moduleLocationChange;
  ConditionsChangeDelegator<IntegratedHcalProcessor> _moduleConnectionChange;
  lcio::LCCollection* _ahcSroModDataCol;

//  virtual void gainCalibrationChanged(lcio::LCCollection* col);
//  virtual void interCalibrationChanged(lcio::LCCollection* col);
//  virtual void mipCalibrationChanged(lcio::LCCollection* col);
  virtual void ahcSroModDataColChanged(lcio::LCCollection* col);

  virtual void temperatureSensorCalibrationChanged(lcio::LCCollection* col);

  virtual void SipmInfoChanged(lcio::LCCollection* col);
  virtual void SipmSaturationChanged(lcio::LCCollection* col);
  virtual void ModuleProductionChanged(lcio::LCCollection* col); 
  virtual void moduleTypeChanged(lcio::LCCollection* col);
  virtual void moduleLocationChanged(lcio::LCCollection* col);
  virtual void moduleConnectionChanged(lcio::LCCollection* col);

  /** Use this function to get the Mip constant of a cell. This
   *  includes temperature correction.
   */
  float getMip( int CellID );
///  float getMip(unsigned int module, unsigned int chip, unsigned int channel);

  float getCorrectedAmplitude( int cellID, float satAmpl );
  float getSaturatedAmplitude( int cellID, float linAmpl );

  /** Use this function to get the Gain constant of a cell. This
   *  includes temperature correction.
   */
  float getGain( int cellID );
///  float getGain(unsigned int module, unsigned int chip, unsigned int channel);

  float getIC( int cellID );
///  float getIC(unsigned int module, unsigned int chip, unsigned int channel);

  float getCellTemp( int cellID );
  float getCellTemp(unsigned int module, unsigned int chip, unsigned int channel);

  //first moduleID, second cellkey
  std::pair<unsigned int, unsigned int> getModuleIDandCellkey(unsigned int module,
                                           unsigned int chip,
                                           unsigned int channel);

  //  void calculateLightYield();
  
  void updateGeometryMaps();

  std::vector<NeighbourItem>* getNeighbourList(const unsigned short module, const unsigned cellkey) const;

  int reverseLookup(int I, int J, int K) const;

  int geometricalLookup(int module, int chip, int channel);

private:
  int _doSatScaling;
  void considerNeighbourRelation(const unsigned centralCellIndex, const int rowOffset, const int columnOffset);
  void establishTileCoverage(const unsigned centralCellIndex, const int rowOffset, const int columnOffset);
};

}

#endif
