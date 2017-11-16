########################################################
# CMake compatibility issues: don't modify this, please!
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.2 )

MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
# allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
########################################################

########################################################
# User section: modify as needed
########################################################
# project name
PROJECT( RootTreeWriter )

###################################################
# standard ilcsoft modules
###################################################
FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )

SET (CMAKE_CXX_FLAGS "-std=c++0x -Wl,-no-undefined -pedantic -Wall")

###########################################
# Workaround for broken MacroCheckDeps.cmake
# ( Ships own version in source tree.
#   Remove if fixed upstream! )
###########################################
SET( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} ${CMAKE_MODULE_PATH} )


# project version
SET( ${PROJECT_NAME}_MAJOR_VERSION 2 )
SET( ${PROJECT_NAME}_MINOR_VERSION 5 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 2 )


### SETTINGS #################################################################

# project options
OPTION( BUILD_SHARED_LIBS "Set to OFF to build static libraries" ON )
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )
OPTION( ROOTTREEEWRITER_WITH_CLUSTERING "Set to OFF to build without clustering engine" ON )



################################################
# find packages (dependencies)                 #
################################################

FIND_PACKAGE( Marlin REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )

FIND_PACKAGE( LCCD REQUIRED )
IF( LCCD_FOUND )
  ADD_DEFINITIONS( "-DUSE_LCCD" )
ENDIF()
FIND_PACKAGE( CondDBMySQL REQUIRED )
IF( CondDBMySQL_FOUND )
  ADD_DEFINITIONS( "-DUSE_CONDDB" )
ENDIF()

FIND_PACKAGE( CALICE_USERLIB REQUIRED)

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    FIND_PACKAGE( streamlog REQUIRED)
ENDIF()

FIND_PACKAGE( ROOT REQUIRED )
FIND_PACKAGE( RAW2CALOHIT REQUIRED)
FIND_PACKAGE( MarlinUtil REQUIRED)

IF ( ROOTTREEEWRITER_WITH_CLUSTERING  )
    FIND_PACKAGE( CLUSTERING REQUIRED)
    ADD_DEFINITIONS( "-DRTW_USE_CLUSTERING" )
ENDIF(ROOTTREEEWRITER_WITH_CLUSTERING)

# set default cmake build type to RelWithDebInfo
# possible options are: None Debug Release RelWithDebInfo MinSizeRel
IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF(NOT CMAKE_BUILD_TYPE)

# set default install prefix to project root directory
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
    SET( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
ENDIF(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")


### INCLUDES #################################################################

#include directories

INCLUDE_DIRECTORIES( BEFORE "${PROJECT_SOURCE_DIR}/include" )
INCLUDE_DIRECTORIES( BEFORE "${PROJECT_SOURCE_DIR}/engines" )

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS}
                     ${Marlin_INCLUDE_DIRS}
                     ${ROOT_INCLUDE_DIRS}
                     ${CALICE_USERLIB_INCLUDE_DIRS} )

IF ( ROOTTREEEWRITER_WITH_CLUSTERING  )
  INCLUDE_DIRECTORIES( ${CLUSTERING_INCLUDE_DIRS}
                       ${MarlinUtil_INCLUDE_DIRS} )
ENDIF()

INCLUDE_DIRECTORIES( ${LCCD_INCLUDE_DIRS}
                     ${CondDBMySQL_INCLUDE_DIRS} )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  INCLUDE_DIRECTORIES( ${streamlog_INCLUDE_DIRS} )
ENDIF()

FILE(GLOB header_files "${PROJECT_SOURCE_DIR}/include/*.hh")
INSTALL(FILES ${header_files} DESTINATION ./include/${PROJECT_NAME})

### SOURCES ##################################################################

# require proper c++
ADD_DEFINITIONS( "${CMAKE_CXX_FLAGS}" )

# get list of all source files
AUX_SOURCE_DIRECTORY( src library_sources )
AUX_SOURCE_DIRECTORY( engines engine_sources )
SET( library_sources ${library_sources} ${engine_sources} )

### DOCUMENTATION ############################################################
FIND_PACKAGE( Doxygen )
IF( DOXYGEN_FOUND )

    ADD_CUSTOM_COMMAND(
        OUTPUT  "${PROJECT_SOURCE_DIR}/doc/html"
        COMMAND "${DOXYGEN_EXECUTABLE}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/doc"
        COMMENT "Building API Documentation..."
        VERBATIM )

    # add doc target
    ADD_CUSTOM_TARGET( doc DEPENDS
        "${PROJECT_SOURCE_DIR}/doc/html" )
