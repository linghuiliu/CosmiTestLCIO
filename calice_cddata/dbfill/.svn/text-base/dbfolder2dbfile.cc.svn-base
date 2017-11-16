#ifdef USE_CONDDB
#include <ConditionsDB/CondDBException.h>
#endif

#include <lccd.h>
#include <lccd/DBInterface.hh>
#include <lccd/ConditionsMap.hh>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>

typedef unsigned int UInt_t;

int print_help(const char *prg_name)
{
  std::cout << prg_name << " [--help] [--folder name] [--tag name]" << std::endl 
	    << std::endl
	    << "[--folder name]    Folder for which an slcio file should be generated."
     	    << "--tag [name]       set this tag for the conditions data." << std::endl
	    << std::endl
	    << "--help             this text."<< std::endl
	    << std::endl
	    << std::endl;

  return -1;
}



int main(int argc, char** argv ){

  std::string tag_name;
  std::vector<std::string> folder;

  // --------------------------------------------------------------------------------
  // parse arguments 
  {

    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      for (UInt_t arg_i=1; arg_i<n_args; arg_i++) {
 	if (strcmp(argv[arg_i],"--tag")==0) {
	  if (arg_i+1>=n_args) {
 	    throw std::runtime_error("expected string argument for --tag");
 	  }
 	  tag_name=argv[++arg_i];
 	}
	else if (strcmp(argv[arg_i],"--folder")==0) {
	  if (arg_i+1>=n_args) {
	    throw std::runtime_error("expected string argument for --folder");
	  }
	  folder.push_back(std::string(argv[++arg_i]));
	}
	else if (strcmp(argv[arg_i],"--help")==0) {
	  return print_help(argv[0]);
	}
	else {
	  std::stringstream message;
	  message << "unknwon argument \"" << argv[arg_i] << "\".";
	  throw std::runtime_error(message.str());
	}
      }
    }
    catch (std::exception &error) {
      print_help(argv[0]);
    
      std::cerr << "Error while parsing arguments:" << error.what() << std::endl;
      return -2;
    }
  }

  // parse arguments
  // ________________________________________________________________________________


  try {
    for (std::vector<std::string>::const_iterator iter=folder.begin(); iter!=folder.end(); iter++) {
      lccd::DBInterface db( lccd::getDBInitString(), *iter , true ) ;
      db.createDBFile(tag_name);  
    }
  } catch (CondDBException &error){
    std::cout << "CondDB Exception:" << error.getErrorCode() << ":" << error.getMessage() << std::endl;
    exit(-1);
  } catch (std::exception &error) {
    std::cout << "Exception:" << error.what() << std::endl;
    exit(-1);
  }

  return 0;

}

