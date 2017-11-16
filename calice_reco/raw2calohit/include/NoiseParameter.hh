#ifndef NoiseParameter_hh
#define NoiseParameter_hh

#include <sstream>

namespace CALICE {//namespace


  /** Information about a pad: Pedestal, noise, etc.
   * This class hold the pedestal, the noise, and a flag which indicates whether the pad is considered to be dead, in function of the geometrical indices. It will be used by both data and MC
   */
  class NoiseParameter
  {
  public:
    NoiseParameter();
    NoiseParameter(int cellID0, int module_index, int cell_index, double noise, double pedestal);
    NoiseParameter(int cellID0, int module_index, int cell_index);
    NoiseParameter(int module_index, int cell_index);
    
    ~NoiseParameter();

    /** Set the pedestal. 
     */
    void setPedestal(double aPedestal);

    /** Get the pedestal.
     */
    double getPedestal() const;


    /** Set the pedestal before Signal Induced Corrections are applied. 
     */
    void setPedestalBeforeSIC(double aOld_pedestal);

    /** Set the pedestal before Global Corrections are applied. 
     */
    void setPedestalBeforeGC(double aOld_pedestal);

    /** Get the pedestal before Signal Induced Corrections are applied. 
     */
    double getPedestalBeforeSIC() const;

    /** Get the pedestal before Global Corrections are applied. 
     */
    double getPedestalBeforeGC() const;


    /** Set the noise.
     */
    void setNoise(double aNoise);
    void setCoherentNoise(double aCohNoise);
    void setNoiseBeforeSIC(double aOld_noise);
    void setNoiseBeforeGC(double aOld_noise);

    /** Get the noise.
     * The result is undefined before the pedestals and the noise are calculated.
     */
    double getNoise() const;
    double getCoherentNoise() const;
    double getNoiseBeforeSIC() const;
    double getNoiseBeforeGC() const;

    /** Return kTRUE if the pad is considered to be dead.
     */
    short isDead() const;

    /** Declare the pad to be dead (or revive pad (is_dead=0).
     * 1=dead, 2=bad, 3=disconnected
     */
    void setDead(short is_dead=1);


    void setModuleIndex(unsigned int aModule_index);
    /** Get the ModuleIndex.
     * The result is undefined before the ModuleIndexs and the noise are calculated.
     */
    unsigned int getModuleIndex() const;
    /** Set the CellIndex. 
     */
    void setCellIndex(unsigned int aCell_index);
    /** Get the CellIndex.
     * The result is undefined before the CellIndexs and the noise are calculated.
     */
    unsigned int getCellIndex() const;

    /** Get and set the Mokka-like geometrical index */
    void setGeomCellIndex(int aCellID0);
    int getGeomCellIndex() const;

    enum {numberOfInts=4};
    const int* intData() const;
    int* intData();

    enum {numberOfFloats=0};
    const float* floatData() const;
    float* floatData();

    enum {numberOfDoubles=7};
    const double* doubleData() const;
    double* doubleData();

  private:
    int _isDead;
    int _moduleIndex;
    int _cellIndex;
    int _cellID0;
    double _pedestalBeforeGC;
    double _noiseBeforeGC;
    double _pedestalBeforeSIC;
    double _noiseBeforeSIC;
    double _pedestal;
    double _noise;
    double _cohnoise;
    


  };
}//namespace
#endif
