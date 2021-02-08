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

#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/ScheduledQueue.h>

#include <time.h>

using namespace SPL;

const long ScheduledQueue::ONE_MILLISECOND = 1000000;
const uint32_t ScheduledQueue::queueSize = 1000;
const int64_t ADAPT_PERIOD = 10;
const double THRESHOLD = 0.05;
namespace SPL {
__thread bool isDraining = false;
}
ScheduledQueue::ScheduleThread::ScheduleThread()
  : _suspended(false)
  , _suspendable(true)
  , _inSuspension(false)
  , _shutdown(false)
  , _allPortsClosed(false)
  , _active(false)
{}
ScheduledQueue::ScheduleThread::~ScheduleThread() {}

void* ScheduledQueue::ScheduleThread::run(void* threadArgs)
{
    registerThread("Dynamic");

    OperatorTracker::init();

    ScheduledQueue* q = reinterpret_cast<ScheduledQueue*>(threadArgs);
    q->registerThread(this);
    try {
        q->schedule(_suspended, _shutdown, _allPortsClosed, _active);
    } catch (const Distillery::DistilleryException& e) {
        PEImpl& pe = PEImpl::instance();
        pe.handleOperatorFailure("Exception when executing operator: ", e.getExplanation());
        pe.shutdown();
    } catch (const std::exception& e) {
        PEImpl& pe = PEImpl::instance();
        pe.handleOperatorFailure("Exception when executing operator: ", e.what());
        pe.shutdown();
    } catch (...) {
        PEImpl& pe = PEImpl::instance();
        pe.handleOperatorFailure("Unknown exception when executing operator: ",
                                 SPL_RUNTIME_UNKNOWN_EXCEPTION);
        pe.shutdown();
    }
    q->unregisterThread();
    OperatorTracker::finalize();
    return NULL;
}

void ScheduledQueue::ScheduleThread::suspend()
{
    APPTRC(L_DEBUG, "Dynamic thread suspending", SPL_PE_DBG);
    _inSuspension = true;
    Distillery::AutoMutex am(_mutex);
    while (_suspended && !_shutdown && !_allPortsClosed) {
        _cv.wait(_mutex);
    }
    _inSuspension = false;
    APPTRC(L_DEBUG, "Dynamic thread waking up", SPL_PE_DBG);
}

void ScheduledQueue::ScheduleThread::block(long ns)
{
    Distillery::AutoMutex am(_mutex);
    timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = ns;
    _cv.waitFor(_mutex, delay);
}

void ScheduledQueue::ScheduleThread::notifySuspended()
{
    _suspended = true;
}

void ScheduledQueue::ScheduleThread::notifyWakeup()
{
    Distillery::AutoMutex am(_mutex);
    _suspended = false;
    _cv.signal();
}

void ScheduledQueue::ScheduleThread::notifyShutdown()
{
    Distillery::AutoMutex am(_mutex);
    _shutdown = true;
    _cv.signal();
}

void ScheduledQueue::ScheduleThread::notifyAllPortsClosed()
{
    Distillery::AutoMutex am(_mutex);
    _allPortsClosed = true;
    _cv.signal();
}

class ScheduledQueue::AdaptThread : public Thread
{
  public:
    AdaptThread(boost::atomic<bool>& s, boost::atomic<bool>& c, time_t p, ScheduledQueue& sq)
      : _isShutdown(s)
      , _allPortsClosed(c)
      , _period(p)
      , _sq(sq)
    {}

    void* run(void*)
    {
        while (!_isShutdown && !_allPortsClosed) {
            Distillery::AutoMutex am(_mutex);
            timespec delay;
            delay.tv_sec = _period;
            delay.tv_nsec = 0;

            _cv.waitFor(_mutex, delay);

            _sq.adapt();
        }

        return NULL;
    }

    void signal()
    {
        Distillery::AutoMutex am(_mutex);
        _cv.signal();
    }

  private:
    boost::atomic<bool>& _isShutdown;
    boost::atomic<bool>& _allPortsClosed;
    time_t _period;
    ScheduledQueue& _sq;
    Distillery::Mutex _mutex;
    Distillery::CV _cv;
};

