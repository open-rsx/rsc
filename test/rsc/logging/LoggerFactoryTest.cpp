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

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsc/logging/ConsoleLoggingSystem.h"
#include "rsc/logging/LoggerFactory.h"
#include "rsc/misc/langutils.h"

#include "mocks.h"

using namespace std;
using namespace rsc;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace testing;

TEST(LoggerFactoryTest, testSingleton) {
    EXPECT_EQ(&LoggerFactory::getInstance(), &LoggerFactory::getInstance());
}

TEST(LoggerFactoryTest, testSingletonLoggers) {
    LoggerFactory::getInstance().reconfigure(Logger::LEVEL_WARN);
    stringstream name;
    name << rand();
    LoggerPtr logger = LoggerFactory::getInstance().getLogger(name.str());
    EXPECT_EQ(Logger::LEVEL_WARN, logger->getLevel());
    EXPECT_EQ(logger, LoggerFactory::getInstance().getLogger(name.str()));
    EXPECT_NE(logger,
            LoggerFactory::getInstance().getLogger(name.str() + "blubb"));
}

TEST(LoggerFactoryTest, testReconfigure) {
    LoggerFactory::getInstance().reconfigure(Logger::LEVEL_ERROR);
    stringstream name;
    name << rand();
    LoggerPtr logger = LoggerFactory::getInstance().getLogger(name.str());
    LoggerPtr rootLogger = LoggerFactory::getInstance().getLogger("");
    EXPECT_EQ(Logger::LEVEL_ERROR, logger->getLevel());
    EXPECT_EQ(Logger::LEVEL_ERROR, rootLogger->getLevel());

    LoggerFactory::getInstance().reconfigure(Logger::LEVEL_TRACE);
    EXPECT_EQ(Logger::LEVEL_TRACE, logger->getLevel());
    EXPECT_EQ(Logger::LEVEL_TRACE, rootLogger->getLevel());
    stringstream name2;
    name2 << rand();
    LoggerPtr logger2 = LoggerFactory::getInstance().getLogger(name2.str());
    EXPECT_EQ(Logger::LEVEL_TRACE, logger2->getLevel());
}

TEST(LoggerFactoryTest, testReselectLoggingSystem) {

    LoggerFactory::getInstance().clearKnownLoggers();

    // ensure that there are other logging systems
    StrictMock<MockLoggingSystem>* l1 = new StrictMock<MockLoggingSystem>(
            randAlnumStr(25));
    loggingSystemRegistry()->addRegistree(l1);
    StrictMock<MockLoggingSystem>* l2 = new StrictMock<MockLoggingSystem>(
            randAlnumStr(25));
    loggingSystemRegistry()->addRegistree(l2);

    // force default
    LoggerFactory::getInstance().reselectLoggingSystem(
            LoggerFactory::DEFAULT_LOGGING_SYSTEM);
    EXPECT_EQ(ConsoleLoggingSystem::getName(),
            LoggerFactory::getInstance().getLoggingSystemName());
    // if the logging system is illegally reported by the line above the next
    // line will cause the mock object to report an error
    string dummyName = randAlnumStr(20);
    boost::algorithm::to_lower(dummyName);
    LoggerPtr logger(LoggerFactory::getInstance().getLogger(dummyName));

    // force hint
    string name = randAlnumStr(10);
    boost::algorithm::to_lower(name);
    boost::shared_ptr<StubLogger> nameLogger(new StubLogger(name));
    EXPECT_CALL(*l2, createLogger("")).Times(1).WillOnce(
            Return(LoggerPtr(new StubLogger(dummyName))));
    EXPECT_CALL(*l2, createLogger(dummyName)).Times(1).WillOnce(
            Return(LoggerPtr(new StubLogger(dummyName))));
    EXPECT_CALL(*l2, createLogger(name)).Times(1).WillOnce(Return(nameLogger));
    LoggerFactory::getInstance().reselectLoggingSystem(l2->name);
    EXPECT_EQ(l2->name, LoggerFactory::getInstance().getLoggingSystemName());
    logger = LoggerFactory::getInstance().getLogger(name);
    logger->debug("Narf");
    EXPECT_EQ(size_t(1), nameLogger->logs.size());

    LoggerFactory::getInstance().reselectLoggingSystem(
            LoggerFactory::DEFAULT_LOGGING_SYSTEM);
    loggingSystemRegistry()->removeRegistree(l1->name);

    // wrong hint, fallback to other available system
    string newName = randAlnumStr(12);
    boost::algorithm::to_lower(newName);
    EXPECT_CALL(*l2, createLogger("")).Times(1).WillOnce(
            Return(LoggerPtr(new StubLogger(dummyName))));
    EXPECT_CALL(*l2, createLogger(dummyName)).Times(1).WillOnce(
            Return(LoggerPtr(new StubLogger(dummyName))));
    EXPECT_CALL(*l2, createLogger(name)).Times(1).WillOnce(
            Return(LoggerPtr(new StubLogger(name))));
    EXPECT_CALL(*l2, createLogger(newName)).Times(1).WillOnce(
            Return(LoggerPtr(new StubLogger(newName))));
    LoggerFactory::getInstance().reselectLoggingSystem(randAlnumStr(123));
    EXPECT_EQ(l2->name, LoggerFactory::getInstance().getLoggingSystemName());
    logger = LoggerFactory::getInstance().getLogger(newName);

    // The original nameLogger must not be touched anymore, because we now have
    // a new logging system
    logger->debug("Narf 2");
    EXPECT_EQ(size_t(1), nameLogger->logs.size());

    // cleanup mocks
    LoggerFactory::getInstance().reselectLoggingSystem(
            LoggerFactory::DEFAULT_LOGGING_SYSTEM);
    loggingSystemRegistry()->removeRegistree(l2->name);

    LoggerFactory::getInstance().clearKnownLoggers();

}

