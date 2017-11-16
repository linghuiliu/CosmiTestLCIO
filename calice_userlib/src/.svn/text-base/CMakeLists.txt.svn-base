SET( lib1name userlib )

##############################################################
# include directories                                        #
##############################################################
INCLUDE_DIRECTORIES( BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include" )
INCLUDE_DIRECTORIES( BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/Mapping" )
INCLUDE_DIRECTORIES( BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/CellDescription" )
INCLUDE_DIRECTORIES( BEFORE "${${PROJECT_NAME}_SOURCE_DIR}/include/Neighbours" )

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS}
                     ${Marlin_INCLUDE_DIRS} 
		     ${streamlog_INCLUDE_DIRS} )

IF( BUILD_WITH_USE_LCCD )
  INCLUDE_DIRECTORIES( ${LCCD_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES( ${CondDBMySQL_INCLUDE_DIRS} )
ENDIF( BUILD_WITH_USE_LCCD )

IF ( BUILD_WITH_HAVE_ROOT )
  INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
ENDIF( BUILD_WITH_HAVE_ROOT )

############################################################
# sources                                                  #
############################################################
ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS} )
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )
ADD_DEFINITIONS( -DDB_INIT_STRING=\"${DB_INIT_STRING}\" )

#collect all source files in current (source) directory
AUX_SOURCE_DIRECTORY( . ${lib1name}_srcs )

#alternatively give explicit list
#SET( ${lib1name}_srcs <explicit file list> )

#############################################################
# what to build                                             #
#############################################################

# require proper c++
SET_SOURCE_FILES_PROPERTIES(  ${${lib1name}_srcs} PROPERTIES
                              COMPILE_FLAGS " ${CMAKE_CXX_FLAGS}" )

ADD_LIBRARY( ${lib1name} ${${lib1name}_srcs} )

TARGET_LINK_LIBRARIES(${lib1name} ${LCIO_LIBRARIES}
${Marlin_LIBRARIES} ${streamlog_LIBRARIES})

IF( BUILD_WITH_USE_LCCD )
  TARGET_LINK_LIBRARIES(${lib1name} ${LCCD_LIBRARIES} ${CondDBMySQL_LIBRARIES})
ENDIF( BUILD_WITH_USE_LCCD )


SET_TARGET_PROPERTIES( ${lib1name} PROPERTIES
		       VERSION ${${PROJECT_NAME}_VERSION}
                       SOVERSION ${${PROJECT_NAME}_SOVERSION}
                        # not needed at the moment (no build of  
                        # static and dynamic lib at the same time)
                        # CLEAN_DIRECT_OUTPUT 1
                      )

#############################################################
# install                                                   #
#############################################################
# Fixme: use explicit list of header files to install?
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/" DESTINATION ${INCLUDE_INSTALL_DIR}
        PATTERN "*~" EXCLUDE
        PATTERN "*CVS*" EXCLUDE
        PATTERN "*SVN*" EXCLUDE )

# fixme: execute permission not needed for static library 
INSTALL( TARGETS ${lib1name}
	 DESTINATION ${LIB_INSTALL_DIR}
	 PERMISSIONS
	 OWNER_READ OWNER_WRITE OWNER_EXECUTE
	 GROUP_READ GROUP_EXECUTE
	 WORLD_READ WORLD_EXECUTE  )


##############################################################
# prepare for XXXXConfig.cmake                               #
#############################################################
GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${lib1name} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARY_NAME )


