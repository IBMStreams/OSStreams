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

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Function/BuiltinUtilFunctions.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Utility/JNIUtils.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <UTILS/SupportFunctions.h>

#include <algorithm>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/scoped_array.hpp>
#include <dlfcn.h>
#include <sstream>

using namespace std;
using namespace SPL;
using namespace SPL::JNI;
namespace bf = boost::filesystem;

template class Singleton<JVMFactory>;

static string getJavaClassPath(const string& streamsInstallDir)
{
    if (streamsInstallDir.empty()) {
        THROW_STRING(
          SPLRuntime,
          SPL_RUNTIME_EXCEPTION_RESOURCE_CLEANUP(
            "Can't build Java class path: STREAMS_INSTALL environment variable has not been set."));
    }
    bf::path result;
    result = streamsInstallDir;
    result /= "system/impl/lib";
    result /= "com.ibm.streams.javart.bootstrap.jar";
    string path = result.string();
    /*
        result = streamsInstallDir;
        result /= "system/impl/lib";
        result /= "com.ibm.streams.spl.internal.impl.jar";
        path += ":";
        path += result.string();
    */
    return path;
}

// Get the path to the directory containing libjvm.so
static bf::path getJVMLibDirPath()
{
    bf::path result;
    // This code has been evolving over time.
    // In V3.0 we needed to capture the jvm location in the ADL.  In V3.1 we added an internal
    // java install to our streams install and if JAVA_HOME isn't set we use that JVM.
    // For portability we don't capture the setting of JAVA_HOME in the ADL if it points
    // to our install.  We simply find the install here and compute the java path if needed. In
    // v6.0, we removed the internal JVM to use a global JVM installed in /opt. Therefore,
    // we simply rely on the container's JAVA_HOME definition.
    char* javaHome = getenv("JAVA_HOME");
    if (NULL != javaHome) {
        result = javaHome;
    } else {
        THROW_STRING(SPLRuntime, SPL_RUNTIME_JAVA_NOT_FOUND);
    }

    APPTRC(L_INFO, "Using Java VM from " << result, SPL_JAVART_DBG);
    result /= "jre/lib";

    bf::directory_iterator dir_end;
    for (bf::directory_iterator iArch(result); iArch != dir_end; iArch++) {
        bf::path arch = *iArch;
        if (bf::is_directory(arch)) {
            for (bf::directory_iterator iVmType(arch); iVmType != dir_end; iVmType++) {
                bf::path vmType = *iVmType;
                if (vmType.leaf() == "j9vm" || vmType.leaf() == "server") {
                    result = vmType;
                    goto archLoopDone;
                }
            }
        }
    }
archLoopDone:
    return result;
}

JVMEnv::JVMEnv(bool doDetach)
  : env_(NULL)
  , detachOnEnd_(doDetach)
{
    javaVM_ = JVMFactory::instance().getJVM();
    attach();
}

JVMEnv::JVMEnv(JavaVM* vm, bool doDetach)
  : env_(NULL)
  , javaVM_(vm)
  , detachOnEnd_(doDetach)
{
    attach();
}

JVMEnv::~JVMEnv()
{
    if (detachOnEnd_) {
        detach();
    }
}

JavaVM* JVMEnv::getJVM() const
{
    return javaVM_;
}

bool JVMEnv::useJvmCache()
{
    if (!isIBMJVM()) {
        return false;
    }
    std::string useCache =
      Distillery::tolowercase(Distillery::get_environment_variable("STREAMS_JVM_CACHE", "true"));
    return (useCache.compare("false") != 0);
}

bool JVMEnv::isIBMJVM()
{
    std::string javaHome = Distillery::get_environment_variable("JAVA_HOME", "");
    std::string streamsInstall = Distillery::get_environment_variable("STREAMS_INSTALL", "");
    return ((Distillery::tolowercase(javaHome).find("ibm") != std::string::npos) ||
            (streamsInstall.size() > 0 && (javaHome.find(streamsInstall) == 0)));
}

