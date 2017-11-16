#ifndef FastCalibProcessor_h
#define FastCalibProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <map>

#include <lcio.h>
#include <lccd.h>


#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <LCHcalCalibrationObject.hh>
#include <ConditionsChangeDelegator.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <marlin/ConditionsProcessor.h>
#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>
#include <VRawADCValueProcessor.hh>
#include <collection_names.hh>
#include <TempModel.hh>
#include <CalibrationSet.hh>

namespace CALICE 
{
  
  template <class T, class H, const char *str, class M = TempModel>
  class FastCalibProcessor;
  
  /** Class which handles the application of a certain type T:LCHcalCalibrationObject to an 
   * input collection of CaliceHit and writes an output collection of CaliceHit
   * @author S.Schmidt, DESY
   * @date Sep 23 2006 
   **/
  
  template <class T, class H, const char *str, class M>
  class FastCalibProcessor : public marlin::Processor 
  {
  public: 
    virtual Processor* newProcessor() 
    {
      return new FastCalibProcessor<T,H,str,M>();
    }
    

    /***************************************************************************************************/
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /***************************************************************************************************/
    FastCalibProcessor() : Processor(str),
			   _calibrationChange(this,&FastCalibProcessor<T,H,str,M>::calibrationChanged),
			   _ahcSroModDataChange(this, &FastCalibProcessor<T,H,str,M>::ahcSroModDataColChanged),
			   _ahcSroModDataCol(0)
    {
      _description = "This processor makes a calibration constant available which is stored module wise";
      registerProcessorParameter("InputCollectionName", "Name of the input collection",
				 _inputColName, std::string("CaliceHitsLevel1"));
      
      registerProcessorParameter("OutputCollectionName", "Name of the output collection",
				 _outputColName, std::string("CaliceHitsLevel2"));
      
      registerProcessorParameter("CalibrationCollectionTemplate", 
				 "Template for the name of the module wise collections with the calibration constants",
				 _calColName, std::string("Calibration_"));
      
      registerProcessorParameter("AHC_SRO_ModData","Name of the conditions collection for the AHC Slow r/o data.",
				 _ahcSroModDataColName, std::string(COL_AHC_SRO_MOD));
      
      IntVec modulesExample;
      modulesExample.push_back(0);
      
      registerOptionalParameter("Module", "Add calibration for module", _modules, modulesExample, modulesExample.size());
      
      registerProcessorParameter("ZeroSuppression", 
				 "apply (1) or ignore (0) calibration induced cut",
				 _zeroSuppression,
				 1);
  
      registerProcessorParameter("DoMipCalibration", 
				 "apply (true) or ignore (false) MIP calibration",
				 _doMipCalibration,
				 bool(true));
      _tempModel = new M();
      _calibSet = new CalibrationSet<T>(_tempModel);
    };
    
    
    /***************************************************************************************************/
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /***************************************************************************************************/
     virtual ~FastCalibProcessor() 
    {
      delete _calibSet;
      delete _tempModel;
    };
  
  
    /***************************************************************************************************/
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /***************************************************************************************************/
    virtual void init() 
    {
      printParameters();
      std::stringstream message;
      bool error = false;

      if (parameterSet("Module")) 
	{
	  unsigned index = 0 ;
	  message << "Missing condition data: "; 

	  while (index < _modules.size() )
	    {	    
	      char _mod[10];
	      sprintf(_mod,"%.2d",_modules[index++]); 	
	      std::string _module(_mod);
	      std::string _finalCalColName = _calColName + _module;
	      
	      if (!marlin::ConditionsProcessor::registerChangeListener (&_calibrationChange ,_finalCalColName)) 
		{
		  message << " " << _finalCalColName;
		  error=true;
		} 
	      else 
		{
		  streamlog_out(DEBUG) << "FastCalibrationProcessor: register conditions data handler for collection " << _finalCalColName << std::endl; 
		}
	    }
	  if (error) 
	    { 
	      message <<  ".";
	      throw ErrorMissingConditionsDataHandler(message.str());
	    }
	}

      if(!marlin::ConditionsProcessor::registerChangeListener(&_ahcSroModDataChange, _ahcSroModDataColName)) 
	{
	  std::stringstream message;
	  message << "FastCalibrationProcessor::init(): no conditions data handler " << _ahcSroModDataColName;
	  throw ErrorMissingConditionsDataHandler(message.str());
	}

      _overwrite = (_inputColName == _outputColName);
      if (_overwrite)
        std::cout << "Replacing collection " << _outputColName << std::endl;
      else
        std::cout << "Appending collection " << _outputColName << std::endl; 
    };
    
    /***************************************************************************************************/
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /***************************************************************************************************/    
    void calibrationChanged(lcio::LCCollection* col) 
    {
      _calibSet->fill(col);
      _calibSet->setTemp(_ahcSroModDataCol);
    };


    /***************************************************************************************************/
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /***************************************************************************************************/
     void ahcSroModDataColChanged(lcio::LCCollection* col) 
    {
      _ahcSroModDataCol = col;
      _calibSet->setTemp(_ahcSroModDataCol);
    };
    

