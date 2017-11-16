#############################################################
# cmake module for finding BOOST
#
# returns:
#   BOOST_FOUND        : set to TRUE or FALSE
#   BOOST_INCLUDE_DIRS : paths to BOOST includes
#   BOOST_LIBRARY_DIRS : paths to BOOST libraries
#   BOOST_LIBRARIES    : list of BOOST libraries
#
# @author Jan Engels, DESY
#############################################################
IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
 message( STATUS "Will 'Find' Boost by setting it to the afs install of ilcsoft v01-17 64bit")
 SET( BOOST_DIR "/afs/desy.de/project/ilcsoft/sw/x86_64_gcc41_sl5/v01-17/LCFIVertex/v00-06-01/include" )
ENDIF()


# ---------- includes ---------------------------------------------------------
SET( BOOST_INCLUDE_DIRS BOOST_INCLUDE_DIRS-NOTFOUND )
MARK_AS_ADVANCED( BOOST_INCLUDE_DIRS )

FIND_PATH( BOOST_INCLUDE_DIRS NAMES boost/foreach.hpp PATHS ${BOOST_DIR} NO_DEFAULT_PATH )
IF( NOT BOOST_DIR )
    FIND_PATH( BOOST_INCLUDE_DIRS NAMES boost/foreach.hpp )
ENDIF()

IF( BOOST_INCLUDE_DIRS )
    GET_FILENAME_COMPONENT( BOOST_ROOT ${BOOST_INCLUDE_DIRS} PATH )
    SET ( BOOST_FOUND TRUE )
    SET ( BOOST_LIBRARIES "" )
ENDIF()

MESSAGE(STATUS "BOOST_INCLUDE_DIRS=${BOOST_INCLUDE_DIRS}")

# ---------- libraries --------------------------------------------------------
#INCLUDE( MacroCheckPackageLibs )
#
# only standard libraries should be passed as arguments to CHECK_PACKAGE_LIBS
# additional components are set by cmake in variable PKG_FIND_COMPONENTS
# first argument should be the package name
#CHECK_PACKAGE_LIBS( BOOST mathlib kernlib )
#
#
#
# ---------- final checking ---------------------------------------------------
#INCLUDE( FindPackageHandleStandardArgs )
# set BOOST_FOUND to TRUE if all listed variables are TRUE and not empty
#FIND_PACKAGE_HANDLE_STANDARD_ARGS( BOOST DEFAULT_MSG BOOST_INCLUDE_DIRS BOOST_LIBRARIES )

