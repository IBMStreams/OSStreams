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

#ifndef SPL_RUNTIME_UTILITY_JNIUTILS_H
#define SPL_RUNTIME_UTILITY_JNIUTILS_H

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/RuntimeExceptionImpl.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <boost/atomic/atomic.hpp>

#include <cassert>
#include <jni.h>
#include <string>
#include <unicode/unistr.h>
#include <vector>

struct JavaVM_;
typedef struct JavaVM_ JavaVM;

/**
 * @file JNIUtils.h
 * @brief Definition of JNI utility classes.
 */

/**
 * JNI error handling.
 *
 * Note that SPLRuntimeJNIException is internal and should be caught and
 * wrapped by a public exception.
 */
#define JNI_THROW(env, where)                                                                      \
    {                                                                                              \
        env->ExceptionDescribe();                                                                  \
        env->ExceptionClear();                                                                     \
        std::string m__("A Java exception occurred during JNI invocation at: ");                   \
        m__.append(where);                                                                         \
        APPTRC(L_ERROR, m__, SPL_JAVAOP_DBG);                                                      \
        THROW_STRING(SPL::SPLRuntimeJNI, m__);                                                     \
    }

#define JNI_CHECK_REF(env, reference, refType)                                                     \
    {                                                                                              \
        if (reference == NULL || env->ExceptionCheck()) {                                          \
            env->ExceptionDescribe();                                                              \
            env->ExceptionClear();                                                                 \
            std::string m__("JNI invocation returned null reference to: ");                        \
            m__.append(refType);                                                                   \
            APPTRC(L_ERROR, m__, SPL_JAVAOP_DBG);                                                  \
            THROW_STRING(SPL::SPLRuntimeJNI, m__);                                                 \
        }                                                                                          \
    }

#define JNI_CHECK_METHOD_REF(env, reference, refType, methodName, signature)                       \
    {                                                                                              \
        if (reference == NULL || env->ExceptionCheck()) {                                          \
            env->ExceptionDescribe();                                                              \
            env->ExceptionClear();                                                                 \
            std::string m__("JNI invocation returned null reference to: ");                        \
            std::string t__(refType);                                                              \
            std::replace(t__.begin(), t__.end(), '/', '.');                                        \
            m__.append(t__).append(".").append(methodName).append(signature);                      \
            APPTRC(L_ERROR, m__, SPL_JAVAOP_DBG);                                                  \
            THROW_STRING(SPL::SPLRuntimeJNI, m__);                                                 \
        }                                                                                          \
    }

namespace SPL {
namespace JNI {

class JVMListener
{
  public:
    /**
     * This function is implemented by classes that want to perform
     * some setup when the JVM is started.
     * This function will be called when the JVMFactory's javaMutex is locked.
     */
    virtual void setupJVM() = 0;

    virtual ~JVMListener() {}
};

/**
 * Responsible for loading and starting the Java VM.
 */
class DLL_PUBLIC JVMFactory : public Singleton<JVMFactory>
{
  public:
    /**
     * Constructor.
     * @param streamsInstallDir The current Streams install path,
     *      used internally to build the Java classpath.
     * @param vmargs Additional Java VM invocation arguments.
     */
    JVMFactory(std::string const& streamsInstallPath,
               std::vector<std::string>* const vmargs = NULL);

    /**
     * Retrieve the JVM started by the current PE, or start one if needed.
     */
    JavaVM* getJVM();

    /**
     * Destroy the Java VM if one was started.
     */
    void destroyJVM();

    /**
     * Add the specified listener to this instance.
     *
     * Listeners are notified after a JVM is created and started.
     * @param l The listener.
     */
    void addListener(JVMListener* l);

    /**
     * Return true if a JVM was already created and started, otherwise false.
     */
    bool isJVMStarted() const;

    void setArguments(const std::vector<std::string>& vmargs) { javaVMArgs_ = &vmargs; }

