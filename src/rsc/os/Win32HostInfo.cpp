/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2014, 2018 Jan Moringen
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#include "HostInfo.h"

#define _WIN32_WINNT 0x0600 // For GetTickCount64 as per MSDN
#define SECURITY_WIN32
#include <windows.h>
#include <security.h>

#include <stdexcept>
#include <sstream>

#include <boost/format.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Win32Common.h"

namespace rsc {
namespace os {

// {Machine,Software} {Type,Version}

std::string currentMachineType () {
    throw std::runtime_error("not available");
}

std::string currentMachineVersion () {
    throw std::runtime_error("not available");
}

std::string currentSoftwareType () {
    return "win32";
}

std::string currentSoftwareVersion () {
    throw std::runtime_error("not available");
}

// Hostname

const unsigned int HOSTNAME_MAX_LENGTH = 1024;

std::string currentHostname() {
    char buffer[HOSTNAME_MAX_LENGTH];
    long unsigned int length = HOSTNAME_MAX_LENGTH;
    if (GetComputerName(buffer, &length) != 0) {
        return std::string(buffer, length);
    } else {
        throw std::runtime_error(
                boost::str(
                        boost::format("GetComputerName failed: %1%")
                                % GetLastErrorString()));
    }
}

// Host ID

const unsigned int HOST_ID_MAX_LENGTH = 1024;

std::string currentHostId() {
    char buffer[HOST_ID_MAX_LENGTH];
    long unsigned int length = HOST_ID_MAX_LENGTH;
    if (GetComputerObjectName(NameUniqueId, buffer, &length) != 0) {
        return std::string(buffer, length);
    } else {
        throw std::runtime_error(
                boost::str(boost::format("GetComputerObjectName(NameUniqueId)"
                        " failed: %1%") % GetLastErrorString()));
    }
}

// Boot time

boost::posix_time::ptime currentBootTime() {
    // at least an approximation due to runtime differences in the two calls
    boost::uint64_t millisSinceBoot = GetTickCount64();
    return boost::posix_time::microsec_clock::local_time()
            - boost::posix_time::millisec(millisSinceBoot);
}

}
}
