//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:    createEPTDescription.cc
// Package: cddata
// Purpose: store EPT module descriptions into the conditions DB.
//
// 05Dec2012 - S.Lu  created
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
       << "\t[--input-file name]"<< endl
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
  std::string input_file;

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
	else if (strcmp(argv[arg_i],"--input-file")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --input-file");
 	  }
 	  input_file=argv[++arg_i];
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
    lcio::LCTime begin(2015, 8, 1, 0, 0, 0);
    //lcio::LCTime end(2015, 3, 31, 0, 0, 0);//Feb2015
    //lcio::LCTime end(2015, 4, 30, 0, 0, 0);//April2015
    lcio::LCTime end(2015, 8, 31, 23, 59, 59);//June2015
    lcio::LCTime farFuture(2019, 12, 31, 23, 59, 59);

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


    LCCollectionVec* module_description_col = new LCCollectionVec( LCIO::LCGENERICOBJECT );

    std::ifstream fin( input_file.c_str() );

    if (! fin.is_open() )
      {
	std::stringstream msg;
	msg << "Cannot open file " << input_file.c_str();
	throw std::runtime_error( msg.str() );
      }
    

    std::string line;
    int module, chip, channel, CellID1, I, J, K, CellID0;

    cout<<"\n ReadFlatFile "<<input_file.c_str()<<endl;

    // TCMT parameters
    UInt_t nCells=144;
    
    // lengths in mm
    Float_t moduleSize = 360.; //mm
    Float_t cellSizeX = 30; //mm
    Float_t cellSizeY = 30; //mm

    //SINGLE HBU LAYER
    ModuleDescription  *EPTModule1 = new ModuleDescription(4, 144, "_A",kModuleDescriptionWithCellDimensionAndZPos);
    EPTModule1->setWidth(360);
    EPTModule1->setHeight(360);

    //BIG LAYER 2*2 HBUs
    ModuleDescription  *EPTModule2 = new ModuleDescription(6, 576, "_B",kModuleDescriptionWithCellDimensionAndZPos);
    EPTModule2->setWidth(720);
    EPTModule2->setHeight(720);

    //EBU 0
    ModuleDescription  *EPTModule3 = new ModuleDescription(8, 144, "_C",kModuleDescriptionWithCellDimensionAndZPos);
    EPTModule3->setWidth(180);
    EPTModule3->setHeight(180);

    //EBU 1
    ModuleDescription  *EPTModule4 = new ModuleDescription(10, 144, "_D",kModuleDescriptionWithCellDimensionAndZPos);
    EPTModule4->setWidth(180);
    EPTModule4->setHeight(180);

    //EBU 2
    ModuleDescription  *EPTModule5 = new ModuleDescription(12, 144, "_E",kModuleDescriptionWithCellDimensionAndZPos);
    EPTModule5->setWidth(180);
    EPTModule5->setHeight(180);

    while ( fin.good() )
      {
	getline( fin, line );
	if ( line == "" || line.at(0) == '#' ) continue;
	
	std::stringstream ln( line );
	ln >> module >> chip >> channel >> CellID1 >> I >> J >> K >> CellID0;
	
	cout << "module " << module << endl;

	if (module==1){
	  moduleSize = 720.; //mm
	  cellSizeX = 30; //mm
	  cellSizeY = 30; //mm
	}
	if (module==2){
	  moduleSize = 720.; //mm
	  cellSizeX = 30; //mm
	  cellSizeY = 30; //mm
	}
	if (module==3 || module==5){
	  moduleSize = 180.; //mm
	  cellSizeX = 45; //mm
	  cellSizeY =  5; //mm
	}
	if (module==4){
	  moduleSize = 180.; //mm
	  cellSizeX =  5; //mm
	  cellSizeY = 45; //mm
	}

	// set EPT Tile positions inside module
	Double_t startOffsetX = - moduleSize/2 + cellSizeX/2;
	Double_t startOffsetY = - moduleSize/2 + cellSizeY/2;	

	UInt_t iCell = chip*36+channel;
	UInt_t  raw = I; //I:x
	UInt_t  column = J; //J:y
	UInt_t  Layer  = K;
	Float_t cellPosX = startOffsetX + (raw-1)*cellSizeX;
	Float_t cellPosY = startOffsetY + (column-1)*cellSizeY;
	Float_t cellPosZ = 1.25;

	cout << "module " << module << " moduleSize " << moduleSize 
	     << " cellSizeX " << cellSizeX << " cellSizeY " << cellSizeY 
	     << " startOffsetX " << startOffsetX << " startOffsetY " << startOffsetY
	     << " I " << I << " cellPosX " << cellPosX
	     << " J " << J << " cellPosY " << cellPosY << endl;
	
	if (module==1){
	  EPTModule1->setCellZPos(iCell, cellPosZ);

	  EPTModule1->setCellPos(iCell, cellPosX, cellPosY);
	  EPTModule1->setIndividualCellWidth(iCell, cellSizeX);
	  EPTModule1->setIndividualCellHeight(iCell, cellSizeY);

	  UInt_t EPTCellID  =  (HcalCellIndex(column, raw, Layer).getCellIndex());
	  EPTModule1 ->setGeometricalCellIndex(iCell, EPTCellID);
	}
	if (module==2){
	  EPTModule2->setCellZPos(iCell, cellPosZ);

	  EPTModule2->setCellPos(iCell, cellPosX, cellPosY);
	  EPTModule2->setIndividualCellWidth(iCell, cellSizeX);
	  EPTModule2->setIndividualCellHeight(iCell, cellSizeY);

	  UInt_t EPTCellID  =  (HcalCellIndex(column, raw, Layer).getCellIndex());
	  EPTModule2 ->setGeometricalCellIndex(iCell, EPTCellID);
	}
	if (module==3){
	  EPTModule3->setCellZPos(iCell, cellPosZ);

	  EPTModule3->setCellPos(iCell, cellPosX, cellPosY);
	  EPTModule3->setIndividualCellWidth(iCell, cellSizeX);
	  EPTModule3->setIndividualCellHeight(iCell, cellSizeY);

	  UInt_t EPTCellID  =  (HcalCellIndex(column, raw, Layer).getCellIndex());
	  EPTModule3 ->setGeometricalCellIndex(iCell, EPTCellID);
	}
	if (module==4){
	  EPTModule4->setCellZPos(iCell, cellPosZ);

	  EPTModule4->setCellPos(iCell, cellPosX, cellPosY);
	  EPTModule4->setIndividualCellWidth(iCell, cellSizeX);
	  EPTModule4->setIndividualCellHeight(iCell, cellSizeY);

	  UInt_t EPTCellID  =  (HcalCellIndex(column, raw, Layer).getCellIndex());
	  EPTModule4 ->setGeometricalCellIndex(iCell, EPTCellID);
	}
	if (module==5){
	  EPTModule5->setCellZPos(iCell, cellPosZ);

	  EPTModule5->setCellPos(iCell, cellPosX, cellPosY);
	  EPTModule5->setIndividualCellWidth(iCell, cellSizeX);
	  EPTModule5->setIndividualCellHeight(iCell, cellSizeY);

	  UInt_t EPTCellID  =  (HcalCellIndex(column, raw, Layer).getCellIndex());
	  EPTModule5 ->setGeometricalCellIndex(iCell, EPTCellID);
	}
      }    

    if(verbose) {
      cout << "module type=" << (int)EPTModule1->getModuleType()
	   <<" ("<< EPTModule1->getModuleTypeName() <<")"
	   << " n_floats=" << EPTModule1->getNFloat()
	   << " n_ints=" << EPTModule1->getNInt() 
	   << " n_double=" << EPTModule1->getNDouble() 
	   << " n_cells=" << EPTModule1->getNCells()
	   << endl;

      cout << "module type=" << (int)EPTModule2->getModuleType()
	   <<" ("<< EPTModule2->getModuleTypeName() <<")"
	   << " n_floats=" << EPTModule2->getNFloat()
	   << " n_ints=" << EPTModule2->getNInt() 
	   << " n_double=" << EPTModule2->getNDouble() 
	   << " n_cells=" << EPTModule2->getNCells()
	   << endl;

      cout << "module type=" << (int)EPTModule3->getModuleType()
	   <<" ("<< EPTModule3->getModuleTypeName() <<")"
	   << " n_floats=" << EPTModule3->getNFloat()
	   << " n_ints=" << EPTModule3->getNInt() 
	   << " n_double=" << EPTModule3->getNDouble() 
	   << " n_cells=" << EPTModule3->getNCells()
	   << endl;

      cout << "module type=" << (int)EPTModule4->getModuleType()
	   <<" ("<< EPTModule4->getModuleTypeName() <<")"
	   << " n_floats=" << EPTModule4->getNFloat()
	   << " n_ints=" << EPTModule4->getNInt() 
	   << " n_double=" << EPTModule4->getNDouble() 
	   << " n_cells=" << EPTModule4->getNCells()
	   << endl;

      cout << "module type=" << (int)EPTModule5->getModuleType()
	   <<" ("<< EPTModule5->getModuleTypeName() <<")"
	   << " n_floats=" << EPTModule5->getNFloat()
	   << " n_ints=" << EPTModule5->getNInt() 
	   << " n_double=" << EPTModule5->getNDouble() 
	   << " n_cells=" << EPTModule5->getNCells()
	   << endl;

    }
      
    module_description_col->addElement(EPTModule1);
    module_description_col->addElement(EPTModule2);
    module_description_col->addElement(EPTModule3);
    module_description_col->addElement(EPTModule4);
    module_description_col->addElement(EPTModule5);

    if(update_conditions_db) {
#ifdef USE_CONDDB
      dbInterface->storeCollection(since,till,module_description_col,db_module_description_description);
      // finally create a database file for this folder
      if (write_flat_file) {
	dbInterface->createDBFile();  
      }
    }
#endif
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