ScheduledQueue::ScheduledQueue(uint32_t minThreads,
                               uint32_t maxThreads,
                               uint32_t activeThreads,
                               bool adaptOn,
                               uint32_t allInputPorts)
  : _freePorts(NULL)
  , _minThreads(minThreads)
  , _maxThreads(maxThreads)
  , _activeThreads(activeThreads)
  , _prevThreadLevel(0)
  , _unSuspendableThreads(0)
  , _adapter()
  , _adaptThread(NULL)
  , _portsClosed(0)
  , _allInputPorts(allInputPorts)
  , _globalIsShutdown(false)
  , _globalAllPortsClosed(false)
  , _prevNTuples(0)
  , _portAdapter(NULL)
  , _adaptCount(0)
{
    APPTRC(L_TRACE, "ScheduledQueue instantiation", SPL_PE_DBG);

    if (_minThreads == 0) {
        APPTRC(L_ERROR, "requested minThreads is 0; overriding to 1", SPL_PE_DBG);
        _minThreads = 1;
    }
    if (_maxThreads == 0) {
        APPTRC(L_ERROR, "requested maxThreads is 0; overriding to 1", SPL_PE_DBG);
        _maxThreads = 1;
    }
    if (_activeThreads == 0) {
        APPTRC(L_ERROR, "requested activeThreads is 0; overriding to 1", SPL_PE_DBG);
        _activeThreads = 1;
    }

    if (_minThreads > _maxThreads) {
        APPTRC(L_ERROR,
               "requested minThreads (" << _minThreads << ") is greater than maxThreads ("
                                        << _maxThreads << "), overriding minThreads to 1",
               SPL_PE_DBG);
        _minThreads = 1;
    }
    if (_activeThreads < _minThreads || _activeThreads > _maxThreads) {
        APPTRC(L_ERROR,
               "requested activeThreads (" << _activeThreads << ") is not between requested min ("
                                           << _minThreads << ") and max (" << _maxThreads
                                           << "), overriding to 1",
               SPL_PE_DBG);
        _activeThreads = 1;
    }

    for (uint32_t i = 0; i < _activeThreads; ++i) {
        _threads.push_back(new ScheduleThread());
    }

    if (adaptOn) {
        queueClosureProcessed = queueClosureRequested = 0;
        _adjustingQueueInProgress = true;
        _shouldValidatePerformance = false;
        efficiencyFactor = 0.6;
        _lastQueueAdjustment = NULL;
        _adapter.initialize(_minThreads, _maxThreads, _activeThreads);
        _portAdapter = new ElasticPortAdapter(*this);
        _adaptThread =
          new AdaptThread(_globalIsShutdown, _globalAllPortsClosed, ADAPT_PERIOD, *this);
    }
}

// Note: Must be performed before calling start:
// 1. We assume that the _queues hash table does not change during execution.
// 2. We build up the free ports list in a non-lock-free queue during startup so that
//    we know exactly how many scheduled ports are in the PE. We can then allocate a fixed-size
//    lockfree queue, which makes a big performance difference. Non-fixed size lockfree queues
//    can incur significant cost allocating new nodes.
void ScheduledQueue::registerOperatorPort(ProcessSignal* sig,
                                          OperatorMetricsImpl& opm,
                                          uint32_t port)
{
    APPTRC(L_TRACE, "ScheduledQueue registering port", SPL_PE_DBG);
    _startupFreePorts.push_back(sig);
    CircularQueue<ScheduledData>* queue = new CircularQueue<ScheduledData>(queueSize);
    for (uint32_t i = 0; i < queue->getCapacity(); ++i) {
        ScheduledData& data = queue->getRawData(i);
        data.data().setType(Data::Tuple);
        /*
         * BUG(xrg) the call below creates a new tuple that is never released.
         */
        Tuple* tuple = sig->createTuple().release();
        data.data().setTuple(*tuple);
        /*
         * BUG(xrg) the call below creates a new punct that is never released.
         */
        Punctuation* p = new Punctuation(Punctuation::Invalid);
        data.data().setPunctuation(*p);
    }
    uint32_t queueIndex = static_cast<uint32_t>(_queues.size());
    sig->setQueueIndex(queueIndex);
    _queues.push_back(new SPSCEnforcer(queue));
    opm.setQueueSize(port, queueSize);

    operatorSigMap[sig->getOperatorIndex()].push_back(sig);
    if (_portAdapter) {
        sig->setSkipScheduledPort();
        sig->completeDrain();
    }
}

