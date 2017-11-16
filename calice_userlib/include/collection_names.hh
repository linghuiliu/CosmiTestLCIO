#ifndef collection_names_h 
#define collection_names_h 

// Some global definitions/typdefs 

// collection parameter which contains the daq record type
#define SUBRECORDTYPE               "SubRecordType"

// collection parameter which contains the daq record type
// DAQ ADC collection
#define COL_ADC                     "CALDAQ_ADCCol"
//Raw information of DHCAL
#define COL_DHCRAW                  "CALDAQ_DHCRaw" 
//Fe Header Information
//Attention these collection are missing the CALDAQ Prefix, heritage from earlier versions
#define COL_FE_HEAD                  "FeHeaderInfo"  //Header of each FE
#define COL_BOARD_HEAD               "BoardHeaderInfo" //Crc Board Header Info
#define COL_BOARD_TRAIL              "TrailerInfo" //Crc Board Trailer Info

// AHC FE configuration 
#define COL_FE_CONF                 "CALDAQ_FeConfiguration"
// EMC FE configuration 
#define COL_EMC_FE_CONF                 "CALDAQ_EmcFeConfiguration"
// AHC FE configuration 
#define COL_AHC_FE_CONF                 "CALDAQ_AhcFeConfiguration"
// AHC VFE configuration 
#define COL_AHC_VFE_CONF                 "CALDAQ_AhcVfeConfiguration"
// configuration number added to collection as parameter with this name
#define PAR_CONF_NR_IN_RUN          "CALDAQ_ConfNrInRun"
// AHC Slow ReadoutData (only table position!!!)
#define COL_AHC_SRO_DATA       "CALDAQ_AhcSroData"
// AHC Slow ReadoutData (temperatures, voltages etc.)
#define COL_AHC_SRO_MOD       "CALDAQ_AhcSroModData"
// SCE Slow Readout Data
#define COL_SCE_SRO_MOD       "CALDAQ_SceSroModData" 
// SCE Slow Temperature Data
#define COL_SCE_SRO_TEMP       "CALDAQ_SceSroTempData" 
// EMC Stage Data
#define COL_EMC_STAGE_DATA       "CALDAQ_EmcStageData"
// CrcReadout Configuration Data
#define COL_RO_CONF       "CALDAQ_ReadOutConfigurationData"
// DhcReadout Configuration Data
#define COL_DHCRO_CONF       "CALDAQ_DhcReadoutConfigurationData"
// TrgReadout Configuration Data
#define COL_TRGRO_CONF       "CALDAQ_TrgReadoutConfigurationData"
// Acquisition Start Data
#define COL_ACQ_START   "CALDAQ_AcquisitionStart"      
// Acquisition End Data
#define COL_ACQ_END   "CALDAQ_AcquisitionEnd"      
//Eventheader data (event records)
#define COL_EVENT_HEADER "CALDAQ_EventHeader"
//Eventheader data (trigger records)
#define COL_TRIGGER_HEADER "CALDAQ_TriggerEventHeader"
//Hodoscope Event data
#define COL_HOD_EVT_DATA "CALDAQ_HodoscopeEventData"
//DHC FE configuration data
#define COL_DHC_FE "CALDAQ_DhcFeConfiguration"


//reconstructed
#define COL_HITS "Calorimeter_hits"
#define COL_RAWHITS "RawCalorimeter_hits"

// -- drift chamber
//Caen TDCs as used at CERN 2006, 2007, 2010 (raw data)
//Obsolete as of 30/10/10 replace by generic CAEN TDC Collections, kept for backward compatibility 
#define COL_CAEN767TDC   "CALDAQ_Caen767TDCCol"
#define COL_CAEN767TDC_CONF "CALDAQ_Caen767Configuration"
#define COL_CAEN767TDC_ROCONF "CALDAQ_Caen767ReadoutConfiguration"
//Generic CAEN TDC Cols
#define COL_CAENTDC   "CALDAQ_CaenTDCCol"
#define COL_TDCSUP  "CALDAQ_TdcSupplementary"
#define COL_CAENTDC_ROCONF "CALDAQ_CaenReadoutConfiguration"
#define COL_CAENTDC_CONF "CALDAQ_CaenConfiguration"
//CAEN TDC Types
//Caen TDC as used at CERN 2006/07 
#define CAEN_767  "CAEN_767"
//Caen TDC as used at CERN 2010 
#define CAEN_1290  "CAEN_1290"

// raw (the LeCroy TDC used e.g for desy 2006 , in principle this should be integrated also into the/a generic tdc collection,
//see above, might create however a number of problems)
#define COL_TDC "CALDAQ_TdcCol"
// reconstructed
#define COL_DCTRACK                 "driftchamber_track"
#define COL_DCHITS                  "driftchamber_hits"
#define PAR_DCHIT_TIME              "time"
#define PAR_DCHIT_ID                "chamberID"

// -- event header parameter

