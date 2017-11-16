#include "createEcalModuleLocationConddata.hh"

#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <cassert>

#include <IMPL/LCCollectionVec.h>
#include <UTIL/LCTime.h>

#include <ReadDaqMap/parseDateTime.hh>
#include <ReadDaqMap/ReadLine.hh>
#include <ReadDaqMap/TLineIterator.hh>

#include <ModuleLocation.hh>
#include <CellIndex.hh>

#include <RtypesSubSet.h>

void old_geometry(std::vector<float> &layer_z_pos_old, std::vector<float>::const_iterator &layer_z_pos_iter, std::vector<float>::const_iterator &layer_z_pos_end)
{
  layer_z_pos_old.clear();

    const UInt_t n_structures=3;
    const UInt_t structure_end[3]={10,20,30};

    // gap between two aveolar structures
    // frames are separated by 2mm aluminum plates, the alveolar structure is slightly wider than the frame
    const float structure_sep=0.2-2*.02; //cm

    // the distance of the detector front plate and the first wafer
    const float first_layer_sep=
      //      0.05       //carbon?
      + 0.14        //tungsten
      + 0.01       //shielding
      + 0.         //gap
      + 0.21       //pcb
      + 0.01       //glue
      + 0.0525*.5; // 1/2 Si

    // separation between two wafers from even to odd layers (without the tungsten sheet)
    // 1/2 SI + ground + carbon + tungsten + carbon + ground + 1/2 SI
    const float layer_sep_even_to_odd=
      0.0525    //2* 1/2 Si
      + 0.005*2    //2* glue between Si and ground
      + 0.003*2    //2* ground
      + 0.015*2;   //2* carbon (i.e. carbon + tungsten + carbon)

    // separation between two wafers from odd to even layers (without the tungsten sheet)
    // 1/2 SI + glue + PCB + slab_gap=0 + shielding + aveolar_gap + tungsten + aveolar_gap + shielding + slab_gap + PCB + glue + 1/2 SI
    const float layer_sep_odd_to_even=
      0.0525    //2* 1/2 Si
      + 0.01*2    //2* glue
      + 0.21*2    //2* PCB
      + 0.        //2*slab_gap
      + 0.01*2    //shielding
      + 0.03*2;    //alveolar gap
    //cm

    // the layer seperation for the three aveolar structures including the tungsten sheeet
    const float layer_sep[3][2]={
      {layer_sep_even_to_odd+0.14*1, layer_sep_odd_to_even+0.14*1},
      {layer_sep_even_to_odd+0.14*2, layer_sep_odd_to_even+0.14*2},
      {layer_sep_even_to_odd+0.14*3, layer_sep_odd_to_even+0.14*3}
    }; //cm

    // calculate z-coordinates of the layers
    {
      UInt_t layer_i=0;
      Float_t z_pos_old=first_layer_sep;
      
      for (UInt_t structure_i=0; structure_i<n_structures; structure_i++) {

	// new z pos
	//	Float_t z_pos=structure_start_z[structure_i];
	UInt_t old_layer_i=layer_i;

	layer_i=old_layer_i;
	for (;layer_i<structure_end[structure_i]; layer_i++) {
	  z_pos_old +=   layer_sep[structure_i][(layer_i+1)%2];
	  layer_z_pos_old.push_back( z_pos_old );
	}
	z_pos_old += structure_sep;
      }
      layer_z_pos_old.push_back( z_pos_old );
      
      Float_t z_shift=layer_z_pos_old.back()+0.7; // to match Mokka -7mm?
      Float_t last=*(layer_z_pos_old.begin());
      layer_i=0;

      for (std::vector<float>::iterator layer_z_pos_old_iter=layer_z_pos_old.begin();
	   layer_z_pos_old_iter!=layer_z_pos_old.end();
	   layer_z_pos_old_iter++, layer_i++) {

	assert (layer_z_pos_iter != layer_z_pos_end ) ;

	(*layer_z_pos_old_iter) -= z_shift;
	std::cout << layer_i << ": " << std::setw(14) << (*layer_z_pos_old_iter) << " " << std::setw(14) << (*layer_z_pos_old_iter) - last
		  << std::setw(14) << (*layer_z_pos_iter) 
		  << " " << std::setw(14) << (*layer_z_pos_iter) - (*layer_z_pos_old_iter)
		  << std::endl;
	last= (*layer_z_pos_old_iter);
	layer_z_pos_iter++;
      }
    }

}