void ScheduledQueue::registerThread(ScheduleThread* thread)
{
    _thisThread.reset(thread);
}

void ScheduledQueue::unregisterThread()
{
    // if we don't release the pointer, the ScheduleThread object will get deleted on
    // thread exit, causing problems (segfaults) in joinThread.
    _thisThread.release();
}

ScheduledQueue::~ScheduledQueue()
{
    APPTRC(L_TRACE, "ScheduledQueue destructing", SPL_PE_DBG);
    for (size_t i = 0; i < _threads.size(); ++i) {
        delete _threads[i];
    }
    for (std::vector<SPSCEnforcer*>::iterator it = _queues.begin(); it != _queues.end(); ++it) {
        (*it)->freeQueue();
    }
    if (_freePorts != NULL) {
        delete _freePorts;
    }
    if (_portAdapter != NULL) {
        delete _portAdapter;
    }
    if (_adaptThread != NULL) {
        delete _adaptThread;
    }
}

void ScheduledQueue::start()
{
    APPTRC(L_INFO, "ScheduledQueue allocating freePorts queue of size " << _startupFreePorts.size(),
           SPL_PE_DBG);
    _freePorts = new boost::lockfree::queue<ProcessSignal*, boost::lockfree::fixed_sized<true> >(
      _startupFreePorts.size());
    for (size_t i = 0; i < _startupFreePorts.size(); ++i) {
        _freePorts->push(_startupFreePorts[i]);
    }
    _startupFreePorts.clear();

    APPTRC(L_INFO, "ScheduledQueue starting " << _threads.size() << " threads", SPL_PE_DBG);
    for (size_t i = 0; i < _threads.size(); ++i) {
        _threads[i]->create(NULL, false, this);
    }

    if (_adaptThread) {
        _adaptThread->create(NULL, false, NULL);
    }
}

void ScheduledQueue::createNewThread()
{
    Distillery::AutoMutex am(_cvMutex);
    APPTRC(L_DEBUG, "ScheduledQueue starting a new thread", SPL_PE_DBG);
    _threads.push_back(new ScheduleThread());
    _threads.back()->create(NULL, false, this);
}

void ScheduledQueue::createNewThreads(uint32_t toCreate)
{
    for (size_t i = 0; i < toCreate; ++i) {
        createNewThread();
    }
}

void ScheduledQueue::suspendThreads(uint32_t toSuspend)
{
    APPTRC(L_DEBUG, "Suspending " << toSuspend << " threads", SPL_PE_DBG);
    uint32_t suspended = 0;
    for (size_t i = 0; i < _threads.size(); ++i) {
        ScheduleThread* t = _threads[i];
        if (t->isSuspendable() && !t->isSuspended()) {
            t->notifySuspended();
            _suspended.push_back(t);
            ++suspended;
            if (suspended == toSuspend) {
                break;
            }
        }
    }
    if (suspended != toSuspend) {
        APPTRC(L_ERROR,
               "Suspended " << suspended << " threads, but was supposed to suspend " << toSuspend
                            << "threads",
               SPL_PE_DBG);
    }
}

void ScheduledQueue::wakeupThreads(uint32_t toWakeup)
{
    APPTRC(L_DEBUG, "Waking up " << toWakeup << " threads", SPL_PE_DBG);
    for (size_t i = 0; i < toWakeup; ++i) {
        _suspended.front()->notifyWakeup();
        _suspended.pop_front();
    }
}

uint32_t ScheduledQueue::findUnSuspendableThreads()
{
    uint32_t newUnsuspendable = 0;
    for (std::list<ScheduleThread*>::iterator it = _suspended.begin(); it != _suspended.end();) {
        if (!(*it)->isInSuspension()) {
            (*it)->notSuspendable();
            it = _suspended.erase(it);
            ++newUnsuspendable;
            ++_unSuspendableThreads;
        } else {
            ++it;
        }
    }
    return newUnsuspendable;
}

