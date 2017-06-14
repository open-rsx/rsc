/* ============================================================
 *
 * This file is a part of the RSC project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "ConfigSource.h"

#include <stdexcept>

#include <boost/tokenizer.hpp>

namespace rsc {
namespace config {

ConfigSource::~ConfigSource() = default;

void ConfigSource::splitKeyAtDots(const std::string& input,
                                  std::vector<std::string>& output) {
    typedef boost::escaped_list_separator<char> SeparatorType;
    typedef boost::tokenizer<SeparatorType> TokenizerType;

    try {
        SeparatorType sep('\\', '.');
        TokenizerType tok(input, sep);
        copy(tok.begin(), tok.end(), back_inserter(output));
    } catch (boost::escaped_list_error &e) {
        throw invalid_argument(e.what());
    }

    for (auto component : output) {
        if (component.empty()) {
            throw invalid_argument("Empty component in key '" + input + "'");
        }
    }

    if (output.empty()) {
        throw std::invalid_argument("Option key is empty");
    }

}

}
}
