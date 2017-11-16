#include "lcio.h"

//#include "IO/LCWriter.h"
#include <EVENT/LCIO.h>

#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCIntVec.h>
#include <EVENT/LCFloatVec.h>
#include <UTIL/LCTime.h>

#ifdef USE_CONDDB
#include <ConditionsDB/CondDBException.h>
#endif
#include <lccd.h>
#include <lccd/DBInterface.hh>
#include <lccd/ConditionsMap.hh>

#include <ExperimentalSetup.hh>
#include <RtypesSubSet.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <map>
#include <queue>
#include <vector>
#include <cmath>
#include <cstring>

#include <ReadDaqMap/ReadLine.hh>
#include <ReadDaqMap/TLineIterator.hh>
#include <ReadDaqMap/getRunStartTime.hh>
#include <ReadDaqMap/parseDateTime.hh>

using namespace lcio ;
using namespace CALICE;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

void print_help(const char *prg_name)
{
  std::cout << prg_name << " [--only-angle #] [--help]  [--detector-centre # #] \\" << std::endl
	    << "\t\t [--angle #] --beam-type name  \\" << std::endl
	    << "\t\t [--since date time] [--till date time] \\" << std::endl
	    << "\t [--top-folder name] [--write] [--write-file]" << std::endl
    //	    << "\t [--tag name] [--comment name]" << std::endl
	    << std::endl
	    << "--help             this text."<< std::endl
	    << "--verbose          Be more verbose."<< std::endl
    //    	    << "--tag [name]       set this tag for the conditions data." << std::endl
    //    	    << "--comment [test]   add this comment to the conditions data." << std::endl
            << "--since date time  since time of the setup parameters." << std::endl
            << "--till date time   till time of the setup parameters." << std::endl
            << std::endl
	    << "--beam-type [name] type of the beam: electrons, muons, pions, protons, cosmics, pion+electron. " << std::endl
	    << "--energy [float]   nominal beam energy." << std::endl
	    << "--detector-centre [float] [float] place the detector at this position (cm)." << std::endl
	    << "--angle [float]    measured angle (-45 - 0)." << std::endl
	    << std::endl
            << "--top-folder name  Name to be given to the top folder."
	    << "--write            without this option nothing is written but only printed to the screen." << std::endl
	    << "--write-file       write conditions data stored in the database folder into a LCIO file." << std::endl
	    << std::endl;
}