bool ScheduledQueue::someThreadsRunnable()
{
    bool runnable = false;
    for (size_t i = 0; i < _threads.size(); ++i) {
        runnable = runnable || (_threads[i]->isActive() || _threads[i]->isInSuspension());
        _threads[i]->resetActive();
    }
    return runnable;
}

void ScheduledQueue::increaseMinThreads(uint32_t newMin)
{
    APPTRC(L_INFO,
           "Increasing min thread level because of unavailable threads; new min level is "
             << newMin,
           SPL_PE_DBG);
    if (newMin > _threads.size()) {
        createNewThreads(newMin - _threads.size());
    }
    _minThreads = newMin;
    _activeThreads = _threads.size();
    APPTRC(L_INFO, "Waking up all threads and resetting thread level histories", SPL_PE_DBG);
    for (size_t i = 0; i < _threads.size(); ++i) {
        _threads[i]->notifyWakeup();
    }
    _suspended.clear();
    _adapter.initialize(_minThreads, _maxThreads, _activeThreads);
}

void ScheduledQueue::shutdown()
{
    Distillery::AutoMutex am(_cvMutex);
    APPTRC(L_INFO, "ScheduledQueue shutdown", SPL_PE_DBG);
    _globalIsShutdown = true;
    for (size_t i = 0; i < _threads.size(); ++i) {
        _threads[i]->notifyShutdown();
    }
    _cv.broadcast();
}

void ScheduledQueue::portClosed()
{
    Distillery::AutoMutex am(_cvMutex);
    ++_portsClosed;
    APPTRC(L_DEBUG, "ScheduledQueue port closing; total closed is " << _portsClosed, SPL_PE_DBG);
    if (_portsClosed == _allInputPorts) {
        _globalAllPortsClosed = true;
        for (size_t i = 0; i < _threads.size(); ++i) {
            _threads[i]->notifyAllPortsClosed();
        }
        _cv.broadcast();
        APPTRC(L_INFO, "ScheduledQueue all ports closed", SPL_PE_DBG);
    }
}

void ScheduledQueue::joinThreads()
{
    APPTRC(L_DEBUG, "ScheduledQueue join threads ...", SPL_PE_DBG);
    Distillery::AutoMutex am(_cvMutex);

    // we do a cv wait here so that we avoid race conditions between joining and adding new
    // threads; shutdown() wakes us up
    if (!_globalIsShutdown && !_globalAllPortsClosed) {
        _cv.wait(_cvMutex);
    }

    if (_adaptThread) {
        _adaptThread->signal();
        _adaptThread->join();
    }
    for (size_t i = 0; i < _threads.size(); ++i) {
        _threads[i]->join();
    }

    APPTRC(L_DEBUG, "ScheduledQueue all threads joined", SPL_PE_DBG);
}

void ScheduledQueue::schedule(const boost::atomic<bool>& suspended,
                              const boost::atomic<bool>& localShutdown,
                              const boost::atomic<bool>& localAllPortsClosed,
                              boost::atomic<bool>& active)
{
    APPTRC(L_DEBUG, "ScheduledQueue thread starting", SPL_PE_DBG);

    ScheduledData data;
    uint64_t processed = 0;
    /*
     * When returning true, the SPSCEnforcer consumer is locked.
     */
    while (findWorkBlocking(data, suspended, localShutdown, localAllPortsClosed, active)) {
        ProcessSignal* port = data.port();
        if (port->shouldSkipScheduledPort() && port->tryDrain()) {
            execute(data);
            ++processed;

            SPSCEnforcer* mq = _queues[data.port()->getQueueIndex()];
            uint32_t localProcessed = 0;
            while (mq->front(data)) {
                execute(data);
                ++processed;
                ++localProcessed;
                if (localProcessed > queueSize || suspended || localShutdown ||
                    localAllPortsClosed) {
                    break;
                }
            }
            queueClosureProcessed++;
            freePort(data.port());
            port->completeDrain();
        } else {
            execute(data);
            ++processed;

            SPSCEnforcer* mq = _queues[data.port()->getQueueIndex()];
            uint32_t localProcessed = 0;
            while (mq->front(data)) {
                execute(data);
                ++processed;
                ++localProcessed;
                if (localProcessed > queueSize || suspended || localShutdown ||
                    localAllPortsClosed) {
                    break;
                }
            }

            freePort(data.port());
        }
    }

    std::stringstream reason;
    reason << "ScheduledQueue scheduler exiting (with " << processed << " processed) "
           << "because ";
    if (_globalIsShutdown) {
        reason << "shutdown requested; ";
    }
    if (_globalAllPortsClosed) {
        reason << "all ports closed";
    }
    APPTRC(L_DEBUG, reason.str(), SPL_PE_DBG);
}

