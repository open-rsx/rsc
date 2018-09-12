/* ============================================================
 *
 * This file is a part of RSC project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "langutils.h"

#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <iterator>

#ifndef _WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif
#include <time.h>

namespace rsc {
namespace misc {

void NullDeleter::operator()(void* /*ignored*/) const {
}

#ifdef _WIN32

boost::uint64_t windowsTime() {

    static const __int64 magic = 116444736000000000; // 1970/1/1
    SYSTEMTIME st;
    GetSystemTime(&st);
    FILETIME ft;
    SystemTimeToFileTime(&st,&ft); // in 100-nanosecs...
    __int64 t;
    memcpy(&t,&ft,sizeof t);
    return t - magic;

}

#endif

boost::uint64_t currentTimeMillis() {

#ifdef _WIN32

    return windowsTime() / 10000; // scale to millis.

#else

    timeval tv;
    gettimeofday(&tv, NULL);

    // (second-value in milliseconds) + (microsecond value in milliseconds)
    return (((boost::uint64_t) tv.tv_sec) * 1000l) + (tv.tv_usec / 1000l);

#endif

}

boost::uint64_t currentTimeMicros() {

#ifdef _WIN32

    return windowsTime() / 10;

#else

    timeval tv;
    gettimeofday(&tv, NULL);

    // (second-value in microseconds) + (microsecond value)
    return (((boost::uint64_t) tv.tv_sec) * 1000000ull) + (tv.tv_usec);

#endif

}

char randAlnumChar() {
    char c;
    do {
        c = (char) (rand() % ('z' - '0' + 1) + '0');
    } while (!std::isalnum(c));
    return c;
}

std::string randAlnumStr(const std::string::size_type& length) {
    std::string s;
    s.reserve(length);
    generate_n(std::back_inserter(s), length, randAlnumChar);
    return s;
}

}
}
