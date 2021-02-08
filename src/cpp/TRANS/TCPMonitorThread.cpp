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

#include <TRANS/TCPInstance.h>
#include <TRANS/TCPMonitorThread.h>
#include <TRC/RuntimeTrcAspects.h>

#include <algorithm>

using namespace Distillery;
using namespace Distillery::debug;
using namespace std;

const streams_time_interval_t TCPMonitorThread::heartbeatMillis_ = 3000;
const streams_time_interval_t TCPMonitorThread::sliceMillis_ = 500;

void* TCPMonitorThread::run(void* threadArgs)
{
    while (1) {
        streams_time_t beginTime = TCPCommon::sysTimeMillis();
        tcpInstance_->doHeartbeat(&shutdown_, beginTime - heartbeatMillis_, beginTime);
        if (shutdown_) {
            SPCDBG(L_INFO, "TCPMonitorThread will end", CORE_TRANS_TCP);
            break;
        }

        // sleep at least minSleepBetweenRuns and up to intervalMillis
        streams_time_interval_t toNextRun =
          (beginTime + heartbeatMillis_ - TCPCommon::sysTimeMillis());
        this->sleep(toNextRun);
    } // end while
    return NULL;
}

void TCPMonitorThread::shutdown()
{
    shutdown_ = true;
    if (hasStarted) {
        SPCDBG(L_INFO, "Shutting down TCPMonitorThread", CORE_TRANS_TCP);
        void* dummy;
        this->join((void*)&dummy);
        hasStarted = false;
        SPCDBG(L_INFO, "TCPMonitorThread has stopped", CORE_TRANS_TCP);
    }
}

// Sleep 'duration' but check for shutdown every sliceMillis_ and return
// if shutdown_ is set.
void TCPMonitorThread::sleep(const streams_time_interval_t duration)
{
    if (duration <= 0) {
        return;
    }

    streams_time_interval_t d = duration;
    for (streams_time_interval_t slice = std::min(duration, TCPMonitorThread::sliceMillis_);
         !shutdown_ && d > 0; d -= slice) {
        usleep(slice * 1000); // usecs
    }
}
