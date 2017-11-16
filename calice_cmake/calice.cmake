## cmake configuration file for CALICE packages
##
## Note: you need to install the latest ILC software
## which is compatible with the CALICE software.
##
## Don't forget to change the paths below according
## to your system
################################################

# calice code installation directory

SET( CALICE_HOME 
     "/directorypath/calice"
     CACHE PATH "Path to CALICE Software" FORCE )
MARK_AS_ADVANCED( CALICE_HOME )

# calice cmake module directory

SET( CMAKE_MODULE_PATH 
     "${CALICE_HOME}/calice_cmake/v01-01/"
     CACHE PATH "Path to CMake Modules" FORCE )

#################################################
## Set the installation prefix, 
## i.e. the directory where you want your libraries
## and binaries to be installed.
#################################################
#SET( CMAKE_INSTALL_PREFIX "/directorypath/calice/install"
#     CACHE PATH "Install prefix" FORCE)
#
#################################################
##
## CALICE packages
## 
#################################################
SET( CALICE_USERLIB_HOME "${CALICE_HOME}/calice_userlib/trunk/cmake/"
     CACHE PATH "Path to calice_userlib" FORCE )

SET( CALICE_RECO_HOME "${CALICE_HOME}/calice_reco/trunk/cmake/"
     CACHE PATH "Path to calice_reco" FORCE )

SET( RAW2CALOHIT_HOME "${CALICE_HOME}/calice_reco/trunk/raw2calohit/cmake/"
     CACHE PATH "Path to calice_reco/raw2calohit" FORCE )

SET( CLUSTERING_HOME "${CALICE_HOME}/calice_reco/trunk/clustering/cmake"
     CACHE PATH "Path to calice_reco/clustering" FORCE )



