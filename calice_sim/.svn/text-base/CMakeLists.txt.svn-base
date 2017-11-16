PROJECT( CALICE_SIM )

#needed for using ctest
ENABLE_TESTING()
INCLUDE(CTest)

################################################
# cmake options                                #
################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.2 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

###################################################
# standard ilcsoft modules
###################################################
FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )

#######################################################
# c++11 and flags                                     #
#######################################################

INCLUDE( CheckCXXCompilerFlag )
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
SET (CMAKE_CXX_FLAGS "-std=c++11 -Wl,-no-undefined -pedantic -Wall")
ELSE()
MESSAGE( STATUS "Compiler does not support c++11." )
SET (CMAKE_CXX_FLAGS " -Wall -ansi")
ENDIF()

#################################################
# project version                               #
#################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 3 )
SET( ${PROJECT_NAME}_MINOR_VERSION 3 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   0 )

# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )


#################################################
# modules/macros                                #
#################################################
#add projects cmake directory to module path
SET( CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )
SET( CMAKE_MODULE_PATH ${CALICE_SIM_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )


#################################################
# options                                       #
#################################################
# build shared library by default
OPTION( BUILD_SHARED_LIBS "Create shared libraries" ON )

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#CALICE specific cmake macros
## Macro BUILD_FLAG provides options "BUILD_WITH_<xyz>"
INCLUDE( BUILD_FLAG )
INCLUDE( FULL_INSTALL_PATH )

BUILD_FLAG( HAVE_ROOT       "use ROOT" ON )
BUILD_FLAG( USE_LCCD        "use LCCD" ON )
BUILD_FLAG( HCALRECO_DEBUG  "additional debug output" )
BUILD_FLAG( HCALDIGI_DEBUG  "additional debug output" )


###################################################
# find packages (dependencies)                    #
###################################################

FIND_PACKAGE( CLHEP REQUIRED )
FIND_PACKAGE( Marlin REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )
FIND_PACKAGE( GEAR REQUIRED )

IF(BUILD_WITH_USE_LCCD)
    FIND_PACKAGE( LCCD REQUIRED )
    FIND_PACKAGE( CondDBMySQL REQUIRED )
ENDIF()

FIND_PACKAGE( CALICE_USERLIB REQUIRED)

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    FIND_PACKAGE( streamlog REQUIRED)
ENDIF()

IF(BUILD_WITH_HAVE_ROOT)
    FIND_PACKAGE( ROOT REQUIRED )
ENDIF()

FIND_PACKAGE( RAW2CALOHIT REQUIRED )
FIND_PACKAGE( RECOSIPM REQUIRED )
FIND_PACKAGE( CELLGEOMETRY REQUIRED )

###########################################################
# installation options                                    #
###########################################################
# change default install prefix
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
    SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )
ENDIF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )

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


###########################################################
# install configuration                                   #
###########################################################
FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )

MESSAGE( STATUS ${FULL_LIB_INSTALL_DIR} )
MESSAGE( STATUS ${FULL_INCLUDE_INSTALL_DIR} )

CONFIGURE_FILE( ${CALICE_SIM_SOURCE_DIR}/cmake/CALICE_SIMConfig.cmake.in
		${CALICE_SIM_BINARY_DIR}/CALICE_SIMConfig.cmake  @ONLY   )

INSTALL( FILES ${CALICE_SIM_BINARY_DIR}/CALICE_SIMConfig.cmake
	 DESTINATION ${CONFIG_INSTALL_DIR}                               )

IF( APPLE )
    SET( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
    MARK_AS_ADVANCED( CMAKE_INSTALL_NAME_DIR )
ENDIF( APPLE )


# INCLUDE(ilcsoft_default_uninstall_target)

###########################################################
# documentation                                           #
###########################################################
INCLUDE (DOCUMENTATION)

###########################################################
# print status report                                     #
###########################################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS                       = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX                    = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE                        = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "BUILD_WITH_HCALRECO_DEBUG               = ${BUILD_WITH_HCALRECO_DEBUG} ")
MESSAGE( STATUS "BUILD_WITH_USE_LCCD                     = ${BUILD_WITH_USE_LCCD} ")
MESSAGE( STATUS "BUILD_WITH_USE_LCCD                     = ${BUILD_WITH_HAVE_ROOT} ")
MESSAGE( STATUS "LCIO_HOME                               = ${LCIO_HOME} ")
MESSAGE( STATUS "LCCD_HOME                               = ${LCCD_HOME} ")
MESSAGE( STATUS "Marlin_HOME                             = ${Marlin_HOME} ")
MESSAGE( STATUS "GEAR_HOME                               = ${GEAR_HOME} ")
MESSAGE( STATUS "CLHEP_HOME                              = ${CLHEP_HOME} ")
MESSAGE( STATUS "ROOT_HOME                               = ${ROOT_HOME} ")
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )


###########################################################
# subdirectories                                          #
###########################################################
#leave me here, don't place me before setting of
#INCLUDE_INSTALL_DIR, etc
ADD_SUBDIRECTORY( digitization )
