/* ============================================================
 *
 * This file is a part of RSC project
 *
 * Copyright (C) 2013 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <gtest/gtest.h>

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "rsc/plugins/Plugin.h"
#include "rsc/plugins/Manager.h"
#include "rsc/plugins/Configurator.h"
#include "rsc/runtime/NoSuchObject.h"

#include  "testconfig.h"

using namespace std;
using namespace rsc;
using namespace rsc::plugins;
using namespace rsc::runtime;

class ConfiguratorTest: public ::testing::Test {
protected:

    ConfiguratorTest() {
    }

    virtual void SetUp() {
        // start over with a fresh instance
        pluginManager.reset(new Manager());
        pluginManager->addPath(
                boost::filesystem::path(TEST_PLUGIN_DIRECTORY));
    }

    ManagerPtr pluginManager;
    const vector<boost::filesystem::path> defaultPath;
};

TEST_F(ConfiguratorTest, testListOfPluginsWithPreviousValue) {

    Configurator c(this->pluginManager, this->defaultPath);
    vector<string> loadOption;
    loadOption.push_back("plugins");
    loadOption.push_back("cpp");
    loadOption.push_back("load");
    c.handleOption(loadOption, "testplugin:testplugin");
    c.handleOption(loadOption, "testplugin::");

    c.execute();

}


TEST_F(ConfiguratorTest, testNoExceptionSamePluginSpecifiedTwice) {

    Configurator c(this->pluginManager, this->defaultPath);
    vector<string> loadOption;
    loadOption.push_back("plugins");
    loadOption.push_back("cpp");
    loadOption.push_back("load");
    c.handleOption(loadOption, "testplugin");
    c.handleOption(loadOption, "testplugin");

    c.execute();

}

TEST_F(ConfiguratorTest, testExceptionIfPluginDoesNotExist) {

    Configurator c(this->pluginManager, this->defaultPath);
    vector<string> loadOption;
    loadOption.push_back("plugins");
    loadOption.push_back("cpp");
    loadOption.push_back("load");
    c.handleOption(loadOption, "doesntexist");

    EXPECT_THROW(c.execute(), NoSuchObject);

}

TEST_F(ConfiguratorTest, testNoExceptionIfPluginDoesNotExist) {

    Configurator c(this->pluginManager, this->defaultPath);
    vector<string> loadOption;
    loadOption.push_back("plugins");
    loadOption.push_back("cpp");
    loadOption.push_back("load");
    c.handleOption(loadOption, "doesntexist");

    c.execute(false);

}

TEST_F(ConfiguratorTest, testInvalidPluginPath) {
    Configurator c(this->pluginManager, this->defaultPath);

    vector<string> name;
    name.push_back("plugins");
    name.push_back("cpp");
    name.push_back("path");

    EXPECT_THROW(c.handleOption(name, "\\"), invalid_argument);
    EXPECT_THROW(c.handleOption(name, "C:\\"), invalid_argument);
}

TEST_F(ConfiguratorTest, testInvalidPluginLoad) {
    Configurator c(this->pluginManager, this->defaultPath);

    vector<string> name;
    name.push_back("plugins");
    name.push_back("cpp");
    name.push_back("load");

    EXPECT_THROW(c.handleOption(name, "\\"), invalid_argument);
}
