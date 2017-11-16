#ifndef AhcSimpleTempProvider_h
#define AhcSimpleTempProvider_h

#include <cmath>

#include "AhcSlowReadoutModBlock.hh"
#include "AhcTempProvider.hh"
#include "AhcTempSensorIndex.hh"

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
   */
  
  class AhcSimpleTempProvider : public AhcTempProvider
  {
  public:
    AhcSimpleTempProvider()
      : _moduleMeanTemp(N_MODULES,0),
	_moduleMeanTempRMS(N_MODULES,0),
	_moduleMeanCount(N_MODULES,0)
    {}
    
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
    
    float _caloMeanTemp;
    float _caloMeanTempRMS;
    int   _caloMeanCount;
    
    std::vector<float> _moduleMeanTemp;
    std::vector<float> _moduleMeanTempRMS;
    std::vector<int>   _moduleMeanCount;
  };
  
}

#endif
