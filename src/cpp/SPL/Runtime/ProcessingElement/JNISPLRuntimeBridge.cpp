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

#include <Runtime/ProcessingElement/JNISPLRuntimeBridge.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/Port/TupleVisualizer.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/ViewProperties.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <algorithm>
#include <cassert>
#include <dlfcn.h>
#include <map>

using namespace std;
using namespace SPL;
using namespace SPL::JNI;

static const char* jniBridgeClassName = "com/ibm/streams/operator/internal/jni/JNISPLRuntimeBridge";

// Map from internal log and trace values to external
static jint mapTraceLogLevelToExternal(int level);

static string dotName(const char* s)
{
    string name(s);
    replace(name.begin(), name.end(), '/', '.');
    return name;
}

JNISPLRuntimeBridge::JNISPLRuntimeBridge(JavaVM* vm)
  : jvm_(vm)
{
    JVMEnv env(vm);
    assert(env.isAttached());

    JClass jnibridge(env.get(), jniBridgeClassName);
    setAppLogLevel_ = jnibridge.getStaticMethodID("setAppLogLevel", "(I)V");
    setAppTrcLevel_ = jnibridge.getStaticMethodID("setAppTrcLevel", "(I)V");
    setVisualizing_ = jnibridge.getStaticMethodID("setVisualizing", "(Z)V");
    startSPLJavaRuntime_ = jnibridge.getStaticMethodID("startSPLJavaRuntime",
                                                       "(Ljava/lang/String;[Ljava/lang/String;)V");
    shutdownSPLJavaRuntime_ = jnibridge.getStaticMethodID("shutdownSPLJavaRuntime", "(Z)V");
    connectOperators_ = jnibridge.getStaticMethodID("connectOperators", "()V");
    addView_ = jnibridge.getStaticMethodID("addView", "(IIJ)V");
    deleteView_ = jnibridge.getStaticMethodID("deleteView", "(IIJ)V");

    bridgeClass_ = reinterpret_cast<jclass>(env->NewGlobalRef(jnibridge.get()));

    APPTRC(L_INFO, "Successfully loaded class " << dotName(jniBridgeClassName), SPL_PE_DBG);
}

JNISPLRuntimeBridge::~JNISPLRuntimeBridge() {}

void JNISPLRuntimeBridge::startSPLJavaRuntime(string const& model, vector<string> const* params)
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    APPTRC(L_DEBUG, "Starting the SPL Java runtime", SPL_PE_DBG);
    SPL::JNI::JString jstrModel(env.get(), model);
    SPL::JNI::JObjectArray jParams(env.get(), params);
    env->CallStaticVoidMethod(bridgeClass_, startSPLJavaRuntime_, jstrModel.get(), jParams.get());
    processException(env.get());

    APPTRC(L_INFO, "Started the SPL Java runtime", SPL_PE_DBG);
}

void JNISPLRuntimeBridge::shutdownSPLJavaRuntime(bool finalNotice)
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    env->CallStaticVoidMethod(bridgeClass_, shutdownSPLJavaRuntime_, finalNotice);
    // just log exceptions, if any, don't rethrow
    processException(env.get(), false);

    releaseJavaBridge(env.get());
}

void JNISPLRuntimeBridge::logLevelUpdate(int level)
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    env->CallStaticVoidMethod(bridgeClass_, setAppLogLevel_, mapTraceLogLevelToExternal(level));
    processException(env.get());
}

void JNISPLRuntimeBridge::traceLevelUpdate(int level)
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    env->CallStaticVoidMethod(bridgeClass_, setAppTrcLevel_, mapTraceLogLevelToExternal(level));
    processException(env.get());
}

void JNISPLRuntimeBridge::visualizerUpdate(bool isVisualizing)
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    env->CallStaticVoidMethod(bridgeClass_, setVisualizing_, isVisualizing);
    processException(env.get());
}

void JNISPLRuntimeBridge::connectOperators()
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    env->CallStaticVoidMethod(bridgeClass_, connectOperators_);
    processException(env.get());
}

void JNISPLRuntimeBridge::addView(int operatorIndex,
                                  int outputPortIndex,
                                  ViewProperties* properties)
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    env->CallStaticVoidMethod(bridgeClass_, addView_, operatorIndex, outputPortIndex, properties);
    processException(env.get());
}

