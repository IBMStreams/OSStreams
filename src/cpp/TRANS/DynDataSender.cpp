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

#include <TRANS/DynDataSender.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/CV.h>
#include <UTILS/HostInfo.h>
#include <UTILS/Mutex.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/Thread.h>

#include <cassert>
#include <errno.h>
#include <netinet/in.h>

UTILS_NAMESPACE_USE;
using namespace std;
DEBUG_NAMESPACE_USE

UTILS_NAMESPACE_BEGIN

// Provide a connection helper thread service.
class ConnectionHelperImpl
  : public ConnectionHelper
  , private Thread
{
  public:
    ConnectionHelperImpl(DynDataSender* dds);
    ~ConnectionHelperImpl();
    void setNeedHelp();
    void* run(void* args);
    void shutdown();

  private:
    DynDataSender* _dds;
    CV _cv;
    Mutex _m;
    bool _waitingForWork; // optimization: reduce overhead when setNeedHelp() called frequently
    bool _needHelp;       // true when the background connect processing needs to be run
    bool _shutdown;
};

UTILS_NAMESPACE_END

DynDataSender::DynDataSender(DataSender* unprotected_client)
  : _unprotected_sender(unprotected_client)
{
    assert(_unprotected_sender);
    _unprotected_sender->connsSynchronizedOnWrite_ = true; // PERFORMANCE
    if (_unprotected_sender->needsHelper()) {
        _helper = boost::shared_ptr<ConnectionHelper>(new ConnectionHelperImpl(this));
        _unprotected_sender->setConnectionHelper(_helper);
    }
}

DynDataSender::~DynDataSender()
{
    delete _unprotected_sender;
}

void DynDataSender::shutdown()
{
    SPCDBG(L_TRACE, "Enter DynDataSender::shutdown()", CORE_UTILS);

    _unprotected_sender->shutdown();

    if (_helper.get() != 0) {
        (dynamic_cast<ConnectionHelperImpl*>(_helper.get()))->shutdown();
    }

    SPCDBG(L_TRACE, "Exit DynDataSender::shutdown()", CORE_UTILS);
}

ConnectionHelperImpl::ConnectionHelperImpl(DynDataSender* dds)
  : _dds(dds)
  , _waitingForWork(false)
  , _needHelp(false)
  , _shutdown(false)
{
    create(); // create/start the thread
}

ConnectionHelperImpl::~ConnectionHelperImpl()
{
    shutdown();
}

void ConnectionHelperImpl::shutdown()
{
    SPCDBG(L_TRACE, "Enter ConnectionHelperImpl::shutdown()", CORE_UTILS);

    bool alreadyShutdown;
    {
        AutoMutex l(_m);
        alreadyShutdown = _shutdown;
        _shutdown = true;
        _cv.signal();
    }

    if (!alreadyShutdown) {
        SPCDBG(L_DEBUG, "waiting for helper thread to shutdown...", CORE_UTILS);

        join();

        SPCDBG(L_DEBUG, "thread shutdown complete", CORE_UTILS);
    }
    SPCDBG(L_TRACE, "Exit ConnectionHelperImpl::shutdown()", CORE_UTILS);
}

void ConnectionHelperImpl::setNeedHelp()
{
    SPCDBG(L_TRACE, "Enter ConnectionHelperImpl::setNeedHelp()", CORE_UTILS);
    {
        AutoMutex l(_m);
        _needHelp = true;
        if (_waitingForWork) {
            _cv.signal();
            _waitingForWork = false;
        }
    }
    SPCDBG(L_TRACE, "Exit ConnectionHelperImpl::setNeedHelp()", CORE_UTILS);
}

void* ConnectionHelperImpl::run(void* args)
{
    SPCDBG(L_TRACE, "Enter ConnectionHelperImpl::run()", CORE_UTILS);

    while (1) {
        {
            AutoMutex l(_m);

            if (!_needHelp) {
                // Wait for setNeedHelp() to tell us there's something to do

                _waitingForWork = true;

                while (!_needHelp && !_shutdown) {
                    _cv.wait(_m);
                }
            } else {
                // There's more to do but throttle our scans/processing.
                // Wait for a throttle timeout.
                //
                // This also lessens the overhead of setNeedHelp()
                // (which can end up getting called as frequently as write()
                // is called).  It also prevents unnecessary overhead/wakeups
                // while we're awaiting the throttle timeout.

                _waitingForWork = false;

                static const int HELPER_MIN_WAIT_SEC = 1;
                struct timespec relto = { HELPER_MIN_WAIT_SEC, 0 };
                bool timedout = false;

                while (!timedout && !_shutdown) {
                    timedout = _cv.waitFor(_m, relto);
                }
            }

            if (_shutdown) {
                SPCDBG(L_DEBUG, "ConnectionHelperImpl shutting down", CORE_UTILS);
                break;
            }

            _waitingForWork = false; // don't signal when we're not waiting
            _needHelp = false;       // detect new requests for help after we unlock
        }

        // Must NOT be holding the lock when doing this or deadlock can result

        // The transport's processing is obligated to call setNeedHelp() again
        // (as part of its continueConnects() processing) if it wasn't successful
        // and more background processing is required.

        _dds->continueConnects();
    }

    SPCDBG(L_TRACE, "Exit ConnectionHelperImpl::run()", CORE_UTILS);
    return 0;
}
