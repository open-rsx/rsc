/* ============================================================
 *
 * This file is a part of RSC project
 *
 * Copyright (C) 2012 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <stdexcept>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsc/config/ConfigFileSource.h"
#include "rsc/logging/LoggerFactory.h"
#include "rsc/logging/LoggingSystemFactory.h"
#include "rsc/logging/OptionBasedConfigurator.h"

#include "testconfig.h"
#include "mocks.h"

using namespace std;
using namespace rsc;
using namespace rsc::config;
using namespace rsc::logging;
using namespace testing;

TEST(OptionBasedConfiguratorTest, testConfigFromFile) {

    LoggerFactory::getInstance().clearKnownLoggers();

    OptionBasedConfigurator configurator;
    boost::filesystem::ifstream stream(
            TEST_ROOT + "/rsc/logging/logging.config");
    ASSERT_TRUE(stream.good());
    ConfigFileSource(stream).provideOptions(configurator);

    EXPECT_EQ(Logger::LEVEL_TRACE, Logger::getLogger("")->getLevel());
    EXPECT_EQ(Logger::LEVEL_DEBUG, Logger::getLogger("sub.loGGer")->getLevel());
    EXPECT_EQ(Logger::LEVEL_TRACE,
            Logger::getLogger("another.logger")->getLevel());
    EXPECT_EQ(Logger::LEVEL_TRACE, Logger::getLogger("sub.foo")->getLevel());

}

TEST(OptionBasedConfiguratorTest, testDifferentRoot) {

    LoggerFactory::getInstance().clearKnownLoggers();

    vector<string> root;
    root.push_back("a");
    root.push_back("new");
    root.push_back("root");
    OptionBasedConfigurator configurator(root);
    boost::filesystem::ifstream stream(
            TEST_ROOT + "/rsc/logging/logging.config");
    ASSERT_TRUE(stream.good());
    ConfigFileSource(stream).provideOptions(configurator);

    EXPECT_EQ(Logger::LEVEL_OFF, Logger::getLogger("sub.logger")->getLevel());

}

TEST(OptionBasedConfiguratorTest, testFactoryConfiguration) {

    LoggerFactory::getInstance().clearKnownLoggers();
    LoggerFactory::getInstance().reconfigureFromFile(
            TEST_ROOT + "/rsc/logging/logging.config");

    EXPECT_EQ(Logger::LEVEL_TRACE, Logger::getLogger("")->getLevel());
    EXPECT_EQ(Logger::LEVEL_DEBUG, Logger::getLogger("sub.loGGer")->getLevel());
    EXPECT_EQ(Logger::LEVEL_TRACE,
            Logger::getLogger("another.logger")->getLevel());
    EXPECT_EQ(Logger::LEVEL_TRACE, Logger::getLogger("sub.foo")->getLevel());

}

/*TEST(OptionBasedConfiguratorTest, testSystemSelection) {

    LoggerFactory::getInstance().clearKnownLoggers();

    // Set up mock logging system
    const std::string systemName = "DummyLoggingSystem";
    LoggingSystemFactory::getInstance().impls()
        .register_(systemName, &StrictMock<MockLoggingSystem>::create);

    std::vector<std::string> root;
    root.push_back("system");
    root.push_back("config");
    OptionBasedConfigurator configurator(root);
    boost::filesystem::ifstream stream
        (TEST_ROOT + "/rsc/logging/logging.config");
    ASSERT_TRUE(stream.good());
    ConfigFileSource(stream).provideOptions(configurator);

    EXPECT_EQ("mock", LoggerFactory::getInstance().getLoggingSystemName());

    // Back to defaults
    LoggerFactory::getInstance().reselectLoggingSystem
        (LoggerFactory::DEFAULT_LOGGING_SYSTEM);

    LoggingSystemFactory::getInstance().impls().unregister(systemName);

}*/

TEST(OptionBasedConfiguratorTest, testCaseInsensitiveSettings) {

    LoggerFactory::getInstance().clearKnownLoggers();
    LoggerFactory::getInstance().reconfigureFromFile(
            TEST_ROOT + "/rsc/logging/loggingCaseInsensitive.config");

    EXPECT_EQ(Logger::LEVEL_TRACE, Logger::getLogger("")->getLevel());
    EXPECT_EQ(Logger::LEVEL_DEBUG, Logger::getLogger("sub.loGGer")->getLevel());
    EXPECT_EQ(Logger::LEVEL_TRACE,
            Logger::getLogger("another.logger")->getLevel());
    EXPECT_EQ(Logger::LEVEL_TRACE, Logger::getLogger("sub.foo")->getLevel());

}
