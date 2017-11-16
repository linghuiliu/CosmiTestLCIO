#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include "lcio.h"

//#include "IO/LCWriter.h"
#include <EVENT/LCIO.h>

#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <UTIL/LCTime.h>


#define VISUALISE

#if (defined(HAVE_ROOT) && defined(VISUALISE))
#include <TROOT.h>
#include <TList.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TBox.h>
#include <TLatex.h>
#endif

#ifdef USE_CONDDB
#include <ConditionsDB/CondDBException.h>
#endif
#include <lccd.h>
#include <lccd/DBInterface.hh>
#include <lccd/ConditionsMap.hh>

#include <ModuleLocation.hh>
#include <HcalCellIndex.hh>

#include <ReadDaqMap/parseDateTime.hh>
#include <ReadDaqMap/ReadLine.hh>
#include <ReadDaqMap/TLineIterator.hh>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <Exceptions.h>
#include <vector>

#ifdef __APPLE__
#include <float.h>
#else
#include <values.h>
#endif

#define HCALMODULETYPEOFFSET 4

using namespace lcio ;
using namespace CALICE;
using namespace std;

typedef unsigned int UInt_t;
typedef int Int_t;
typedef float Float_t;

void print_help(const char *prg_name)
{
  std::cout << prg_name << " [--help] [--top-folder name] [--write] [--write-file] \\" << std::endl
	    << "\t[--input-file name]"
	    << std::endl
	    << "--help             this text."<< std::endl
	    << std::endl
	    << "--input-file name    The name of the file which contains the layer and shift values of connected locations."
	    << std::endl
            << "--top-folder name  Name to be given to the top folder." << std::endl
	    << "--write            without this option nothing is written but only printed to the screen." << std::endl
	    << "--write-file       write conditions data stored in the database folder into a LCIO file." << std::endl
	    << std::endl
	    << "--description      Create descriptions of the modules." << std::endl
	    << "--locations        Create the module location conditions data (requires --input-file)." << std::endl
#if (defined(HAVE_ROOT) && defined(VISUALISE))
	    << std::endl
            << "--print-name name  Save the module graphics to the specified file."
#endif  
	    << std::endl;
}

    enum EModuleType {kModuleA,kModuleB,kModuleC,kModuleD,kNModuleTypes,kNotConnected};
    
    class ModuleLocation_t {
    public:
      ModuleLocation_t() : _layerNumber(-1),_cellIndexOffset(0),_type(kNotConnected) {};
      ModuleLocation_t(int layer, int cell_offset, EModuleType type) : _layerNumber(layer),_cellIndexOffset(cell_offset),_type(type) {};
      
      int _layerNumber;
      int _cellIndexOffset;
      EModuleType _type;
    };

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

int main(int argc, char** argv ){

  // ATTENTION: Internal length units are cm length. The Units used for LCObjects are mm
  const Float_t length_unit=1e-2/1e-3; // comversion from cm to mm

  bool show_help=false;
  bool update_conditions_db=false;
  bool write_flat_file=false;
  std::string top_folder;
  std::string input_file;

  bool create_description=false;
  bool create_location=false;

#if (defined(HAVE_ROOT) && defined(VISUALISE))
  std::string print_name;
#endif

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
	else if (strcmp(argv[arg_i],"--description")==0) {
	  create_description=true;
	}
	else if (strcmp(argv[arg_i],"--locations")==0) {
	  create_location=true;
	}
	else if (strcmp(argv[arg_i],"--top-folder")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --top-folder");
 	  }
 	  top_folder=argv[++arg_i];
	}
	else if (strcmp(argv[arg_i],"--help")==0) {
	  show_help=true;
	}
#if (defined(HAVE_ROOT) && defined(VISUALISE))
	else if (strcmp(argv[arg_i],"--print-name")==0) {
 	  if (arg_i+1>=n_args) {
 	    throw runtime_error("expected string argument for --print-name");
 	  }
 	  print_name=argv[++arg_i];
	}
