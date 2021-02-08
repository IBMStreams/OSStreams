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

/*
 * Support code for calling toolkit Java native functions from SPL via
 * generated C++ glue code that uses the usual C++ native function mechanisms.
 *
 * The C++ glue code interface has been split into an internal class and
 * a public API so that most of the implementation details are internal.
 */

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Function/SPLJavaFunction.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Utility/JNIUtils.h>
#include <UTILS/DistilleryException.h>
#include <boost/filesystem/path.hpp>
#include <unicode/unistr.h>

using namespace std;
using namespace SPL::JNI;
namespace bf = boost::filesystem;

// Avoid type-punned pointer alias warnings, eg from optimised build
union EnvUnion
{
    JNIEnv* envPtr;
    void* rawPtr;
};
#define env_ (envu_.envPtr)
#define rawEnv (envu_.rawPtr)

/*
 * This internal class supports loading and calling a Java native function.
 *
 * The constructor must be called for each thread that calls the function.
 * Once the constructor has been called, the thread remains attached to
 * the JVM, and any required memory remains allocated, for the duration
 * of the current process. The thread is attached as a daemon so that the
 * JVM will shutdown without waiting for the thread to exit.
 */
namespace SPL {
class SPLJavaFunctionImpl
{
  public:
    // Constructor that loads the class containing the Java native function.
    SPLJavaFunctionImpl(const char* toolkitName,
                        const char* toolkitVers,
                        const char* adlFileName,
                        const char* funcClassLibRelPath,
                        const char* funcClassName,
                        const char* funcMethodName,
                        const char* funcSignature,
                        const char* SPLNamespace,
                        const char* prototype,
                        jclass* classGlobalRefPtr,
                        jmethodID* midPtr)
    {
        static string const loaderClassName =
          "com/ibm/streams/operator/internal/toolkits/ToolkitManagement";
        static string const loaderMethodName = "loadToolkitClass";
        static string const loaderSignature =
          "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Class;";
        SPLNamespace_ = SPLNamespace;
        prototype_ = prototype;
        env_ = NULL;
        jvm_ = SPL::ProcessingElement::pe().getJavaVM();
        if (jvm_ == NULL) {
            throwError("no JVM");
        }
        if (jvm_->AttachCurrentThreadAsDaemon(&rawEnv, NULL) != JNI_OK) {
            throwError("AttachCurrentThreadAsDaemon failed on function load");
        }
        if (env_->ExceptionCheck()) {
            throwError("pending Java exception before call on function load");
        }
        SPL::JNI::JClass loaderJClass(env_, loaderClassName.c_str());
        jmethodID loaderMid =
          loaderJClass.getStaticMethodID(loaderMethodName.c_str(), loaderSignature.c_str());
        jclass loaderClass = loaderJClass.get();

        string toolkitPath = SPL::ProcessingElement::pe().getToolkitDirectory(toolkitName);
        UnicodeString us = icu::UnicodeString(toolkitPath.c_str(), "UTF-8");
        jstring toolkitPathString = env_->NewString((jchar*)us.getBuffer(), (jsize)us.length());
        if (env_->ExceptionCheck()) {
            throwError("failed to create string '" + (string)toolkitPath + "'");
        }

        us = icu::UnicodeString(funcClassLibRelPath, "UTF-8");
        jstring funcClassLibRelPathString =
          env_->NewString((jchar*)us.getBuffer(), (jsize)us.length());
        if (env_->ExceptionCheck()) {
            throwError("failed to create string '" + (string)funcClassLibRelPath + "'");
        }

        us = icu::UnicodeString(funcClassName, "UTF-8");
        jstring funcClassNameString = env_->NewString((jchar*)us.getBuffer(), (jsize)us.length());
        if (env_->ExceptionCheck()) {
            throwError("failed to create string '" + (string)funcClassName + "'");
        }

        jclass funcClass =
          (jclass)env_->CallStaticObjectMethod(loaderClass, loaderMid, toolkitPathString,
                                               funcClassLibRelPathString, funcClassNameString);

        if (env_->ExceptionCheck()) {
            throwError("failed to load class '" + (string)funcClassName + "' from '" +
                       funcClassLibRelPath + "'");
        }
        *midPtr = env_->GetStaticMethodID(funcClass, funcMethodName, funcSignature);
        if (env_->ExceptionCheck()) {
            throwError("method '" + (string)funcMethodName + "' with signature '" +
                       (string)funcSignature + "' not found");
        }
        *classGlobalRefPtr = (jclass)env_->NewGlobalRef((jobject)funcClass);
        if (*classGlobalRefPtr == NULL) {
            throwError("failed to get global reference for '" + (string)funcClassName + "'");
        }
        env_->DeleteLocalRef(loaderClass);
        env_->DeleteLocalRef(toolkitPathString);
        env_->DeleteLocalRef(funcClassLibRelPathString);
        env_->DeleteLocalRef(funcClassNameString);
        env_->DeleteLocalRef(funcClass);
    }

