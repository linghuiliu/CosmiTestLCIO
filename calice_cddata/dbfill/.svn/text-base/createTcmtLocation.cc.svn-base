//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:    createTcmtLocation.cc
// Package: cddata
// Purpose: store TCMT module locations into the conditions DB.
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

#include "ModuleLocation.hh"
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
// units
Float_t mm = 1;
Float_t cm = 10*mm;
Float_t in = 25.4*mm;

void print_help(const char *prg_name)
{
  cout <<"\n"<< prg_name << " [--help] [--verbose] \\" << endl
       << "\t[--input-layers-file name] \\"<< endl
       << "\t[--since date] [--till date]" << endl
       << "\t[--folder name] [--write] [--write-file] [--dbinit]\\" << endl
       << endl
       << "--help             this text."<< endl
       << "--verbose          enable debugging output"<< endl
//     << "--tag [name]       set this tag for the conditions data." << endl
//     << "--comment [test]   add this comment to the conditions data." << endl
       << endl
       << "--since date       initial validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << "--till date        final validity timestamp (format YYYY:MM:DD:HH:mm:ss)" << endl
       << endl
       << "--input-layers-file    Monitor's file with layer <--> module mapping."<< endl
       << endl
       << "--folder name      Name of the folder where collection will be stored." << endl
       << "--write            without this option nothing is written but only printed to the screen." << endl
       << "--write-file       write conditions data stored in the database folder into a LCIO file." << endl
       << endl;
}

/* TCMT module types: 21=vertical, 22=horizontal*/
enum EModuleType {kCassetteVertical=21,kCassetteHorizontal,kNModuleTypes,kNotConnected};
    
class ModuleLocation_t 
{
public:
  ModuleLocation_t()
    : _layerNumber(-1),_cellIndexOffset(0),_type(kNotConnected) {};
  ModuleLocation_t(int layer, int cell_offset, EModuleType type)
    : _layerNumber(layer),_cellIndexOffset(cell_offset),_type(type) {};
      
  int _layerNumber;
  int _cellIndexOffset;
  EModuleType _type;
};


