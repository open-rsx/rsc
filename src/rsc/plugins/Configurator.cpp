/* ============================================================
 *
 * This file is part of the RSC project.
 *
 * Copyright (C) 2012-2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Configurator.h"

#include <boost/format.hpp>

#include <boost/tokenizer.hpp>

#include "../runtime/ContainerIO.h"
#include "../runtime/NoSuchObject.h"

#include "../config/Utility.h"

#include "../logging/LoggerFactory.h"

using namespace std;

using namespace boost;
using namespace boost::filesystem;

namespace rsc {
namespace plugins {

Configurator::Configurator(ManagerPtr                             manager,
                           const vector<boost::filesystem::path>& defaultPath)
    : logger(logging::LoggerFactory::getInstance()
             .getLogger("rsc.plugins.Configurator")),
      manager(manager),
      defaultPath(defaultPath) {
    // Gets interpreted as default path entries in execute().
    this->path.push_back("");
}

Configurator::~Configurator() = default;

void Configurator::execute(bool errorOnMissing) {
    addPathEntries(this->path);
    loadPlugins(this->load, errorOnMissing);
}

void Configurator::handleOption(const vector<string>& key,
                                const string& value) {
    RSCDEBUG(this->logger, "Processing option " << key << " = `" << value << "'");

    // Ignore other options.
    if (!((key.size() == 3)
          && (key[0] == "plugins")
          && (key[1] == "cpp"))) {
        return;
    }

    // Process plugins.cpp.{path,load} options.
    if (key[2] == "path") {
        this->path = config::mergeSequenceValue("plugin load path",
                                                key, value, this->path);
    } else if (key[2] == "load") {
        this->load = config::mergeSequenceValue("list of plugins to load",
                                                key, value, this->load);
    } else {
        throw invalid_argument(str(format("Invalid option key `%1%'; plugin related option keys are `path' and `load'.")
                                   % boost::io::group(std::container_none,
                                                      std::element_sequence(".", ""),
                                                      key)));
    }
}

void Configurator::addDefaultPath() {
    for (vector<boost::filesystem::path>::const_iterator it = this->defaultPath.begin();
         it != this->defaultPath.end(); ++it) {
        this->manager->addPath(*it);
    }
}

void Configurator::addPathEntries(const vector<string>& entries) {
    for (auto path : entries) {
        try {
            if (path.empty()) {
                path = "<default path>"; // for exception message
                RSCINFO(this->logger, "Adding default searchpath: "
                        << this->defaultPath);
                addDefaultPath();
            } else {
                this->manager->addPath(path);
            }
        } catch (const std::exception& e) {
            throw runtime_error(str(format("Failed to add path `%1%' as requested via configuration: %2%")
                                    % path
                                    % e.what()));
        }
    }
}

void Configurator::loadPlugins(const vector<string>& names,
                               bool errorOnMissing) {
    for (auto pattern : names) {
        // Treat each element as a regular expression. Find matching
        // plugins and load them. Note that regex syntax error can
        // cause an exception here.
        RSCDEBUG(this->logger, "Looking up plugins matching pattern `"
                 << pattern << "'");
        set<PluginPtr> matches;
        try {
            matches = this->manager->getPlugins(boost::regex(pattern));
        } catch (const std::exception& e) {
            throw runtime_error(str(format("Failed to lookup matching plugins for patterns `%1%' as requested via configuration: %2%")
                                    % pattern
                                    % e.what()));
        }
        RSCDEBUG(this->logger, "Found " << matches.size() << " match(es)");

        if (matches.empty() && errorOnMissing) {
            throw runtime::NoSuchObject(str(format(
                                                "Cannot find a plugin with name %1%") % pattern));
        }

        // Try to load all plugins matching the pattern.
        for (auto match : matches) {
            RSCDEBUG(this->logger, "Loading plugin " << match->getName());
            try {
                match->ensureLoaded();
            } catch (const std::exception& e) {
                throw runtime_error(str(format("Failed to load plugin `%1%' as requested via configuration: %2%")
                                        % match->getName()
                                        % e.what()));
            }
        }
    }
}

}
}