void ScheduledQueue::adapt()
{
    if (_globalIsShutdown || _globalAllPortsClosed) {
        return;
    }
    uint64_t nTuples = PEImpl::instance().getOperatorsNTuplesSubmitted();
    uint64_t diff = nTuples - _prevNTuples;
    double throughput = diff / static_cast<double>(ADAPT_PERIOD);
    if (_adaptCount > 0) {
        APPTRC(L_INFO, "throughput now " << throughput << " previous " << _prevThroughput,
               SPL_PE_DBG);
    } else {
        APPTRC(L_DEBUG, "throughput now " << throughput << " previous " << _prevThroughput,
               SPL_PE_DBG);
    }
    _prevNTuples = nTuples;
    _prevThroughput = throughput;
    _adaptCount++;

    if (queueClosureProcessed != queueClosureRequested) {
        APPTRC(L_INFO,
               "request to drain " << queueClosureRequested << " queues, drained "
                                   << queueClosureProcessed << " queues",
               SPL_PE_DBG);
        return;
    } else if (queueClosureRequested != 0) {
        queueClosureRequested = queueClosureProcessed = 0;
        _adaptCount = 0;
        return;
    }

    if (_adaptCount > 0) {
        if (_adjustingQueueInProgress) {
            APPTRC(L_INFO, "choose queue elasticity", SPL_PE_DBG);
            queueElasticity(throughput);
        } else {
            APPTRC(L_INFO, "choose thread elasticity", SPL_PE_DBG);
            threadElasticity(throughput);
        }
    }
}

void ScheduledQueue::queueElasticity(double& throughput)
{
    // determine if a change is needed;
    if (_shouldValidatePerformance) {
        validatePerformance(throughput);
    } else {
        _threadQueueAdjustInfo[_activeThreads] = true;
        _portAdapter->adjust(throughput);
    }
}

void ScheduledQueue::validatePerformance(double& throughput)
{
    if (_prevThreadLevel != 0 && _activeThreads > _prevThreadLevel) {
        std::map<uint32_t, double>::iterator it = _threadPerfInfo.find(_prevThreadLevel);
        assert(it != _threadPerfInfo.end());
        double expectedEffiency = efficiencyFactor * ((double)(_activeThreads) / _prevThreadLevel);
        if (throughput / it->second > std::max(1.05, expectedEffiency)) {
            _threadQueueAdjustInfo[_activeThreads] = false;
            adjustingDone(throughput);
            return;
        }
    }

    _shouldValidatePerformance = false;
    _threadQueueAdjustInfo[_activeThreads] = true;
    _portAdapter->adjust(throughput);
}

