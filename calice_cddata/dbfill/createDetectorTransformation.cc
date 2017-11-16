// to prevent conflicts with RtypesSubSet.h, Rtypes.h is included at the top
#include "RtypesSubSet.h"
#include "lcio.h"

#include "EVENT/LCIO.h"

#include "EVENT/LCEvent.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"

#ifdef USE_CONDDB
#include "ConditionsDB/CondDBException.h"
#endif
#include "lccd.h"
#include "lccd/DBInterface.hh"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include "createDetectorTransformationConddata.hh"

#include "collection_names.hh"

using namespace lcio ;
using namespace std;

using std::cout;
using std::endl;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;


/****************************************************************************************/
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void print_help(const char *prg_name)
{
  cout << prg_name << " [--help] [--top-folder name] [--write] [--write-file] \\" << endl
	    << "\t [--input-file name] --ecal/hcal/tcmt" << endl
	    << endl
	    << "--help             this text."<< endl
	    << endl
            << "--ecal/hcal/tcmt/ref  parameters are for this sub-detector or reference (obligatory)." << endl
	    << "--input-file name  of the format:"<<endl
	    << endl
	    << "# example input file to create DetectorTransformation conditions data" << endl
	    << "validity: 2006/08/26 18:47:30 2006/08/28 00:31:58" << endl
	    << "rotationAngleZX:   45" <<endl
	    << "rotationX0:        0" << endl
	    << "rotationZ0:        0" << endl
	    << "detectorPostionX:  0" << endl
	    << "detectorPostionY: -9" << endl
	    << "detectorPostionZ:  0" << endl
	    << endl
            << "--top-folder name  Name to be given to the top folder (obligatory)."
	    << "--write            without this option nothing is written but only printed to the screen." << endl
	    << "--write-file       write conditions data stored in the database folder into a LCIO file." << endl
	    << endl
            << "--db-init-string   db init string in form host:database:username:password:[3306]." << endl;
}

