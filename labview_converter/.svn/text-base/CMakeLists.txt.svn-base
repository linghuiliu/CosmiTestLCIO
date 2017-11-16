###################################################
# project                                         #
###################################################
PROJECT( LABVIEW_CONV )


###################################################
# cmake                                           #
###################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.2 )
SET( CMAKE_MODULE_PATH ${LABVIEW_CONV_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )




#################################################
# project version                               #
#################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 0 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   0 )
   
# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
     "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
     "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )

###################################################
# subdirectories                                  #
###################################################
ADD_SUBDIRECTORY( raw2lcio )
