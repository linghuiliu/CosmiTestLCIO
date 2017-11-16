SET(${PROJECT_NAME}_LIBRARY_NAME ${PROJECT_NAME})


#################################
# include directories           #
#################################
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )

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
#AUX_SOURCE_DIRECTORY( . ${${PROJECT_NAME}_LIBRARY_NAME}_srcs )

#alternatively give explicit list
SET( ${${PROJECT_NAME}_LIBRARY_NAME}_srcs
  Bezier.cc
  Fitlikelihood.cc
  Langaus.cc
  )

################################
# what to build                #
################################
# library (require proper c++ when compiling)


SET_SOURCE_FILES_PROPERTIES(  ${${${PROJECT_NAME}_LIBRARY_NAME}_srcs} PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )


ADD_LIBRARY( ${${PROJECT_NAME}_LIBRARY_NAME} SHARED ${${${PROJECT_NAME}_LIBRARY_NAME}_srcs} )
TARGET_LINK_LIBRARIES(  ${${PROJECT_NAME}_LIBRARY_NAME}  ${ROOT_LIBRARIES} )


SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES
  VERSION ${${PROJECT_NAME}_VERSION}
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  # not needed at the moment (no build of
  # static and dynamic lib at the same time)
  # CLEAN_DIRECT_OUTPUT 1
  )

MESSAGE(STATUS "")
#the RooFit is not enabled by default with ROOT, so we need to check
#if the corresponding library exists
FIND_LIBRARY(ROOTFITCORE_FOUND libRooFitCore.so ${ROOT_ROOT}/lib)
IF(NOT ROOTFITCORE_FOUND)
       MESSAGE(FATAL_ERROR "Library libFitCore.so not found")
ENDIF()

TARGET_LINK_LIBRARIES(${${PROJECT_NAME}_LIBRARY_NAME} ${ROOT_ROOT}/lib/libRooFit.so
  ${ROOT_ROOT}/lib/libRooFitCore.so
  ${ROOT_ROOT}/lib/libFoam.so
  ${ROOT_ROOT}/lib/libSpectrum.so )


#binaries (eventually)
#ADD_CUSTOM_TARGET( ${BIN_INSTALL_DIR})
ADD_EXECUTABLE( fitMIP fitMIP.cc)
TARGET_LINK_LIBRARIES( fitMIP ${${PROJECT_NAME}_LIBRARY_NAME}  )

INSTALL(TARGETS fitMIP
  RUNTIME DESTINATION ${BIN_INSTALL_DIR})


#ADD_DEPENDENCIES( ${BIN_INSTALL_DIR} example)
# INSTALL(TARGETS fitMIP DESTINATION ${BIN_INSTALL_DIR})
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

MESSAGE( STATUS "ROOT_ROOT     = ${ROOT_ROOT}" )
