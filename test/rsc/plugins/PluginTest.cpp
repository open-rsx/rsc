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
#include "rsc/runtime/NoSuchObject.h"

#include  "testconfig.h"

using namespace std;
using namespace rsc;
using namespace rsc::plugins;
using namespace rsc::runtime;

class PluginTest: public ::testing::Test {
protected:

    virtual void SetUp() {
        // start over with a fresh instance
        Manager::killInstance();
        Manager::getInstance().addPath(TEST_PLUGIN_DIRECTORY);
    }

};

TEST_F(PluginTest, testName) {

    // this is a kind of indirect test, things will throw if the name is wrong
    EXPECT_NO_THROW(Manager::getInstance().getPlugin("testplugin"));
    EXPECT_NO_THROW(Manager::getInstance().getPlugin("testplugin-a-comPlex42NAME"));
    EXPECT_NO_THROW(Manager::getInstance().getPlugin("testplugin-with"))<< "So far we assume plugin names are stripped at the first dot.";

    EXPECT_THROW(Manager::getInstance().getPlugin("iDoNotExist"), NoSuchObject);

}

TEST_F(PluginTest, testLoadingAndShutdown) {

    boost::filesystem::path callFilePath(PLUGIN_CALL_FILE);
    boost::filesystem::create_directories(callFilePath.parent_path());
    boost::filesystem::remove(callFilePath);

    PluginPtr plugin = Manager::getInstance().getPlugin("testplugin");

    plugin->load();
    EXPECT_TRUE(boost::filesystem::exists(callFilePath));
    ifstream callFile(callFilePath.c_str());
    string callFileContent((istreambuf_iterator<char>(callFile)),
            (istreambuf_iterator<char>()));
    boost::algorithm::trim(callFileContent);
    EXPECT_EQ("INIT", callFileContent);

    plugin->unload();
    callFile.seekg(0);
    callFileContent = string((istreambuf_iterator<char>(callFile)),
            (istreambuf_iterator<char>()));
    boost::algorithm::trim(callFileContent);
    vector<string> parts;
    boost::algorithm::split(parts, callFileContent, boost::is_any_of("\n\r"),
            boost::token_compress_on);
    ASSERT_EQ(size_t(2), parts.size());
    EXPECT_EQ("SHUTDOWN", parts[1]);

}

TEST_F(PluginTest, testMissingSymbols) {

    PluginPtr plugin = Manager::getInstance().getPlugin("testplugin-missing-init");
    EXPECT_THROW(plugin->load(), runtime_error);
    EXPECT_THROW(plugin->unload(), runtime_error)<< "Unloading a plugin which was not loaded correctly must be an error condition.";

    plugin = Manager::getInstance().getPlugin("testplugin-missing-shutdown");
    EXPECT_THROW(plugin->load(), runtime_error) << "It must not be possible to load a pugin with missing symbols.";

}
