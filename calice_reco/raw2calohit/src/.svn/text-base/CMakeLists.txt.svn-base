SET( lib1name raw2calohit )


###############################################
# include directories                         #
###############################################

INCLUDE_DIRECTORIES( BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include"
  BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/TBTrack"
  BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/TBTrackUtil"
  BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/gui"
  BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/histmgr"
  BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/utilities"   )

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS} 
                     ${Marlin_INCLUDE_DIRS}
                     ${CALICE_USERLIB_INCLUDE_DIRS})

IF( BUILD_WITH_USE_LCCD )
  INCLUDE_DIRECTORIES( ${LCCD_INCLUDE_DIRS} )
ENDIF( BUILD_WITH_USE_LCCD )

IF( BUILD_WITH_HAVE_ROOT )
  INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
ENDIF( BUILD_WITH_HAVE_ROOT )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  INCLUDE_DIRECTORIES( ${streamlog_INCLUDE_DIRS} )
ENDIF()

###############################################
# sources                                     #
###############################################
ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS}        )
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )
ADD_DEFINITIONS( -DDB_INIT_STRING=\"${DB_INIT_STRING}\" )

#collect all source files in current (source) directory
#AUX_SOURCE_DIRECTORY( . ${lib1name}_srcs_main )
#AUX_SOURCE_DIRECTORY( ./histmgr  ${lib1name}_srcs_histmgr )
#...
#SET( ${lib1name}_srcs ${${lib1name}_srcs_main} ${${lib1name}_srcs_histmgr} )


#alternatively give explicit list
#SET( ${lib1name}_srcs AdcValueAccess.cc
#                      AlignmentMatrix.cc
#                      AverageHistoryGraphs.cc
#                      CalibrateAndApplyThreshold.cc
#                      CalibrationFactory.cc
#                      CaliceEcalCalibration.cc
#                      CaliceTriggerProcessor.cc
#                      Clusteriser.cc
#                      ConstantCalibration.cc
#                      DriftChamberEfficiency.cc
#                      DriftChambertoTrack.cc
#                      FastGainCalibrationProcessor.cc
#                      FastInterCalibrationProcessor.cc
#                      FastMIPCalibrationProcessor.cc
#                      FastPedestalCalibrationProcessor.cc
#                      FastSaturationCalibrationProcessor.cc
#                      HcalModuleIndexReverseLookup.cc
#                      HoldScanAnalysis.cc
#                      IntegratedHcalCalibrationProcessor.cc
#                      IntegratedHcalProcessor.cc
#                      MakeGraph.cc
#                      MipSelect.cc
#                      ModuleIndexReverseLookup.cc
#                      NoOpCalibration.cc
#                      NoiseParameter.cc
#                      SquareFinder.cc
#                      PedestalNoiseHistograms.cc
#                      PedestalOnTheFlyProcessor.cc
#                      RawValueViewProcessor.cc
#                      RunInfoProcessor.cc
#                      MCRunTimeProcessor.cc
#                      SiPmPropertiesProcessor.cc
#                      SimpleHcalCalibrationProcessor.cc
#                      SimpleHitSearch.cc
#                      TBEcalDigitisation.cc
#                      TBTrack/TBTrackAligner.cc
#                      TBTrack/TBTrackBaseProcessor.cc
#                      TBTrack/TBTrackDbHandler.cc
#                      TBTrack/TBTrackDigitizer.cc
#                      TBTrack/TBTrackMapper.cc
#                      TBTrack/TBTrackMokkaCheck.cc
#                      TBTrack/TBTrackProducer.cc
#                      TBTrack/TBTrackProducerCheck.cc
#                      TBTrack/TBTrackRawCheck.cc
#                      TBTrack/TBTrackRemover.cc
#                      TBTrack/TBTrackScatter.cc
#                      TBTrack/TBTrackTdcHitsCheck.cc
#                      TBTrackUtil/AlnConstants.cc
#                      TBTrackUtil/FitConstants.cc
#                      TBTrackUtil/LinearFitResult.cc
#                      TBTrackUtil/LinearFitter.cc
#                      TBTrackUtil/MapConstants.cc
#                      TBTrackUtil/SimConstants.cc
#                      TBTrackUtil/TrackFinder.cc
#                      TBTrackUtil/TrackFitInitialisation.cc
#                      TBTrackUtil/TrackFitResult.cc
#                      TBTrackUtil/TrackFitter.cc
#                      TBTrackUtil/TrackProjection.cc
#                      TcmtMappingIIProcessor.cc
#                      TcmtMappingIProcessor.cc
#                      TcmtMipCalibrationProcessor.cc
#                      TriggerProcessorBase.cc
#                      VRawADCValueProcessor.cc
#                      VetoCounter.cc
#                      fastMappingIIProcessor.cc
#                      fastMappingIProcessor.cc
#                      gui/TriggerAnalysis.cc
#                      histmgr/FloatHistogram1D.cc
#                      histmgr/FloatHistogram2D.cc
#                      histmgr/GraphCollection_t.cc
#                      histmgr/HistMgr.cc
#                      histmgr/Histogram1D.cc
#                      histmgr/Histogram2D.cc
#                      histmgr/Histogram2DCollection_t.cc
#                      histmgr/HistogramCollection_t.cc
#                      histmgr/HistogramDisplay.cc
#                      histmgr/HistogramOutput.cc
#                      histmgr/Profile1D.cc
#                      histmgr/ProfileCollection_t.cc
#                      histmgr/RootWriter.cc
#                      histmgr/RootWriterKit.cc
#                      histogram2d.cc
#                      utilities/CollectionHistogramer.cc
#                      utilities/CollectionSelector.cc
#                      utilities/ProgressHandler.cc
#                     mappingIconditionsProcessor.cc )


# get list of all source files in this directory
AUX_SOURCE_DIRECTORY( . all_srcs )
AUX_SOURCE_DIRECTORY( ./histmgr all_srcs )
AUX_SOURCE_DIRECTORY( ./gui all_srcs )
AUX_SOURCE_DIRECTORY( ./TBTrack all_srcs )
AUX_SOURCE_DIRECTORY( ./TBTrackUtil all_srcs )
AUX_SOURCE_DIRECTORY( ./utilities all_srcs )

# remove "./" from the list of source files
STRING( REPLACE "./" "" all_srcs  "${all_srcs}" )

# library sources
SET( ${lib1name}_srcs ${all_srcs} )



######################################################
# what to build                                      #
######################################################
# require proper c++
SET_SOURCE_FILES_PROPERTIES(  ${${lib1name}_srcs} PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )

ADD_LIBRARY( ${lib1name} ${${lib1name}_srcs} )

SET_TARGET_PROPERTIES( ${lib1name} PROPERTIES
  VERSION ${${PROJECT_NAME}_VERSION}
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  # not needed at the moment (no build of
  # static and dynamic lib at the same time)
  # CLEAN_DIRECT_OUTPUT 1
  )

TARGET_LINK_LIBRARIES(${lib1name} ${LCIO_LIBRARIES}
${Marlin_LIBRARIES} ${CALICE_USERLIB_LIBRARIES})

IF( BUILD_WITH_USE_LCCD )
    TARGET_LINK_LIBRARIES(${lib1name} ${LCCD_LIBRARIES})
ENDIF( BUILD_WITH_USE_LCCD )

IF( BUILD_WITH_HAVE_ROOT )
   TARGET_LINK_LIBRARIES(${lib1name} ${ROOT_LIBRARIES})
ENDIF( BUILD_WITH_HAVE_ROOT )


######################################################
# install                                            #
######################################################
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/" DESTINATION ${INCLUDE_INSTALL_DIR}
  PATTERN "*~" EXCLUDE
  PATTERN "*CVS*" EXCLUDE 
  PATTERN "*SVN*" EXCLUDE )


INSTALL( TARGETS ${lib1name}
  DESTINATION ${LIB_INSTALL_DIR}
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE  )


#####################################################
# prepare for XXXXConfig.cmake                      #
#####################################################
GET_TARGET_PROPERTY(${Lib1name}_prefix ${lib1name} PREFIX)
GET_TARGET_PROPERTY(${Lib1name}_suffix ${lib1name} SUFFIX)
SET(lib_${lib1name}_name ${${Lib1name}_prefix}${Lib1name}${${Lib1name}_suffix})

GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${lib1name} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARY_NAME )


