SET(${PROJECT_NAME}_LIBRARY_NAME ${PROJECT_NAME})


#################################
# include directories           #
#################################
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

##INCLUDE_DIRECTORIES("$GOOGLEPERF_INCLUDE_DIRS")

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS}
                     ${Marlin_INCLUDE_DIRS}
                     ${LCCD_INCLUDE_DIRS}
                     ${CondDBMySQL_INCLUDE_DIRS}
                     ${ROOT_INCLUDE_DIRS}
                     ${CALICE_USERLIB_INCLUDE_DIRS}
                     ${RAW2CALOHIT_INCLUDE_DIRS}
                     ${RECOSIPM_INCLUDE_DIRS} )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  INCLUDE_DIRECTORIES( ${streamlog_INCLUDE_DIRS} )
ENDIF()


#################################
# project definitions           #
#################################
ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS}        )
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )

################################
# sources                      #
################################
#collect all source files in current (source) directory
AUX_SOURCE_DIRECTORY( . ${${PROJECT_NAME}_LIBRARY_NAME}_srcs )

#alternatively give explicit list
#SET( ${lib1name}_srcs <explicit file list> )

################################
# what to build                #
################################
# library (require proper c++ when compiling)
SET_SOURCE_FILES_PROPERTIES(  ${${${PROJECT_NAME}_LIBRARY_NAME}_srcs} PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )

ADD_LIBRARY( ${${PROJECT_NAME}_LIBRARY_NAME} SHARED ${${${PROJECT_NAME}_LIBRARY_NAME}_srcs} )

TARGET_LINK_LIBRARIES(${${PROJECT_NAME}_LIBRARY_NAME}
${LCIO_LIBRARIES} ${Marlin_LIBRARIES} ${LCCD_LIBRARIES}
${CondDBMySQL_LIBRARIES} ${ROOT_LIBRARIES} ${ROOT_SPECTRUM_LIBRARY} ${CALICE_USERLIB_LIBRARIES}
${RAW2CALOHIT_LIBRARIES} ${RECOSIPM_LIBRARIES} ${CELLGEOMETRY_LIBRARIES})


SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES
  VERSION ${${PROJECT_NAME}_VERSION}
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  # not needed at the moment (no build of
  # static and dynamic lib at the same time)
  # CLEAN_DIRECT_OUTPUT 1
  )

#binaries (eventually)
# ADD_CUSTOM_TARGET( ${BIN_INSTALL_DIR})
# ADD_EXECUTABLE( myexe myexe.cc)
#FOREACH( pkg ${packages})
#  IF( BUILD_WITH_USE_${pkg} )
#    TARGET_LINK_LIBRARIES( myexe ${${PROJECT_NAME}_LIBRARY_NAME ${${pkg}_LIBRARIES} )
#  ENDIF()
#ENDFOREACH()
# ADD_DEPENDENCIES( ${BIN_INSTALL_DIR} example)
# INSTALL(TARGETS myexe DESTINATION ${BIN_INSTALL_DIR})
#################################
# install                       #
#################################
#install library
INSTALL( TARGETS ${${PROJECT_NAME}_LIBRARY_NAME}
  DESTINATION ${LIB_INSTALL_DIR}
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE  )

#install header files
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/"
  DESTINATION ${INCLUDE_INSTALL_DIR}
  PATTERN "*~" EXCLUDE
  PATTERN "*CVS*" EXCLUDE )


################################
# prepare for XXXXConfig.cmake #
################################
GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${${PROJECT_NAME}_LIBRARY_NAME} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL
  "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARY_NAME )
