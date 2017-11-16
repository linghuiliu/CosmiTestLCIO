# Ref: GEAR CMakeLists.txt

IF( INSTALL_DOC )

    FIND_PACKAGE( Doxygen )
   
    IF( DOXYGEN_FOUND )
    
        ADD_CUSTOM_COMMAND(
            OUTPUT "${PROJECT_BINARY_DIR}/doc/html"
            COMMAND "${DOXYGEN_EXECUTABLE}"
            WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/doc"
            COMMENT "Building Documentation..."
            DEPENDS "${PROJECT_BINARY_DIR}/doc/Doxyfile"
        )
        
        ADD_CUSTOM_TARGET( doc DEPENDS "${PROJECT_BINARY_DIR}/doc/html" )
   
    ELSE( DOXYGEN_FOUND )
            MESSAGE( STATUS "Doxygen not found in your system!!" )
            MESSAGE( STATUS "INSTALL_DOC forced to OFF" )
            SET( INSTALL_DOC OFF )
    ENDIF( DOXYGEN_FOUND )

    # create documentation Doxyfile
    CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                    ${PROJECT_BINARY_DIR}/doc/Doxyfile @ONLY )

    # make sure doxygen is executed before make install
    INSTALL( CODE "EXEC_PROGRAM(${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doc)" )
    # by just calling "make doc" doxygen won't run if the documentation has already been built
    #INSTALL( CODE "EXEC_PROGRAM(${CMAKE_BUILD_TOOL} ${PROJECT_BINARY_DIR} ARGS doc)" )
    
ENDIF()
