PROJECT( calice_cddata )

#######################################################
# cmake options                                       #
#######################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

#As of CMake 2.6.0 the ELSE() and ENDIF() constructs can be empty
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

##############################################
# modules/macros                             #
##############################################
#add projects cmake directory to module path
FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )
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

SET( CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

#CALICE specific cmake macros
INCLUDE( BUILD_FLAG )
INCLUDE( FULL_INSTALL_PATH )

#####################################################
# project version                                   #
#####################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 5 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   1 )

# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )


#############################################
# options                                   #
#############################################
# build shared library by default
OPTION( BUILD_SHARED_LIBS "Create shared libraries" ON )

#default build: optimisation and debug symbols (-O2 -g)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#############################################
# required packages (dependencies)          #
#############################################

FIND_PACKAGE( LCIO REQUIRED )
FIND_PACKAGE( Marlin REQUIRED )
FIND_PACKAGE( LCCD REQUIRED )
IF( LCCD_FOUND )
  ADD_DEFINITIONS( "-DUSE_LCCD" )
ENDIF()
FIND_PACKAGE( CondDBMySQL REQUIRED )
IF( CondDBMySQL_FOUND )
  ADD_DEFINITIONS( "-DUSE_CONDDB" )
ENDIF()
FIND_PACKAGE( CALICE_USERLIB REQUIRED)
FIND_PACKAGE( RAW2CALOHIT REQUIRED)
FIND_PACKAGE( ROOT REQUIRED )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    FIND_PACKAGE( streamlog REQUIRED)
ENDIF()

############################################
# installation options                     #
############################################
# change default install prefix
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
  SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )
ENDIF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )

# default destination for header files: ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
SET( INCLUDE_INSTALL_DIR "include" CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for executables: ${CMAKE_INSTALL_PREFIX}/bin
SET( BIN_INSTALL_DIR "bin" CACHE PATH "Directory to install the executables" )
MARK_AS_ADVANCED( BIN_INSTALL_DIR )

# default destination for library files: ${CMAKE_INSTALL_PREFIX}/lib
SET( LIB_INSTALL_DIR "lib" CACHE PATH "Directory to install the libraries" )
MARK_AS_ADVANCED( LIB_INSTALL_DIR )

# default destination for .cmake files: ${CMAKE_INSTALL_PREFIX}/cmake
SET( CONFIG_INSTALL_DIR "cmake" CACHE PATH "Directory to install the XXXConfig.cmake files" )
MARK_AS_ADVANCED( CONFIG_INSTALL_DIR )

#####################
# include directories
#####################

INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )
INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${LCCD_INCLUDE_DIRS} ${LCCD_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS} ${LCIO_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CondDBMySQL_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${CALICE_USERLIB_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${RAW2CALOHIT_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${RAW2CALOHIT_INCLUDE_DIRS}/TBTrack )
INCLUDE_DIRECTORIES( ${RAW2CALOHIT_INCLUDE_DIRS}/TBTrackUtil )
MESSAGE( "LCIO_INCLUDE_DIR=${LCIO_INCLUDE_DIR}" )
MESSAGE( "LCIO_INCLUDE_DIRS=${LCIO_INCLUDE_DIRS}" )
MESSAGE( "LCCD_INCLUDE_DIR=${LCCD_INCLUDE_DIR}" )
MESSAGE( "LCCD_INCLUDE_DIRS=${LCCD_INCLUDE_DIRS}" )
MESSAGE( "CALICE_USERLIB_INCLUDE_DIRS=${CALICE_USERLIB_INCLUDE_DIRS}" )

####################
# sources
####################

# full path for link libraries in executables
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# provide nicer directory layout in build directory
SET( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH
  "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH
  "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )

##################################################
# subdirs                                        #
##################################################
ADD_SUBDIRECTORY( dbfill )
ADD_SUBDIRECTORY( ahc_map )

##############################################
# general build options                      #
##############################################
# append link paths to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )


##############################################
# install configuration                      #
##############################################
#compute install path for bin, lib and include
FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )

MESSAGE( STATUS ${FULL_LIB_INSTALL_DIR} )
MESSAGE( STATUS ${FULL_INCLUDE_INSTALL_DIR} )

#CONFIGURE_FILE( ${${PROJECT_NAME}_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
#  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Config.cmake  @ONLY   )

#INSTALL( FILES ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Config.cmake
#  DESTINATION ${CONFIG_INSTALL_DIR}                               )

IF( APPLE )
  SET( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
  MARK_AS_ADVANCED( CMAKE_INSTALL_NAME_DIR )
ENDIF( APPLE )


#INCLUDE(ilcsoft_default_uninstall_target)

#############################################
# documentation                             #
#############################################
#file doc/Doxyfile.in exist, but expects ${source}/include directory, which does not exist
#INCLUDE( DOCUMENTATION )

#############################################
# print status report                       #
#############################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS                       = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX                    = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE                        = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "LCIO_HOME                               = ${LCIO_HOME} ")
MESSAGE( STATUS "LCCD_HOME                               = ${LCCD_HOME} ")
MESSAGE( STATUS "LCCD_DIR                                = ${LCCD_DIR} ")
MESSAGE( STATUS "                                                                               " )
MESSAGE( STATUS "USED DEFINITIONS                        = ${${PROJECT_NAME}_USED_DEFINITIONS}  " )
MESSAGE( STATUS "                                                                               " )

MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )
