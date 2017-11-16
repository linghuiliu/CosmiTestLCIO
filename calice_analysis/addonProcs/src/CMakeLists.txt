SET( lib1name caliceAnalysis )

###############################################
# include directories                         #
###############################################
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS}
                     ${Marlin_INCLUDE_DIRS}
		     ${LCCD_INCLUDE_DIRS}
                     ${CondDBMySQL_INCLUDE_DIRS}
                     ${GEAR_INCLUDE_DIRS}
                     ${ROOT_INCLUDE_DIRS}
                     ${CLHEP_INCLUDE_DIRS}
                     ${CALICE_USERLIB_INCLUDE_DIRS}
		     ${CED_INCLUDE_DIRS}
		     ${RAW2CALOHIT_INCLUDE_DIRS}
		     ${CELLGEOMETRY_INCLUDE_DIRS}
		     ${RECOSIPM_INCLUDE_DIRS}
		     ${CLUSTERING_INCLUDE_DIRS}
		     ${MarlinUtil_INCLUDE_DIRS}
		     ${BOOST_INCLUDE_DIRS} )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  INCLUDE_DIRECTORIES( ${streamlog_INCLUDE_DIRS} )
ENDIF()

MESSAGE(STATUS "")
MESSAGE(STATUS "CED_INCLUDE_DIRS=${CED_INCLUDE_DIRS}")
MESSAGE(STATUS "CED_LIBRARIES=${CED_LIBRARIES}")
MESSAGE(STATUS "BOOST_INCLUDE_DIRS=${BOOST_INCLUDE_DIRS}")

###############################################
# sources                                     #
###############################################
ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS}        ) 
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )

#collect all source files in current (source) directory
AUX_SOURCE_DIRECTORY( . ${lib1name}_srcs )

###############################################
# what to build                               #
###############################################
# require proper c++
SET_SOURCE_FILES_PROPERTIES(  ${${lib1name}_srcs} PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )

ADD_LIBRARY( ${lib1name} ${${lib1name}_srcs} )

TARGET_LINK_LIBRARIES( ${lib1name} ${LCIO_LIBRARIES}
                       ${Marlin_LIBRARIES} ${LCCD_LIBRARIES} ${CondDBMySQL_LIBRARIES} ${MarlinUtil_LIBRARIES}
                       ${GEAR_LIBRARIES} ${ROOT_TMVA_LIBRARY} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES}
                       ${CALICE_USERLIB_LIBRARIES} ${CED_LIBRARIES}
                       ${RAW2CALOHIT_LIBRARIES} ${CLUSTERING_LIBRARIES}
                       ${CELLGEOMETRY_LIBRARIES} ${RECOSIPM_LIBRARIES})

SET_TARGET_PROPERTIES( ${lib1name} PROPERTIES
		       VERSION ${${PROJECT_NAME}_VERSION}
                       SOVERSION ${${PROJECT_NAME}_SOVERSION}
                        # not needed at the moment (no build of  
                        # static and dynamic lib at the same time)
                        # CLEAN_DIRECT_OUTPUT 1
                      )


ADD_EXECUTABLE( slimLCIOfile slimLCIOfile.cc )
FIND_PACKAGE( DD4hep )
IF ( DD4hep_FOUND )
TARGET_LINK_LIBRARIES( slimLCIOfile ${lib1name} ${ROOT_LIBRARIES} ${ROOT_TMVA_LIBRARY} ${DD4HEP_LIBRARIES})
MESSAGE(STATUS "DD4HEP_LIBRARIES=${DD4hep_LIBRARIES}")
ENDIF()
if ( NOT DD4hep_FOUND )
MESSAGE("DD4hep is not found with this ilcsoft version!")
TARGET_LINK_LIBRARIES( slimLCIOfile ${lib1name} ${ROOT_LIBRARIES} ${ROOT_TMVA_LIBRARY})
endif( NOT DD4hep_FOUND )

INSTALL(TARGETS slimLCIOfile RUNTIME DESTINATION ${BIN_INSTALL_DIR})

###############################################
# install                                     #
###############################################
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/" DESTINATION ${INCLUDE_INSTALL_DIR}
        PATTERN "*~" EXCLUDE
        PATTERN "*SVN*" EXCLUDE
        PATTERN "*CVS*" EXCLUDE )

INSTALL( TARGETS ${lib1name}
	 DESTINATION ${LIB_INSTALL_DIR}
	 PERMISSIONS
	 OWNER_READ OWNER_WRITE OWNER_EXECUTE
	 GROUP_READ GROUP_EXECUTE
	 WORLD_READ WORLD_EXECUTE  )


##############################################
# prepare for XXXXConfig.cmake               #
##############################################
GET_TARGET_PROPERTY(${Lib1name}_prefix ${lib1name} PREFIX)
GET_TARGET_PROPERTY(${Lib1name}_suffix ${lib1name} SUFFIX)
SET(lib_${lib1name}_name ${${Lib1name}_prefix}${Lib1name}${${Lib1name}_suffix})

GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${lib1name} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARY_NAME )