/***********************************************************************/
/*                                                                     */
/*                                                                     */
/*                                                                     */
/***********************************************************************/
int main(int argc, char** argv ) 
{
  /* ATTENTION:  The default units used for LCObjects are mm*/
  
  bool show_help = false;
  bool verbose   = false;
  // string tag_name;
  // string comment;
  bool update_conditions_db = false;
  bool write_flat_file      = false;
  string folder;
  string input_layers_file;
  string dbinit = "";
  
  /*default timestamp */
  struct timespec cur_time;
  long64 time_cur;
  clock_gettime(CLOCK_REALTIME, &cur_time);
  time_cur = (long64) ( (cur_time.tv_sec)*SECTONS + cur_time.tv_nsec );
  LCTime now(time_cur);
  lccd::LCCDTimeStamp _since = now.timeStamp();
  lccd::LCCDTimeStamp _till = now.timeStamp();

  /* ------------------------------------------------------------------------
     parse arguments */
  {

    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      if(n_args == 1) /* no arguments given*/
	{  
	  print_help(argv[0]);
	  return -2;
      }

      for (UInt_t arg_i = 1; arg_i < n_args; arg_i++) 
	{
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
	if (strcmp(argv[arg_i],"--write") == 0) 
	  {
	    update_conditions_db=true;
	  }
	else if (strcmp(argv[arg_i],"--write-file") == 0) 
	  {
	    write_flat_file=true;
	  }
	else if (strcmp(argv[arg_i],"--input-layers-file") == 0) 
	  {
	    if (arg_i+1 >= n_args) 
	      {
		throw runtime_error("expected string argument for --input-layers-file");
	      }
	    input_layers_file = argv[++arg_i];
	  }
	else if (strcmp(argv[arg_i],"--folder") == 0) 
	  {
	    if (arg_i+1 >= n_args) 
	      {
		throw runtime_error("expected string argument for --folder");
	      }
	    folder = argv[++arg_i];
	  }
	else if (strcmp(argv[arg_i],"--dbinit") == 0) 
	  {
	    if (arg_i+1 >= n_args) 
	      {
		throw runtime_error("expected string argument for --dbinit");
	      }
	    dbinit = argv[++arg_i];
	  }
	else if(strcmp(argv[arg_i],"--since") == 0) 
	  {
	    if (arg_i+1 >= n_args) throw runtime_error("expected string argument for --since");
	    _since = interpretTimeStamp( argv[++arg_i] );
	  }
	else if(strcmp(argv[arg_i],"--till") == 0) 
	  {
	    if (arg_i+1 >= n_args) throw runtime_error("expected string argument for --till");
	    _till = interpretTimeStamp( argv[++arg_i] );
	  }
	
	else if (strcmp(argv[arg_i],"--help") == 0) 
	  {
	    show_help=true;
	  }
	else if (strcmp(argv[arg_i],"--verbose") == 0) 
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
    catch (std::exception &error) 
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
    
    if (input_layers_file.size() == 0) 
      {
	cerr << "ERROR: No input file specified for layer <--> module mapping." << endl;
	return -2;
      }
  }
  
  /* end of parse arguments
     ________________________________________________________________*/


  try {
#ifdef USE_CONDDB
    if (folder.size()>0 && folder[folder.size()-1]=='/') {
      folder.erase(folder.size()-1);
    }
#endif

    /* some important CALICE dates*/
    lcio::LCTime beginAug06(2006, 8, 1, 0, 0, 0);
    lcio::LCTime   endAug06(2006, 9, 8,23,59,59);
    lcio::LCTime beginOct06(2006,10, 5, 0, 0, 0);
    lcio::LCTime   endOct06(2006,10,30,16,59,59);
    lcio::LCTime beginJul07(2007, 6,29, 0, 0, 0);
    lcio::LCTime   endJul07(2007, 8,31,23,59,59);
    lcio::LCTime beginApr08(2008, 4,01, 0, 0, 0);
    lcio::LCTime   endMay08(2008, 5,31,23,59,59);
    lcio::LCTime  farFuture(2010,12,31,23,59,59);

    /* time validity*/
    {
      LCTime since( _since );
      LCTime till( _till );
      cout<<"Validity times: from="<< since.getDateString() <<" to "<< till.getDateString() << endl;
    }

    if(_since >= _till) 
      {
	cerr << "No valid time interval provided." << endl;
	print_help(argv[0]);
	return -1;
      }

    /* detector setup
       --------------
       number of layers, types and common cellIDs*/
    vector<ModuleLocation_t> moduleLocation(NLAYERS+1);

    ifstream layersFile( input_layers_file.c_str() );
    assert(layersFile);
    char line[256];
    layersFile.getline(line,256);  // skip header line
    int slot,fe,layer,type,module;
    int slots[NLAYERS] = {-1};
    int fes[NLAYERS] = {-1};
    int modules[NLAYERS] = {-1};
    int types[NLAYERS] = {-999};

    for(;;) 
      {
	layersFile >> slot >> fe >> layer >> type >> module;
	if(layersFile.eof()) break;
	
	slots[layer] = slot;
	fes[layer] = fe;
	modules[layer] = module;
	types[layer] = 21+type;
	
	moduleLocation[layer]._layerNumber = layer;
	moduleLocation[layer]._cellIndexOffset = (layer << 24);
	moduleLocation[layer]._type = (EModuleType)(type + kCassetteVertical);
      }

    layersFile.close();

    /* nominal positions*/
    Float_t posL1[3] = {0,0,0};   /* first layer*/
    
    /*Angela Lucaci: In case you wonder what the hard-coded numbers below are,
      they are just the TCMT thicknesses for fine and coarse layers.
      If you want to check the numbers yourself, have a look at one of the Mokka 
      models which contains the TCMT.
     */

    Float_t deltaPosFine[3]   = {0, 0, 50.8*mm};
    Float_t deltaPosCoarse[3] = {0, 0, 133.35*mm};

    /* layer staggering*/
    Float_t xstagger[NLAYERS+1] = {0};
    Float_t ystagger[NLAYERS+1] = {0};
    if(_since >= beginJul07.timeStamp() && _till<=endJul07.timeStamp() ) 
      {
	xstagger[3] = xstagger[7] = xstagger[11] = xstagger[15] = -1*in;
	ystagger[4] = ystagger[8] = ystagger[12] = ystagger[16] = +1*in;
      }
    
    {
      // create LCCollection for DB
      LCCollectionVec* db_lcio_col = new LCCollectionVec( LCIO::LCGENERICOBJECT );

      Float_t pos[3];
      pos[0] = posL1[0];
      pos[1] = posL1[1];
      pos[2] = posL1[2];

      for (UInt_t ilayer = 1; ilayer < moduleLocation.size(); ilayer++) 
	{
	  /* update nominal positions*/
	  if(ilayer > 1 && ilayer <= 9) /* previous is a fine layer*/
	    { 
	    pos[0] += deltaPosFine[0];
	    pos[1] += deltaPosFine[1];
	    pos[2] += deltaPosFine[2];
	    }
	  else if(ilayer > 9) /* previous is a coarse layer*/
	    { 
	      pos[0] += deltaPosCoarse[0];
	      pos[1] += deltaPosCoarse[1];
	      pos[2] += deltaPosCoarse[2];
	    }
	  
	  /* skip not connected modules*/
	  if(moduleLocation[ilayer]._type >= kNModuleTypes) continue;
	  
	  
	  /* add staggering*/
	  float x = pos[0] + xstagger[ilayer];
	  float y = pos[1] + ystagger[ilayer];
	  float z = pos[2];
	  
	  /* Mokka counts indices from 1 to n (not 0 to n-1)*/
	  ModuleLocation *location = new ModuleLocation;
	  (*location).setX(x).setY(y).setZ(z);
	  location->setModuleType(moduleLocation[ilayer]._type);
	  location->setCellIndexOffset(moduleLocation[ilayer]._cellIndexOffset);
	  
	  /* debugging printout*/
	  if(verbose) 
	    {
	      cout<<" layer="<< ilayer
		  <<" type="<< (UInt_t)location->getModuleType()
		  <<" cellID_offset="<< hex<< location->getCellIndexOffset() << dec
		  <<" pos=("<< location->getX() <<"; "<< location->getY() <<"; "<< location->getZ() <<")"
		  << endl;
	    }

	  db_lcio_col->addElement(location);
	}
      
#ifdef USE_CONDDB
      if (update_conditions_db && folder.empty()) 
	{
	  std::cerr << "\n***** ERROR: No folder given.  Use --help for details.\n" << std::endl;
	  return -2;
	}
      
      /* ------ set module location*/
      lccd::DBInterface* dbInterface(0);
      if (update_conditions_db) 
	{
	  dbInterface = new lccd::DBInterface( dbinit, folder.c_str() , true ) ;
	 
	  string db_description("The positions (x/y/z),layer nr, module type (centre (normal,flipped), lower (r,l))");
	  dbInterface->storeCollection( _since, _till, db_lcio_col, db_description);
	  
#else
	  /* FIXME: write flat file*/
	  if (write_flat_file) dbInterface->createDBFile();
#endif
      }

      /*FIXME: Do I have to delete the collection*/
      delete db_lcio_col;

    } /* end of scope for LCCollectionVec*/

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
  catch (exception &err) 
    {
      cout << "Exception:" << err.what() << endl;
      exit(-1);
    }
}
