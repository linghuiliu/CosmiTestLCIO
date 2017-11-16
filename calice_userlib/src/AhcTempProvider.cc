#include "AhcTempProvider.hh"

using namespace lcio;

namespace CALICE {
  
  AhcTempProvider::AhcTempProvider()
    : newSroModCol(true), newCalibCol(true), newSanityRange(true),
      _sanityMin(0), _sanityMax(300), _sroModCol(0), _calibCol(0)
  {
    sensorTemp.resize(N_MODULES);
    sensorTempError.resize(N_MODULES);
    sensorCalib.resize(N_MODULES);
    sensorCalibError.resize(N_MODULES);
    sensorCalibStatus.resize(   N_MODULES);
 
    for(unsigned i=0; i<N_MODULES; i++) {
      sensorTemp[i].resize(N_SENSORS,-1);
      sensorTempError[i].resize(N_SENSORS,-1);
      sensorCalib[i].resize(N_SENSORS,0);
      sensorCalibError[i].resize(N_SENSORS,0);
      sensorCalibStatus[i].resize(   N_SENSORS, 0);
    }
  }
  
  void AhcTempProvider::setAhcSroModBlocks( LCCollection* col )
  {
    _sroModCol = col;
    newSroModCol = true;
  }
  
  void AhcTempProvider::setCalibrations( LCCollection* col )
  {
    if(!col) throw std::logic_error("AhcTempProvider::setCalibrations(): "
                                    "Null pointer col!");
    
    _calibCol = col;
    newCalibCol = true;
    
    for( int i=0; i < col->getNumberOfElements(); i++ ) {
        CALICE::SimpleValue sv( col->getElementAt(i) );
	
        AhcTempSensorIndex idx( sv.getCellID() );
	
        sensorCalib[idx.getModule()-1][idx.getSensor()]       = sv.getValue();
        sensorCalibError[idx.getModule()-1][idx.getSensor()]  = sv.getError();
        sensorCalibStatus[idx.getModule()-1][idx.getSensor()] = sv.getStatus();

    }
  }
  
  void AhcTempProvider::setSanityRange( float min, float max )
  {
    _sanityMin = min;
    _sanityMax = max;
    newSanityRange = true;
  }
  
  void AhcTempProvider::updateCache()
  {
    if(!_sroModCol) throw std::invalid_argument( "AhcSimpleTempProvider::"
                                                 "updateCache(): "
                                                 " null pointer _sroModCol!" );
    
    //for each module
    for( int i=0; i<_sroModCol->getNumberOfElements(); i++ ) {
      AhcSlowReadoutModBlock mb( _sroModCol->getElementAt(i) );
      
      int module = mb.getModuleNumber()-1;
      
      if(mb.getCmbTemperatures().size()==0) {
	cout<<"WARNING no cmb temperatures for module "<<mb.getModuleNumber()
	    <<endl;
      } 
      for(unsigned s=0; s<N_SENSORS && s<mb.getCmbTemperatures().size(); s++)
        {
	  //get temperature
	  sensorTemp[module][s] = mb.getCmbTemperatures()[s];
	  
	  //calibrate 
            if( _calibCol ) {
	      sensorTemp[module][s] += sensorCalib[module][s];
            }
        }
    }
    
    newCalibCol = false;
    newSroModCol = false;
  }
  
}