void ScheduledQueue::threadElasticity(double& throughput)
{
    _threadPerfInfo[_activeThreads] = throughput;
    uint32_t unsuspendable = findUnSuspendableThreads();
    bool runnable = someThreadsRunnable();
    bool loadChanged = false;
    _prevThreadLevel = _activeThreads;
    if (unsuspendable == 0 && runnable) {
        uint32_t old = _activeThreads;
        uint32_t threads = _adapter.calculateThreads(throughput, loadChanged);
        if (threads < _activeThreads) {
            suspendThreads(_activeThreads - threads);
        } else if (threads > _activeThreads) {
            if (threads > _threads.size()) {
                createNewThreads(threads - _threads.size());
            } else {
                wakeupThreads(threads - _activeThreads);
            }
        }
        _activeThreads = threads;
        APPTRC(L_INFO,
               "" << old << " threads -> " << throughput << " tuples/s; now has " << _activeThreads
                  << " threads",
               SPL_PE_DBG);
    } else if (!runnable) {
        APPTRC(L_INFO, "Delaying adaptation: no runnable threads; -> " << throughput << " tuples/s",
               SPL_PE_DBG);
        increaseMinThreads(_threads.size() + 1);
        return;
    } else {
        APPTRC(L_INFO,
               "Delaying adaptation: "
                 << _activeThreads << " threads are supposed to be active, but " << unsuspendable
                 << " threads have not been suspended; -> " << throughput << " tuples/s",
               SPL_PE_DBG);
        if (_unSuspendableThreads > _minThreads) {
            increaseMinThreads(_unSuspendableThreads);
        } else {
            suspendThreads(unsuspendable);
        }
        return;
    }

    if (loadChanged) {
        if (_lastQueueAdjustment != NULL) {
            delete _lastQueueAdjustment;
        }
        _lastQueueAdjustment = NULL;
        _threadPerfInfo.clear();
        _threadQueueAdjustInfo.clear();
        _adjustingQueueInProgress = true;
        _shouldValidatePerformance = false;
        _prevThreadLevel = 0;
        _adaptCount = 0;
        _portAdapter->clear();
        return;
    }

    if (_prevThreadLevel == _activeThreads) {
        std::map<uint32_t, bool>::iterator it = _threadQueueAdjustInfo.find(_activeThreads);
        if (it != _threadQueueAdjustInfo.end() && it->second) {
            // queue adjustment has done before
            return;
        }
    }
    if (_lastQueueAdjustment != NULL) {
        uint32_t lastAdjThreadNum = _lastQueueAdjustment->getThreadCount();
        shouldTriggerQueueElasticity(lastAdjThreadNum, _activeThreads);
    } else {
        _adjustingQueueInProgress = true;
        _shouldValidatePerformance = true;
        _adaptCount = 0;
        _portAdapter->initialize(UP);
    }
}

void ScheduledQueue::shouldTriggerQueueElasticity(uint32_t& lastAdjThreadNum,
                                                  uint32_t& _activeThread)
{
    if (lastAdjThreadNum != _activeThreads) {
        if (_lastQueueAdjustment->toContinue(_activeThreads,
                                             lastAdjThreadNum > _activeThreads ? false : true)) {
            _adjustingQueueInProgress = true;
            if (_activeThreads == _maxThreads) {
                // force queue adjustment if it is the maximum number of threads
                _shouldValidatePerformance = false;
            } else {
                _shouldValidatePerformance = true;
            }
            _adaptCount = -1;
            _portAdapter->initialize(lastAdjThreadNum > _activeThreads ? DOWN : UP);
        }
    }
}

static double getTimeOfDay()
{
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return tval.tv_sec;
}

void ScheduledQueue::turnOnScheduledPort(std::vector<uint32_t>& operators, std::pair<int, int>& p)
{
    unsigned int toChange = 0;
    for (int i = p.first; i < p.second; ++i) {
        std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
          operatorSigMap.find(operators[i]);
        if (it != operatorSigMap.end()) {
            for (unsigned int j = 0; j < it->second.size(); ++j) {
                if (!it->second[j]->isNonChangable()) {
                    toChange++;
                }
                it->second[j]->unsetFlag();
            }
        }
    }

    if (toChange > 0) {
        unsigned int changed = 0;
        double timeStart = getTimeOfDay();
        while (true) {
            for (int i = p.first; i < p.second; ++i) {
                std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
                  operatorSigMap.find(operators[i]);
                if (it != operatorSigMap.end()) {
                    for (unsigned int j = 0; j < it->second.size(); ++j) {
                        if (!(it->second[j]->isNonChangable()) && !(it->second[j]->hasFlagSet())) {
                            SPSCEnforcer* scq = _queues[(it->second[j])->getQueueIndex()];
                            if (scq->tryLockProducer()) {
                                if (scq->tryLockConsumer()) {
                                    it->second[j]->resetSkipScheduledPort();
                                    changed++;
                                    scq->unLockConsumer();
                                }
                                scq->unLockProducer();
                            }
                        }
                    }
                }
            }
            if (changed == toChange) {
                break;
            }
            double timeEnd = getTimeOfDay();
            if (timeEnd - timeStart > 10) {
                break;
            }
        }

        // mark the port as nonchangeable
        for (int i = p.first; i < p.second; ++i) {
            std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
              operatorSigMap.find(operators[i]);
            if (it != operatorSigMap.end()) {
                for (unsigned int j = 0; j < it->second.size(); ++j) {
                    if (!it->second[j]->hasFlagSet()) {
                        it->second[j]->setNonChangable();
                    }
                    it->second[j]->unsetFlag();
                }
            }
        }

        APPTRC(L_INFO,
               "turn on " << changed << " scheduled ports, found " << toChange - changed
                          << " non changable ports",
               SPL_PE_DBG);
    }
    _adaptCount = 0;
}

