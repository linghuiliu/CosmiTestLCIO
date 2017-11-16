SET( lib1name labview2lcio )


################################################
# include directories                          #
################################################
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS}
                     ${Marlin_INCLUDE_DIRS}
		     ${LCCD_INCLUDE_DIRS}
                     ${CALICE_USERLIB_INCLUDE_DIRS}
		     ${ROOT_INCLUDE_DIRS}  )

IF( BUILD_WITH_USE_LCCD )
  INCLUDE_DIRECTORIES( ${LCCD_INCLUDE_DIRS}
                       ${CondDBMySQL_INCLUDE_DIRS} )
ENDIF( BUILD_WITH_USE_LCCD )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  INCLUDE_DIRECTORIES( ${streamlog_INCLUDE_DIRS} )
ENDIF()

################################################
# project definitions                          #
################################################
ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS}        )
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )

################################################
# sources                                      #
################################################
AUX_SOURCE_DIRECTORY( . ${lib1name}_srcs )
SET_SOURCE_FILES_PROPERTIES(  ${${lib1name}_srcs} PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS}" )

################################################
# what to build                                #
################################################
ADD_LIBRARY( ${lib1name} SHARED ${${lib1name}_srcs} )

TARGET_LINK_LIBRARIES(${lib1name} ${LCIO_LIBRARIES}
				  ${Marlin_LIBRARIES} 
				  ${LCCD_LIBRARIES} 
				  ${CALICE_USERLIB_LIBRARIES} 
				  ${ROOT_LIBRARIES})


SET_TARGET_PROPERTIES( ${lib1name} PROPERTIES
  VERSION ${${PROJECT_NAME}_VERSION}
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  )

################################################
# install                                      #
################################################
# install header files
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/"
  DESTINATION ${INCLUDE_INSTALL_DIR}
  PATTERN "*~" EXCLUDE
  PATTERN "*CVS*" EXCLUDE )

# install library
INSTALL( TARGETS ${lib1name}
  DESTINATION ${LIB_INSTALL_DIR}
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE  )


################################################
# prepare for XXXConfig.cmake                  #
################################################
GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${lib1name} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARY_NAME )
