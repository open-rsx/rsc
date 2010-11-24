/* ============================================================
 *
 * This file is a part of the RSC project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PERIODICTASK_H_
#define PERIODICTASK_H_

#include "Task.h"

#include <iostream>

namespace rsc {
namespace threading {

template<class R>
class PeriodicTask: public Task<R> {
public:

    PeriodicTask(boost::function<R(Task<R>*)> delegate, int ms) :
        Task<R> (delegate), cycleTime(ms), logger(
                rsc::logging::Logger::getLogger("rsc.threading.task")) {
    }

    virtual ~PeriodicTask() {
        RSCTRACE(logger, "~PeriodicTask() entered");
        //	if (!cancelRequest) cancel();
    }

    virtual void cancel() {
        RSCTRACE(logger, "PeriodicTask::cancel() entered");
        Task<R>::cancel();
    }

protected:

    virtual bool continueExec() {
        RSCTRACE(logger, "~PeriodicTask()::continueExec() entered");
        // wait, give others a chance
        bool cont = false;
        if (cycleTime != 0) {
            boost::xtime time;
            xtime_get(&time, boost::TIME_UTC);
            time.nsec += cycleTime * 1000000;
            // TODO provide option to interrupt in cancel using boost::this_thread
            try {
                RSCTRACE(logger, "PeriodicTask()::continueExec() before thread sleep, sleeping " << cycleTime << " ms");
                boost::thread::sleep(time);
                //	thread->sleep(time);
            } catch (const boost::thread_interrupted &e) {
                RSCWARN(logger, "PeriodicTask()::continueExec() catched boost::thread_interrupted exception");
            }
            RSCTRACE(logger, "PeriodicTask()::continueExec() thread woke up");
        }
        RSCTRACE(logger, "PeriodicTask()::continueExec() before lock");
        //boost::recursive_mutex::scoped_lock lock(Task<R>::m);
        if (!this->cancelRequest) {
            cont = true;
        } else {
            RSCTRACE(logger, "PeriodicTask()::continueExec() cancel requested");
        }
        RSCTRACE(logger, "PeriodicTask()::continueExec() finished");
        return cont;
    }

private:
    int cycleTime;
    rsc::logging::LoggerPtr logger;

};

}
}

#endif /* PERIODICTASK_H_ */
