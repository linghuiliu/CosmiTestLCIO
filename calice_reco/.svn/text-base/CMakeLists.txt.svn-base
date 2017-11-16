PROJECT( CALICE_RECO )

#needed for using ctest
ENABLE_TESTING()
#to enable submission to Dart Testing Dashboard
INCLUDE(CTest)

###################################################
# cmake                                           #
###################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.2 )
SET( CMAKE_MODULE_PATH ${CALICE_RECO_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

##############################################
# modules/macros                             #
##############################################
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

###################################################
# options                                         #
###################################################
OPTION( BUILD_WITH_CLUSTERING "Build clustering" ON )

###################################################
# subdirectories                                  #
###################################################
ADD_SUBDIRECTORY( raw2calohit )


IF( BUILD_WITH_CLUSTERING )
  ADD_SUBDIRECTORY( clustering )
ENDIF (BUILD_WITH_CLUSTERING)

ADD_SUBDIRECTORY( cellGeometry )
ADD_SUBDIRECTORY( recoSiPM )

