/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_SCHEDULED_QUEUE_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_SCHEDULED_QUEUE_H

#include <SPL/Runtime/Operator/OperatorMetrics.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/ProcessingElement/ElasticPortAdapter.h>
#include <SPL/Runtime/ProcessingElement/ElasticThreadAdapter.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/ProcessingElement/ScheduledData.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Toolkit/CircularQueue.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>

#include <boost/atomic/atomic.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/thread/tss.hpp>

#include <list>
#include <map>
#include <set>
#include <vector>

namespace SPL {
class ElasticPortAdapter;
class ScheduledQueue
{
  private:
    class ScheduleThread;

  public:
    ScheduledQueue(uint32_t minThreads,
                   uint32_t maxThreads,
                   uint32_t activeThreads,
                   bool adaptOn,
                   uint32_t allInputPorts);
    ~ScheduledQueue();
    void start();
    void registerOperatorPort(ProcessSignal* sig, OperatorMetricsImpl& opm, uint32_t port);
    void registerThread(ScheduleThread* thread);
    void unregisterThread();
    void shutdown();
    void portClosed();
    void joinThreads();
    void schedule(const boost::atomic<bool>& suspended,
                  const boost::atomic<bool>& shutdown,
                  const boost::atomic<bool>& allPortsClosed,
                  boost::atomic<bool>& active);
    void adapt();

    void push(ScheduledData& data)
    {
        SPSCEnforcer* mq = _queues[data.port()->getQueueIndex()];

        if (!mq->push(data)) {
            reSchedule(mq, data);
        }
    }

    void turnOnScheduledPort(std::vector<uint32_t>& operators, std::pair<int, int>& p);
    void turnOffScheduledPort(std::vector<uint32_t>& operators, std::pair<int, int>& p);
    void turnOffScheduledPort(std::set<uint32_t>& operators);
    bool hasScheduledPort(uint32_t op);
    uint32_t getScheduledPortCount();
    void adjustingDone(bool recordHist,
                       AdjustDirection dir,
                       double maxThroughput,
                       std::vector<uint32_t>& affectedPorts);
    void adjustingDone(double maxThroughput);

  private:
    void createNewThread();
    void createNewThreads(uint32_t toCreate);
    void suspendThreads(uint32_t toSuspend);
    void wakeupThreads(uint32_t toWakeup);
    void increaseMinThreads(uint32_t newMin);
    uint32_t findUnSuspendableThreads();
    bool someThreadsRunnable();

    bool isFinished()
    {
        // We try to avoid hitting global values - if we're a thread started internally here, we
        // can use thread locals. Otherwise, we're a source or PE input port thread, and we have
        // to check the global values.
        if (_thisThread.get() != NULL) {
            return _thisThread->isFinished();
        }
        return _globalIsShutdown.load(boost::memory_order_acquire) ||
               _globalAllPortsClosed.load(boost::memory_order_acquire);
    }

    bool isSuspended()
    {
        if (_thisThread.get() != NULL) {
            return _thisThread->isSuspended();
        }
        return false;
    }

    class SPSCEnforcer;

    void reSchedule(SPSCEnforcer* mq, ScheduledData& data)
    {
        // We're blocked from proceeding because mq->queue is full. So we will loop on trying to
        // push into it, and in the body, we will try to get exclusive consume access to that very
        // queue. If we do, we partially drain it. Notice that we are trying to drain the very queue
        // we're blocked on. So we will proceed when either: someone else frees up space for us, or
        // we free up space for ourselves.
        //
        // Also note that even after we drain mq->queue, we have to check the result of pushing into
        // it. This is a multi-producer queue, so it's possible that the queue re-fills back up
        // after we partially drain it.
        while (!mq->push(data) && !isFinished()) {
            if (mq->tryLockConsumer()) {
                ProcessSignal* port = data.port();
                if (port->shouldSkipScheduledPort() && port->tryDrain()) {
                    ScheduledData reData;
                    uint32_t localProcessed = 0;
                    while (mq->front(reData)) {
                        execute(reData);
                        ++localProcessed;
                        if (localProcessed > queueSize || isFinished() || isSuspended()) {
                            break;
                        }
                    }
                    queueClosureProcessed++;
                    port->completeDrain();
                } else {
                    ScheduledData reData;
                    uint32_t localProcessed = 0;
                    while (mq->front(reData)) {
                        execute(reData);
                        ++localProcessed;
                        if (localProcessed > queueSize / 4 || isFinished() || isSuspended()) {
                            break;
                        }
                    }
                }
                mq->unLockConsumer();
            }
        }
    }

    void execute(ScheduledData& schedData)
    {
        ProcessSignal* port = schedData.port();
        Data& data = schedData.data();

        if (data.isTuple()) {
            port->submit(data.getTuple());
            data.getTuple().clear();
        } else {
            port->submit(data.getPunctuation());
        }

        schedData.decrementQueueCounter();
        schedData.pop();
    }

