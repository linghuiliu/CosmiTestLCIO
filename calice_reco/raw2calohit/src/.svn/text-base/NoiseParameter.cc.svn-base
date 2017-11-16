#include <cassert>
#include <cmath>
#include <cstring>

#include "NoiseParameter.hh"

namespace CALICE {//namespace calice

  NoiseParameter::NoiseParameter(){

    
    //CRP: Also this is not needed, has to be removed in next release!!!!!
    //CAMM: kept it to be aligned with TBTrackUtil class.
    memset(this,0,sizeof(NoiseParameter));
    _isDead = 0;
    _moduleIndex = 0;
    _cellIndex = 0;
    _cellID0 = 0;
    _pedestalBeforeGC = -999;
    _noiseBeforeGC = 0;
    _pedestalBeforeSIC = -999;
    _noiseBeforeSIC = 0;
    _pedestal = -999;
    _noise = 0;
    _cohnoise = 0;
  }

  NoiseParameter::NoiseParameter(int module_index, int cell_index){
    assert(sizeof(NoiseParameter)==
	   sizeof(int)*numberOfInts+
	   sizeof(double)*numberOfDoubles);
    
    memset(this,0,sizeof(NoiseParameter));

    _isDead = 0;
    _moduleIndex = module_index;
    _cellIndex = cell_index;
    _cellID0 = 0;
    _pedestalBeforeGC = -999;
    _noiseBeforeGC = 0;
    _pedestalBeforeSIC = -999;
    _noiseBeforeSIC = 0;
    _pedestal = -999;
    _noise = 0;
    _cohnoise = 0;

  }

  NoiseParameter::NoiseParameter(int cellID0, int module_index, int cell_index){
    assert(sizeof(NoiseParameter)==
	   sizeof(int)*numberOfInts+
	   sizeof(double)*numberOfDoubles);
    
    memset(this,0,sizeof(NoiseParameter));

    _isDead = 0;
    _moduleIndex = module_index;
    _cellIndex = cell_index;
    _cellID0 = cellID0;
    _pedestalBeforeGC = -999;
    _noiseBeforeGC = 0;
    _pedestalBeforeSIC = -999;
    _noiseBeforeSIC = 0;
    _pedestal = -999;
    _noise = 0;
    _cohnoise = 0;

  }

  NoiseParameter::NoiseParameter(int cellID0, int module_index, int cell_index, double noise, double pedestal){

    assert(sizeof(NoiseParameter)==
	   sizeof(int)*numberOfInts+
	   sizeof(double)*numberOfDoubles);
    
    memset(this,0,sizeof(NoiseParameter));

    _isDead = 0;
    _moduleIndex = module_index;
    _cellIndex = cell_index;
    _cellID0 = cellID0;
    _pedestalBeforeGC = -999;
    _noiseBeforeGC = 0;
    _pedestalBeforeSIC = -999;
    _noiseBeforeSIC = 0;
    _pedestal = pedestal;
    _noise = noise;
    if (noise > 6.0) _cohnoise = sqrt(noise*noise-6.*6.);
    else _cohnoise = 0;

  }
    
  NoiseParameter::~NoiseParameter(){

  }

  void NoiseParameter::setPedestal(double aPedestal){
    _pedestal = aPedestal;
  }
  void NoiseParameter::setPedestalBeforeSIC(double aOld_pedestal){
    _pedestalBeforeSIC = aOld_pedestal;
  }

  void NoiseParameter::setPedestalBeforeGC(double aOld_pedestal){
    _pedestalBeforeGC = aOld_pedestal;
  }

  double NoiseParameter::getPedestal() const{
    return _pedestal;
  }



  double NoiseParameter::getPedestalBeforeSIC() const{
    return _pedestalBeforeSIC;
  }

  double NoiseParameter::getPedestalBeforeGC() const{
    return _pedestalBeforeGC;
  }


  void NoiseParameter::setNoise(double aNoise){
    _noise = aNoise;
    if (_noise > 6.0) _cohnoise = sqrt(_noise*_noise-6.*6.);
    else _cohnoise = 0;
  }

  void NoiseParameter::setCoherentNoise(double aCohNoise){
    _cohnoise = aCohNoise;
  }
  void NoiseParameter::setNoiseBeforeSIC(double aOld_noise){
    _noiseBeforeSIC = aOld_noise;
  }
  void NoiseParameter::setNoiseBeforeGC(double aOld_noise){
    _noiseBeforeGC = aOld_noise;
  }

  double NoiseParameter::getNoise() const{
    return _noise;
  }
  double NoiseParameter::getCoherentNoise() const{
    return _cohnoise;
  }
  double NoiseParameter::getNoiseBeforeSIC() const{
    return _noiseBeforeSIC;
  }
  double NoiseParameter::getNoiseBeforeGC() const{
    return _noiseBeforeGC;
  }

  short NoiseParameter::isDead() const{
    return static_cast<short>(_isDead);
  }

  void NoiseParameter::setDead(short is_dead){
    _isDead = is_dead;
  }


  void NoiseParameter::setModuleIndex(unsigned int aModule_index){
    _moduleIndex = aModule_index;
  }

  unsigned int NoiseParameter::getModuleIndex() const{
    return _moduleIndex;
  }

  void NoiseParameter::setCellIndex(unsigned int aCell_index){
    _cellIndex = aCell_index;
  }

  unsigned int NoiseParameter::getCellIndex() const{
    return _cellIndex;
  }

  void NoiseParameter::setGeomCellIndex(int aCellID0){
    _cellID0 = aCellID0;
  }
  int NoiseParameter::getGeomCellIndex() const{
    return _cellID0;
  }


  const int* NoiseParameter::intData() const {
    return &_isDead;
  }

  int* NoiseParameter::intData() {
    return &_isDead;
  }

  const float* NoiseParameter::floatData() const {
    return 0;
  }

  float* NoiseParameter::floatData() {
    return 0;
  }

  const double* NoiseParameter::doubleData() const {
    return &_pedestalBeforeGC;
  }

  double* NoiseParameter::doubleData() {
    return &_pedestalBeforeGC;
  }

}//namespace