void create_new_geometry(std::vector<float> &layer_z_pos)
{

    // There are three structures.
    // The first ends after layer 10, the second ends after layer 20, ...
    const UInt_t n_structures=3;
    const UInt_t structure_end[3]={10,20,30};
  
    // the total width of the three alveolar structures.
    const double width_of_alveolar_structures[3]={
      5.29,
      6.69,
      8.09
    }; //cm

    // the separation of two alveolar structures
    const double alveolar_sep=0.1; //cm

    // 2 * composite around tungsten plate
    const double alveolar_composite_width=2*0.03; //cm

    // width of the tungsten+composite separation walls of the alveolar
    // float alveolar_tungsten_composite_width[3]; //cm :  tungsten + 2*composite 

    // the width of the carbon/composite at the end of the alveolar.
    const double alveolar_end_composite_width=.04; // cm

    // gap between the aluminium shielduing of a slab and the sepration walls of the alveolar.
    // const float slab_wall_gap=0.01; // cm

    // the distance between two sensitive layers insided a slab
    // the hit is located in the middle of the silicon
    const double slab_inner_width_no_tungsten =
      + .03   //2 * .015           // carbon composite
      // considered to be neglectable      + 2 * .008           // glue+grounding (Al);
      + .0525 //2 * 0.5 * .0525     // half a Si wafer
      ;

    const double tungsten_width[3]={
      .14,
      .28, // .14*2,
      .42 // .14*3
    };  //cm

    // the width of the alveol of the 3 structures 
    // (difference == additional thickness of tungsten sheets)
    //const float alveol_width[3]={
    //  .85,
    //  .99,
    //  .113
    //}; //cm


    // the inner slab distance between the centres of two Si wafers.
    //    float slab_inner_width[3]; //cm : tungsten + carbon composite, glue, grounding and half Si wafers.

    //    const float slab_outer_width=
    //      + 0.5*.0525           // half a Si wafer
    //      +     .01             // glue Si-PCB
    //      +     .21            // a PCB
    //      +     .0395           // gap
    //      +     .01             // Al shielding
    //      ;

    //    float structure_start_z[3];
    float alveol_centre_z[3][5];
    float z_pos_alveolar_outer_plane=0;
    
    {
      double structure_start=z_pos_alveolar_outer_plane;
      for (UInt_t structure_i=3; structure_i-->0; ) {
	structure_start -= width_of_alveolar_structures[structure_i];
	
	// distance between two alveol of the same structure
	Float_t alveol_spacing=(width_of_alveolar_structures[structure_i]-alveolar_end_composite_width)/5.;
	
	// the thickness of the tungsten + composite at the beginning of each alveol
	Float_t alveol_tungsten_width = tungsten_width[structure_i] + alveolar_composite_width;

	// the width of the empty space;
	Float_t alveol_width=alveol_spacing - alveol_tungsten_width;

	for (UInt_t alveol_i=0; alveol_i<5; alveol_i++) {
	  double centre_z= structure_start 
	    + alveol_tungsten_width  + alveol_width * .5
	    + alveol_spacing * alveol_i;

	  alveol_centre_z[structure_i][alveol_i]=static_cast<float>(centre_z);
	}

	// structure_start_z[structure_i]=structure_start;

	//   slab_inner_width[structure_i]=tungsten_width[structure_i]+slab_inner_width_no_tungsten;
	//   alveolar_tungsten_composite_width[structure_i]=tungsten_width[structure_i]+alveolar_composite_width;

	structure_start -= alveolar_sep;
	//	std::cout << structure_i << " : " 
	//		  << " slab_inner = " << slab_inner_width[structure_i] 
	//		  << " W/comp=" << alveolar_tungsten_composite_width[structure_i] << std::endl;
      }
    }

    // calculate z-coordinates of the layers
    layer_z_pos.clear();
    {
      UInt_t layer_i=0;
      for (UInt_t structure_i=0; structure_i<n_structures; structure_i++) {

	UInt_t alveol_i=0;

	double inner_slab_width = slab_inner_width_no_tungsten + tungsten_width[structure_i];
	double half_inner_slab_width = inner_slab_width * .5;

	for (;layer_i<structure_end[structure_i]; layer_i+=2) {

	  double z_pos = alveol_centre_z[structure_i][alveol_i]
	    - ( half_inner_slab_width  ) ;

	  layer_z_pos.push_back( static_cast<float>(z_pos) );

	  z_pos += inner_slab_width;

	  layer_z_pos.push_back( static_cast<float>(z_pos) );

	  alveol_i++;
	}

	if (structure_i + 1 ==n_structures) {

	  // memorise the z pos of the last outer plane.
	  layer_z_pos.push_back( z_pos_alveolar_outer_plane );
	}

      }
      
    }
}


