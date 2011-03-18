/* ============================================================
 *
 * This file is a part of RSC project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

#include <gtest/gtest.h>

#include "rsc/threading/SynchronizedQueue.h"

using namespace std;
using namespace rsc;
using namespace rsc::threading;

void waitOnQueueElement(SynchronizedQueue<int> *queue, int *resultVar,
        bool *interrupted) {
    try {
        *resultVar = queue->pop();
    } catch (InterruptedException e) {
        *interrupted = true;
    }
}

TEST(SynchronizedQueueTest, testBasicPushPopSingleThreaded)
{

    SynchronizedQueue<int> queue;

    const int first = 12;
    const int second = 24;
    const int third = 36;

    queue.push(first);
    queue.push(second);
    queue.push(third);

    EXPECT_EQ(first, queue.pop());
    EXPECT_EQ(second, queue.pop());
    EXPECT_EQ(third, queue.pop());

}

TEST(SynchronizedQueueTest, testTryPop)
{

    SynchronizedQueue<int> queue;

    EXPECT_THROW(queue.tryPop(), QueueEmptyException);

    const int i = 3;
    queue.push(i);
    EXPECT_EQ(i, queue.tryPop());

    EXPECT_THROW(queue.tryPop(), QueueEmptyException);

}

TEST(SynchronizedQueueTest, testBasicPushPopMultiThreaded)
{

    SynchronizedQueue<int> queue;

    int waiter1Result;
    bool waiter1Interrupted = false;
    boost::function<void()> waiter1 = boost::bind(waitOnQueueElement, &queue,
            &waiter1Result, &waiter1Interrupted);
    int waiter2Result;
    bool waiter2Interrupted = false;
    boost::function<void()> waiter2 = boost::bind(waitOnQueueElement, &queue,
            &waiter2Result, &waiter2Interrupted);

    boost::thread w1(waiter1);
    boost::thread w2(waiter2);

    int first = 12;
    int second = 24;

    queue.push(first);
    queue.push(second);

    w1.join();
    w2.join();

    EXPECT_TRUE((waiter1Result == first && waiter2Result == second) ||
            (waiter1Result == second && waiter2Result == first));
    EXPECT_FALSE(waiter1Interrupted);
    EXPECT_FALSE(waiter2Interrupted);

}

TEST(SynchronizedQueueTest, testInterruption)
{

    SynchronizedQueue<int> queue;

    int waiter1Result;
    bool waiter1Interrupted = false;
    boost::function<void()> waiter1 = boost::bind(waitOnQueueElement, &queue,
            &waiter1Result, &waiter1Interrupted);
    int waiter2Result;
    bool waiter2Interrupted = false;
    boost::function<void()> waiter2 = boost::bind(waitOnQueueElement, &queue,
            &waiter2Result, &waiter2Interrupted);

    boost::thread w1(waiter1);
    boost::thread w2(waiter2);

    queue.interrupt();

    w1.join();
    w2.join();

    EXPECT_TRUE(waiter1Interrupted);
    EXPECT_TRUE(waiter2Interrupted);

    EXPECT_THROW(queue.pop(), InterruptedException) << "Even new calls to pop must throw after interruption";

}

TEST(SynchronizedQueueTest, testEmpty)
{

    SynchronizedQueue<int> queue;

    EXPECT_TRUE(queue.empty());
    queue.push(12);
    EXPECT_FALSE(queue.empty());
    queue.interrupt();
    EXPECT_FALSE(queue.empty());

}

TEST(SynchronizedQueueTest, testSizeLimit)
{

    SynchronizedQueue<int> queue(3);

    unsigned int elems = 5;
    for (unsigned int i = 0; i < elems; ++i) {
        queue.push(i);
    }

    EXPECT_EQ(2, queue.pop());
    EXPECT_EQ(3, queue.pop());
    EXPECT_EQ(4, queue.pop());
    EXPECT_TRUE(queue.empty());

}

void drop(const int &dropped, vector<int> *results) {
    results->push_back(dropped);
}

TEST(SynchronizedQueueTest, testDropHandler)
{

    vector<int> dropped;
    SynchronizedQueue<int> queue(2, boost::bind(&drop, _1, &dropped));

    unsigned int elems = 5;
    for (unsigned int i = 0; i < elems; ++i) {
        queue.push(i);
    }

    EXPECT_EQ((size_t) 3, dropped.size());
    EXPECT_EQ(0, dropped[0]);
    EXPECT_EQ(1, dropped[1]);
    EXPECT_EQ(2, dropped[2]);

}