TEST(LoggerFactoryTest, testHierarchicalNames) {

    LoggerFactory::killInstance();

    LoggerPtr rootLogger = LoggerFactory::getInstance().getLogger("");
    LoggerPtr level1Logger = LoggerFactory::getInstance().getLogger("test");
    LoggerPtr level2Logger = LoggerFactory::getInstance().getLogger("test.it");
    LoggerPtr level3Logger = LoggerFactory::getInstance().getLogger(
            "test.it.now");
    LoggerPtr level4Logger = LoggerFactory::getInstance().getLogger(
            "test.it.now.really");

    EXPECT_EQ("", rootLogger->getName());
    EXPECT_EQ("test", level1Logger->getName());
    EXPECT_EQ("test.it", level2Logger->getName());
    EXPECT_EQ("test.it.now", level3Logger->getName());
    EXPECT_EQ("test.it.now.really", level4Logger->getName());

}

TEST(LoggerFactoryTest, testHierarchicalLevels) {

    LoggerFactory::killInstance();

    LoggerPtr rootLogger = LoggerFactory::getInstance().getLogger("");
    LoggerPtr level1Logger = LoggerFactory::getInstance().getLogger("test");
    LoggerPtr level2Logger = LoggerFactory::getInstance().getLogger("test.it");
    LoggerPtr level3Logger = LoggerFactory::getInstance().getLogger(
            "test.it.now");
    LoggerPtr level4Logger = LoggerFactory::getInstance().getLogger(
            "test.it.now.really");

    EXPECT_EQ(LoggerFactory::DEFAULT_LEVEL, rootLogger->getLevel());
    EXPECT_EQ(LoggerFactory::DEFAULT_LEVEL, level1Logger->getLevel());
    EXPECT_EQ(LoggerFactory::DEFAULT_LEVEL, level2Logger->getLevel());
    EXPECT_EQ(LoggerFactory::DEFAULT_LEVEL, level3Logger->getLevel());

    ASSERT_NE(LoggerFactory::DEFAULT_LEVEL, Logger::LEVEL_DEBUG);
    ASSERT_NE(LoggerFactory::DEFAULT_LEVEL, Logger::LEVEL_TRACE);

    level3Logger->setLevel(Logger::LEVEL_TRACE);
    EXPECT_EQ(level3Logger->getLevel(), Logger::LEVEL_TRACE);
    EXPECT_EQ(level4Logger->getLevel(), Logger::LEVEL_TRACE);

    rootLogger->setLevel(Logger::LEVEL_DEBUG);
    EXPECT_EQ(rootLogger->getLevel(), Logger::LEVEL_DEBUG);
    EXPECT_EQ(level1Logger->getLevel(), Logger::LEVEL_DEBUG);
    EXPECT_EQ(level2Logger->getLevel(), Logger::LEVEL_DEBUG);
    EXPECT_EQ(level3Logger->getLevel(), Logger::LEVEL_TRACE);
    EXPECT_EQ(level4Logger->getLevel(), Logger::LEVEL_TRACE);

}

