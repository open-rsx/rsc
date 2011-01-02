# - Generate a cppcheck documentation for a project.
# The function GENERATE_CPPCHECK is provided to create a "cppcheck" target that
# performs static code analysis using the cppcheck utility program.
#
# GENERATE_CPPCHECK(SOURCES <sources to check...> [SUPPRESSION_FILE <file>] [TARGET_NAME <name>])
#
# Generates a target "cppcheck" that executes cppcheck on the specified sources.
# SUPPRESSION_FILE may be give additionally to specify suppressions for#
# cppcheck. The sources mentioned in the suppression file must be in the same
# format like given for SOURCES. This means if you specified them relative to
# CMAKE_CURRENT_SOURCE_DIR, then the same relative paths must be used in the
# suppression file.
# With TARGET_NAME a different name for the generated check target can be
# specified. This is useful if several calles to this function are made in one
# CMake project, as otherwise the target names collide.
#
# This function can always be called, even if no cppcheck was found. Then no
# target is created.
#
# Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

GET_FILENAME_COMPONENT(GENERATE_CPPCHECK_MODULE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

FIND_PACKAGE(Cppcheck)

FUNCTION(GENERATE_CPPCHECK)

    IF(CPPCHECK_FOUND)
    
        PARSE_ARGUMENTS(ARG "SOURCES;SUPPRESSION_FILE;TARGET_NAME" "" ${ARGN})
        
        SET(TARGET_NAME "cppcheck")
        SET(TARGET_NAME_SUFFIX "")
        # parse target name
        LIST(LENGTH ARG_TARGET_NAME TARGET_NAME_LENGTH)
        IF(${TARGET_NAME_LENGTH} EQUAL 1)
            SET(TARGET_NAME ${ARG_TARGET_NAME})
            SET(TARGET_NAME_SUFFIX "-${ARG_TARGET_NAME}")
        ENDIF()
        
        SET(CPPCHECK_CHECKFILE "${CMAKE_BINARY_DIR}/cppcheck-files${TARGET_NAME_SUFFIX}")
        SET(CPPCHECK_REPORT_FILE "${CMAKE_BINARY_DIR}/cppcheck-report${TARGET_NAME_SUFFIX}.xml")
        SET(CPPCHECK_WRAPPER_SCRIPT "${CMAKE_BINARY_DIR}/cppcheck${TARGET_NAME_SUFFIX}.cmake")
    
        # write a list file containing all sources to check for the call to
        # cppcheck
        FILE(WRITE "${CPPCHECK_CHECKFILE}" "")
        FOREACH(SOURCE ${ARG_SOURCES})
            FILE(APPEND "${CPPCHECK_CHECKFILE}" "${SOURCE}\n")
        ENDFOREACH()
        
        # prepare a cmake wrapper to write the stderr output of cppcheck to
        # the result file
        
        LIST(LENGTH ARG_SUPPRESSION_FILE SUPPRESSION_FILE_LENGTH)
        IF(${SUPPRESSION_FILE_LENGTH} EQUAL 1)
            GET_FILENAME_COMPONENT(ABS "${ARG_SUPPRESSION_FILE}" ABSOLUTE)
            MESSAGE(STATUS "Using suppression file ${ABS}")
            SET(SUPPRESSION_ARGUMENT --suppressions)
            SET(SUPPRESSION_FILE "\"${ABS}\"")
        ENDIF()
        
        FILE(WRITE ${CPPCHECK_WRAPPER_SCRIPT}
"
EXECUTE_PROCESS(COMMAND \"${CPPCHECK_EXECUTABLE}\" ${SUPPRESSION_ARGUMENT} ${SUPPRESSION_FILE} --enable=style --xml \"--file-list=${CPPCHECK_CHECKFILE}\"
                RESULT_VARIABLE CPPCHECK_EXIT_CODE
                ERROR_VARIABLE ERROR_OUT
                WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\")
IF(NOT CPPCHECK_EXIT_CODE EQUAL 0)
    MESSAGE(FATAL_ERROR \"Error executing cppcheck for target ${TARGET}, return code: \${CPPCHECK_EXIT_CODE}\")
ENDIF()
IF(ERROR_OUT)
    MESSAGE(\"Detected errors:\\n\${ERROR_OUT}\")
ENDIF()
FILE(WRITE \"${CPPCHECK_REPORT_FILE}\" \"\${ERROR_OUT}\")
"
            )
            
        ADD_CUSTOM_TARGET(${TARGET_NAME} ${CMAKE_COMMAND} -P "${CPPCHECK_WRAPPER_SCRIPT}"
                          COMMENT "Generates the cppcheck result for this project")

    ENDIF()

ENDFUNCTION()
