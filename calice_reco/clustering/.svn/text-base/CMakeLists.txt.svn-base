###############################################
# For any problem with cmake, see cmake help: #
#       cmake --help <cmake command>          #
# E.g.  cmake --help PROJECT                  #
###############################################
#set the name of the project
PROJECT( CLUSTERING )

###############################################
# cmake options                               #
###############################################
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF()

#Add projects cmake directory to module path
SET( CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

#Include special cmake macros for setting the right build flags
#and the right install paths
INCLUDE( BUILD_FLAG )
INCLUDE( FULL_INSTALL_PATH )

##################################################
# project version                                #
##################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 0 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   3 )

# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )


##################################################
# project options                                #
##################################################
# build shared library by default
OPTION( BUILD_SHARED_LIBS "Create shared libraries" ON )

BUILD_FLAG( USE_LCCD   "use lccd" ON ) #used in calice_userlib/include/MappingAndAlignment.hh



################################################
# find packages (dependencies)                 #
################################################

FIND_PACKAGE( Marlin REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )
FIND_PACKAGE( MarlinUtil REQUIRED )

IF(BUILD_WITH_USE_LCCD)
    FIND_PACKAGE( LCCD REQUIRED )
    FIND_PACKAGE( CondDBMySQL REQUIRED )
    IF( CondDBMySQL_FOUND )
      ADD_DEFINITIONS( "-DUSE_CONDDB" )
    ENDIF()
ENDif()

FIND_PACKAGE( CALICE_USERLIB REQUIRED)

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    FIND_PACKAGE( streamlog REQUIRED)
ENDIF()


################################################
# installation options                         #
################################################
# change default install prefix
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
  SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}"
    CACHE PATH "Install prefix" FORCE )
ENDIF( )

# default destination for header files: ${CMAKE_INSTALL_PREFIX}/include
SET( INCLUDE_INSTALL_DIR "include/${PROJECT_NAME}"
  CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for library files: ${CMAKE_INSTALL_PREFIX}/lib
SET( LIB_INSTALL_DIR "lib"
  CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( LIB_INSTALL_DIR )

# default destination for .cmake files: ${CMAKE_INSTALL_PREFIX}/cmake
SET( CONFIG_INSTALL_DIR "cmake"
  CACHE PATH "Directory to install the XXXConfig.cmake files" )
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

################################################
# documentation                                #
################################################

INCLUDE ( DOCUMENTATION )



################################################
# sudirs                                       #
################################################
ADD_SUBDIRECTORY( src )

################################################
# install configuration                        #
################################################
FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )

CONFIGURE_FILE( ${CLUSTERING_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${CLUSTERING_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake  @ONLY )

INSTALL( FILES ${CLUSTERING_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
  DESTINATION ${CONFIG_INSTALL_DIR} )



################################################
# output                                       #
################################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS                       = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX                    = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE                        = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "BUILD_WITH_USE_LCCD                     = ${BUILD_WITH_USE_LCCD} ")

MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )
