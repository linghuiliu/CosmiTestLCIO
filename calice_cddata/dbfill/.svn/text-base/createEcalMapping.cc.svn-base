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
#include <ReadDaqMap/ReadDaqMap.hh>
#include <collection_names.hh>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <exception>
#include <cstring>

#include <ReadDaqMap/parseDateTime.hh>
#include "createEcalMappingConddata.hh"

using namespace lcio ;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

void print_help(const char *prg_name)
{
  std::cout << prg_name << " [--help] [--top-folder name] [--write] [--write-file] \\" << std::endl 
	    << "\t\t[--data-dir name] [--date date time] [--crate nr] [--map-dir name]" << std::endl
    //	    << "\t\t [--tag name] [--comment name]" << std::endl
	    << std::endl
	    << "--help             this text."<< std::endl
//     	    << "--tag [name]       set this tag for the conditions data." << std::endl
//     	    << "--comment [test]   add this comment to the conditions data." << std::endl
	    << std::endl
	    << "--map-file name    The name of the file which contains the mapping between PCBs and the front-ends."
            << "                        data/dat -> data/map" << std::endl
	    << "--date date time   start then end date and time in the format \"2005/02/28\" and \"00:01:00\"." << std::endl
	    << "--crate nr         Crate number which houses the ecal crc boards." << std::endl
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
   std::string input_file;
  std::string dbinit_string;
  bool userdef_dbinit(false);

  vector<std::string> map_file;
  vector<LCTime>      map_since_time;
  UInt_t crate_nr=static_cast<UInt_t>(-1);

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
	if (strcmp(argv[arg_i],"--map-file")==0) {
	  if (arg_i+1>=n_args) {
	    throw runtime_error("expected string argument for --map-file");
	  }
	  map_file.push_back(string(argv[++arg_i]));
	}
	else if (strcmp(argv[arg_i],"--date")==0) {
	  if (arg_i+2>=n_args) {
	    throw runtime_error("expected date(2005/02/28) and time (07:05:00) arguments for --date");
	  }
	  map_since_time.push_back(parseDateTime(argv[arg_i+1],argv[arg_i+2]));
	  arg_i+=2;
	}
	else if (strcmp(argv[arg_i],"--crate")==0) {
	  if (arg_i+1>=n_args) {
	    throw runtime_error("expected numeric argument for --crate");
	  }
	 crate_nr=atoi(argv[++arg_i]);
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

  if (map_file.empty()) {
    std::cerr << "No map files given.";
    return -1;
  }
  if (!map_since_time.empty() && map_file.size()!=map_since_time.size() && map_file.size()+1!=map_since_time.size() ) {
    std::cerr << "Not enough/too many since/till dates given for the given map files.";
    return -1;
  }


  // parse arguments
  // ________________________________________________________________________________


  try {
  //  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;


  //  LCCollectionVec *module_location_col=new LCCollectionVec(LCIO::LCGENERICOBJECT);


                    //Year,month, dat,hour,min,sec
  LCTime   dummy( 2015,     3,   1,    4 ,  1 , 5);
  if (map_since_time.size()==map_file.size()) {
    map_since_time.push_back(dummy);
  }

  //LCFloatVec* impact_par_col = new LCFloatVec;
#ifdef USE_CONDDB
    if (top_folder.size()>0 && top_folder[top_folder.size()-1]=='/') {
      top_folder.erase(top_folder.size()-1);
    }
    std::stringstream total_folder_name;
    total_folder_name << top_folder << "/Ecal/Mapping";
    lccd::DBInterface db_mapping( dbinit_string, total_folder_name.str() , true ) ;

#endif
  
  // loop over front_ends;
  
  std::cout << "Size of Daq File Map: " << map_file.size() << std::endl;
  std::vector<std::string>::const_iterator  map_iter=map_file.begin();
  UInt_t map_i=0;
  for (;map_iter!=map_file.end();map_iter++,map_i++) {

    UTIL::LCTime since(0);
    UTIL::LCTime till(0);
    if (!map_since_time.empty()) {
      if (map_i+1>=map_since_time.size()) {
	throw std::logic_error("Not enough since/till time stamps for the given map files.");
      }
      since=map_since_time[map_i];
      till=map_since_time[map_i+1];
    }

    lcio::LCCollection *mapping_col = createEcalMappingConddata( since, till, *map_iter, crate_nr );


    if (update_conditions_db) {
#ifdef USE_CONDDB
      std::string db_mapping_description("Between front-ends and the modules ");

      db_mapping.storeCollection         ( since.timeStamp(), till.timeStamp(), mapping_col,   db_mapping_description);
#else
      // FIXME::write flat file
#endif
    }
    // FIXME: Do I have to delete the collection?
    delete mapping_col;

  }
    
    
  
  // create a simple file with the conditions data:
  //    db.createSimpleFile( runStart.timeStamp() , "" ) ;
  
  //------------------------------------------------------------------------

  
  // finally create a database file for this folder
#ifdef USE_CONDDB
  //  if (tag_name.empty()) {
    if (write_flat_file) {
      db_mapping.createDBFile();  
    }
//   }
//   else {
//     db_mapping.tagFolder(tag_name,comment);
    
//     if (write_flat_file) {
//       db_mapping.createDBFile(tag_name);  
//     }
//   }
#else
    // FIXME::write flat file
#endif
  }
#ifdef USE_CONDDB
  catch (CondDBException &error){
    std::cout << "CondDB Exception:" << error.getErrorCode() << ":" << error.getMessage() << std::endl;
    exit(-1);
  }
#endif
 catch (std::exception &error) {
    std::cout << "Exception:" << error.what() << std::endl;
    exit(-1);
 }

}

