/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "ConsoleLogger.h"

#include "../misc/langutils.h"

using namespace std;

namespace rsc {
namespace logging {

ConsoleLogger::ConsoleLogger(const string& name, const Level& level) :
    name(name), level(level) {
}

ConsoleLogger::~ConsoleLogger() {
}

Logger::Level ConsoleLogger::getLevel() const {
    boost::recursive_mutex::scoped_lock lock(mutex);
    return level;
}

void ConsoleLogger::setLevel(const Logger::Level& level) {
    boost::recursive_mutex::scoped_lock lock(mutex);
    this->level = level;
}

string ConsoleLogger::getName() const {
    boost::recursive_mutex::scoped_lock lock(mutex);
    return name;
}

void ConsoleLogger::setName(const string& name) {
    boost::recursive_mutex::scoped_lock lock(mutex);
    this->name = name;
}

ostream& ConsoleLogger::printHeader(ostream& stream, const Level& level) {
    return stream << rsc::misc::currentTimeMillis() << " " << this->name
            << " [" << level << "]: ";
}

std::ostream& ConsoleLogger::printBody(std::ostream&      stream,
                                       const Level&       level,
                                       const std::string& msg) {
    return stream << msg << endl;
}

void ConsoleLogger::log(const Level& level, const string& msg) {
    boost::recursive_mutex::scoped_lock lock(mutex);
    if (isEnabledFor(level)) {
        std::ostream& stream = printHeader(cerr, level);
        printBody(stream, level, msg);
    }
}

}
}
