#ifndef AhcCern2010TempProvider_h
#define AhcCern2010TempProvider_h

#include "AhcTempProvider.hh"
#include "AhcMapper.hh"
//#include "AhcTempSensorIndex.hh"

namespace CALICE {
  
  /** \brief This is a temperature provider for the AHCAL.based on the studies
      of the temperature profiles from CERN 2010 data.
   
   */
  
  class AhcCern2010TempProvider : public AhcTempProvider
  {
  public:
    AhcCern2010TempProvider(AhcMapper const *mapper);
    
    //void  setAhcMapper( const AhcMapper &mapper );
    void  applyCorrection();
    float getSensorTemp( int module, int sensor );
    float getCellTemp( int module, int chip, int chan );
    float getCellTempError( int module, int chip, int chan );

    /** Return the average calorimeter temperature
     */
    float getAvgTemp();

    /**Return the average temperature for the specified module
     */
    float getAvgModuleTemp( unsigned module );

    /** Returns the closest sensor to cell connected to chip, chan according to
	DESY-THESIS-2008-050. For the connection between the tile, chip and temperature 
	sensors numbers, see 
	http://www.desy.de/~richters/IJ-to-chip-channel/        
     */
    int   getSensor( int chip, int chan );

  protected:
    void applyCorrectionForUnreasonableSensors(int module, int sensor);
    bool applyCorrectionForUnreasonableModules(int module, int sensor);
    
  private:
    bool isModuleOff( int module );
    double median(float a[], int n);

    AhcMapper const *_mapper;

    float _calorimeterAverageTemperature; /**<average temperature of the calorimeter*/ 
    float _calorimeterAverageTemperatureRMS;
    float _calorimeterAverageTemperatureCount;
   
    std::vector<float> _moduleAverageTemperature;
    std::vector<float> _moduleAverageTemperatureRMS;
    std::vector<int>   _moduleAverageTemperatureCount;
  };
  
}

#endif