/****************************************************************************************/
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
int main(int argc, char** argv )
{
  
  bool show_help            = false;
  bool update_conditions_db = false;
  bool write_flat_file      = false;
  std::string top_folder;
  std::string input_file;
  std::string dbinit_string;
  bool userdef_dbinit(false);
  
  std::vector<std::string> sub_det_name;
  sub_det_name.push_back(std::string("Ecal"));
  sub_det_name.push_back(std::string("Hcal"));
  sub_det_name.push_back(std::string("Tcmt"));
  sub_det_name.push_back(std::string("ExperimentalHall"));
  std::vector<std::string> col_add_on;
  col_add_on.push_back(std::string());
  col_add_on.push_back(std::string());
  col_add_on.push_back(std::string("Tcmt"));
  col_add_on.push_back(std::string());
  UInt_t sub_det=sub_det_name.size();

  assert( col_add_on.size() == sub_det_name.size());

  /* --------------------------------------------------------------------------------
     parse arguments 
     ----------------------------------------------------------------------------------*/
  try 
    {
      const UInt_t n_args = static_cast<UInt_t>(argc);

      for (UInt_t arg_i = 1; arg_i < n_args; arg_i++) 
	{
	  if (strcmp(argv[arg_i],"--write") == 0) 
	    {
	      update_conditions_db = true;
	    }
	  else if (strcmp(argv[arg_i],"--input-file") == 0) 
	    {
	      if (arg_i + 1 >= n_args) 
		{
		  throw runtime_error("expected string argument for --input-file");
		}
	      input_file=argv[++arg_i];
	    }
	  else if (strcmp(argv[arg_i],"--write-file") == 0) 
	    {
	      write_flat_file=true;
	    }
	  else if (strcmp(argv[arg_i],"--ecal") == 0) 
	    {
	      sub_det=0;
	    }
	  else if (strcmp(argv[arg_i],"--hcal") == 0) 
	    {
	      sub_det=1;
	    }
	  else if (strcmp(argv[arg_i],"--tcmt") == 0) 
	    {
	      sub_det=2;
	    }
	  else if (strcmp(argv[arg_i],"--ref") == 0) 
	    {
	      sub_det=3;
	    }
	  else if (strcmp(argv[arg_i],"--top-folder") == 0) 
	    {
	      if (arg_i + 1 >= n_args) 
		{
		  throw runtime_error("expected string argument for --top-folder");
		}
	      top_folder = argv[++arg_i];
	}
	else if (strcmp(argv[arg_i],"--help") == 0) 
	  {
	    show_help=true;
	  }
        else if (strcmp(argv[arg_i],"--db-init-string") == 0) 
	  {
	    if (arg_i + 1 >= n_args) 
	      {
		throw runtime_error("expected string argument for --db-init-string ");
	    }
	    userdef_dbinit = true;
	    dbinit_string  = argv[++arg_i];
        }
	else 
	  {
	    std::stringstream message;
	    message << "unknwon argument \"" << argv[arg_i] << "\".";
	    throw runtime_error(message.str());
	  }
	  
	}
    }
  catch (std::exception &error) 
    {
      print_help(argv[0]);
      
      std::cerr << "Error while parsing arguments:" << error.what() << endl;
      return -2;
    }
  if (show_help) 
    {
      print_help(argv[0]);
      return -1;
    }
  if (top_folder.empty()) 
    {
      std::cerr << "ERROR: No top folder name given. Abort." << endl;
      return -2;
    }
  if (sub_det >=sub_det_name.size()) 
    {
      std::cerr << "ERROR: No sub detector specified: --ecal, --hcal --tcmt --ref." << endl;
      return -2;
    }
  
  /* --------------------------------------------------------------------------------
     end parsing arguments 
     ----------------------------------------------------------------------------------*/


  try 
    {
#ifdef USE_CONDDB
      if (top_folder.size() > 0 && top_folder[top_folder.size()-1] == '/') 
	{
	  top_folder.erase(top_folder.size()-1);
	}

      std::stringstream total_folder_name;
      if(sub_det < 3) 
	{
	  total_folder_name << top_folder << "/" << sub_det_name[sub_det] << "/" << col_add_on[sub_det] << "DetectorPosition";
	} 
      else 
	{
	  total_folder_name << top_folder << "/" << sub_det_name[sub_det] << "/" << col_add_on[sub_det] << "ReferenceFrame";
	}

      if(!userdef_dbinit)  dbinit_string=lccd::getDBInitString();
      lccd::DBInterface db_detector_position( dbinit_string, total_folder_name.str() , true ) ;

      cout<<"\n folder name: "<<total_folder_name.str()<<endl;
#endif
    std::string db_description_trigger_assigment("Detector Transformation.");
    
    std::vector<UTIL::LCTime> since_times;
    since_times.push_back(UTIL::LCTime(0));
    since_times.push_back(UTIL::LCTime(0));
    
    EVENT::LCCollection* detector_position_col = createDetectorTransformationConddata(since_times[0],since_times[1] , input_file )  ;
    
    assert( detector_position_col->getNumberOfElements() > 0);
    assert( since_times[0].timeStamp() < since_times[1].timeStamp() );
    
    if (update_conditions_db) 
      {
#ifdef USE_CONDDB
	db_detector_position.storeCollection(since_times[0].timeStamp(),since_times[1].timeStamp(),
					     detector_position_col, db_description_trigger_assigment);
#endif
      }
    delete detector_position_col;
    
    /* finally create a database file for this folder*/
    if (update_conditions_db) 
      {
#ifdef USE_CONDDB
	if (write_flat_file) 
	  {
	    db_detector_position.createDBFile();  
	  }
#endif
      }
    
    }
#ifdef USE_CONDDB
  catch (CondDBException &error)
    {
      cout << "CondDB Exception:" << error.getErrorCode() << ":" << error.getMessage() << endl;
      exit(-1);
    }
#endif
  catch (lcio::Exception &err) 
    {
      cout << "LCIO Exception:" << err.what() << endl;
      exit(-1);
    }
  catch (std::exception &err) 
    {
      cout << "Exception:" << err.what() << endl;
      exit(-1);
    }
}

