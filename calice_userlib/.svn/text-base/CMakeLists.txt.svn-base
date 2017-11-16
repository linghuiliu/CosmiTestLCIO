PROJECT( CALICE_USERLIB )

#needed for using ctest
ENABLE_TESTING()
INCLUDE(CTest)

#######################################################
# cmake options                                       #
#######################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

#As of CMake 2.6.0 the ELSE() and ENDIF() constructs can be empty
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

######################################################
# modules/macros                                     #
######################################################
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

SET( CMAKE_MODULE_PATH ${CALICE_USERLIB_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

#CALICE specific cmake macros
INCLUDE( BUILD_FLAG )
INCLUDE( FULL_INSTALL_PATH )


#####################################################
# project version                                   #
#####################################################
SET( ${PROJECT_NAME}_MAJOR_VERSION 6 )
SET( ${PROJECT_NAME}_MINOR_VERSION 6 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   1 )

# library Unix style versioning
SET( ${PROJECT_NAME}_SOVERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )

#####################################################
# options                                           #
#####################################################
# build shared library by default
OPTION( BUILD_SHARED_LIBS "Create shared libraries" ON )


#default build: optimisation and debug symbols (-O2 -g)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


####################################################
## Macro BUILD_FLAG provides options "BUILD_WITH_<xyz>"
#todo: switch (some) of these on by default if build type is "Debug"?
BUILD_FLAG( BOUNDARY_CHECK      "additional boundary check" )
BUILD_FLAG( VALUE_CHECK         "additional boundary check" )
BUILD_FLAG( CONV_DEBUG          "additional debug output for converter" )
BUILD_FLAG( RECO_DEBUG          "additional debug output for reco software" )
BUILD_FLAG( DEBUG_CONDDB_WRITER "additional debug output for conddb writer" )
BUILD_FLAG( USE_LCCD            "use lccd" ON )
BUILD_FLAG( HAVE_ROOT            "use root" ON )

#build examples. Option HAVE_ROOT only used in examples
OPTION( BUILD_EXAMPLES "build examples" OFF )

#These definitions change the library API and must be exported to other projects
BUILD_FLAG( DEPRECATED                   "use deprecated API"          OFF ${PROJECT_NAME}_EXPORT_DEFINITIONS WARN )
BUILD_FLAG( TRIGGER_HANDLER_IS_SINGLETON "TriggerHandler is singleton" ON  ${PROJECT_NAME}_EXPORT_DEFINITIONS WARN )

#write API changing definitions to the cache
SET( ${PROJECT_NAME}_EXPORT_DEFINITIONS ${${PROJECT_NAME}_EXPORT_DEFINITIONS} CACHE STRING "definitions to export" FORCE )
MARK_AS_ADVANCED( ${PROJECT_NAME}_EXPORT_DEFINITIONS )

#add API changing definitions to assure usage
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )

##################################################################
# 32/64 bit issues                                               #
##################################################################
IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    MESSAGE( STATUS "32 bit architecture detected" )
ENDIF()

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( STATUS "64 bit architecture detected" )

ENDIF()


##################################################################
# find packages (dependencies)                                   #
##################################################################

FIND_PACKAGE( Marlin REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )

IF(BUILD_WITH_USE_LCCD)
    FIND_PACKAGE( LCCD REQUIRED )
    FIND_PACKAGE( CondDBMySQL REQUIRED )
    IF( CondDBMySQL_FOUND )
      ADD_DEFINITIONS( "-DUSE_CONDDB" )
    ENDIF()
ENDIF()

IF ( BUILD_WITH_HAVE_ROOT )
  FIND_PACKAGE( ROOT REQUIRED)
ENDIF( BUILD_WITH_HAVE_ROOT )

IF ( APPLE )
  SET( ${PROJECT_NAME}_DEPENDS "${${PROJECT_NAME}_DEPENDS} streamlog" )
ENDIF( APPLE )



#################################################################
# installation options                                          #
#################################################################
# change default install prefix
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
    SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )
ENDIF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )

