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

#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/PthreadHelper.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/Thread.h>
#include <cassert>
#include <cerrno>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <TRC/DistilleryDebug.h>

using namespace std;
UTILS_NAMESPACE_USE

UTILS_NAMESPACE_BEGIN

/// Info for signaling thread starting up
class SignalStartedInfo
{
  public:
    SignalStartedInfo()
      : signalledStarted(false){};
    ~SignalStartedInfo(){};

    Mutex m;
    CV cv;
    bool signalledStarted;
};

/// Thread information object
struct ThreadInfo_t
{
    ThreadInfo_t(Thread* t, void* tArgs)
      : threadObject(t)
      , threadArgs(tArgs)
      , ssp(0){};

    /// pointer to the thread object
    Thread* threadObject;
    /// pointer to the thread's arguments
    void* threadArgs;
    /// other internal state (for signaling thread starting up)
    SignalStartedInfo* ssp;
};

void* threadStart(void* p_in)
{

    auto_ptr<ThreadInfo_t> p((ThreadInfo_t*)p_in);

    if (p->ssp != 0) {
        AutoMutex(p->ssp->m);
        p->ssp->signalledStarted = true;
        p->ssp->cv.signal();
        p->ssp = 0;
    }

    (p->threadObject)->OSTid = gettid();

    void* ret = (p->threadObject)->run(p->threadArgs);

    return ret;
}

UTILS_NAMESPACE_END

long Thread::CLOCK_TICK_LONG = sysconf(_SC_CLK_TCK);
double Thread::CLOCK_TICK_DOUBLE = sysconf(_SC_CLK_TCK);

Thread::Thread(void)
  : myTid(0)
  , hasStarted(false)
  , isDetached(false)
{}

Thread::Thread(const Thread&)
{
    assert(0);
}

int Thread::create(pthread_attr_t* myAttrs,
                   bool detachIt,
                   void* threadArgs,
                   bool signalStart,
                   size_t stackSize,
                   pthread_t* id)
{

    int ret = 0;
    pthread_attr_t attr;

    if (myAttrs == NULL) {
        PTHREAD_CALL(pthread_attr_init(&attr), ret);
        if (ret != 0) {
            return ret;
        }

        if (detachIt) {
            PTHREAD_CALL(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED), ret);
            isDetached = true;
        } else
            PTHREAD_CALL(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE), ret);
        if (ret != 0) {
            return ret;
        }

        myAttrs = &attr;
    }

    SPCDBG(L_DEBUG, "Requested stack size of " << stackSize << " bytes", "Core.Threads");
    if (stackSize) {
        SPCDBG(L_DEBUG, "Setting requested stack size of " << stackSize << " bytes",
               "Core.Threads");
        PTHREAD_CALL_NORET(pthread_attr_setstacksize(&attr, stackSize));
    }

    // build arg to pass to the new thread
    auto_ptr<ThreadInfo_t> p(new ThreadInfo_t(this, threadArgs)); // new thread will own stg
    auto_ptr<SignalStartedInfo> ssp;                              // we always own this stg
    if (signalStart) {
        ssp.reset(new SignalStartedInfo);
        p->ssp = ssp.get();
    }

    PTHREAD_CALL_NORET(pthread_create(&myTid, myAttrs, threadStart, (void*)p.get()));
    p.release(); // success.  thread owns the stg
    if (id != NULL) {
        *id = myTid;
    }

    hasStarted = true;

    if (signalStart) {
        AutoMutex(ssp->m);
        while (!ssp->signalledStarted) {
            ssp->cv.wait(ssp->m);
        }
    }

    PTHREAD_CALL_NORET(pthread_attr_destroy(&attr));

    return 0;
}

int Thread::join(void* status)
{
    int ret = 0;
    PTHREAD_CALL(pthread_join(myTid, &status), ret);
    return ret;
}

void Thread::detach(void)
{
    if (isDetached) {
        return;
    }
    if (!hasStarted) {
        THROW(PthreadFailure, "thread has not started yet");
    }
    if (pthread_equal(myTid, pthread_self()) || hasStarted) {
        PTHREAD_CALL_NORET(pthread_detach(myTid));
    }
    isDetached = true;
}

void Thread::kill(int sig)
{
    PTHREAD_CALL_NORET(pthread_kill(myTid, sig));
}

