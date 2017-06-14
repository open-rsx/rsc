/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2011-2017 Jan Moringen
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

#include "ConfigFileSource.h"

#include <iterator>

#include <boost/format.hpp>
#include <boost/tokenizer.hpp>

#include "../runtime/ContainerIO.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;

namespace rsc {
namespace config {

// Taken from Boost.ProgramOptions
string trim(const string& s) {
    string::size_type n, n2;
    n = s.find_first_not_of(" \t\r\n");
    if (n == string::npos) {
        return string();
    } else {
        n2 = s.find_last_not_of(" \t\r\n");
        return s.substr(n, n2 - n + 1);
    }
}

ConfigFileSource::ConfigFileSource(istream& stream) :
    logger(Logger::getLogger("rsc.config.ConfigFileSource")), stream(stream) {

    // Parse option keys and values and store them in an internal map.
    // Option keys are of the form
    //
    //   COMPONENT1.COMPONENT2.COMPONENT3 ...
    //
    // Components of option keys can be quoted to allow embedded '.'
    // characters. For example:
    //
    //   logging."system.subsystem"
    //
    // would be parsed as option key
    //
    //   (logging system.subsystem) instead of
    //   (logging system subsystem)
    string name;
    string value;
    while (getOption(name, value)) {
        vector<string> key;
        splitKeyAtDots(name, key);

        RSCTRACE(logger, "Option " << key << " -> " << value);
        options[key] = value;
    }
}

void ConfigFileSource::provideOptions(OptionHandler& handler) {
    for (map<vector<string>, string>::const_iterator it = options.begin();
            it != options.end(); ++it) {
        handler.handleOption(it->first, it->second);
    }
}

// Based on Boost.ProgramOptions
bool ConfigFileSource::getOption(string& name, string& value) {
    string line;
    while (getline(this->stream, line)) {

        // check for files encoded with the old Mac EOL style.
        size_t rPos = line.find('\r');
        if (rPos != line.npos && rPos != 0 && rPos != line.size() - 1) {
            throw invalid_argument("Old Mac EOL style '\\r' is not supported.");
        }

        // Strip '#' comments and whitespace
        string::size_type n;
        if ((n = line.find('#')) != string::npos) {
            line = line.substr(0, n);
        }
        line = trim(line);

        if (!line.empty()) {
            // Handle section name
            if (*line.begin() == '[' && *line.rbegin() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                if (*currentSection.rbegin() != '.') {
                    currentSection += '.';
                }
            } else if ((n = line.find('=')) != string::npos) {
                name = currentSection + trim(line.substr(0, n));
                value = trim(line.substr(n + 1));
                return true;
            } else {
                throw invalid_argument(
                        str(format("Syntax error in line `%1%'") % line));
            }
        }
    }
    return false;
}

}
}
