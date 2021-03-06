/* ============================================================
 *
 * This file is part of the RSC project.
 *
 * Copyright (C) 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "SGRConsoleLogger.h"

#include "../misc/langutils.h"

namespace rsc {
namespace logging {

SGRConsoleLogger::SGRConsoleLogger(const std::string& name) :
    ConsoleLogger(name) {
}

SGRConsoleLogger::~SGRConsoleLogger() {}

std::ostream& SGRConsoleLogger::printHeader(std::ostream& stream, const Level& level) {
    return (stream
            << "[34m" << rsc::misc::currentTimeMillis() << "[0m"
            << " [1m" << this->name << "[0m"
            << " [31m[" << level << "][0m: ");
}

std::ostream& SGRConsoleLogger::printBody(std::ostream&      stream,
                                          const Level&       level,
                                          const std::string& msg) {
    switch (level) {
    case LEVEL_TRACE:
    case LEVEL_DEBUG:
        return stream << "[2m" << msg << "[0m" << std::endl;
        break;
    case LEVEL_WARN:
        return stream << "[32m" << msg << "[0m" << std::endl;
        break;
    case LEVEL_ERROR:
        return stream << "[31m" << msg << "[0m" << std::endl;
        break;
    case LEVEL_FATAL:
        return stream << "[31;1m" << msg << "[0m" << std::endl;
        break;
    default:
        return stream << msg << std::endl;
        break;
    }
}

}
}
