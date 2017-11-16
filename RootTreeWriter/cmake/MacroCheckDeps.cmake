MESSAGE( STATUS "Fixed MacroCheckDeps.cmake loaded...." )
#################################################
# cmake macro for checking dependencies
# @author Jan Engels, DESY
#################################################

MACRO( CHECK_DEPS )
    MESSAGE( STATUS "Execute fixed MacroCheckDeps.cmake" )	
    # load macro
    MESSAGE( STATUS "-------------------------------------------------------------------------------" )
    MESSAGE( STATUS "Change a module with: cmake -D<ModuleName>_HOME=<Path_to_Module>" )
    MESSAGE( STATUS )

    #chek for macro or issue warning
    SET( _MacroCheckDepsFound FALSE )
    FOREACH( _cmp ${CMAKE_MODULE_PATH} )
       IF( EXISTS "${_cmp}/MacroLoadPackage.cmake" )
          SET( _MacroCheckDepsFound TRUE )
       ENDIF()       
    ENDFOREACH()

    IF( NOT _MacroCheckDepsFound )
        MESSAGE( FATAL_ERROR
            "\nSorry, could not find MacroLoadPackage.cmake...\n"
            "Please set CMAKE_MODULE_PATH correctly with: "
            "cmake -DCMAKE_MODULE_PATH=<path_to_cmake_modules>" )
    ENDIF()

    # load macro
    INCLUDE( "MacroLoadPackage" )


    # project dependencies
    IF( DEFINED ${PROJECT_NAME}_DEPENDS )
        SEPARATE_ARGUMENTS( ${PROJECT_NAME}_DEPENDS )
        MARK_AS_ADVANCED( ${PROJECT_NAME}_DEPENDS )
        FOREACH( req_pkg ${${PROJECT_NAME}_DEPENDS} )
            LOAD_PACKAGE( ${req_pkg} REQUIRED )
        ENDFOREACH()
        SET( ${PROJECT_NAME}_DEPENDS "${${PROJECT_NAME}_DEPENDS}" CACHE STRING
            "${PROJECT_NAME} dependencies" FORCE )
    ENDIF()

    # user defined dependencies
    IF( DEFINED BUILD_WITH )
        SEPARATE_ARGUMENTS( BUILD_WITH )
        MARK_AS_ADVANCED( BUILD_WITH )
        FOREACH( opt_pkg ${BUILD_WITH} )
            LOAD_PACKAGE( ${opt_pkg} REQUIRED )
        ENDFOREACH()
        SET( BUILD_WITH "${BUILD_WITH}" CACHE STRING
            "Build ${PROJECT_NAME} with these optional packages" FORCE )
    ENDIF()

    # user defined dependencies
    IF( DEFINED LINK_WITH )
        SEPARATE_ARGUMENTS( LINK_WITH )
        MARK_AS_ADVANCED( LINK_WITH )
        FOREACH( lnk_pkg ${LINK_WITH} )
            LOAD_PACKAGE( ${lnk_pkg} LINK_ONLY )
        ENDFOREACH()
        SET( LINK_WITH "${LINK_WITH}" CACHE STRING
            "Link ${PROJECT_NAME} with these optional packages" FORCE )
    ENDIF()

    MESSAGE( STATUS "-------------------------------------------------------------------------------" )
ENDMACRO( CHECK_DEPS )
