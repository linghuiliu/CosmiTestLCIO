#include <iostream>
#include <iomanip>
#include <cstdlib>

#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCParameters.h>
#include <EVENT/LCIO.h>
#include <UTIL/LCTime.h>

#include <ModuleDescription.hh>
#include <CellIndex.hh>

EVENT::LCCollection *createEcalModuleDescriptionConddata(UTIL::LCTime &since, UTIL::LCTime &till, bool verbose)
{
  // ATTENTION: Internal length units are cm length. The Units used for LCObjects are mm
  const Float_t length_unit=1e-2/1e-3; // comversion from cm to mm
  
    // module description i.e. PCB
    //------------------------
    // modules: central, low left, low right
    // three 2 times 3 wafers with a gap of ~0.015cm
    const int  n_wafer_columns=3;
    const int  n_wafer_rows   =2;

    //    const int  n_pad_columns=6;
    const int  n_pad_rows   =6;
  
    const float wafer_width=6.2; //cm
    const float wafer_height=6.2; //cm
    const float wafer_sep_x=0.015; // cm
    const float wafer_sep_y=0.015; // cm

    const Float_t module_height=
      wafer_height         * n_wafer_rows
      + wafer_sep_y * (n_wafer_rows-1);

    const Float_t module_width=
      wafer_width * n_wafer_columns
      + wafer_sep_x * (n_wafer_columns-1);


    // wafer description
    // -----------------
    const Float_t cell_width=  1.;  //cm 
    const Float_t cell_height= 1.; //cm

    const float gaurd_ring_width=    0.1;   // cm
    const float pad_sep=    0.;    // cm

    // derived properties:
    // seperation between pads

    const float pad_centre_sep_x = cell_width+pad_sep;
    const float pad_centre_sep_y = cell_height+pad_sep;
  
    //  const float adjacent_wafer_pad_centre_sep_x=
    //      gaurd_ring_width
    //    + separation_of_wafers
    //    + cell_width;

    //  const float adjacent_wafers_pad_centre_sep_y=
    //      gaurd_ring_width
    //    + separation_of_wafers
    //    + cell_height;

    const float first_pad_centre_x=
      gaurd_ring_width
      + cell_width *.5
      + wafer_sep_x;

    //CRP Modules labelled 'D' are readout right-to-left
    const float first_pad_centre_x_flipped=module_width -
      (gaurd_ring_width
      + 5*cell_width *.5
      + wafer_sep_x);

    // included again: wafer_sep_x

    const float first_pad_centre_y=
      gaurd_ring_width
      + cell_height *.5
      + wafer_sep_y;
    //  included again: wafer_sep_y


    enum EModuleType {kCentral, kCentralFlipped, kLowLeft,kLowRight,kNModuleTypes,kNotConnected};

    UInt_t n_module_types=kNModuleTypes;
    EModuleType module_type[kNModuleTypes]={kCentral,kCentralFlipped,kLowLeft,kLowRight};
    const char *module_type_name[kNModuleTypes]={"C","C","G","D"};

    UInt_t cells_per_module[kNModuleTypes]={216,216,108,108};
    //Why this offset?????
    //Float_t module_y_offset[kNModuleTypes]={0.,0.,0.,6.2+.015+.2}; //cm
    Float_t module_y_offset[kNModuleTypes]={0.,0.,0.,0.}; //cm
    Bool_t flip_y_coordinate[kNModuleTypes]={false,true,false,true};
    Bool_t flip_x_coordinate[kNModuleTypes]={false,false,false,true};
    UInt_t n_multiplex(18);
    UInt_t n_maxchips(12);

    //Year,month, dat,hour,min,sec
    LCTime firstModule( 2004,     1,   2,   8,  0 , 0) ;
    LCTime   farFuture( 2015,     3,   1,   4 , 1 , 5 ) ;
    
    if (since.timeStamp()>=till.timeStamp()) {
      since=firstModule;
      till=farFuture;
    }

    IMPL::LCCollectionVec* module_description_col = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT )  ;

#if  LCIO_VERSION_GE( 1 , 7 )
    module_description_col->parameters().setValue(LCIO::CellIDEncoding,"M:3,S-1:3,I:9,J:9,K-1:6");
