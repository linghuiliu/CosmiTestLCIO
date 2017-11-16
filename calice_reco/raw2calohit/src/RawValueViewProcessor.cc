//CRP 12/4/06 added  HAVE_CALICEGUI
#ifdef HAVE_CALICEGUI

#include "RawValueViewProcessor.hh"
#include <RawValueDisplay.hh>
#include <RawValueDisplayData.hh>
#include <AdcValueAccess.hh>
#include <CellParameterAccess.hh>
#include <Display.hh>
#include <GuiThread.hh>
#include <stdexcept>
#include <iostream>
#include <NoOpCalibration.hh>
#include <CalibrationFactory.hh>
#include <marlin/Exceptions.h>

namespace CALICE {

  RawValueViewProcessor a_RawValueViewProcessor_instance;

  RawValueViewProcessor::RawValueViewProcessor() 
    : VRawADCValueProcessor("RawValueViewProcessor"),
      _calibration(0),
      _display(0)
  {
    _cellParameterCollectionName="CellParameters";
    registerProcessorParameter( "CellParameterCollectionName" , 
				"The name of the collection which contains the pedestals, the noise, etc. of all the cells." ,
				_cellParameterCollectionName ,
				_cellParameterCollectionName);

    // ---- collection name of calibration constants and name of calibration object
    registerProcessorParameter( "Calibration" , 
    				"Name of object to be used for the calibration"  ,
				_calibrationObjectName ,
    				std::string("NoOpCalibration") ) ;

    registerOptionalParameter( "CalibrationConstants" , 
			       "Name of the conditions data collection which contains the calibration constants"  ,
			       _calibrationConstantColName ,
			       std::string("") );

  }

  RawValueViewProcessor::~RawValueViewProcessor() 
  {
    if (_display) {
      GLVIEW::GuiThread *gui=GLVIEW::GuiThread::getInstance();
      Display *display_ptr=static_cast<Display *>(_display);
      gui->removeDisplay(&display_ptr);
    }
    delete _calibration;
  }

  void RawValueViewProcessor::init( )
  {
    Display *display_ptr=0;
    RawValueDisplayKit display_kit;
    GLVIEW::GuiThread *gui=GLVIEW::GuiThread::getInstance();
    gui->registerDisplay(&display_ptr,&display_kit,"Value Display");

    VRawADCValueProcessor::init();
    printParameters();

    // --- Calibration object
    delete _calibration;
    CalibrationFactory::getInstance()->listKits();
    _calibration=CalibrationFactory::getInstance()->createCalibrationObject(_calibrationObjectName, _colNameModuleDescription, _calibrationConstantColName);

    if (!_calibration) {
      std::stringstream message;
      message << "SimpleHitSearch::init> Failed to create calibration object \"" << _calibrationObjectName << "\".";
      throw std::runtime_error(message.str());
    }

    gui->waitForDisplay(&display_ptr);
    _display=dynamic_cast<RawValueDisplay *>(display_ptr);
    if (!_display) {
      throw std::runtime_error("RawValueViewProcessor::Init> ERROR: Value Display creation failed!");
    }

  }

  void RawValueViewProcessor::processEvent( LCEvent * evtP )
  {
    try {

      LCCollection* col_adc = evtP->getCollection( _adcColName ) ;
      //TODO: not very efficient to copy the int vector every time. In particular, since it rarly changes.
      if (col_adc && col_adc->getNumberOfElements()>0) {
	

	DisplayDataPtr<RawValueDisplayData> buffer(_display, evtP->getRunNumber(), evtP->getEventNumber());

	if (buffer) {
	  buffer->reset();
	  for (UInt_t module_i = 0; module_i<_alignment.getNModules(); module_i++) {
	    buffer->setNCells(module_i, _alignment.getNCellsPerModule(module_i));
	  };
	  
	  AdcValueAccess adc_access(col_adc, &_alignment, _calibration, 0);
	  if (adc_access.hasConnectedBlocks()) {
	    EVENT::LCCollection *a_col=evtP->getCollection(_cellParameterCollectionName);
	    
	    CellParameterAccess cell_paramter_access(_cellParameterCollectionName, evtP);
	    //	UInt_t n_values=0;
	    do {
	      UInt_t module_index=cell_paramter_access.getIndexOffset(adc_access.getModuleIndex());
	      do {
		//	    Int_t raw_adc_value=adc_access.getAdcValue();
		try {
		  const CellParameter cell_parameter(cell_paramter_access.getCellParameter(module_index+adc_access.getCellIndexOnModule()));
		  
		  buffer->set(adc_access.getModuleIndex(),adc_access.getCellIndexOnModule(),
			      adc_access.getAdcValue(),
			    adc_access.getCalibratedValue(cell_parameter.getPedestal()),
			      cell_parameter.getPedestal(),
			      cell_parameter.getNoise(),
			      cell_parameter.getNHits(),
			      cell_parameter.isDead());
		}
		catch (std::logic_error &err) {
		  std::cerr << adc_access.getModuleIndex() << ", " << adc_access.getCellIndexOnModule() 
			    << " -> " << module_index+adc_access.getCellIndexOnModule() << " / " << a_col->getNumberOfElements()
			    << ": " << err.what() << std::endl;
		}
		
	      } while (adc_access.nextValue());
	    } while (adc_access.nextBlock());
	  }
	}
      }

    }
    catch (  DataNotAvailableException &err) {
    }

  }

  void RawValueViewProcessor::end()
  {
    // VRawADCValueProcessor::end()
  }
}

//CRP endif  HAVE_CALICEGUI
#endif
