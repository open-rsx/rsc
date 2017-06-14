/* ============================================================
 *
 * This file is part of the RSC project.
 *
 * Copyright (C) 2012-2017 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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
 * ============================================================  */

#include "Manager.h"

#include <stdexcept>
#include <algorithm>

#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>

#include "../runtime/NoSuchObject.h"
#include "../logging/LoggerFactory.h"

using namespace std;

using namespace boost;
using namespace boost::filesystem;

namespace rsc {
namespace plugins {

Manager::Manager()
  : logger(logging::LoggerFactory::getInstance()
           .getLogger("rsc.plugins.Manager")) {
}

Manager::~Manager() = default;

vector<path> Manager::getPath() const {
  return this->path;
}

void Manager::addPath(const boost::filesystem::path& path) {
    // Ignore duplicates.
    if (find(this->path.begin(), this->path.end(), path)
        != this->path.end()) {
        return;
    }

    // Search specified directory for plugins.
    RSCINFO(this->logger, "Adding path " << path);
    this->path.push_back(path);
    if (!is_directory(path)) {
        RSCINFO(this->logger, "Ignoring non-existent path `" << path << "'");
        return;
    }

    set<string> namesInThisPath;
    for (directory_iterator it = directory_iterator(path);
         it != directory_iterator(); ++it) {
        RSCTRACE(this->logger, "Processing " << it->path());
        if (!is_regular_file(it->path())) {
            RSCINFO(this->logger, "Ignoring non-regular file " << it->path());
            continue;
        }

        // Extract the plugin name from the library name.
        string name = it->path()
#if BOOST_FILESYSTEM_VERSION == 3
            .filename().string();
#else
            .filename();
#endif
        // Strip leading "lib" and trailing ".so*", ".dylib*", etc
        regex libraryName (
#if defined(__linux__)
                "^lib([^.]*)(.*)\\.so(.*)$"
#elif defined(__APPLE__)
                "^lib([^.]*)(.*)\\.dylib$"
#elif defined(_WIN32)
                "^([^.]*)(.*)\\.dll$"
#else
                ""
#endif
                );
        if (!regex_match(name, libraryName)) {
            RSCINFO(this->logger, "Ignoring non-matching file " << name);
            continue;
        }

        name = regex_replace(name, libraryName, "\\1");
        if (namesInThisPath.count(name) != 0) {
            throw runtime_error(
                    boost::str(
                            boost::format(
                                    "The path %1% contains multiple plugins forming the plugin name %2%.")
                                    % path % name));
        }
        namesInThisPath.insert(name);

        boost::filesystem::path library = it->path();

        // If there is not yet an entry for the given name, add a
        // new plugin entry. Otherwise, ignore the plugin. This
        // logic implements precedence of searchpath entries.
        RSCINFO(this->logger, "Found plugin `"
                << name << "' [" << library << "]");
        if (this->plugins.find(name) == this->plugins.end()) {
            this->plugins[name]
                = Plugin::create(name, library.string());
        }
    }
}

set<PluginPtr> Manager::getPlugins(const boost::regex& regex) const {
    set<PluginPtr> result;
    for (auto entry : this->plugins) {
        if (regex_match(entry.first, regex)) {
            result.insert(entry.second);
        }
    }
    return result;
  }

set<PluginPtr> Manager::getPlugins(const string& regex) const {
    return getPlugins(boost::regex(regex));
}

PluginPtr Manager::getPlugin(const string& name) const {
    PluginMap::const_iterator it;
    if ((it = this->plugins.find(name)) == this->plugins.end()) {
      throw runtime::NoSuchObject(name);
    }
    return it->second;
}

}
}
