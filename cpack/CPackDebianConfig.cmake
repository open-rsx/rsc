include(CheckLSBTypes)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Cannot configure CPack to generate Debian packages on non-linux systems.")
endif()

# To create a proper Debian/Ubuntu package, the following CMake
# options should be used:

# SET(BUILD_TESTS OFF)
# SET(BUILD_EXAMPLES OFF)

# SET(CMAKE_BUILD_TYPE Release)
set(CPACK_STRIP_FILES TRUE)

#set(CPACK_INSTALL_PREFIX "/usr")
set(CPACK_SET_DESTDIR On)
set(CMAKE_INSTALL_PREFIX ${CPACK_INSTALL_PREFIX})

# Actual packaging options

set(VERSION_SUFFIX "${RSC_VERSION_MAJOR}.${RSC_VERSION_MINOR}")

set(CPACK_GENERATOR "DEB")

set(CPACK_DEBIAN_PACKAGE_NAME        "lib${RSC_NAME}")
set(CPACK_DEBIAN_PACKAGE_VERSION     "${CPACK_PACKAGE_VERSION}${CPACK_PACKAGE_REVISION}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Sebastian Wrede <swrede@techfak.uni-bielefeld.de>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Library of reusable C++ and CMake code.
 RSC is a project focused on all aspects of reusable library functions
 in robotics systems. RSC is meant to improve the C++ std lib for
 robotic systems without pulling in many additional dependencies.")
set(CPACK_DEBIAN_PACKAGE_PRIORITY    "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION     "devel")
#SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE        ${CMAKE_SYSTEM_PROCESSOR}) # Debian uses different names here
set(CPACK_DEBIAN_PACKAGE_DEPENDS     "libc6, libboost-signals-dev, libboost-program-options-dev, libboost-filesystem-dev, libboost-thread-dev")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS    "cmake-data, cmake, doxygen, cppcheck, sloccount, lcov")

set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}")

set(SHLIBS_FILE     "${CMAKE_CURRENT_BINARY_DIR}/shlibs")
set(POSTINST_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/postinst")
set(POSTRM_SCRIPT   "${CMAKE_CURRENT_BINARY_DIR}/postrm")

# Generate postinst, prerm and postrm hooks
file(WRITE "${POSTINST_SCRIPT}" "#!/bin/sh\nset -e\n\n")
file(WRITE "${POSTRM_SCRIPT}"   "#!/bin/sh\nset -e\n\n")

# Generate shlibs
file(WRITE "${SHLIBS_FILE}"
     "lib${RSC_NAME} ${SO_VERSION} ${CPACK_DEBIAN_PACKAGE_NAME}")
file(APPEND "${POSTINST_SCRIPT}"
     "if [ \"$1\" = \"configure\" ]; then
        ldconfig
fi
")
file(APPEND "${POSTRM_SCRIPT}"
     "if [ \"$1\" = \"remove\" ]; then
        ldconfig
fi")

execute_process(COMMAND chmod 644 "${SHLIBS_FILE}")
execute_process(COMMAND chmod 755 "${POSTINST_SCRIPT}" "${POSTRM_SCRIPT}")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${SHLIBS_FILE};${POSTINST_SCRIPT};${POSTRM_SCRIPT}")

# Write license file
file(WRITE "${CMAKE_BINARY_DIR}/copyright"
     "Copyright (C) 2010-2018 ${CPACK_DEBIAN_PACKAGE_MAINTAINER}

   This software may be licensed under the terms of the
   GNU Lesser General Public License Version 3 (the ``LGPL''),
   or (at your option) any later version.

   Software distributed under the License is distributed
   on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
   express or implied. See the LGPL for the specific language
   governing rights and limitations.

   You should have received a copy of the LGPL along with this
   program. If not, go to http://www.gnu.org/licenses/lgpl.html
   or write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

On Debian systems, the complete text of the GNU Lesser General Public
License can be found in `/usr/share/common-licenses/LGPL-3'.")
install(FILES "${CMAKE_BINARY_DIR}/copyright"
        DESTINATION "share/doc/${CPACK_DEBIAN_PACKAGE_NAME}")

# Generate required change log files
execute_process(COMMAND ${GIT_EXECUTABLE}
                        log "--format=%ad  %an  <%ae>%n%n%w(76,8,10)%s%w(76,8,8)%n%n%b%n"
                        --date=short
                COMMAND gzip -n -9
                OUTPUT_FILE "${CMAKE_BINARY_DIR}/changelog.gz")
execute_process(COMMAND sh -c "echo -n \"sed -e '\" ; for c in $(${GIT_EXECUTABLE} rev-list --all -- \"${CMAKE_CURRENT_LIST_FILE}\") ; do echo -n \"s/$c/$(${GIT_EXECUTABLE} describe --tags $c | sed -re s/[^0-9]*\\([0-9]+\\)\\.\\([0-9]+\\)-\\([0-9]+\\)-.*/\\\\1.\\'\\$\\(\\(\\\\2+1\\)\\)\\'.\\\\3/)/\\;\" ; done ; echo \"'\""
                OUTPUT_VARIABLE RULES)
execute_process(COMMAND ${GIT_EXECUTABLE}
                        log "--format=${CPACK_DEBIAN_PACKAGE_NAME} (%H) ${LSB_CODENAME}; urgency=low%n%n%w(76,8,10)%s%w(76,8,8)%n%n%b%n%n%w(200,1,1)-- %an <%ae>  %ad%n"
                        --date=rfc
                        -- "${CMAKE_CURRENT_LIST_FILE}"
                COMMAND sh -c ${RULES}
                COMMAND gzip -n -9
                OUTPUT_FILE "${CMAKE_BINARY_DIR}/changelog.Debian.gz")
install(FILES "${CMAKE_BINARY_DIR}/changelog.gz"
              "${CMAKE_BINARY_DIR}/changelog.Debian.gz"
        DESTINATION "share/doc/${CPACK_DEBIAN_PACKAGE_NAME}")

message(STATUS "Debian Package: ${CPACK_DEBIAN_PACKAGE_NAME} (${CPACK_DEBIAN_PACKAGE_VERSION}) [${CPACK_PACKAGE_FILE_NAME}.deb]")
