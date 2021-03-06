MACRO(SET_FILE_PROPERTIES PROPERTY)
    FOREACH(arg ${ARGN})
        STRING(REGEX REPLACE "^([^ ]+) .+$" "\\1" VAR_NAME ${arg})
        STRING(REGEX REPLACE "^[^ ]+ +(.+)$" "\\1" VALUE ${arg})
        STRING(REGEX REPLACE ":" ";" VALUE ${VALUE})
        SET(${PROPERTY}_${VAR_NAME} "${VALUE}")
    ENDFOREACH()
ENDMACRO()

FUNCTION(OPENMEEG_COMPARISON_TEST TEST_NAME FILENAME REFERENCE_FILENAME)
    SET(COMPARISON_COMMAND compare_matrix)
    IF (NOT IS_ABSOLUTE ${FILENAME})
        SET(FILENAME ${OpenMEEG_BINARY_DIR}/tests/${FILENAME})
    ENDIF()
    IF (NOT IS_ABSOLUTE ${REFERENCE_FILENAME})
        SET(REFERENCE_FILENAME ${OpenMEEG_SOURCE_DIR}/tests/${REFERENCE_FILENAME})
    ENDIF()

    IF (WIN32)
        SET(COMPARISON_COMMAND "${EXECUTABLE_OUTPUT_PATH}/${COMPARISON_COMMAND}")
    ELSE()
        SET(COMPARISON_COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${COMPARISON_COMMAND}")
    ENDIF()

    OPENMEEG_TEST("cmp-${TEST_NAME}" 
             ${COMPARISON_COMMAND} ${FILENAME} ${REFERENCE_FILENAME} ${ARGN} DEPENDS ${TEST_NAME})
ENDFUNCTION()
