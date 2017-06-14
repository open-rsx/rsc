/* ============================================================
 *
 * This file is a part of RSC project
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

#pragma once

#include <string>

#include "../runtime/Properties.h"

#include "LoggingSystem.h"

#include "rsc/rscexports.h"

namespace rsc {
namespace logging {

/**
 * Default logging system using the console for output.
 *
 * @author jwienke
 */
class RSC_EXPORT ConsoleLoggingSystem: public LoggingSystem {
public:

    ConsoleLoggingSystem();
    virtual ~ConsoleLoggingSystem();

    const std::string getName() const;

    LoggerPtr createLogger(const std::string& name);

    static ConsoleLoggingSystem* create(const runtime::Properties& properties);

private:

    /*
     * True when SGRConsoleLoggers should be used. Usually initialized
     * by calling isatty(2).
     */
    bool canUseSGRs;

};

}
}
