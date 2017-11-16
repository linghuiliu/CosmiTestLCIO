##################################################
# Name of your project                           #
##################################################
PROJECT( ahc_map )

##################################################
# cmake options                                  #
##################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.6 )
MARK_AS_ADVANCED( CMAKE_BACKWARDS_COMPATIBILITY )

SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

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

##################################################
# modules/macros                                 #
##################################################
#add projects cmake directory to module path
SET( CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake 
     ${CMAKE_MODULE_PATH} )
#include useful macros
#macro BUILD_FLAG provides options "BUILD_WITH_<xyz>"
INCLUDE( BUILD_FLAG )
#macro FULL_INSTALL_PATH computes the full install path
#for given component 
INCLUDE( FULL_INSTALL_PATH )

##################################################
# project version                                #
##################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 0 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   1 )

# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION	       
    "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )


################################################
# project options                              #
################################################
#SET(CMAKE_BUILD_TYPE "Debug")
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF()

#use the packages you need
BUILD_FLAG( USE_Marlin "Use Marlin package" ON)
BUILD_FLAG( USE_LCIO   "Use LCIO package"   ON)
BUILD_FLAG( USE_LCCD   "Use LCCD package"   ON)
BUILD_FLAG( USE_ROOT   "Use ROOT package"   OFF)

BUILD_FLAG( USE_CALICE_USERLIB   "Use calice_userlib package"               ON)
BUILD_FLAG( USE_RAW2CALOHIT      "Use calice_reco/raw2calohit package"      OFF)
BUILD_FLAG( USE_RAW2LCIO         "User calice_lcioconv/raw2lcio package"    OFF)



###############################################
# installation options                        #
###############################################
# change default install prefix
IF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" 
         CACHE PATH "Install prefix" FORCE )
ENDIF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )

# default destination for header files: ${CMAKE_INSTALL_PREFIX}/include
SET( INCLUDE_INSTALL_DIR "include" CACHE PATH 
     "Directory to install the header files" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for library files: ${CMAKE_INSTALL_PREFIX}/lib
SET( LIB_INSTALL_DIR "lib" CACHE PATH 
     "Directory to install the libraries" )
MARK_AS_ADVANCED( LIB_INSTALL_DIR )

# default destination for executables: ${CMAKE_INSTALL_PREFIX}/bin
SET( BIN_INSTALL_DIR "bin" CACHE PATH 
     "Directory to install the executables" )
MARK_AS_ADVANCED( BIN_INSTALL_DIR )

# default destination for .cmake files: ${CMAKE_INSTALL_PREFIX}/cmake
SET( CONFIG_INSTALL_DIR "cmake" CACHE PATH 
      "Directory to install the XXXConfig.cmake files" )
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

###############################################
# add subdirectories                          #
###############################################
ADD_SUBDIRECTORY( src )



###############################################
# install configuration                       #
###############################################
#compute install path for bin, lib and include
#needed for XXXConfig.cmake.in
#FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
#FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )
#FULL_INSTALL_PATH( BIN     FULL_BIN_INSTALL_DIR )




##############################################
# print status report                        #
##############################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX               = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE                   = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "USE_Marlin                         = ${BUILD_WITH_USE_Marlin} ")
MESSAGE( STATUS "USE_LCIO                           = ${BUILD_WITH_USE_LCIO} ")
MESSAGE( STATUS "USE_LCCD                           = ${BUILD_WITH_USE_LCCD} ")
MESSAGE( STATUS "USE_ROOT                           = ${BUILD_WITH_USE_ROOT} ")
MESSAGE( STATUS "USE_CALICE_USERLIB                 = ${BUILD_WITH_USE_CALICE_USERLIB} ")
MESSAGE( STATUS "USE_RAW2CALOHIT                    = ${BUILD_WITH_USE_RAW2CALOHIT} ")
MESSAGE( STATUS "USE_RAW2LCIO                       = ${BUILD_WITH_USE_RAW2LCIO} ")
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------" )


