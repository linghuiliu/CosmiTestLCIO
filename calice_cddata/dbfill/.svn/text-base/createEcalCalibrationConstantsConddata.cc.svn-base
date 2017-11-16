#include "RootTools/TDirIter.hh"
#include "RootTools/find.hh"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>

#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCParameters.h>
#include <EVENT/LCIO.h>
#include <UTIL/LCTime.h>

#include <EcalModuleCalibration.hh>

#include <vector>

#include <Average_t.hh>

//Quick and dirty hack to set calibration const. for new LAyers (July 2007)
void setDummyValues(EVENT::LCCollection*, Float_t);


EVENT::LCCollection *createEcalCalibrationConstantsConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::vector<std::string> &fit_result_file_names, 
							    Float_t nominal_mpv,  Float_t min_calibration_constant, Float_t max_calibration_constant,
							    bool verbose)
{

  LCCollectionVec* calibration_constants_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;
  Average_t av_calibration_tot;

  unsigned int module_count(0); 
  for(std::vector<std::string>::const_iterator fileiter= fit_result_file_names.begin(); fileiter != fit_result_file_names.end(); fileiter++) {
    //TFile fit_result_file(fit_result_file_name.c_str());
    std::cout << "Open File: " << (*fileiter) << std::endl; 
    TFile fit_result_file((*fileiter).c_str());
    if (!fit_result_file.IsOpen()) {
      std::stringstream message;
      message << "File with fit results \"" << (*fileiter) << "\" does not exist or is not a ROOT file.";
      throw std::runtime_error(message.str());
    }
    //  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;
      
      
      
      
    TDirIter pcb_iter(&fit_result_file,TRegexp("^PCB"),TDirectory::Class_Name());
      
    // --- distinguish module types
    UInt_t n_module_types=5;
    const char *module_type_names[]={
      "C", "G", "D", "L", "R"
    };
    const UInt_t module_type_ids[]={
      0,    1,   2,   1,   2
    };  
    // the number of cells is used as a sanity check.
    const UInt_t n_expected_cells[]={
      216,108,108,108,108
    };
      
    for(TKey *a_key; (a_key=pcb_iter.Next()); ) {
	
      UInt_t module_type=UINT_MAX;
      UInt_t module_id=UINT_MAX;
      std::string module_type_name;
	
      // --- get module id and module type from directory name
      const char *ptr=a_key->GetName();
      ptr+=3;
      const char *id_start=ptr;
      while (isdigit(*ptr)) ptr++;
      if (id_start==ptr) {
	std::cerr << "Directory name in fit results file \"" << a_key->GetName() << "\" does not contain serial number. SKIPPING!" << std::endl;
	continue;
      }
      module_id=atoi(id_start);
      //      const char *id_end=ptr;
      if (*ptr=='_') {
	ptr++;
      }
      else {
	std::cerr << "Strange directory in fit results file \"" << a_key->GetName() << "\". SKIPPING!" << std::endl;
	continue;
      }
      UInt_t type_i=0;
      for (; type_i<n_module_types; type_i++) {
	if (strcmp(ptr,module_type_names[type_i])==0) {
	  module_type=module_type_ids[type_i];
	  module_type_name=module_type_names[type_i];
	  break;
	}
      }
      if (type_i>=n_module_types) {
	std::cerr << "Unrecognised module type \"" << ptr << "\" for directory \"" << a_key->GetName() << "\". SKIPPING!" << std::endl;
	continue;
      }
	
      // --- enter directory of the PCB and loop over all cells
      std::stringstream regexp;
      TDirectory *pcb_dir=enter_dir(&fit_result_file,TRegexp(Form("^%s$",a_key->GetName())),1);
      if (!pcb_dir) {
	std::cerr << "Could not enter directory \"" << a_key->GetName() << "\". SKIPPING!" << std::endl;
	continue;
      }
	
      std::vector<float> calibration_constants;
      
      TDirIter a_cell_iter(pcb_dir,TRegexp(Form("fit_%s_",a_key->GetName())),TH1::Class_Name());
      for (TKey *hist_key; (hist_key=a_cell_iter.Next()); ) {
	const char *ptr=hist_key->GetName();
	// "fit_" + [PCB name]
	ptr+=4+strlen(a_key->GetName());
	if (*ptr=='_') ptr++;
	const char *cell_id_start=ptr;
	while (isdigit(*ptr)) ptr++;
	if (cell_id_start==ptr) {
	  std::cerr << "Histogram \"" << hist_key->GetName() << "\" does not contain cell number at expected place:\""
		    << cell_id_start
		    << "\". SKIPPING!" << std::endl;
	  continue;
	}
	if (*ptr!='\0') {
	  std::cerr << "Histogram name \"" << hist_key->GetName() << "\" contains unexpected characters after cell index. SKIPPING!" << std::endl;
	  continue;
	}
	
	TH1 *h1=get_obj<TH1>(hist_key);
	if (!h1) {
	  std::cerr << "Failed to read histogram \"" << hist_key->GetName() << "\". SKIPPING!" << std::endl;
	  continue;
	}
	// the fitted MPV is stored in the second bin of the histogram
	Float_t a_calibration_constant=h1->GetBinContent(2);
	
	delete h1;
	UInt_t cell_index=atoi(cell_id_start);
	if (cell_index==calibration_constants.size()) {
	  calibration_constants.push_back(a_calibration_constant);
	}
	if (cell_index>calibration_constants.size()) {
	  calibration_constants.resize(cell_index+1,-1.);
	}
	calibration_constants[cell_index]=a_calibration_constant;
	if (std::isnan(a_calibration_constant) || a_calibration_constant<=min_calibration_constant || a_calibration_constant>max_calibration_constant) {
	  std::cerr << "WARNING: " << hist_key->GetName() << ": invalide calibration constant (" << a_calibration_constant 
		    << ") for cell " << cell_index << " of pcb " << a_key->GetName() <<"." << std::endl;
	}
	
      }
      for(UInt_t cell_index=0; cell_index<n_expected_cells[type_i]; cell_index++) {
	// FIXME: -1. is not a rebust marker for missing cells.
	if (cell_index>=calibration_constants.size() || calibration_constants[cell_index]<0.) {
	  std::cerr << "WARNING: missing calibration constant for cell " << cell_index << " of pcb " << a_key->GetName() <<"." << std::endl;
	  if (cell_index>=calibration_constants.size()) {
	    std::cerr << "... and all following cells" << std::endl;
	    break;
	  }
	}
      }
      
      if (calibration_constants.size()!=n_expected_cells[type_i]) {
	std::cerr << "WARNING: The number of calibration constants (" << calibration_constants.size()
		  << ") retrieved from \"" << a_key->GetName() << "\" does not correspond to the number of cells ("  << n_expected_cells[type_i]
		  << ")." <<std::endl
		  << "The list is extended to " << n_expected_cells[type_i] << " cells. All cells with missing calibration constants are declared dead!" << std::endl;
	calibration_constants.resize(n_expected_cells[type_i],-1.);
      }
      
      std::cout << "Creating module: " << module_type_name << ", " << module_id << std::endl;
      CALICE::EcalModuleCalibration *a_module=new CALICE::EcalModuleCalibration(module_type_name,module_id,calibration_constants.size());
      
      Average_t av_calibration;
      UInt_t n_broken_cells=0;
      for(UInt_t cell_index=0; cell_index<calibration_constants.size(); cell_index++) {
	Float_t a_calibration_constant=calibration_constants[cell_index];
	if (std::isnan(a_calibration_constant) || a_calibration_constant<=min_calibration_constant || a_calibration_constant>max_calibration_constant) {
	  a_calibration_constant=0.;
	  n_broken_cells++;
	}
	else {
	  av_calibration.add(a_calibration_constant);
          av_calibration_tot.add(a_calibration_constant);
	  // the calibration constants are multiplicative;
	  a_calibration_constant=nominal_mpv/a_calibration_constant;
	}
	(*a_module)
	  .setCalibrationConstant(cell_index, a_calibration_constant);
      }
      av_calibration.calculate();
      if (verbose) {
       
	std::cout << "Stat for " << a_key->GetName() << ": type=" << a_module->getModuleTypeName() << "(" << module_type << ") ID=" << module_id 
		  << " n_floats=" << a_module->getNFloat() << " n_ints=" << a_module->getNInt() 
		  << " n_cells=" << a_module->getNCells()
		  << " broken cells=" << n_broken_cells
		  << " calibration:"
		  << av_calibration
		  << std::endl
		  << std::endl;
      }
      
      calibration_constants_col->addElement(a_module);
    }
      
  }
  std::cout << "size of calib col before: " << calibration_constants_col->getNumberOfElements() << std::endl;
  av_calibration_tot.calculate();
  Float_t all_average(av_calibration_tot.mean()); 
  //setDummyValues(calibration_constants_col, all_average);
  std::cout << "size of calib col after: " << calibration_constants_col->getNumberOfElements() << std::endl;

  return calibration_constants_col;
}

void setDummyValues(LCCollection* calibration_constants_col, Float_t theAverage) {

  

  for(UInt_t module_id=12; module_id < 18; module_id++) {

    CALICE::EcalModuleCalibration *a_module_d=new CALICE::EcalModuleCalibration("D",module_id, 108);
    CALICE::EcalModuleCalibration *a_module_g=new CALICE::EcalModuleCalibration("G",module_id, 108);
    for (UInt_t icell=0; icell < 108; icell++) {
      /*(*a_module_d)
	.setCalibrationConstant(icell, 1/45);
      (*a_module_g)
	.setCalibrationConstant(icell, 1/45);
      */
      /*(*a_module_d)
	.setCalibrationConstant(icell, (static_cast<Double_t>(1))/(static_cast<Double_t>(45)));
      (*a_module_g)
	.setCalibrationConstant(icell, static_cast<Double_t>(1/45));
      */
      (*a_module_d)
	.setCalibrationConstant(icell, (static_cast<Double_t>(1))/static_cast<Double_t>(theAverage));
      (*a_module_g)
	.setCalibrationConstant(icell, (static_cast<Double_t>(1))/static_cast<Double_t>(theAverage));
      


    }
    calibration_constants_col->addElement(a_module_d);
    calibration_constants_col->addElement(a_module_g);
  }
}
