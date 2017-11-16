#ifndef RAW2LCIO_NAMESPACE_H
#define RAW2LCIO_NAMESPACE_H 1



/** \mainpage <a> CALICE USERLIB Documentation</a> (v06-00-03) 
  *  This package contains the interface classes need to analyse the lcio
  * files which are produced out<br>
  * of the calice native raw data. In addition it contains the data type
  * classes for the <br>
  * calice conditions data. The library being produced out of the sources
  * has to be<br>
  * linked to the analysis applications. <br>
  *  Versions of other packages to build the library:<br>
  *  <br>
  *  lcio v01-08 (or higher)<br>
  *  marlin v00-09-07 (or higher)<br>
  *  ---- In case database accesses (conditions data) are needed ---<br>
  *  lccd v00-03 (or higher, v00-03-05 recommended)<br>
  *  CondDBMySQLLisbon-0-5-6 (or higher, CondDBMySQL_ILC-0-5-10 recommended, available in calice cvs repository)<br>
  *  ---------------------------------------------------------------<br>
  *  <br>
  *  Links to older releases (links do currently not work, to be solved!!!!):<br>
  *  <br>
  *  <a href="http://www-flc.desy.de/hcal/simsoft/calice_soft/reco/v01-02/userlib/">v01-02</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/hcal/simsoft/calice_soft/reco/v02-00-pre3/userlib/">v02-00-pre3</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v03-05/">v03-05</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v03-05-01/">v03-05-01</a>  
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v03-06/">v03-06</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v03-06-01/">v03-06-01</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v03-07/">v03-07</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-01-01/">v04-01-01</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-02/">v04-02</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-03/">v04-03</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-03-01/">v04-03-01</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-04-02/">v04-04-02</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-04-03/">v04-04-03</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-04-04/">v04-04-04</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-04-05/">v04-04-05</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-05-01/">v04-05-01</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-05-02/">v04-05-02</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-05-03/">v04-05-03</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-06-00/">v04-06-00</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-06-01/">v04-06-01</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-06-02/">v04-06-02</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-07/">v04-07</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-07-01/">v04-07-01</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-07-02/">v04-07-02</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-08/">v04-08</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-09/">v04-09</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-10/">v04-10</a>
  *  <br>
  *  <a href="http://www-flc.desy.de/store/hcal/simsoft/calice_soft/userlib/v04-10-mc03/">v04-10-mc03</a>
  *  <br>
  *  <a font size="-1"><a> Changes implemented on 30/10/10 </a>
  *  <br>
  *  - BmlEventDataSup - New class for supplementary tdc information. <br>
  *  - BmlEventData - Access to BmlEventDataSup implemented. <br>
  *  - BmlCaenConfigurationBlock - Replaces BmlCaen767ConfigurationBlock on advent of Can 1290 in 2010. <br>
  *  - BmlCaenReadoutConfigurationBlock - typedef to existing BmlCaen767ReadoutConfigurationBlock (identical  for 767 and 1290 TDC). <br>
  *  - TriggerHandlerCalice - FeConfiguration changes include and minor bug fixes. <br>
  *  - collection_names - Generic Caen TDC collection names added plus a collection name for supplementary information. Names for handling 767 Caen TDC kept for backward compatibility.<br>
  *
  *  <a font size="-1"><a> Changes implemented on 7/12/10 </a>
  *  <br>
  *  - AcquisitionData - Dif counter implemented and restructuration after bug fixes in converter. <br>
  *  - collection_names - New collection COL_DIFTRIGGER.<br>
  *  - ErrorBits - New error bit kCorruptDifTrigCounter,  bool corruptDifTrigCounter() <br>
  *  <a font size="-1"><a> Changes implemented on 22/1/11 </a>
  *  <br>
  *  - DhcRawChipContent: New interface classe for dhcal raw data
  *  - collection_names - Collection names for dhc data added (raw and conditions data)
  *  <a font size="-1"><a> Changes implemented on 12/5/11 </a>
  *  <br>
  *  - DhcRawChipContent: returns now the VerSum
  *  - DhcReadoutConfBlock: New class which is the interface class to the 
DhcReadoutConfigurationData 
  *  - collection_names - Collection name COL_DHCRO_CONF for DhcReadoutconfigurationData 
  *  <a font size="-1"><a> Changes implemented on 03/06/11 </a>
  *  <br>
  *  - TriggerBits, TriggerHandlerCalice: Preparation for cern 2011 whcal running.
  */


/** The namespace CALICE contains calice specific software
  * needed for the  processing of CALICE data
  * including the interface classes to the CALICE Raw Data
  * and convenient MARLIN processors
  */
namespace CALICE{

  //Need to declare it here since we moved 
  //The conditions data writer to another namespace
  //Not nice ...
  class ConditionsDataWriteHandler;
}


/** The namespace CALICE contains calice specific software
  * needed for the processing of CALICE data
  * including the interface classes to the CALICE Raw Data
  * and convenient MARLIN processors
  */

/** These are <a href="http://ilcsoft.desy.de/portal/software_packages/marlin/index_eng.html">
MARLIN </a> Processors which are therefore added to the marlin
namespace  */
namespace marlin {}

#endif
