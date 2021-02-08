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

#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Toolkit/Delay.h>
#include <UTILS/SupportFunctions.h>
#include <string.h>
#include <string>
#include <time.h>

#ifdef __powerpc__
inline static uint64_t getRTDSC()
{
    return Distillery::rdtsc();
}
static uint64_t nanoMultiplier =
  static_cast<uint64_t>(Distillery::SystemStatics::getTicksPerNanosec());

#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
#ifndef rdtsc
#define rdtsc(low, high) __asm__ __volatile__("rdtsc" : "=a"(low), "=d"(high))
#endif

inline uint64_t getRTDSC(void)
{
    uint32_t v1, v2;
    rdtsc(v1, v2);

    return (static_cast<uint64_t>(v2) << 32) | v1;
}

static uint64_t nanoMultiplier =
  static_cast<uint64_t>(Distillery::SystemStatics::getCPUFrequency() / 1000.0f);

#else
#error "Need to implement getRTDSC() and nanoMultiplier for new architecture"
#endif

inline static void shortDelay(uint64_t nanoSecondsDelay)
{
    if (nanoSecondsDelay < 50) {
        return;
    }
    uint64_t start = getRTDSC();
    uint64_t end = start + nanoSecondsDelay * nanoMultiplier - 100;
    do {
    } while (getRTDSC() < end);
}

static uint64_t getSwitchover()
{
    struct timespec resolution;
    clock_getres(CLOCK_REALTIME, &resolution);
    return resolution.tv_nsec * 2 * 1000;
}

static uint64_t preciseSwitchoverInNanos = getSwitchover();

void SPL::preciseBlockUntilShutdownRequest(double timeInSeconds, ProcessingElement& pe)
{

    struct timespec req;
    req.tv_sec = static_cast<time_t>(timeInSeconds);
    req.tv_nsec = static_cast<uint64_t>((timeInSeconds - req.tv_sec) / 1e-9);
    if (req.tv_sec != 0 || req.tv_nsec > preciseSwitchoverInNanos) {
        pe.blockUntilShutdownRequest(req);
    } else {
        shortDelay(req.tv_nsec);
    }
}
