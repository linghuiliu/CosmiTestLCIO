//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:    createTcmtDescription.cc
// Package: cddata
// Purpose: store TCMT module descriptions into the conditions DB.
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

#include "lccd.h"
#include "lccd/DBInterface.hh"
#ifdef USE_CONDDB
#include "ConditionsDB/CondDBException.h"
#endif

#include "ModuleDescription.hh"
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

enum EModuleType {kCassetteVertical,kCassetteHorizontal,kNModuleTypes,kNotConnected};
    

class StripPar_t {
public:
  StripPar_t() : _column(UINT_MAX), _row(UINT_MAX) {}
  StripPar_t(Float_t pos_x, Float_t pos_y, UInt_t column_i, UInt_t row_i) : _posX(pos_x),_posY(pos_y),_column(column_i),_row(row_i) {};
  Float_t posX()  const {return _posX;};
  Float_t posY()  const {return _posY;};
  UInt_t column() const {return _column;};
  UInt_t row()    const {return _row;};

  void setPosX(Float_t pos_x) {_posX=pos_x;};
  void setPosY(Float_t pos_y) {_posY=pos_y;};
  void setColumn(UInt_t column_i) {_column=column_i;};
  void setRow(UInt_t row_i) {_row=row_i;};

  Float_t _posX;
  Float_t _posY;
  UInt_t _column;
  UInt_t _row;
};

int main(int argc, char** argv ) {

  // ATTENTION:  The default units used for LCObjects are mm

  bool show_help=false;
  bool verbose = false;
// string tag_name;
// string comment;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  string folder;

  // ----------------------------------------------------------------
  // parse arguments 
  {
    try {
      const UInt_t n_args=static_cast<UInt_t>(argc);
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
    string db_module_description_description("Description of TCMT modules: type, x,y pos. of cells, width and height of the modules and cells");
 

    // detector setup
    // --------------
    // some important CALICE dates
    lcio::LCTime beginAug06(2006, 8, 1, 0, 0, 0);
    lcio::LCTime endAug06(2006, 9, 8, 23, 59, 59);
    lcio::LCTime beginOct06(2006, 10, 05, 0, 0, 0);
    lcio::LCTime endOct06(2006, 10, 30, 16, 59, 59);
    lcio::LCTime beginJul07(2007, 06, 29, 0, 0, 0);
    lcio::LCTime endJul07(2007, 07, 31, 23, 59, 59);
    lcio::LCTime farFuture(2010, 12, 31, 23, 59, 59);

    // time validity
    lccd::LCCDTimeStamp since = beginAug06.timeStamp();
    lccd::LCCDTimeStamp till = farFuture.timeStamp();


    // TCMT parameters
    UInt_t nStrips=20;
    vector<StripPar_t> strip_pos(nStrips);

    // lengths in mm
    Float_t moduleSize = 1000.;
    Float_t stripWidth = 50;
    Float_t stripLength = 1000;

#ifdef USE_CONDDB
    // ------- set module description
    lccd::DBInterface* dbInterface(0);
    if (update_conditions_db) {
      //=== specify DB parameters to be used ===
      cout<<"\ndbinit(""dbserver:calice:user:????:3306"")"<< endl;
      cout<<"Username: ";
      string user;  cin >> user;
      cout<<"Password: ";
      string pwd;  cin >> pwd;

      string dbtext("dbserver:calice:"+user+":"+pwd+":3306");
      std::string dbinit(dbtext.c_str());
      dbInterface = new lccd::DBInterface( dbinit, folder.c_str() , true ) ;
    }
#endif


    LCCollectionVec* module_description_col = new LCCollectionVec( LCIO::LCGENERICOBJECT );

    // types: 21=vertical, 22=horizontal
    ModuleDescription  *vertModule = new ModuleDescription(21, nStrips, "_V");
    vertModule->setWidth(moduleSize);
    vertModule->setHeight(moduleSize);
    vertModule->setCellWidth(stripWidth);
    vertModule->setCellHeight(stripLength);

    ModuleDescription  *horizModule = new ModuleDescription(22, nStrips, "_H");
    horizModule->setWidth(moduleSize);
    horizModule->setHeight(moduleSize);
    horizModule->setCellWidth(stripWidth);
    horizModule->setCellHeight(stripLength);

    // set strip positions inside module
    Double_t startOffset = moduleSize/2 - stripWidth/2;
    for(UInt_t istrip=0; istrip<nStrips; ++istrip) {
      Double_t stripPos = startOffset - istrip*stripWidth;

      vertModule->setCellPos(istrip, -stripPos, 0.0);
      horizModule->setCellPos(istrip, 0.0, -stripPos);

//       UInt_t vertCellID  = (0x1ff <<15) | (istrip<<6);
//       UInt_t horizCellID = (istrip<<15) | (0x1ff <<6);
      UInt_t vertCellID  = (istrip+1)<<6;
      UInt_t horizCellID = (istrip+1)<<15;
      vertModule ->setGeometricalCellIndex(istrip, vertCellID);
      horizModule->setGeometricalCellIndex(istrip, horizCellID);

      if(verbose) {
	cout<<"strip "<< istrip <<" pos="<< stripPos
	    <<" IDoffset(v,h)="<< hex << vertCellID <<" "<< horizCellID
	    << dec << endl;
      }
    }
      
    if(verbose) {
      cout << "module type=" << (int)vertModule->getModuleType()
	   <<" ("<< vertModule->getModuleTypeName() <<")"
	   << " n_floats=" << vertModule->getNFloat()
	   << " n_ints=" << vertModule->getNInt() 
	   << " n_double=" << vertModule->getNDouble() 
	   << " n_cells=" << vertModule->getNCells()
	   << endl;
      
      cout << "module type=" << (int)horizModule->getModuleType()
	   <<" ("<< horizModule->getModuleTypeName() <<")"
	   << " n_floats=" << horizModule->getNFloat()
	   << " n_ints=" << horizModule->getNInt() 
	   << " n_double=" << horizModule->getNDouble() 
	   << " n_cells=" << horizModule->getNCells()
	   << endl;
    }
      
    module_description_col->addElement(vertModule);
    module_description_col->addElement(horizModule);

    if(update_conditions_db) {
#ifdef USE_CONDDB
      dbInterface->storeCollection(since,till,module_description_col,db_module_description_description);
      // finally create a database file for this folder
      //      if (tag_name.empty()) {
      if (write_flat_file) {
	dbInterface->createDBFile();  
      }
      //       }
      //       else {
      // 	dbInterface->tagFolder(tag_name,comment);

      // 	if (write_flat_file) {
      // 	  dbInterface->createDBFile(tag_name);  
      // 	}
      //       }
#else
      //FIXME: write flat file
#endif
    }
    //FIXME: Do I have to delete the collection?
    delete module_description_col;
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
