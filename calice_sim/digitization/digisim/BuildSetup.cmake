#############################################################################
# Example of a BuildSetup.cmake file for a Marlin Package
#
# For building with cmake type:
# (1) $ mkdir build
# (2) $ cd build
# (3) $ cmake -C ../BuildSetup.cmake ..
# (4) $ make install
#
# @author Jan Engels, DESY
#############################################################################


#############################################################################
# Setup path variables
#############################################################################

# ILC_HOME
SET( ILC_HOME "/k2dist/lib/ilc/"
    CACHE PATH "Path to ILC Software" FORCE )

# Path to Marlin
SET( Marlin_HOME "${ILC_HOME}/Marlin/v00-09-10"
    CACHE PATH "Path to Marlin" FORCE )

# Path to LCIO
SET( LCIO_HOME "${ILC_HOME}/lcio/v01-09"
    CACHE PATH "Path to LCIO" FORCE )

# Path to LCCD
SET( LCCD_HOME "${ILC_HOME}/lccd/v00-03-06"
    CACHE PATH "Path to LCCD" FORCE)

# Path to CondDB
SET( CondDBMySQL_HOME "${ILC_HOME}/CondDBMySQL/CondDBMySQL_ILC-0-5-10"
    CACHE PATH "Path to CondDBMySQL" FORCE)

# Path to GEAR
SET( GEAR_HOME "${ILC_HOME}/gear/v00-08"
    CACHE PATH "Path to GEAR" FORCE )

# Path to CLHEP
SET( CLHEP_HOME "${ILC_HOME}/CLHEP/2.0.3.1"
    CACHE PATH "Path to CLHEP" FORCE )

# Path to ROOT
SET( ROOT_HOME "${ILC_HOME}/root/v5.16.00"
    CACHE PATH "Path to ROOT" FORCE )

# CMake Modules Path
SET( CMAKE_MODULE_PATH "${ILC_HOME}/CMakeModules/v01-05"
    CACHE PATH "Path to CMake Modules" FORCE )


#############################################################################
# Optional packages
#############################################################################

# if you want to build and link your package with one or more additional
# tools you also have to define the corresponding "home" paths above
# if you uncomment the next 2 lines don't forget to change 'mymarlin' to your package name
#SET( BUILD_WITH "LCCD CondDBMySQL" 
#    CACHE STRING "Build DigiSim with these optional packages" FORCE )


#############################################################################
# Project options
#############################################################################

#SET( INSTALL_DOC OFF CACHE BOOL "Set to OFF to skip build/install Documentation" FORCE )

# set cmake build type, default value is: RelWithDebInfo
# possible options are: None Debug Release RelWithDebInfo MinSizeRel
#SET( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE )

#############################################################################
# Advanced options
#############################################################################

#SET( BUILD_SHARED_LIBS OFF CACHE BOOL "Set to OFF to build static libraries" FORCE )

# where do you want files (libraries, binaries, ...) to be placed for a "make install"
# if you uncomment the next line don't forget to change 'mymarlin' to your package name
#SET( CMAKE_INSTALL_PREFIX "/foo/bar" CACHE STRING "Where to install mymarlin" FORCE )