#endif


    for (UInt_t module_i=0; module_i<n_module_types; module_i++) {
      CALICE::ModuleDescription *a_module=new CALICE::ModuleDescription(module_type[module_i],
									cells_per_module[module_i],
									module_type_name[module_i]);
      (*a_module)
	.setWidth(module_width*length_unit)
	.setHeight(module_height*length_unit)
	.setCellWidth(cell_width*length_unit)
	.setCellHeight(cell_height*length_unit);

      // calculate cell positions of one module (the origin is the lower left corner)
      // a wafer contains 6x6 pads
      // 3 pad columns are connected to one chip, the other 3 columns to the other chip
      // The read directions is: first columns, then rows.
      // The lower rows are read first.

      if(verbose) std::cout << "Description of Module type: " << module_i << std::endl;

      for (UInt_t cell_i=0; cell_i<cells_per_module[module_i]; cell_i++) {

	std::cout << "cell_i: " << cell_i  << " mod 12: " << (cell_i%12) << std::endl;

	UInt_t wafer_i=(cell_i%(cells_per_module[module_i]/n_multiplex))/2;
	if (flip_x_coordinate[module_i]) wafer_i =  n_wafer_columns - wafer_i -1;


        //UInt_t wafer_i=(cell_i%12)/2;
	UInt_t wafer_column=wafer_i%n_wafer_columns;
	UInt_t wafer_row=wafer_i/n_wafer_columns;

	//	std::cout << "wafer i: " << wafer_i << std::endl;   
	//	std::cout << "wafer row desc: " << wafer_row << std::endl;   
     

	UInt_t pad_index=(cell_i*n_multiplex/cells_per_module[module_i]);
	UInt_t chip_index=(cell_i%2);
        if(flip_x_coordinate[module_i]) chip_index = abs((cell_i%2)-1);      

	UInt_t pad_row_i=pad_index/3;
        UInt_t pad_column_i;
        pad_column_i=pad_index%3+chip_index*3;


	//        std::cout << "pad index desc: " << pad_index << std::endl;
	//        std::cout << "pad row desc: " << pad_row_i << std::endl;
	

        float pos_x= first_pad_centre_x
	  + (pad_column_i) * pad_centre_sep_x
	  + (wafer_column)*(wafer_width+wafer_sep_x);

      
	if (!flip_y_coordinate[module_i]) {
	  wafer_row=n_wafer_rows-((n_maxchips*n_multiplex)/cells_per_module[module_i])-wafer_row;
	  pad_row_i=n_pad_rows-1-pad_row_i;
	}
	float pos_y;
	//	if (!flip_y_coordinate[module_i]) {
	  pos_y=
	    first_pad_centre_y
	    + (n_pad_rows-1-pad_row_i) * pad_centre_sep_y
	    + (wafer_row ) * (wafer_height + wafer_sep_y)
	    + module_y_offset[module_i];
	  //	}
	  //	else {
	  //	  pos_y=
	  //	    first_pad_centre_y
	  //	    + (pad_row_i) * pad_centre_sep_y
	  //	    + (n_wafer_rows-1-wafer_row ) * (wafer_height + wafer_sep_y)
	  //	    + module_y_offset[module_i];
	  //	}
	  
	  if (verbose) {
	    std::cout << std::setw(3) << cell_i << ":" 
		      << std::setw(1) << wafer_column << "/" << std::setw(1) << wafer_row 
		      << " " << std::setw(1) << pad_column_i << "/" << std::setw(1) << pad_row_i
		      << std::setw(7) << pos_x << "/" << std::setw(7) << pos_y ;
	    //if (cell_i%6==5)
	      std::cout <<std::endl;
	  }

	a_module->setCellPos(cell_i,pos_x*length_unit,pos_y*length_unit);
	
	// the layer index and the wafer row indices need to be adjusted to complete the index of installed modules.
	// change wafer/pad numbering from 1 - 3/ 1-6

		a_module->setGeometricalCellIndex(cell_i,CALICE::CellIndex(wafer_row + 1, wafer_column + 1,
								   n_pad_rows - 1 - pad_row_i + 1, pad_column_i+1,1).getCellIndex());
      }
      if (verbose) {
	std::cout <<std::endl;
	
	std::cout << "module " << module_i << " n_floats=" << a_module->getNFloat() << " n_ints=" << a_module->getNInt() 
		  << " n_double=" << a_module->getNDouble() 
		  << " n_cells=" << a_module->getNCells()
		  << std::endl;
      }

      module_description_col->addElement(a_module);
    }
    return module_description_col;
}
