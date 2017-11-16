#include "SceSlowReadoutModBlock.hh"
#include "to_binary_bitops.hh"

namespace CALICE{
   

   void SceSlowReadoutModBlock::setTypes() {
    //define the doubles
    _theDblTypes.clear();
    _theDblTypes["CMBTEMPERATURES"] = std::make_pair<int, std::vector<double>* > (kIntSroModPosCmbTemps, &_cmbTemperatures);
    _theDblTypes["CMBVOLTAGES"] = std::make_pair<int, std::vector<double>* > (kIntSroModPosCmbVolts, &_cmbVoltages);
    _theDblTypes["HBABVOLTAGES"] = std::make_pair<int, std::vector<double>* > (kIntSroModPosHbabVolts, &_hbabVoltages);
    _theDblTypes["HBABCURRENTS"] = std::make_pair<int, std::vector<double>* > (kIntSceSroModPosHbabCurs, &_hbabCurrents);

    _startOfDoubles=kIntSceSroModDblValues;

    //define the ints 
    _theIntTypes.clear();
     _startOfInts=kIntSceSroModIntValues;

    //define the floats
    _theIntTypes.clear();
     _startOfFloats=kIntSceSroModIntValues;

  }
 



 /** Convenient print method 
   */
  void SceSlowReadoutModBlock::print(std::ostream& os) {
    //prepare the results
    prepareOutputVecs();    

    os << "Sce SlowReadout Module Data" << std::endl;
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
      if(icmbtemps == 0) os << "CMB Temperature lower: " << (*vec_iter) << " C" << std::endl;
      if(icmbtemps == 1) os << "CMB Temperature upper: " << (*vec_iter) << " C" << std::endl;
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


    os << "*** HBAB Voltages and Currents: ***" << std::endl;
    //Print the hbab voltages/currents values
    //int ihbabvolts(0);
    std::vector<double> hbabVoltages(getHbabVoltages());
    std::vector<double> hbabCurrents(getHbabCurrents());
    if(hbabVoltages.size() == hbabCurrents.size()) {
    std::vector<double>::iterator veccur_iter=hbabCurrents.begin();
      for(std::vector<double>::iterator vecvol_iter = hbabVoltages.begin(); vecvol_iter != hbabVoltages.end(); vecvol_iter++, veccur_iter++) {
	os << "HBAB SCE HV voltage: " << (*vecvol_iter) << " V" << std::endl;
   	os << "HBAB SCE HV voltage: " << (*veccur_iter) << " A" << std::endl;    
      }
    } else {
      os << "WARNING - Number of measured HBAB Currents does not correspond to number of measured HBAB Voltages!!!" << std::endl;
      os << "Number of measured HBAB Currents is: " << hbabCurrents.size() << std::endl;
      os << "Number of measured HBAB Voltages is: " << hbabVoltages.size() << std::endl;
    }
  }
}