// -- Trigger 
// status (obsolete)
#define PAR_TRIGGER_STATUS          "CALDAQ_TriggerStatus"
// assignment
//#define PAR_TRIGGER_TYPE_NAMES      "Trigger_Types"
#define PAR_TRIGGER_TYPE_NAMES      "TriggerTypes"
//From Goetz reco
#define PAR_TRIGGER_MASK            "TriggerMask"
#define PAR_TRIGGER_VALUE           "TriggerValue"
#define PAR_TRIGGER_DELAY           "TriggerDelay"
#define PAR_TRIGGER_JITTER          "TriggerJitter"
#define PAR_TRIGGER_ANDENABLE       "TriggerAndEnable"
#define PAR_TRIGGER_BITS            "TriggerBitPos"
#define COL_TRIGGER_ASSIGNMENT      "CALDAQ_TriggerAssignment"
#define COL_TRIGGER_CHECK           "CALDAQ_TriggerCheck"
//Parameters for Trigger Information
#define PAR_TRIGGER_MAIN_WORD "TriggerMainWord"
#define PAR_TRIGGER_POST_HISTORY "TriggerPostHist"
#define PAR_TRIGGER_PRE_HISTORY "TriggerPreHist"

#define PAR_MULTI_AMPL "MultiAmplitude"
#define PAR_MULTI_BIT "MultiBit"


// event data
#define COL_TRIGGER_EVENT           "CALDAQ_BeTrgEventData"
//trigger data contained in fe
#define COL_FETRG                   "CALDAQ_FeTrgData"
//trigger polling
#define COL_TRIGGER_POLL            "CALDAQ_BeTrgPollData"
// configuration
#define COL_TRIGGER_CONF            "CALDAQ_BeTrgConf"
//Trigger bit information as shipped with the event
#define PAR_TRIGGER_EVENT           "Event_Trigger"
//Trigger bit information as configured for a given part of a run 
#define PAR_TRIGGER_CONF            "Conf_Trigger"
//Be event information 
#define COL_BE_EVENT                 "CALDAQ_BeEventData"
//Fe event information 
#define COL_FE_EVENT                 "CALDAQ_FeEventData"
//Dif Trigger information (2nd DAQ Generation interfaced)
#define COL_DIFTRIGGER         "CALDAQ2_DifTrigger"

// -- reconstruction
// the state of the reconstruction for this event
// states are:
 enum ERecoState {kRecoStateUnknown,          /**< 0: unknown state.*/
                  kRecoStateSkipEvent,        /**< 1: events are skipped unproces
                                                      sed.*/
		  kRecoStateCosmics,          /**< 2: events are treated as cosmics 
                                                      (low, signals at random locations).*/
                  kRecoStateBeam,             /**< 3: events are treated as beam 
					              events (high, localised signals).*/
		  kRecoStatePedestals,        /**< 4: events are treated as
                                                      pedestal/noise events (no signals).*/
                  kRecoStateInitialPedestals, /**< 5: pedestals and noise are estimated 
                                                      assuming little signal at random locations.*/
                  kRecoStateRefinePedestals,  /**< 6: pedestal and noise estimates are improved 
                                                      assuming little signal at random 
                                                      locations.*/
                  kRecoStateCalibration,      /**< 7: There are  permanent (calibration) 
                                                      signals at specific locations .*/
                  kNRecoStates};              /**< 8: The number of considered states.*/

#define PAR_RECO_STATE "ReconstructionState"



// Error bits
#define PAR_ERROR_BITS              "CALDAQ_ErrorBits"

// event(record) type 
#define PAR_EVENT_TYPE              "CALDAQ_EventType"

// event numbers attached to the event header
#define PAR_EVENT_NR_IN_SPILL       "CALDAQ_EventNrInSpill"
#define PAR_EVENT_NR_IN_CONF        "CALDAQ_EventNrInConfiguration"

// -- run header parameter
// parameters of the run header
#define PAR_RUN_NUMBER              "CALDAQ_RunNumber"
#define PAR_RUN_TYPE                "CALDAQ_RunTypeName"
#define PAR_RUN_SUB_TYPE            "CALDAQ_RunSubType"
#define PAR_RUN_MAJ_TYPE            "CALDAQ_RunMajorType"
//Firmwar IDs of DAQ
#define PAR_RUN_BE_FIRM "CALDAQ_BeRunFirmwareID"
#define PAR_RUN_FE_FIRM "CALDAQ_FeRunFirmwareID"
#define PAR_RUN_BETRG_FIRM "CALDAQ_BeTrgFirmwareID"
#define PAR_RUN_VME_FIRM "CALDAQ_VmeFirmwareID"

// run conditions data
#define COL_RUN_INFO    "CALDAQ_RunInfo"
#define COL_RUNTIME_INFO    "CALDAQ_RunTimeInfo"
#define COL_RUN_SUM    "CALDAQ_RunSummary"


//General beamline info
#define COL_BML_RUNDATACERN    "CALDAQ_BmlSroRunDataCern"
//At Fnal bml data were read out at Acquisition start and Acquisition end
#define COL_BML_RUNDATAFNAL    "CALDAQ_BmlSroRunDataFnal"
#define COL_BML_ACQDATAFNAL    "CALDAQ_BmlSroAcqDataFnal"


//Some global definitions as needed by some classes

#define YEARSHIFT 16
#define MONTHSHIFT 8
#define DAYSHIFT 0

#define HOURSHIFT 16
#define MINUTESHIFT 8
#define SECONDSHIFT 0

#define TM_YEARSTART 1900


#endif
