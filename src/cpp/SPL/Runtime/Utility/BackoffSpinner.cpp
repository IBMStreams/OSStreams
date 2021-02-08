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

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Utility/BackoffSpinner.h>
#include <dst-config.h>

#ifndef AVOID_SCHED_YIELD
#define AVOID_SCHED_YIELD 0
#endif

bool BackoffSpinner::doYield = !AVOID_SCHED_YIELD;

void BackoffSpinner::setYieldBehaviour(const AvoidYieldBehaviour b)
{
    switch (b) {
        case No:
            doYield = true;
            break;
        case Yes:
            doYield = false;
            break;
        case Auto:
            doYield = !AVOID_SCHED_YIELD;
            break;
    }
    if (doYield) {
        APPTRC(L_INFO, "BackoffSpinner: use pthread_yield()", SPL_PE_DBG);
    } else {
        APPTRC(L_INFO, "BackoffSpinner: avoid pthread_yield()", SPL_PE_DBG);
    }
}

BackoffSpinner::AvoidYieldBehaviour BackoffSpinner::getYieldBehaviour()
{
    return (doYield) ? No : Yes;
}
