enable_testing()

set(TEST_RESULT_DIR ${CMAKE_BINARY_DIR}/testresults)
set(PKGCONFIG_TEST_NAME "${RSC_TEST_NAME}_pkgconfig")
set(TEST_PLUGIN_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/testplugins")
set(TEST_PLUGIN_DIRECTORY_NAME_CLASH "${CMAKE_CURRENT_BINARY_DIR}/testplugins_clash")
set(TEST_PLUGIN_DIRECTORY_OVERRIDE "${CMAKE_CURRENT_BINARY_DIR}/testplugins_override")
set(PLUGIN_CALL_FILE "${CMAKE_CURRENT_BINARY_DIR}/testplugins/callfile")
file(TO_NATIVE_PATH ${TEST_PLUGIN_DIRECTORY} TEST_PLUGIN_DIRECTORY_NATIVE)
string(REPLACE "\\" "\\\\" TEST_PLUGIN_DIRECTORY_NATIVE ${TEST_PLUGIN_DIRECTORY_NATIVE})
file(TO_NATIVE_PATH ${TEST_PLUGIN_DIRECTORY_NAME_CLASH} TEST_PLUGIN_DIRECTORY_NAME_CLASH_NATIVE)
string(REPLACE "\\" "\\\\" TEST_PLUGIN_DIRECTORY_NAME_CLASH_NATIVE ${TEST_PLUGIN_DIRECTORY_NAME_CLASH_NATIVE})
file(TO_NATIVE_PATH ${TEST_PLUGIN_DIRECTORY_OVERRIDE} TEST_PLUGIN_DIRECTORY_OVERRIDE_NATIVE)
string(REPLACE "\\" "\\\\" TEST_PLUGIN_DIRECTORY_OVERRIDE_NATIVE ${TEST_PLUGIN_DIRECTORY_OVERRIDE_NATIVE})
file(TO_NATIVE_PATH ${PLUGIN_CALL_FILE} PLUGIN_CALL_FILE_NATIVE)
string(REPLACE "\\" "\\\\" PLUGIN_CALL_FILE_NATIVE ${PLUGIN_CALL_FILE_NATIVE})

include_directories(BEFORE SYSTEM ${GMOCK_INCLUDE_DIRS})
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

add_definitions(${GMOCK_CFLAGS})

set(TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(testconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

# --- library test ---

file(GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
                       "rsc/debug/*.cpp"
                       "rsc/logging/*.cpp"
                       "rsc/config/*.cpp"
                       "rsc/math/*.cpp"
                       "rsc/threading/*.cpp"
                       "rsc/runtime/*.cpp"
                       "rsc/patterns/*.cpp"
                       "rsc/os/*.cpp")
set(TEST_SOURCES ${TEST_SOURCES} "rsc/RscTestSuite.cpp")
list(APPEND TEST_SOURCES "rsc/misc/UUIDTest.cpp" "rsc/misc/langutilsTest.cpp")
list(APPEND TEST_SOURCES "rsc/plugins/ConfiguratorTest.cpp"
                         "rsc/plugins/PluginTest.cpp")

# subprocess
if(UNIX)
    set(TEST_SOURCES ${TEST_SOURCES} rsc/subprocess/UnixSubprocessTest.cpp)
elseif(WIN32)
    set(TEST_SOURCES ${TEST_SOURCES} rsc/subprocess/WindowsSubprocessTest.cpp)
endif()

add_executable(${RSC_TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${RSC_TEST_NAME}
                      ${RSC_NAME}
                      ${GMOCK_LIBRARIES})

add_test(${RSC_TEST_NAME} ${EXECUTABLE_OUTPUT_PATH}/${RSC_TEST_NAME}
                          "--dummy-for-process-info-test"
                          "--gtest_output=xml:${TEST_RESULT_DIR}/")

# test plugins
set(TESTPLUGIN_NAMES testplugin
                     testplugin-a-comPlex42NAME
                     testplugin-with.dots.in.name.or.so.version.0.9
                     testplugin-missing-init
                     testplugin-missing-shutdown
                     testplugin-init-exception
                     testplugin-shutdown-exception)
foreach(TESTPLUGIN_NAME ${TESTPLUGIN_NAMES})
    add_library(${TESTPLUGIN_NAME} SHARED rsc/plugins/testplugin.cpp)
    target_link_libraries(${TESTPLUGIN_NAME} ${Boost_LIBRARIES})
    set_target_properties(${TESTPLUGIN_NAME} PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY ${TEST_PLUGIN_DIRECTORY}
                          RUNTIME_OUTPUT_DIRECTORY ${TEST_PLUGIN_DIRECTORY})
endforeach()
set_target_properties(testplugin-missing-init PROPERTIES
                      COMPILE_DEFINITIONS "PLUGIN_MISSING_INIT")
set_target_properties(testplugin-missing-shutdown PROPERTIES
                      COMPILE_DEFINITIONS "PLUGIN_MISSING_SHUTDOWN")
set_target_properties(testplugin-init-exception PROPERTIES
                      COMPILE_DEFINITIONS "PLUGIN_INIT_EXCEPTION")
set_target_properties(testplugin-shutdown-exception PROPERTIES
                      COMPILE_DEFINITIONS "PLUGIN_SHUTDOWN_EXCEPTION")

# test plugins with name clashes
add_library(testplugin.clash SHARED rsc/plugins/testplugin.cpp)
add_library(testplugin.clashed SHARED rsc/plugins/testplugin.cpp)
target_link_libraries(testplugin.clash ${Boost_LIBRARIES})
target_link_libraries(testplugin.clashed ${Boost_LIBRARIES})
set_target_properties(testplugin.clash testplugin.clashed PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY ${TEST_PLUGIN_DIRECTORY_NAME_CLASH}
                      RUNTIME_OUTPUT_DIRECTORY ${TEST_PLUGIN_DIRECTORY_NAME_CLASH})

# test plugin overriding one from the usual path
add_library(testplugin.override SHARED rsc/plugins/testplugin.cpp)
target_link_libraries(testplugin.override ${Boost_LIBRARIES})
set_target_properties(testplugin.override PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY ${TEST_PLUGIN_DIRECTORY_OVERRIDE}
                      RUNTIME_OUTPUT_DIRECTORY ${TEST_PLUGIN_DIRECTORY_OVERRIDE})
set_target_properties(testplugin.override PROPERTIES
                      COMPILE_DEFINITIONS "PLUGIN_OVERRIDE")

# Pkg-config test
if(UNIX)
    configure_file(pkg-config-test.sh.in "${CMAKE_CURRENT_BINARY_DIR}/pkg-config-test.sh")
    add_test(${PKGCONFIG_TEST_NAME} "${CMAKE_CURRENT_BINARY_DIR}/pkg-config-test.sh")
endif()

if(WIN32)
    set(PATH_STRING "$ENV{PATH};${Boost_LIBRARY_DIRS}")
    # requried for PATH entries with a slash before the semicolon
    string(REPLACE "\\;" ";" PATH_STRING "${PATH_STRING}")
    string(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    set_property(TEST ${RSC_TEST_NAME}
                 PROPERTY ENVIRONMENT "PATH=${PATH_STRING}")
endif()