  private:
    /**
     * Start the Java VM
     * @note caller must hold javaMutex_
     */
    JavaVM* launchJVM();

    /**
     * Load or get an already loaded Java VM.
     *
     * On return, the current thread is detached from the jvm.
     * @note caller must hold javaMutex_
     *
     * @param env        The JNI environment.
     * @param vm_args    The JVM initialization arguments.
     * @param startedJVM On return, this parameter is true if the JVM is
     *      created by this function call and false if the JVM had been
     *      created prior to this function call.
     * @return the JVM
     */
    JavaVM* createJavaVM(void*& env, JavaVMInitArgs& vm_args, jboolean& startedJVM);

    /** @note caller must hold javaMutex_ */
    void notifyListeners();

    std::string const& streamsInstallPath_;
    const std::vector<std::string>* javaVMArgs_;

    mutable Distillery::Mutex javaMutex_;
    JavaVM* javaVM_;
    boost::atomic<bool> isJVMStarted_;
    std::vector<JVMListener*> listeners_;
};

/**
 * Basic wrapper of a Java object which manages a JNI interface
 * pointer obtained after attaching to the JVM.
 *
 * A JVMEnv instance constructed as a stack object ensures that the execution
 * thread detaches from the JVM when the object goes out of scope.
 *
 * @note A JVMFactory must be instantiated before using a JVMEnv instance or
 * static function.
 *
 * @code
 * void foo(jobject bridge)
 *  {
 *      JVMEnv env;
 *      env->CallVoidMethod(bridge, _completeMethod);
 *      if (env->ExceptionCheck()) {
 *           // Env gets destructed during the stack unwind
 *           // and the thread gets detached
 *           JNI_THROW("....");
 *      }
 *  }
 *  @endcode
 */
class DLL_PUBLIC JVMEnv
{
  public:
    /**
     * Retrieve the JVM started by the current PE, or NULL if a JVM has
     * not been started.
     * @deprecated As of V4.0.1, replaced by JVMFactory::getJVM() .
     * */
    JavaVM* getJVM() const;

    /**
     * Attach the current thread and return the JNI pointer.
     *
     * Use this method when you don't want to detach from the current thread
     * after going out of scope.  For example:
     * @code
     *   JNIEnv* env = NULL;
     *   JVMEnv::getJNIEnv(&env)
     * @endcode
     *
     * @param env output parameter where the pointer to JNIEnv interface
     *      will be returned
     * @param jvm  the current JavaVM. If NULL, then JVMEnv::getJVM is
     *      invoked to get the current JVM.
     */
    static void getJNIEnv(JNIEnv** env, JavaVM* jvm = NULL)
    {
        if (jvm == NULL) {
            jvm = JVMFactory::instance().getJVM();
        }
        jvm->AttachCurrentThread(reinterpret_cast<void**>(env), NULL);
    }

    /**
     * Attach the current thread and return the JNI pointer.
     *
     * Use this method when you don't want to detach from the current thread
     * after going out of scope.  For example:
     * @code
     *   JNIEnv* env = NULL;
     *   JVMEnv::getJNIEnv(&env)
     * @endcode
     *
     * @param env output parameter where the pointer to JNIEnv interface
     *      will be returned
     * @param jvm  the current JavaVM. If NULL, then JVMEnv::getJVM is
     *      invoked to get the current JVM.
     */
    static void getDaemonJNIEnv(JNIEnv** env, JavaVM* jvm = NULL)
    {
        if (jvm == NULL) {
            jvm = JVMFactory::instance().getJVM();
        }
        jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(env), NULL);
    }

    /**
     * @brief Indicates whether the jvm should be created with shared classes
     * cache.
     *
     * This method returns false if not an IBM JVM, or if the
     * "STREAMS_JVM_CACHE" environment variable is defined and is equal to
     * "false" (case insensitive), otherwise it returns true.
     */
    static bool useJvmCache();