JVMFactory::JVMFactory(string const& streamsInstallPath, vector<string>* const vmargs)
  : Singleton<JVMFactory>(*this)
  , streamsInstallPath_(streamsInstallPath)
  , javaVMArgs_(vmargs)
  , javaVM_(NULL)
  , isJVMStarted_(false)
{}

JavaVM* JVMFactory::getJVM()
{
    if (javaVM_ == NULL) {
        APPTRC(L_INFO, "JVM not found", SPL_JAVAOP_DBG);

#if 0
        //If PE is shutting down then the JVM should not be constructed
        if(ProcessingElement::pe().getShutdownRequested()) {
            APPTRC(L_INFO, "JVM not constructed since PE is shutting down", SPL_JAVAOP_DBG);
            return NULL;
        }
#endif

        Distillery::AutoMutex am(javaMutex_);
        if (javaVM_ != NULL) {
            return javaVM_;
        }
        APPTRC(L_INFO, "Launching JVM", SPL_JAVAOP_DBG);
        javaVM_ = launchJVM();
        if (javaVM_ == NULL) {
            THROW(SPL::SPLRuntimeJNI, "Failed to start a Java VM");
        }
        isJVMStarted_ = true;
        APPTRC(L_INFO, "Notifying JVMFactory listeners", SPL_JAVAOP_DBG);
        notifyListeners();
    }
    return javaVM_;
}

/** Create Java VM */
JavaVM* JVMFactory::launchJVM()
{
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_4;
    vm_args.nOptions = 5 + (javaVMArgs_ == NULL ? 0 : javaVMArgs_->size());
    vm_args.ignoreUnrecognized = JNI_TRUE;

    boost::scoped_array<JavaVMOption> options(new JavaVMOption[vm_args.nOptions]);
    JavaVMOption* pOption = options.get();
    std::string classPath =
      std::string("-Djava.class.path=") + getJavaClassPath(streamsInstallPath_);
    pOption->optionString = const_cast<char*>(classPath.c_str());
    ++pOption;
    // (ifsdb00022892)
    // Disable Java Attach api to stop PEs creating code in other PEs.
    // The Attach api may be enabled in the future once a secure
    // solution has been found.
    pOption->optionString = const_cast<char*>("-Dcom.ibm.tools.attach.enable=no");
    ++pOption;
    pOption->optionString = const_cast<char*>("-Djava.awt.headless=true");
    ++pOption;
    // Specify the correct log4j.config file
    pOption->optionString = const_cast<char*>(
      "-Dlog4j.configuration=com/ibm/streams/operator/internal/logtrace/log4j/log4j.properties");
    ++pOption;
    pOption->optionString = const_cast<char*>("-Xrs");
    ++pOption;

    if (javaVMArgs_) {
        for (uint32_t i = 0; i < javaVMArgs_->size(); i++) {
            const string& arg = javaVMArgs_->at(i);
            APPTRC(L_TRACE, "JVM arg: " << arg, SPL_JAVART_DBG);
            pOption->optionString = const_cast<char*>(arg.c_str());
            ++pOption;
        }
    }

    void* env = NULL;
    vm_args.options = options.get();
    jboolean startedJVM;
    JavaVM* jvm = createJavaVM(env, vm_args, startedJVM);
    return jvm;
}

