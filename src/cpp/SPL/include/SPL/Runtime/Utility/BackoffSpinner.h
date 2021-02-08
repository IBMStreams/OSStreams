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

#ifndef SPL_BACKOFF_SPINNER
#define SPL_BACKOFF_SPINNER

#include <inttypes.h>
#include <pthread.h>
#include <time.h>
#include <SPL/Runtime/Utility/Visibility.h>

class DLL_PUBLIC BackoffSpinner
{
public:
    enum AvoidYieldBehaviour {Auto, No, Yes};

    BackoffSpinner() : state_(Pause), count_(1), yieldTime_(0) {}
    void wait() {
      switch (state_) {
        case Pause:
          if (count_ <= (uint32_t) loops) {
            pause(count_);
            count_ *= 2; // double
            return;
          }
          if (doYield) {
              yieldTime_ = time(NULL);
              state_ = Yield;
              pthread_yield();
          } else {
              state_ = Sleep;
          }
          return;
        case Yield:
          // Haven't been waiting too long...
          if ((time(NULL) - yieldTime_) < 2) {
            pthread_yield();
            return;
          }
          state_ = Sleep;
          // FALLTHROUGH
        case Sleep:
          // we have been waiting for a while - take a break of 1 ns (will use min sleep)
          timespec delay = {0, 1};
          nanosleep (&delay, NULL);
          break;
      }
    }
    static void pause(uint32_t count) {
        for (uint32_t i=0; i< count; i++) {
#if defined(__powerpc__)
            __asm__ __volatile__("or 30,30,30");
#elif !(defined (__i386__) ||  defined (__x86_64__))
            pthread_yield();
#else
            __asm__ __volatile__("pause;");
#endif
        }
    }

    /// Should we avoid calling pthread_yield?
    /// @param b desired behaviour
    static void setYieldBehaviour (const AvoidYieldBehaviour b);

    /// Return the yield behavior that has been set
    static AvoidYieldBehaviour getYieldBehaviour();

private:
    enum {Pause,Yield,Sleep} state_;
    uint32_t count_;
    time_t yieldTime_;
    static const uint32_t loops = 16;
    static bool doYield;
};
#endif /* SPL_BACKOFF_SPINNER */
