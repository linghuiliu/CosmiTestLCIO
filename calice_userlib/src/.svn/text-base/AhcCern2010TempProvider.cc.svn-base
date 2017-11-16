#include "AhcCern2010TempProvider.hh"

#include "AhcSlowReadoutModBlock.hh"

//#include "TMath.h"
#include <cmath>


namespace CALICE 
{
  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  AhcCern2010TempProvider::AhcCern2010TempProvider( AhcMapper const *mapper)
    :_moduleAverageTemperature(AhcTempProvider::N_MODULES, 0),
     _moduleAverageTemperatureRMS(AhcTempProvider::N_MODULES, 0),
     _moduleAverageTemperatureCount(AhcTempProvider::N_MODULES, 0)
  {
    _mapper = mapper;
  }

  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  float AhcCern2010TempProvider::getCellTemp( int module, int chip, int chan )
  {
#ifdef AHCTEMP_DEBUG
    cout<<"\n AhcCern2010TempProvider::getCellTemp: newCalibCol="<<newCalibCol
             <<" newSroModCol="<<newSroModCol
             <<" newSanityRange="<<newSanityRange<<endl;
#endif

    if( AhcTempProvider::newCalibCol || AhcTempProvider::newSroModCol || AhcTempProvider::newSanityRange ) 
      {
        this->applyCorrection();
     }
 
    
    return sensorTemp.at(module-1).at(this->getSensor(chip,chan));
  }

  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  float AhcCern2010TempProvider::getCellTempError( int module, int chip, int chan )
  {
    if( AhcTempProvider::newCalibCol || AhcTempProvider::newSroModCol || AhcTempProvider::newSanityRange ) 
      {
        this->applyCorrection();
      }
    
    return sensorTempError.at(module-1).at(this->getSensor(chip,chan));
  }

