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

#include <SPL/Runtime/ProcessingElement/PECleaners.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace SPL;
using namespace Distillery;

void AutoPeCleaner::initialize()
{
    AutoMutex am(mutex_);
    if (status_ != NONE) {
        return;
    }
    initializeImpl();
    status_ = INIT;
}
void AutoPeCleaner::prepareToFinalize()
{
    AutoMutex am(mutex_);
    if (status_ != INIT) {
        return;
    }
    prepareToFinalizeImpl();
    status_ = PREP;
}
void AutoPeCleaner::finalize()
{
    prepareToFinalize();
    AutoMutex am(mutex_);
    if (status_ != PREP) {
        return;
    }
    finalizeImpl();
    status_ = FINL;
}
void AutoPeCleaner::finalize_nothrow()
{
    try {
        finalize();
    } catch (DistilleryException const& e) {
        APPTRC(L_ERROR, SPL_RUNTIME_EXCEPTION_RESOURCE_CLEANUP(e.getExplanation()), SPL_PE_DBG);
    } catch (...) {
        APPTRC(L_ERROR, SPL_RUNTIME_UNKNOWN_EXCEPTION_RESOURCE_CLEANUP, SPL_PE_DBG);
    }
}
AutoPeCleaner::~AutoPeCleaner()
{
    finalize_nothrow();
}

void AutoDebugServices::initializeImpl()
{
    pe_.runDebugServices();
}
void AutoDebugServices::prepareToFinalizeImpl()
{
    pe_.prepareDebugServicesForTermination();
}
void AutoDebugServices::finalizeImpl()
{
    pe_.terminateDebugServices();
}

void AutoPePorts::initializeImpl()
{
    // Open input/output ports
    APPTRC(L_DEBUG, "Opening ports...", SPL_PE_DBG);
    pe_.openPorts();
    APPTRC(L_INFO, "Opened all ports", SPL_PE_DBG);
}
void AutoPePorts::prepareToFinalize()
{
    // Possible timing hole
    if (status_ == NONE) {
        // We are still stuck trying to initialize; if we lock, we will hang
        APPTRC(L_DEBUG, "Shutting down PE ports while still initializing", SPL_PE_DBG);
        prepareToFinalizeImpl();
        status_ = PREP;
        return;
    }

    // Normal case
    AutoMutex am(mutex_);
    if (status_ != INIT) {
        return;
    }
    prepareToFinalizeImpl();
    status_ = PREP;
}
void AutoPePorts::prepareToFinalizeImpl()
{
    pe_.preparePortsForTermination();
}
void AutoPePorts::finalizeImpl()
{
    // Open input/output ports
    APPTRC(L_DEBUG, "Closing ports...", SPL_PE_DBG);
    pe_.closePorts(); // in subclass
    APPTRC(L_INFO, "Closed all ports", SPL_PE_DBG);
}
void AutoPePorts::finalize_nothrow()
{
    // We have been initialized properly, just do the right thing
    if (status_ != NONE) {
        AutoPeCleaner::finalize_nothrow();
        return;
    }

    // We haven't been initialized, but we STILL have to cleanup the ports
    AutoMutex am(mutex_);
    try {
        finalizeImpl();
    } catch (DistilleryException const& e) {
        APPTRC(L_ERROR, SPL_RUNTIME_EXCEPTION_RESOURCE_CLEANUP(e.getExplanation()), SPL_PE_DBG);
    } catch (...) {
        APPTRC(L_ERROR, SPL_RUNTIME_UNKNOWN_EXCEPTION_RESOURCE_CLEANUP, SPL_PE_DBG);
    }

    // We don't want to get here again
    status_ = FINL;
}

void AutoCheckpointServices::initializeImpl()
{
    pe_.runCheckpointService();
}
void AutoCheckpointServices::finalizeImpl()
{
    pe_.terminateCheckpointService();
}

void AutoMetricsIntervalTimer::initializeImpl()
{
    pe_.createMetricsIntervalTimer();
}
void AutoMetricsIntervalTimer::finalizeImpl()
{
    pe_.terminateMetricsIntervalTimer();
}

void AutoCreateOperators::initializeImpl()
{
    pe_.createOperators();
}
void AutoCreateOperators::prepareToFinalizeImpl()
{
    pe_.prepareOperatorsForTermination();
}
void AutoCreateOperators::finalizeImpl()
{
    pe_.deleteOperators();
}