    inline void freePort(ProcessSignal* port) ALWAYS_INLINE
    {
        SPSCEnforcer* mq = _queues[port->getQueueIndex()];
        mq->unLockConsumer();
        while (!_freePorts->bounded_push(port)) {
        }
    }

    bool findWorkNonBlocking(ScheduledData& data)
    {
        ProcessSignal* first = NULL;
        if (_freePorts->pop(first)) {
            // we do a priming read because we need to remember the first port
            // so that we can detect when we've wrapped around the queue
            SPSCEnforcer* mq = _queues[first->getQueueIndex()];
            if (mq->tryLockConsumer()) {
                if (mq->front(data)) {
                    return true;
                } else {
                    // no work item in the queue
                    if (first->shouldSkipScheduledPort() && first->tryDrain()) {
                        first->completeDrain();
                        queueClosureProcessed++;
                    }
                }
                mq->unLockConsumer();
            }
            // no work to be found, push the first back to the end of the queue
            while (!_freePorts->bounded_push(first)) {
            }

            ProcessSignal* port = first;
            while (_freePorts->pop(port)) {
                // see if we can grab work
                SPSCEnforcer* scq = _queues[port->getQueueIndex()];
                if (scq->tryLockConsumer()) {
                    if (scq->front(data)) {
                        return true;
                    } else {
                        // no work item in the queue
                        if (port->shouldSkipScheduledPort() && port->tryDrain()) {
                            port->completeDrain();
                            queueClosureProcessed++;
                        }
                    }
                    scq->unLockConsumer();
                }
                while (!_freePorts->bounded_push(port)) {
                }

                // have we looped around the entire queue?
                if (port == first) {
                    // yes, leave the loop
                    break;
                }
            }
        }
        return false;
    }

    bool findWorkBlocking(ScheduledData& data,
                          const boost::atomic<bool>& suspended,
                          const boost::atomic<bool>& localShutdown,
                          const boost::atomic<bool>& localAllPortsClosed,
                          boost::atomic<bool>& active)
    {
        long nsDelay = 1;
        while (!localShutdown.load(boost::memory_order_acquire) &&
               !localAllPortsClosed.load(boost::memory_order_acquire)) {
            if (suspended.load(boost::memory_order_acquire)) {
                _thisThread->suspend();
            }
            active.store(true, boost::memory_order_release);

            if (findWorkNonBlocking(data)) {
                return true;
            }

            _thisThread->block(nsDelay);
            if (nsDelay < 100 * ONE_MILLISECOND) {
                nsDelay *= 10;
            }
        }

        // if we reach here, we're done and shutting down
        return false;
    }

    void queueElasticity(double& throughput);
    void threadElasticity(double& throughput);
    void validatePerformance(double& throughput);
    void shouldTriggerQueueElasticity(uint32_t& lastAdjThreadNum, uint32_t& _activeThread);

    class AdaptThread;

    // record for one queue adjustment
    class AdjustRec
    {
      public:
        AdjustRec(uint32_t t, double throughput)
          : threadCount_(t)
          , throughput_(throughput)
          , hasAdjustment_(true)
        {}

        AdjustRec(uint32_t t, double throughput, bool hasAdjustment)
          : threadCount_(t)
          , throughput_(throughput)
          , hasAdjustment_(hasAdjustment)
        {}

        uint32_t getThreadCount() { return threadCount_; }

        double getThroughput() { return throughput_; }

        void addThread(uint32_t t) { threadsExplored_.insert(t); }

        void updateThroughput(double newThroughput) { throughput_ = newThroughput; }

        bool toContinue(uint32_t newThreadCount, bool increasing)
        {
            if (hasAdjustment_) {
                if (increasing) {
                    for (std::set<uint32_t>::iterator it = threadsExplored_.begin();
                         it != threadsExplored_.end(); it++) {
                        uint32_t t = *it;
                        if (t >= newThreadCount) {
                            return false;
                        }
                    }
                } else {
                    for (std::set<uint32_t>::iterator it = threadsExplored_.begin();
                         it != threadsExplored_.end(); it++) {
                        uint32_t t = *it;
                        if (t <= newThreadCount) {
                            return false;
                        }
                    }
                }
                return true;
            } else {
                for (std::set<uint32_t>::iterator it = threadsExplored_.begin();
                     it != threadsExplored_.end(); it++) {
                    uint32_t t = *it;
                    if (t == newThreadCount) {
                        return false;
                    }
                }
                return true;
            }
        }

      private:
        uint32_t threadCount_;
        double throughput_;
        bool hasAdjustment_;
        std::set<uint32_t> threadsExplored_;
    };

    class ScheduleThread : public Thread
    {
      public:
        ScheduleThread();
        ~ScheduleThread();
        void* run(void* threadArgs);
        void suspend();
        void block(long nsDelay);
        void notifySuspended();
        void notifyWakeup();
        void notifyShutdown();
        void notifyAllPortsClosed();

        bool isFinished() const { return _shutdown || _allPortsClosed; }

