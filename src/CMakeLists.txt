# --- library ---

if(INIT_METHOD_ATTRIBUTE_CONSTRUCTOR)
    set(INIT_METHOD_NAME RSC_HAVE_INIT_METHOD_ATTRIBUTE_CONSTRUCTOR)
elseif(INIT_METHOD_CRT)
    set(INIT_METHOD_NAME RSC_HAVE_INIT_METHOD_CRT)
else()
    set(INIT_METHOD_NAME RSC_HAVE_NO_INIT_METHOD_ERROR)
endif()
string(REPLACE "." "_" RSC_EXPORTS_NAME ${RSC_NAME})
configure_file(rsc/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/rsc/config.h @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsc/config.h DESTINATION "include/${INSTALL_PATH_PREFIX}/rsc")

configure_file(rsc/rscexports.h.in ${CMAKE_CURRENT_BINARY_DIR}/rsc/rscexports.h @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsc/rscexports.h DESTINATION "include/${INSTALL_PATH_PREFIX}/rsc")

# version stuff
include(PadString)
pad_string(MAJOR_PADDED 2 "0" ${RSC_VERSION_MAJOR})
pad_string(MINOR_PADDED 2 "0" ${RSC_VERSION_MINOR})
pad_string(PATCH_PADDED 2 "0" ${RSC_VERSION_PATCH})
set(RSC_VERSION_NUMERIC "${MAJOR_PADDED}${MINOR_PADDED}${PATCH_PADDED}")
configure_file(rsc/Version.cpp.in "${CMAKE_CURRENT_BINARY_DIR}/rsc/Version.cpp" @ONLY)
configure_file(rsc/Version.h.in "${CMAKE_CURRENT_BINARY_DIR}/rsc/Version.h" @ONLY)

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/rsc)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
                  "rsc/logging/*.cpp"
                  "rsc/config/*.cpp"
                  "rsc/math/*.cpp"
                  "rsc/runtime/*.cpp"
                  "rsc/patterns/*.cpp"
                  "rsc/threading/*.cpp"
                  "rsc/plugins/*.cpp")
set(SOURCES ${SOURCES}
            rsc/subprocess/Subprocess.cpp

            rsc/debug/DebugTools.cpp

            rsc/misc/langutils.cpp
            rsc/misc/IllegalStateException.cpp
            rsc/misc/UnsupportedOperationException.cpp
            rsc/misc/UUID.cpp

            "${CMAKE_CURRENT_BINARY_DIR}/rsc/Version.cpp")

file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
                  "rsc/logging/*.h"
                  "rsc/config/*.h"
                  "rsc/math/*.h"
                  "rsc/misc/*.h"
                  "rsc/runtime/*.h"
                  "rsc/patterns/*.h"
                  "rsc/patterns/detail/*.h"
                  "rsc/threading/*.h"
                  "rsc/plugins/*.h"
                  "rsc/os/*.h")
set(HEADERS ${HEADERS}
            rsc/debug/DebugTools.h
            rsc/subprocess/Subprocess.h
            ${CMAKE_CURRENT_BINARY_DIR}/rsc/Version.h
            ${CMAKE_CURRENT_BINARY_DIR}/rsc/config.h
            ${CMAKE_CURRENT_BINARY_DIR}/rsc/rscexports.h)

# Platform-specific backend choices
message(STATUS "Selecting platform-specific backends")
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    message(STATUS "  UnixSubprocess")
    list(APPEND SOURCES rsc/subprocess/UnixSubprocess.cpp)
    list(APPEND HEADERS rsc/subprocess/UnixSubprocess.h)
    add_definitions(-DSUBPROCESS_UNIX)

    message(STATUS "  LinuxDebugTools")
    list(APPEND SOURCES rsc/debug/LinuxDebugTools.cpp)
    list(APPEND HEADERS rsc/debug/LinuxDebugTools.h)
    add_definitions(-DDEBUGTOOLS_LINUX)

    message(STATUS "  PosixSignalWaiter")
    list(APPEND SOURCES rsc/misc/PosixSignalWaiter.cpp)

    message(STATUS "  LinuxProcessInfo")
    list(APPEND SOURCES rsc/os/LinuxProcessInfo.cpp)

    message(STATUS "  LinuxHostInfo")
    list(APPEND SOURCES rsc/os/LinuxHostInfo.cpp
                        rsc/os/PosixUtilities.cpp)