    /***************************************************************************************************/
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /*                                                                                                 */
    /***************************************************************************************************/
    virtual void processEvent(LCEvent *evt) 
    {
      streamlog_out(DEBUG)<<"\n\n process EVENT="<<evt->getEventNumber()<<std::endl;

      try 
	{
	  streamlog_out(DEBUG)<<"Try to get "<<_inputColName<<" collection"<<std::endl;
	  lcio::LCCollection* inVector = evt->getCollection(_inputColName);

	  lcio::LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::RAWCALORIMETERHIT);
	  lcio::LCFlagImpl hitFlag(_outputCol->getFlag());
	  hitFlag.setBit(LCIO::RCHBIT_TIME);
	  hitFlag.setBit(LCIO::CHBIT_ID1);
	  _outputCol->setFlag(hitFlag.getFlag());

	  streamlog_out(DEBUG) << "FastCalibrationProcessor::processEvent(), number of oldhits: " << inVector->getNumberOfElements() << std::endl;
	  
	  for (int i = 0; i < inVector->getNumberOfElements(); i++) 
	    {
	      streamlog_out(DEBUG) << "FastCalibrationProcessor::processEvent() oldHit: " << _inputColName << " ";
	      H* oldHit = dynamic_cast<H*>(inVector->getElementAt(i));

	      //oldHit->print(std::cout);

	      T* aCalibration = _calibSet->getCalib(oldHit->getModuleID(), oldHit->getCellKey());	  
	      if (aCalibration) 
		{
		  if (aCalibration->calibrationValid()) 
		    {
		      float _newValue = 0;
		      float _newError = 0;

		      if (_doMipCalibration == true)
			{
			  _newValue = aCalibration->applyCalibration(oldHit->getEnergyValue());
			  _newError = aCalibration->applyCalibrationError(oldHit->getEnergyValue(), oldHit->getEnergyError());
			}
		      else
			{
			  streamlog_out(DEBUG)<<"Do not do MIP calibration, keep old energy value"<<endl;
			  _newValue = oldHit->getEnergyValue();
			  _newError = oldHit->getEnergyError();
			}

		      streamlog_out(DEBUG)<<" old energy: "<<oldHit->getEnergyValue()
				  <<" new energy: "<<_newValue<<" MIP value: "<<aCalibration->getConstant()
				  <<" moduleID="<<oldHit->getModuleID()
				  <<" cellKey="<<oldHit->getCellKey()
				  <<" overwrite="<<_overwrite
				  <<" zeroSuppression="<<_zeroSuppression
				  <<" keepEvent="<<aCalibration->keepEvent(_newValue, _newError)
				  <<endl;
		      
		      if (!_overwrite) 
			{
			  if ((_zeroSuppression != 1 ) 
			      || aCalibration->keepEvent(_newValue, _newError) /* Angela Lucaci: here the 0.5 MIPs cut is applied, see calice_userlib/include/MIPConstants.h*/
			      ) 
			    {
			      H* aHit = new H(oldHit->getModuleID(), 
					      oldHit->getChip(), 
					      oldHit->getChannel(), 
					      _newValue, 
					      _newError, 
					      oldHit->getTimeStamp());

			      streamlog_out(DEBUG) << "FastCalibrationProcessor<T,str>, newHit module="
					   <<oldHit->getModuleID()
					   <<" chip="<<oldHit->getChip()
					   <<" channel="<<oldHit->getChannel()
					   <<" energy="<<_newValue
					   <<" +- "<<_newError
					   <<endl;
			      
			      //aHit->print(std::cout);

			      if (aHit) _outputCol->addElement(aHit); 
			    }
			} 
		      else 
			{
			  if ((_zeroSuppression != 1) || aCalibration->keepEvent(_newValue, _newError)) 
			    {
			      oldHit->setEnergyValue(_newValue);
			      oldHit->setEnergyError(_newError);
			    } 
			  else 
			    {
			      inVector->removeElementAt(i--);
			      delete oldHit;
			    }
			}
		    } 
		  else 
		    {
		      if (_overwrite) 
			{ 
			  inVector->removeElementAt(i--); delete oldHit; 
			}

		      streamlog_out(DEBUG) << "FastCalibrationProcessor<T,str>::processEvent(): calibration not valid" << std::endl;;

		    }
		} 
	      else 
		{
		  // std::cout << "No calibration found for hit " << oldHit->getCellID() << std::endl;
		  if (_overwrite) 
		    { 
		      inVector->removeElementAt(i--); 
		      delete oldHit; 
		    }

		  streamlog_out(DEBUG)<< "FastCalibrationProcessor<T,str>::processEvent(): no calibration" << std::endl;

		}
	    } /*----------------------- end loop over i --------------------------------------------------------*/ 
	  
	  if (!_overwrite) evt->addCollection(_outputCol, _outputColName);
	}/*------------ end of try() ---------------------------------------------------------------------------*/
      catch (DataNotAvailableException &e) 
	{
	  streamlog_out(DEBUG) << "FastCalibrationProcessor<T,str>::processEvent(): data not available exception" << std::endl;
	}	
    };
    
    

  protected:
    std::string _inputColName;
    std::string _outputColName;
    std::string _calColName;
    std::string _ahcSroModDataColName;
    IntVec _modules;
    bool _overwrite;
    int _zeroSuppression;
    bool _doMipCalibration;
    CalibrationSet<T>* _calibSet;
    M* _tempModel;
     
  private:
  ConditionsChangeDelegator<FastCalibProcessor<T, H, str, M> > _calibrationChange;
  ConditionsChangeDelegator<FastCalibProcessor<T, H, str, M> > _ahcSroModDataChange;
    lcio::LCCollection* _ahcSroModDataCol;

  };
  
} 

#endif
