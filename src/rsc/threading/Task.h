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

#ifndef TASK_H_
#define TASK_H_

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/timer.hpp>

#include "../logging/Logger.h"

namespace rsc {
namespace threading {

template<class R>
class Task;
typedef boost::shared_ptr<Task<void> > TaskPtr;

//template < class R >
//typedef boost::shared_ptr<Task<R> > TaskPtr;
// TODO think about if its possible to pass a smart pointer to this object
//      to the callbacks
typedef boost::function<void()> Callback;

template<class R>
class Task {
public:
    Task(boost::function<R(Task<R>*)> delegate) :
        d(delegate), cancelRequest(false), cancelled(false), logger(
                rsc::logging::Logger::getLogger("rsc.threading.Task")) {
        //    pre = boost::bind(&TimedObject::beforeCycle,this);
        //    post = boost::bind(&TimedObject::afterCycle,this);
    }

    virtual ~Task() {
    }

    void run() {
        RSCTRACE(logger, "run() entered"); // << *id);
        // TODO Think about returning an iterator to the results of execute here!
        do {
            // TODO add exception handling
            // call pre hook
            if (pre)
                pre();
            // call template method
            d(this);
            // call post hook
            if (post)
                post();
            RSCTRACE(logger, "run cycle done");
        } while (continueExec());
        boost::recursive_mutex::scoped_lock lock(m);
        RSCINFO(logger, "run() finished");
        cancelled = true;
        this->c.notify_all();
    }

    boost::shared_ptr<boost::thread> start() {
        boost::shared_ptr<boost::thread> thread =
                boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(
                        &Task::run, this)));
        return thread;
    }

    // beautify this
    // provide time mesaurement as default implementation
    void setPreCallback(Callback f) {
        pre = f;
    }

    void setPostCallback(Callback f) {
        pre = f;
    }

    virtual void cancel() {
        RSCTRACE(logger, "Task::cancel() entered");
        //		// protect setting and comparison of cancel boolean, see execute()
        //		boost::recursive_mutex::scoped_lock lock(m);
        cancelRequest = true;
    }

    virtual bool isCancelRequested() {
        return cancelRequest;
    }
    virtual void restart() {
        cancelRequest = false;
    }

    virtual void waitDone() {
        RSCDEBUG(logger, "waitDone() entered");
        boost::recursive_mutex::scoped_lock lock(m);
        RSCDEBUG(logger, "waitDone() after lock, before wait");
        while (!cancelled) {
            this->c.wait(lock);
        }
        RSCDEBUG(logger, "waitDone() finished");
    }

    friend std::ostream& operator<<(std::ostream& out, const Task<R>& t) {
        out << "Task[cancelRequest=" << t.cancelRequest << "]";
        return out;
    }

protected:

    boost::function<R(Task<R>*)> d;
    //boost::shared_ptr<boost::thread> thread;

    mutable boost::recursive_mutex m;
    boost::condition c;

    void beforeCycle() {
        timer.restart();
    }

    void afterCycle() {
        // calculate processing time for last cycle, last n cycle, variance...
        // TODO change to logging
        RSCINFO(logger, "Times (last cycle = " << timer.elapsed() << "s)");
    }

    virtual bool continueExec() {
        return false;
    }

    volatile bool cancelRequest;
    volatile bool cancelled;

private:
    rsc::logging::LoggerPtr logger;
    boost::timer timer;
    Callback pre;
    Callback post;

};

}
}

#endif /* TASK_H_ */