enum EModuleType {kCentral, kCentralFlipped, kLowLeft,kLowRight,kNModuleTypes,kNotConnected};

class ModuleLocation_t {
public:
  ModuleLocation_t() : _layerNumber(-1),_cellIndexOffset(0),_type(kNotConnected) {};
  ModuleLocation_t(int layer, int cell_offset, EModuleType type) : _layerNumber(layer),_cellIndexOffset(cell_offset),_type(type) {};
  
  int _layerNumber;
  int _cellIndexOffset;
  EModuleType _type;
} ;

EVENT::LCCollection *createEcalModuleLocationConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &geometry_file)
{
  // ATTENTION: Internal length units are cm length. The Units used for LCObjects are mm
  const Float_t length_unit=1e-2/1e-3; // comversion from cm to mm

    const float wafer_width=6.2; //cm
    const float wafer_height=6.2; //cm
    //    const float wafer_sep_x=0.015; // cm
    const float wafer_sep_y=0.015; // cm

    const float wafer_sep_of_central_and_lower_slab_y=
      2*0.015    // gap between wafer and carbon support (H); both slabs
      + 2*0.03   // width of carbon support (H)
      + 2*0.01   // shielding
      + 2*0.01   // gap between slab and slab support of the aveolar structure

      + 0.04;    // width of the slab support of the aveolar structure
    //cm

    const float odd_layer_shift_x=-.25; //cm

    const float module_ensemble_shift_x=
      - 37.45/2.           // half the width of the detector front
      + 0.7                // start of plug
      + 1.4                // width of plug width
      + 0.3                // width of upper plug-pcb separator
      //      + 1.0                // gap between plug and first wafer
      +.98 // value in mokka  is .98 instead of 1.0
      + wafer_width // glass wafer (not considered by the module definition)
                    // removed the gap between the glass wafer and the 
      - odd_layer_shift_x; // the odd layers are closer to the plug (and the odd_layer_shift_x is negative)
    // cm
    
    
    //lower / central
    const float module_ensemble_shift_y[2]={
      - wafer_sep_of_central_and_lower_slab_y 
      - (wafer_height  + wafer_sep_y * 3)
      - (wafer_height*.5 + wafer_sep_y ),
      - (wafer_height*.5 + wafer_sep_y )
    };
  
    std::cout << "y shift central: " << module_ensemble_shift_y[0] << std::endl;
    std::cout << "y shift bottom: " << module_ensemble_shift_y[1] << std::endl;


    std::vector<ModuleLocation_t> moduleLocation;
    //The following leads to a vector with 2*n_layer entries (e.g. 60) since we have 30 central and 30 lower layers.
    for (UInt_t layer_i=0; layer_i<30; layer_i++) {
      moduleLocation.push_back(ModuleLocation_t(layer_i,CALICE::CellIndex(0,1,0,0,layer_i+1).getCellIndex(),(layer_i%2==0 ? kLowRight : kLowLeft)));
      moduleLocation.push_back(ModuleLocation_t(layer_i,CALICE::CellIndex(1,1,0,0,layer_i+1).getCellIndex(),(layer_i%2==0 ? kCentralFlipped : kCentral)));
    }
    std::vector<Double_t> moduleShift;
    std::vector<bool> moduleConnected;
    moduleShift.resize(moduleLocation.size(),0.);
    moduleConnected.resize(moduleLocation.size(),false);

    std::vector<Float_t> layerShift;
    Float_t ensemble_shift_y=0;
    Float_t ensemble_shift_z=0;

    Bool_t new_line;

    ReadLine in(200,geometry_file.c_str());
    while ((new_line=in.ReadNextLine())) {
      TLineIterator line_iter(in.GetBuffer());
      if (line_iter.IsEmpty()) continue;    //skip empty ln the ines
      if (line_iter.IsComment()) continue;  //skip header

      if (line_iter.IsAlpha()) {
	std::string first_word=line_iter.GetWord();
	if (first_word=="validity:") { 
	  if (since.timeStamp()<till.timeStamp()) {
	    continue;
	  }
	  else {
	    since=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
	    till=parseDateTime(line_iter.GetWord(true),line_iter.GetWord(true));
	  }
	}
	else if (first_word=="LayerShift:") { 
	  UInt_t start_layer=line_iter.GetUnsignedInteger(); 
	  UInt_t end_layer=line_iter.GetUnsignedInteger(); 
	  Float_t shift=static_cast<Float_t>(line_iter.GetDouble());
	  if (start_layer<=end_layer && end_layer<=40) {
	    if (end_layer>=layerShift.size()) layerShift.resize(end_layer+1,0.);
	    for (UInt_t layer_i=start_layer; layer_i<=end_layer; layer_i++) {
	      layerShift[layer_i]+=shift;
	    }
	  }
	}
	else if (first_word=="yShift:") { 
	  ensemble_shift_y=static_cast<Float_t>(line_iter.GetDouble());
	}
	else if (first_word=="zShift:") { 
	  ensemble_shift_z=static_cast<Float_t>(line_iter.GetDouble());
	}
	continue;  //skip header
      }
      
      UInt_t layer_i=line_iter.GetUnsignedInteger(); 
      UInt_t lower_or_center=line_iter.GetUnsignedInteger(); 
      Double_t shift=line_iter.GetDouble();
      UInt_t  connected=line_iter.GetUnsignedInteger();
      if (layer_i>=30 && lower_or_center>1 && layer_i*2+lower_or_center<moduleLocation.size()) {
	std::stringstream message;
	message << "ERROR:createEcalModuleLocationConddata> " << "Error in line " << in.GetLineNumber() << ". Invalid layer(0-29) or lower=0/central=1 flag.";
	throw std::runtime_error(message.str());
      }
      if (connected>0) {
	moduleConnected[layer_i*2+lower_or_center]=true;
	moduleShift[layer_i*2+lower_or_center]=shift;
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

    if (since.timeStamp()>=till.timeStamp()) {
      std::runtime_error("ERROR:createEcalModuleLocationConddata> No valid validity range in input file.");
    }
  
    std::vector<float> layer_z_pos;
    create_new_geometry(layer_z_pos);

      LCCollectionVec* module_position_col = new LCCollectionVec( LCIO::LCGENERICOBJECT )  ;
    
      for (UInt_t module_i=0; module_i<moduleLocation.size(); module_i++) {
      
	// skip not connected modules
	if (moduleLocation[module_i]._type>=kNModuleTypes) continue;

	//	UInt_t struct_i=structure_number[moduleLocation[module_i]._layerNumber];
	float pos_x=0.;
	std::cout << "Moudule Ensemble Shift: " << module_ensemble_shift_x << std::endl;
	std::cout << "Module Shift: Module " << module_i  << " Shift:" << moduleShift[module_i] << std::endl;
	std::cout << "Module Shift Layer: " << module_i << " Shift: " << layerShift[moduleLocation[module_i]._layerNumber] << std::endl;  
	pos_x  = module_ensemble_shift_x;
       	pos_x += moduleShift[module_i];
	if (static_cast<unsigned int>(moduleLocation[module_i]._layerNumber) < layerShift.size()) {
	  pos_x += layerShift[moduleLocation[module_i]._layerNumber];
	}
	if (moduleLocation[module_i]._layerNumber%2==1) {
	  //Kick the modules in the other direction otherwise they are closer to the
          //reference point of the measurement which is defined to be the far right end
          //of the slab (as defined by Marc-Anduze)
          //It's not necessarily the definition applied before (-> to be verified)
	  //pos_x += odd_layer_shift_x;
	    pos_x -= odd_layer_shift_x;
	}

	float pos_y=module_ensemble_shift_y[(moduleLocation[module_i]._type==kLowRight || moduleLocation[module_i]._type==kLowLeft ? 0 : 1) ] + ensemble_shift_y;

	assert ( static_cast<unsigned int>(moduleLocation[module_i]._layerNumber) + 1 < layer_z_pos.size() );
	float pos_z=layer_z_pos[moduleLocation[module_i]._layerNumber] + ensemble_shift_z;


	CALICE::ModuleLocation *a_module_position=new CALICE::ModuleLocation;
	(*a_module_position).setX(pos_x*length_unit).setY(pos_y*length_unit).setZ(pos_z*length_unit)
	  .setModuleType(moduleLocation[module_i]._type)
	  .setCellIndexOffset( moduleLocation[module_i]._cellIndexOffset); 
			       /* now k-1 is stored! not needed anymore + CALICE::CellIndex(0,0,0,0,1).getCellIndex()) */

        

	a_module_position->print(std::cout);
	module_position_col->addElement(a_module_position);
      }
      return module_position_col;
}
