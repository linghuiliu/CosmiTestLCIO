// to prevent conflicts with RtypesSubSet.h, Rtypes.h is included at the top
#include "lcio.h"

//#include "IO/LCWriter.h"
#include <EVENT/LCIO.h>

#include <EVENT/LCEvent.h>
#include <EVENT/LCCollection.h>
#include <UTIL/LCTime.h>

#ifdef USE_CONDDB
#include <ConditionsDB/CondDBException.h>
#endif
#include <lccd.h>
#include <lccd/DBInterface.hh>
#include <lccd/ConditionsMap.hh>

#include <createEcalCalibrationConstantsConddata.hh>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>

#include <Exceptions.h>

#ifdef __APPLE__
#include <float.h>
#else
#include <values.h>
#endif


using namespace lcio ;
using namespace std;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

void print_help(const char *prg_name)
{
  std::cout << prg_name << " [--help] [--numfiles number] [--source-files name(s)] [--write] [--write-file] \\" << std::endl
	    << "\t [--top-folder name]"
    //	    << "\t [--tag name] [--comment name]" << std::endl
	    << std::endl
	    << "--help             this text."<< std::endl
    //    	    << "--tag [name]       set this tag for the conditions data." << std::endl
    //    	    << "--comment [test]   add this comment to the conditions data." << std::endl
	    << std::endl
            << "--numfiles [unsigned int] Number of root files with mip signal fit results. " << std::endl
	    << "--source-files [name1] ... [name3]  Name of the ROOT file(s) with the mip signal fit results." << std::endl
	    << "--nominal-mpv [float] the nominal MPV after calibration." << std::endl
	    << std::endl
	    << "--calibration-constant-range min-value max-value" << std::endl
	    << "                   The allowed range (min-value,max-value] of the calibration constants. Cells" << std::endl
	    << "                   with smaller or larger values are declared dead (constant set to zero)." << std::endl
	    << std::endl
            << "--top-folder name  Name to be given to the top folder."
	    << "--write            without this option nothing is written but only printed to the screen." << std::endl
	    << "--write-file       write conditions data stored in the database folder into a LCIO file." << std::endl
             << "--db-init-string   db init string in form host:database:username:password:[3306]." 
	    << std::endl;

}