    /**
     * @brief Returns true if running an IBM JVM, otherwise false.
     *
     * This method returns true if the "JAVA_HOME" environment variable
     * contains the string "ibm" (case insensitive), or it starts with the
     * value of the "STREAMS_INSTALL" environment variable.
     */
    static bool isIBMJVM();

    /**
     * Retrieve the current JVM, construct an instance of JVMEnv and attach
     * the current thread.
     *
     * @param doDetach  if true, then automatically detach the current thread
     * on destruct
     */
    JVMEnv(bool doDetach = true);

    /**
     * Uses the specified JVM to construct an instance of JVMEnv
     * and attach the current thread.
     */
    JVMEnv(JavaVM* vm, bool doDetach = true);

    /** Get the current JNI environment */
    JNIEnv* get() const // never throws
    {
        return env_.envPtr;
    };

    JNIEnv* operator->() const // never throws
    {
        assert(env_.envPtr != 0);
        return env_.envPtr;
    }

    /** Detach the current thread from the JVM */
    void detach()
    {
        if (env_.rawPtr != NULL) {
            env_.rawPtr = NULL;
            javaVM_->DetachCurrentThread();
        }
    }

    /** Determine whether the current thread is attached to the JVM. */
    bool isAttached() const { return env_.rawPtr != NULL; }

    ~JVMEnv();

  private:
    /** Attach the current thread to the JVM */
    JNIEnv* attach()
    {
        javaVM_->AttachCurrentThread(&env_.rawPtr, NULL);
        return get();
    }

    /** Attach the current thread as a daemon thread to the JVM */
    JNIEnv* attachDaemon()
    {
        javaVM_->AttachCurrentThreadAsDaemon(&env_.rawPtr, NULL);
        return get();
    }

    // Avoid type-punned pointer alias warnings
    union EnvUnion
    {
        EnvUnion(void* p) { rawPtr = p; }
        JNIEnv* envPtr;
        void* rawPtr;
    };

    EnvUnion env_;
    JavaVM* javaVM_;
    bool detachOnEnd_;
};

/**
 * Scoped wrapper for building JNI jstring objects from C/C++ strings.
 *
 * Creates a JNI String and a global reference when the object is constructed.
 * The global reference is deleted when the object goes out of scope.
 */
class DLL_PUBLIC JString
{
  public:
    JString(JNIEnv* env, char const* const str)
      : env_(env)
      , js_(NULL)
    {
        init(str);
    }

    JString(JNIEnv* env, std::string const& s)
      : env_(env)
      , js_(NULL)
    {
        init(s.c_str());
    }

    /** Get the current string reference */
    jstring get() const // never throws
    {
        return js_;
    };

    jstring operator->() const // never throws
    {
        assert(js_ != NULL);
        return js_;
    }

    ~JString() { cleanup(); }

    static jstring newUnicodeString(JNIEnv* env, char const* const str)
    {
        UnicodeString us = icu::UnicodeString(str, "UTF-8");
        return env->NewString((jchar*)us.getBuffer(), (jsize)us.length());
    }

  private:
    void init(char const* const str);
    void cleanup()
    {
        assert(env_ != NULL);
        if (js_ != NULL) {
            env_->DeleteGlobalRef(js_);
        }
    }

    JNIEnv* env_;
    jstring js_;
};

/**
 * Scoped wrapper for converting a JNI modified UTF-8 string to a C string.
 */
class DLL_PUBLIC JStringUTFChars
{
  public:
    JStringUTFChars(JNIEnv* env, jstring s)
      : env_(env)
      , s_(s)
      , len_(env_->GetStringUTFLength(s))
      , cstr_(env_->GetStringUTFChars(s, NULL))
    {}

    /** Get the current string buffer */
    const char* c_str() const // never throws
    {
        return cstr_;
    };

    /** Get the current string buffer length */
    size_t len() const // never throws
    {
        return len_;
    };

