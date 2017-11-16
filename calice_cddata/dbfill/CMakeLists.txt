#################################
# sources for library           #
#################################
SET( helper_lib_srcs
    collectionParameterHelper
    writeLCGenericObject
)

#################################
# executables                   #
#################################
SET( ${PROJECT_NAME}_executables
  browsefoldervert
  browseHoldVals
  browseInfoAt
  connmaptodb
  createBeamParameterData
  createdbfile
  createDetectorTransformation
  createEcalCalibrationConstants
  createEcalGeometryData
  createEcalMapping
  createExperimentalSetupSimple
  createHcalGeometryData
  createRunLocationData
  createRunTimeInfo
  createTcmtDescription
  createTriggerAssignment
  dbfolder2dbfile
  getRunTimeInfo
  hcalboards_conntodb
  hcalcassvscrctodb
  hcalmaptodb
  sipmmaptodb
  sipmvolcorrtodb
  tagdbfolder
  TBTrackDbFillAln
  TBTrackDbFillFit
  #           TBTrackDbFillFitNoMs
  TBTrackDbFillSim
  TBTrackDbInspector
  #           TBTrackDesy
  #           TBTrackScattering
  #           tiles_iteptodb  # problems with TilesItep
  triggerchecktodb
  triggerfromdb
  triggermaptodb
  writeHcalCalibration
  writeHcalProductionTable
  writeSimpleValues
  writeSimpleValuesVector
  writeLinearFitConstants
  writeLinearFitSlopes
  writeDeadCellMap
  writeRunLocation
  writeScECALGainfit
  writeScECALIntercalib
  writeScECALMIPfit
  writeScECALMapping
  writeScECALTemperature
  writeScECALNoisyChannel
  writeSaturationParameters
  writeAhc2HardwareConnection

  createEPTDescription
  createEPTModuleLocation
  createEPTModuleConnection

  createEPTDescription_TBAug2015
  createEPTModuleLocation_TBAug2015
  createEPTModuleConnection_TBAug2015

  createEPTDescription_TBMay2016
  createEPTModuleLocation_TBMay2016
  createEPTModuleConnection_TBMay2016

  createEPTDescription_TBJuly2016
  createEPTModuleLocation_TBJuly2016
  createEPTModuleConnection_TBJuly2016
)

# some programs cannot be compiled on MAC because of missing imlementation
# of clock_gettime() etc in Mac OS (10.6), add them to list of programs only
# if NOT compiling on Mac
IF( NOT APPLE )
  SET( ${PROJECT_NAME}_executables
    ${${PROJECT_NAME}_executables}
    createDetectorTransformationSimple
    createTcmtConnection
    createTcmtLocation
    createTcmtMIPCalibration
    createTcmtSaturationConstants
    dacvalcalc
    hcalboards_connfromdb
    hcalmapfromdb
    sipmmapfromdb
    )
ENDIF(NOT APPLE)


################################
# what to build                #
################################
# library (require proper c++ when compiling)
SET_SOURCE_FILES_PROPERTIES(  ${helper_lib_srcs} PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )

ADD_LIBRARY( helper_lib SHARED ${helper_lib_srcs} )
TARGET_LINK_LIBRARIES(   helper_lib
                         ${LCIO_LIBRARIES}
                         ${LCCD_LIBRARIES}
                         ${Marlin_LIBRARIES}
                         ${CALICE_USERLIB_LIBRARIES}
                         ${CondDBMySQL_LIBRARIES}
                         ${ROOT_LIBRARIES}
 )


ADD_LIBRARY( condDataCreators_lib SHARED ReadDaqMap/ReadDaqMap.cc
  ReadDaqMap/ReadLine.cc
  ReadDaqMap/TLineIterator.cc
  ReadDaqMap/parseDateTime.cc
  ReadDaqMap/getRunStartTime.cc
  RootTools/find.cc
  RootTools/TDirIter.cc
  createBeamParameterConddata.cc
  createDetectorTransformationConddata.cc
  createEcalCalibrationConstantsConddata.cc
  createEcalMappingConddata.cc
  createEcalModuleDescriptionConddata.cc
  createEcalModuleLocationConddata.cc
  createRunLocationConddata.cc
  createTriggerAssignmentConddata.cc
  )
TARGET_LINK_LIBRARIES(   condDataCreators_lib
                         ${LCIO_LIBRARIES}
                         ${LCCD_LIBRARIES}
                         ${Marlin_LIBRARIES}
                         ${CALICE_USERLIB_LIBRARIES}
                         ${CondDBMySQL_LIBRARIES}
                         ${ROOT_LIBRARIES}
 )


#################################
## TBTrackDbFillAln  
## TBTrackDbFillFit  
## TBTrackDbFillSim  
## TBTrackDbInspector
## need RAW2CALOHIT_LIBRARIES
#################################
FOREACH ( exec ${${PROJECT_NAME}_executables} )
  INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ./ ReadDaqMap )
  ADD_EXECUTABLE( ${exec} ${exec} )
  TARGET_LINK_LIBRARIES( ${exec} 
                         helper_lib 
                         condDataCreators_lib 
                         rt 
                         ${RAW2CALOHIT_LIBRARIES})
ENDFOREACH( exec ${${PROJECT_NAME}_executables} )

INSTALL(TARGETS ${${PROJECT_NAME}_executables}
  RUNTIME DESTINATION ${BIN_INSTALL_DIR})


MESSAGE( STATUS "INSTALLATION of ${${PROJECT_NAME}_executables} to ${BIN_INSTALL_DIR} " )


#################################
# install                       #
#################################
#install library
SET(${PROJECT_NAME}_libs_to_install  helper_lib
                                     condDataCreators_lib)

FOREACH( lib ${${PROJECT_NAME}_libs_to_install})
INSTALL( TARGETS ${lib}
         DESTINATION ${LIB_INSTALL_DIR}
	 PERMISSIONS
	 OWNER_READ OWNER_WRITE OWNER_EXECUTE
	 GROUP_READ GROUP_EXECUTE
	 WORLD_READ WORLD_EXECUTE  )

ENDFOREACH( lib ${${PROJECT_NAME}_libs_to_install})