 /**********************************************************************************/
  /*                                                                                */
  /* Returns the closest sensor to cell connected to chip, chan according to        */
  /* DESY-THESIS-2008-050                                                           */       
  /*                                                                                */
  /* For the connection between the tile, chip and temperature sensors numbers, see */
  /* http://www.desy.de/~richters/IJ-to-chip-channel/                               */
  /*                                                                                */
  /* In case you wonder why there is chan the argument of the function, even though */
  /* it is not used, it is there just for possible future usage (it would make sense*/
  /* to use the channel number also to find the sensor)                             */
  /*                                                                                */
  /**********************************************************************************/
  int AhcCern2010TempProvider::getSensor( int chip, int chan )
  {
    if( chip == 0 ) return 0;
    else if( (chip > 0) && (chip < 4) ) return 1;
    else if( (chip > 3) && (chip < 8) ) return 2;
    else if( (chip > 7) && (chip < 11) ) return 3;
    else if( chip == 11 ) return 4;
    
    throw std::out_of_range( "AhcCern2010TempProvider::getSensor(): "
                             "argument chip out of range (0...11)");

    return 0; /*should never happen */
  }


  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  float AhcCern2010TempProvider::getAvgTemp()
  {
#ifdef AHCTEMP_DEBUG
    cout<<"\n AhcCern2010TempProvider::getAvgTemp"<<endl;
#endif
    if( newCalibCol || newSroModCol || newSanityRange ) 
      {
        this->applyCorrection();
      }
     
    return _calorimeterAverageTemperature;
  }

  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  float AhcCern2010TempProvider::getAvgModuleTemp( unsigned module )
  {
#ifdef AHCTEMP_DEBUG
    cout<<"\n AhcCern2010TempProvider::getAvgModuleTemp"<<endl;
#endif
    if( newCalibCol || newSroModCol || newSanityRange ) 
      {
        this->applyCorrection();
      }
    
    return _moduleAverageTemperature[module-1];   
  }



  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  void AhcCern2010TempProvider::applyCorrection(  )
  {
    if(!AhcTempProvider::_sroModCol) throw std::invalid_argument( "AhcCern2010TempProvider::applyCorrection(): "
                                                 " null pointer _sroModCol!" );
    if( newCalibCol || newSroModCol ) 
      {
	AhcTempProvider::updateCache();
      }

#ifdef AHCTEMP_DEBUG
   cout<<"\n AhcCern2010TempProvider::applyCorrection"<<endl;
#endif

    for (unsigned int module = 1; module < _mapper->getMaxK(); ++module)
      {
  
	for(unsigned int sensor = 0; sensor < N_SENSORS; ++sensor)
        {
	  this->applyCorrectionForUnreasonableSensors(module, sensor);
	}/*------------ end loop over sensor -----------------------------------*/
      }/*-------------- end loop over i ----------------------------------------*/
  

    _calorimeterAverageTemperature      = 0;
    _calorimeterAverageTemperatureRMS   = 0;
    _calorimeterAverageTemperatureCount = 0;

    for (unsigned int module = 1; module < _mapper->getMaxK(); ++module)
      {
	_moduleAverageTemperature[module - 1]      = 0;
	_moduleAverageTemperatureRMS[module - 1]   = 0;
	_moduleAverageTemperatureCount[module - 1] = 0;

	for(unsigned int sensor = 0; sensor < N_SENSORS; ++sensor)
        {
	  this->applyCorrectionForUnreasonableModules(module, sensor);

	  _moduleAverageTemperature[module - 1]    += sensorTemp[module - 1][sensor];
	  _moduleAverageTemperatureRMS[module - 1] += sensorTemp[module - 1][sensor] * sensorTemp[module - 1][sensor];
	  _moduleAverageTemperatureCount[module - 1]++;

	  _calorimeterAverageTemperature += sensorTemp[module - 1][sensor];
	  _calorimeterAverageTemperatureRMS += sensorTemp[module - 1][sensor] * sensorTemp[module - 1][sensor];
	  _calorimeterAverageTemperatureCount++;

	}/*------------ end loop over sensor -----------------------------------*/

	if (_moduleAverageTemperatureCount[module - 1] > 0)
	  {
	    _moduleAverageTemperature[module - 1]    /= _moduleAverageTemperatureCount[module - 1];
	    _moduleAverageTemperatureRMS[module - 1] /= _moduleAverageTemperatureCount[module - 1];
	    _moduleAverageTemperatureRMS[module - 1] -=  _moduleAverageTemperature[module - 1] * _moduleAverageTemperature[module - 1];
	    _moduleAverageTemperatureRMS[module - 1] = sqrt(_moduleAverageTemperatureRMS[module - 1]);

	  }
      }/*-------------- end loop over i ----------------------------------------*/
  
    if (_calorimeterAverageTemperatureCount > 0)
      {
	_calorimeterAverageTemperature    /= _calorimeterAverageTemperatureCount;
	_calorimeterAverageTemperatureRMS /= _calorimeterAverageTemperatureCount;
      }


    newCalibCol    = false;
    newSroModCol   = false;
    newSanityRange = false;

    /*
    for (int i = 1; i <= 38; ++i)
      {
	cout<<"lntai i="<<i<<" _moduleAverageTemperature="<<_moduleAverageTemperature[i - 1]
	    <<" _moduleAverageTemperatureRMS="<<_moduleAverageTemperatureRMS[i - 1]
	    <<endl;
	cout<<"            _moduleAverageTemperatureCount: "<<_moduleAverageTemperatureCount[i - 1] <<endl;
	for (int j = 0; j < 5; ++j)
	  cout<<"            sensor="<<j+1<<" temp="<<sensorTemp[i - 1][j]<<" status="<<sensorCalibStatus.at(i-1).at(j) <<endl;
	
      }
    */

  }


