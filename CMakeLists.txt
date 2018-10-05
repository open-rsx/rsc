cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(InstallFilesRecursive)
include(EnableCoverageReport)
include(EnableSlocCount)
include(GenerateDoxygen)
include(GenerateCppcheck)
include(DefineProjectVersion)
include(CurrentDate)

# --- user options ---

option(BUILD_TESTS "Decide if the test suite shall be built or not" ON)
option(BUILD_EXAMPLES "Decide if the examples shall be built or not" ON)
option(INSTALL_TOOLCHAINS "Decide if CMake toolchain files should be installed" ON)
option(EXPORT_TO_CMAKE_PACKAGE_REGISTRY "If set to ON, RSC will be exported to the CMake user package registry so that downstream projects automatically find the workspace location in find_package calls." OFF)
option(ENCODE_VERSION "If set to ON, install paths and library name will have the version encoded ('rsc{major,minor}' instead of 'rsc') to allow parallel installation of different versions." ON)

# --- global definitions ---

set(VERSION_MAJOR "0" CACHE STRING "Major project version part")
set(VERSION_MINOR "18" CACHE STRING "Minor project version part")

project("RSC" VERSION "${VERSION_MAJOR}.${VERSION_MINOR}"
              LANGUAGES C CXX)

define_project_version(RSC_ ${VERSION_MAJOR} ${VERSION_MINOR} 0 "archive")
current_date(RSC_BUILD_DATE)

message(STATUS "This is RSC version: ${RSC_VERSION}-${RSC_WC_REVISION}")

set(RSC_ABI_VERSION "4")
set(SO_VERSION "${RSC_VERSION_MAJOR}.${RSC_VERSION_MINOR}")

if(ENCODE_VERSION)
    set(RSC_NAME "rsc${RSC_VERSION_MAJOR}.${RSC_VERSION_MINOR}")
else(ENCODE_VERSION)
    set(RSC_NAME "rsc")
endif(ENCODE_VERSION)
set(VERSION_SUFFIX "${RSC_VERSION_MAJOR}.${RSC_VERSION_MINOR}")
set(INSTALL_PATH_PREFIX "${RSC_NAME}"
    CACHE STRING "Prefix path applied to all non-versioned installed files in order to prevent version clashes.")
set(INSTALL_PATH_PREFIX_CMAKE "rsc-cmake${RSC_VERSION_MAJOR}.${RSC_VERSION_MINOR}"
    CACHE STRING "Prefix path applied to the config files for the CMake library.")

set(RSC_TEST_NAME "rsctest")

set(RSC_CMAKE_PATH "share/${INSTALL_PATH_PREFIX}/cmake")
set(RSC_CMAKE_MODULE_PATH "${RSC_CMAKE_PATH}/Modules")
set(RSC_CMAKE_TOOLCHAIN_PATH "${RSC_CMAKE_PATH}/Toolchains")

set(OUTPUT_PATH "${CMAKE_BINARY_DIR}/build")
set(ARCHIVE_OUTPUT_PATH ${OUTPUT_PATH})
set(LIBRARY_OUTPUT_PATH ${OUTPUT_PATH})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_PATH})

# --- global compiler flags ---

include(PedanticCompilerWarnings)

if(WIN32)
    add_definitions(/D_USE_MATH_DEFINES)
endif()

check_include_file("sys/types.h" HAVE_TYPES_H)
if(HAVE_TYPES_H)
    add_definitions(-DHAVE_TYPES_H)
endif()
check_include_file("sys/wait.h" HAVE_WAIT_H)
if(HAVE_WAIT_H)
    add_definitions(-DHAVE_WAIT_H)
endif()
check_include_file("unistd.h" HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
    add_definitions(-DHAVE_UNISTD_H)
endif()

# decide how to do name demangling
check_include_file_cxx("cxxabi.h" HAVE_CXXABI_H)
if(HAVE_CXXABI_H)
    add_definitions(-DDEMANGLE_GCC)
elseif(MSVC)
    add_definitions(-DDEMANGLE_MSVC)
else()
    message(SEND_ERROR "No demangling solution found for the system.")
endif()

# --- dependency handling ---

find_package(Threads REQUIRED)

set(Boost_USE_VERSION 1.38 CACHE INTERNAL "Boost Version to use")
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_ALL_DYN_LINK)

if(BOOST_ROOT)
    set(Boost_NO_SYSTEM_PATHS ON)
endif()

