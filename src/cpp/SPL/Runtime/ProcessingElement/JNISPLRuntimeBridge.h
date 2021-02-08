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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_JNISPLRUNTIMEBRIDGE_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_JNISPLRUNTIMEBRIDGE_H

#include <SPL/Runtime/Utility/JNIUtils.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
class ViewProperties;

namespace JNI {

/**
 * Class which interfaces with the Java side of the JavaSPLRuntime.
 *
 * Loads the @c JNIBridge Java class when initialized.
 */
class DLL_PUBLIC JNISPLRuntimeBridge
{
  public:
    /**
     * Construct an instance and initialize the Java side of the bridge.
     */
    JNISPLRuntimeBridge(JavaVM* vm);

    ~JNISPLRuntimeBridge();

    /**
     * Starts the Java runtime bridge class.
     * @param level The log level.
     */
    void startSPLJavaRuntime(std::string const& model, std::vector<std::string> const* params);

    /**
     * Shuts down the Java runtime bridge class.
     *
     * This method releases the Java bridge.  Subsequent invocations of Java
     * bridge methods will SEGV.
     *
     * @param finalNotice  false when the shutdown of the PE is being
     *  requested prior to the operators being shutdown; otherwise true.
     */
    void shutdownSPLJavaRuntime(bool finalNotice);

    /**
     * Notifies the Java runtime on log level changes.
     * @param level The log level.
     */
    void logLevelUpdate(int level);

    /**
     * Notifies the Java runtime on trace level changes.
     * @param level The trace level.
     */
    void traceLevelUpdate(int level);

    /**
     * Notifies the Java runtime on visualization changes.
     * @param isVisualizing
     */
    void visualizerUpdate(bool isVisualizing);

    /**
     * Connect Java operators, in cases where the Java operators connect
     * directly to each other without use of JNI.
     */
    void connectOperators();

    void addView(int operatorIndex, int outputPortIndex, ViewProperties* propertie);
    void deleteView(int operatorIndex, int outputPortIndex, ViewProperties* propertie);

  private:
    /*
     * Get the latest Java exception, if any.
     * @param rethrow  if true, then map and rethrow as a C++ exception;
     * otherwise just log the exception.
     */
    void processException(JNIEnv* env, bool rethrow = true);

    void releaseJavaBridge(JNIEnv* env)
    {
        env->DeleteGlobalRef(bridgeClass_);
        bridgeClass_ = NULL;
    }

    JavaVM* jvm_;

    // Global reference to the JNIBridge java class
    jclass bridgeClass_;

    // Java JNIBridge methods
    jmethodID setAppTrcLevel_;
    jmethodID setAppLogLevel_;
    jmethodID setVisualizing_;
    jmethodID startSPLJavaRuntime_;
    jmethodID shutdownSPLJavaRuntime_;
    jmethodID connectOperators_;
    jmethodID addView_;
    jmethodID deleteView_;
};
}
} // end SPL::JNI

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_JNISPLRUNTIMEBRIDGE_H */
