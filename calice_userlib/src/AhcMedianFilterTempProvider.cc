#include "AhcMedianFilterTempProvider.hh"
#include <numeric>

namespace CALICE {
  
  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  AhcMedianFilterTempProvider::AhcMedianFilterTempProvider( AhcMapper const *mapper)
    :_moduleMeanTemp(AhcTempProvider::N_MODULES, 0),
     _moduleMeanTempRMS(AhcTempProvider::N_MODULES, 0),
     _moduleMeanCount(AhcTempProvider::N_MODULES, 0)
  {
    _mapper = mapper;
  }
  
  float AhcMedianFilterTempProvider::getCellTemp( int module, int chip, int chan )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return sensorTemp.at(module-1).at(getSensor(chip,chan));
    //return _caloMeanTemp;
  }
  
  float AhcMedianFilterTempProvider::getCellTempError( int module, int chip, int chan )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return sensorTempError.at(module-1).at(getSensor(chip,chan));
  }
  
  
  float AhcMedianFilterTempProvider::getAvgModuleTemp( unsigned module )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return _moduleMeanTemp[module-1];   
  }
  
  
  float AhcMedianFilterTempProvider::getAvgTemp()
  {
    if( newCalibCol || newSroModCol || newSanityRange ) {
      applyCorrection();
    }
    
    return _caloMeanTemp;
  }
  
  
  float AhcMedianFilterTempProvider::getSensorTemp( int module, int sensor )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) 
      {
        applyCorrection();
      }
    
    if (sensor > 4) throw std::invalid_argument( "AhcMedianFilterTempProvider::"
                                                 "AhcMedianFilterTempProvider::getSensorTemp: "
                                                 " sensor out of range ( > 4)" );
    return sensorTemp.at(module-1).at(sensor);
    //return _caloMeanTemp;
  }
  
  
  
  /** Returns the closest sensor to cell connected to chip, chan according to
   * DESY-THESIS-2008-050
   */
  int AhcMedianFilterTempProvider::getSensor( int chip, int chan )
  {
    if( chip == 0 ) return 0;
    else if( (chip > 0) && (chip < 4) ) return 1;
    else if( (chip > 3) && (chip < 8) ) return 2;
    else if( (chip > 7) && (chip < 11) ) return 3;
    else if( chip == 11 ) return 4;
    
    throw std::out_of_range( "AhcMedianFilterTempProvider::getSensor(): "
			     "argument chip out of range (0...11)");
    
    return 0; //never happens logically 
  }
  
  bool AhcMedianFilterTempProvider::isBadValue( float t )
  {
    return (t<_sanityMin) || (t>_sanityMax);
  }
  
  void AhcMedianFilterTempProvider::applyCorrection()
  {
    if(!_sroModCol) throw std::invalid_argument( "AhcMedianFilterTempProvider::"
                                                 "applyCorrection(): "
                                                 " null pointer _sroModCol!" );
    
    //  throw an exception when no sensor calib is available - 
    //    this is essential to retain consistency for correction of T-drifts.
    //  Catch this exception in your code if you can live without absolute
    //    temperature values
    //    if(!_calibCol) throw std::invalid_argument( "AhcMedianFilterTempProvider::"
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
      _moduleMeanTemp[bad_mods[i]] = _caloMeanTemp;
      _moduleMeanTempRMS[bad_mods[i]] = _caloMeanTempRMS;
      //int currentLayer = _mapper->getK(bad_mods[i]);
      //std::cout << "All sensors are bad in module = " << bad_mods[i] << ", layer = " << currentLayer << std::endl;
    }
    
    //averaging:
    // 1) make median filter with 3 layer window from 2 to 37 layers
    // 2) set average per module to all sensors
 
    //const unsigned maxK = _mapper->getMaxK();
    int module2layer[39];
    int layer2module[39];
  
    module2layer[0] = 0;
    layer2module[0] = 0;
 
    for( unsigned module=1; module<=38; module++ ) {
      unsigned layer = _mapper->getK(module);
      //std::cout << "module,layer" << module << "," << layer << std::endl;
      module2layer[module] = layer;
      layer2module[layer] = module;  
    }
    	
    float moduleMeanTempMedianFiltered[39];
    
    moduleMeanTempMedianFiltered[0] = 0.;
    	
    for (unsigned iLayer=2; iLayer<=37; iLayer++){
      float temperatureForMedian[3];
      temperatureForMedian[0] = _moduleMeanTemp[layer2module[iLayer-1]-1];
      temperatureForMedian[1] = _moduleMeanTemp[layer2module[iLayer]-1];
      temperatureForMedian[2] = _moduleMeanTemp[layer2module[iLayer+1]-1];
      float median = this->median(&temperatureForMedian[0], 3);
      moduleMeanTempMedianFiltered[layer2module[iLayer]] = median;
    }
    moduleMeanTempMedianFiltered[layer2module[1]] = _moduleMeanTemp[layer2module[1]-1];
    moduleMeanTempMedianFiltered[layer2module[38]] = _moduleMeanTemp[layer2module[38]-1];
    
    float newCaloMeanTemp = 0.;
    
    for( unsigned module=1; module<=38; module++ ) {
      for( unsigned s=0; s<N_SENSORS; s++ ) {
        sensorTemp[module-1][s]      = moduleMeanTempMedianFiltered[module];
	sensorTempError[module-1][s] = _moduleMeanTempRMS[module-1];
      }
    _moduleMeanTemp[module-1] = moduleMeanTempMedianFiltered[module];
    newCaloMeanTemp += moduleMeanTempMedianFiltered[module]; 
    }

    _caloMeanTemp = newCaloMeanTemp/38.;
    
    newCalibCol = false;
    newSroModCol = false;
    newSanityRange = false;
  }
  

  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  double AhcMedianFilterTempProvider::median(float a[], int n)
  {  
    float temp;
    
    for(int i = 0; i < n; ++i)
      {
	for( int j = i + 1; j < n; ++j)
	  {
	    if(a[i] > a[j])
	      {	    
		temp = a[j];
		a[j] = a[i];
		a[i] = temp;	    
	      }	
	  }/*end loop over j*/
      }/*end loop over i*/
    
    if(n % 2 == 0)
      {
	return (a[n/2]+a[n/2-1])/2;
      }
    else
      {
	return a[n/2];
      }
  }
  
}
