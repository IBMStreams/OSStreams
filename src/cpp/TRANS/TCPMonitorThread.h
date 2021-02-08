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

#ifndef TRANS_TCPMONITORTHREAD_H_
#define TRANS_TCPMONITORTHREAD_H_

#include <TRANS/TCPCommon.h>
#include <UTILS/Thread.h>
#include <UTILS/UTILSTypes.h>

#include <boost/noncopyable.hpp>
#include <cassert>

namespace Distillery {
class TCPInstance;

/**
 * @internal General monitoring thread currently used for heartbeat.
 */
class TCPMonitorThread
  : public Thread
  , private boost::noncopyable
{
  public:
    // TODO create task list and move time intervals to task configuration
    /** Interval between heartbeat packets sent.  Value must be >= 0 and
     *  smaller than heartbeat_timeout_milli. Default 3000 (ms).  */
    static const streams_time_interval_t heartbeatMillis_;

    TCPMonitorThread(TCPInstance* inst)
      : tcpInstance_(inst)
      , shutdown_(false)
    {
        assert(tcpInstance_ != NULL);
    }

    /** @Override Thread::run */
    void* run(void* threadArgs);

    /** Shut down the monitor thread */
    void shutdown();

  private:
    /**
     * Sleep for the specified duration; test the shutdown flag every
     * sliceMillis_ and return if set.
     */
    void sleep(const streams_time_interval_t millis);

    // not used
    TCPMonitorThread()
      : tcpInstance_(NULL)
      , shutdown_(false)
    {}

    TCPInstance* const tcpInstance_;
    volatile bool shutdown_;

    // Interval for sampling the shutdown flag
    static const streams_time_interval_t sliceMillis_;
};
}
#endif /* !TRANS_TCPMONITORTHREAD_H_ */
