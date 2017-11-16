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
#include "MIPConstants.hh"
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
typedef map<UInt_t,MIPConstants*> CalibrationMap;

const UInt_t NLAYERS = 16;
const Float_t ADC_MIP_MIN = 40.;
const Float_t ADC_MIP_MAX = 900.;

void print_help(const char *prg_name)
{
  cout <<"\n"<< prg_name << " [--help] [--verbose] [--since date] [--till date]" << endl
       << "\t[--input-layers-file name] [--input-channels-file name] [--input-calib-file name] \\"<< endl
       << "\t[--folder name] [--write] [--dbinit]\\" << endl

       << endl
       << "--help             this text."<< endl
       << "--verbose          enable debugging output"
       << endl
       << "--since date  initial validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << "--till date   final validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << endl
       << "--input-layers-file    Monitor's file with layer <--> module mapping."<< endl
       << "--input-channels-file  Monitor's file with layer,strip <--> chip,channel"<< endl
       << "--input-calib-file     Input MIP calib file with layer,strip,mipVal,mipErr"<< endl
       << endl
       << "--folder name   Folder name where conditions are to be stored."<<endl
       << "--write         without this option, the conditions DB is not updated."<<endl
       << endl;
}


int main(int argc, char** argv) 
{
  
  bool show_help=false;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  string folder;
  string input_layers_file="";
  string input_channels_file="";
  string input_calib_file="";
  string dbinit = "";
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

      for (UInt_t arg_i=1; arg_i< n_args ; arg_i++) 
	{
	  if (strcmp(argv[arg_i],"--write") == 0) 
	    {
	      update_conditions_db=true;
	    }
	  
	  else if (strcmp(argv[arg_i],"--input-layers-file") == 0) 
	    {
	      if (arg_i + 1 >= n_args) 
		{
		  throw runtime_error("expected string argument for --input-layers-file");
		}
	      input_layers_file=argv[++arg_i];
	    }
	  else if (strcmp(argv[arg_i],"--input-channels-file")==0) 
	    {
	      if (arg_i+1>=n_args) 
		{
		  throw runtime_error("expected string argument for --input-channels-file");
		}
	      input_channels_file=argv[++arg_i];
	    }
	  else if (strcmp(argv[arg_i],"--input-calib-file")==0) 
	    {
	      if (arg_i+1>=n_args) 
		{
		  throw runtime_error("expected string argument for --input-calib-file");
		}
	      input_calib_file=argv[++arg_i];
	    }
	  else if (strcmp(argv[arg_i],"--dbinit")==0) 
	    {
	      if (arg_i+1>=n_args) 
		{
		  throw runtime_error("expected string argument for --dbinit");
		}
	      dbinit=argv[++arg_i];
	    }
	  else if (strcmp(argv[arg_i],"--folder")==0) 
	    {
	      if (arg_i+1>=n_args) {
		throw runtime_error("expected string argument for --folder");
	      }
	      folder=argv[++arg_i];
	    }
	  else if(strcmp(argv[arg_i],"--since")==0) 
	    {
	      if (arg_i+1>=n_args) throw runtime_error("expected string argument for --since");
	      _since = interpretTimeStamp( argv[++arg_i] );
	    }
	  else if(strcmp(argv[arg_i],"--till")==0) 
	    {
	      if (arg_i+1>=n_args) throw runtime_error("expected string argument for --till");
	      _till = interpretTimeStamp( argv[++arg_i] );
	    }
	  
	  else if (strcmp(argv[arg_i],"--help")==0) 
	    {
	      show_help=true;
	    }
	  else if (strcmp(argv[arg_i],"--verbose")==0) 
	    {
	      verbose=true;
	    }
	  else 
	    {
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

   if (dbinit.size()==0) {
      cerr << "ERROR: No DBINIT specified " << endl;
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

    cout<<"\n Read "<<input_layers_file.c_str()<<endl;

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

    if (verbose) cout<<"\n\n MAPPING:"<<endl;

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

      if (verbose) 
	cout<<" layer="<<layer<<" strip="<<strip<<" chip="<<chip<<" chan="<<chan
	    <<" moduleNr="<<moduleNr<<" modChipChanID="<<modChipChanID<<endl;
      connectionMap.insert(make_pair( modChipChanID, pConnection));
    }


    //=== MIP calibration stuff  - read constants from data file
    // ---------------------
    std::ifstream mipDataFile;
    mipDataFile.open(input_calib_file.c_str());
    assert(mipDataFile);
    if ( !mipDataFile.is_open()) {
      cout << "Error opening file " << input_calib_file << endl;   
      return -1;
    }

    if (verbose) cout<<"\n\n\n MIPs"<<endl;

    float mipValue, mipError=30;
    CalibrationMap calibMap;
    mipDataFile.getline(line, 256);   // skip first line
    while (true) {
//       mipDataFile >> layer >> strip >> mipValue >> mipError;
      mipDataFile >> module >> chip >> chan >> mipValue;  // if reading from monitor calib file
      if(!mipDataFile.good()) break;

      if (verbose)
	cout<<" module="<<module<<" chip="<<chip<<" chan="<<chan<<" mipValue="<<mipValue<<endl;

      if( mipValue < ADC_MIP_MIN || mipValue > ADC_MIP_MAX || mipError > 30 ) 
	{	
	  UInt_t modChipChanID = (module<<16) | (chip<<8) | (chan<<0);
	  TcmtConnection* pconn = connectionMap.find( modChipChanID ) -> second;

	  if (pconn != NULL)
	    {
	      UInt_t badstrip = pconn->getStripID();
	      UInt_t badlayer = -1;
	      
	      for(UInt_t i=0; i<NLAYERS; ++i) 
		{
		  if(modules[i]==(int)module) badlayer=i;
		}
	      cout<<"*** TCMT MIP calib problem: bad layer="<< badlayer <<" bad strip="<< badstrip
		  <<", mip="<< mipValue <<" mipErr="<< mipError<<" (dropped)"<< endl;
	      
	    }
	  else
	    {
	      cout<<"-->NULL pointer to TcmtConnection" << endl;
	    }
	}
      
      if (verbose) cout<<" \n\n step 2"<<endl;
      if (mipValue >= ADC_MIP_MIN && mipValue <= ADC_MIP_MAX) 
	{
	  // for TCMT, MIPConstants are keyed from module,strip rather than module,chip,chan
	  UInt_t modChipChanID = (module<<16) | (chip<<8) | (chan<<0);
	  TcmtConnection* pconn = connectionMap.find( modChipChanID ) -> second;
	
	  if (verbose)
	    cout<<" module="<<module<<" chip="<<chip<<" chan="<<chan
		<<" mipValue="<<mipValue
		<<" modChipChanID="<<modChipChanID
		<<endl;
	      
	  if (pconn != NULL)
	    {
	      UInt_t strip = pconn->getStripID();
	      
	      MIPConstants* mipConstant = new CALICE::MIPConstants(module, strip, mipValue, mipError);
	      if(verbose) mipConstant->print(cout);
	      UInt_t cellKey = (module << 8) + strip;
	      
	      calibMap.insert(make_pair(cellKey,mipConstant));
	    }
	  else
	    {
	      if (verbose) cout<<"NULL pointer to TcmtConnection" << endl;
	    }
	  
	  // next line useful to produce a new tcmMIP.dat file, for ASCII-based mip calibration (a la Beni)
	  // 	cout<<"debug: "<< module <<"\t"<< strip <<"\t"<< chip <<"\t"<< chan <<"\t"<< mipConstant->getFloatVal(0) << endl;
	}
    }
    mipDataFile.close();



    // debugging output
    if(verbose) {
      LCTime lcSince( _since );
      LCTime lcTill( _till );
      cout<<"\n from=<"<< lcSince.getDateString() <<" till=<"<< lcTill.getDateString() << endl;
    }

    if (verbose) cout<<"\n\n Before writing to DB"<<endl;

#ifdef USE_CONDDB
    if (update_conditions_db) 
      {
	std::string description("TCMT MIP calibration");
	CALICE::CalibrationWriter calWriter(dbinit, folder, description);
	
	for(CalibrationMap::const_iterator iter = calibMap.begin(); iter!=calibMap.end(); ++iter) 
	  {
	    // Tcmt-specific calibration format is different from Hcal:
	    //   calWriter.putCalibration(0, cellKey, mipConstant);              // for TCMT
	    //   calWriter.putCalibration(_moduleID, _chip, _chan, mipConstant); // for Hcal
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