void Thread::cancel(void)
{
    PTHREAD_CALL_NORET(pthread_cancel(myTid));
}

void Thread::enableCancel(int type)
{
    PTHREAD_CALL_NORET(pthread_setcanceltype(type, 0));
    PTHREAD_CALL_NORET(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0));
}

void Thread::disableCancel(void)
{
    PTHREAD_CALL_NORET(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0));
}

pthread_t Thread::getPTHid(void) const
{
    return myTid;
}

pid_t Thread::getOSTHid(void) const
{
    return OSTid;
}

// I would prefer to use pthread_setname_np, but that requires a version of glibc that
// not all of our supported systems have at the moment.
static void callPrctlSetName(const char* name)
{
    int res = prctl(PR_SET_NAME, name);
    if (res < 0) {
        const char* errstr;
        char err[256];
        const char* rc = strerror_r(res, err, 256);
        if (rc == NULL) {
            errstr = "could not retrieve the message corresponding to the error number";
        } else {
            errstr = rc;
        }
        SPCDBG(L_DEBUG, "Unable to set thread name, error: " << errstr, "Core.Threads");
    }
}

void Thread::setName(const std::string& name)
{
    // the length of 15 is a restriction of the thread name; see
    // the man page
    if (name.size() > 15) {
        // we're doing a reverse truncation; the rationale here is that in Streams, the
        // end of the full operator name is more helpful than the beginning
        std::string truncated = name.substr(name.size() - 15, 15);
        SPCDBG(L_DEBUG, "Thread requested name " << name << ", but truncated to " << truncated,
               "Core.Threads");
        callPrctlSetName(truncated.c_str());
    } else {
        SPCDBG(L_DEBUG, "Setting thread name to " << name, "Core.Threads");
        callPrctlSetName(name.c_str());
    }
}

void Thread::getcpuclockid(clockid_t& cid)
{
    PTHREAD_CALL_NORET(pthread_getcpuclockid(pthread_self(), &cid));
}

void Thread::startTimer(void)
{
    struct timespec ts = { 0, 0 };
    clockid_t cid;
    getcpuclockid(cid);
    ERRNO_EX(clock_settime(cid, &ts) != 0, PthreadFailure, "failed clock_settime");
}

double Thread::endTimer(void)
{
    struct timespec ts;
    clockid_t cid;
    getcpuclockid(cid);
    ERRNO_EX(clock_gettime(cid, &ts) != 0, PthreadFailure, "failed clock_gettime");
    return ts.tv_sec + ts.tv_nsec / static_cast<double>(1000 * 1000 * 1000.0);
}

/*
uint64_t Thread::getCPUTimeInJiffies(pid_t tid) {
  char procFilename[256];
  char buffer[1024];

  pid_t pid=getpid();

  sprintf(procFilename,"/proc/%d/task/%d/stat",pid,tid);
  int fd, num_read;
  fd=open(procFilename, O_RDONLY, 0);
  if (fd<0) {
    THROW(PthreadFailure,"failure to retrieve thread time for thread " << tid << " -- attempting to
read file '" << procFilename << "'");
  }
  num_read=read(fd, buffer, 1023);
  close(fd);
  buffer[num_read]='\0';

  char* ptrUsr=strrchr(buffer, ')') + 1 ;
  for(int i = 3; i != 14; ++i) {
    ptrUsr = strchr(ptrUsr+1, ' ');
  }
  ptrUsr++;
  long jiffies_user = atol(ptrUsr);
  long jiffies_sys = atol(strchr(ptrUsr,' ') + 1);

  return static_cast<uint64_t>(jiffies_user+jiffies_sys);
}
*/

unsigned Thread::getNCurrentCandidateCPUs(pid_t tid)
{
    return ::getNCurrentCandidateCPUs(tid);
}

std::vector<int32_t> Thread::getCurrentCandidateCPUs(pid_t tid)
{
    return ::getCurrentCandidateCPUs(tid);
}

void Thread::setCPU(const int32_t cpu, pid_t tid)
{
    ::setCPU(cpu, tid);
}

void Thread::addCPU(const int32_t cpu, pid_t tid)
{
    ::addCPU(cpu, tid);
}

void Thread::useAllAvailableCPUs(pid_t tid)
{
    ::useAllAvailableCPUs(tid);
}

Thread::~Thread(void) {}