#endif
	else {
	  std::stringstream message;
	  message << "unknwon argument \"" << argv[arg_i] << "\".";
	  throw runtime_error(message.str());
	}
      }
    }
    catch (std::exception &error) {
      print_help(argv[0]);    
      std::cerr << "Error while parsing arguments:" << error.what() << std::endl;
      return -2;
    }
    if (show_help) {
      print_help(argv[0]);
      return -1;
    }
    if (create_location && input_file.size()==0) {
      std::cerr << "ERROR: No input file specified which describes the module locations:" << std::endl;
      return -2;
    }
    if (!create_location && !create_description) {
      std::cerr << "ERROR: You have to specify at least one of: --description or --locations." << std::endl;
      return -2;
    }
  }


  try {
#ifdef USE_CONDDB
    if (top_folder.size()>0 && top_folder[top_folder.size()-1]=='/') {
      top_folder.erase(top_folder.size()-1);
    }
    std::stringstream descr_folder_name;
    descr_folder_name << top_folder << "/Hcal/HcalModuleDescription";
#endif
    std::string db_module_description_description("Description of modules: type, x,y pos. of cells, width and height of the modules and cells");

    std::vector<ModuleLocation_t> moduleLocation;
    for (UInt_t layer_i=1; layer_i<=30; layer_i++) {
      moduleLocation.push_back(ModuleLocation_t(layer_i,HcalCellIndex(0,0,layer_i+2).getCellIndex(),kModuleA));
      moduleLocation.push_back(ModuleLocation_t(layer_i,HcalCellIndex(0,0,layer_i+2).getCellIndex(),kModuleB));
    }
    for (UInt_t layer_i=31; layer_i<=38; layer_i++) {
      moduleLocation.push_back(ModuleLocation_t(layer_i,HcalCellIndex(0,0,layer_i+2).getCellIndex(),kModuleC));
      moduleLocation.push_back(ModuleLocation_t(layer_i,HcalCellIndex(0,0,layer_i+2).getCellIndex(),kModuleD));
    }
    std::vector<Double_t> moduleShift;
    std::vector<bool> moduleConnected;
    moduleShift.resize(moduleLocation.size(),0.);
    moduleConnected.resize(moduleLocation.size(),false);

    Bool_t new_line;
    UTIL::LCTime _since;
    UTIL::LCTime _till;
    Double_t _rotationAngle(0.);
    if (create_location) {
      ReadLine in(200,input_file.c_str());
      while ((new_line=in.ReadNextLine())) {
	TLineIterator line_iter(in.GetBuffer());
	if (line_iter.IsEmpty()) continue;    //skip empty lines
	if (line_iter.IsComment()) continue;  //skip header

	if (line_iter.IsAlpha()) {
	  std::string first_word=line_iter.GetWord(true);
	  if (first_word=="validity:") { 
	    _since=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
	    _till=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
	  }
	  if (first_word=="rotation:") {
	    _rotationAngle=line_iter.GetDouble();
	  }
	  continue;  //skip header
	}
      
	UInt_t layer_i=line_iter.GetUnsignedInteger(); 
	UInt_t lower_or_center=line_iter.GetUnsignedInteger(); 
	Double_t shift=line_iter.GetDouble();
	UInt_t connected=line_iter.GetUnsignedInteger();
	if ((layer_i<=0) || (layer_i>=39) || (lower_or_center>1) || ((layer_i-1)*2+lower_or_center>=moduleLocation.size())) {
	  std::cerr << "Error in line " << in.GetLineNumber() << ". Invalid layer(1-38) or lower=0/central=1 flag." << std::endl;
	  return -1;
	}
	if (connected>0) {
	  moduleConnected[(layer_i-1)*2+lower_or_center]=true;
	  moduleShift[(layer_i-1)*2+lower_or_center]=shift;
	}
      }
      UInt_t location_i=0;
      for(std::vector<bool>::const_iterator location_iter=moduleConnected.begin();
	  location_iter!=moduleConnected.end();
	  location_iter++,location_i++) {
	if (!*location_iter) {
	  moduleLocation[location_i]._type=kNotConnected;
	}
      }

      if (_since.timeStamp()>=_till.timeStamp()) {
	std::cerr << "No valid validity range in input file." << std::endl;
	return -1;
      }
    }

    // calculate tile positions 
    // and channel
    vector<TilePar_t> tile_pos;
    tile_pos.resize(108*4);
    for (unsigned j=0; j<108*4; j++) {
      tile_pos[j]=TilePar_t();
    }
    vector<int> module_first;
    module_first.resize(4);
    vector<int> module_last;
    module_last.resize(4);

    Float_t module_width=30.*3.; //cm
    Float_t module_height=30.*3./2.;//cm
    Float_t module_ensemble_shift_x=-module_width/2;
    Float_t module_ensemble_shift_y=-module_height;

    Float_t layer_sep=3.173; //cm
    Float_t first_layer_z=0; //cm;
    // FIXME: currently there is no possibility to specify different cell sizes per module.
    Float_t cell_width=3; //cm
    Float_t cell_height=3; //cm

    Bool_t swap_x = true;
    Bool_t swap_y = false;

#if (defined(HAVE_ROOT) && defined(VISUALISE))
    UInt_t n_cols=30;
    UInt_t n_rows=30;

    const char *canvas_name="HcalCasette";
    TCanvas *canvas=(TCanvas *) gROOT->GetListOfCanvases()->FindObject(canvas_name);
    if (!canvas) {
      canvas=new TCanvas(canvas_name,canvas_name);
    }
    canvas->Clear();
    
    UInt_t color[4]={5,7,5,3};
    Float_t tile_size=3; //cm;
    Float_t cell_size=.98/n_cols;
    Float_t the_cell_size=cell_size;
    for (UInt_t col_i=0; col_i<n_cols; col_i++) {
      Float_t cent_x=(col_i+.5)*.98/n_cols+.01;
      {
	TLine *line=new TLine(cent_x+0.5*cell_size,0,cent_x+0.5*cell_size,1.);
	line->SetLineColor(color[col_i%4]);
	if (col_i==3 || col_i==25) {
	  line->SetLineWidth(3);
	}
	else if (col_i!=9 && col_i!=19) {
	  line->SetLineStyle(2);
	}
	else {
	  line->SetLineWidth(2);
	}
	line->Draw();
	TLatex *latex=new TLatex(cent_x,cell_size*.5,Form("%i",col_i));
	latex->SetTextAlign(22);
	latex->SetTextSize(cell_size*.5);
	latex->SetNDC(kTRUE);
	latex->SetTextColor(3);
	latex->Draw();
      }
      if (col_i<3 && (col_i!=0)) continue;
      if (col_i>=30-3 && col_i!=29) continue;
      for (UInt_t row_i=0; row_i<n_rows; row_i++) {
	Float_t cent_y=1.-(((n_rows-row_i)+.5)*.98/n_rows+.01);
	if (col_i==n_cols/2-1) {
	  TLatex *latex=new TLatex(cell_size*.5,cent_y, Form("%i",row_i));
	  latex->SetTextAlign(22);
	  latex->SetTextSize(cell_size*.5);
	  latex->SetNDC(kTRUE);
	  latex->SetTextColor(3);
	  latex->Draw();
	  
	  TLine *line=new TLine(0,cent_y-0.5*cell_size,1,cent_y-0.5*cell_size);
	  line->SetLineColor(color[row_i%4]);
	  if (row_i==3 || row_i==25) {
	    line->SetLineWidth(3);
	  }
	  else if (row_i!=9 && row_i!=19) {
	    line->SetLineStyle(2);
	  }
	  else {
	    line->SetLineWidth(2);
	  }
	  line->Draw();
	}
      }
   }  
#endif
	
    ifstream _finePositionMap;
    _finePositionMap.open("HCAL_pos_map_fine.dat");
    if (!_finePositionMap.good()) throw runtime_error("File HCAL_pos_map_fine.dat not found.");
    for (unsigned i=0; i<216; i++) {
      unsigned _chip, _channel, _rowX, _rowY, _tileSize;
      _finePositionMap >> _chip >> _channel >> _rowX >> _rowY >> _tileSize;
      if (swap_x) _rowX = 90 - _rowX - _tileSize + 2;
      if (swap_y) _rowY = 90 - _rowY - _tileSize + 2;
      float _tileX = _rowX - 1 + _tileSize/2.;
      float _tileY = _rowY - 1 + _tileSize/2.;
      tile_pos[_chip*18+_channel]=TilePar_t(_tileX, _tileY, _rowX, _rowY, _tileSize);
#if (defined(HAVE_ROOT) && defined(VISUALISE))
      TBox *box=new TBox((_rowX)*0.98/(n_rows*cell_width)+0.005,
                         (_rowY)*0.98/(n_cols*cell_height)+0.005,
			 (_rowX+_tileSize)*0.98/(n_rows*cell_width)-0.005,
			 (_rowY+_tileSize)*0.98/(n_cols*cell_height)-0.005);
      box->SetFillStyle(1001);
      box->SetFillColor(color[(_rowX/int(_tileSize/cell_width)+_rowY/int(_tileSize/cell_height))%4]);
      box->Draw();
      TLatex *latex=new TLatex((_rowX+_tileSize*0.5)*0.98/(n_rows*cell_width),
                               (_rowY+_tileSize*0.5)*0.98/(n_cols*cell_width),
			       Form("%i",_chip*18+_channel));
      latex->SetTextSize(cell_size*.7);
      latex->SetNDC(kTRUE);
      latex->SetTextAlign(22);
      latex->Draw();
#endif
    }
    _finePositionMap.close();
    module_first[kModuleA]=0;
    module_last[kModuleA]=107;
    module_first[kModuleB]=108;
    module_last[kModuleB]=215;

    ifstream _coarsePositionMap;
    _coarsePositionMap.open("HCAL_pos_map_coarse.dat");
    if (!_coarsePositionMap.good()) throw runtime_error("File HCAL_pos_map_coarse.dat not found.");
    for (unsigned i=0; i<141; i++) {
      unsigned _chip, _channel, _rowX, _rowY, _tileSize;
      _coarsePositionMap >> _chip >> _channel >> _rowX >> _rowY >> _tileSize;
      if (swap_x) _rowX = 90 - _rowX - _tileSize + 2;
      if (swap_y) _rowY = 90 - _rowY - _tileSize + 2;
      float _tileX = _rowX - 1 + _tileSize/2.;
      float _tileY = _rowY - 1 + _tileSize/2.;
      tile_pos[_chip*18+_channel+216]=TilePar_t(_tileX, _tileY, _rowX, _rowY, _tileSize);
    }
    _coarsePositionMap.close();
    module_first[kModuleC]=216;
    module_last[kModuleC]=323;
    module_first[kModuleD]=324;
    module_last[kModuleD]=431;
 
#if (defined(HAVE_ROOT) && defined(VISUALISE))
    if (canvas && print_name.size()>0) {
      canvas->Print(print_name.c_str());
    }
#endif

    for (UInt_t channel_i=0; channel_i<tile_pos.size(); channel_i++) {
      if ((tile_pos[channel_i].row()!=UINT_MAX) && (tile_pos[channel_i].column()!=UINT_MAX))
        std::cout << channel_i << ": "  
	   	  << std::setw(14) << tile_pos[channel_i].posX() << ", "
		  << std::setw(14) << tile_pos[channel_i].posY()
		  << std::setw(10) << tile_pos[channel_i].column() << ", "
		  << std::setw(10) << tile_pos[channel_i].row() << ", "
		  << std::endl;
      else 
        std::cout << channel_i << ": not used" << std::endl;		   	  
    }
    
    Float_t module_pos_x[kNModuleTypes];
    Float_t module_pos_y[kNModuleTypes];
    for (UInt_t _moduleType = kModuleA; _moduleType < kNModuleTypes; _moduleType++) {
      module_pos_x[_moduleType]=0;
      if ((_moduleType==kModuleA)||(_moduleType==kModuleC))  module_pos_y[_moduleType] = module_height; else module_pos_y[_moduleType] = 0;
      for (UInt_t tile_i=module_first[_moduleType];tile_i<module_last[_moduleType]+1;tile_i++) {
        tile_pos[tile_i].setPosX(tile_pos[tile_i].posX()-module_pos_x[_moduleType]);
        tile_pos[tile_i].setPosY(tile_pos[tile_i].posY()-module_pos_y[_moduleType]);
      }
    }

    // ------- set module description
    if (create_description) {
#ifdef USE_CONDDB
      lccd::DBInterface* db_module_descriptions;
      if (update_conditions_db) {
        db_module_descriptions = new lccd::DBInterface( lccd::getDBInitString(), descr_folder_name.str() , true ) ;
      }
#endif

      LCCollectionVec* module_description_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;
 
      const char *module_type_name[kNModuleTypes]={"_A","_B","_C","_D"};
    
      for (UInt_t module_i=0; module_i<kNModuleTypes; module_i++) {
        ModuleDescription *a_module=new ModuleDescription(module_i+HCALMODULETYPEOFFSET,
	  						  module_last[module_i]-module_first[module_i]+1,
							  module_type_name[module_i],
							  kModuleDescriptionWithCellDimension);
        (*a_module)
	  .setWidth(module_width*length_unit)
	  .setHeight(module_height*length_unit);
        for (UInt_t cell_i=0; cell_i<module_last[module_i]-module_first[module_i]+1; cell_i++) {
	  UInt_t tile_i=cell_i+module_first[module_i];
	  if ((tile_pos[tile_i].row()==UINT_MAX)||(tile_pos[tile_i].column()==UINT_MAX)) {
	    std::cout << tile_i << ": not connected" << std::endl;
	    continue;
	  }
	  a_module->setCellPos(cell_i, tile_pos[tile_i].posX()*length_unit, tile_pos[tile_i].posY()*length_unit);
	  /*
	  a_module->setGeometricalCellIndex(cell_i,
	     HcalCellIndex(tile_pos[tile_i].row()-module_i, tile_pos[tile_i].column()-module_i, 1).getCellIndex());
	  */
	  a_module->setGeometricalCellIndex(cell_i,
	     HcalCellIndex(tile_pos[tile_i].row(), tile_pos[tile_i].column(), 0).getCellIndex());
	  std::cout << tile_i << ": " << tile_pos[tile_i].posX() << "/ " << tile_pos[tile_i].posY()
	                      << ", " << tile_pos[tile_i].column() << "/ " << tile_pos[tile_i].row() << std::endl;
          a_module->setIndividualCellWidth(cell_i, tile_pos[tile_i].tileSize()*length_unit);			      
          a_module->setIndividualCellHeight(cell_i, tile_pos[tile_i].tileSize()*length_unit);			      
        }
        std::cout << "module " << module_i << " n_floats=" << a_module->getNFloat() << " n_ints=" << a_module->getNInt() 
	   	  << " n_double=" << a_module->getNDouble() 
		  << " n_cells=" << a_module->getNCells()
		  << std::endl;
        module_description_col->addElement(a_module);
      }
    
#ifdef USE_CONDDB
      if (update_conditions_db) {
        //Year,month,dat,hour,min,sec
        LCTime firstModule(2004, 1, 2, 8, 0, 0);
        LCTime farFuture(2015, 3, 1, 4 ,1 ,5);

        db_module_descriptions->storeCollection(firstModule.timeStamp(),farFuture.timeStamp(),module_description_col,db_module_description_description);
        if (write_flat_file) {
	  db_module_descriptions->createDBFile();  
        }
#endif
    }
    delete module_description_col;
    }

    
    // ------ set module location
    if (create_location) {
#ifdef USE_CONDDB
      std::stringstream location_folder_name;
      location_folder_name << top_folder << "/Hcal/HcalModuleLocation";
      lccd::DBInterface* db_module_positions;
      if (update_conditions_db) {
        db_module_positions = new lccd::DBInterface( lccd::getDBInitString(), location_folder_name.str() , true ) ;
      }
#endif
      
      std::string db_module_position_description("The positions (x/y/z),layer nr, module type (upper/lower, fine/coarse, flipped).");
      LCCollectionVec* module_position_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;
	
      for (UInt_t module_i=0; module_i<moduleLocation.size(); module_i++) {
	  
	if (moduleLocation[module_i]._type>=kNModuleTypes) continue;
	  
	float pos_x = module_ensemble_shift_x + module_pos_x[moduleLocation[module_i]._type];
	float pos_y = module_ensemble_shift_y + module_pos_y[moduleLocation[module_i]._type];	  
        float pos_z = moduleLocation[module_i]._layerNumber * layer_sep + first_layer_z;
	pos_x -= std::atan(_rotationAngle)*(39*layer_sep+first_layer_z-pos_z);
	  
	ModuleLocation *a_module_position = new ModuleLocation;
	(*a_module_position)
	  .setX(pos_x*length_unit)
	  .setY(pos_y*length_unit)
	  .setZ(pos_z*length_unit)
	  .setModuleType(moduleLocation[module_i]._type+HCALMODULETYPEOFFSET)
	  /*	  .setCellIndexOffset(moduleLocation[module_i]._cellIndexOffset + 
	    HcalCellIndex(moduleLocation[module_i]._type,moduleLocation[module_i]._type,1).getCellIndex());*/
	  .setCellIndexOffset(moduleLocation[module_i]._cellIndexOffset); 
	// a_module_position->print(std::cout);
	std::cout << "  internal module index: " << module_i
		  << "  index offset: " << moduleLocation[module_i]._cellIndexOffset
		  << std::endl;
	module_position_col->addElement(a_module_position);
      }

#ifdef USE_CONDDB
      if (update_conditions_db) {
	db_module_positions->storeCollection(_since.timeStamp(), _till.timeStamp(), module_position_col, db_module_position_description);
	if (write_flat_file) {
	  db_module_positions->createDBFile();
	}
      }		
#endif
      delete module_position_col;
    }
  }
#ifdef USE_CONDDB
  catch (CondDBException &error){
    std::cout << "CondDB Exception:" << error.getErrorCode() << ":" << error.getMessage() << std::endl;
    exit(-1);
  }
#endif
 catch (lcio::Exception &err) {
   std::cout << "LCIO Exception:" << err.what() << std::endl;
    exit(-1);
 }
 catch (std::exception &err) {
   std::cout << "Exception:" << err.what() << std::endl;
   exit(-1);
 }
}

