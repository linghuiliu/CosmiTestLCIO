# The deadAndNoisyTools depend on ROOT
FIND_PACKAGE( ROOT REQUIRED )
INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${ROOT_LIBRARIES} )
ADD_DEFINITIONS( ${ROOT_DEFINITIONS} )

# Require proper C++
ADD_DEFINITIONS( "${CMAKE_CXX_FLAGS}" )

# add install path to the rpath list
SET( CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH )
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# output directories
SET( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH
    "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH
     "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/include )

####################################
# The section for the root library #
####################################

INCLUDE( MacroRootDict )


PREPARE_ROOT_DICT_HEADERS( ${PROJECT_SOURCE_DIR}/include )

# all .cxx files are input
FILE(GLOB ${PROJECT_NAME}_CXX_FILES "*.cxx")

GEN_ROOT_DICT_SOURCES( ${${PROJECT_NAME}_CXX_FILES} )

#SET( ${PROJECT_NAME}_ROOT_DICT "${CMAKE_CURRENT_BINARY_DIR}/DeadAndNoisyToolsDict.cxx")

#MESSAGE( STATUS "before I do anything stupid: ${${PROJECT_NAME}_LIBRARY_NAME}")
SET(${PROJECT_NAME}_LIBRARY_NAME DeadAndNoisyTools)
#MESSAGE( STATUS "ok, is this better? : ${${PROJECT_NAME}_LIBRARY_NAME}")

ADD_LIBRARY(
   ${${PROJECT_NAME}_LIBRARY_NAME}
   SHARED
   ${${PROJECT_NAME}_CXX_FILES}
   ${ROOT_DICT_OUTPUT_SOURCES}
)

SET_TARGET_PROPERTIES( ${${PROJECT_NAME}_LIBRARY_NAME} PROPERTIES
		       VERSION ${${PROJECT_NAME}_VERSION}
                       SOVERSION ${${PROJECT_NAME}_SOVERSION}
)

INSTALL(
   TARGETS     ${${PROJECT_NAME}_LIBRARY_NAME}
   DESTINATION lib
   PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
               GROUP_READ GROUP_EXECUTE
               WORLD_READ WORLD_EXECUTE
)

##############################################################
# prepare for XXXXConfig.cmake                               #
#############################################################
GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${${PROJECT_NAME}_LIBRARY_NAME} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARYNAME )


####################################
# The section for the executables  #
####################################
# all .cpp files are input
FILE(GLOB ${PROJECT_NAME}_CPP_FILES "*.cpp")

# each cpp contains its own main(), make it a target
foreach (file ${${PROJECT_NAME}_CPP_FILES})
   # remove the .cpp and the path from the target name
   STRING (REPLACE ".cpp"  ""  _TARGET ${file})
   STRING (REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ ""  _TARGET ${_TARGET})

   MESSAGE( STATUS "New target ${_TARGET}" )

   # add the target, the dependendy on the shared object and schedule for installation
   ADD_EXECUTABLE(${_TARGET} ${file})
   TARGET_LINK_LIBRARIES(${_TARGET} ${${PROJECT_NAME}_LIBRARY_NAME} )
   INSTALL(
	TARGETS ${_TARGET} DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        	    GROUP_READ GROUP_EXECUTE
               	    WORLD_READ WORLD_EXECUTE )	    
endforeach (file)