  /**********************************************************************************/
  /*                                                                                */
  /*  If the specified sensor has an unreasonable offset flag, set the temperature  */
  /*  the median of all sensors in that module                                      */
  /*                                                                                */
  /**********************************************************************************/
  void AhcCern2010TempProvider::applyCorrectionForUnreasonableSensors(int module, int sensor)
  {
    if (this->isModuleOff(module) == false)
      {
#ifdef AHCTEMP_DEBUG
	cout<<" \n\nsize of sensorTemp["<<module<<"]="<<sensorTemp[module-1].size()<<endl;
#endif
	
	std::vector<float> temperatureForMedian;
	temperatureForMedian.clear();
	
	for (unsigned int i = 0; i < sensorTemp[module-1].size(); ++i)
	  {
#ifdef AHCTEMP_DEBUG
	    cout<<"sensor: "<<i<<" module: "<<module<<" sensorStatus: "<<sensorCalibStatus.at(module-1).at(i)
		<<" temperature="<<sensorTemp[module-1][i]<<endl;
#endif
	    /*calculate the median only from 'good' sensors, i.e. flag > 0*/
	    if (sensorCalibStatus.at(module-1).at(i) > 0)
	      {
		temperatureForMedian.push_back(sensorTemp[module-1][i]);
	      }
	  }
	
#ifdef AHCTEMP_DEBUG
	cout<<"size of temperatureForMedian: "<<temperatureForMedian.size()<<endl;
	for (unsigned int j = 0; j < temperatureForMedian.size(); ++j)
	  cout<<" temperatureForMedian: "<<temperatureForMedian[j]<<endl;
#endif
	
	float median = this->median(&temperatureForMedian[0], temperatureForMedian.size());
	/*
	  if (median == 0) cout<<"AhcSimpleTempProvider::applyCorrectionForUnreasonableTemperature: something might be wrong, "
	  <<" median=0, all sensors for module "<<module<<" have bad status...."<<endl;
	*/
	
	if (!sensorCalibStatus.at(module-1).at(sensor))
	  {
	    sensorTemp[module-1][sensor] = median;
	  }    
	
      }   
    
  }
  /**********************************************************************************/
  /*                                                                                */
  /*  If a module has ALL temperature sensors off, set for each sensor the          */
  /*  temperature to the average of neighbouring sensors (neighbouring in layers)   */
  /*                                                                                */
  /**********************************************************************************/
  bool AhcCern2010TempProvider::applyCorrectionForUnreasonableModules(int module, int sensor)
  {
    if (this->isModuleOff(module) == true)
      {
	int currentLayer = _mapper->getK(module);
	const int maxK = _mapper->getMaxK();
	
	/*assume we have at least 4 layers in the calorimeter*/
	unsigned int layerNeighbour1 = 0;
	unsigned int layerNeighbour2 = 0;

	/*---------- case 1: first layer -------------------------------------*/
	if (currentLayer == 1)
	  {
	    //cout<<" -->case1"<<endl;

	    layerNeighbour1 = _mapper->getK(module + 1);
	    layerNeighbour2 = _mapper->getK(module + 2);
	    
	    /*if first neighbour is OFF*/
	    if (this->isModuleOff(module + 1)    == true
		&& this->isModuleOff(module + 2) == false)
	      {
		if (this->isModuleOff(module + 3) == false)
		  {
		    layerNeighbour1 = layerNeighbour2;
		    layerNeighbour2 = _mapper->getK(module + 3);
		  }
		else
		  {
		    cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
			<<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;

		    sensorTemp[module-1][sensor] = 0;
		    return false;
		    
		  }
	      }
	    /*if last neighbour is OFF*/
	    if (this->isModuleOff(module + 1)    == false
		&& this->isModuleOff(module + 2) == true)
	      {
		if (this->isModuleOff(module + 3) == false)
		  {
		    layerNeighbour2 = _mapper->getK(module + 3);
		  }
		else
		  {
		    cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
			<<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;

		    sensorTemp[module-1][sensor] = 0;
		    return false;
		    
		  }
	      }

	    /*if both neighbours are OFF*/
	    if (this->isModuleOff(module + 1)    == true
		&& this->isModuleOff(module + 2) == true)
	      {
		cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
		    <<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;

		sensorTemp[module-1][sensor] = 0;
		return false;
	      }
	  }/*end if (current == 1)*/

	/*------------ case 2: last layer ---------------------------------*/
	else if (currentLayer == maxK - 1)
	  {
	    //cout<<" -->case2"<<endl;

	    layerNeighbour1 = _mapper->getK(module - 1);
	    layerNeighbour2 = _mapper->getK(module - 2);
	    
	    /*if first neighbour is OFF*/
	    if (this->isModuleOff(module - 1)    == true
		&& this->isModuleOff(module - 2) == false)
	      {
		if (this->isModuleOff(module - 3) == false)
		  {
		    layerNeighbour1 = layerNeighbour2;
		    layerNeighbour2 = _mapper->getK(module - 3);
		  }
		else
		  {
		    cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
			<<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;
		    sensorTemp[module-1][sensor] = 0;
		    return false;
		    
		  }
	      }
	    /*if last neighbour is OFF*/
	    if (this->isModuleOff(module - 1)    == false
		&& this->isModuleOff(module - 2) == true)
	      {
		if (this->isModuleOff(module - 3) == false)
		  {
		    layerNeighbour2 = _mapper->getK(module - 3);
		  }
		else
		  {
		    cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
			<<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;
		    sensorTemp[module-1][sensor] = 0;
		    return false;
		    
		  }
	      }

	    /*if both neighbours are OFF*/
	    if (this->isModuleOff(module - 1)    == true
		&& this->isModuleOff(module - 2) == true)
	      {
		cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
		    <<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;
		sensorTemp[module-1][sensor] = 0;
		return false;
	      }
	  }
	    
	/*------- last case: the other layers -------------------------------*/
	else 
	  {
	    //cout<<" -->case other"<<endl;

	    
	    layerNeighbour1 = _mapper->getK(module - 1);
	    layerNeighbour2 = _mapper->getK(module + 1);
	    
	    /*if the lower neighbour is OFF*/
	    if (this->isModuleOff(module - 1)    == true
		&& this->isModuleOff(module + 1) == false)
	      {
		if ((module - 2) < maxK  && this->isModuleOff(module - 2) == false)
		  layerNeighbour1 = _mapper->getK(module - 2);
		else
		  {
		    cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
			<<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;
		    sensorTemp[module-1][sensor] = 0;
		    return false;
		    
		  }
	      }
	    
	    /*if the upper neighbour is OFF*/
	    else if (this->isModuleOff(module - 1)    == false
		     && this->isModuleOff(module + 1) == true)
	      {
		if ((module + 2) < maxK  && this->isModuleOff(module + 2) == false)
		  layerNeighbour2 = _mapper->getK(module + 2);
		else
		  {
		    cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
			<<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;
		    sensorTemp[module-1][sensor] = 0;
		    return false;
		    
		  }
	      }
	    /*if both neighbours are OFF*/
	    else if (this->isModuleOff(module - 1)    == true
		     && this->isModuleOff(module + 1) == true)
	      {
		cout<<"\n===============> AhcTempProvider::applyCorrectionForUnreasonableModules: "
		    <<" more than 2 consecutive AHCAL modules are OFF in temperatures, don't know how to correct this"<<endl;
		sensorTemp[module-1][sensor] = 0;
		return false;
	      }
	    
	  }/*end of last case*/

	float tempNeighbour1 = sensorTemp[_mapper->getModule(layerNeighbour1) - 1][sensor];
	float tempNeighbour2 = sensorTemp[_mapper->getModule(layerNeighbour2) - 1][sensor];

	float extrapolatedTemperature = 0;

	if (currentLayer == 1 || currentLayer == maxK - 1)
	    extrapolatedTemperature = tempNeighbour1 - (tempNeighbour2 - tempNeighbour1);
	else
	  extrapolatedTemperature = (tempNeighbour1 + tempNeighbour2)/2.;
	
	if (extrapolatedTemperature > 0) 
	  sensorTemp[module-1][sensor] = extrapolatedTemperature;

      }/*end if isModuleOff*/

    return true;
  }

  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  bool AhcCern2010TempProvider::isModuleOff( int module )
  {
    unsigned int countOffSensorsPerModule = 0;
    for(unsigned iSensor = 0; iSensor < N_SENSORS; ++iSensor)
      {
	if ( sensorCalibStatus.at(module-1).at(iSensor) ==0 )
	  countOffSensorsPerModule++;
      }
    
    if (countOffSensorsPerModule == N_SENSORS) return true;
    else return false;
  }


  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  float AhcCern2010TempProvider::getSensorTemp( int module, int sensor )
  {
    if( newCalibCol || newSroModCol || newSanityRange ) 
      {
        this->applyCorrection();
      }

    if (sensor > 4) throw std::invalid_argument( "AhcSimpleTempProvider::"
                                                 "AhcSimpleTempProvider::getSensorTemp: "
                                                 " sensor out of range ( > 4)" );
      return sensorTemp.at(module-1).at(sensor);
  }

  
  /**********************************************************************************/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /**********************************************************************************/
  double AhcCern2010TempProvider::median(float a[], int n)
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
