/* ============================================================
 *
 * This file is a part of the the RSC project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#pragma once

#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "rsc/rscexports.h"

namespace rsc {
namespace threading {

/**
 * Thrown when the result of the computation represented by a future
 * is not available for some reason.
 *
 * Derived classes represent different reasons more precisely.
 *
 * @author jmoringe
 */
class RSC_EXPORT FutureException: public std::runtime_error {
public:
    explicit FutureException(const std::string &message);
};

/**
 * Exception that is thrown if the result of a Future is not available because
 * the underlying process generated an error.
 *
 * @author jwienke
 */
class RSC_EXPORT FutureTaskExecutionException: public FutureException {
public:

    /**
     * Constructor.
     *
     * @param msg error message
     */
    explicit FutureTaskExecutionException(const std::string &msg);

};

/**
 * Thrown when the execution of a @ref Future's task does not complete
 * within the specified amount of time.
 *
 * @author jwienke
 * @author jmoringe
 */
class RSC_EXPORT FutureTimeoutException: public FutureException {
public:
    /**
     * Construct a new FutureTimeoutException instance.
     *
     * @param message A string describing the circumstances of the
     * timeout.
     */
    explicit FutureTimeoutException(const std::string &message);
};

/**
 * Class providing access to the result of a process that is asynchronously
 * running. If the result is requested before the task finished, the ::get
 * operation will block until a results or error is available.
 *
 * @author jwienke
 * @tparam R result type, should be copyable but this is not a hard requirement
 */
template<class R>
class Future {
protected:
    typedef boost::mutex MutexType;
    typedef boost::condition ConditionType;
private:
    R result;
    bool taskFinished;
    bool taskError;
    std::string errorMsg;
    ConditionType condition;
    MutexType mutex;
public:

    /**
     * Create a new Future object that does not have a result and is
     * thus suitable for representing an in-progress computation.
     */
    Future() :
        taskFinished(false), taskError(false) {
    }

    /**
     * Try to obtain and then return the result of the operation
     * represented by the Future object.
     *
     * If necessary, this method waits for the operation to complete,
     * and then retrieves its result.
     *
     * @return The result of the operation if it did complete
     * successfully.
     * @throw FutureTaskExecutionException If the operation
     * represented by the Future object failed.
     */
    R get() {
        return get(0);
    }

    /**
     * Try to obtain and then return the result of the operation represented by the
     * Future object.
     *
     * If necessary, this method waits up to @a timeout seconds for
     * the operation to complete. If the operation does not complete
     * within this time a @ref FutureTimeoutException is thrown.
     *
     * @param timeout The amount of time in seconds in which the
     * operation has to complete.
     * @return The result of the operation if it did complete
     * successfully within the given amount of time.
     * @throw FutureTaskExecutionException If the operation
     * represented by the Future object failed.
     * @throw FutureTimeoutException If the result does not become
     * available within the amount of time specified via @a timeout.
     */
    R get(double timeout) {
        MutexType::scoped_lock lock(mutex);
        while (!taskFinished) {
            if (timeout <= 0) {
                condition.wait(lock);
            } else {
                boost::xtime xt;
                xtime_get(&xt, boost::TIME_UTC);
                xt.sec += timeout;
                if (!condition.timed_wait(lock, xt)) {
                    throw FutureTimeoutException(
                        boost::str(boost::format("Timeout while waiting for result. Waited %1% seconds.")
                                   % timeout));
                }
            }
        }

        if (taskError) {
            throw FutureTaskExecutionException(errorMsg);
        }

        return result;
    }

    /**
     * Tells whether the computation of the underlying process
     * finished and provided a result or generated an error.
     *
     * @return @c true if a result or error is available, else @c
     * false
     */
    bool isDone() {
        MutexType::scoped_lock lock(mutex);
        return taskFinished;
    }

    /**
     * Provide the result for this future.
     *
     * @param data result data
     */
    void set(R data) {
        result = data;
        {
            MutexType::scoped_lock lock(mutex);
            taskFinished = true;
            condition.notify_all();
        }
    }

    /**
     * Indicate an error while processing.
     *
     * @param message error description
     */
    void setError(const std::string &message) {
        errorMsg = message;
        {
            MutexType::scoped_lock lock(mutex);
            taskFinished = true;
            taskError = true;
        }
        condition.notify_all();
    }
protected:
    MutexType &getMutex() {
        return this->mutex;
    }

    ConditionType &getCondition() {
        return this->condition;
    }
};

}
}