ELSE()
    MESSAGE( STATUS "Doxygen not found in your system!!" )
    IF( INSTALL_DOC )
        MESSAGE( STATUS "INSTALL_DOC forced to OFF" )
        SET( INSTALL_DOC OFF )
    ENDIF()
ENDIF()

# install documentation
IF( INSTALL_DOC )
    # make sure doxygen is executed (make doc) before make install
    INSTALL( CODE "EXEC_PROGRAM(${CMAKE_BUILD_TOOL} ${PROJECT_BINARY_DIR} ARGS doc)" )
    # install documentation
    INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/doc/html" "${PROJECT_SOURCE_DIR}/doc/latex"
            DESTINATION "doc/${PROJECT_NAME}"
            PATTERN "*CVS*" EXCLUDE )
ENDIF()


##########################################################################################
# End of User section: please try not to modify below this line
##########################################################################################


# library *nix style versioning
SET( ${PROJECT_NAME}_SOVERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}" )
SET( ${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_SOVERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )

# add library install path to the rpath list
SET( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH )

# default destination for .cmake files: ${CMAKE_INSTALL_PREFIX}/cmake
SET( CONFIG_INSTALL_DIR "cmake"
  CACHE PATH "Directory to install the XXXConfig.cmake files" )
MARK_AS_ADVANCED( CONFIG_INSTALL_DIR )

# append link pathes to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# output directories
SET( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" CACHE PATH
    "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" CACHE PATH
    "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )


# require proper c++
SET_SOURCE_FILES_PROPERTIES( ${library_sources} PROPERTIES
                              COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )

# LIBRARY
ADD_LIBRARY( lib_${PROJECT_NAME} ${library_sources} )

# create symbolic lib target for calling target lib_XXX
ADD_CUSTOM_TARGET( lib DEPENDS lib_${PROJECT_NAME} )

TARGET_LINK_LIBRARIES(lib_${PROJECT_NAME} ${Marlin_LIBRARIES}
${LCIO_LIBRARIES} ${LCCD_LIBRARIES} ${CondDBMySQL_LIBRARIES}
${ROOT_LIBRARIES} ${RAW2CALOHIT_LIBRARIES} ${MarlinUtil_LIBRARIES} ${CALICE_USERLIB_LIBRARIES})

IF ( ROOTTREEEWRITER_WITH_CLUSTERING  )
   TARGET_LINK_LIBRARIES(lib_${PROJECT_NAME}  ${CLUSTERING_LIBRARIES})
ENDIF(ROOTTREEEWRITER_WITH_CLUSTERING)


# change lib_target properties
SET_TARGET_PROPERTIES( lib_${PROJECT_NAME} PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME ${PROJECT_NAME} )

# install library
INSTALL( TARGETS lib_${PROJECT_NAME} DESTINATION lib PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE )

# # create uninstall configuration file 
# CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
#                 "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
#                 IMMEDIATE @ONLY )
# 
# # create uninstall target
# ADD_CUSTOM_TARGET( uninstall
#   "${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake" )

# create configuration file from .in file
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY )

# install configuration file
INSTALL( FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" 
  DESTINATION ${CONFIG_INSTALL_DIR} )

# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "${PROJECT_NAME}_DEPENDS = \"${${PROJECT_NAME}_DEPENDS}\"" )
MESSAGE( STATUS "BUILD_WITH = \"${BUILD_WITH}\"" )
MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

# force some variables that could be defined in the command line
# to be written to cache
SET( BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}" CACHE BOOL
    "Set to OFF to build static libraries" FORCE )
SET( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
    "Where to install ${PROJECT_NAME}" FORCE )
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH
    "Path to custom CMake Modules" FORCE )
SET( INSTALL_DOC "${INSTALL_DOC}" CACHE BOOL
    "Set to OFF to skip build/install Documentation" FORCE )

# export build settings
INCLUDE( CMakeExportBuildSettings )
CMAKE_EXPORT_BUILD_SETTINGS( "${PROJECT_NAME}BuildSettings.cmake" )

# export library dependencies (keep this as the last line in the file)
EXPORT_LIBRARY_DEPENDENCIES( "${PROJECT_NAME}LibDeps.cmake" )

