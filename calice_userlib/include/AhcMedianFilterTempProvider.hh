#ifndef AhcMedianFilterTempProvider_h
#define AhcMedianFilterTempProvider_h

#include <cmath>

#include "AhcSlowReadoutModBlock.hh"
#include "AhcTempProvider.hh"
#include "AhcTempSensorIndex.hh"
#include "AhcMapper.hh"

namespace CALICE {
  
  /** \brief This is a very simple temperature provider for the ahcal.
   *
   * What it does:
   *  - the calibration set is applied
   *  - if a temperature sensor gives a value outside the defined sanity range
   *    the value is replaced by the average of the four sensors in that module
   *  - for each cell the temperature value from the 'closest' sensor, according
   *    to the table 3.1, DESY-THESIS-2008-050 is returned
   *
   * Also see the documentation for the several functions for more detailed
   * information.
   * This new temperature provider with median filter created:
   *   Date: August 2013
   *   Author: Sergey Morozov
   *  The temperature provider is based on the AhcSimpleTempProvider with some
   *  updates: 
   *  - all temperature sensors are set to the layer average temperature.  
   *  - the median filter to layer-by-layer profile has been introduced. This will
   *    help to remove the out-layers with wrong temperature measurements in some
   *    layers of AHCAL.
   *  Note: you need smooth (median filtered for example) MIP and gain temperature
   *        profiles in the MIP and gain calibrations to use this tempetarure 
   *        provider for data reconstruction!
   */
  
  class AhcMedianFilterTempProvider : public AhcTempProvider
  {
  public:
    AhcMedianFilterTempProvider(AhcMapper const *mapper);
    
    /** \brief Returns the temperature for the cell in the specified module
     *        connected to the given chip and channel.
     *
     * For each cell the temperature value from the 'closest' sensor, according
     * to the table 3.1, DESY-THESIS-2008-050 is returned.
     */
    virtual float getCellTemp( int module, int chip, int chan );
    
    /** \brief Returns the error on the temperature for the cell in the
     *        specified module connected to the given chip and channel.
     */
    virtual float getCellTempError( int module, int chip, int chan );
    
    /** \brief Returns the average calorimeter temperature.
     *  
     */
    virtual float getAvgTemp();
    
    
    /** \brief Returns the average module temperature.
     *  
     */
    virtual float getAvgModuleTemp( unsigned module );
    
    /** Return the temperature for the specified module and sensor
     */
    float getSensorTemp( int module, int sensor );
    
  protected:
    /** \brief This function applies the correction to the internal 'cache' for
     *        the temperature values from the five sensors.
     *
     * It should be called everytime a new collection of temperatures or
     * calibrations have been at set or the sanity range has been changed,
     * i.e. if one of newSroModCol, newCalibCol or newSanityRange is true.
     * This is done in getCellTemp() and getCellTempError().
     */
    virtual void applyCorrection();
    
    /** \brief Returns true if argument value is outside the sanity range.
     */
    bool isBadValue( float t );
    
  private:
    /** Returns the closest sensor to cell connected to chip, chan according to
     * DESY-THESIS-2008-050
     */
    int getSensor( int chip, int chan );
    
    double median(float a[], int n);
    
    AhcMapper const *_mapper;
    
    float _caloMeanTemp;
    float _caloMeanTempRMS;
    int   _caloMeanCount;
    
    std::vector<float> _moduleMeanTemp;
    std::vector<float> _moduleMeanTempRMS;
    std::vector<int>   _moduleMeanCount;
  };
  
}

#endif
