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

#include "Plugin.h"

#include <errno.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#endif

#if defined(__linux__) or defined(__APPLE__)
#include <dlfcn.h>
#endif

#include <stdexcept>

#include <boost/format.hpp>

#include "../logging/Logger.h"
#include "../logging/LoggerFactory.h"

using namespace std;

using namespace boost;

namespace rsc {
namespace plugins {

const std::string PLUGIN_INIT_SYMBOL     = "rsc_plugin_init";
const std::string PLUGIN_SHUTDOWN_SYMBOL = "rsc_plugin_shutdown";

class Impl {
public:
    Impl(const std::string& name,
         const std::string& library)
        : logger(logging::LoggerFactory::getInstance()
                 .getLogger(str((format("rsc.plugins.Plugin[%1%]")
                                 % name)))),
          name(name), library(library),
          loaded(false), handle(NULL),
          init(NULL), shutdown(NULL) {
    }

    const string& getName() const {
        return this->name;
    }

    const string& getLibrary() const {
        return this->library;
    }

    void load(bool wrapExceptions) {

        if (this->loaded) {
            throw runtime_error(
                    boost::str(
                            boost::format(
                                    "Plugin %1% is already loaded. Cannot load it again,")
                                    % this->name));
        }

        RSCINFO(this->logger, "Trying to load library `" << this->library << "'");

        // Load the library containing the plugin.
        loadLibrary();

        // Lookup init and shutdown functions in the plugin library.
        this->init
            = reinterpret_cast<InitFunction>(resolveSymbol(PLUGIN_INIT_SYMBOL));
        this->shutdown
            = reinterpret_cast<ShutdownFunction>(resolveSymbol(PLUGIN_SHUTDOWN_SYMBOL));

        // Initialize the plugin.
        RSCINFO(this->logger, "Initializing");
        if (wrapExceptions) {
            try {
                this->init();
                this->loaded = true;
            } catch (const std::exception& e) {
                throw runtime_error(str(format("Plugin `%1%' failed to initialize: %2%")
                                        % this->name
                                        % e.what()));
            } catch (...) {
                throw runtime_error(str(format("Plugin `%1%' failed to initialize due to unknown error.")
                                        % this->name));
            }
        } else {
          this->init();
          this->loaded = true;
        }
    }

    void unload(bool wrapExceptions) {

        if (!loaded) {
            throw runtime_error(
                    str(format("Plugin `%1%' failed cannot be unloaded because it has not been loaded correctly.")
                               % this->name));
        }

        // Shut the plugin down.
        RSCINFO(this->logger, "Shutting down");
        this->loaded = false;

        if (wrapExceptions) {
            try {
                this->shutdown();
            } catch (const std::exception& e) {
                throw runtime_error(str(format("Plugin `%1%' failed to shutdown: %2%")
                                        % this->name
                                        % e.what()));
            } catch (...) {
                throw runtime_error(str(format("Plugin `%1%' failed to shutdown due to unknown error.")
                                        % this->name));
            }
        } else {
            this->shutdown();
        }
    }

    bool ensureLoaded(bool wrapExceptions) {
        if (this->loaded) {
            return false;
        }
        load(wrapExceptions);
        return true;
    }
private:
    typedef void (*InitFunction)();
    typedef void (*ShutdownFunction)();

    rsc::logging::LoggerPtr logger;

    string name;
    string library;

    bool loaded;

#if defined(_WIN32)
    HMODULE          handle;
#else
    void*            handle;
#endif
    InitFunction     init;
    ShutdownFunction shutdown;

    void loadLibrary() {
#if defined(__linux__) || defined(__APPLE__)
        if (!(this->handle = dlopen(this->library.c_str(), RTLD_NOW))) {
            const char* result = dlerror();
            throw runtime_error(str(format("Failed to load plugin `%1%' from shared object `%2%': %3%.")
                                    % this->name % this->library
                                    % (result ? result : "<unknown error>")));
        }
#elif defined(_WIN32)
        if (!(this->handle= LoadLibrary(this->library.c_str()))) {
            throw runtime_error(str(format("Failed to load plugin `%1%' from shared object `%2%': %3%.")
                                    % this->name % this->library % GetLastError()));
        }
#else
        throw runtime_error("Plugins are not implemented for this platform.");
#endif
    }

    void* resolveSymbol(const string& name) {
        RSCINFO(this->logger, "Resolving symbol `"
                << name << "' in library `" << this->library << "'");

        assert(this->handle);

        void *address;
#if defined(__linux__) || defined(__APPLE__)
        if (!(address = dlsym(this->handle, name.c_str()))) {
            const char* result = dlerror();
            throw runtime_error(str(format("Plugin `%1%' failed to define function `%2%': %3%")
                                    % this->name % name
                                    % (result ? result : "<unknown error>")));
        }
#elif defined(_WIN32)
        if (!(address = reinterpret_cast<void*>(GetProcAddress(this->handle, name.c_str())))) {
            throw runtime_error(str(format("Plugin `%1%' failed to define function `%2%': %3%")
                                    % this->name % name % GetLastError()));
        }
#else
        throw runtime_error("Plugins are not implemented for this platform.");
#endif
        return address;
    }
};

Plugin::Plugin(Impl* impl)
    : impl(impl) {
}

Plugin::~Plugin() {
}

const string& Plugin::getName() const {
    return this->impl->getName();
}

void Plugin::load(bool wrapExceptions) {
    this->impl->load(wrapExceptions);
}

void Plugin::unload(bool wrapExceptions) {
    this->impl->unload(wrapExceptions);
}

bool Plugin::ensureLoaded(bool wrapExceptions) {
    return this->impl->ensureLoaded(wrapExceptions);
}

string Plugin::getLibrary() const {
    return this->impl->getLibrary();
}

PluginPtr Plugin::create(const std::string& name, const std::string& library) {
    return PluginPtr(new Plugin(new Impl(name, library)));
}

}
}
