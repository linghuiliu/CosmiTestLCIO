//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:    createDetectorTransformationSimple.cc
// Package: cddata
// Purpose: store detector transformation info into the conditions DB
//
// 20070822 - G.Lima  created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif

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

#include "DetectorTransformation.hh"
#include "GLTimeUtil.hh"
#include "Exceptions.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstring>

using namespace lcio;
using namespace CALICE;
using namespace std;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

const UInt_t NLAYERS = 16;
// units
Float_t mm = 1;
Float_t cm = 10*mm;
Float_t in = 25.4*mm;


void print_help(const char *prg_name)
{
  cout <<"\n"<< prg_name << " [--help] [--verbose] \\" << endl
       << "\t[--detector-face x y z] [--angleXZ deg] [--rotationAxisXZ x z] \\" << endl
       << "\t[--since date] [--till date]" << endl
       << "\t[--folder name] [--write] [--write-file] \\" << endl
       << endl
       << "--help             this text."<< endl
       << "--verbose          enable debugging output"<< endl
       << endl
//     << "--tag [name]       set this tag for the conditions data." << endl
//     << "--comment [test]   add this comment to the conditions data." << endl
       << endl
       << "--since date  initial validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << "--till date   final validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << endl
       << "--detector-face # # #   x,y,z position of the detector face (mm)." << endl
       << "--angleXZ [float]  measured angle in degrees." << endl
       << "--rotationAxisXZ [float] [float]    rotation axis in XZ plane." << endl
       << endl

       << "--folder name      Name of the folder where collection will be stored." << endl
       << "--write            without this option nothing is written but only printed to the screen." << endl
       << "--write-file       write conditions data stored in the database folder into a LCIO file." << endl
       << endl;
}