# determine the required libraries for boost based on its version
find_package(Boost ${Boost_USE_VERSION} REQUIRED)
set(BOOST_COMPONENTS date_time thread filesystem signals program_options system regex)
if(Boost_VERSION STREQUAL 105000 OR Boost_VERSION STRGREATER 105000)
    list(APPEND BOOST_COMPONENTS chrono)
endif()
find_package(Boost ${Boost_USE_VERSION} REQUIRED ${BOOST_COMPONENTS})
include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# --- source code ---

include_directories(BEFORE src "${CMAKE_CURRENT_BINARY_DIR}/src")

add_subdirectory(src)
if(BUILD_TESTS)
    include(ProvideGoogleMock)
    if(GMOCK_AVAILABLE)
        add_subdirectory(test)
    else()
        message(WARNING "Could not build unit tests even though desired because Google Mock could not be installed.")
    endif()
endif()
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(cmake)


# --- package ---
# default configuration, independent of packaging mechanism
set(CPACK_PACKAGE_VERSION_MAJOR ${RSC_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${RSC_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${RSC_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION       "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR        "CoR-Lab Bielefeld University")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
include(ProvideFlexibleCPack)

# --- pkgconfig file ---

if(UNIX)
    set(EXTERNAL_INCLUDES ${Boost_INCLUDE_DIRS})
    string(REPLACE ";" " -I" EXTERNAL_INCLUDE_COMMANDS "${EXTERNAL_INCLUDES}")
    string(LENGTH "${EXTERNAL_INCLUDE_COMMANDS}" EXT_LENGTH)
    if(${EXT_LENGTH} GREATER 0)
        set(EXTERNAL_INCLUDE_COMMANDS "-I${EXTERNAL_INCLUDE_COMMANDS}")
    endif()
    configure_file(rsc.pc.in "${CMAKE_BINARY_DIR}/${RSC_NAME}.pc" @ONLY)
    install(FILES "${CMAKE_BINARY_DIR}/${RSC_NAME}.pc" DESTINATION lib/pkgconfig)
endif()

# --- cmake config files ---

if(EXPORT_TO_CMAKE_PACKAGE_REGISTRY)
    message(STATUS "Exported RSC to CMake package registry")
    export(PACKAGE RSC)
endif()

configure_file(RSCBuildTreeSettings.cmake.in
               "${CMAKE_BINARY_DIR}/RSCBuildTreeSettings.cmake" @ONLY)

set(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
if(CMAKE_LINK_LIBRARY_SUFFIX)
    set(LIB_SUFFIX ${CMAKE_LINK_LIBRARY_SUFFIX})
endif()
configure_file(RSCConfig.cmake.in
               "${CMAKE_BINARY_DIR}/RSCConfig.cmake"
               @ONLY)
configure_file(RSC-CMakeConfig.cmake.in
               "${CMAKE_BINARY_DIR}/RSC-CMakeConfig.cmake"
               @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/RSC-CMakeConfig.cmake"
              "${CMAKE_BINARY_DIR}/RSC-CMakeConfigVersion.cmake"
        DESTINATION "share/${INSTALL_PATH_PREFIX_CMAKE}")
configure_file(RSCConfigVersion.cmake.in
               "${CMAKE_BINARY_DIR}/RSCConfigVersion.cmake"
               @ONLY)
configure_file(RSCConfigVersion.cmake.in
               "${CMAKE_BINARY_DIR}/RSC-CMakeConfigVersion.cmake"
               @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/RSCConfig.cmake"
              "${CMAKE_BINARY_DIR}/RSCConfigVersion.cmake"
        DESTINATION "share/${INSTALL_PATH_PREFIX}")

export(TARGETS ${RSC_NAME} FILE "${CMAKE_BINARY_DIR}/RSCDepends.cmake")
install(EXPORT RSCDepends
        DESTINATION "share/${INSTALL_PATH_PREFIX}")

# --- enable unit tests if desired ---

if(BUILD_TESTS)
    enable_testing()
endif()

# --- documentation generation ---

generate_doxygen(VERSION "${RSC_VERSION}")

# --- coverage ---

enable_coverage_report(TARGETS ${RSC_NAME}
                       TESTS  ${RSC_TEST_NAME}
                       FILTER "*test/*")

# --- sloccount ---

enable_sloccount(FOLDERS src test examples)

# --- cppcheck ---

generate_cppcheck(SOURCES src test examples
                  "${CMAKE_CURRENT_BINARY_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/test" "${CMAKE_CURRENT_BINARY_DIR}/examples"
                  ENABLE_IDS style
                  INLINE_SUPPRESSION)
