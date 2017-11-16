//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:    createTcmtSaturationConstants.cc
// Package: cddata
// Purpose: store TCMT saturation correction constants into the conditions DB.
//
// 20080315 - G.Lima  created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "lcio.h"
#include "UTIL/LCTime.h"
#include "Exceptions.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstring>

#include "lccd.h"
#ifdef USE_CONDDB
#include "ConditionsDB/CondDBException.h"
#endif

#include "CalibrationWriter.hh"
#include "TcmtSaturationConstants.hh"
#include "TcmtConnection.hh"

#include "time.h"
#include "GLTimeUtil.hh"

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

using namespace std;
using namespace lcio;
using namespace CALICE;

typedef map<UInt_t,TcmtConnection*> ConnectionMap;
typedef map<UInt_t,TcmtSaturationConstants*> CalibrationMap;

const UInt_t NLAYERS = 16;
const Float_t ADC_CPIX_MIN = 0.0001;
const Float_t ADC_CPIX_MAX = 0.1000;

void print_help(const char *prg_name)
{
  cout <<"\n"<< prg_name << " [--help] [--verbose] [--since date] [--till date]" << endl
       << "\t[--input-layers-file name] [--input-channels-file name] [--input-calib-file name] \\"<< endl
       << "\t[--folder name] [--write] \\" << endl
//     << "\t [--tag name] [--comment name]" << endl
       << endl
       << "--help             this text."<< endl
       << "--verbose          enable debugging output"
//     << "--tag [name]       set this tag for the conditions data." << endl
//     << "--comment [test]   add this comment to the conditions data." << endl
       << endl
       << "--since date  initial validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << "--till date   final validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << endl
       << "--input-layers-file    Monitor's file with layer <--> module mapping."<< endl
       << "--input-channels-file  Monitor's file with layer,strip <--> chip,channel"<< endl
       << "--input-calib-file     Input MIP calib file with layer,strip,CpixVal,CpixErr"<< endl
       << endl
       << "--folder name   Folder name where conditions are to be stored."<<endl
       << "--write         without this option, the conditions DB is not updated."<<endl
//        << "--write-file    write conditions data stored in the database folder into a LCIO file."<<endl
       << endl;
}


