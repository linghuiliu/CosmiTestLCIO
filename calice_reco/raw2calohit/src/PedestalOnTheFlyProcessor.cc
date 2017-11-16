#include "PedestalOnTheFlyProcessor.hh"

#include <iostream>
#include <cmath>

#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <IMPL/LCCollectionVec.h>

#include <marlin/Exceptions.h>

#include <collection_names.hh>
#include <TriggerBits.hh>
#include <FastCaliceHit.hh>
#include <TcmtHit.hh>
#include <cassert>
#include <string>
using namespace std;

#include <HcalTileIndex.hh>
#include <UTIL/CellIDEncoder.h>

#define HCALRECO_DEBUG 

namespace CALICE {

  PedestalOnTheFlyProcessor aPedestalOnTheFlyProcessor;

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  PedestalOnTheFlyProcessor::PedestalOnTheFlyProcessor() : marlin::Processor("PedestalOnTheFlyProcessor")
  {
    _description = "Pedestal subtraction using pedestal extracted on the fly from the same run.";
    
    registerProcessorParameter("InputCollection",
			       "Name of input collection of CaliceHits",
			       _inputColName,
			       std::string("RawCaliceHits"));

    registerProcessorParameter("OutputCollection",
			       "Name of output collection of CaliceHits",
			       _outputColName,
			       std::string("CalibratedCaliceHits"));

    registerProcessorParameter("SignificanceCut", 
			       "Defines how significant the deviation of a hit from the pedestal has to be to survive the zero suppression.",
			       _significanceCut,
			       (float)3.0);

    registerProcessorParameter("SkipPedestals",
			       "Skip all events which have been used for pedestal calculations in the following processors",
			       _skipPedestals,
			       (int)0);

    registerProcessorParameter("SkipStartUpPedestals",
			       "Skip all pedestals for which the minimal number of pedestals has not yet been reached",
			       _skipStartUpPedestals,
			       (int)0);

    registerProcessorParameter("minPedNumber",
			       "Minimum number of pedestal before the pedestal value is considered valid and pedestal substraction is applied",
			       _minPedNumber,
			       (int)20);
        
    registerProcessorParameter("ThrowSkipEventException",
			       "Do the skipping via SkipEventException",
			       _throwSkipEventException,
			       true);
    
  }
  
  
  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void PedestalOnTheFlyProcessor::init()
  {
    printParameters();
    for (unsigned short mod = 0; mod < HCAL_N_MOD + 1; mod++)
      {
	for (unsigned short cell = 0; cell < HCAL_N_CELL; cell++)
	  {
	    _pedSum[mod][cell] = 0;
	    _pedSumSquare[mod][cell] = 0;
	    _pedNum[mod][cell] = 0;
	    _ped[mod][cell] = 0;
	    _pedError[mod][cell] = 0;
	  }
      }
    _pedCounter = 0;
  }
  

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void PedestalOnTheFlyProcessor::processEvent(LCEvent *evt)
  {
    LCCollection *inCol = evt->getCollection(_inputColName);
    
    if (inCol == NULL)
      {
	streamlog_out(DEBUG)<<"Collection "<<_inputColName<<" not found"<<endl;
	return;
      }
     
    streamlog_out(DEBUG)<<"\n\nprocess EVENT="<<evt->getEventNumber()<< endl;
    streamlog_out(DEBUG)<<"Input collection "<<_inputColName<<" has "<<inCol->getNumberOfElements()<<" elements"<<endl;
     
    TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));
    if (trigBits.isPurePedestalTrigger()) 
      {
	streamlog_out(DEBUG) << "-->collecting pedestals" << endl;

	for (unsigned int i = 0; i <static_cast<unsigned int>(inCol->getNumberOfElements()); i++)
	  {
	    LCObject const* pobj = inCol->getElementAt(i);
	    FastCaliceHit const* fhit = dynamic_cast<FastCaliceHit const*>(pobj);
	    TcmtHit const* thit = dynamic_cast<TcmtHit const*>(pobj);
	    
	    unsigned short mod  = 0;
	    unsigned short cell = 0;
	    double ampl         = 0;
	    double amplErr      = 0;

	    
	    if(fhit) /*if HCAL hit (historically kept)*/
	      {
		mod = fhit->getModule();
		cell = fhit->getChip()*18 + fhit->getChannel();
		ampl = fhit->getEnergyValue();
		amplErr = fhit->getEnergyError();
		assert(mod<=39);
		assert(cell<=216);
	      }
	    else if(thit)  /*if TCMT hit*/
	      {
		unsigned int cellKey = thit->getCellKey();
		mod = (cellKey >> 8) & 0xFF;
		cell = cellKey & 0xFF;
		ampl = thit->getEnergyValue();
		amplErr = thit->getEnergyError();
		assert(mod<=16);
		assert(cell<=20);
	      }

	    streamlog_out(DEBUG)<<"old hit: module="<< mod <<" cell="<< cell 
			<<" energy="<< ampl <<" +- "<< amplErr <<" fhit="<<fhit<<" thit="<<thit<< endl;

	    assert( (mod <= HCAL_N_MOD+1) && (cell <= HCAL_N_CELL));
	    
	    _pedSum[mod][cell] += ampl;
	    _pedSumSquare[mod][cell] += pow(ampl,2);
	    _pedNum[mod][cell]++;
	    
	    _ped[mod][cell] = _pedSum[mod][cell]/_pedNum[mod][cell];
	    _pedError[mod][cell] = sqrt(_pedSumSquare[mod][cell]/_pedNum[mod][cell]-pow(_pedSum[mod][cell]/_pedNum[mod][cell],2));
	  }
	
	_pedCounter++;
	
	if(_skipPedestals != 0) 
	  { 
	    if(_throwSkipEventException == true) 
	      {
		throw marlin::SkipEventException(this);
	      } 
	    else 
	      {
		setReturnValue("Skip",true);
	      }
	    
	  }
	
	if((_skipStartUpPedestals !=0 ) && (_pedCounter < _minPedNumber)) 
	  {
	    
	    if(_throwSkipEventException == true) 
	      {
		throw marlin::SkipEventException(this);
	      } 
	    else 
	      {
		setReturnValue("Skip",true);
	      }
	    
	  }
      }
    
    /*debug statements*/
    for (unsigned short mod = 1; mod < 16 + 1; mod++)
      {
	for (unsigned short cell = 0; cell < 20; cell++)
	  {
	    streamlog_out(DEBUG)<<" mod="<<mod<<" cell="<<cell<<" pedSum="<<_pedSum[mod][cell]
			<<" pedSumSquare="<<_pedSumSquare[mod][cell]
			<<" pedNum="<<_pedNum[mod][cell]
			<<" ped="<<_ped[mod][cell]
			<<" pedError="<<_pedError[mod][cell]<<endl;
	  }
      }


    streamlog_out(DEBUG)<<" minPedNumber="<<_minPedNumber<<" pedCounter="<<_pedCounter<<endl;

    if (_pedCounter >= _minPedNumber) 
      {
	streamlog_out(DEBUG) << "-->applying pedestals in EVENT="<<evt->getEventNumber() << endl;
	
	LCCollectionVec *outCol = new LCCollectionVec(LCIO::RAWCALORIMETERHIT);
	
	std::string cellIDEncoding(HcalTileIndex::getEncodingString(0));
	CellIDEncoder<IMPL::RawCalorimeterHitImpl> CellIDEncoder(cellIDEncoding, outCol);

	unsigned int nhits = inCol->getNumberOfElements();
	for(unsigned int i = 0; i < nhits; ++i) 
	  {
	    LCObject const* pobj = inCol->getElementAt(i);
	    FastCaliceHit const* fhit = dynamic_cast<FastCaliceHit const*>( pobj );
	    TcmtHit const* thit = dynamic_cast<TcmtHit const*>( pobj );
	    unsigned short mod  = 0;
	    unsigned short cell = 0;
	    float ampl          = 0;
	    float amplErr       = 0;
	    
	    if(fhit) /*if HCAL hit (historically kept)*/
	      {
		mod = (fhit->getModuleID() & 0xFF00) >> 8;
		cell = fhit->getChip()*18 + fhit->getChannel();
		ampl = fhit->getEnergyValue();
		amplErr = sqrt(fhit->getEnergyError()*fhit->getEnergyError()+pow(_pedError[mod][cell],2));
	      }
	    else if(thit) /*if TCMT hit*/
	      {
		mod = (thit->getModuleID()>>8) & 0xff00;
		assert( mod == 0 );
		unsigned int cellKey = thit->getCellKey();
		mod = (cellKey >> 8) & 0xFF;
		cell = cellKey & 0xFF;
		ampl = thit->getEnergyValue();
		amplErr = sqrt(thit->getEnergyError()*thit->getEnergyError()+pow(_pedError[mod][cell],2));
	      }
	    
	    assert( (mod <= HCAL_N_MOD+1) && (cell <= HCAL_N_CELL) );
	    
	    streamlog_out(DEBUG) << "old hit: mod="
			 << mod << " cell=" << cell
			 << " ADC=" << ampl
			 <<" (+- "<<amplErr
			 <<") - ped="<< _ped[mod][cell]
			 << endl;
	    
	    // apply significance cut 
	    ampl -= _ped[mod][cell];

	    if (ampl < _significanceCut * amplErr) continue; 
	    
	    streamlog_out(DEBUG) << "new hit: ADC="<<ampl
			 <<" mod="<< mod<<" cell=" << cell<< endl;
	    
	    if(fhit) 
	      {
		unsigned chip = fhit->getChip();
		unsigned chan = fhit->getChannel();
		FastCaliceHit* newHit = new FastCaliceHit(mod, chip, chan,
							  ampl, amplErr, fhit->getTimeStamp());
		outCol->addElement(newHit);
	      }
	    else if(thit) 
	      {
		TcmtHit* newHit = new TcmtHit(thit->getCellID(),
					      ampl, amplErr, thit->getTimeStamp());
		outCol->addElement(newHit);
	      }
	  }
	evt->addCollection(outCol,_outputColName);
	
	setReturnValue("Skip",false);
	
      }
    
  }
  
  
}