        bool isSuspended() const { return _suspended; }

        bool isSuspendable() const { return _suspendable; }

        void notSuspendable() { _suspendable = false; }

        bool isInSuspension() const { return _inSuspension; }

        bool isActive() const { return _active.load(boost::memory_order_acquire); }

        void resetActive() { _active.store(false, boost::memory_order_release); }

      private:
        // for graceful thread suspension
        Distillery::Mutex _mutex;
        Distillery::CV _cv;

        // for lock-free notifications
        boost::atomic<bool> _suspended;
        boost::atomic<bool> _suspendable;
        boost::atomic<bool> _inSuspension;
        boost::atomic<bool> _shutdown;
        boost::atomic<bool> _allPortsClosed;
        boost::atomic<bool> _active;
    };

    class SPSCEnforcer
    {
      public:
        SPSCEnforcer()
          : _queue(NULL)
          , _producerLocked(false)
          , _consumerLocked(false)
        {}
        SPSCEnforcer(CircularQueue<ScheduledData>* q)
          : _queue(q)
          , _producerLocked(false)
          , _consumerLocked(false)
        {}
        SPSCEnforcer(const SPSCEnforcer& other)
          : _queue(other._queue)
          , _producerLocked(other._producerLocked.load())
          , _consumerLocked(other._consumerLocked.load())
        {}

        bool push(ScheduledData& data)
        {
            if (tryLockProducer()) {
                ProcessSignal* port = data.port();
                if (port->shouldSkipScheduledPort()) {
                    if (port->hasDrained()) {
                        Data& actualData = data.data();
                        if (actualData.isTuple()) {
                            port->submit(actualData.getTuple());
                        } else {
                            port->submit(actualData.getPunctuation());
                        }
                        unLockProducer();
                        return true;
                    } else {
                        unLockProducer();
                        return false;
                    }
                } else {
                    size_t queueLength;
                    if (_queue->full(&queueLength)) {
                        data.incrementWaitCounter();
                        unLockProducer();
                        return false;
                    }
                    _queue->rear().copy(data);
                    _queue->push_back();
                    data.incrementQueueCounter((int64_t)queueLength + 1);
                    unLockProducer();
                    return true;
                }
            }
            return false;
        }

        bool front(ScheduledData& data)
        {
            assert(_consumerLocked == true);
            if (_queue->empty()) {
                return false;
            }
            data = _queue->front();
            data.setQueue(_queue);
            return true;
        }

        bool tryLockConsumer()
        {
            bool expected = false;
            return _consumerLocked.compare_exchange_strong(expected, true,
                                                           boost::memory_order_acquire);
        }

        bool tryLockProducer()
        {
            bool expected = false;
            return _producerLocked.compare_exchange_strong(expected, true,
                                                           boost::memory_order_acquire);
        }

        void unLockConsumer() { _consumerLocked.store(false, boost::memory_order_release); }

        void unLockProducer() { _producerLocked.store(false, boost::memory_order_release); }

        // We can't make this the destructor because we copy these objects into vectors
        void freeQueue() { delete _queue; }

      private:
        CircularQueue<ScheduledData>* _queue;
        boost::atomic<bool> _producerLocked;
        boost::atomic<bool> _consumerLocked;
    };

    std::vector<SPSCEnforcer*> _queues;
    boost::lockfree::queue<ProcessSignal*, boost::lockfree::fixed_sized<true> >* _freePorts;
    std::vector<ProcessSignal*> _startupFreePorts;
    std::vector<ScheduleThread*> _threads;
    std::list<ScheduleThread*> _suspended;
    uint32_t _minThreads;
    uint32_t _maxThreads;
    uint32_t _activeThreads;
    uint32_t _prevThreadLevel; // thread level after the previous thread count adaptation
    uint32_t _unSuspendableThreads;
    boost::thread_specific_ptr<ScheduleThread> _thisThread;

    ElasticThreadAdapter _adapter;
    AdaptThread* _adaptThread;

    Distillery::CV _cv;
    Distillery::Mutex _cvMutex;

    volatile size_t _portsClosed; // only accessed when _cvMutex is acquired
    uint32_t _allInputPorts;
    boost::atomic<bool> _globalIsShutdown;
    boost::atomic<bool> _globalAllPortsClosed;
    uint64_t _prevNTuples;
    double _prevThroughput;

    static const long ONE_MILLISECOND;
    static const uint32_t queueSize;

    // for adjusting scheduler queues
    ElasticPortAdapter* _portAdapter;
    bool _adjustingQueueInProgress;
    bool _shouldValidatePerformance;
    std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> > operatorSigMap;
    AdjustRec* _lastQueueAdjustment;
    std::map<uint32_t, double> _threadPerfInfo;
    std::map<uint32_t, bool> _threadQueueAdjustInfo;
    int _adaptCount;
    double efficiencyFactor;

    boost::atomic<int> queueClosureRequested;
    boost::atomic<int> queueClosureProcessed;
};
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_SCHEDULED_QUEUE_H */