void ScheduledQueue::turnOffScheduledPort(std::vector<uint32_t>& operators, std::pair<int, int>& p)
{
    unsigned int toChange = 0;
    for (int i = p.first; i < p.second; ++i) {
        std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
          operatorSigMap.find(operators[i]);
        if (it != operatorSigMap.end()) {
            for (unsigned int j = 0; j < it->second.size(); ++j) {
                if (!it->second[j]->isNonChangable()) {
                    toChange++;
                }
                it->second[j]->unsetFlag();
            }
        }
    }

    if (toChange > 0) {
        unsigned int changed = 0;
        double timeStart = getTimeOfDay();
        while (true) {
            for (int i = p.first; i < p.second; ++i) {
                std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
                  operatorSigMap.find(operators[i]);
                if (it != operatorSigMap.end()) {
                    for (unsigned int j = 0; j < it->second.size(); ++j) {
                        if (!it->second[j]->isNonChangable() && !it->second[j]->hasFlagSet()) {
                            SPSCEnforcer* scq = _queues[(it->second[j])->getQueueIndex()];
                            if (scq->tryLockProducer()) {
                                if (scq->tryLockConsumer()) {
                                    it->second[j]->setSkipScheduledPort();
                                    changed++;
                                    queueClosureRequested++;
                                    scq->unLockConsumer();
                                }
                                scq->unLockProducer();
                            }
                        }
                    }
                }
            }
            if (changed == toChange) {
                break;
            }
            double timeEnd = getTimeOfDay();
            if (timeEnd - timeStart > 10) {
                break;
            }
        }

        // mark the port as nonchangeable
        for (int i = p.first; i < p.second; ++i) {
            std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
              operatorSigMap.find(operators[i]);
            if (it != operatorSigMap.end()) {
                for (unsigned int j = 0; j < it->second.size(); ++j) {
                    if (!it->second[j]->hasFlagSet()) {
                        it->second[j]->setNonChangable();
                    }
                    it->second[j]->unsetFlag();
                }
            }
        }

        APPTRC(L_INFO,
               "turn off " << changed << " scheduled ports, found " << toChange - changed
                           << " non changable ports",
               SPL_PE_DBG);
    }

    _adaptCount = 0;
}