JavaVM* JVMFactory::createJavaVM(void*& env, JavaVMInitArgs& vm_args, jboolean& startedJVM)
{
    // Load the shared object file (DLL).
    bf::path javaLibPath = getJVMLibDirPath() /= "libjvm.so";
    void* javaLibHandle = dlopen(javaLibPath.string().c_str(), RTLD_LAZY | RTLD_DEEPBIND);
    if (NULL == javaLibHandle) {
        std::stringstream ss;
        ss << "dlopen(" << javaLibPath.string().c_str() << ")";
        char* lastError = dlerror();
        THROW(SPL::SPLRuntimeJNI,
              SPL_RUNTIME_FAILED_LOAD_JVM(ss.str(), ((lastError != NULL) ? lastError : "NULL")));
    }
    // RTC20738
    // Load libawt.so and set its symbol scoping to be self-contained
    // so the the JNI_Onload from librum.so is not selected as
    // the JNI_OnLoad for libawt.so
    bf::path javaAWTPath = getJVMLibDirPath().parent_path() /= "libawt.so";
    dlopen(javaAWTPath.string().c_str(), RTLD_LAZY | RTLD_DEEPBIND);

    typedef jint CreateJVMFun(JavaVM**, void**, void*);
    CreateJVMFun* createJVM = (CreateJVMFun*)dlsym(javaLibHandle, "JNI_CreateJavaVM");
    if (NULL == createJVM) {
        char* lastError = dlerror();
        THROW(SPL::SPLRuntimeJNI,
              SPL_RUNTIME_FAILED_LOAD_JVM("dlsym(\"JNI_CreateJavaVM\")",
                                          ((lastError != NULL) ? lastError : "NULL")));
    }
    typedef jint GetCreatedJVMsFun(JavaVM**, jsize, jsize*);
    GetCreatedJVMsFun* getCreatedJVMs =
      (GetCreatedJVMsFun*)dlsym(javaLibHandle, "JNI_GetCreatedJavaVMs");
    if (NULL == getCreatedJVMs) {
        char* lastError = dlerror();
        THROW(SPL::SPLRuntimeJNI,
              SPL_RUNTIME_FAILED_LOAD_JVM("dlsym(\"JNI_GetCreatedJavaVMs\")",
                                          ((lastError != NULL) ? lastError : "NULL")));
    }

    // Create the JVM.
    JavaVM* jvm = NULL;
    jint cresult = (*createJVM)(&jvm, &env, &vm_args);
    if (JNI_OK != cresult) {
        jsize vmCount;
        jint gresult = (*getCreatedJVMs)(&jvm, 1, &vmCount);
        if (JNI_OK != gresult) {
            THROW(SPL::SPLRuntimeJNI, SPL_RUNTIME_FAILED_START_JVM((cresult), (gresult)));
        }
        startedJVM = false;
        APPTRC(L_INFO, "Found Java VM", SPL_JAVART_DBG);
    } else {
        startedJVM = true;
        jvm->DetachCurrentThread();
        APPTRC(L_INFO, "Created Java VM", SPL_JAVART_DBG);
    }
    return jvm;
}

void JVMFactory::destroyJVM()
{
    Distillery::AutoMutex am(javaMutex_);

    if (!javaVM_) {
        return;
    }

    APPTRC(L_DEBUG, "Destroying Java VM", SPL_JAVART_DBG);
    jint res = javaVM_->DestroyJavaVM();
    javaVM_ = NULL;
    isJVMStarted_ = false;

    std::stringstream status;
    if (JNI_OK == res) {
        status << "cleanly";
    } else {
        status << "with error: " << res;
    }

    APPTRC(L_INFO, "Destroyed Java VM " << status.str(), SPL_JAVART_DBG);
}

void JVMFactory::addListener(JVMListener* l)
{
    Distillery::AutoMutex am(javaMutex_);

    // if java VM is already created then call the setup right away.
    if (javaVM_) {
        l->setupJVM();
    }
    listeners_.push_back(l);
}

bool JVMFactory::isJVMStarted() const
{
    return isJVMStarted_;
}

void JVMFactory::notifyListeners()
{
    for (size_t i = 0, iu = listeners_.size(); i < iu; ++i) {
        listeners_[i]->setupJVM();
    }
}

void JString::init(char const* const str)
{
    assert(env_ != NULL);
    jstring lref = JString::newUnicodeString(env_, str);
    js_ = (jstring)env_->NewGlobalRef(lref);
    env_->DeleteLocalRef(lref);
}

