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

#include <Runtime/ProcessingElement/JavaSPLRuntime.h>
#include <SAM/SAMHelperFunctions.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/ProcessingElement/PEForwardDeclarations.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/JNIUtils.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <TRC/DistilleryDebug.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <cassert>
#include <dlfcn.h>
#include <jni.h>
#include <memory>

using namespace SPL;
using namespace std;
namespace bf = boost::filesystem;

// defined in <SPL/Runtime/Function/UtilFunctions.h>
namespace SPL {
namespace Functions {
namespace Utility {
SPL::rstring getConfiguredHostName();
}
}
}

JavaSPLRuntime::JavaSPLRuntime(PEImpl& pe, SPL::JNI::JVMFactory& factory)
  : pe_(pe)
  , jvmFactory_(factory)
{
    //    jvmFactory_.addListener(this);
}

void JavaSPLRuntime::setupJVM()
{
    std::tr1::shared_ptr<SPL::JNI::JNISPLRuntimeBridge> bridge(
      new SPL::JNI::JNISPLRuntimeBridge(jvmFactory_.getJVM()));
    startSPLJavaRuntime(bridge.get());

    setStarted(bridge); // The java runtime has started
}

void JavaSPLRuntime::startSPLJavaRuntime(SPL::JNI::JNISPLRuntimeBridge* bridge)
{
    assert(bridge != NULL);

    // Setup additional SPL Java runtime parameters
    vector<string> params;
    params.resize(2); // pass two parameters
    string standalone(pe_.isStandalone() ? "true" : "false");
    params.at(PEX_STANDALONE) = standalone;
    if (!pe_.isStandalone()) {
        params.at(PEX_CONFIGURED_HOST_NAME) = SPL::Functions::Utility::getConfiguredHostName();
    }

    bridge->logLevelUpdate(Distillery::debug::logger_level);
    bridge->traceLevelUpdate(Distillery::debug::app_trace_level);
    bridge->startSPLJavaRuntime(Streams::toString(*(pe_.getModel())), &params);
}

void JavaSPLRuntime::shutdown(bool finalNotice)
{
    if (isStarted()) {
        APPTRC(L_DEBUG, "Java runtime shutdown", SPL_PE_DBG);
        bridge_->shutdownSPLJavaRuntime(finalNotice);

        // TODO: This reset ensures that all future calls to the JavaSPLRuntime will be
        // benign. At the moment, the finalNotice flag does nothing. In the future, if it
        // does indeed indicate "request" versus "final", then we should only do this reset
        // on "final", which is when finalNotice is true.
        bridge_.reset();
    } else {
        APPTRC(L_DEBUG, "Java runtime not started; shutdownSPLJavaRuntime ignored", SPL_PE_DBG);
    }
}

void JavaSPLRuntime::logLevelUpdate(int level)
{
    if (isStarted()) {
        bridge_->logLevelUpdate(level);
    } else {
        APPTRC(L_DEBUG, "Java runtime not started yet; update ignored", SPL_PE_DBG);
    }
}

void JavaSPLRuntime::traceLevelUpdate(int level)
{
    if (isStarted()) {
        bridge_->traceLevelUpdate(level);
    } else {
        APPTRC(L_DEBUG, "Java runtime not started yet; update ignored", SPL_PE_DBG);
    }
}

void JavaSPLRuntime::visualizerUpdate(bool isVisualizing)
{
    if (isStarted()) {
        bridge_->visualizerUpdate(isVisualizing);
    } else {
        APPTRC(L_DEBUG, "Java runtime not started yet; update ignored", SPL_PE_DBG);
    }
}

void JavaSPLRuntime::connectJavaOperators()
{
    if (isStarted()) {
        bridge_->connectOperators();
    } else {
        APPTRC(L_DEBUG,
               "Java runtime not started; connectJavaOperators ignored.  This probably means the "
               "PE does not contain any java operators so the call is not needed.",
               SPL_PE_DBG);
    }
}

void JavaSPLRuntime::addView(int operatorIndex, int outputPortIndex, ViewProperties* properties)
{
    if (isStarted()) {
        bridge_->addView(operatorIndex, outputPortIndex, properties);
    }
}

void JavaSPLRuntime::deleteView(int operatorIndex, int outputPortIndex, ViewProperties* properties)
{
    if (isStarted()) {
        bridge_->deleteView(operatorIndex, outputPortIndex, properties);
    }
}
