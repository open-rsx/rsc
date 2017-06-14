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

#include "DebugTools.h"

#if defined(DEBUGTOOLS_LINUX)
#include "LinuxDebugTools.h"
#elif defined(SUBPROCESS_WINDOWS)
#include "WindowsDebugTools.h"
#endif

using namespace std;

namespace rsc {
namespace debug {

DebugTools::DebugTools() {
}

DebugTools::~DebugTools() = default;

DebugToolsPtr DebugTools::newInstance() {
#if defined(DEBUGTOOLS_LINUX)
    return DebugToolsPtr(new LinuxDebugTools);
#elif defined(SUBPROCESS_WINDOWS)
    return DebugToolsPtr(new WindowsDebugTools);
#endif
}

string DebugTools::formatBacktrace(const vector<string>& trace) {
    stringstream s;
    for (auto frame : trace) {
        s << "\t" << frame << std::endl;
    }
    return s.str();
}

}
}
