/* ============================================================
 *
 * This file is part of the RSC project
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
 * ============================================================ */

#include "Configuration.h"

#include <stdexcept>

#include <boost/tuple/tuple.hpp> // for boost::tie
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/fstream.hpp>

#include "../logging/Logger.h"
#include "../logging/LoggerFactory.h"

#include "Environment.h"
#include "ConfigFileSource.h"
#include "CommandLinePropertySource.h"

#include "Utility.h"

using namespace std;

using namespace rsc::logging;

namespace rsc {
namespace config {

const std::string DEFAULT_DEBUG_VARIABLE_NAME = "__CONFIG_DEBUG";

const std::string DEFAULT_FILE_VARIABLE_NAME = "__CONFIG_FILES";

const std::string CONFIG_FILE_KEY_PREFIX = "%prefix";
const std::string CONFIG_FILE_KEY_USER   = "%user";
const std::string CONFIG_FILE_KEY_PWD    = "%pwd";

LoggerPtr getLogger() {
    static LoggerPtr logger
    = logging::LoggerFactory::getInstance().getLogger("rsc.config.configure()");
    return logger;
}

std::vector<std::string>
defaultConfigurationFiles(const std::string& fileVariableName) {
    boost::shared_ptr<std::string> files;
    if ((files = getEnvironmentVariable(fileVariableName))) {
        return splitSequenceValue(*files);
    }

    std::vector<std::string> result;
    result.push_back(CONFIG_FILE_KEY_PREFIX);
    result.push_back(CONFIG_FILE_KEY_USER);
    result.push_back(CONFIG_FILE_KEY_PWD);
    return result;
}

std::pair<boost::filesystem::path, std::string>
resolveConfigurationFile(const std::string&             spec,
                         const boost::filesystem::path& prefix,
                         const std::string&             configFileName) {
    if (spec == CONFIG_FILE_KEY_PREFIX) {
        return std::make_pair(prefixConfigDirectory(prefix)
                              / configFileName,
                              "Prefix wide config file");
    }
    if (spec == CONFIG_FILE_KEY_USER) {
        return std::make_pair(userConfigDirectory()
                              / configFileName,
                              "User config file");
    }
    if (spec == CONFIG_FILE_KEY_PWD) {
        return std::make_pair(configFileName,
                              "Current directory file");
    }
    return std::make_pair(boost::filesystem::path(spec),
                          "User specified config file");
}

void describeFileStream(const std::string&             label,
                        const boost::filesystem::path& path,
                        const istream&                 stream) {
    cerr << "  " << label << " " << path << " "
         << (stream ? "exists" : "does not exist") << endl;
}

void processConfigFile(unsigned int                   index,
                       const std::string&             spec,
                       const boost::filesystem::path& prefix,
                       const std::string&             configFileName,
                       bool                           debug,
                       OptionHandler&                 handler) {
    boost::filesystem::path file;
    std::string             description;
    try {
        boost::tie(file, description)
            = resolveConfigurationFile(spec, prefix, configFileName);
    } catch (const runtime_error& e) {
        RSCWARN(getLogger(),
                "Failed to resolve configuration file designated by "
                << spec << "': " << e.what());
        return;
    }
    try {

        boost::filesystem::ifstream stream(file);
        if (debug) {
            describeFileStream("   " + boost::lexical_cast<string>(index) + ". "
                               + description,
                               file, stream);
        }
        if (stream) {
            ConfigFileSource source(stream);
            source.provideOptions(handler);
        }
    } catch (const runtime_error& e) {
        RSCWARN(getLogger(),
                "Failed to process " << description << " `"
                << file << "': " << e.what());
    }
}

void configure(OptionHandler&                  handler,
               const string&                   configFileName,
               const string&                   environmentVariablePrefix,
               int                             argc,
               const char**                    argv,
               bool                            stripEnvironmentVariablePrefix,
               const boost::filesystem::path&  prefix,
               const std::string&              debugVariableName,
               const std::vector<std::string>& configurationFiles) {
    bool debug = getEnvironmentVariable(debugVariableName).get();

    // 0) In debug mode, header first.
    if (debug) {
        cerr << "Configuring with sources (lowest priority first)" << endl;
    }

    // 1) Configuration files
    //    (lowest priority)
    if (debug) {
        cerr << "  1. Configuration files" << endl;
    }
    unsigned int index = 1;
    for (auto file : configurationFiles) {
        processConfigFile(index++, file, prefix, configFileName, debug, handler);
    }

    // 2) Add environment Variables
    {
        EnvironmentVariableSource source(environmentVariablePrefix,
                stripEnvironmentVariablePrefix);
        if (debug) {
            cerr << "  2. Environment variables with prefix "
                 << environmentVariablePrefix << endl;
            EnvironmentVariableSource::Matches matches = source.getMatches();
            if (!matches.empty()) {
                for (EnvironmentVariableSource::Matches::const_iterator it
                         = matches.begin(); it != matches.end(); ++it) {
                    cerr << "     "
                         << it->getRawName();
                    if (it->getRawName() != it->getTransformedName()) {
                        cerr << " (mapped to "
                             << it->getTransformedName()
                             << ")";
                    }
                    cerr << " -> " << it->getValue()
                         << endl;
                }
            } else {
                cerr << "     <none>" << endl;
            }
        }
        source.provideOptions(handler);
    }

    // 3) Command line
    //    (highest priority)
    if (debug) {
        cerr << "  3. Commandline options" << endl;
    }
    if (argc > 0) {
        if (debug){
            for (int i = 0; i < argc; ++i) {
                cerr << "     " << argv[i] << endl;
            }
        }
        CommandLinePropertySource source(argc, argv);
        source.provideOptions(handler);
    } else {
        if (debug) {
            cerr << "     <none>" << endl;
        }
    }

}

}
}