void JNISPLRuntimeBridge::deleteView(int operatorIndex,
                                     int outputPortIndex,
                                     ViewProperties* properties)
{
    JVMEnv env(jvm_);
    assert(env.isAttached());

    env->CallStaticVoidMethod(bridgeClass_, deleteView_, operatorIndex, outputPortIndex,
                              properties);
    processException(env.get());
}

static jint mapTraceLogLevelToExternal(int level)
{
    switch (level) {
        case iL_OFF:
            return L_OFF;
        case iL_ERROR:
            return L_ERROR;
        case iL_WARN:
            return L_WARN;
        case iL_INFO:
            return L_INFO;
        case iL_DEBUG:
            return L_DEBUG;
        case iL_TRACE:
            return L_TRACE;
    }
    return L_ERROR;
}

void JNISPLRuntimeBridge::processException(JNIEnv* env, bool rethrow /* = true */)
{
    jthrowable thr = env->ExceptionOccurred();
    if (thr == NULL) {
        return;
    }
    env->ExceptionClear();

    JClass throwableClass(env, "java/lang/Throwable", env->GetObjectClass(thr));
    // TODO use Throwable.getLocalizedMessage() instead?
    //    jmethodID throwable_getLocalizedMessage =
    //    throwableClass.getMethodID("getLocalizedMessage", "()Ljava/lang/String;"); JStringUTFChars
    //    msg(env, (jstring) env->CallObjectMethod(thr, throwable_getLocalizedMessage));
    jmethodID throwable_toString = throwableClass.getMethodID("toString", "()Ljava/lang/String;");
    JStringUTFChars msg(env, (jstring)env->CallObjectMethod(thr, throwable_toString));

    // Map some java exceptions to equivalent SPLRuntime exceptions
    JClass ioExceptionClass(env, "java/io/IOException");
    if (rethrow) {
        if (throwableClass.isAssignableFrom(ioExceptionClass)) {
            THROW(SPLRuntimeIO, msg.c_str());
        }
        THROW(SPLRuntime, msg.c_str());
    } else {
        APPTRC(L_WARN, "Java exception caught: " << msg.c_str(), SPL_PE_DBG);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Functions invoked by com.ibm.streams.operator.internal.jni.JNISPLRuntimeBridge

#ifdef __cplusplus
extern "C" {
#endif
//...

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNISPLRuntimeBridge
 * Method:    visualizeTuple
 * Signature: (IIL/java/nio/ByteBuffer;I)V
 */
JNIEXPORT DLL_PUBLIC void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNISPLRuntimeBridge_visualizeTuple(JNIEnv* env,
                                                                              jclass clz,
                                                                              jint operatorIndex,
                                                                              jint outputPortIndex,
                                                                              jobject rawTuple,
                                                                              jint tupleSize)
{
    PEImpl* pi = dynamic_cast<PEImpl*>(&SPL::ProcessingElement::pe());
    void* rawTupleData = env->GetDirectBufferAddress(rawTuple);
    NativeByteBuffer buf((unsigned char*)rawTupleData, tupleSize);
    OperatorImpl& operatorImpl = pi->getOperatorImpl(operatorIndex);
    operatorImpl.visualizeTuple(outputPortIndex, buf);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNISPLRuntimeBridge
 * Method:    setTagData
 * Signature: (Ljava/lang/String;Ljava/util/Map;)V
 */
JNIEXPORT DLL_PUBLIC void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNISPLRuntimeBridge_setTagData(JNIEnv* env,
                                                                          jclass clz,
                                                                          jstring tagName,
                                                                          jobject tagValues)
{
    PEImpl* pi = dynamic_cast<PEImpl*>(&SPL::ProcessingElement::pe());
    try {
        JStringUTFChars tagNameC(env, tagName);
        std::map<std::string, std::string> tagValuesMap;

        JClass setClass(env, "java/util/Set");
        jmethodID setIteratorMethod = setClass.getMethodID("iterator", "()Ljava/util/Iterator;");

        JClass mapClass(env, "java/util/Map");
        jmethodID mapKeySetMethod = mapClass.getMethodID("keySet", "()Ljava/util/Set;");
        jmethodID mapGetMethod =
          mapClass.getMethodID("get", "(Ljava/lang/Object;)Ljava/lang/Object;");

        JClass iteratorClass(env, "java/util/Iterator");
        jmethodID iteratorNextMethod = iteratorClass.getMethodID("next", "()Ljava/lang/Object;");
        jmethodID iteratorHasNextMethod = iteratorClass.getMethodID("hasNext", "()Z");
        // Set<String> keys = tagValues.keySet();
        // Iterator<String> keysIterator = keys.iterator();
        // while (keysIterator.hasNext()) {
        //   String key = keysIterator.next();
        //   String value = tagValues.get(key);
        //   JStringUTFChars keyC(env, key);
        //   JStringUTFChars valueC(env, value);
        //   tagValuesMap[key] = value;
        // }

        jobject keys = env->CallObjectMethod(tagValues, mapKeySetMethod);
        jobject keysIterator = env->CallObjectMethod(keys, setIteratorMethod);
        while (env->CallBooleanMethod(keysIterator, iteratorHasNextMethod)) {
            jstring key =
              static_cast<jstring>(env->CallObjectMethod(keysIterator, iteratorNextMethod));
            jstring value =
              static_cast<jstring>(env->CallObjectMethod(tagValues, mapGetMethod, key));
            // The Java map allows nulls on key and value, but the C++ class
            // does not.  Discard and warn.
            if (key != NULL && value != NULL) {
                JStringUTFChars keyC(env, key);
                JStringUTFChars valueC(env, value);
                tagValuesMap[keyC.c_str()] = valueC.c_str();
            } else {
                if (key == NULL) {
                    APPTRC(L_WARN,
                           "Tag data for tag name " << tagNameC.c_str()
                                                    << ": value with null key discarded.",
                           SPL_PE_DBG);
                }
                if (value == NULL) {
                    APPTRC(L_WARN,
                           "Tag data for tag name " << tagNameC.c_str()
                                                    << ": key with null value discarded.",
                           SPL_PE_DBG);
                }
            }
        }

        pi->setTagData(tagNameC.c_str(), tagValuesMap);
    } catch (Distillery::DistilleryException const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNISPLRuntimeBridge
 * Method:    getApplicationConfigAsByteArray
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT DLL_PUBLIC jbyteArray JNICALL
Java_com_ibm_streams_operator_internal_jni_JNISPLRuntimeBridge_getApplicationConfigAsByteArray(
  JNIEnv* env,
  jclass clz,
  jstring cfgName)
{
    JStringUTFChars cfgNameC(env, cfgName);
    try {
        PEImpl* pi = dynamic_cast<PEImpl*>(&SPL::ProcessingElement::pe());
        PlatformAdapter& platform = pi->getPlatform();
        std::map<std::string, std::string> properties;
        platform.getAppConfig(platform.getUserSessionId(), cfgNameC.c_str(), properties);

        // Serialized using Java byte order
        NetworkByteBuffer buf;
        buf.addInt32(0);                 // version 0
        buf.addInt32(properties.size()); // map entry count

        for (std::map<std::string, std::string>::iterator it = properties.begin();
             it != properties.end(); ++it) {

            rstring rkey(it->first);
            buf.addSTLString(rkey);
            rstring rvalue(it->second);
            buf.addSTLString(rvalue);
        }

        // Copy into a Java byte array, we can't use a ByteBuffer as the C++
        // buffer will become invalid once this function returns.
        return JByteArray::newArray(env, buf.getPtr(), buf.getContentSize());
    } catch (Distillery::DistilleryException const& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"),
                      (std::string("Cannot retrieve application configuration for ") +
                       cfgNameC.c_str() + ". " + e.getExplanation())
                        .c_str());
    } catch (std::exception const& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"),
                      (std::string("Cannot retrieve application configuration for ") +
                       cfgNameC.c_str() + ". " + e.what())
                        .c_str());
    } catch (...) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"),
                      (std::string("Cannot retrieve application configuration for ") +
                       cfgNameC.c_str() + ". Unknown exception.")
                        .c_str());
    }
    return NULL; // squelch compiler warnings
}

#ifdef __cplusplus
}
#endif