// used when load has changed
void ScheduledQueue::turnOffScheduledPort(std::set<uint32_t>& operators)
{
    unsigned int toChange = 0;
    for (std::set<uint32_t>::iterator ij = operators.begin(); ij != operators.end(); ij++) {
        std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
          operatorSigMap.find(*ij);
        if (it != operatorSigMap.end()) {
            for (unsigned int j = 0; j < it->second.size(); ++j) {
                if (!it->second[j]->isNonChangable()) {
                    toChange++;
                }
                it->second[j]->unsetFlag();
            }
        }
    }
    if (toChange > 0) {
        unsigned int changed = 0;
        double timeStart = getTimeOfDay();
        while (true) {
            for (std::set<uint32_t>::iterator ij = operators.begin(); ij != operators.end(); ij++) {
                std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
                  operatorSigMap.find(*ij);
                if (it != operatorSigMap.end()) {
                    for (unsigned int j = 0; j < it->second.size(); ++j) {
                        if (!it->second[j]->isNonChangable() && !it->second[j]->hasFlagSet()) {
                            SPSCEnforcer* scq = _queues[(it->second[j])->getQueueIndex()];
                            if (scq->tryLockProducer()) {
                                if (scq->tryLockConsumer()) {
                                    it->second[j]->setSkipScheduledPort();
                                    changed++;
                                    queueClosureRequested++;
                                    scq->unLockConsumer();
                                }
                                scq->unLockProducer();
                            }
                        }
                    }
                }
            }
            if (changed == toChange) {
                break;
            }
            double timeEnd = getTimeOfDay();
            if (timeEnd - timeStart > 10) {
                break;
            }
        }

        // mark the port as nonchangeable
        for (std::set<uint32_t>::iterator ij = operators.begin(); ij != operators.end(); ij++) {
            std::tr1::unordered_map<int32_t, std::vector<ProcessSignal*> >::iterator it =
              operatorSigMap.find(*ij);
            if (it != operatorSigMap.end()) {
                for (unsigned int j = 0; j < it->second.size(); ++j) {
                    if (!it->second[j]->hasFlagSet()) {
                        it->second[j]->setNonChangable();
                    }
                    it->second[j]->unsetFlag();
                }
            }
        }

        APPTRC(L_INFO,
               "turn off " << changed << " scheduled ports, found " << toChange - changed
                           << " non changable ports",
               SPL_PE_DBG);
    }

    _adaptCount = 0;
}

bool ScheduledQueue::hasScheduledPort(uint32_t op)
{
    return operatorSigMap.find(op) != operatorSigMap.end();
}

uint32_t ScheduledQueue::getScheduledPortCount()
{
    return operatorSigMap.size();
}

void ScheduledQueue::adjustingDone(bool recordHist,
                                   AdjustDirection dir,
                                   double throughput,
                                   std::vector<uint32_t>& affectedPorts)
{
    if (recordHist) {
        if (_lastQueueAdjustment != NULL && throughput <= _lastQueueAdjustment->getThroughput()) {
            // The performance of the new combination of thread&queue adjustment is worse, revert
            // the changes We do it here because we want to ensure each queue adjustment is positive
            // It does not hurt if we do not revert, just that more adjustments will be needed when
            // thread count changes
            if (dir == UP) {
                std::pair<int, int> r(0, (int)(affectedPorts.size()));
                turnOffScheduledPort(affectedPorts, r);
                _portAdapter->confirmToAdd(affectedPorts, r);
            } else {
                std::pair<int, int> r(0, (int)(affectedPorts.size()));
                turnOnScheduledPort(affectedPorts, r);
                _portAdapter->confirmToAdd(affectedPorts, r);
            }
            // the previous queue adjustment works for this thread count after reverting
            _lastQueueAdjustment->addThread(_activeThreads);
        } else {
            // now we have changed the queue elasticity, we need to update the information in the
            // threadAdapter
            _adapter.untrustLevel(_activeThreads, throughput);
            if (dir == UP) {
                _adapter.untrustAbove();
            }
            if (_lastQueueAdjustment != NULL) {
                delete _lastQueueAdjustment;
            }
            _lastQueueAdjustment = new AdjustRec(_activeThreads, throughput);
            _threadPerfInfo[_activeThreads] = throughput;
        }
    } else {
        // no queue adjustment has been made
        if (_lastQueueAdjustment != NULL) {
            if (throughput > _lastQueueAdjustment->getThroughput()) {
                _lastQueueAdjustment->updateThroughput(throughput);
            }
            _lastQueueAdjustment->addThread(_activeThreads);
        } else {
            _lastQueueAdjustment = new AdjustRec(_activeThreads, throughput, false);
        }
    }
    // mark the queueElasticity is done, switch to threadElasticity
    _adjustingQueueInProgress = false;
    _adaptCount = -1;
}

void ScheduledQueue::adjustingDone(double throughput)
{
    if (_lastQueueAdjustment != NULL) {
        if (throughput > _lastQueueAdjustment->getThroughput()) {
            _lastQueueAdjustment->updateThroughput(throughput);
        }
    }
    // mark the queueElasticity is done, switch to threadElasticity
    _adjustingQueueInProgress = false;
    _adaptCount = 0;
}