TEST(LoggerFactoryTest, testRootLoggerCorrectLevelAfterReselect) {

    LoggerFactory::killInstance();

    ASSERT_NE(Logger::LEVEL_FATAL,
            LoggerFactory::getInstance().getLogger("")->getLevel());
    LoggerFactory::getInstance().getLogger("")->setLevel(Logger::LEVEL_FATAL);
    LoggerFactory::getInstance().reselectLoggingSystem(
            ConsoleLoggingSystem::getName());
    EXPECT_EQ(Logger::LEVEL_FATAL,
            LoggerFactory::getInstance().getLogger("")->getLevel());

}

TEST(LoggerFactoryTest, testReconfigureSameSemantics) {

    LoggerFactory::killInstance();
    LoggerFactory& factory = LoggerFactory::getInstance();

    LoggerPtr rootLogger = factory.getLogger();
    LoggerPtr childLevel1 = factory.getLogger("this");
    LoggerPtr childLevel2 = factory.getLogger("this.is");
    LoggerPtr childLevel3 = factory.getLogger("this.is.achild");

    childLevel2->setLevel(Logger::LEVEL_ERROR);
    EXPECT_EQ(LoggerFactory::DEFAULT_LEVEL, rootLogger->getLevel());
    EXPECT_EQ(LoggerFactory::DEFAULT_LEVEL, childLevel1->getLevel());
    EXPECT_EQ(Logger::LEVEL_ERROR, childLevel2->getLevel());
    EXPECT_EQ(Logger::LEVEL_ERROR, childLevel3->getLevel());

    factory.reconfigure(Logger::LEVEL_FATAL);
    EXPECT_EQ(Logger::LEVEL_FATAL, rootLogger->getLevel());
    EXPECT_EQ(Logger::LEVEL_FATAL, childLevel1->getLevel());
    EXPECT_EQ(Logger::LEVEL_FATAL, childLevel2->getLevel());
    EXPECT_EQ(Logger::LEVEL_FATAL, childLevel3->getLevel());

    // after reconfiguration add a new child and see that this child has
    // the right derived level
    EXPECT_EQ(Logger::LEVEL_FATAL,
            factory.getLogger("this.is.achild.again")->getLevel());

    // now, changing level1 must not change children as level had an assigned level
    childLevel1->setLevel(Logger::LEVEL_OFF);
    EXPECT_EQ(Logger::LEVEL_OFF, childLevel1->getLevel());
    EXPECT_EQ(Logger::LEVEL_FATAL, childLevel2->getLevel());
    EXPECT_EQ(Logger::LEVEL_FATAL, childLevel3->getLevel());

}

TEST(LoggerFactoryTest, testCaseInsensitiveNames) {

    LoggerFactory::killInstance();
    LoggerFactory& factory = LoggerFactory::getInstance();

    LoggerPtr upperCaseLogger = factory.getLogger("THIS.IS.A.TEST");
    LoggerPtr lowerCaseLogger = factory.getLogger("this.is.a.test");

    EXPECT_EQ(upperCaseLogger.get(), lowerCaseLogger.get());
    EXPECT_EQ(upperCaseLogger->getName(), lowerCaseLogger->getName());

    EXPECT_EQ("this.is.a.test", upperCaseLogger->getName()) << "Names should be all lower-case";

}

TEST(LoggerFactoryTest, testInvalidLoggerNames) {

    // Logger names ending with level or system (case-insensitive) are not
    // allowed as these are reserved keywords

    LoggerFactory::killInstance();
    LoggerFactory& factory = LoggerFactory::getInstance();

    EXPECT_THROW(factory.getLogger("LEVEL"), invalid_argument);
    EXPECT_THROW(factory.getLogger("level"), invalid_argument);
    EXPECT_THROW(factory.getLogger("LeVeL"), invalid_argument);
    EXPECT_THROW(factory.getLogger("test.LEVEL"), invalid_argument);
    EXPECT_THROW(factory.getLogger("test.level"), invalid_argument);
    EXPECT_THROW(factory.getLogger("test.LeVeL"), invalid_argument);

    EXPECT_THROW(factory.getLogger("SYSTEM"), invalid_argument);
    EXPECT_THROW(factory.getLogger("system"), invalid_argument);
    EXPECT_THROW(factory.getLogger("SysTem"), invalid_argument);
    EXPECT_THROW(factory.getLogger("test.SYSTEM"), invalid_argument);
    EXPECT_THROW(factory.getLogger("test.system"), invalid_argument);
    EXPECT_THROW(factory.getLogger("test.SysTem"), invalid_argument);

}