elseif(APPLE)

    message(STATUS "  UnixSubprocess")
    list(APPEND SOURCES rsc/subprocess/UnixSubprocess.cpp)
    list(APPEND HEADERS rsc/subprocess/UnixSubprocess.h)
    add_definitions(-DSUBPROCESS_UNIX)

    message(STATUS "  LinuxDebugTools")
    list(APPEND SOURCES rsc/debug/LinuxDebugTools.cpp)
    list(APPEND HEADERS rsc/debug/LinuxDebugTools.h)
    add_definitions(-DDEBUGTOOLS_LINUX)

    message(STATUS "  MacSignalWaiter")
    list(APPEND SOURCES rsc/misc/MacSignalWaiter.cpp)

    message(STATUS "  MacProcessInfo")
    list(APPEND SOURCES rsc/os/MacProcessInfo.cpp)

    message(STATUS "  MacHostInfo")
    list(APPEND SOURCES rsc/os/MacHostInfo.cpp
                        rsc/os/PosixUtilities.cpp)


elseif(UNIX)
    message(STATUS "  UnixSubprocess")
    list(APPEND SOURCES rsc/subprocess/UnixSubprocess.cpp)
    list(APPEND HEADERS rsc/subprocess/UnixSubprocess.h)
    add_definitions(-DSUBPROCESS_UNIX)

    message(STATUS "  LinuxDebugTools")
    list(APPEND SOURCES rsc/debug/LinuxDebugTools.cpp)
    list(APPEND HEADERS rsc/debug/LinuxDebugTools.h)
    add_definitions(-DDEBUGTOOLS_LINUX)

    message(STATUS "  PosixSignalWaiter")
    list(APPEND SOURCES rsc/misc/PosixSignalWaiter.cpp)

    message(STATUS "  PosixProcessInfo")
    list(APPEND SOURCES rsc/os/PosixProcessInfo.cpp)

    message(STATUS "  PosixHostInfo")
    list(APPEND SOURCES rsc/os/PosixHostInfo.cpp
                        rsc/os/PosixUtilities.cpp)

elseif(WIN32)
    message(STATUS "  WindowsSubprocess")
    list(APPEND SOURCES rsc/subprocess/WindowsSubprocess.cpp)
    list(APPEND HEADERS rsc/subprocess/WindowsSubprocess.h)
    add_definitions(-DSUBPROCESS_WINDOWS)

    message(STATUS "  WindowsDebugTools")
    list(APPEND SOURCES rsc/debug/WindowsDebugTools.cpp)
    list(APPEND HEADERS rsc/debug/WindowsDebugTools.h)
    add_definitions(-DDEBUGTOOLS_WINDOWS)

    message(STATUS "  Win32SignalWaiter")
    list(APPEND SOURCES rsc/misc/Win32SignalWaiter.cpp)

    list(APPEND HEADERS rsc/os/Win32Common.h)
    list(APPEND SOURCES rsc/os/Win32Common.cpp)

    message(STATUS "  Win32ProcessInfo")
    list(APPEND SOURCES rsc/os/Win32ProcessInfo.cpp)

    message(STATUS "  Win32HostInfo")
    list(APPEND SOURCES rsc/os/Win32HostInfo.cpp)

else()
    message(FATAL_ERROR "No Subprocess, DebugTools, ProcessInfo or HostInfo implementation is available for this platform")
endif()

add_library(${RSC_NAME} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${RSC_NAME} ${Boost_LIBRARIES}
                                  ${CMAKE_THREAD_LIBS_INIT})
# System libraries for dynamic loading (required by plugin system)
if(UNIX)
    target_link_libraries(${RSC_NAME} dl)
endif()
# Windows-specific libraries
if(WIN32)
    target_link_libraries(${RSC_NAME} Secur32 Psapi)
endif()
set_target_properties(${RSC_NAME}
                      PROPERTIES
                      VERSION ${SO_VERSION})

install(TARGETS ${RSC_NAME}
        EXPORT RSCDepends
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install_files_recursive("include/${INSTALL_PATH_PREFIX}" HEADERS)

include(InstallDebugSymbols)
install_debug_symbols(TARGETS ${RSC_NAME})
