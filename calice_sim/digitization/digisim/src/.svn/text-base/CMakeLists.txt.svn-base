SET( lib1name digisim )

#####################
# include directories
#####################

INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS}
                     ${Marlin_INCLUDE_DIRS} 
                     ${LCCD_INCLUDE_DIRS}

                     ${GEAR_INCLUDE_DIRS}                      
                     ${CLHEP_INCLUDE_DIRS}        
                     ${ROOT_INCLUDE_DIRS}        
                   )


####################
# sources
####################

ADD_DEFINITIONS( ${${PROJECT_NAME}_DEFINITIONS}        ) 
ADD_DEFINITIONS( ${${PROJECT_NAME}_EXPORT_DEFINITIONS} )

ADD_DEFINITIONS( ${LCIO_DEFINITIONS} 
                 ${Marlin_DEFINITIONS}
                 ${LCCD_DEFINITIONS} 

                 ${GEAR_DEFINITIONS}
                 ${CLHEP_DEFINITIONS}
               )

#collect all source files in current (source) directory
AUX_SOURCE_DIRECTORY( . ${lib1name}_srcs )

####################
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

TARGET_LINK_LIBRARIES( ${lib1name} ${LCIO_LIBRARIES} 
		                   ${Marlin_LIBRARIES}
                                   ${LCCD_LIBRARIES}

                                   ${GEAR_LIBRARIES}
                                   ${CLHEP_LIBRARIES}
                                   ${ROOT_LIBRARIES}
                                   )

####################
# install
####################


# Fixme: use explicit list of header files to install?
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/" DESTINATION ${INCLUDE_INSTALL_DIR}
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