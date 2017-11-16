PROJECT( RAW2CALOHIT)

####################################################
# cmake options                                    #
####################################################
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

#add projects cmake directory to module path
SET( CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

#CALICE specific cmake macros
INCLUDE( BUILD_FLAG )
INCLUDE( FULL_INSTALL_PATH )


####################################################
# project version                                  #
####################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 6 )
SET( ${PROJECT_NAME}_MINOR_VERSION 8 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   1 )

# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )


###################################################
# options                                         #
###################################################
# build shared library by default
OPTION( BUILD_SHARED_LIBS "Create shared libraries" ON )

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

####################################################
## Macro BUILD_FLAG provides options "BUILD_WITH_<xyz>"
#todo: switch (some) of these on by default if build type is "Debug"

BUILD_FLAG( BOUNDARY_CHECK      "additional boundary check" )
BUILD_FLAG( DEBUG               "additional debug output" )
BUILD_FLAG( DEBUG_ALLOC         "additional debug output" )
BUILD_FLAG( DEBUG_RECO          "additional debug output" )
BUILD_FLAG( DEBUG_VERBOSE       "additional debug output" )
BUILD_FLAG( HCALDIGI_DEBUG      "additional debug output" )
BUILD_FLAG( HCALRECO_DEBUG      "additional debug output" )
BUILD_FLAG( RECO_DEBUG          "additional debug output" )
BUILD_FLAG( EXPORT_SIGNAL_TO_NOISE_RATIO "export signal to noise ratio (CalibrateAndApplyThreshold.hh)" OFF )
BUILD_FLAG( WITH_CELL_PAR_INDEX         "with cell parameter index (CellParameter.hh) "                 OFF )
BUILD_FLAG( WITH_CONTROL_HISTOGRAMS     "create control histograms "                                    OFF )

BUILD_FLAG( HAVE_CALICEGUI   "????????????? " OFF )
BUILD_FLAG( HAVE_PLOTMM      "????????????? " OFF )

BUILD_FLAG( USE_LCCD   "use lccd" ON )
BUILD_FLAG( HAVE_ROOT  "use ROOT" ON )

#####################################################
# find packages (dependencies)                      #
#####################################################

FIND_PACKAGE( Marlin REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )
FIND_PACKAGE( GEAR REQUIRED )

IF(BUILD_WITH_USE_LCCD)
    FIND_PACKAGE( LCCD REQUIRED )
    FIND_PACKAGE( CondDBMySQL REQUIRED )
    IF( CondDBMySQL_FOUND )
      ADD_DEFINITIONS( "-DUSE_CONDDB" )
    ENDIF()
ENDif()

IF ( BUILD_WITH_HAVE_ROOT )
    FIND_PACKAGE( ROOT REQUIRED)
ENDIF( BUILD_WITH_HAVE_ROOT )

FIND_PACKAGE( CALICE_USERLIB REQUIRED )

IF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  FIND_PACKAGE( streamlog REQUIRED )
ENDIF()



#####################################################
# installation options                              #
#####################################################
# change default install prefix
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
  SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )
ENDIF( CMAKE_INSTALL_PREFIX  STREQUAL "/usr/local" )

# default destination for header files: ${CMAKE_INSTALL_PREFIX}/install
SET( INCLUDE_INSTALL_DIR "include" CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for library files: ${CMAKE_INSTALL_PREFIX}/install
SET( LIB_INSTALL_DIR "lib" CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( LIB_INSTALL_DIR )

# default destination for .cmake files: ${CMAKE_INSTALL_PREFIX}/install
SET( CONFIG_INSTALL_DIR "cmake" CACHE PATH "Directory to install the XXXConfig.cmake files" )
MARK_AS_ADVANCED( CONFIG_INSTALL_DIR )

# append link pathes to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# provide nicer directory layout in build directory
SET( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH
  "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH
  "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )



###################################################
#  compile-time DB init string                    #
###################################################
IF( NOT DB_INIT_STRING )

  SET( DB_INIT_STRING "$ENV{DB_INIT_STRING}"
    CACHE PATH "DB init string from environment" FORCE )

  IF ( NOT DB_INIT_STRING )
    SET( DB_INIT_STRING "flccaldb01.desy.de:calice:caliceon:Delice.1:3306"
      CACHE PATH "default DB init string for read acceess" FORCE )
  ENDIF( NOT DB_INIT_STRING )
ENDIF( NOT DB_INIT_STRING )


##################################################
# subdirs                                        #
##################################################
ADD_SUBDIRECTORY( src )


##################################################
# install configuration                          #
##################################################
#compute install path for bin, lib and include

FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )

MESSAGE( STATUS "FULL_LIB_INSTALL_DIR     = ${FULL_LIB_INSTALL_DIR}" )
MESSAGE( STATUS "FULL_INCLUDE_INSTALL_DIR = ${FULL_INCLUDE_INSTALL_DIR} ")

CONFIGURE_FILE( ${RAW2CALOHIT_SOURCE_DIR}/cmake/RAW2CALOHITConfig.cmake.in
  ${RAW2CALOHIT_BINARY_DIR}/RAW2CALOHITConfig.cmake  @ONLY   )

INSTALL( FILES ${RAW2CALOHIT_BINARY_DIR}/RAW2CALOHITConfig.cmake
  DESTINATION ${CONFIG_INSTALL_DIR}                               )


##################################################
# documentation                                  #
##################################################
INCLUDE ( DOCUMENTATION )


##################################################
# print status report                            #
##################################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS                       = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX                    = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE                        = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "BUILD_WITH_BOUNDARY_CHECK               = ${BUILD_WITH_BOUNDARY_CHECK} ")
MESSAGE( STATUS "BUILD_WITH_USE_LCCD                     = ${BUILD_WITH_USE_LCCD} ")
MESSAGE( STATUS "LCIO_HOME                               = ${LCIO_HOME} ")
MESSAGE( STATUS "LCCD_HOME                               = ${LCCD_DIR} ")
MESSAGE( STATUS "Marlin_HOME                             = ${Marlin_HOME} ")
IF( BUILD_WITH_HAVE_ROOT )
  MESSAGE( STATUS "ROOT_HOME                               = ${ROOT_HOME} ")
ENDIF( BUILD_WITH_HAVE_ROOT )
MESSAGE( STATUS "DB_INIT_STRING                          = ${DB_INIT_STRING} ")

MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

