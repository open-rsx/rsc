/* ============================================================
 *
 * This file is a part of RSC project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "rsc/runtime/Properties.h"

#include "rsc/logging/Logger.h"
#include "rsc/logging/LoggingSystem.h"

class MockLogger: public rsc::logging::Logger {
public:
    MOCK_CONST_METHOD0(getLevel, rsc::logging::Logger::Level());MOCK_METHOD1(setLevel, void(const Level& level));MOCK_CONST_METHOD0(getName, std::string());MOCK_METHOD1(setName, void(const std::string& name));MOCK_METHOD2(log, void(const rsc::logging::Logger::Level& level, const std::string& msg));
};

class MockLoggingSystem: public rsc::logging::LoggingSystem {
public:
    const std::string getName() const {
        return "mock";
    }

    MOCK_METHOD1(createLogger, rsc::logging::LoggerPtr(const std::string& name));

    static MockLoggingSystem* create(const rsc::runtime::Properties& /*properties*/) {
        return new MockLoggingSystem();
    }

};

class StubLogger: public rsc::logging::Logger {
private:
    std::string name;
    Logger::Level level;
public:

    StubLogger(const std::string& name) :
            name(name) {
    }

    Logger::Level getLevel() const {
        return level;
    }

    void setLevel(const Logger::Level& level) {
        this->level = level;
    }

    std::string getName() const {
        return name;
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    std::vector<std::pair<Logger::Level, std::string> > logs;

    void log(const Logger::Level& level, const std::string& msg) {
        logs.push_back(make_pair(level, msg));
    }

};
