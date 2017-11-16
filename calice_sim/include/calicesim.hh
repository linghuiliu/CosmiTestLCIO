#ifndef CALICE_SIM_NAMESPACE_H
#define CALICE_SIM_NAMESPACE_H 1



/** \mainpage <a> CALICE SIM Documentation</a> (v01-00) 
  *  This package contains classes to transform the simulated data of calice.
  *  into hits which can be handled by the classes in the calice reco package.. 
  *  This comprises cell ganging (for the analogue hcal and the tail catcher) 
  *  and the digitization step for the various sub detectors.   
  *  Dependencies on other packages: <br>
  *
  *  Note that this version contains the full cmake support.
  *
  *  lcio v01-09-nda<br>
  *  marlin v00-09-10 - including clhep dependency (version 2.0.2.3 or version 2.0.2.2) <br>
  *  gear v00-08<br>
  *  lccd v00-03-06<br>
  *  CondbMySql CondDBMySQL_ILC-0-5-10 (available in calice cvs repository)<br>
  *  calice userlib v04-10(-mc03)<br>
  *  calice reco v04-06(-mc02)<br>
  *  root package 5.12.00e or higher 
  *  <br>
  *  Links to older releases:<br>
  *  First release of this package
  *
  *  See http://www-flc.desy.de/flc/flcwiki/HCAL_Digitization and
  *  CALICE::AHCAL::Digitization::IntegratedHcalDigitizationProcessor
  *  for AHcal digitization documentation.
  *  
  */




/** The namespace CALICE contains calice specific software
  * needed for the processing of CALICE data
  * including the interface classes to the CALICE Raw Data
  * and convenient MARLIN processors
  */
namespace CALICE {}



#endif
