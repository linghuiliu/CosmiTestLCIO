PROJECT( CALICE_ANALYSIS )

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

###################################################
# standard ilcsoft cmake modules
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

#SET( CMAKE_MODULE_PATH ${CALICE_HH_PROC_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

ADD_SUBDIRECTORY( addonProcs )

#INCLUDE(ilcsoft_default_uninstall_target)