int main(int argc, char** argv )
{

  // ATTENTION: Internal length units are cm length. The Units used for LCObjects are mm
  //            Internal energy units are GeV. The energy units for LCObjects are MeV ?
  const Float_t length_unit=1e-2/1e-3; // comversion from cm to mm
  const Float_t energy_unit=1e9/1e6;   // comversion from GeV to MeV

  Float_t detector_centre_x0=0;
  Float_t detector_centre_y0=0;

  Float_t beam_pos_x=0;
  Float_t beam_pos_y=0;

  Float_t the_angle=0;

  Float_t beam_energy=0;

  LCTime   since( 0 ) ;
  LCTime   till(  0 ) ;


  //  Float_t detector_centre_x0=39.95;   // cm
  //  Float_t detector_centre_y0=19.51;    // cm

  bool show_help=false;
  bool update_conditions_db=false;
  bool write_flat_file=false;

  bool verbose=false;

  std::string top_folder;
  ExperimentalSetup::EBeamType the_beam_type=ExperimentalSetup::kUnknown;
  std::string tag_name;
  std::string comment;

  //  Int_t last_event[6]={2005,     2,   24,   20 , 38 , 0};
  UTIL::LCTime lastEvent(2005,     2,   24,   20 , 38 , 0);


 typedef std::map<std::string, ExperimentalSetup::EBeamType>  BeamTypeMap_t;
    std::map<std::string, ExperimentalSetup::EBeamType> beam_types;
    beam_types["electrons"]=ExperimentalSetup::kElectronBeam;
    beam_types["electron"]=ExperimentalSetup::kElectronBeam;
    beam_types["muons"]=ExperimentalSetup::kMuonBeam;
    beam_types["muon"]=ExperimentalSetup::kMuonBeam;
    beam_types["pions"]=ExperimentalSetup::kPionBeam;
    beam_types["pion"]=ExperimentalSetup::kPionBeam;
    beam_types["mixed"]=ExperimentalSetup::kMixed;
    beam_types["pions+electrons"]=ExperimentalSetup::kPionElectronBeam;
    beam_types["pion+electron"]=ExperimentalSetup::kPionElectronBeam;
    beam_types["electrons+pions"]=ExperimentalSetup::kPionElectronBeam;
    beam_types["electron+pion"]=ExperimentalSetup::kPionElectronBeam;
    beam_types["protons"]=ExperimentalSetup::kProtonBeam;
    beam_types["proton"]=ExperimentalSetup::kProtonBeam;
    beam_types["protons+pions"]=ExperimentalSetup::kPionProtonBeam;
    beam_types["proton+pion"]=ExperimentalSetup::kPionProtonBeam;
    beam_types["pion+protons"]=ExperimentalSetup::kPionProtonBeam;
    beam_types["pion+proton"]=ExperimentalSetup::kPionProtonBeam;
    beam_types["cosmics"]=ExperimentalSetup::kCosmics;
    beam_types["calibration"]=ExperimentalSetup::kCalibrationPulse;
    beam_types["noise"]=ExperimentalSetup::kNoise;

  // --------------------------------------------------------------------------------
  // parse arguments 
  {
   
    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      for (UInt_t arg_i=1; arg_i<n_args; arg_i++) {
	if (strcmp(argv[arg_i],"--angle")==0) {
	  if (arg_i+1>=n_args) {
	    throw runtime_error("expected float argument for --angle");
	  }
	  char *ptr;
	  the_angle=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("expected float argument for --angle");
	  }
	}
	else if (strcmp(argv[arg_i],"--energy")==0) {
	  if (arg_i+1>=n_args) {
	    throw runtime_error("expected float argument for --energy");
	  }
	  char *ptr;
	  beam_energy=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("expected float argument for --energy");
	  }
	}
	else if (strcmp(argv[arg_i],"--detector-centre")==0) {
	  if (arg_i+2>=n_args) {
	    throw runtime_error("expected two float argument for --detector-centre");
	  }
	  char *ptr;
	  detector_centre_x0=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("first argument for --detector-centre not a float");
	  }
	  detector_centre_y0=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("second argument for --detector-centre not a float");
	  }
	}
	else if (strcmp(argv[arg_i],"--beam-pos")==0) {
	  if (arg_i+2>=n_args) {
	    throw runtime_error("expected two float argument for --beam-pos");
	  }
	  char *ptr;
	  beam_pos_x=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("first argument for --beam-pos not a float");
	  }
	  beam_pos_y=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("second argument for --beam-pos not a float");
	  }
	}
	else if (strcmp(argv[arg_i],"--beam-type")==0) {
	  if (arg_i+1>=n_args) {
	    throw runtime_error("expected string argument for --beam-type");
	  }
	  BeamTypeMap_t::const_iterator a_beam_type=beam_types.find(argv[++arg_i]);
	  if (a_beam_type==beam_types.end()) {
	    std::stringstream message;
	    message << "unsupported beam type \"" << argv[arg_i] << "\".";
	    throw runtime_error(message.str());
	  }
	  else {
	    the_beam_type=a_beam_type->second;
	  }
	}
	else if (strcmp(argv[arg_i],"--since")==0) {
	  if (arg_i+2>=n_args) {
	    throw runtime_error("expected date(2005/02/28) and time (07:05:00) arguments for --since");
	  }
	  try {
	    since=parseDateTime(argv[arg_i+1],argv[arg_i+2]);
	  }
	  catch( std::runtime_error &err) {
	    std::cerr << "Error in argument --since:  " << err.what() << std::endl;
	    exit(-1);
	  }
	  arg_i+=2;
	}
	else if (strcmp(argv[arg_i],"--till")==0) {
	  if (arg_i+2>=n_args) {
	    throw runtime_error("expected date(2005/02/28) and time (07:05:00) arguments for --till");
	  }
	  try {
	    till=parseDateTime(argv[arg_i+1],argv[arg_i+2]);
	  }
	  catch( std::runtime_error &err) {
	    std::cerr << "Error in argument --till:  " << err.what() << std::endl;
	    exit(-1);
	  }
	  arg_i+=2;
	}