int main(int argc, char** argv) {

  bool show_help=false;
// string tag_name;
// string comment;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  string folder;
  string input_layers_file="";
  string input_channels_file="";
  string input_calib_file="";
  bool verbose = false;

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
  {

    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      if(n_args==1) {  // no arguments given
	print_help(argv[0]);
	return -2;
      }

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
// 	else if (strcmp(argv[arg_i],"--write-file")==0) {
//  	  write_flat_file=true;
// 	}
	else if (strcmp(argv[arg_i],"--input-layers-file")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --input-layers-file");
 	  }
 	  input_layers_file=argv[++arg_i];
	}
	else if (strcmp(argv[arg_i],"--input-channels-file")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --input-channels-file");
 	  }
 	  input_channels_file=argv[++arg_i];
	}
	else if (strcmp(argv[arg_i],"--input-calib-file")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --input-calib-file");
 	  }
 	  input_calib_file=argv[++arg_i];
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

	else if (strcmp(argv[arg_i],"--help")==0) {
	  show_help=true;
	}
	else if (strcmp(argv[arg_i],"--verbose")==0) {
	  verbose=true;
	}
	else {
	  stringstream message;
	  message << "unknown argument \"" << argv[arg_i] << "\".";
	  throw runtime_error(message.str());
	}
      }
    }
    catch (std::exception &error) {
      print_help(argv[0]);

      cerr << "Error while parsing arguments:" << error.what() << endl;
      return -2;
    }
    if (show_help) {
      print_help(argv[0]);
      return -1;
    }

    if (input_layers_file.size()==0) {
      cerr << "ERROR: No input file specified for layer <--> module mapping." << endl;
      return -2;
    }
    if (input_calib_file.size()==0) {
      cerr << "ERROR: No input file specified for layer,strip <--> chip,channel mapping." << endl;
      return -2;
    }
  }

  // end parse arguments
  // ________________________________________________________________________________


  try {

#ifdef USE_CONDDB
    if (update_conditions_db && folder.empty()) {
      std::cerr << "\n***** ERROR: No folder given.  Use --help for details.\n" << std::endl;
      return -2;
    }

    if (folder.size()>0 && folder[folder.size()-1]=='/') {
      folder.erase(folder.size()-1);
    }
#endif

    // time validity
    if(_since >= _till) {
      cerr << "No valid time interval provided." << endl;
      print_help(argv[0]);
      return -1;
    }

    //=== read layer <--> module mapping
    ifstream layersFile( input_layers_file.c_str() );
    assert(layersFile);
    char line[256];
    layersFile.getline(line,256);  // skip header line
    UInt_t slot,fe,layer,type,module;
    int slots[NLAYERS] = {-1};
    int fes[NLAYERS] = {-1};
    int modules[NLAYERS] = {-1};
    int types[NLAYERS] = {-999};
    for(;;) {
      layersFile >> slot >> fe >> layer >> type >> module;
      if(layersFile.eof()) break;

      slots[layer] = slot;
      fes[layer] = fe;
      modules[layer] = module;
      types[layer] = 21+type;
    }
    layersFile.close();


    //=== read layer,strip <--> chip,channel mapping
    ifstream mappingFile( input_channels_file.c_str() );
    assert(mappingFile);
    mappingFile.getline(line,256);  // skip header line
    int strip,chip,chan;
    ConnectionMap connectionMap;
    for(;;) {
      mappingFile >> layer >> strip >> chip >> chan;
      if(mappingFile.eof()) break;

      TcmtConnection* pConnection = new TcmtConnection();
      (*pConnection).setModuleID(modules[layer])
	.setChip(chip)
	.setChannel(chan)
	.setStripID(strip);

      UInt_t moduleNr = modules[layer];
      UInt_t modChipChanID = (moduleNr << 16) | (chip<<8) | (chan<<0);
      connectionMap.insert(make_pair( modChipChanID, pConnection));
    }


    //=== Saturation correction stuff  - read constants from data file
    // ---------------------
    std::ifstream cpixDataFile;
    cpixDataFile.open(input_calib_file.c_str());
    assert(cpixDataFile);
    if ( !cpixDataFile.is_open()) {
      cout << "Error opening file " << input_calib_file << endl;   
      return -1;
    }


    float cpixValue, relativeError=0.10;
    CalibrationMap calibMap;
    cpixDataFile.getline(line,256);   // skip first line
    while (true) {
      cpixDataFile >> layer >> strip >> cpixValue; // >> cpixError;
//       cpixDataFile >> module >> chip >> chan >> cpixValue;  // if reading from monitor calib file
      if(!cpixDataFile.good()) break;

      float cpixError = relativeError * cpixValue;

      if( cpixValue<ADC_CPIX_MIN || cpixValue>ADC_CPIX_MAX ) {
	// bad channel
	cout<<"*** TCMT CPIX calib problem: layer="<< layer <<" strip="<< strip
	    <<", cpix="<< cpixValue <<" cpixErr="<< cpixError <<" (dropped)"<< endl;
	continue;
      }

      // search for this strip in connection map
      UInt_t cass = modules[layer];
      chip=-1;
      chan=-1;
      TcmtConnection* pConn = NULL;
      for(ConnectionMap::const_iterator iter = connectionMap.begin(); iter!=connectionMap.end(); ++iter) {
	pConn = iter->second;
	if((UInt_t)pConn->getModuleID() != cass) continue;
	UInt_t istrip = pConn->getStripID();
	if(istrip==(UInt_t)strip) {
	  chip = pConn->getChip();
	  chan = pConn->getChannel();
	  break;
	}
      }

      // consistency check
      assert(chip!=-1);
      assert(chan!=-1);


      if (cpixValue>=ADC_CPIX_MIN && cpixValue<=ADC_CPIX_MAX) {

	TcmtSaturationConstants* cpixConstant = new CALICE::TcmtSaturationConstants(cass, strip, cpixValue, cpixError);
	if(verbose) cpixConstant->print(cout);
	UInt_t cellKey = (cass << 8) + strip;

	calibMap.insert(make_pair(cellKey,cpixConstant));

	// next line useful to produce a new tcmCPIX.dat file, for ASCII-based cpix calibration
//  	cout<<"debug: "<< module <<"\t"<< strip <<"\t"<< chip <<"\t"<< chan
// 	    <<"\t"<< cpixConstant->getFloatVal(0) <<" +/- "<< cpixConstant->getFloatVal(1) << endl;
      }
    }
    cpixDataFile.close();


    // debugging output
    if(verbose) {
      LCTime lcSince( _since );
      LCTime lcTill( _till );
      cout<<"\n from=<"<< lcSince.getDateString() <<" till=<"<< lcTill.getDateString() << endl;
    }


#ifdef USE_CONDDB
    if (update_conditions_db) {

      cout<<"\ndbinit(""dbserver:calice:user:????:3306"")"<< endl;
      cout<<"Username: ";
      string user;  cin >> user;
      cout<<"Password: ";
      string pwd;  cin >> pwd;

      string dbtext("dbserver:calice:"+user+":"+pwd+":3306");
      std::string dbinit(dbtext.c_str());
      std::string description("Cpix constants for saturation correction of TCMT hits");

      CALICE::CalibrationWriter calWriter(dbinit, folder, description);

      for(CalibrationMap::const_iterator iter = calibMap.begin(); iter!=calibMap.end(); ++iter) {
	// Tcmt-specific calibration format is different from Hcal:
	//   calWriter.putCalibration(0, cellKey, cpixConstant);              // for TCMT
	//   calWriter.putCalibration(_moduleID, _chip, _chan, cpixConstant); // for Hcal
	calWriter.putCalibration(0, iter->first, iter->second);  // tcmt
      }

      if(verbose) cout<<"\n*** calling calWriter->flushCalibration() now..."<< endl;
      calWriter.flushCalibration(_since, _till + (_till==lccd::LCCDPlusInf ? 0 : 1), write_flat_file);
    }
#endif


    // cleanup maps
    // elements are cleaned up by LCIO collection, no need to delete them here
    calibMap.clear();

    for(ConnectionMap::const_iterator iter = connectionMap.begin(); iter!=connectionMap.end(); ++iter) {
      if(iter->second) delete iter->second;
    }
    connectionMap.clear();
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
