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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_JAVASPLRUNTIME_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_JAVASPLRUNTIME_H

#include <SPL/Runtime/Utility/JNIUtils.h>

#include <Runtime/ProcessingElement/JNISPLRuntimeBridge.h>
#include <jni.h>
#include <tr1/memory>

namespace SPL {
class PEImpl;
class ViewProperties;

/**
 * The Java SPL language runtime, responsible for the execution of the
 * Java side of the SPL runtime.
 */
class JavaSPLRuntime : public SPL::JNI::JVMListener
{
  public:
    /**
     * Constructor.
     *
     * @param pe      The PE implementation
     * @param factory The JVM factory.
     */
    JavaSPLRuntime(PEImpl& pe, SPL::JNI::JVMFactory& factory);

    JavaVM* getJavaVM() const { return jvmFactory_.getJVM(); }

    bool isStarted() const { return (bridge_.get() != NULL); }

    /**
     * Shut down this JavaSPLRuntime.
     */
    void shutdown(bool /* TODO finalNotice NOT USED */);

    /**
     * JVMListener implementation, gets invoked by the JVMFactory after
     * loading the JVM.
     */
    virtual void setupJVM();

    void logLevelUpdate(int level);
    void traceLevelUpdate(int level);
    void visualizerUpdate(bool isVisualizing);
    void connectJavaOperators();
    void addView(int operatorIndex, int outputPortIndex, ViewProperties* properties);
    void deleteView(int operatorIndex, int outputPortIndex, ViewProperties* properties);

  private:
    // Indexes where extra parameters are passed to the SPL Java runtime
    typedef enum PEXtra_t
    {
        PEX_STANDALONE = 0,      // PE is standalone
        PEX_CONFIGURED_HOST_NAME // configured host name
    } PEXtra;

    // Start the Java runtime Java bridge
    void startSPLJavaRuntime(SPL::JNI::JNISPLRuntimeBridge* bridge);

    void setStarted(std::tr1::shared_ptr<SPL::JNI::JNISPLRuntimeBridge> bridge)
    {
        bridge_ = bridge;
    }

    PEImpl& pe_;
    SPL::JNI::JVMFactory& jvmFactory_;

    // Bridge to the JNI interface
    std::tr1::shared_ptr<SPL::JNI::JNISPLRuntimeBridge> bridge_; // TODO use unique_ptr
};
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_JAVASPLRUNTIME_H */
