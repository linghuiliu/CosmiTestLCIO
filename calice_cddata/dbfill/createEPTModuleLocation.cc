//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:    createEPTDescription.cc
// Package: cddata
// Purpose: store EPT module descriptions into the conditions DB.
//
// 07Dec2012 - S.Lu  created
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

#include "lccd.h"
#include "lccd/DBInterface.hh"
#ifdef USE_CONDDB
#include "ConditionsDB/CondDBException.h"
#endif

#include <HcalCellIndex.hh>
#include "ModuleLocation.hh"
#include "Exceptions.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <cstring>

using namespace lcio;
using namespace CALICE;
using namespace std;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

void print_help(const char *prg_name)
{
  cout <<"\n"<< prg_name << " [--help] [--verbose] \\" << endl
       << "\t[--folder name] [--write] [--write-file] \\" << endl
       << endl
       << "--help             this text."<< endl
       << endl
       << "--folder name      Name of the folder where collection will be stored." << endl
       << "--write            without this option nothing is written but only printed to the screen." << endl
       << "--write-file       write conditions data stored in the database folder into a LCIO file." << endl
       << endl;
}

enum EModuleType {kModuleEPT,kNModuleTypes,kNotConnected};
    


class TilePar_t {
public:
  TilePar_t() : _posX(0), _posY(0), _column(UINT_MAX), _row(UINT_MAX), _tileSize(0) {};
  TilePar_t(Float_t pos_x, Float_t pos_y, UInt_t column_i, UInt_t row_i, UInt_t tileSize_i) : 
    _posX(pos_x),_posY(pos_y),_column(column_i),_row(row_i),_tileSize(tileSize_i) {};
  Float_t posX() const {return _posX;};
  Float_t posY() const {return _posY;};
  UInt_t column() const {return _column;};
  UInt_t row() const {return _row;};
  UInt_t tileSize() const {return _tileSize;};
  
  void setPosX(Float_t pos_x) {_posX=pos_x;};
  void setPosY(Float_t pos_y) {_posY=pos_y;};
  void setColumn(UInt_t column_i) {_column=column_i;};
  void setRow(UInt_t row_i) {_row=row_i;};
  void setTileSize(UInt_t tileSize_i) {_tileSize=tileSize_i;};
  
  Float_t _posX;
  Float_t _posY;
  UInt_t _column;
  UInt_t _row;
  UInt_t _tileSize;
};


int main(int argc, char** argv ) {

  // ATTENTION:  The default units used for LCObjects are mm

  bool show_help=false;
  bool verbose = false;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  string folder;

  // ----------------------------------------------------------------
  // parse arguments 
  {
    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
      for (UInt_t arg_i=1; arg_i< n_args ; arg_i++) {
	if (strcmp(argv[arg_i],"--write")==0) {
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
  }

  // end of parse arguments
  // ________________________________________________________________


  try {
#ifdef USE_CONDDB
    if (folder.size()>0 && folder[folder.size()-1]=='/') {
      folder.erase(folder.size()-1);
    }
#endif
    string db_module_description_description("Description of EPT modules: type, x,y pos. of cells, width and height of the modules and cells");
 

    // detector setup
    // --------------
    // some important CALICE dates
    lcio::LCTime begin(1970, 1, 1, 0, 0, 0);
    lcio::LCTime end(2014, 12, 31, 0, 0, 0);
    lcio::LCTime beginRun(2012, 1, 1, 0, 0, 0);
    lcio::LCTime endRun(2014, 1, 31, 0, 0, 0);
    lcio::LCTime farFuture(2014, 1, 31, 23, 59, 59);

    // time validity
    lccd::LCCDTimeStamp since = begin.timeStamp();
    lccd::LCCDTimeStamp till = end.timeStamp();

#ifdef USE_CONDDB
    // ------- set module description
    lccd::DBInterface* dbInterface(0);
    if (update_conditions_db) {
      //=== specify DB parameters to be used ===
      cout<<"\ndbinit(""flccaldb02.desy.de:calice:user:????:3306"")"<< endl;
      cout<<"Username: ";
      string user;  cin >> user;
      cout<<"Password: ";
      string pwd;  cin >> pwd;

      string dbtext("flccaldb02.desy.de:calice:"+user+":"+pwd+":3306");
      std::string dbinit(dbtext.c_str());
      dbInterface = new lccd::DBInterface( dbinit, folder.c_str() , true ) ;
    }
#endif

    /*****************************************/
    /*                                       */
    /*     creat ModuleLocation              */
    /*                                       */
    /*****************************************/

    LCCollectionVec* module_location_col = new LCCollectionVec( LCIO::LCGENERICOBJECT );

    string db_module_location_description("Description of EPT modules: The positions (x/y/z),layer nr, module type");

    //Lan makes specific configuration for testbeam Autumn-2014

    for (int ilayer = 1; ilayer <= 15; ilayer++){
      /* add staggering*/
      float x = 0;
      float y = 0;
      float z = (ilayer-1)*26.5;//should be increased with thickness Layer
    
      if (ilayer==13)
	z = 13*26.5;
      if (ilayer==14)
	z = 21*26.5;
      if (ilayer==15)
	z = 31*26.5;

      UInt_t module_type = 0;
      if (ilayer==1)
	module_type = 8;
      if (ilayer==2)
	module_type = 10;
      if (ilayer==3) 
	module_type = 12;
      if (ilayer>=4 && ilayer<=11)
	module_type = 4;
      if (ilayer>=12)
	module_type = 6;

      UInt_t moduleID = ilayer;

      UInt_t cellIndexOffset = ( (moduleID-1)<< 24 );
      //UInt_t cellIndexOffset = ( moduleID << 24 );
    
      /* Mokka counts indices from 1 to n (not 0 to n-1)*/
      ModuleLocation *location = new ModuleLocation;
      (*location).setX(x).setY(y).setZ(z);
      location->setModuleType( module_type );
      location->setCellIndexOffset( cellIndexOffset );
    
      /* debugging printout*/
      if(verbose) 
	{
	  cout<<" layer " << ilayer
	      <<" type="<< (UInt_t)location->getModuleType()
	      <<" cellID_offset="<< hex<< location->getCellIndexOffset() << dec
	      <<" pos=("<< location->getX() <<"; "<< location->getY() <<"; "<< location->getZ() <<")"
	      << endl;
	}
      
      module_location_col->addElement(location);

    }//end of loop ilayer

    
    if(update_conditions_db) {
#ifdef USE_CONDDB
      //For ModuleLocation
      dbInterface->storeCollection(since,till,module_location_col,db_module_location_description);
      // finally create a database file for this folder
      if (write_flat_file) {
	dbInterface->createDBFile();  
      }
    }
    delete module_location_col;
  }
#endif
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
