//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:    createTcmtConnection.cc
// Package: cddata
// Purpose: store TCMT connection info into the conditions DB.
//
// 20070822 - G.Lima  created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"

#include "lccd.h"
#include "lccd/DBInterface.hh"
#ifdef USE_CONDDB
#include "ConditionsDB/CondDBException.h"
#endif

#include "TcmtConnection.hh"
#include "GLTimeUtil.hh"
#include "Exceptions.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstring>

using namespace lcio;
using namespace CALICE;
using namespace std;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

const UInt_t NLAYERS = 16;

void print_help(const char *prg_name)
{
  cout <<"\n"<< prg_name << " [--help] [--verbose] \\" << endl
       << "\t[--input-layers-file name] [--input-channels-file name] \\"<< endl
       << "\t[--since date] [--till date]" << endl
       << "\t[--folder name] [--write] [--write-file] [--dbinit]\\" << endl
       << endl
       << "--help             this text."<< endl
       << "--verbose          enable debugging output"<< endl
       << endl
       << "--since date       initial validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << "--till date        final validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << endl
       << "--input-layers-file    Monitor's file with layer <--> module mapping."<< endl
       << "--input-channels-file  Monitor's file with layer,strip <--> chip,channel"<< endl
       << "--crate #          crate number"<< endl
       << endl
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
// string tag_name;
// string comment;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  string folder;
  string input_layers_file;
  string input_channels_file;
  string dbinit = "";
  int crate = 172;

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
	if (strcmp(argv[arg_i],"--write")==0) {
	  update_conditions_db=true;
	}
	else if (strcmp(argv[arg_i],"--write-file")==0) {
	  write_flat_file=true;
	}
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
	else if (strcmp(argv[arg_i],"--dbinit")==0) 
	  {
	    if (arg_i+1>=n_args) 
	      {
		throw runtime_error("expected string argument for --dbinit");
	      }
	    dbinit=argv[++arg_i];
	  }
	else if (strcmp(argv[arg_i],"--crate")==0) {
	  char *ptr;
	  if (arg_i+1>=n_args) {
	    throw runtime_error("expected integer argument from --crate");
	  }
	  crate = (int)strtod(argv[++arg_i],&ptr);
	  if (ptr && *ptr) {
	    throw runtime_error("second argument for --crate not an int");
	  }
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
    if (input_channels_file.size()==0) {
      cerr << "ERROR: No input file specified for layer,strip <--> chip,channel mapping." << endl;
      return -2;
    }
  }

  // end of parse arguments
  // ________________________________________________________________


  try {
#ifdef USE_CONDDB
    if (folder.size()>0 && folder[folder.size()-1]=='/') {
      folder.erase(folder.size()-1);
    }
#endif
 
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

    // detector setup
    // --------------
    // read layer <--> module mapping
    ifstream layersFile( input_layers_file.c_str() );
    assert(layersFile);
    char line[256];
    layersFile.getline(line,256);  // skip header line
    int slot,fe,layer,type,module;
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

    {
      // create LCCollection for DB
      LCCollectionVec* db_lcio_col = new LCCollectionVec( LCIO::LCGENERICOBJECT );

      // read layer,strip <--> chip,channel mapping
      ifstream mappingFile( input_channels_file.c_str() );
      assert(mappingFile);
      mappingFile.getline(line,256);  // skip header line
      int strip,chip,chan;
      for(;;) {
	mappingFile >> layer >> strip >> chip >> chan;
	if(mappingFile.eof()) break;

	TcmtConnection* pConnection = new TcmtConnection();
	(*pConnection).setCrate(crate)
	  .setIndexOfLowerLeftCell(layer<<24)
	  .setModuleType(types[layer])
	  .setModuleID(modules[layer])
	  .setSlot(slots[layer])
	  .setFrontEnd(fes[layer])
	  .setChip(chip)
	  .setChannel(chan)
	  .setStripID(strip);

	// debugging printout
	if(verbose) {
	  cout<<" layer="<< layer
	      <<" strip="<< pConnection->getStripID()
	      <<" modID="<< pConnection->getModuleID()
	      <<" type="<< (int)pConnection->getModuleType()
	      <<" crate="<< pConnection->getCrate()
	      <<" slot="<< pConnection->getSlot()
	      <<" fe="<< pConnection->getFrontEnd()
	      <<" chip="<< pConnection->getChip()
	      <<" channel="<< pConnection->getChannel()
	      << endl;
	}

	db_lcio_col->addElement(pConnection);
      }

#ifdef USE_CONDDB
      if (update_conditions_db && folder.empty()) {
	std::cerr << "\n***** ERROR: No folder given.  Use --help for details.\n" << std::endl;
	return -2;
      }

      // ------ set module connection
      lccd::DBInterface* dbInterface(0);
      if (update_conditions_db) 
	{
	  dbInterface = new lccd::DBInterface( dbinit, folder.c_str() , true ) ;

	  string db_description("The positions (x/y/z),layer nr, module type (centre (normal,flipped), lower (r,l))");
	  dbInterface->storeCollection( _since, _till, db_lcio_col, db_description);

#else
	// FIXME: write flat file
	if (write_flat_file) dbInterface->createDBFile();
#endif
      }

      //FIXME: Do I have to delete the collection
      delete db_lcio_col;

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
