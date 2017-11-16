#ifndef AhcTempProvider_h
#define AhcTempProvider_h

#include <stdexcept>

#include "lcio.h"
#include "lccd.h"
#include "EVENT/LCCollection.h"

#include "SimpleValue.hh"
#include "AhcTempSensorIndex.hh"
#include "AhcSlowReadoutModBlock.hh"

namespace CALICE {
  
  /** \brief This is an abstract class defining an interface to access the
   *        temperatures in single AHCAL cells.
   *
   */
  
  class AhcTempProvider
  {
  public:
    /** \brief Default constructor
     */
    AhcTempProvider();
    
    /** \brief Default destructor
     */
    virtual ~AhcTempProvider() {}
    
    /** \brief Returns the temperature for the cell in the specified module
     *        connected to the given chip and channel.
     *
     * This is only the definition of the interface, a concrete funtion has
     * to be implemented in a derived class. Note: Modules are counted from
     * 1! The valid range for module numbers thus is 1 to 38.
     */
    virtual float getCellTemp( int module, int chip, int chan ) = 0;
    
    /** \brief Returns the error on the temperature for the cell in the
     *        specified module connected to the given chip and channel.
     *
     * This is only the definition of the interface, a concrete funtion has
     * to be implemented in a derived class. Note: Modules are counted from
     * 1! The valid range for module numbers thus is 1 to 38.
     */
    virtual float getCellTempError( int module, int chip, int chan ) = 0;
    
    /** \brief Set the collection of AhcSlowReadoutModBlock objects to extract
     *        the temperature data from.
     */
    virtual void setAhcSroModBlocks( lcio::LCCollection *col );
    
    
    /** \brief Returns the average calorimeter temperature.
     *  
     */
    virtual float getAvgTemp()=0;
    
    
    /** \brief Returns the average module temperature.
     *  
     */
    virtual float getAvgModuleTemp( unsigned module )=0;
    
    /** \brief Returns the temperature of the specified sensor in the given module
     *  
     */
    virtual float getSensorTemp(int module, int sensor )=0;
    
    /** \brief Set the collection of SimpleValue objects to use for the offset
     *        calibrations of the temperature sensors. 
     */
    virtual void setCalibrations( lcio::LCCollection *col );
    
    /** \brief Define a sanity range of temperature values that make sense.
     *
     * There are some dead sensors, e.g giving only the value 117 degree
     * Celsius, which makes no sense. Also a temperature of zero degree
     * Celsius is very unlikely. With this function you specify the range
     * in which temperatures are accepted. A sensible range would be 10 to
     * 50 degree Celsius for example. Note: This is an abstract class - how
     * data from temperature sensors giving values outside this range is
     * handled is only depended from the implementation of in the derived
     * class. It may completely ignore the range specified here.
     */
    virtual void setSanityRange( float min, float max );
    
    const static unsigned N_MODULES = 38;
    const static unsigned N_SENSORS = 5;
    
  protected:
    /** \brief This function updates the internal 'cache' for the temperature
     *        values from the five sensors and applies the calibration
     *        offsets, if available.
     *
     * It should be called everytime a new collection of temperatures or
     * calibrations have been at set or the sanity range has been changed,
     * i.e. if one of newSroModCol, newCalibCol or newSanityRange is true.
     */
    virtual void updateCache();
    
    /** If true this indicates that setSroModBlocks() was called and a new 
     * collection of AhcSroModBlock objects was set.
     *
     * Can be useful for caching to achieve speedup in processing.
     */
    bool newSroModCol;
    
    /** If true this indicates that setCalibrations() was called and a new 
     * collection of calibrations was set.
     *
     * Can be useful for caching to achieve speedup in processing.
     */
    bool newCalibCol;
    
    /** If true this indicates that setSanityRange() was called and a new 
     * sanity range was set.
     *
     * Can be useful for caching to achieve speedup in processing.
     */
    bool newSanityRange;
    
    /** This is a 'cache' for the values of the temperature sensors after
     * calibration and and correction. It is a vector of a vector, first key
     * is module (0...37) and second key is sensor number (0...4).
     * Note: this is only an abstract class, look at getCellTemp() and
     * applyCalibCorr() in your implentation to understand how and when it is
     * updated.
     */ 
    std::vector< std::vector< float > > sensorTemp;
    
    /** This is a 'cache' for the errors on the values of the temperature
     * sensors after calibration and and correction. It is a vector of a
     * vector, first key is module (0...37) and second key is sensor number
     * (0...4).
     * Note: this is only an abstract class, look at getCellTempError() and
     * applyCalibCorr() in your implentation to understand how and when it is
     * updated.
     */ 
    std::vector< std::vector< float > > sensorTempError;
    
    /** This is a 'cache' for the calibration offsets for the temperature
     * sensors. It is a vector of a vector, first key is module (0...37) and
     * second key is sensor number (0...4).
     * It is filled when setCalibrations() is called;
     */ 
    std::vector< std::vector< float > > sensorCalib;
    
    /** This is a 'cache' for the errors on the calibration offsets for the
     * temperature sensors. It is a vector of a vector, first key is module
     * (0...37) and second key is sensor number (0...4).
     * It is filled when setCalibrations() is called;
     */ 
    std::vector< std::vector< float > > sensorCalibError;
    
    std::vector< std::vector< int > > sensorCalibStatus; /**<Vector to save the status of the calibration offsets*/
    
    
    /** Lower limit of the sanity range
     */
    float _sanityMin;
    
    /** Upper limit of the sanity range
     */
    float _sanityMax;
    
    /** A pointer to the LCCollection containing the AhcSlowReadOut objects
     */
    lcio::LCCollection *_sroModCol;    
    
    /** A pointer to the LCCollection containing the calibration offsets objects
     */
    lcio::LCCollection *_calibCol;
    
  };
  
}

#endif
