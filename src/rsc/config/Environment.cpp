/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2011, 2012 Jan Moringen
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

#include "Environment.h"

#include <algorithm>
#include <iterator>

// For dynamic libs the environ variable is not available
// on MacOS, hence a workaround is needed. See also:
// http://article.gmane.org/gmane.comp.lib.boost.devel/103843
#if defined(__APPLE__)  && defined(__DYNAMIC__)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/environment_iterator.hpp>

#include <rsc/runtime/ContainerIO.h>

using namespace std;

using namespace boost;
using namespace boost::filesystem;

using namespace rsc::logging;

namespace rsc {
namespace config {

path systemConfigDirectory() {
#ifndef WIN32
    return "/etc/";
#else
    return "c:\\";
#endif
}

path userHomeDirectory() {
#ifndef WIN32
    char* rawHome = getenv("HOME");
    if (!rawHome) {
        throw runtime_error("Home directory not defined in HOME variable.");
    }
    string home = string(rawHome) + string("/");
#else
    char* rawHomeDrive = getenv("HOMEDRIVE");
    if (!rawHomeDrive) {
        throw runtime_error("HOMEDRIVE variable not set.");
    }
    char* rawHomePath = getenv("HOMEPATH");
    if (!rawHomePath) {
        throw runtime_error("HOMEPATH variable not set.");
    }
    string home = string(rawHomeDrive) + string(rawHomePath) + string("\\");
#endif
    return home;
}

path userConfigDirectory() {
    return userHomeDirectory() / ".config";
}

string transformName(const string& name, const string& prefix,
        const bool& stripPrefix) {
    if (starts_with(name, prefix)) {
        string result;
        string::const_iterator start = name.begin();
        if (stripPrefix) {
            start + prefix.size();
        }
        transform(start, name.end(), back_inserter(result), &::tolower);
        return result;
    } else {
        return "";
    }
}

EnvironmentVariableSource::EnvironmentVariableSource(const string& prefix,
        const bool& stripPrefix) :
        logger(Logger::getLogger("rsc.config.EnvironmentVariableSource")), prefix(
                prefix), stripPrefix(stripPrefix) {
}

void EnvironmentVariableSource::provideOptions(OptionHandler& handler) {
    for (environment_iterator it = environment_iterator(environ); it
            != environment_iterator(); ++it) {
        string name = transformName(it->first, this->prefix, this->stripPrefix);
        if (name.empty()) {
            continue;
        }

        vector<string> key;
        split(key, name, is_any_of("_"));
        string value = it->second;

        RSCTRACE(logger, "Option " << key << " -> " << value);

        handler.handleOption(key, value);
    }
}

}
}
