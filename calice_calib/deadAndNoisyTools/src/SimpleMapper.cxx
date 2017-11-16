#include "SimpleMapper.h"
#include "ModuleType.h"

#include <iostream>
#include <fstream>
#include <string>

#define AHCAL_MAX_LAYERS 38

SimpleMapper::SimpleMapper( std::string configFileName)
{
  
  // the input stream for the text file
  std::fstream configFileInputStream;
  configFileInputStream.open(configFileName.c_str());

  // the input buffer for the line we just read
  std::string inputLine;

  // parse the text file line by line
  while ( std::getline( configFileInputStream, inputLine ) )
  {
    // std::cout << "DEBUG: Read " << inputLine << std::endl;
    
    // skip comment lines
    if ( inputLine.find("#") != std::string::npos ) 
    {
      // If there is no # the return value is npos.
      // So if it is not npos there is a # -> skip this line
      // std::cout << "skipped comment line" << std::endl;
      continue;
    }
    
    // the input variables to be read from the string
    int slotConf     = -1;
    int feConf       = -1;
    int moduleConf   = -1;
    int stackPos     = -1;
    int cmbId        = -1;
    int cmbCanAdr    = -1;
    int cmbPinId     = -1;
    int Hold_ext     = -1;
    int Hold_CM_LED  = -1;
    int Hold_PM_LED  = -1;
    int Vcalib_CM    = -1;
    int Vcalib_PM    = -1;
    int tcmt         = -1;

    // use sscanf to scan for the pattern. It is the line containing AHCAL.
    int nParametersRead = sscanf( inputLine.c_str(), " %d %d AHCAL8 %d %d %d %d %d %d %d %d %d %d",
				  &slotConf, &feConf, &moduleConf, &stackPos, 
				  &cmbId, &cmbCanAdr, &cmbPinId, &Hold_ext, &Hold_CM_LED, 
				  &Hold_PM_LED, &Vcalib_CM, &Vcalib_PM);

    unsigned int  moduleType(0);
    bool validConfig = false;

    if ( nParametersRead != 12 )
    {
      nParametersRead = sscanf( inputLine.c_str(), " %d %d AHCAL %d %d %d %d %d %d %d %d %d %d",
				&slotConf, &feConf, &moduleConf, &stackPos, 
				&cmbId, &cmbCanAdr, &cmbPinId, &Hold_ext, &Hold_CM_LED, 
				&Hold_PM_LED, &Vcalib_CM, &Vcalib_PM);

      if ( nParametersRead != 12 )
      {
	
	nParametersRead = sscanf( inputLine.c_str(), " %d %d TCMT  DAC_tcmt%d.dat %d %d %d %d %d",
				  &slotConf, &feConf, &tcmt, &Hold_ext, &Hold_CM_LED, 
				  &Hold_PM_LED, &Vcalib_CM, &Vcalib_PM);

	if ( nParametersRead == 8 )
	{
	  moduleType = ModuleType::TCMT;
	  validConfig=true;
	  moduleConf=AHCAL_MAX_LAYERS + tcmt;
	  stackPos=AHCAL_MAX_LAYERS + tcmt;
	}
	else
	{
	  //std::cout << "Unknown line "  << inputLine << std::endl;
	}
      }
      else //scanf ahc == 12
      {
	moduleType = ModuleType::AHC;
	validConfig=true;
      }
    }
    else  //scanf ahc== 12
    {
      moduleType = ModuleType::AHC8;
      validConfig=true;
    }
 
   // if the number of parameters is 12 as expected fill the map
    if ( validConfig )
    {
      // std::cout << " there we are:" << slotConf << "\t" << feConf << "\t" 
      // << moduleConf << "\t" << stackPos << std::endl;
      _sf2mlMap[ SlotFrontendID( slotConf, feConf ) ] 
	= ModuleDescription ( moduleConf, stackPos, moduleType); 
    }

  }

  configFileInputStream.close();
  
}

void SimpleMapper::print()
{
  std::cout << "# slot frontEnd module layer type" << std::endl;
  for ( std::map< SlotFrontendID, ModuleDescription >::iterator it = _sf2mlMap.begin();
	it != _sf2mlMap.end(); it++ )
  {
    SlotFrontendID sfID = it->first;
    ModuleDescription moduleDesc = it->second;
    
    std::cout << sfID.first  <<  "\t" << sfID.second  << "\t" 
	      << moduleDesc._module <<  "\t" << moduleDesc._layer <<  "\t";
    switch ( moduleDesc._moduleType )
    {
      case ModuleType::AHC: std::cout << "AHC";
	break;
      case  ModuleType::AHC8: std::cout << "AHC8";
	break;
      case  ModuleType::TCMT: std:: cout << "TCMT";
	break;
    }
    std::cout << std::endl;
  }
}

SimpleMapper::ModuleLayerID SimpleMapper::getModuleLayerID( SimpleMapper::SlotFrontendID sfID )
{
  return ModuleLayerID( _sf2mlMap[sfID]._module, _sf2mlMap[sfID]._layer);
}


bool SimpleMapper::isAHC8( SimpleMapper::SlotFrontendID sfID )
{
  return ( _sf2mlMap[sfID]._moduleType ==  ModuleType::AHC8 );
}

bool SimpleMapper::isTCMT( SimpleMapper::SlotFrontendID sfID )
{
  return ( _sf2mlMap[sfID]._moduleType ==  ModuleType::TCMT );
}

SimpleMapper::ModuleDescription::ModuleDescription( int module, int layer,
						    unsigned int moduleType ) 
  : _module( module ), _layer( layer), _moduleType( moduleType ) 
{}

unsigned int SimpleMapper::getModuleType( SimpleMapper::SlotFrontendID sfID )
{
  return _sf2mlMap[sfID]._moduleType;
}