int main(int argc, char** argv ){

  bool show_help=false;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  std::string top_folder;
  //  std::string tag_name;
  //  std::string comment;
  //std::string fit_result_file_name;
  unsigned int numfiles;
  std::vector<std::string> fit_result_file_names;
  std::string dbinit_string;
  bool userdef_dbinit(false);

  Float_t min_calibration_constant=0.;
  Float_t max_calibration_constant=FLT_MAX;

  Float_t nominal_mpv=1.;  // measure energy in units of mips

  // --------------------------------------------------------------------------------
  // parse arguments 
  {

    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      for (UInt_t arg_i=1; arg_i<n_args; arg_i++) {
// 	if (strcmp(argv[arg_i],"--tag")==0) {
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
//	else 
        if (strcmp(argv[arg_i],"--numfiles")==0) {
	  std::cout << "Parsing numfiles: " << std::endl;
	  if (arg_i+1>=n_args) {
	    throw runtime_error("Expecting file number of files argument after --numfiles");
	  }
          char *ptr;
	  numfiles=strtol(argv[++arg_i],&ptr,0);
          if (ptr && *ptr) {
	    throw runtime_error("Argument for --numfiles not am int");
	  }
	  std::cout << "numfiles: " << numfiles << std::endl;
          arg_i++;
	  std::cout << " String to come " << argv[arg_i] << std::endl;
	
	}
        if (strcmp(argv[arg_i],"--source-files")==0) {
	  if (arg_i+1>=n_args) {
	    throw runtime_error("Expecting file name argument after --source-files");
	  }
          for(unsigned int i=0; i<numfiles;i++){ 
	    fit_result_file_names.push_back(string(argv[++arg_i]));
          }
          for(unsigned int i=0; i<numfiles;i++){ 
	    std::cout << "Filename: " << fit_result_file_names.at(i) << std::endl;;
          }

	}
	else if (strcmp(argv[arg_i],"--nominal-mpv")==0) {
	  if (arg_i+1>=n_args) {
	    throw runtime_error("Expecting one float argument for --nominal-mpv");
	  }
	  char *ptr;
	  nominal_mpv=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("Argument for --nominal-mpv not a float");
	  }
	}
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
	else if (strcmp(argv[arg_i],"--db-init-string")==0) {
          if (arg_i+1>=n_args) {
            throw runtime_error("expected string argument for --db-init-string ");
          }
          userdef_dbinit=true;
          dbinit_string=argv[++arg_i];
        }
	else if (strcmp(argv[arg_i],"--calibration-constant-range")==0) {
	  if (arg_i+2>=n_args) {
	    throw runtime_error("Expecting two float arguments for --calibration-constant-range");
	  }
	  char *ptr;
	  min_calibration_constant=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("First argument for --calibration-constant-range not a float");
	  }
	  max_calibration_constant=strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("Second argument for --calibration-constant-range not a float");
	  }
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
  }

  // parse arguments
  // ________________________________________________________________________________


  try {
    
#ifdef USE_CONDDB
    /*    if (top_folder.size()>0 && top_folder[top_folder.size()-1]=='/') {
      top_folder.erase(top_folder.size()-1);
    }
    std::stringstream total_folder_name;
    total_folder_name << top_folder << "/Ecal/CalibrationConstants";
    */
    std::stringstream total_folder_name;
    total_folder_name << top_folder << "/Ecal/CalibrationConstants";
    if(!userdef_dbinit)  dbinit_string=lccd::getDBInitString();
    lccd::DBInterface db_calibration_constants(dbinit_string, total_folder_name.str() , true ) ;
#endif
    std::string db_calibration_constants_description("EcalModuleCalibration : Calibration constants of all cells of all modules chip+chennel * n_chips.");
      
    //Year,month, dat,hour,min,sec
    //LCTime   first_event( 2004,     12,   16,    0 ,  0 , 0)
    //LCTime   first_event( 2008,     4,   01,    0 ,  0 , 0);
    //LCTime   last_possible_event( 2008,     7,   31,    23 ,  59 , 59);

    //LCTime   first_event( 2007,     6,   23,    0 ,  0 , 0);
    //LCTime   last_possible_event( 2007,     12,   31,    23 ,  59 , 59);

    //2008 - T20 First July period
    //LCTime   first_event( 2008,     7,   6,    23 ,  0 , 0);
    //LCTime   last_possible_event( 2008,     7,   13,    20 ,  59 , 59);

    //2008 - T20 Second July period
    LCTime   first_event( 2008,     7,   15,    11 ,  0 , 0);
    LCTime   last_possible_event( 2008,     7,   15,    13 ,  0, 0);

    //2008 - T10 July running
    //LCTime   first_event( 2008,     7,   13,    21 ,  0 , 0);
    //LCTime   last_possible_event( 2008,     8,   3,    0 ,  0 , 0);
  
    //2011 
    //LCTime   first_event( 2011,     4,   1,    0 ,  0 , 0);
    //LCTime   last_possible_event( 2011,     5,   31,    23 ,  59 , 59);


    EVENT::LCCollection *calibration_constants_col=createEcalCalibrationConstantsConddata(first_event,last_possible_event,
											  //Hack 
											  fit_result_file_names,
											  nominal_mpv,min_calibration_constant,max_calibration_constant,
											  true);
    if (update_conditions_db) {
#ifdef USE_CONDDB
      db_calibration_constants.storeCollection(first_event.timeStamp(),last_possible_event.timeStamp(),calibration_constants_col,db_calibration_constants_description);
#else
      //FIXME: write flat file
#endif
    }
    //FIXME: Do I have to delete the collection?
    delete calibration_constants_col;
  
    // finally create a database file for this folder
    if (update_conditions_db) {
#ifdef USE_CONDDB
      //      if (tag_name.empty()) {
	if (write_flat_file) {
	  db_calibration_constants.createDBFile();  
	}
//       }
//       else {
// 	db_calibration_constants.tagFolder(tag_name,comment);
	
// 	if (write_flat_file) {
// 	  db_calibration_constants.createDBFile(tag_name);  
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
 catch (std::exception &err) {
   std::cout << "Exception:" << err.what() << std::endl;
   exit(-1);
 }
}