jobjectArray JObjectArray::init(char const* const* strArray, size_t size)
{
    if (strArray == NULL) {
        return NULL;
    }

    assert(env_ != NULL);

    jclass clazz = env_->FindClass("java/lang/String");
    JNI_CHECK_REF(env_, clazz, "java.lang.String");

    jobjectArray array = env_->NewObjectArray(size, clazz, NULL);
    JNI_CHECK_REF(env_, array, "java.lang.String[]");

    for (size_t i = 0; i < size; ++i) {
        jstring js = JString::newUnicodeString(env_, strArray[i]);
        env_->SetObjectArrayElement(array, i, js);
        env_->DeleteLocalRef(js);
    }
    return array;
}

jobjectArray JObjectArray::init(std::vector<std::string> const* vec)
{
    if (vec == NULL) {
        return NULL;
    }

    assert(env_ != NULL);

    jclass clazz = env_->FindClass("java/lang/String");
    JNI_CHECK_REF(env_, clazz, "java.lang.String");

    jobjectArray array = env_->NewObjectArray(vec->size(), clazz, NULL);
    JNI_CHECK_REF(env_, array, "java.lang.String[]");

    for (size_t i = 0; i < vec->size(); ++i) {
        jstring js = JString::newUnicodeString(env_, (*vec)[i].c_str());
        env_->SetObjectArrayElement(array, i, js);
        env_->DeleteLocalRef(js);
    }
    return array;
}

void JObjectArray::cleanup()
{
    assert(env_ != NULL);

    if (joArray_ != NULL) {
        env_->DeleteGlobalRef(joArray_);
        joArray_ = NULL;
    }
}

jobject JObjectArray::at(jsize index) const
{
    jobject jref = env_->GetObjectArrayElement(joArray_, index);
    if (env_->ExceptionCheck()) {
        std::basic_ostringstream<char> s;
        s << "JObjectArray.at(" << index << ")";
        JNI_THROW(env_, s.str());
    }
    return jref;
}

JClass::JClass(JNIEnv* env, const char* const name)
  : env_(env)
  , jclass_(NULL)
  , name_(name)
{
    const char* const bsName = "com/ibm/streams/javart/bootstrap/Bootstrap";
    jclass bsClass = env->FindClass(bsName);
    JNI_CHECK_REF(env, bsClass, bsName);

    const char* const fcmName = "findClass";
    const char* const fcmSig = "(Ljava/lang/String;)Ljava/lang/Class;";
    jmethodID fcm = env_->GetStaticMethodID(bsClass, fcmName, fcmSig);
    JNI_CHECK_METHOD_REF(env_, fcm, bsName, fcmName, fcmSig);

    APPTRC(L_TRACE, "Loading class: " << name, SPL_JAVART_DBG);

    jstring jname = JString::newUnicodeString(env_, name);
    jclass_ = (jclass)env_->CallStaticObjectMethod(bsClass, fcm, jname);
    JNI_CHECK_REF(env, jclass_, name);

    //    jclass_ = reinterpret_cast<jclass>(JGlobalRef::newRef(env, foundClass, true));
    // jclass_ = env->FindClass(name);
}

jmethodID JClass::getMethodID(const char* methodName, const char* signature) const
{
    jmethodID method = env_->GetMethodID(jclass_, methodName, signature);
    JNI_CHECK_METHOD_REF(env_, method, name_, methodName, signature);
    return method;
}

jmethodID JClass::getStaticMethodID(const char* methodName, const char* signature) const
{
    jmethodID method = env_->GetStaticMethodID(jclass_, methodName, signature);
    JNI_CHECK_METHOD_REF(env_, method, name_, methodName, signature);
    return method;
}

jobject JGlobalRef::newRef(JNIEnv* env, jobject localRef, bool deleteLocalRef)
{
    assert(env != NULL);
    if (localRef == NULL) {
        return NULL;
    }

    jobject ref = env->NewGlobalRef(localRef);
    if (ref == NULL) {
        jclass expClass = env->FindClass("java/lang/OutOfMemoryError");
        env->ThrowNew(expClass, "Could not create global reference.");
    }
    if (deleteLocalRef) {
        env->DeleteLocalRef(localRef);
    }
    return ref;
}
