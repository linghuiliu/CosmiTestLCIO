#ifndef CalorimeterProfileProcessor_hh
#define CalorimeterProfileProcessor_hh 1

// Marlin includes
#include "marlin/Processor.h"

// lcio includes
#include "EVENT/LCEvent.h"

// c++ includes
#include <string>
#include <vector>
#include <map>
#include <limits>

// CALICE includes
#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"
#include "VirtualCells.hh"
#include "BinnedVector.hh"

namespace CALICE {

  /** Processor to generate 3D shower profiles and to calculate the correct normalization.
  *
  * @author B. Lutz
  * @date 2010
  *
  */
  class CalorimeterProfileProcessor : marlin::Processor {

  public:


    virtual Processor*  newProcessor() { return new CalorimeterProfileProcessor; }

    /*
     * Constructor contains parameter registration.
     */
    CalorimeterProfileProcessor() ;

    /*
     * Called at the beginning of the job before anything is read.
     */
    virtual void init() ;

    /*
     * Called for every event - the working horse.
     */
    virtual void processEvent( EVENT::LCEvent * evt ) ;

    /*
     * Called after data processing for clean up.
     */
    virtual void end() ;

    static float _debugSSZ;


  protected:

    const Mapper *_mapper;

    MappedContainer<VirtualCells>* _virtualCells;
    MappedContainer<CellDescription>* _cellDescriptions;

    /*
     */
    unsigned int getIndexForValue(const std::map<float, unsigned int> &lookup, const float value) const;

    /* calculate radial and longitudinal normalization (add 1 to all bins covered by event)
     */
    void calculateNorm(FloatVec &rNorm, FloatVec &lNorm, const std::map<float, unsigned int> &rIndex, const std::map<float, unsigned int> &lIndex, float xOffset=0, float yOffset=0, float zOffset=0);

    /* calculate 2D normalization (add 1 to all bins covered by event)
     */
    void calculateNorm( BinnedVector<float, BinnedVector<float, float> > &norm, BinnedVector<float, BinnedVector<float, float> > &normLowBound, BinnedVector<float, BinnedVector<float, float> > &normHighBound, BinnedVector<float, float> &rNorm, BinnedVector<float, float> &lNorm, const float xOffset=0, const float yOffset=0, const float zOffset=0, const float lowBoundOffset = 0, const float highBoundOffset = 0, bool calculateBounds = false);

    /* convert 2D BinnedVector to 1D FloatVec for appending to .slcio collection
     */
    void fillLinearFloatVec( const BinnedVector<float, BinnedVector<float,float> >& input, FloatVec& output) const;

    /* convert 2D BinnedVector to 1D FloatVec for appending to .slcio collection
     */
    void fillLinearCenterFloatVec( const BinnedVector<float, BinnedVector<float,float> >& input, FloatVec& output_center1, FloatVec& output_center2 ) const;

  private:

    /*
     */
    void initGlobalPosition();

    bool _firstEvent;

    unsigned int _virtualCellsVersion;
    std::vector<VirtualCells*> _allVirtualCells;

    std::string _mappingProcessorName;
    std::string _cellDescriptionProcessorName;
    std::string _virtualCellsProcessorName;
    std::string _showerStartProcessorName;
    std::string _cogXYParameterName;
    std::string _caloColName;
    std::string _simCaloColName;

    float _threshold;
    float _simEnergyScaleFactor;

    IntVec   _middleCellIndex;
    IntVec   _middleCellIndexBack;
    IntVec   _zRange;
    FloatVec _rBinning;
    FloatVec _zOffset;

    unsigned int _nZBins;
    float _zMin;
    float _zMax;
    float _zMinStart;
    float _zMaxStart;

    float _zMinDetectorAcceptance;
    float _zMaxDetectorAcceptance;
    int _suppressOutsideFirstAndLastLayerCenter;

    unsigned int _nRBins;
    float _rMax;

    FloatVec _rPos;
    FloatVec _lPos;
    FloatVec _lStartPos;

    FloatVec _2DrPos;
    FloatVec _2DrStartPos;
    FloatVec _2DrCogPos;
    FloatVec _2DlPos;
    FloatVec _2DlStartPos;
    FloatVec _2DlCogPos;

    FloatVec _rGlobalNorm;
    FloatVec _lGlobalNorm;
    FloatVec _globalNorm;


  };

} // end namespace CALICE

#endif