# default destination for header files: ${CMAKE_INSTALL_PREFIX}/install
SET( INCLUDE_INSTALL_DIR "include" CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for executables: ${CMAKE_INSTALL_PREFIX}/install
SET( BIN_INSTALL_DIR "bin" CACHE PATH "Directory to install the executables" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for library files: ${CMAKE_INSTALL_PREFIX}/install 
SET( LIB_INSTALL_DIR "lib" CACHE PATH "Directory to install the libraries" )
MARK_AS_ADVANCED( LIB_INSTALL_DIR )

# default destination for .cmake files: ${CMAKE_INSTALL_PREFIX}/install 
SET( CONFIG_INSTALL_DIR "cmake" CACHE PATH "Directory to install the XXXConfig.cmake files" )
MARK_AS_ADVANCED( CONFIG_INSTALL_DIR )

# provide nicer directory layout in build directory
SET( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH
    "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH
    "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )


##################################################################
# general build options                                          #
##################################################################
# append link paths to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )


##################################################################
# subdirectories                                                 #
##################################################################
ADD_SUBDIRECTORY( src )

if ( BUILD_EXAMPLES )
  ADD_SUBDIRECTORY( examples )
endif( BUILD_EXAMPLES )


##################################################################
# install configuration                                          #
##################################################################
#compute install path for bin, lib and include
FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )

MESSAGE( STATUS ${FULL_LIB_INSTALL_DIR} )
MESSAGE( STATUS ${FULL_INCLUDE_INSTALL_DIR} )

CONFIGURE_FILE( ${CALICE_USERLIB_SOURCE_DIR}/cmake/CALICE_USERLIBConfig.cmake.in
		${CALICE_USERLIB_BINARY_DIR}/CALICE_USERLIBConfig.cmake  @ONLY   )

INSTALL( FILES ${CALICE_USERLIB_BINARY_DIR}/CALICE_USERLIBConfig.cmake
	 DESTINATION ${CONFIG_INSTALL_DIR}                               )

IF( APPLE )
    SET( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
    MARK_AS_ADVANCED( CMAKE_INSTALL_NAME_DIR )
ENDIF( APPLE )

#todo: provide uninstall (see cmake wiki)
#INCLUDE(ilcsoft_default_uninstall_target)

#################################################################
#  compile-time DB init string                                  #
#################################################################
IF( NOT DB_INIT_STRING )
  SET( DB_INIT_STRING "$ENV{DB_INIT_STRING}" 
       CACHE PATH "DB init string from environment" FORCE )

  IF ( NOT DB_INIT_STRING )
    SET( DB_INIT_STRING "flccaldb01.desy.de:calice:caliceon:Delice.1:3306" 
         CACHE PATH "default DB init string for read acceess" FORCE )
  ENDIF( NOT DB_INIT_STRING )
ENDIF( NOT DB_INIT_STRING )


################################################################
# documentation                                                #
################################################################
INCLUDE( DOCUMENTATION )

################################################################
# print status report                                          #
################################################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS                       = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX                    = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE                        = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "BUILD_WITH_BOUNDARY_CHECK               = ${BUILD_WITH_BOUNDARY_CHECK} ")
MESSAGE( STATUS "BUILD_WITH_VALUE_CHECK                  = ${BUILD_WITH_VALUE_CHECK} ")
MESSAGE( STATUS "BUILD_WITH_CONV_DEBUG                   = ${BUILD_WITH_CONV_DEBUG} ")
MESSAGE( STATUS "BUILD_WITH_RECO_DEBUG                   = ${BUILD_WITH_RECO_DEBUG} ")
MESSAGE( STATUS "BUILD_WITH_DEBUG_CONDDB_WRITER          = ${BUILD_WITH_DEBUG_CONDDB_WRITER} ")
MESSAGE( STATUS "BUILD_WITH_USE_LCCD                     = ${BUILD_WITH_USE_LCCD} ")
MESSAGE( STATUS "BUILD_WITH_HAVE_ROOT                    = ${BUILD_WITH_HAVE_ROOT} ")

IF( BUILD_WITH_USE_LCCD )
  MESSAGE( STATUS "LCCD_HOME                            = ${LCCD_HOME} ")
  MESSAGE( STATUS "Marlin_HOME                          = ${Marlin_HOME} ")
ENDIF( BUILD_WITH_USE_LCCD )

MESSAGE( STATUS "BUILD_EXAMPLES                          = ${BUILD_EXAMPLES} ") 
IF( BUILD_EXAMPLES )
  MESSAGE( STATUS "BUILD_WITH_HAVE_ROOT                 = ${BUILD_WITH_HAVE_ROOT} ")
ENDIF( BUILD_EXAMPLES )

MESSAGE( STATUS "BUILD_WITH_DEPRECATED                   = ${BUILD_WITH_DEPRECATED} ")
MESSAGE( STATUS "BUILD_WITH_TRIGGER_HANDLER_IS_SINGLETON = ${BUILD_WITH_TRIGGER_HANDLER_IS_SINGLETON} ")

MESSAGE( STATUS "LCIO_HOME                               = ${LCIO_HOME} ")
MESSAGE( STATUS "LCCD_DIR                                = ${LCCD_DIR} ")
MESSAGE( STATUS "Marlin_HOME                             = ${Marlin_HOME} ")

IF( BUILD_WITH_HAVE_ROOT )
  MESSAGE( STATUS "ROOT_HOME                               = ${ROOT_HOME} ")
ENDIF( BUILD_WITH_HAVE_ROOT )
MESSAGE( STATUS "DB_INIT_STRING                          = ${DB_INIT_STRING} ")

MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )

