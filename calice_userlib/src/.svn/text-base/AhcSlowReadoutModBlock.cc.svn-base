#include "AhcSlowReadoutModBlock.hh"
#include "to_binary_bitops.hh"

namespace CALICE{
   

   void AhcSlowReadoutModBlock::setTypes() {
    //define the doubles
    _theDblTypes.clear();
    _theDblTypes["CMBTEMPERATURES"] = std::make_pair<int, std::vector<double>* > (kIntSroModPosCmbTemps, &_cmbTemperatures);
    _theDblTypes["CMBVOLTAGES"] = std::make_pair<int, std::vector<double>* > (kIntSroModPosCmbVolts, &_cmbVoltages);
    _theDblTypes["CMBVALUES"]  = std::make_pair<int, std::vector<double>* > (kIntAhcSroModPosCmbVals, &_cmbValues);
    _theDblTypes["HBABTEMPERATURES"] = std::make_pair<int, std::vector<double>* > (kIntAhcSroModPosHbabTemps, &_hbabTemperatures);
    _theDblTypes["HBABVOLTAGES"] = std::make_pair<int, std::vector<double>* > (kIntSroModPosHbabVolts, &_hbabVoltages);

    _startOfDoubles=kIntAhcSroModDblValues;

    //define the ints 
    _theIntTypes.clear();
     _startOfInts=kIntAhcSroModIntValues;

    //define the floats
    _theIntTypes.clear();
     _startOfFloats=kIntAhcSroModIntValues;

  }
 



 /** Convenient print method 
   */
  void AhcSlowReadoutModBlock::print(std::ostream& os) {
    //prepare the results
    prepareOutputVecs();    

    os << "Ahc SlowReadout Module Data" << std::endl;
    os << "Module Number: " << getModuleNumber() << std::endl;
    os << "Time Stamp: " << getTimeStamp().getDateString() << std::endl;
    os << "Led Setting: " << std::hex << "hex: " << getLedSetting() << " " << std::dec; 
    to_binary_bitops(getLedSetting());
    os << "CMB Width: " <<  getCmbWidth() << std::endl;
    os << "CMB Height: " << getCmbHeight() << std::endl;
    os << "*** CMB Temperatures: ***" << std::endl;
  
    //Print the cmb temperatures
    int icmbtemps(0);
    std::vector<double> cmbTemperatures(getCmbTemperatures());
    for(std::vector<double>::iterator vec_iter = cmbTemperatures.begin(); vec_iter !=
	  cmbTemperatures.end(); vec_iter++) {
      if(icmbtemps < 5 ) os << "CMB Temperature [" << icmbtemps+1 << "]: " << (*vec_iter) << " C" << std::endl;
      if(icmbtemps == 5) os << "CMB Temperature lower: " << (*vec_iter) << std::endl;
      if(icmbtemps == 6) os << "CMB Temperature upper: " << (*vec_iter) << std::endl;
      if(icmbtemps > 6)  os << "CMB Temperature UNKNOWN TYPE [" << icmbtemps+1 << "]: " << (*vec_iter) << " C" << std::endl;
      icmbtemps++;
    }


    os << "*** CMB Voltages: ****" << std::endl;
    //Print the cmb voltages
    int icmbvolts(0);
    std::vector<double> cmbVoltages(getCmbVoltages());
    for(std::vector<double>::iterator vec_iter = cmbVoltages.begin(); vec_iter !=
	  cmbVoltages.end(); vec_iter++) {
      if(icmbvolts==0) os << "CMB calib U041: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==1) os << "CMB 12V power: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==2) os << "CMB 1.235V: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==3) os << "CMB VLDA upper: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==4) os << "CMB VLDB upper: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==5) os << "CMB VLDC upper: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==6) os << "CMB VLDD lower: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==7) os << "CMB 10 V bias: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts==8) os << "CMB calib U051 voltage: " << (*vec_iter) << " V" << std::endl;
      if(icmbvolts > 8)  os << "CMB Voltage UNKNOWN TYPE [" << icmbvolts+1 << "]: " << (*vec_iter) << " C" << std::endl;
      icmbvolts++;
    }


    os << "*** CMB Values: ***" << std::endl;
    //Print the cmb values
    int icmbvals(0);
    std::vector<double> cmbValues(getCmbValues());
    for(std::vector<double>::iterator vec_iter = cmbValues.begin(); vec_iter !=
	  cmbValues.end(); vec_iter++) {
      if(icmbvals==0) os << "CMB 12V external 1: " << (*vec_iter) << " V" << std::endl;
      if(icmbvals==1) os << "CMB 12V external 2: " << (*vec_iter) << " V" << std::endl;
      if(icmbvals>1)  os << "CMB Values UNKNOWN TYPE [" << icmbvals+1 << "]: " << (*vec_iter) << " V" << std::endl;
      icmbvals++;
    }

    os << "*** HBAB Temperatures: ***" << std::endl;
    //Print the hbab temperatures
    int ihbabtemps(0);
    std::vector<double> hbabTemperatures(getHbabTemperatures());
    for(std::vector<double>::iterator vec_iter = hbabTemperatures.begin(); vec_iter !=
	  hbabTemperatures.end(); vec_iter++) {
      if(ihbabtemps==0) os << "HBAB Temperature top 1: " << (*vec_iter) << " C" << std::endl;
      if(ihbabtemps==1) os << "HBAB Temperature top 2: " << (*vec_iter) << " C" << std::endl;
      if(ihbabtemps==2) os << "HBAB Temperature bot 1: " << (*vec_iter) << " C" << std::endl;
      if(ihbabtemps==3) os << "HBAB Temperature bot 2: " << (*vec_iter) << " C" << std::endl;
      if(ihbabtemps>3)  os << "HBAB Temperature bot UNKNOWN TYPE [" << ihbabtemps+1 << "]: " << (*vec_iter) << " X" << std::endl;
      ihbabtemps++;
    }


    os << "*** HBAB Voltages: ***" << std::endl;
    //Print the hbab voltages/currents values
    int ihbabvolts(0);
    std::vector<double> hbabVoltages(getHbabVoltages());
    for(std::vector<double>::iterator vec_iter = _hbabVoltages.begin(); vec_iter !=
	  _hbabVoltages.end(); vec_iter++) {
      if(ihbabvolts==0) os << "HBAB HV top voltage: " << (*vec_iter) << " V" << std::endl;
      if(ihbabvolts==1) os << "HBAB HV bot voltage: " << (*vec_iter) << " V" << std::endl;
      if(ihbabvolts==2) os << "HBAB HV top current: " << (*vec_iter) << " A" << std::endl;
      if(ihbabvolts==3) os << "HBAB HV bot current: " << (*vec_iter) << " A" << std::endl;
      if(ihbabvolts==4) os << "HBAB LV top voltage: " << (*vec_iter) << " V" << std::endl;
      if(ihbabvolts==5) os << "HBAB LV bot voltage: " << (*vec_iter) << " V" << std::endl;
      if(ihbabvolts==6) os << "HBAB LV top current: " << (*vec_iter) << " A" << std::endl;
      if(ihbabvolts==7) os << "HBAB LV bot current: " << (*vec_iter) << " A" << std::endl;
      if(ihbabvolts==8) os << "HBAB LVn top voltage: " << (*vec_iter) << " V" << std::endl;
      if(ihbabvolts==9) os << "HBAB LVn bot voltage: " << (*vec_iter) << " V" << std::endl;
      if(ihbabvolts==10) os << "HBAB LVn top current: " << (*vec_iter) << " A" << std::endl;
      if(ihbabvolts==11) os << "HBAB LVn bot current: " << (*vec_iter) << " A" << std::endl;
      if(ihbabvolts>11)  os << "HBAB Voltage UNKNOWN TYPE [" << ihbabtemps+1 << "]: " << (*vec_iter) << " X" << std::endl;
      ihbabvolts++;
    }





  }


  
  
}
