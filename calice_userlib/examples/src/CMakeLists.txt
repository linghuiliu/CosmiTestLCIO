SET( lib1name Dataproc )


#####################
# include directories
#####################

MESSAGE ( "PROJECT ${${PROJECT_NAME}_SOURCE_DIR}" )

INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" 
                     "${${PROJECT_NAME}_SOURCE_DIR}/examples/include"   )

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS} ${Marlin_INCLUDE_DIRS} 
                     ${CALICE_USERLIB_INCLUDE_DIRS}              )

IF( BUILD_WITH_USE_LCCD )
  INCLUDE_DIRECTORIES( ${LCCD_INCLUDE_DIRS} )
ENDIF( BUILD_WITH_USE_LCCD )

IF( BUILD_WITH_HAVE_ROOT )
  INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
ENDIF( BUILD_WITH_HAVE_ROOT )


OPTION( SUPPORT_OLD_ILC_CORE_MODULES "Support old ilcsoft cmake modules" ON )
MARK_AS_ADVANCED( SUPPORT_OLD_ILC_CORE_MODULES )
# Support for old core modules
# remove in future releases
IF ( SUPPORT_OLD_ILC_CORE_MODULES )
  INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIR} ${Marlin_INCLUDE_DIR}  )

  IF( BUILD_WITH_USE_LCCD )
    INCLUDE_DIRECTORIES( ${LCCD_INCLUDE_DIR} )
  ENDIF( BUILD_WITH_USE_LCCD )

  IF( BUILD_WITH_HAVE_ROOT )
    INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIR} )
  ENDIF( BUILD_WITH_HAVE_ROOT )
ENDIF( SUPPORT_OLD_CORE_MODULES )




####################
# sources
####################

ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS}        ) 
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )
ADD_DEFINITIONS( ${LCIO_DEFINITIONS} ${Marlin_DEFINITIONS} )
ADD_DEFINITIONS( ${CALICE_USERLIB_DEFINITIONS}         )


IF( BUILD_WITH_USE_LCCD )
  ADD_DEFINITIONS( ${LCCD_DEFINITIONS} )
ENDIF( BUILD_WITH_USE_LCCD )

IF( BUILD_WITH_HAVE_ROOT )
  ADD_DEFINITIONS( ${ROOT_DEFINITIONS} )
ENDIF( BUILD_WITH_HAVE_ROOT )




           

# get list of all source files in this directory
AUX_SOURCE_DIRECTORY( . all_srcs )

# remove "./" from the list of source files
STRING( REPLACE "./" "" all_srcs  "${all_srcs}" )

# library sources
SET( ${lib1name}_srcs ${all_srcs} )



#####################
# what to build
####################

# require proper c++
SET_SOURCE_FILES_PROPERTIES(  ${${lib1name}_srcs} PROPERTIES
  COMPILE_FLAGS "-Wall -ansi " )

ADD_LIBRARY( ${lib1name} ${${lib1name}_srcs} )

SET_TARGET_PROPERTIES( ${lib1name} PROPERTIES
		       VERSION ${${PROJECT_NAME}_VERSION}
                       SOVERSION ${${PROJECT_NAME}_SOVERSION}
                        # not needed at the moment (no build of  
                        # static and dynamic lib at the same time)
                        # CLEAN_DIRECT_OUTPUT 1
                      )

                    

###############
# link against
###############


# todo: use options to change link behaviour to link with dependent libraries 
#       this would only be necessary if mixing static and shared libraries
#       because this is dangerous anyway this should not matter now
TARGET_LINK_LIBRARIES( ${lib1name} ${LCIO_LIBRARIES} 
		                   ${Marlin_LIBRARIES} 
				   ${CALICE_USERLIB_LIBRARIES}  )

IF( BUILD_WITH_USE_LCCD )
  TARGET_LINK_LIBRARIES( ${lib1name} ${LCCD_LIBRARIES} )
ENDIF( BUILD_WITH_USE_LCCD )

IF( BUILD_WITH_HAVE_ROOT )
  TARGET_LINK_LIBRARIES( ${lib1name} ${ROOT_LIBRARIES} )
ENDIF( BUILD_WITH_HAVE_ROOT )




####################
# install
####################

# Fixme: use explicit list of header files to install?
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/examples/include/" DESTINATION ${INCLUDE_INSTALL_DIR}
        PATTERN "*~" EXCLUDE
        PATTERN "*CVS*" EXCLUDE )

# fixme: execute permission not needed for static library 
INSTALL( TARGETS ${lib1name}
	 DESTINATION ${LIB_INSTALL_DIR}
	 PERMISSIONS
	 OWNER_READ OWNER_WRITE OWNER_EXECUTE
	 GROUP_READ GROUP_EXECUTE
	 WORLD_READ WORLD_EXECUTE  )


################################
# prepare for XXXXConfig.cmake
################################



GET_TARGET_PROPERTY(${Lib1name}_prefix ${lib1name} PREFIX)
GET_TARGET_PROPERTY(${Lib1name}_suffix ${lib1name} SUFFIX)
SET(lib_${lib1name}_name ${${Lib1name}_prefix}${Lib1name}${${Lib1name}_suffix})



GET_TARGET_PROPERTY(FULL_LIBRARY_NAME ${lib1name} LOCATION)
STRING(REGEX REPLACE ".*/" "" FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME})
SET( FULL_LIBRARY_NAME ${FULL_LIBRARY_NAME} CACHE INTERNAL "Full library name" FORCE )

MARK_AS_ADVANCED( FULL_LIBRARY_NAME )


