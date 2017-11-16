
#include "lcio.h"

//#include "IO/LCWriter.h"
#include <EVENT/LCIO.h>

#include <IMPL/LCCollectionVec.h>
#include <UTIL/LCTime.h>

#ifdef USE_CONDDB
#include <ConditionsDB/CondDBException.h>
#endif
#include <lccd.h>
#include <lccd/DBInterface.hh>
#include <lccd/ConditionsMap.hh>

#include <ReadDaqMap/parseDateTime.hh>

#include "createBeamParameterConddata.hh"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <Exceptions.h>
#include <vector>
#include <cassert>
#include <cstring>

using namespace lcio ;
using namespace std;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

void print_help(const char *prg_name)
{
  std::cout << prg_name << " [--help] [--top-folder name] [--write] [--write-file] \\" << std::endl
	    << "\t[--input-file name --db-init-string]"
    //	    << "\t [--tag name] [--comment name]" << std::endl
	    << std::endl
	    << "--help             this text."<< std::endl
    //    	    << "--tag [name]       set this tag for the conditions data." << std::endl
    //    	    << "--comment [test]   add this comment to the conditions data." << std::endl
	    << std::endl
	    << "--input-file name   The file which contains the beam paramters to be filled."
	    << std::endl
            << "--top-folder name  Name to be given to the top folder."
	    << "--write            without this option nothing is written but only printed to the screen." << std::endl
	    << "--write-file       write conditions data stored in the database folder into a LCIO file." << std::endl
	    << std::endl
	    << "--description      Create descriptions of the modules." << std::endl
	    << "--locations        Create the module location conditions data (requires --input-file)." << std::endl
	    << std::endl
	    << "--db-init-string   db init string in form host:database:username:password:[3306]." << std::endl;
  
}


int main(int argc, char** argv ){

  bool show_help=false;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  std::string top_folder;
  //  std::string tag_name;
  //  std::string comment;
  std::string input_file;
  std::string dbinit_string;


  bool userdef_dbinit(false);


  // --------------------------------------------------------------------------------
  // parse arguments 
  {

    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      for (UInt_t arg_i=1; arg_i< n_args ; arg_i++) {
	if (strcmp(argv[arg_i],"--write")==0) {
	  update_conditions_db=true;
	}
	else if (strcmp(argv[arg_i],"--write-file")==0) {
	  write_flat_file=true;
	}
	else if (strcmp(argv[arg_i],"--input-file")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --input-file");
 	  }
 	  input_file=argv[++arg_i];
	}
	else if (strcmp(argv[arg_i],"--top-folder")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --top-folder");
 	  }
 	  top_folder=argv[++arg_i];
	}
	else if (strcmp(argv[arg_i],"--help")==0) {
	  show_help=true;
	}
	else if (strcmp(argv[arg_i],"--db-init-string")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --db-init-string ");
 	  }
	  userdef_dbinit=true;
	  dbinit_string=argv[++arg_i];
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

    if (input_file.size()==0) {
      std::cerr << "ERROR: No input file specified which contains the beam parameters:" << std::endl;
      return -2;
    }
  }

  // parse arguments
  // ________________________________________________________________________________


  try {
    //  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;
#ifdef USE_CONDDB
    if (top_folder.size()>0 && top_folder[top_folder.size()-1]=='/') {
      top_folder.erase(top_folder.size()-1);
    }
#endif
    std::string db_beam_params_description("Beam parameters missing or doubtful after r/o");

 

    // ------- set module description
#ifdef USE_CONDDB
#endif
      //Year,month, dat,hour,min,sec
      LCTime run_min(0) ;
      LCTime run_max(1) ;
      
      //      EVENT::LCCollection* module_description_col = createEcalModuleDescriptionConddata(firstModule,farFuture,!update_conditions_db);
      EVENT::LCCollection* beam_params_col = createBeamParameterConddata(run_min, run_max, input_file);
      std::cout << "first run: " << run_min.timeStamp() << std::endl;
      std::cout << "last run: " << run_max.timeStamp() -1 << std::endl;
      

      std::stringstream beamparams_folder_name;
      beamparams_folder_name << top_folder << "/BeamParameterException";
      if(!userdef_dbinit)  dbinit_string=lccd::getDBInitString();
      lccd::DBInterface db_beam_params( dbinit_string, beamparams_folder_name.str() , true ) ;

      if (update_conditions_db) {
#ifdef USE_CONDDB

	db_beam_params.storeCollection(run_min.timeStamp(),run_max.timeStamp(),beam_params_col, db_beam_params_description);
	// finally create a database file for this folder
	//      if (tag_name.empty()) {
	if (write_flat_file) {
	  db_beam_params.createDBFile();  
	}
#else
	//FIXME: write flat file
#endif
      }
      //FIXME: Do I have to delete the collection?
      delete beam_params_col;
    

    // create a simple file with the conditions data:
    //    db.createSimpleFile( runStart.timeStamp() , "" ) ;
  
    //------------------------------------------------------------------------

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

