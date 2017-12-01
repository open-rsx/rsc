/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2014, 2017 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "SignalWaiter.h"

#include <errno.h>
#include <string.h>

#include <signal.h>
#include <semaphore.h>

#include <boost/format.hpp>

namespace rsc {
namespace misc {

int requestedSignals = 0;
sem_t semaphore;
volatile sig_atomic_t receivedSignal = 0;

void handler(int signal) {
    // Store the number of the received signal and post a token to the
    // semaphore to allow the blocking sem_wait() call in
    // waitForSignal() to complete.
    receivedSignal = signal;
    sem_post(&semaphore);
}

void throwRuntimeError(int errorNumber, const std::string& description) {
    char buffer[256];
    if (strerror_r(errorNumber, buffer, 256) != 0) {
        throw std::runtime_error(boost::str(boost::format("Error %1% (could not determine error message: %2%)")
                                            % errorNumber % errno));
    }
    throw std::runtime_error(boost::str(boost::format("%1%: %2%")
                                        % description % buffer));
}

void initSignalWaiter(int signals) {
    // SIGNALS should specify one or more signals.
    if (((INTERRUPT_REQUESTED | TERMINATE_REQUESTED | QUIT_REQUESTED)
         & signals) == 0) {
        throw std::logic_error("At least one signal has to be specified.");
    }
    requestedSignals = signals;

    // Initialize the semaphore to a blocked state.
    if (sem_init(&semaphore, 0, 0) != 0) {
        throwRuntimeError(errno, "Failed to initialize semaphore");
    }

    // The arrival of any of the requested signals unblocks the
    // semaphore.
    //
    // SA_RESETHAND causes the original handler to be restored after
    // completion of our handler. This reactivates the default
    // behavior of terminating the program for e.g. a second SIGINT
    // signal.
    struct sigaction action;
    action.sa_handler = &handler;
    if (sigemptyset(&action.sa_mask) != 0) {
        throwRuntimeError(errno, "Failed to adjust signal mask");
    }
    action.sa_flags = SA_RESETHAND;
    if ((INTERRUPT_REQUESTED & signals) != 0) {
        if (sigaction(SIGINT, &action, 0) != 0) {
            throwRuntimeError(errno, "Failed to install SIGINT handler");
        }
    }
    if ((TERMINATE_REQUESTED & signals) != 0) {
        if (sigaction(SIGTERM, &action, 0) != 0) {
            throwRuntimeError(errno, "Failed to install SIGTERM handler");
        }
    }
    if ((QUIT_REQUESTED & signals) != 0) {
        if (sigaction(SIGQUIT, &action, 0) != 0) {
            throwRuntimeError(errno, "Failed to install SIGQUIT handler");
        }
    }
}

Signal mappedSignal() {

    if (receivedSignal == 0) {
        return NO_SIGNAL;
    } else if (receivedSignal == SIGINT) {
        return INTERRUPT_REQUESTED;
    } else if (receivedSignal == SIGTERM) {
        return TERMINATE_REQUESTED;
    } else if (receivedSignal == SIGQUIT) {
        return QUIT_REQUESTED;
    } else {
        throw std::runtime_error(
                boost::str(
                        boost::format("unexpected signal number %1%")
                                % (int)receivedSignal));
    }

}

Signal waitForSignal() {
    if (requestedSignals == 0) {
        throw std::logic_error("initSignalWaiter has to be called before"
                               " waitForSignal.");
    }

    // sem_wait can be interrupted by any signal. Since we only want
    // to wait for the specified signals, we have to resume waiting in
    // case of random unrelated interruptions.
    int result;
    while ((-1 == (result = sem_wait(&semaphore))) && (errno == EINTR));
    if (result == -1) {
        throwRuntimeError(errno, "Failed to wait for signal");
    }

    // We do not delete this semaphore because the respective "other"
    // handlers may still run (i.e. we receive SIGINT followed by
    // SIGTERM, for example) and access the semaphore.
    // sem_destroy(&semaphore);

    assert(receivedSignal != 0);
    return mappedSignal();
}

Signal lastArrivedSignal() {
    if (requestedSignals == 0) {
        throw std::logic_error("initSignalWaiter has to be called before"
                               " hasSignalArrived.");
    }

    return mappedSignal();
}

int suggestedExitCode(Signal signal) {
    switch (signal) {
    case INTERRUPT_REQUESTED:
        return 1;
    case TERMINATE_REQUESTED:
        return 1;
    case QUIT_REQUESTED:
        return 1;
    case NO_SIGNAL:
    default:
        throw std::logic_error("Invalid signal number");
    }
}

}
}
