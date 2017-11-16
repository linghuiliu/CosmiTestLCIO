SET(libname ${PROJECT_NAME})

#################################
# find upstream packages        #
#################################
SET(packages Marlin
             LCIO
	     LCCD
	     ROOT
	     CALICE_USERLIB
	     RAW2CALOHIT
	     RAW2LCIO
	     )

FOREACH( pkg ${packages})
  IF( BUILD_WITH_USE_${pkg} )
    FIND_PACKAGE( ${pkg} REQUIRED)
  ENDIF()
ENDFOREACH()

#################################
# include directories           #
#################################
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

FOREACH( pkg ${packages})
  IF( BUILD_WITH_USE_${pkg} )
    INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} ${${pkg}_INCLUDE_DIR})
  ENDIF()
ENDFOREACH()


#################################
# project definitions           #
#################################
ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS}        ) 
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )

FOREACH( pkg ${packages} )
  IF( BUILD_WITH_USE_${pkg} )
    ADD_DEFINITIONS( ${${pkg}_DEFINITIONS} )
  ENDIF()
ENDFOREACH()

################################
# sources                      #
################################
SET( ${libname}_srcs  AHCReader.cc
		      AHCHcalEntry.cc AHCTcmtEntry.cc 
		      AHCPinEntry.cc AHCPmtEntry.cc
		      HcalSetup.cc
)

SET( ${PROJECT_NAME}_execs createHcalMapping
)

#or, alternatively, you can do:
# AUX_SOURCE_DIRECTORY( . ${libname}_srcs)

################################
# what to build                #
################################
# library (require proper c++ when compiling)
SET_SOURCE_FILES_PROPERTIES(  ${${libname}_srcs} PROPERTIES
                              COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )

ADD_LIBRARY( ${libname} SHARED ${${libname}_srcs} )

SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES
		       VERSION ${${PROJECT_NAME}_VERSION}
                       SOVERSION ${${PROJECT_NAME}_SOVERSION}
                        # not needed at the moment (no build of  
                        # static and dynamic lib at the same time)
                        # CLEAN_DIRECT_OUTPUT 1
                      )
# link against 
FOREACH( pkg ${packages} )
  IF( BUILD_WITH_USE_${pkg} )
    TARGET_LINK_LIBRARIES( ${libname} ${${pkg}_LIBRARIES} )
  ENDIF()
ENDFOREACH()

#binaries
ADD_CUSTOM_TARGET( ${BIN_INSTALL_DIR} )
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
FOREACH( exec ${${PROJECT_NAME}_execs} )
  SET_SOURCE_FILES_PROPERTIES( ${exec}.cc PROPERTIES
			      COMPILE_FLAGS "-Wall -ansi -DDB_INIT_STRING=\\\"$ENV{DB_INIT_STRING}\\\" " )
  ADD_EXECUTABLE( ${exec} ${exec}.cc )
  TARGET_LINK_LIBRARIES( ${exec} ${libname} )
  FOREACH( pkg ${packages} )
    IF( BUILD_WITH_USE_${pkg} )
      MESSAGE( "${pkg} libraries are: ${${pkg}_LIBRARIES}" )
      TARGET_LINK_LIBRARIES( ${exec} ${${pkg}_LIBRARIES} )
    ENDIF()
  ENDFOREACH( pkg ${packages} )

  # ADD_DEPENDENCIES( ${BIN_INSTALL_DIR} example)
  INSTALL(TARGETS ${exec} DESTINATION ${BIN_INSTALL_DIR})
ENDFOREACH( exec ${PROJECT_NAME}_execs )
#################################
# install                       #
#################################
#install library
INSTALL( TARGETS ${libname}
 	 DESTINATION ${LIB_INSTALL_DIR}
 	 PERMISSIONS
 	 OWNER_READ OWNER_WRITE OWNER_EXECUTE
 	 GROUP_READ GROUP_EXECUTE
 	 WORLD_READ WORLD_EXECUTE  )

#install header files
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/" 
         DESTINATION ${INCLUDE_INSTALL_DIR}
         PATTERN "*~" EXCLUDE
         PATTERN "*CVS*" EXCLUDE )


################################
# prepare for XXXXConfig.cmake #
################################
GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${libname} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL 
     "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARY_NAME )