    // Get the JNI pointer for calling the Java native function.
    JNIEnv* getJNIEnv()
    {
        env_ = NULL;
        if (jvm_->AttachCurrentThreadAsDaemon(&rawEnv, NULL) != JNI_OK) {
            throwError("AttachCurrentThreadAsDaemon failed");
        }
        if (env_->ExceptionCheck()) {
            throwError("pending Java exception before call");
        }
        return env_;
    }

    // Throw an exception for an error calling the Java native function.
    void throwCallError() { throwError("Java native function call caused Java exception"); }

    // Throw an exception for an error creating an object or array.
    void throwCreateError() { throwError("failed to create object or array"); }

  private:
    JavaVM* jvm_;
    EnvUnion envu_;
    const char* SPLNamespace_;
    const char* prototype_;

    // Throw an exception.
    void throwError(std::string const& msg)
    {
        if (env_ != NULL) {
            env_->ExceptionDescribe();
            env_->ExceptionClear();
        }
        THROW(SPLRuntime, SPL_APPLICATION_RUNTIME_JAVAFUNC_ERROR((string)prototype_,
                                                                 (string)SPLNamespace_, msg));
    }
};
} // namespace SPL

/*
 * The SPLJavaFunction class provides the public API used by the generated
 * C++ glue code to load and call a Java native function.
 *
 * By using a class with static methods, we have only one copy of the code
 * that is declared in the header file included by all the C++ glue code.
 */

// Load the class containing the Java native function.
void* SPL::SPLJavaFunction::loadFunction(const char* toolkitName,
                                         const char* toolkitVers,
                                         const char* adlFileName,
                                         const char* funcClassLibRelPath,
                                         const char* funcClassName,
                                         const char* funcMethodName,
                                         const char* funcSignature,
                                         const char* SPLNamespace,
                                         const char* prototype,
                                         jclass* classGlobalRefPtr,
                                         jmethodID* midPtr)
{
    return reinterpret_cast<void*>(new SPLJavaFunctionImpl(
      toolkitName, toolkitVers, adlFileName, funcClassLibRelPath, funcClassName, funcMethodName,
      funcSignature, SPLNamespace, prototype, classGlobalRefPtr, midPtr));
}

// Get the JNI pointer for calling the Java native function.
JNIEnv* SPL::SPLJavaFunction::getJNIEnv(void* func)
{
    return (reinterpret_cast<SPLJavaFunctionImpl*>(func))->getJNIEnv();
}

// Throw an exception for an error calling the Java native function.
void SPL::SPLJavaFunction::throwCallError(void* func)
{
    return (reinterpret_cast<SPLJavaFunctionImpl*>(func))->throwCallError();
}

// Throw an exception for an error creating an object or array.
void SPL::SPLJavaFunction::throwCreateError(void* func)
{
    return (reinterpret_cast<SPLJavaFunctionImpl*>(func))->throwCreateError();
}
