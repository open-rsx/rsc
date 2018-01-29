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

#pragma once

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "rsc/rscexports.h"

namespace rsc {
namespace plugins {

extern const std::string PLUGIN_INIT_SYMBOL;
extern const std::string PLUGIN_SHUTDOWN_SYMBOL;

class Impl;

/**
 * Instances of this class represent pieces of functionality which can
 * be loaded into a program at runtime.
 *
 * Plugin instances are not thread-safe and access needs to be synchronized.
 *
 * @author jmoringe
 */
class RSC_EXPORT Plugin : boost::noncopyable {
public:
    virtual ~Plugin();

    /**
     * Returns the name of the plugin.
     *
     * @return The name of the plugin.
     */
    const std::string& getName() const;

    /**
     * Tries to load the functionality of the plugin into the current
     * process.
     *
     * @param wrapExceptions if @c true, exceptions generated inside the plugin
     *                       init method are wrapped in a runtime_error.
     *                       Otherwise they are passed through.
     * @throw runtime_error If the plugin cannot be loaded for some
     *                      reason.
     */
    void load(bool wrapExceptions = true);

    /**
     * Tries to unload the functionality of the plugin.
     *
     * @param wrapExceptions if @c true, exceptions generated inside the plugin
     *                       init method are wrapped in a runtime_error.
     *                       Otherwise they are passed through.
     * @throw runtime_error If the plugin cannot be unloaded for some
     *                      reason.
     */
    void unload(bool wrapExceptions = true);

    /**
     * Tries to load the plugin unless it is already loaded.
     *
     * @param wrapExceptions If @c true, exceptions thrown in the init
     *                       method of the plugin are wrapped in a
     *                       runtime_error exception.
     * @return @c false if the plugin was already loaded. @c true
     *         otherwise.
     * @throw runtime_error If the plugin cannot be loaded for some
     *                      reason.
     */
    bool ensureLoaded(bool wrapExceptions = true);

    /**
     * Returns the path to the library implementing this plugin.
     *
     * @return string representing the filesystem path to the library
     *         implementing this plugin
     */
    std::string getLibrary() const;

private:
    friend class Manager;

    boost::scoped_ptr<Impl> impl;

    Plugin(Impl* impl);

    static boost::shared_ptr<Plugin> create(const std::string& name,
                                            const std::string& library);
};

typedef boost::shared_ptr<Plugin> PluginPtr;

}
}
