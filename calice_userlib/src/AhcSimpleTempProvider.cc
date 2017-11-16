#include "AhcSimpleTempProvider.hh"
#include <numeric>

namespace CALICE {
  
  float AhcSimpleTempProvider::getCellTemp( int module, int chip, int chan )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return sensorTemp.at(module-1).at(getSensor(chip,chan));
  }
  
  float AhcSimpleTempProvider::getCellTempError( int module, int chip, int chan )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return sensorTempError.at(module-1).at(getSensor(chip,chan));
  }
  
  
  float AhcSimpleTempProvider::getAvgModuleTemp( unsigned module )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return _moduleMeanTemp[module-1];   
  }
  
  
  float AhcSimpleTempProvider::getAvgTemp()
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return _caloMeanTemp;
  }
  
  
  float AhcSimpleTempProvider::getSensorTemp( int module, int sensor )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) 
      {
        applyCorrection();
      }
    
    if (sensor > 4) throw std::invalid_argument( "AhcSimpleTempProvider::"
                                                 "AhcSimpleTempProvider::getSensorTemp: "
                                                 " sensor out of range ( > 4)" );
    return sensorTemp.at(module-1).at(sensor);
  }
  
  
  
  /** Returns the closest sensor to cell connected to chip, chan according to
   * DESY-THESIS-2008-050
   */
  int AhcSimpleTempProvider::getSensor( int chip, int chan )
  {
    if( chip == 0 ) return 0;
    else if( (chip > 0) && (chip < 4) ) return 1;
    else if( (chip > 3) && (chip < 8) ) return 2;
    else if( (chip > 7) && (chip < 11) ) return 3;
    else if( chip == 11 ) return 4;
    
    throw std::out_of_range( "AhcSimpleTempProvider::getSensor(): "
			     "argument chip out of range (0...11)");
    
    return 0; //never happens logically 
  }
  
  bool AhcSimpleTempProvider::isBadValue( float t )
  {
    return (t<_sanityMin) || (t>_sanityMax);
  }
  
  void AhcSimpleTempProvider::applyCorrection()
  {
    if(!_sroModCol) throw std::invalid_argument( "AhcSimpleTempProvider::"
                                                 "applyCorrection(): "
                                                 " null pointer _sroModCol!" );
    
    //  throw an exception when no sensor calib is available - 
    //    this is essential to retain consistency for correction of T-drifts.
    //  Catch this exception in your code if you can live without absolute
    //    temperature values
    //    if(!_calibCol) throw std::invalid_argument( "AhcSimpleTempProvider::"
    //						"applyCorrection(): "
    //						" null pointer _calibCol!" );
    
    if( newCalibCol || newSroModCol ) {
      updateCache();
    }
    
    _caloMeanTemp=0;
    _caloMeanTempRMS=0;
    _caloMeanCount=0;
    
    //holds all the modules that do not have any sensor in sanity range
    vector<int> bad_mods;
    
    for( int i=0; i<_sroModCol->getNumberOfElements(); i++ ) {
      CALICE::AhcSlowReadoutModBlock mb( _sroModCol->getElementAt(i) );
      
      int module = mb.getModuleNumber()-1;
      _moduleMeanTemp[module]=0;
      _moduleMeanTempRMS[module]=0;
      _moduleMeanCount[module]=0;
      
      for(unsigned s=0; s<N_SENSORS && s<mb.getCmbTemperatures().size(); s++)
        {
	  //we calculate the mean, excluding bad sensors
	  if(!isBadValue(sensorTemp[module][s])) {
	    _moduleMeanTemp[module]    += sensorTemp[module][s];
	    _moduleMeanTempRMS[module] += sensorTemp[module][s] * sensorTemp[module][s];
	    _moduleMeanCount[module]++;
	    
	    _caloMeanTemp += sensorTemp[module][s];
	    _caloMeanTempRMS += sensorTemp[module][s] * sensorTemp[module][s];
	    _caloMeanCount++; 
	  }
        }
      
      if(_moduleMeanCount[module]>0) {
	_moduleMeanTemp[module] /= _moduleMeanCount[module];
	_moduleMeanTempRMS[module] /= _moduleMeanCount[module];
	_moduleMeanTempRMS[module] -= _moduleMeanTemp[module] * _moduleMeanTemp[module];
	_moduleMeanTempRMS[module] = sqrt( _moduleMeanTempRMS[module] );
	
	//now take out the bad guys and replace'em with the mean
	for( unsigned s=0; s<N_SENSORS; s++ ) {
	  if(isBadValue(sensorTemp[module][s])) {
	    sensorTemp[module][s]      = _moduleMeanTemp[module];
	    sensorTempError[module][s] = _moduleMeanTempRMS[module];
	  }
	}
      } else {
	bad_mods.push_back(module); 
      }
    }
    
    if(_caloMeanCount>0) {
      _caloMeanTemp /= _caloMeanCount;
      _caloMeanTempRMS /= _caloMeanCount;
    }
    
    for(unsigned i=0; i<bad_mods.size(); i++) {
      fill(sensorTemp[bad_mods[i]].begin(),sensorTemp[bad_mods[i]].end(), _caloMeanTemp );
      
      fill( sensorTempError[bad_mods[i]].begin(),
	    sensorTempError[bad_mods[i]].end(),
	    _caloMeanTempRMS
            );
    }
    
    newCalibCol = false;
    newSroModCol = false;
    newSanityRange = false;
  }
  
}
