PROJECT( FITMIP )

####################################################
# cmake options                                    #
####################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

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

####################################################
# modules/macros                                   #
####################################################
#add projects cmake directory to module path
SET( CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

#CALICE specific cmake macros
INCLUDE( BUILD_FLAG )
INCLUDE( FULL_INSTALL_PATH )


####################################################
# project version                                  #
####################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   0 )

# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
  "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )


###################################################
# options                                         #
###################################################
# build shared library by default
OPTION( BUILD_SHARED_LIBS "Create shared libraries" ON )

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


#####################################################
# find packages (dependencies)                      #
#####################################################

FIND_PACKAGE( ROOT REQUIRED )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    FIND_PACKAGE( streamlog REQUIRED)
ENDIF()

#####################################################
# installation options                              #
#####################################################
# change default install prefix
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
  SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )
ENDIF( CMAKE_INSTALL_PREFIX  STREQUAL "/usr/local" )

# default destination for header files: ${CMAKE_INSTALL_PREFIX}/install
SET( INCLUDE_INSTALL_DIR "include" CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for library files: ${CMAKE_INSTALL_PREFIX}/install
SET( LIB_INSTALL_DIR "lib" CACHE PATH "Directory to install the libraries" )
MARK_AS_ADVANCED( LIB_INSTALL_DIR )

# default destination for binary files: ${CMAKE_INSTALL_PREFIX}/install
SET( BIN_INSTALL_DIR "bin" CACHE PATH "Directory to install the executables" )
MARK_AS_ADVANCED( BIN_INSTALL_DIR )

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


##################################################
# subdirs                                        #
##################################################
ADD_SUBDIRECTORY( src )


##################################################
# install configuration                          #
##################################################
#compute install path for bin, lib and include

FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )

MESSAGE( STATUS "FULL_LIB_INSTALL_DIR     = ${FULL_LIB_INSTALL_DIR}" )
MESSAGE( STATUS "FULL_INCLUDE_INSTALL_DIR = ${FULL_INCLUDE_INSTALL_DIR} ")

CONFIGURE_FILE( ${${PROJECT_NAME}_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Config.cmake  @ONLY   )

INSTALL( FILES ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  DESTINATION ${CONFIG_INSTALL_DIR}                               )


##################################################
# documentation                                  #
##################################################
INCLUDE ( DOCUMENTATION )


##################################################
# print status report                            #
##################################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS    = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE     = ${CMAKE_BUILD_TYPE}" )

MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

