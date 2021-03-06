/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2012, 2013, 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <string>

#include <boost/filesystem.hpp>

#include "OptionHandler.h"

#include <rsc/rscexports.h>

namespace rsc {
namespace config {

RSC_EXPORT extern const std::string DEFAULT_DEBUG_VARIABLE_NAME;

RSC_EXPORT extern const std::string DEFAULT_FILE_VARIABLE_NAME;

RSC_EXPORT extern const std::string CONFIG_FILE_KEY_PREFIX;
RSC_EXPORT extern const std::string CONFIG_FILE_KEY_USER;
RSC_EXPORT extern const std::string CONFIG_FILE_KEY_PWD;

std::vector<std::string> RSC_EXPORT
defaultConfigurationFiles(const std::string& fileVariableName
                          = DEFAULT_FILE_VARIABLE_NAME);


/**
 * Return a pair of configuration file path and description derived
 * from @a spec.
 *
 * If @a spec is one of @ref CONFIG_FILE_KEY_PREFIX, @ref
 * CONFIG_FILE_KEY_USER or @ref CONFIG_FILE_KEY_PWD, @a configFileName
 * and potentially @a prefix are used to derive a prefix-wide,
 * user-specific or pwd-relative configuration file path.
 *
 * Otherwise @a spec is treated as a filename and returned.
 */
std::pair<boost::filesystem::path, std::string> RSC_EXPORT
resolveConfigurationFile(const std::string&             spec,
                         const boost::filesystem::path& prefix,
                         const std::string&             configFileName);

/**
 * Pass configuration options in from configuration files derived from
 * @a configFileName and environment variables with prefix @a
 * environmentVariablePrefix to @a handler.
 *
 * The following configuration configuration sources are considered
 * based on @a configFileName:
 *
 * -# Prepending a prefix-wide configuration directory, computed by
 *    applying @ref prefixConfigDirectory to the value of the @a
 *    prefix parameter, (e.g. @c /usr/local/etc/) to @a configFileName
 * -# Prepending a user-specific configuration directory (e.g. @c
 *    $HOME/.config) to @a configFileName
 * -# Prepending the current working directory (@c $(pwd)) to @a
 *    configFileName
 * -# Environment Variables
 *
 * See #ConfigFileSource for the configuration file format.
 *
 * Setting the environment variable @c __CONFIG_DEBUG to an arbitrary
 * value enables configuration debugging. All debug output is written
 * to @c stderr.
 *
 * @param handler Receiver of the configuration options.
 * @param configFileName A filename (without directory) from which
 *                       three configuration file names are derived.
 * @param environmentVariablePrefix A prefix string with which all
 *                                  processed environment variables
 *                                  have to start.
 * @param argc number of arguments passed to the main program. If 0, no command
 *             line argument parsing will be performed. If something else is
 *             specified, ensure that @param argv matches the argc. Defaults to
 *             0, so no argument parsing.
 * @param argv argument vector for command line parsing. Must match the length
 *             given in argc. Might be an arbitrary pointer of argc i 0.
 * @param stripEnvironmentVariablePrefix if true, the prefix for environment
 *                                       variable will be stripped before
 *                                       passing options to the handler. If e.g.
 *                                       RSC_ is set as prefix and this is true,
 *                                       RSC_TEST will be passed to the handlers
 *                                       as just TEST. Default is true.
 * @param prefix the (installation) prefix for which the prefix
 *               configuration directory should be computed (by
 *               calling @ref prefixConfigDirectory) when searching
 *               for a prefix-wide configuration file.
 * @param directories
 *
 * @author jmoringe
 */
void RSC_EXPORT configure(OptionHandler&                  handler,
                          const std::string&              configFileName,
                          const std::string&              environmentVariablePrefix,
                          int                             argc                           = 0,
                          const char**                    argv                           = 0,
                          bool                            stripEnvironmentVariablePrefix = true,
                          const boost::filesystem::path&  prefix                         = "/",
                          const std::string&              debugVariableName              = DEFAULT_DEBUG_VARIABLE_NAME,
                          const std::vector<std::string>& configurationFiles             = defaultConfigurationFiles());

}
}
