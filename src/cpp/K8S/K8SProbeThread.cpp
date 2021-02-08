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

#include <K8S/K8SProbeThread.h>
#include <SPL/Runtime/ProcessingElement/BasePEImpl.h>
#include <fcntl.h>
#include <map>

DEBUG_NAMESPACE_USE
K8S_NAMESPACE_USE
UTILS_NAMESPACE_USE

K8SProbeThread::K8SProbeThread(PE const& pe)
  : m_pe(pe)
  , m_mutex()
  , m_cv()
  , m_running(true)
{}

void* K8SProbeThread::run(void* arg)
{
    std::unique_lock<std::mutex> lk(m_mutex);
    std::chrono::seconds shutdownTimer(0), shutdownTimeout(30);
    /*
     * Set the thread name.
     */
    setName("Probe");
    /*
     * Mark the process as live.
     */
    touch("/tmp/live");
    /*
     * Scan for unwanted behavior.
     */
    while (m_running) {
        /*
         * Sleep for 5 second.
         */
        if (m_cv.wait_for(lk, std::chrono::seconds(5)) == std::cv_status::no_timeout) {
            continue;
        }
        /*
         * Check if the PE is shutting down. If so, start the timeout timer.
         */
        if (m_pe->getImpl().getShutdownRequested() && shutdownTimer < shutdownTimeout) {
            shutdownTimer += std::chrono::seconds(5);
        }
        /*
         * If the timer passed the timeout, remove the liveness beacon and exit.
         */
        if (shutdownTimer >= shutdownTimeout) {
            SPCDBG(L_WARN, "PE shutdown timed out, removing liveness beacon", K8S_GENERAL);
            unlink("/tmp/live");
            break;
        }
    }
    /*
     */
    return NULL;
}

void K8SProbeThread::stop()
{
    m_running = false;
    m_cv.notify_all();
}

bool K8SProbeThread::isLive() const
{
    return true;
}

void K8SProbeThread::touch(std::string const& path)
{
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK, 0666);
    if (fd >= 0) {
        close(fd);
    }
}