// 	else if (strcmp(argv[arg_i],"--tag")==0) {
// 	  if (arg_i+1>=n_args) {
// 	    throw runtime_error("expected string argument for --tag");
// 	  }
// 	  tag_name=argv[++arg_i];
// 	}
// 	else if (strcmp(argv[arg_i],"--comment")==0) {
// 	  if (arg_i+1>=n_args) {
// 	    throw runtime_error("expected string argument for --comment");
// 	  }
// 	  comment=argv[++arg_i];
// 	}
	else if (strcmp(argv[arg_i],"--write")==0) {
	  update_conditions_db=true;
	}
	else if (strcmp(argv[arg_i],"--write-file")==0) {
	  write_flat_file=true;
	}
	else if (strcmp(argv[arg_i],"--top-folder")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --top-folder");
 	  }
 	  top_folder=argv[++arg_i];
	}
	else if (strcmp(argv[arg_i],"--verbose")==0) {
	  verbose=true;
	}
	else if (strcmp(argv[arg_i],"--help")==0) {
	  show_help=true;
	}
	else {
	  std::stringstream message;
	  message << "unknwon argument \"" << argv[arg_i] << "\".";
	  throw runtime_error(message.str());
	}
      }
    }
    catch (std::exception &error) {
      print_help(argv[0]);
    
      std::cerr << "Error while parsing arguments:" << error.what() << std::endl;
      return -2;
    }
    if (show_help) {
      print_help(argv[0]);
      return -1;
    }
    if (since.timeStamp() >= till.timeStamp()) {
      std::cerr << "ERROR: Since time is not before till time : since=" << since.getDateString() 
		<< " till=" << till.getDateString() << std::endl;
      return -2;
    }

    if (top_folder.empty()) {
      std::cerr << "ERROR: No top folder given." << std::endl;
      return -2;
    }
  }
  // parse arguments
  // ________________________________________________________________________________

  // --------------------------------------------------------------------------------
  //  create (and write) conditions data
  // ------------------------------------

  try {

#ifdef USE_CONDDB
    if (top_folder.size()>0 && top_folder[top_folder.size()-1]=='/') {
      top_folder.erase(top_folder.size()-1);
    }

    std::stringstream total_folder_name;
    total_folder_name << top_folder << "/ExperimentalSetup";
    lccd::DBInterface db_experimental_setup( lccd::getDBInitString(), total_folder_name.str() , true ) ;
#endif
    // : beam_energy, angle_zx, angle_zy, impact_pos_x, impact_pos_y, detector_angle_zx, detector_rotation_x0, detector_rotation_y0, detector_x0, detector_y0, detector_z0
    std::string db_experimental_setup_description("ExperimentalSetup");
    
    //    LCTime   lastEvent( last_event[0],last_event[1],last_event[2],last_event[3],last_event[4],last_event[5]) ;


    LCCollection *setup_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;
    CALICE::ExperimentalSetup *an_experimental_setup=new CALICE::ExperimentalSetup;
    
    // currently the x - table position and the hit position are anti correlated
    // so the beam impact position has to have the sign inverted
    (*an_experimental_setup)
      .setPeakEnergy(beam_energy*energy_unit)
      .setBeamType(the_beam_type)
      .setBeamAngleZX(the_angle)
      .setBeamAngleZY(0.)
      .setBeamImpactPosition((-beam_pos_x)*length_unit,(-beam_pos_y)*length_unit)
      .setDetectorX0((-detector_centre_x0)*length_unit)
      .setDetectorY0((-detector_centre_y0)*length_unit)
      .setDetectorZ0((0.)*length_unit);
    
	// if rotating the detector
	//	  .setDetectorAngleZX(the_angle)
	//	  .setDetectorRotationX0(rotation_x0)
	//	  .setDetectorRotationZ0(rotation_z0)

	

    setup_col->addElement(an_experimental_setup);
    
    if (verbose) {
      std::cout << " since: " << since.getDateString() 
		<< "  till:" << till.getDateString();
      
      for (UInt_t element_i=0; element_i< static_cast<UInt_t>( setup_col->getNumberOfElements() ); element_i++) {
	CALICE::ExperimentalSetup an_element(setup_col->getElementAt(element_i));
	an_element.print(std::cout);
	std::cout <<endl;
      }
      std::cout <<endl;
    }
    
    if (update_conditions_db) {
#ifdef USE_CONDDB
      try {
	db_experimental_setup.storeCollection(since.timeStamp(),
					      till.timeStamp(),
					      setup_col,
					      db_experimental_setup_description);
      }
      catch (CondDBException &error){
	std::cout << "Error while storing data for time range :" 
		  << since.getDateString() << " - " 
		  << till.getDateString() << " :: "
		  << error.getErrorCode() << ":" << error.getMessage() << std::endl;
	exit(-1);
      }
      catch (lcio::Exception &err) {
	std::cout << "LCIO Exception:" << err.what() << std::endl;
	exit(-1);
      }

#else
      //FIXME: write flat file
#endif
    }
    delete setup_col;
      

    // finally create a database file for this folder
    if (update_conditions_db) {
#ifdef USE_CONDDB
      //     if (tag_name.empty()) {
      if (write_flat_file) {
      db_experimental_setup.createDBFile();  
      }
      //       }
      //       else {
      // 	db_experimental_setup.tagFolder(tag_name,comment);
// 	if (write_flat_file) {
// 	  db_experimental_setup.createDBFile(tag_name);  
// 	}
//       }
#else 
      // FIXME: write flat files
#endif
    }
  }
  
#ifdef USE_CONDDB
  catch (CondDBException &error){
    std::cout << "CondDB Exception:" << error.getErrorCode() << ":" << error.getMessage() << std::endl;
    exit(-1);
  }
#endif
  catch (lcio::Exception &err) {
    std::cout << "LCIO Exception:" << err.what() << std::endl;
    exit(-1);
  }
}