    ~JStringUTFChars() { env_->ReleaseStringUTFChars(s_, cstr_); }

  private:
    JNIEnv* env_;
    jstring s_;
    jsize len_;
    const char* cstr_;
};

/**
 * Wrapper of a Java class or interface.
 */
class DLL_PUBLIC JClass
{
  public:
    /**
     * Constructs a wrapper around the specified class.
     *
     * @param env JNI environment
     * @param name The class or interface name as a JNI signature,
     *      for example @c java/lang/String
     * @param clazz The class JNI reference
     */
    JClass(JNIEnv* env, char const* const name, jclass const clazz)
      : env_(env)
      , jclass_(clazz)
      , name_(name)
    {}

    /**
     * Loads the spacified Java class and constructs a Java class wrapper.
     *
     * @param env JNI environment
     * @param name the class or interface name, e.g. java.lang.String
     */
    JClass(JNIEnv* env, char const* const clazzName);

    /**
     * Destructor.
     */
    ~JClass() {}

    /**
     * Returns the method ID for a nonstatic method of the class.
     *
     * The Java method may be defined in one of the clazz's superclasses and
     * inherited by clazz. The method is determined by its methodName
     * and signature.  Calling this method causes an uninitialized class
     * to be initialized.
     *
     * This method invokes the JNI interface GetMethodID function and checks
     * if a valid reference is returned.
     *
     * @param methodName the name of the method.  To obtain the method ID
     *      of a constructor, supply <init> as the method name and void
     *      (V) as the return type.
     * @param  method signature
     * @return method ID handle
     */
    jmethodID getMethodID(const char* methodName, const char* signature) const;

    /**
     * Returns the method ID for a static method of the class.
     *
     * The Java method may be defined in one of the clazz's superclasses and
     * inherited by clazz. The method is determined by its methodName
     * and signature.  Calling this method causes an uninitialized class
     * to be initialized.
     *
     * This method invokes the JNI interface GetStaticMethodID function and
     * checks if a valid reference is returned.
     *
     * @param methodName the name of the method.
     * @param  method signature
     * @return method ID handle
     */
    jmethodID getStaticMethodID(const char* methodName, const char* signature) const;

    /**
     * Determines whether this JClass can be safely cast to otherclass.
     *
     * @param otherclass the other JClass
     * @return true if this class is the same as the one specified by @c otherclass,
     *      if this class is a subclass of otherclass, or if this class has
     *      otherclass as one of its interfaces; otherwise it returns false.
     *
     */
    bool isAssignableFrom(JClass const& otherclass) const
    {
        return env_->IsAssignableFrom(jclass_, otherclass.get());
    }

    /**
     * Determines whether this JClass can be safely cast to otherclass.
     *
     * @param otherclass the other jclass
     * @return true if this class is the same as the one specified by @c otherclass,
     *      if this class is a subclass of otherclass, or if this class has
     *      otherclass as one of its interfaces; otherwise it returns false.
     */
    bool isAssignableFrom(jclass const otherclass) const
    {
        return env_->IsAssignableFrom(jclass_, otherclass);
    }

    /**
     * Returns the wrapped Java class object.
     */
    jclass get() const { return jclass_; }

  private:
    JNIEnv* env_;
    jclass jclass_;
    const std::string name_;
};

/**
 * Scoped wrapper for a JNI global reference.
 *
 * Creates a global reference from a local one.  Releases the global
 * reference when the wrapper goes out of scope.
 */
class JGlobalRef
{
  public:
    JGlobalRef(JNIEnv* env, jobject localRef)
      : env_(env)
    {
        jref_ = JGlobalRef::newRef(env_, localRef);
    }

    ~JGlobalRef() { deleteRef(env_, jref_); }

    /** Get the global reference */
    jobject get() const { return jref_; };

    /** Get the global reference */
    jobject operator->() const
    {
        assert(jref_ != NULL);
        return jref_;
    }