int main(int argc, char** argv ) {

  // ATTENTION:  The default units used for LCObjects are mm

  bool show_help=false;
  bool verbose = false;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  string folder;

  Float_t the_angle = 0;
  Float_t rotAxis_x0 = 0;
  Float_t rotAxis_z0 = 0;
  Float_t detector_face_x0 = 0;
  Float_t detector_face_y0 = 0;
  Float_t detector_face_z0 = 0;

  //default timestamp 
  struct timespec cur_time;
  long64 time_cur;
  clock_gettime(CLOCK_REALTIME, &cur_time);
  time_cur = (long64) ( (cur_time.tv_sec)*SECTONS + cur_time.tv_nsec );
  LCTime now(time_cur);
  lccd::LCCDTimeStamp _since = now.timeStamp();
  lccd::LCCDTimeStamp _till = now.timeStamp();

  // ------------------------------------------------------------------------
  // parse arguments 

  try {
    const UInt_t n_args=static_cast<UInt_t>(argc);
    if(n_args==1) {  // no arguments given
      print_help(argv[0]);
      return -2;
    }

    for (UInt_t arg_i=1; arg_i<n_args; arg_i++) {
      if (strcmp(argv[arg_i],"--angleXZ")==0) {
	if (arg_i+1>=n_args) {
	  throw runtime_error("expected float argument for --angleXZ");
	}
	char *ptr;
	the_angle=strtod(argv[++arg_i],&ptr);
	if (ptr && *ptr) {
	  throw runtime_error("expected float argument for --angleXZ");
	}
      }
      else if (strcmp(argv[arg_i],"--rotationAxisXZ")==0) {
	if (arg_i+2>=n_args) {
	  throw runtime_error("expected two float argument for --rotationAxisXZ");
	}
	char *ptr;
	rotAxis_x0=strtod(argv[++arg_i],&ptr);
	if (ptr && *ptr) {
	  throw runtime_error("first argument for --rotationAxisXZ not a float");
	}
	rotAxis_z0=strtod(argv[++arg_i],&ptr);
	if (ptr && *ptr) {
	  throw runtime_error("second argument for --rotationAxisXZ not a float");
	}
      }
      else if (strcmp(argv[arg_i],"--detector-face")==0) {
	if (arg_i+3>=n_args) {
	  throw runtime_error("expected two float argument for --detector-face");
	}
	char *ptr;
	detector_face_x0=strtod(argv[++arg_i],&ptr);
	if (ptr && *ptr) {
	  throw runtime_error("first argument for --detector-face not a float");
	}
	detector_face_y0=strtod(argv[++arg_i],&ptr);
	if (ptr && *ptr) {
	  throw runtime_error("second argument for --detector-face not a float");
	}
	detector_face_z0=strtod(argv[++arg_i],&ptr);
	if (ptr && *ptr) {
	  throw runtime_error("third argument for --detector-face not a float");
	}
      }
      else if (strcmp(argv[arg_i],"--write")==0) {
	update_conditions_db=true;
      }
      else if (strcmp(argv[arg_i],"--write-file")==0) {
	write_flat_file=true;
      }
      else if (strcmp(argv[arg_i],"--folder")==0) {
	if (arg_i+1>=n_args) {
	  throw runtime_error("expected string argument for --folder");
	}
	folder=argv[++arg_i];
      }
      else if(strcmp(argv[arg_i],"--since")==0) {
	if (arg_i+1>=n_args) throw runtime_error("expected string argument for --since");
	_since = interpretTimeStamp( argv[++arg_i] );
      }
      else if(strcmp(argv[arg_i],"--till")==0) {
	if (arg_i+1>=n_args) throw runtime_error("expected string argument for --till");
	_till = interpretTimeStamp( argv[++arg_i] );
      }
      else if (strcmp(argv[arg_i],"--verbose")==0) {
	verbose=true;
      }
      else if (strcmp(argv[arg_i],"--help")==0) {
	show_help=true;
      }
      else {
	stringstream message;
	message << "unknwon argument \"" << argv[arg_i] << "\".";
	throw runtime_error(message.str());
      }
    }
  }
  catch (exception &error) {
    print_help(argv[0]);
    cerr << "Error while parsing arguments:" << error.what() << endl;
    return -2;
  }
  if (show_help) {
    print_help(argv[0]);
    return -1;
  }

  // parse arguments
  // ________________________________________________________________________________


  try {
#ifdef USE_CONDDB
    if (folder.size()>0 && folder[folder.size()-1]=='/') {
//       folder.erase(folder.size()-1);
      cout<<"folder is incomplete (last character '/' not allowed)"<< endl;
      return -2;
    }
#endif

//   // some important CALICE dates
//   lcio::LCTime beginAug06(2006, 8, 1, 0, 0, 0);
//   lcio::LCTime endAug06(2006, 9, 8, 23, 59, 59);
//   lcio::LCTime beginOct06(2006, 10, 05, 0, 0, 0);
//   lcio::LCTime endOct06(2006, 10, 30, 16, 59, 59);
//   lcio::LCTime beginJul07(2007, 06, 29, 0, 0, 0);
//   lcio::LCTime endJul07(2007, 07, 31, 23, 59, 59);
//   lcio::LCTime farFuture(2010, 12, 31, 23, 59, 59);

//   // time validity
//   lccd::LCCDTimeStamp since = beginOct06.timeStamp();
//   lccd::LCCDTimeStamp till = farFuture.timeStamp();
//   if (since >= till) {
//     cerr << "No valid validity range in input file." << endl;
//     return -1;
//   }

    // time validity
    {
      LCTime since( _since );
      LCTime till( _till );
      cout<<"Validity times: from="<< since.getDateString() <<" to "<< till.getDateString() << endl;
    }

    if(_since >= _till) {
      cerr << "No valid time interval provided." << endl;
      print_help(argv[0]);
      return -1;
    }
 
    {
      // create LCCollection for DB
      LCCollectionVec* det_transf_col = new LCCollectionVec( LCIO::LCGENERICOBJECT );

      // Mokka counts indices from 1 to n (not 0 to n-1)
      DetectorTransformation *transf = new DetectorTransformation;
      (*transf).setDetectorAngleZX( the_angle )
	.setDetectorRotationX0(rotAxis_x0)
	.setDetectorRotationZ0(rotAxis_z0)
	.setDetectorX0(detector_face_x0)
	.setDetectorY0(detector_face_y0)
	.setDetectorZ0(detector_face_z0);

	// debugging printout
      if(verbose) {
	transf->print(cout);
	cout<< endl;
      }
	  
      det_transf_col->addElement(transf);

#ifdef USE_CONDDB
      // ------ CondDB parameters
      if (update_conditions_db && folder.empty()) {
	cerr << "***** ERROR: No folder given.  Use --help for details.\n" << endl;
	return -2;
      }

      lccd::DBInterface* dbInterface(0);
      if (update_conditions_db) {
	//cout<<"\ndbinit(""dbserver:calice:user:????:3306"")"<< endl;
	cout<<"\ndbinit(""flccaldb02.desy.de:calice:user:????:3306"")"<< endl;
	cout<<"Username: ";
	string user;  cin >> user;
	cout<<"Password: ";
	string pwd;  cin >> pwd;

	//string dbtext("dbserver:calice:"+user+":"+pwd+":3306");
	string dbtext("flccaldb02.desy.de:calice:"+user+":"+pwd+":3306");
	
	string dbinit(dbtext.c_str());
	dbInterface = new lccd::DBInterface( dbinit, folder.c_str() , true ) ;

//	cout << runStart.getDateString()  << " - " << configurationEnd.getDateString() << endl;
	string db_det_transf_description("Global detector positions and rotation parameters.");
	dbInterface->storeCollection( _since, _till, det_transf_col, db_det_transf_description);

#else
	// FIXME: write flat file
	if (write_flat_file) dbInterface->createDBFile();
#endif
      }

      //FIXME: Do I have to delete the collection
      delete det_transf_col;

    } // end of scope for LCCollectionVec

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
  }  catch (exception &err) {
    cout << "Exception:" << err.what() << endl;
    exit(-1);
  }
}
