/* to prevent conflicts with RtypesSubSet.h, Rtypes.h is included at the top*/
#include "RtypesSubSet.h"
#include "lcio.h"

/*#include "IO/LCWriter.h"*/
#include "EVENT/LCIO.h"

#include "EVENT/LCEvent.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"

#ifdef USE_CONDDB
#include "ConditionsDB/CondDBException.h"
#endif
#include "lccd.h"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <Exceptions.h>
#include <vector>
#include <cassert>
#include <cstring>

#include "createTriggerAssignmentConddata.hh"
#include "collection_names.hh"

using namespace lcio;
using namespace std;



void print_help(const char *prg_name)
{
  cout << prg_name << " [--help] [--top-folder name] [--write] [--write-file] \\" << endl
       << "\t [--input-file name] " << endl
       << endl
       << "--help             this text."<< endl
       << "--input-file name  file which contains lines with one integer denoting the bit"<<endl
       << "                   and one string for the trigger name: PEDESTAL, COSMICS, BEAM" <<endl
       << "                   ( empty lines and lines starting with # are ignored.)" << endl
       << "--top-folder name  Name to be given to the top folder." << endl
       << "--write            without this option nothing is written but only printed to the screen." << endl
       << "--write-file       write conditions data stored in the database folder into a LCIO file." << endl
       << endl
       << "--db-init-string   dbinitstring db init string in form host:database:username:password:[3306]." << endl;
}


int main(int argc, char** argv )
{
  bool show_help            = false;
  bool update_conditions_db = false;
  bool write_flat_file      = false;
  bool userdef_dbinit       = false;

  string top_folder;
  string input_file;
  string dbinit_string;

  /* --------------------------------------------------------------------------------
     parse arguments 
  */
  
  try {
    const int n_args = static_cast<int>(argc);

    for (int arg_i = 1; arg_i < n_args; arg_i++) 
      {
	if (strcmp(argv[arg_i], "--write") == 0) 
	  {
	    update_conditions_db = true;
	  }
 	else if (strcmp(argv[arg_i], "--input-file") == 0) 
	  {
	    if (arg_i+1 >= n_args) 
	      {
		throw runtime_error("expected string argument for --input-file");
	      }
	    input_file = argv[++arg_i];
	  }
	else if (strcmp(argv[arg_i], "--write-file") == 0) 
	  {
	    write_flat_file = true;
	  }
	else if (strcmp(argv[arg_i], "--top-folder") == 0) 
	  {
	    if (arg_i+1 >= n_args) 
	      {
		throw runtime_error("expected string argument for --top-folder");
	      }
	    top_folder = argv[++arg_i];
	}
        else if (strcmp(argv[arg_i],"--help")==0) {
	  show_help=true;
	}
        else if (strcmp(argv[arg_i], "--db-init-string") == 0) 
	  {
	    if (arg_i+1 >= n_args) 
	      {
		throw runtime_error("expected string argument for --db-init-string ");
	      }
	    userdef_dbinit = true;
	    dbinit_string  = argv[++arg_i];
	  }
	else 
	  {
	    stringstream message;
	    message << "unknwon argument \"" << argv[arg_i] << "\".";
	    throw runtime_error(message.str());
	  }
	
      }
  }
  catch (exception &error) 
    {
      print_help(argv[0]);
    
      cerr << "Error while parsing arguments:" << error.what() << endl;
      return -2;
    }

  if (show_help) 
    {
      print_help(argv[0]);
      return -1;
    }
  
    /* end of parsing the arguments
       ________________________________________________________________________________*/

  try {
#ifdef USE_CONDDB
    if (top_folder.size() > 0 && top_folder[top_folder.size()-1] == '/') 
      {
	top_folder.erase(top_folder.size()-1);
      }
    stringstream total_folder_name;
    total_folder_name << top_folder << "/CALDAQ_TriggerAssignment";

    if(!userdef_dbinit)  dbinit_string=lccd::getDBInitString();

    /*Need to check if we can create a dbinterface with the given dbinit_string
      If the wrong dbinit_string is given, a CondDBException will be thrown
     */
    lccd::DBInterface *db_trigger_assignment = NULL;

#ifdef USE_CONDDB
    try{
#endif
      db_trigger_assignment = new lccd::DBInterface(dbinit_string, total_folder_name.str(), true);
#ifdef USE_CONDDB
    }
    catch(CondDBException &e)
      {
	cout<<"Cannot create DBInterface with dbinit_string = "<<dbinit_string<<endl;
	exit(1);
      }
#endif

#endif
    string db_description_trigger_assigment("The trigger assignment.");
      
    vector<UTIL::LCTime> since_times;
    since_times.push_back(UTIL::LCTime(0));
    since_times.push_back(UTIL::LCTime(0));

    EVENT::LCCollection* trigger_assignment_col = createTriggerAssignmentConddata(since_times[0],since_times[1] , input_file )  ;

    assert( trigger_assignment_col->getNumberOfElements() > 0);
    assert( since_times[0].timeStamp() < since_times[1].timeStamp() );

    if (update_conditions_db) {
#ifdef USE_CONDDB
      db_trigger_assignment->storeCollection(since_times[0].timeStamp(),since_times[1].timeStamp(),
					    trigger_assignment_col, db_description_trigger_assigment);
#else
     
#endif
    }

    delete trigger_assignment_col;
  
    /* finally create a database file for this folder*/
    if (update_conditions_db) {
#ifdef USE_CONDDB

	if (write_flat_file) {
	  db_trigger_assignment->createDBFile();  
	}

#else 
  
#endif
    }

  }
#ifdef USE_CONDDB
  catch (CondDBException &error){
    cout << "CondDB Exception:" << error.getErrorCode() << ":" << error.getMessage() << endl;
    exit(-1);
  }
#endif
 catch (lcio::Exception &err) {
   cout << "LCIO Exception:" << err.what() << endl;
    exit(-1);
 }
 catch (exception &err) {
   cout << "Exception:" << err.what() << endl;
   exit(-1);
 }
}