    /**
     * Utility function which creates a global reference from the specified
     * local reference.  It also deletes the local reference if the
     * @c deleteLocalRef parameter is true.
     *
     * @param env JNI environment
     * @param localRef  local reference to a Java object.  If NULL, then the
     *      funtion returns NULL.
     * @param deleteLocalRef  if true, then the local reference is deleted
     *      if not NULL
     * @return a new global reference, or NULL if the local reference was NULL
     */
    static jobject newRef(JNIEnv* env, jobject localRef, bool deleteLocalRef = true);

    /** Deletes the specified global reference. */
    static void deleteRef(JNIEnv* env, jobject ref)
    {
        assert(env != NULL);
        if (ref != NULL) {
            env->DeleteGlobalRef(ref);
        }
    }

  private:
    JNIEnv* env_;
    jobject jref_;
};

/**
 * Scoped wrapper for a Java object array.
 *
 * Creates a global reference for the object array, which is deleted when the
 * wrapper object goes out of scope.
 */
class JObjectArray
{
  public:
    /**
     * Creates a @c java.lang.String[] from the specified c-string array
     * of arrays.
     * @param env JNI environment
     * @param strArray array of c strings
     * @param size length of @c strArray
     */
    JObjectArray(JNIEnv* env, char const* const* strArray, size_t size)
      : env_(env)
      , joArray_(NULL)
    {
        jobjectArray array = init(strArray, size);
        joArray_ = (jobjectArray)JGlobalRef::newRef(env, array);
    }

    /**
     * Creates a @c java.lang.String[] from the specified vector of strings.
     * @param env JNI environment
     * @param strArray vector of strings
     */
    JObjectArray(JNIEnv* env, std::vector<std::string> const* vec)
      : env_(env)
      , joArray_(NULL)
    {
        jobjectArray array = init(vec);
        joArray_ = (jobjectArray)JGlobalRef::newRef(env, array);
    }

    /**
     * Wraps a @c JObjectArray around a @ jobjectArray local reference.
     *
     * @param env JNI environment
     * @param array array of Java objects.  Must be not NULL.
     */
    JObjectArray(JNIEnv* env, jobjectArray const array)
      : env_(env)
      , joArray_(NULL)
    {
        JNI_CHECK_REF(env, array, "JNI jobjectArray");
        joArray_ = (jobjectArray)JGlobalRef::newRef(env, array);
    }

    ~JObjectArray() { cleanup(); }

    /** Get the current array reference */
    jobjectArray get() const { return joArray_; };

    jsize length() const { return env_->GetArrayLength(joArray_); }

    /**
     * Returns a reference to the element at specified location index.
     * This function calls the @c GetObjectArrayElement JNI function.
     * No bounds checking is performed on the C++ side, but an exception of
     * type SPLRuntimeJNIException will be thrown if the JNI function returns
     * an error.
     * @param index position of the elemet to return
     * @return element
     */
    jobject at(jsize index) const;

  private:
    jobjectArray init(char const* const* strArray, size_t size);
    jobjectArray init(std::vector<std::string> const* vec);
    void cleanup();

    JNIEnv* env_;
    jobjectArray joArray_;
};

/**
 * Java byte array wrapper.
 */
class JByteArray
{
  public:
    /**
     * Creates a new Java @c byte[] and copies @c size bytes from the
     * specified @c buffer into the new array.
     *
     * @param env JNI environment
     * @param buffer byte array
     * @param size size of the byte array
     * @return new Java @c byte[]
     */
    static jbyteArray newArray(JNIEnv* env, unsigned char const* bytes, size_t size)
    {
        jbyteArray ba = env->NewByteArray(size);
        env->SetByteArrayRegion(ba, 0, size, (jbyte*)bytes);
        return ba;
    }
};

}
} // end namespace SPL::JNI

#endif /* SPL_RUNTIME_UTILITY_JNIUTILS_H */
