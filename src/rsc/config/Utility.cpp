/* ============================================================
 *
 * This file is part of the RSC project.
 *
 * Copyright (C) 2016, 2017 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Utility.h"

#include <boost/format.hpp>
#include <boost/tokenizer.hpp>

#include "../runtime/ContainerIO.h"

using namespace std;

namespace rsc {
namespace config {

vector<string> splitSequenceValue(const string& value) {
    vector<string> result;
    boost::escaped_list_separator<char> sep('\\', ':');
    boost::tokenizer< boost::escaped_list_separator<char> > tok(value, sep);
    copy(tok.begin(), tok.end(), back_inserter(result));
    return result;
}

vector<string> mergeSequenceValue(const string&         description,
                                  const vector<string>& key,
                                  const string&         raw,
                                  const vector<string>& previous) {
    // Split value at ":" tokens. Each substring is an option value.
    vector<string> values;
    try {
        values = splitSequenceValue(raw);
    } catch (const std::exception& e) {
        throw invalid_argument(
                boost::str(boost::format("Invalid %1% (option %2%) value `%3%': %4%")
                           % description
                           % boost::io::group(std::container_none,
                                              std::element_sequence(".", ""),
                                              key)
                           % raw
                           % e.what()));
    }

    // Add all specified values. The empty string, produced by "::"
    // means that the previous value should be spliced in.
    vector<string> result;
    for (auto value : values) {
        if (value.empty()) {
            copy(previous.begin(), previous.end(),
                 inserter(result, result.end()));
        } else {
            result.push_back(value);
        }
    }

    return result;
}

}
}
