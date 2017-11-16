#ifndef Mixture_h
#define Mixture_h 1

#include "marlin/Processor.h"
#include "marlin/EventModifier.h"
#include "lcio.h"
#include <string>

#include "IO/LCReader.h"
#include "IO/LCWriter.h"

using namespace lcio ;
using namespace marlin ;

/** Mixture processor reads a few files with CALICE data selected events
 * and joint them by suming-up all of them into one artificial event 
 * to use it in any reconstruction program.
 *
 * Mixture creates overlapped events from several especially prepared CALICE files 
 *
 * Processor creates two Calorimeter hit collections:
 *      MixtureECAL and MixtureHCAL
 *
 * List of input files should be restricted no more than 10 (0,1,2,3,4,5,6,7,8,9)
 *
 * Mixture processor copies all additional collections into ECAL0,ECAL1, ...,HCAL0,HCAL1, ...,
 * TCAL0,TCAL1, ..., PTRK0,PTRK1, ... to have all of them at the final stage of Analysis.
 * 
 * First particle that is in main LCIO-file does not shifted;
 * all additional particles that is in _inputFileNames
 * should have shifts of X and Y coordinates relatively to the first one.
 * Vector _XY_shifts
 * 
 *                      
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |          First aditional particle in Mixture    
 *                               |           (X1,Y1)                           
 *                               |                                             
 *            Second additional  |                                             
 *                     (X2,Y2)   |                                             
 *            particle           |                                             
 *                               |(0,0) X0,Y0 Zero particle in Mixture         
 *   ----------------------------|-------------------------                    
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                               |                                             
 *                                                                             
 *                                                                             
 *                                                                             
 *                                                                             
 *                                                                             
 *                                                                             
 *                                                                             
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * @author V.L. Morgunov, ITEP
 * @version $Id: Mixture.h,v 1.0 2009/04/02 20:22:50 morgunov Exp $
 * 
 * @param InputFileNames (StringVec) 
 */

class Mixture : public Processor, public EventModifier {
  
 public:

  virtual Processor*  newProcessor() {return new Mixture;}  
  Mixture() ;  
  virtual const std::string & name() const { return Processor::name() ; }

  virtual void modifyEvent(LCEvent* evt); 
  virtual void init();
  virtual void processRunHeader( LCRunHeader* run ) ;
  virtual void check(LCEvent* evt); 
  virtual void end();

 protected:

  /** Input file names.
   */
  StringVec _inputFileNames;
  int _numMixture;
  std::vector< LCReader* > _lcReaders;

  /** Input coordinate shifts for that particles.
   */
  std::vector<float>  _XY_shifts;
  int _numShifts;

  std::string _ECALHitsCol;
  std::string _HCALHitsCol;
  std::string _propCol;

  float ampl[200][200][100];

//     New coeefs from MOKKA; GeV to get MIP_MPV == 1 from lastests simulation
  float emip_vis;   
  float hmip_vis;   
  float ecal_coeff1;
  float ecal_coeff2;
  float ecal_coeff3;
  float hcal_coeff1;
  float hcal_coeff2;
  float tcal_coeff1;
  float tcal_coeff2; 

  int last_layer;
  float er_inner;
  float er_outer;
  float ez_inner;
  float ez_outer;
  float en_sampl;
  float esampling_1;
  float esampling_2;
  float esampling_3;

  int emin_lay;
  int emax_lay;
  float eabsorber;
  float ecell_size;

  float hr_inner;
  float hr_outer;
  float hz_inner;
  float hz_outer;
  float hn_sampl;
  float hsampling;
  int hmin_lay;
  int hmax_lay;
  float habsorber;
  float hcell_size;

  int _nRun ;
  int _nEvt ;
} ;

#endif



