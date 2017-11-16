
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

#include "createEcalModuleLocationConddata.hh"
#include "createEcalModuleDescriptionConddata.hh"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <Exceptions.h>
#include <vector>
#include <cassert>

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
	    << "--input-file name    The name of the file which contains the layer and shift values of connected locations."
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


  bool create_description(false);
  bool create_location(false);
  bool userdef_dbinit(false);


  // --------------------------------------------------------------------------------
  // parse arguments 
  {

    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      for (UInt_t arg_i=1; arg_i< n_args ; arg_i++) {
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
	else if (strcmp(argv[arg_i],"--description")==0) {
	  create_description=true;
	}
	else if (strcmp(argv[arg_i],"--locations")==0) {
	  create_location=true;
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

    if (create_location && input_file.size()==0) {
      std::cerr << "ERROR: No input file specified which describes the module locations:" << std::endl;
      return -2;
    }
    if (!create_location && !create_description) {
      //      std::cerr << "ERROR: You have to specify at least one of: --description or --locations." << std::endl;
      // return -2;
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
    std::string db_module_description_description("Description of modules: type, x,y pos. of cells, width and height of the modules and cells");

 
    if (!create_location && !create_description) {
      std::cerr << "ERROR: You have to specify at least one of: --description or --locations." << std::endl;
       return -2;
    }

    // ------- set module description
#ifdef USE_CONDDB
#endif
    
    if (create_description) {
      //Year,month, dat,hour,min,sec
      LCTime firstModule( 2004,     1,   2,   8,  0 , 0) ;
      LCTime   farFuture( 2015,     3,   1,   4 , 1 , 5 ) ;
      
      //      EVENT::LCCollection* module_description_col = createEcalModuleDescriptionConddata(firstModule,farFuture,!update_conditions_db);
      EVENT::LCCollection* module_description_col = createEcalModuleDescriptionConddata(firstModule,farFuture,true);
      
      std::stringstream descr_folder_name;
      descr_folder_name << top_folder << "/Ecal/ModuleDescription";
      if(!userdef_dbinit)  dbinit_string=lccd::getDBInitString();
      lccd::DBInterface db_module_descriptions( dbinit_string, descr_folder_name.str() , true ) ;

      if (update_conditions_db) {
#ifdef USE_CONDDB
	db_module_descriptions.storeCollection(firstModule.timeStamp(),farFuture.timeStamp(),module_description_col,db_module_description_description);
	// finally create a database file for this folder
	//      if (tag_name.empty()) {
	if (write_flat_file) {
	  db_module_descriptions.createDBFile();  
	}
	//       }
	//       else {
	// 	db_module_descriptions.tagFolder(tag_name,comment);
	
	// 	if (write_flat_file) {
	// 	  db_module_descriptions.createDBFile(tag_name);  
	// 	}
	//       }
#else
	//FIXME: write flat file
#endif
      }
      //FIXME: Do I have to delete the collection?
      delete module_description_col;
    }

    if (create_location) {
      // ------ set module location
#ifdef USE_CONDDB
      std::stringstream location_folder_name;
      location_folder_name << top_folder << "/Ecal/ModuleLocation";
      if(!userdef_dbinit) dbinit_string=lccd::getDBInitString(); 
      lccd::DBInterface db_module_positions( dbinit_string, location_folder_name.str() , true ) ; 
#endif
      
      std::string db_module_position_description("The positions (x/y/z),layer nr, module type (centre (normal,flipped), lower (r,l))");
      
      UTIL::LCTime _since(0);
      UTIL::LCTime _till(0);

      EVENT::LCCollection *module_position_col=createEcalModuleLocationConddata(_since,_till, input_file);
      
      if (update_conditions_db) {
#ifdef USE_CONDDB
	//	std::cout << runStart.getDateString()  << " - " << configurationEnd.getDateString() << std::endl;
	db_module_positions.storeCollection         ( _since.timeStamp(), _till.timeStamp(), module_position_col,   db_module_position_description);
	
	//      if (tag_name.empty()) {
	if (write_flat_file) {
	  db_module_positions.createDBFile();
	}
	//       }
	//       else {
	// 	db_module_positions.tagFolder(tag_name,comment);
	
	// 	if (write_flat_file) {
	// 	  db_module_positions.createDBFile(tag_name);
	// 	}
	//       }
#else
	// FIXME: write flat file
#endif
      }
      //FIXME: Do I have to delete the collection
      delete module_position_col;
    }
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

