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

#include <SPL/Toolkit/JavaOp.h>

#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/Metric.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Function/UtilFunctions.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Operator/OperatorMetrics.h>
#include <SPL/Runtime/Operator/OperatorMetricsHandler.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyCollection.h>
#include <SPL/Runtime/Operator/State/CheckpointContext.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContext.h>
#include <SPL/Runtime/Operator/State/DataStoreEOFException.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/PEMetrics.h>
#include <SPL/Runtime/ProcessingElement/PlatformAdapter.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Type/Decimal.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Utility/WatermarkPunctPayload.h>
#include <SPL/Toolkit/RuntimeException.h>
#include <UTILS/SupportFunctions.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string.h>

using namespace SPL;
using namespace SPL::Meta;
using namespace SPL::JNI;
using namespace std;

/**
   Log the message to SPLAPPLOG. Use of this extra function
   leads to a shorter line in the log file as the function
   is 'log' and not the full JNI method name.

   This is used for operator/function trace messages
   from Java.
*/
void LogTrace::log(jint level, char* msg, char* aspects)
{
    SPLAPPLOG(level, msg, aspects);
}

template<class T>
static T getCheckpointValue(JNIEnv* env, jclass clz, jlong ptr)
{
    T value;
    Checkpoint* ckpt = reinterpret_cast<Checkpoint*>(ptr);
    try {
        *ckpt >> value;
    } catch (DataStoreEOFException const& e) {
        jclass expClass = env->FindClass("java/io/EOFException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
        return false;
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
        return false;
    }
    return value;
}

template<class T>
static void putCheckpointValue(JNIEnv* env, jclass clz, jlong ptr, T value)
{
    Checkpoint* ckpt = reinterpret_cast<Checkpoint*>(ptr);
    try {
        *ckpt << value;
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
}

extern "C" { // prevent C++ name mangling for the functions called from Java

/*
 * Create a C++ Tuple specific to the port and then
 * set its value from the serialized binary form passed in from Java.
 * Call the virtual method to normalize bounded maps and sets as
 * the Java code cannot serialize out a bounded map or set that
 * matches the C++ memory format due to potential differences
 * in hashcodes.
 *
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    submitTuple
 * Signature: (JILjava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_submitTuple(JNIEnv* env,
                                                                 jclass clz,
                                                                 jlong ptr,
                                                                 jint port,
                                                                 jobject rawTuple,
                                                                 jint tupleSize)
{
    void* rawTupleData = env->GetDirectBufferAddress(rawTuple);

    Operator* op = reinterpret_cast<Operator*>(ptr);
    NativeByteBuffer buf((unsigned char*)rawTupleData, tupleSize);
    try {
        op->submit(buf, port);
    } catch (SPLRuntimeInvalidIndexException e) {
        jclass expClass = env->FindClass("java/lang/IllegalStateException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    } catch (Distillery::DistilleryException const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    } catch (std::exception const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.what());
    } catch (...) {
        jclass expClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(expClass, "");
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    punctuate
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_com_ibm_streams_operator_internal_jni_JNIBridge_punctuate(JNIEnv* env,
                                                                                      jclass clz,
                                                                                      jlong ptr,
                                                                                      jint port,
                                                                                      jint mark)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    try {
        op->submit((Punctuation::Value)mark, (uint32_t)port);
    } catch (SPLRuntimeInvalidIndexException e) {
        jclass expClass = env->FindClass("java/lang/IllegalStateException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
}

/*
 * Used to trace operator (application) messages from Java.
 * Class:     com_ibm_streams_operator_internal_jni_JNITraceSendApp
 * Method:    appTrc
 * Signature: (ILjava/nio/ByteBuffer;Ljava/nio/ByteBufferLjava/nio/ByteBuffer;Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNITraceSendApp_appTrc(JNIEnv* env,
                                                                  jclass clz,
                                                                  jint level,
                                                                  jobject msg,
                                                                  jobject aspect,
                                                                  jobject classMethod,
                                                                  jobject file,
                                                                  jint line)
{
    int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[level];
    if (ilvl > Distillery::debug::app_trace_level) {
        return;
    }

    char* msgData = (char*)(env->GetDirectBufferAddress(msg));
    char* aspectData = (char*)(env->GetDirectBufferAddress(aspect));
    char* classMethodData = (char*)(env->GetDirectBufferAddress(classMethod));
    char* fileData = (char*)(env->GetDirectBufferAddress(file));

    Distillery::debug::write_appmsg(ilvl, SPL::splAppTrcAspect(aspectData), classMethodData,
                                    fileData, line, msgData);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNILogSend
 * Method:    appLog
 * Signature: (ILjava/nio/ByteBuffer;Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_com_ibm_streams_operator_internal_jni_JNILogSend_appLog(JNIEnv* env,
                                                                                    jclass clz,
                                                                                    jint level,
                                                                                    jobject msg,
                                                                                    jobject aspect)
{
    char* msgData = (char*)(env->GetDirectBufferAddress(msg));
    char* aspectData = (char*)(env->GetDirectBufferAddress(aspect));
    LogTrace::log(level, msgData, aspectData);
}

/*
 * Used to trace Java runtime messages.
 * Class:     com_ibm_streams_operator_internal_jni_JNITraceSendSPL
 * Method:    splTrc
 * Signature: (ILjava/nio/ByteBuffer;Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNITraceSendSPL_splTrc(JNIEnv* env,
                                                                  jclass clz,
                                                                  jint level,
                                                                  jobject msg,
                                                                  jobject aspect,
                                                                  jobject classMethod,
                                                                  jobject file,
                                                                  jint line)
{

    int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[level];
    if (ilvl > Distillery::debug::app_trace_level) {
        return;
    }

    char* msgData = (char*)(env->GetDirectBufferAddress(msg));
    char* aspectData = (char*)(env->GetDirectBufferAddress(aspect));
    char* classMethodData = (char*)(env->GetDirectBufferAddress(classMethod));
    char* fileData = (char*)(env->GetDirectBufferAddress(file));

    Distillery::debug::write_appmsg(ilvl, aspectData, fileData, classMethodData, line, msgData);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getPortMetric
 * Signature: (JIZZI)J
 * Get a pe/operator output/input port metric and return a pointer
 * to it as a Java long.
 */
JNIEXPORT jlong JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getPortMetric(JNIEnv* env,
                                                                   jclass clz,
                                                                   jlong ptr,
                                                                   jint port,
                                                                   jboolean isPE,
                                                                   jboolean isOutput,
                                                                   jint metricOrdinal)
{
    if (isPE) {
        if (isOutput) {
            return (jlong) & (ProcessingElement::pe().getMetrics().getOutputPortMetric(
                               (uint32_t)port, (PEMetrics::OutputPortMetricName)metricOrdinal));
        } else {
            return (jlong) & (ProcessingElement::pe().getMetrics().getInputPortMetric(
                               (uint32_t)port, (PEMetrics::InputPortMetricName)metricOrdinal));
        }
    } else {
        Operator* op = reinterpret_cast<Operator*>(ptr);
        if (isOutput) {
            return (jlong) &
                   (op->getContext().getMetrics().getOutputPortMetric(
                     (uint32_t)port, (OperatorMetrics::OutputPortMetricName)metricOrdinal));
        } else {
            return (jlong) &
                   (op->getContext().getMetrics().getInputPortMetric(
                     (uint32_t)port, (OperatorMetrics::InputPortMetricName)metricOrdinal));
        }
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getSystemMetric
 * Signature: (JIZZI)J
 * Get an operator system metric and return a pointer
 * to it as a Java long.
 */
JNIEXPORT jlong JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getOperatorMetricInternal(JNIEnv* env,
                                                                               jclass clz,
                                                                               jlong ptr,
                                                                               jint metricOrdinal)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    return (jlong) & (op->getContext().getMetrics().getOperatorMetric(
                       (OperatorMetrics::SystemMetricName)metricOrdinal));
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    addCustomMetric
 * Signature: (JLjava/nio/ByteBuffer;Ljava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_addCustomMetric(JNIEnv* env,
                                                                     jclass clz,
                                                                     jlong ptr,
                                                                     jobject name,
                                                                     jobject description,
                                                                     jint kind)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    string mName((char*)(env->GetDirectBufferAddress(name)));
    string mDesc((char*)(env->GetDirectBufferAddress(description)));
    string oName(op->getContext().getName());
    MetricImpl m(mName, mDesc, (Metric::Kind)kind);
    SPL::PEImpl::instance().getPlatform().addCustomMetric(m.getName(), m.getKindName(),
                                                          m.getDescription(), oName);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getCustomMetric
 * Signature: (JLjava/nio/ByteBuffer;)J
 * get a custom operator metric.
 */
JNIEXPORT jlong JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getCustomMetric(JNIEnv* env,
                                                                     jclass clz,
                                                                     jlong ptr,
                                                                     jobject nb)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    string name((char*)(env->GetDirectBufferAddress(nb)));
    try {
        return (jlong) & (op->getContext().getMetrics().getCustomMetricByName(name));
    } catch (SPLRuntimeInvalidMetricException e) {
        jclass expClass = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
        return 0;
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getCustomMetricNames
 * Signature: (JI)[Ljava/lang/String;
 * get custom operator metric names as a String[]
 * Returns null if Java already has all the metrics.
 */
JNIEXPORT jobjectArray JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getCustomMetricNames(JNIEnv* env,
                                                                          jclass clz,
                                                                          jlong ptr,
                                                                          jint currentCount)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    vector<string> names;
    names = op->getContext().getMetrics().getCustomMetricNames();

    // Java already has all the elements
    if ((jint)names.size() == currentCount) {
        return NULL;
    }

    jobjectArray jnames =
      env->NewObjectArray(names.size(), env->FindClass("java/lang/String"), NULL);
    for (uint32_t i = 0; i < names.size(); i++) {
        jstring jname = JVMControl::javaString(env, names[i]);
        env->SetObjectArrayElement(jnames, i, jname);
        env->DeleteGlobalRef(jname);
    }
    return jnames;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getMetricName
 * Signature: (J)Ljava/lang/String;
 *
 * Simply call the SPL::Metric.getName().
 * Note the conversion to a Java String returns a global reference
 * which we don't want to hold forever so we instead return a local ref.
 */
JNIEXPORT jstring JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getMetricName(JNIEnv* env,
                                                                   jclass clz,
                                                                   jlong ptr)
{
    Metric* metric = reinterpret_cast<Metric*>(ptr);
    jstring jname = JVMControl::javaString(env, metric->getName());
    jstring lref = (jstring)env->NewLocalRef(jname);
    env->DeleteGlobalRef(jname);
    return lref;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getMetricDescription
 * Signature: (J)Ljava/lang/String;
 *
 * Simply call the SPL::Metric.getDescription().
 */
JNIEXPORT jstring JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getMetricDescription(JNIEnv* env,
                                                                          jclass clz,
                                                                          jlong ptr)
{
    Metric* metric = reinterpret_cast<Metric*>(ptr);
    jstring jdesc = JVMControl::javaString(env, metric->getDescription());
    jstring lref = (jstring)env->NewLocalRef(jdesc);
    env->DeleteGlobalRef(jdesc);
    return lref;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getMetricKind
 * Signature: (J)I
 *
 * Simply call the SPL::Metric.getKind().
 */
JNIEXPORT jint JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getMetricKind(JNIEnv* env,
                                                                   jclass clz,
                                                                   jlong ptr)
{
    Metric* metric = reinterpret_cast<Metric*>(ptr);
    return metric->getKind();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getMetricValue
 * Signature: (J)J
 *
 * Simply call the SPL::Metric.getValue().
 */
JNIEXPORT jlong JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getMetricValue(JNIEnv* env,
                                                                    jclass clz,
                                                                    jlong ptr)
{
    Metric* metric = reinterpret_cast<Metric*>(ptr);
    return (jlong)metric->getValue();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    decrementSystemMetricValue
 * Signature: (J)V
 *
 * This should ONLY be called for system metrics i.e. classes of type SystemMetricImpl.
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_decrementSystemMetricValue(JNIEnv* env,
                                                                                jclass clz,
                                                                                jlong ptr)
{
    SystemMetricImpl* metric = reinterpret_cast<SystemMetricImpl*>(ptr);
    metric->decrementValueInternal();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    bumpSystemMetricValue
 * Signature: (J)V
 *
 * This should ONLY be called for system metrics i.e. classes of type SystemMetricImpl.
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_bumpSystemMetricValue(JNIEnv* env,
                                                                           jclass clz,
                                                                           jlong ptr)
{
    SystemMetricImpl* metric = reinterpret_cast<SystemMetricImpl*>(ptr);
    metric->incrementValueInternal();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    setSystemMetricValue
 * Signature: (JJ)V
 *
 * This should ONLY be called for system metrics i.e. classes of type SystemMetricImpl.
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_setSystemMetricValue(JNIEnv* env,
                                                                          jclass clz,
                                                                          jlong ptr,
                                                                          jlong value)
{
    SystemMetricImpl* metric = reinterpret_cast<SystemMetricImpl*>(ptr);
    metric->setValueInternal(value);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    setMaxSystemMetricValue
 * Signature: (JJ)V
 *
 * This should ONLY be called for system metrics i.e. classes of type SystemMetricImpl.
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_setMaxSystemMetricValue(JNIEnv* env,
                                                                             jclass clz,
                                                                             jlong ptr,
                                                                             jlong value)
{
    SystemMetricImpl* metric = reinterpret_cast<SystemMetricImpl*>(ptr);
    metric->setMaxValueInternal(value);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getStreamProperties
 * Signature: (JI)[B;
 *
 * Get the stream properties for a stream exported by properties.
 * The properties are serialized and returned as a byte array.
 */
JNIEXPORT jbyteArray JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getStreamProperties(JNIEnv* env,
                                                                         jclass clz,
                                                                         jlong ptr,
                                                                         jint port)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    StreamPropertyCollection props;
    op->getOutputPortAt(port).getStreamProperties(props);

    // Serialized using Java byte order for simplicity.
    NetworkByteBuffer buf;
    buf.addInt32(0);            // future proof.
    buf.addInt32(props.size()); // number of properties

    for (uint32_t i = 0; i < props.size(); i++) {
        StreamProperty p = props[i];

        // Serialize the name and SPL type as rstrings.

        rstring rn(p.getName());
        buf.addSTLString(rn);

        rstring rt(p.getType().getName());
        buf.addSTLString(rt);

        // and finally the value as its SPL serialized value
        p.getValueHandle().serialize(buf);
    }

    // Copy into a Java byte array, we can't use a ByteBuffer
    // as the C++ buffer will become invalid once this
    // function returns.
    jsize bsize = (jsize)buf.getContentSize();
    jbyteArray ba = env->NewByteArray(bsize);
    env->SetByteArrayRegion(ba, 0, bsize, (jbyte*)buf.getPtr());

    return ba;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    setStreamProperties
 * Signature: (JIZLjava/nio/ByteBuffer;I)V;
 *
 * Set the stream properties for a stream exported by properties.
 * The properties are passed in serialized matching the format
 * of getStreamProperties.
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_setStreamProperties(JNIEnv* env,
                                                                         jclass clz,
                                                                         jlong ptr,
                                                                         jint port,
                                                                         jboolean set,
                                                                         jobject data,
                                                                         jint dataSize)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    StreamPropertyCollection props;

    void* rawData = env->GetDirectBufferAddress(data);
    NetworkByteBuffer buf((unsigned char*)rawData, dataSize);
    buf.getInt32(); // future versioning - always 0 now
    int32 count = buf.getInt32();

    props.reserve(count);

    for (int i = 0; i < count; i++) {
        StreamProperty property;
        string name = buf.getSTLString();
        string type = buf.getSTLString();
        if ("int64" == type) {
            property = StreamProperty(name, (int64)0L);
        } else if ("float64" == type) {
            property = StreamProperty(name, (float64)0.0);
        } else if ("rstring" == type) {
            const rstring v("");
            property = StreamProperty(name, v);
        } else if ("list<int64>" == type) {
            const SPL::list<int64> v;
            property = StreamProperty(name, v);
        } else if ("list<float64>" == type) {
            const SPL::list<float64> v;
            property = StreamProperty(name, v);
        } else if ("list<rstring>" == type) {
            const SPL::list<rstring> v;
            property = StreamProperty(name, v);
        }
        property.getValueHandle().deserialize(buf);
        props.push_back(property);
    }

    try {
        if (set) {
            op->getOutputPortAt(port).setStreamProperties(props);
        } else {
            op->getOutputPortAt(port).addStreamProperties(props);
        }

    } catch (SPLRuntimeStreamPropertyException e) {
        jclass expClass = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    removeStreamProperties
 * Signature: (JI[Ljava/nio/ByteBuffer;)V
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_removeStreamProperties(JNIEnv* env,
                                                                            jclass clz,
                                                                            jlong ptr,
                                                                            jint port,
                                                                            jobjectArray names)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    jsize nl = env->GetArrayLength(names);
    vector<string> cnames;
    cnames.reserve(nl);
    for (int i = 0; i < nl; i++) {
        jobject nb = env->GetObjectArrayElement(names, i);
        string name((char*)(env->GetDirectBufferAddress(nb)));
        cnames.push_back(name);
    }
    try {
        op->getOutputPortAt(port).removeStreamProperties(cnames);
    } catch (SPLRuntimeStreamPropertyException e) {
        jclass expClass = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    setOperatorTracker
 * Signature: (I)V
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_setOperatorTracker(JNIEnv* env,
                                                                        jclass clz,
                                                                        jint index)
{
    SPL::OperatorTracker::setCurrentOperator((uint32_t)index);
}

/* Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    resetOperatorTracker
 * Signature: (I)V
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_resetOperatorTrackerInternal(JNIEnv* env,
                                                                                  jclass clz)
{
    SPL::OperatorTracker::resetCurrentOperator();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getOperatorTracker
 * Signature: ()I
 *
 */
JNIEXPORT jint JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getOperatorTracker(JNIEnv* env, jclass clz)
{
    return (jint)SPL::OperatorTracker::getCurrentOperator();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    waitForShutdown
 * Signature: ()V
 *
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_waitForShutdown(JNIEnv* env, jclass clz)
{
    SPL::ProcessingElement::pe().blockUntilShutdownRequest();
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: getSequenceId
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getConsistentRegionSequenceId(JNIEnv* env,
                                                                                   jclass clz,
                                                                                   jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));
    if (ccContext == NULL) {
        return -1;
    }
    return ccContext->getSequenceId();
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: makeConsistent
 * Signature: (J)V
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_makeConsistent(JNIEnv* env,
                                                                    jclass clz,
                                                                    jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));

    ConsistentRegionPermit crp(ccContext);
    return (jboolean)ccContext->makeConsistent();
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: waitForResume
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_waitForResume(JNIEnv* env,
                                                                   jclass clz,
                                                                   jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));

    ccContext->acquirePermit();
    ccContext->releasePermit();
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: getResetAttempt
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getConsistentRegionResetAttempt(JNIEnv* env,
                                                                                     jclass clz,
                                                                                     jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));
    if (ccContext == NULL) {
        return -1;
    }
    return ccContext->getResetAttempt();
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: resetConsistentRegion
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_resetConsistentRegion(JNIEnv* env,
                                                                           jclass clz,
                                                                           jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));
    if (ccContext != NULL) {
        try {
            ccContext->resetRegion();
        } catch (...) {
            jclass expClass = env->FindClass("java/io/IOException");
            env->ThrowNew(expClass, "Cannot reset region.");
        }
    }
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: mustResetConsistentRegion
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_mustResetConsistentRegion(JNIEnv* env,
                                                                               jclass clz,
                                                                               jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContextImpl* ccContext = static_cast<ConsistentRegionContextImpl*>(
      op->getContext().getOptionalContext(CONSISTENT_REGION));
    if (ccContext != NULL) {
        ccContext->setMustReset(true);
    }
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: makeConsistentNonBlocking
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_makeConsistentNonBlocking(JNIEnv* env,
                                                                               jclass clz,
                                                                               jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));

    ConsistentRegionPermit crp(ccContext);
    return ccContext->makeConsistentNonBlocking();
}

/*
 * Class: com_ibm_streams_operator_internal_jni_JNIBridge
 * Method: enableNonBlockingCheckpoint
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_enableNonBlockingCheckpoint(JNIEnv* env,
                                                                                 jclass clz,
                                                                                 jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));
    if (ccContext == NULL) {
        return;
    }
    ccContext->enableNonBlockingCheckpoint();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getUserSession
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getUserSession(JNIEnv* env, jclass clazz)
{
    try {
        PEImpl* pi = dynamic_cast<PEImpl*>(&SPL::ProcessingElement::pe());
        std::string sessionId = pi->getPlatform().getUserSessionId();
        jstring jSessionId = JVMControl::javaString(env, sessionId);
        jstring lref = (jstring)env->NewLocalRef(jSessionId);
        env->DeleteGlobalRef(jSessionId);
        return lref;
    } catch (Distillery::DistilleryException const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    } catch (std::exception const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.what());
    } catch (...) {
        jclass expClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(expClass, "");
    }
    return jstring();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    getUserName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_getUserName(JNIEnv* env, jclass clazz)
{
    try {
        PEImpl* pi = dynamic_cast<PEImpl*>(&SPL::ProcessingElement::pe());
        std::string userName = pi->getPlatform().getUserName();
        jstring jUserName = JVMControl::javaString(env, userName);
        jstring lref = (jstring)env->NewLocalRef(jUserName);
        env->DeleteGlobalRef(jUserName);
        return lref;
    } catch (Distillery::DistilleryException const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    } catch (std::exception const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.what());
    } catch (...) {
        jclass expClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(expClass, "");
    }
    return jstring();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    submitWatermark
 * Signature: (JJII)B
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_submitWatermark(JNIEnv* env,
                                                                     jclass clazz,
                                                                     jlong ptr,
                                                                     jlong secs,
                                                                     jint nanos,
                                                                     jint port)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    timestamp ts = timestamp(secs, nanos);
    EventTimeContext* etc = EventTimeContext::getContext(*op);
    if (etc) {
        return etc->submit(ts, port);
    }
    return false;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    submitWatermarkAll
 * Signature: (JJI)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_submitWatermarkAll(JNIEnv* env,
                                                                        jclass clazz,
                                                                        jlong ptr,
                                                                        jlong secs,
                                                                        jint nanos)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    timestamp ts = timestamp(secs, nanos);
    EventTimeContext* etc = EventTimeContext::getContext(*op);
    if (etc) {
        etc->submit(ts);
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointBridge
 * Method:    createCheckpoint
 * Signature: (JJ)B
 * Create a checkpoint
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointBridge_createCheckpoint(JNIEnv* env,
                                                                                jclass clz,
                                                                                jlong ptr,
                                                                                jlong key)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    CheckpointContextImpl* ckptContext =
      (dynamic_cast<OperatorContextImpl*>(&op->getContext()))->getCheckpointContextImpl();
    if (ckptContext == NULL) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass,
                      ("Internal checkpoint context cannot be retrieved for this operator: " +
                       op->getContext().getLogicalName())
                        .c_str());
    }
    try {
        return ckptContext->createCheckpoint(key);
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return false;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointBridge
 * Method:    createCheckpointDefault
 * Signature: (JJ)B
 * Create a checkpoint outside Consistent Region
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointBridge_createCheckpointDefault(JNIEnv* env,
                                                                                       jclass clz,
                                                                                       jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    CheckpointContextImpl* ckptContext =
      (dynamic_cast<OperatorContextImpl*>(&op->getContext()))->getCheckpointContextImpl();
    if (!op->getContext().isCheckpointingOn() || ckptContext == NULL) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass,
                      ("Internal checkpoint context cannot be retrieved for this operator: " +
                       op->getContext().getLogicalName())
                        .c_str());
    }
    try {
        return ckptContext->createCheckpoint();
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return false;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointBridge
 * Method:    restoreCheckpoint
 * Signature: (JJ)B
 * Restore a checkpoint
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointBridge_restoreCheckpoint(JNIEnv* env,
                                                                                 jclass clz,
                                                                                 jlong ptr,
                                                                                 jlong key)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    CheckpointContextImpl* ckptContext =
      (dynamic_cast<OperatorContextImpl*>(&op->getContext()))->getCheckpointContextImpl();
    if (ckptContext == NULL) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass,
                      ("Internal checkpoint context cannot be retrieved for this operator: " +
                       op->getContext().getLogicalName())
                        .c_str());
    }
    try {
        return ckptContext->restoreCheckpoint(key);
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return false;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointBridge
 * Method:    deleteCheckpoint
 * Signature: (JJ)B
 * Delete a checkpoint
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointBridge_deleteCheckpoint(JNIEnv* env,
                                                                                jclass clz,
                                                                                jlong ptr,
                                                                                jlong key)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    CheckpointContextImpl* ckptContext =
      (dynamic_cast<OperatorContextImpl*>(&op->getContext()))->getCheckpointContextImpl();
    if (ckptContext == NULL) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass,
                      ("Internal checkpoint context cannot be retrieved for this operator: " +
                       op->getContext().getLogicalName())
                        .c_str());
    }
    try {
        return ckptContext->deleteCheckpoint(key);
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return false;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointBridge
 * Method:    isEnabled
 * Signature: (J)B
 * Returns whether checkpointing/restore is enabled
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointBridge_isEnabled(JNIEnv* env,
                                                                         jclass clz,
                                                                         jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    CheckpointContextImpl* ckptContext =
      (dynamic_cast<OperatorContextImpl*>(&op->getContext()))->getCheckpointContextImpl();
    return ckptContext->isEnabled();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointTransport
 * Method:    getByte
 * Signature: (J)B
 * Get a byte value from a checkpoint
 */
JNIEXPORT jbyte JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointTransport_getByte(JNIEnv* env,
                                                                          jclass clz,
                                                                          jlong ptr)
{
    return (jbyte)getCheckpointValue<int8_t>(env, clz, ptr);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointTransport
 * Method:    putByte
 * Signature: (JB)V
 * Put a byte value into a checkpoint
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointTransport_putByte(JNIEnv* env,
                                                                          jclass clz,
                                                                          jlong ptr,
                                                                          jbyte value)
{
    putCheckpointValue<int8_t>(env, clz, ptr, (int8_t)value);
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointTransport
 * Method:    getByteBuffer
 * Signature: (J[java/lang/String;)I
 * Get a byte buffer into a checkpoint
 */
JNIEXPORT jint JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointTransport_getByteBuffer(JNIEnv* env,
                                                                                jclass clz,
                                                                                jlong ptr,
                                                                                jobject buf,
                                                                                jint size)
{
    char* ch = (char*)env->GetDirectBufferAddress(buf);
    Checkpoint* ckpt = reinterpret_cast<Checkpoint*>(ptr);
    try {
        uint64_t returnSize;
        ckpt->getFixedCharSequence(ch, (uint64_t)size, returnSize);
        return returnSize;
    } catch (DataStoreEOFException const& e) {
        jclass expClass = env->FindClass("java/io/EOFException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return -1;
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointTransport
 * Method:    putByteBuffer
 * Signature: (J[java/lang/String;)V
 * Put a byte buffer into a checkpoint
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointTransport_putByteBuffer(JNIEnv* env,
                                                                                jclass clz,
                                                                                jlong ptr,
                                                                                jobject buf,
                                                                                jint size)
{
    char* ch = (char*)env->GetDirectBufferAddress(buf);
    Checkpoint* ckpt = reinterpret_cast<Checkpoint*>(ptr);
    try {
        ckpt->addCharSequence(ch, size);
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointTransport
 * Method:    getTimestamp
 * Signature: (J)J
 * Get the timestamp from a checkpoint
 */
JNIEXPORT jlong JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointTransport_getTimestamp(JNIEnv* env,
                                                                               jclass clz,
                                                                               jlong ptr)
{
    Checkpoint* ckpt = reinterpret_cast<Checkpoint*>(ptr);
    try {
        return (jlong)(ckpt->getTimestamp() * 1000);
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/io/IOException");
        env->ThrowNew(expClass, e.getExplanation().c_str());
        return 0;
    }
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointBridge
 * Method:    isCheckpointingOn
 * Signature: (J)B
 * Returns whether checkpointing is ON
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointBridge_isCheckpointingOn(JNIEnv* env,
                                                                                 jclass clz,
                                                                                 jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    return op->getContext().isCheckpointingOn();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNICheckpointBridge
 * Method:    getCheckpointingInterval
 * Signature: (J)D
 * Returns whether checkpointing is ON
 */
JNIEXPORT jdouble JNICALL
Java_com_ibm_streams_operator_internal_jni_JNICheckpointBridge_getCheckpointingInterval(JNIEnv* env,
                                                                                        jclass clz,
                                                                                        jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    return op->getContext().getCheckpointingInterval();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    setMustRethrowException
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_setMustRethrowException(JNIEnv* env,
                                                                             jclass clz,
                                                                             jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    PEImpl* pi = dynamic_cast<PEImpl*>(&(op->getContext().getPE()));
    pi->setMustRethrowException();
}

/*
 * Class:     com_ibm_streams_operator_internal_jni_JNIBridge
 * Method:    mustRethrowException
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_ibm_streams_operator_internal_jni_JNIBridge_mustRethrowException(JNIEnv* env,
                                                                          jclass clz,
                                                                          jlong ptr)
{
    Operator* op = reinterpret_cast<Operator*>(ptr);
    PEImpl* pi = dynamic_cast<PEImpl*>(&(op->getContext().getPE()));
    return pi->mustRethrowException();
}

} // extern "C"

/**
   Construct the class used to invoke methods on an OperatorSetup instance.
   This class is shared across multiple operators.
 */
OpSetupInvoker::OpSetupInvoker(JavaVM* jvm, JNIEnv* env)
{
    _jvm = jvm;

    JClass opSetup(env, "com/ibm/streams/operator/internal/runtime/api/OperatorSetup");
    /*
    if(env->ExceptionCheck()) {
        JAVAOP_THROW(env, "class OperatorSetup"); // Internal error if this can't be found
    }
*/
    _opSetup = reinterpret_cast<jclass>(env->NewGlobalRef(opSetup.get()));

    _constructor = opSetup.getMethodID("<init>", "(Ljava/lang/String;)V");
    _setParameterMethod =
      opSetup.getMethodID("setParameter", "(Ljava/lang/String;Ljava/lang/String;)V");

    _setNumberOfInputPortsMethod = opSetup.getMethodID("setNumberOfInputPorts", "(I)V");
    _setInputPortMethod = opSetup.getMethodID("setInputPort", "(IILjava/lang/String;I)V");
    _setInputPortAttributeMethod =
      opSetup.getMethodID("setInputPortAttribute", "(IILjava/lang/String;Ljava/lang/String;)V");

    _setNumberOfOutputPortsMethod = opSetup.getMethodID("setNumberOfOutputPorts", "(I)V");
    _setOutputPortMethod = opSetup.getMethodID("setOutputPort", "(IILjava/lang/String;I)V");
    _setOutputPortAttributeMethod =
      opSetup.getMethodID("setOutputPortAttribute", "(IILjava/lang/String;Ljava/lang/String;)V");
}

/**
   Return an instance of OperatorSetup (Java object) specific to
   the operator. Returns a global reference to the Java object.
 */
jobject OpSetupInvoker::newSetup(JNIEnv* env, Operator* op)
{
    APPTRC(L_DEBUG, "Creating new setup for operator " << op->getContext().getName(),
           SPL_JAVART_DBG);

    // Create an instance of OperatorSetup
    // new OperatorSetup(name)
    jstring jname = JVMControl::javaString(env, op->getContext().getName());
    jobject setup = env->NewObject(_opSetup, _constructor, jname);

    if (env->ExceptionCheck()) {
        JAVAOP_THROW(env, "new OperatorSetup()"); // Internal error if this can't be created
    }
    env->DeleteGlobalRef(jname);

    /*
    ** Pass the ports information to OperatorSetup
    ** using the reflective type api.
    ** First the input ports then the output ports.
    ** Pseudo-Java code for input ports, similar code for output ports
    **
    ** setup.setNumberOfInputPorts(count);
    ** for (ports) {
    **   setup.setInputPort(port, name, attributeCount);
    **   for (attributes) {
    **        setup.setInputPortAttribute(port, position, type, name);
    **   }
    ** }
    */

    setPortCount(env, setup, false, op->getNumberOfInputPorts());

    ConsistentRegionContext* ccContext =
      static_cast<ConsistentRegionContext*>(op->getContext().getOptionalContext(CONSISTENT_REGION));
    for (uint32_t port = 0; port < op->getNumberOfInputPorts(); port++) {
        if (ccContext != NULL) {
            setParameter(
              env, setup,
              ("[opInputPortCutOblivious]." + boost::lexical_cast<std::string>(port))
                .c_str(),
              !ccContext->isConsistentInputPort(port));
        }
    }

    setPortCount(env, setup, true, op->getNumberOfOutputPorts());

    /*
    for (port = 0; port < op->getNumberOfOutputPorts(); port++) {
        OperatorOutputPort & outport = op->getOutputPortAt(port);
        setPort(env, setup, true, outport, (jint) outport.getExportType());
    }
*/

    /*
    ** Pass which operator input ports in the same PE are connected to
    ** the output ports of this operator.
    **
    ** This allows us to identify fused Java connections where we can
    ** use direct method calls to improve performance.
    */

    using namespace xmlns::prod::streams::application;
    OPModel const& opmod = op->getOperatorImpl().getContextImpl().getOperatorModel();
    PEImpl& peimpl = op->getOperatorImpl().getPE();
    operInstanceOutputPortsType::outputPort_const_iterator it;
    const operInstanceOutputPortsType::outputPort_sequence& oports =
      opmod.outputPorts().outputPort();
    for (it = oports.begin(); it != oports.end(); it++) {
        const operInstanceOutputPortType::intraPeConnection_sequence& conns =
          it->intraPeConnection();
        operInstanceOutputPortType::intraPeConnection_const_iterator it2;
        const string buf =
          "[intraPeConnection]." + boost::lexical_cast<std::string>(it->index());
        const char* name = buf.c_str();
        for (it2 = conns.begin(); it2 != conns.end(); it2++) {
            /*
            ** The parameter name identifies the output port for this operator.
            ** Two values are passed for each connection on this port.
            ** - receiving operator index
            ** - receiving operator port
            ** These values must be sent and received in the same order.
            */
            const operInstanceOutgoingConnectionType& c = *it2;
            setParameter(env, setup, name,
                         (int32_t)peimpl.modelOperIndexToPEOperIndex(c.operInstanceIndex()));
            setParameter(env, setup, name, (int32_t)c.iportIndex());
        }
    }

    // Set internal parameters
    setParameter(env, setup, "[operatorIndex]", (int32_t)op->getIndex());

    if (!op->getPE().isStandalone()) {
        setParameter(env, setup, "[configuredHostName]",
                     SPL::Functions::Utility::getConfiguredHostName());
    }

    // Set consistent cut parameters
    if (ccContext != NULL) {
        setParameter(env, setup, "[opIsInCutRegion]", true);
        setParameter(env, setup, "[opIsStartOfCutRegion]", ccContext->isStartOfRegion());
        setParameter(env, setup, "[opIsEndOfCutRegion]", ccContext->isEndOfRegion());
        setParameter(env, setup, "[opIsTriggerOperator]", ccContext->isTriggerOperator());
        setParameter(env, setup, "[opCCRegionIndex]", ccContext->getIndex());
    } else {
        setParameter(env, setup, "[opIsInCutRegion]", false);
    }

    APPTRC(L_DEBUG, "Created new setup for operator " << op->getContext().getName(),
           SPL_JAVART_DBG);

    return env->NewGlobalRef(setup);
}

void OpSetupInvoker::setParameter(JNIEnv* env, jobject setup, const char* name, const char* value)
{
    setParameter(env, setup, name, std::string(value));
}
void OpSetupInvoker::setParameter(JNIEnv* env, jobject setup, const char* name, bool value)
{
    setParameter(env, setup, name, std::string(value ? "true" : "false"));
}
void OpSetupInvoker::setParameter(JNIEnv* env, jobject setup, const char* name, int32_t value)
{
    std::stringstream buf;
    buf << value;
    setParameter(env, setup, name, buf.str());
}

void OpSetupInvoker::setParameter(JNIEnv* env, jobject setup, const char* name, uint64_t value)
{
    std::stringstream buf;
    buf << value;
    setParameter(env, setup, name, buf.str());
}
void OpSetupInvoker::setParameter(JNIEnv* env, jobject setup, const char* name, int64_t value)
{
    std::stringstream buf;
    buf << value;
    setParameter(env, setup, name, buf.str());
}
void OpSetupInvoker::setParameter(JNIEnv* env, jobject setup, const char* name, double value)
{
    std::stringstream buf;
    buf << value;
    setParameter(env, setup, name, buf.str());
}
void OpSetupInvoker::setParameter(JNIEnv* env,
                                  jobject setup,
                                  const char* name,
                                  const SPL::ustring& value)
{
    setParameter(env, setup, name, SPL::ustring::toUTF8(value));
}
void OpSetupInvoker::setParameter(JNIEnv* env,
                                  jobject setup,
                                  const char* name,
                                  const SPL::decimal32& value)
{
    std::stringstream buf;
    serializeWithPrecision(buf, value);
    setParameter(env, setup, name, buf.str());
}
void OpSetupInvoker::setParameter(JNIEnv* env,
                                  jobject setup,
                                  const char* name,
                                  const SPL::decimal64& value)
{
    std::stringstream buf;
    serializeWithPrecision(buf, value);
    setParameter(env, setup, name, buf.str());
}
void OpSetupInvoker::setParameter(JNIEnv* env,
                                  jobject setup,
                                  const char* name,
                                  const SPL::decimal128& value)
{
    std::stringstream buf;
    serializeWithPrecision(buf, value);
    setParameter(env, setup, name, buf.str());
}

/**
   Set a parameter value from the JavaOp invocation, can be called
   multiple times for a given parameter name to allow multiple values
   per parameter.
 */
void OpSetupInvoker::setParameter(JNIEnv* env,
                                  jobject setup,
                                  const char* name,
                                  const std::string& value)
{
    APPTRC(L_TRACE, "ENTER setParameter " << name << "=" << value, SPL_JAVART_DBG);
    jstring jname = JVMControl::javaString(env, name);
    jstring jvalue = JVMControl::javaString(env, value);

    env->CallVoidMethod(setup, _setParameterMethod, jname, jvalue);

    if (env->ExceptionCheck()) {
        env->DeleteGlobalRef(jname);
        env->DeleteGlobalRef(jvalue);

        // Internal error if this can't be called
        JAVAOP_THROW(env, "OperatorSetup.setParameter()");
    }
    env->DeleteGlobalRef(jname);
    env->DeleteGlobalRef(jvalue);
}

/**
   Set the number of input or output ports.
 */
void OpSetupInvoker::setPortCount(JNIEnv* env, jobject setup, bool output, jint count)
{
    jmethodID method = output ? _setNumberOfOutputPortsMethod : _setNumberOfInputPortsMethod;
    env->CallVoidMethod(setup, method, count);

    if (env->ExceptionCheck()) {
        // Internal error if this can't be called
        JAVAOP_THROW(env, "OperatorSetup.setPortCount()");
    }
}

/**
   Set a port's meta-data
 */
void OpSetupInvoker::setPort(JNIEnv* env,
                             jobject setup,
                             bool output,
                             OperatorPort& port,
                             jint typeOrdinal)
{
    TupleType const& tuple = dynamic_cast<TupleType const&>(port.getTupleType());

    // Set the position, name and attribute count
    jmethodID method = output ? _setOutputPortMethod : _setInputPortMethod;
    jstring jname = JVMControl::javaString(env, port.getName());
    env->CallVoidMethod(setup, method, (jint)port.getIndex(), typeOrdinal, jname,
                        (jint)tuple.getNumberOfAttributes());
    if (env->ExceptionCheck()) {
        env->DeleteGlobalRef(jname);
        // Internal error if this can't be called
        JAVAOP_THROW(env, "OperatorSetup.setPort()");
    }
    env->DeleteGlobalRef(jname);

    // Set the type and name for each attribute
    method = output ? _setOutputPortAttributeMethod : _setInputPortAttributeMethod;
    uint32_t ai;
    for (ai = 0; ai < tuple.getNumberOfAttributes(); ai++) {
        jstring jatype = JVMControl::javaString(env, tuple.getAttributeType(ai).getName().c_str());
        jstring janame = JVMControl::javaString(env, tuple.getAttributeName(ai).c_str());

        env->CallVoidMethod(setup, method, (jint)port.getIndex(), (jint)ai, jatype, janame);
        if (env->ExceptionCheck()) {
            env->DeleteGlobalRef(jatype);
            env->DeleteGlobalRef(janame);
            // Internal error if this can't be called
            JAVAOP_THROW(env, "OperatorSetup.setPortAttribute()");
        }
        env->DeleteGlobalRef(jatype);
        env->DeleteGlobalRef(janame);
    }
}

/**
   Create a global reference to a Java String object from a C++ string object.
 */
jstring JVMControl::javaString(JNIEnv* env, const std::string& value)
{
    return JVMControl::javaString(env, value.c_str());
}

/**
   Convert a null-terminated UTF-8 character sequence into an icu:UnicodeString
   and then create a Java String object from that to be passed into the Java
   code through JNI. The returned reference is a global reference.
   UnicodeString, Java strings & JNI NewString strings all use UTF16
   UChar is equivalent to jchar
 */
jstring JVMControl::javaString(JNIEnv* env, const char* value)
{
    UnicodeString us = icu::UnicodeString(value, "UTF-8");
    jstring lref = env->NewString((jchar*)us.getBuffer(), (jsize)us.length());
    jstring gref = (jstring)env->NewGlobalRef(lref);
    env->DeleteLocalRef(lref);
    return gref;
}

/**
   Handler to provide access to Java operator metrics from the C++ operator context.
   Custom metrics for a JavaOp operator are stored in Java.
 */
class JavaOpMetricsHandler : public SPL::OperatorMetricsHandler
{
  public:
    JavaOpMetricsHandler(JNIBridgeInvoker* bi, jobject bridge)
    {
        _bi = bi;
        _bridge = bridge;
    }

    // Custom metrics access
    void createCustomMetric(std::string const& name,
                            std::string const& description,
                            Metric::Kind kind,
                            int64_t value)
    {
        _bi->createCustomMetric(_bridge, name, description, kind, value);
    }
    int64_t getCustomMetricValue(std::string const& name)
    {
        return _bi->getCustomMetricValue(_bridge, name);
    }
    void setCustomMetricValue(std::string const& name, int64_t value)
    {
        return _bi->setCustomMetricValue(_bridge, name, value);
    }
    bool hasCustomMetric(std::string const& name) { return _bi->hasCustomMetric(_bridge, name); }
    std::vector<std::string> getCustomMetricNames() { return _bi->getCustomMetricNames(_bridge); }

  private:
    JNIBridgeInvoker* _bi;
    jobject _bridge;
};

/**
   Construct the class used to invoke methods on an JNIBridge instance.
   This class is shared across multiple operators.
 */
JNIBridgeInvoker::JNIBridgeInvoker(JavaVM* jvm, JNIEnv* env)
{
    _jvm = jvm;

    // Everything goes through the JNIBridge class between C++ & Java
    // for runtime operators
    JClass jnibridge(env, "com/ibm/streams/operator/internal/jni/JNIBridge");
    /*
    if(env->ExceptionCheck()) {
        JAVAOP_THROW(env, "class JNIBridge"); // Internal error if this can't be found
    }
*/

    _jnibridge = reinterpret_cast<jclass>(env->NewGlobalRef(jnibridge.get()));
    //  _jnibridge = (jclass) env->NewGlobalRef(jnibridge);

    // General management and stream handling methods for the operator.
    _constructor = jnibridge.getMethodID(
      "<init>", "(JILcom/ibm/streams/operator/internal/runtime/api/OperatorSetup;)V");
    _allPortsReadyMethod = jnibridge.getMethodID("allPortsReady", "()V");
    _completeMethod = jnibridge.getMethodID("complete", "()V");
    _processMethod = jnibridge.getMethodID("process", "(ILjava/nio/ByteBuffer;)V");
    _punctMethod = jnibridge.getMethodID("process", "(II)V");
    _watermarkMethod = jnibridge.getMethodID("processWatermark", "(IJIII)V");
    _getWatermarkMethod =
      jnibridge.getMethodID("getWatermark", "()Lcom/ibm/streams/operator/types/Timestamp;");

    _shutdownMethod = jnibridge.getMethodID("shutdown", "()V");

    // Consistent region punctuation handling method for the operator.
    _actionMethod = jnibridge.getMethodID("action", "(II)V");

    // State handler methods for consistent region handling for the operator.
    _drainMethod = jnibridge.getMethodID("drain", "()V");
    _checkpointMethod = jnibridge.getMethodID("checkpoint", "(JJ)V");
    _resetMethod = jnibridge.getMethodID("reset", "(JJ)V");
    _resetToInitialStateMethod = jnibridge.getMethodID("resetToInitialState", "()V");
    _retireCheckpointMethod = jnibridge.getMethodID("retireCheckpoint", "(J)V");
    _prepareForNonBlockingCheckpointMethod =
      jnibridge.getMethodID("prepareForNonBlockingCheckpoint", "(J)V");
    _regionCheckpointedMethod = jnibridge.getMethodID("regionCheckpointed", "(J)V");

    // Method for consistent region non-blocking checkpointing support
    _blockConsistentRegionPermitMethod =
      jnibridge.getMethodID("blockConsistentRegionPermit", "()V");

    // Method for consistent region resume processing support
    _releaseConsistentRegionPermitMethod =
      jnibridge.getMethodID("releaseConsistentRegionPermit", "()V");

    // Methods for custom metrics access
    _createCustomMetricMethod =
      jnibridge.getMethodID("createCustomMetric", "(Ljava/lang/String;Ljava/lang/String;IJ)V");
    _getCustomMetricValueMethod =
      jnibridge.getMethodID("getCustomMetricValue", "(Ljava/lang/String;)J");
    _setCustomMetricValueMethod =
      jnibridge.getMethodID("setCustomMetricValue", "(Ljava/lang/String;J)V");
    _hasCustomMetricMethod = jnibridge.getMethodID("hasCustomMetric", "(Ljava/lang/String;)Z");
    _getCustomMetricNamesMethod =
      jnibridge.getMethodID("getCustomMetricNames", "()[Ljava/nio/ByteBuffer;");
}

/**
   Create a JNIBridge for runtime interaction with the user's
   Java Operator implementation. Operator.initialize() will
   be called by this method (in the Java code through the
   JNIBridge constructor).
   Once this method returns the caller should discard the
   setup reference as it will be invalid.
   Returns a global reference to the JNIBridge instance.
 */
jobject JNIBridgeInvoker::newBridge(JNIEnv* env, Operator* op, jobject setup)
{
    jlong opPtr = (jlong)op;

    // Create an instance of JNIBridge
    // new JNIBridge(opPtr, setup)
    APPTRC(L_DEBUG, "before calling JNIBridge.<init>()", SPL_JAVAOP_DBG);
    jobject localBridge =
      env->NewObject(_jnibridge, _constructor, opPtr, (jint)op->getIndex(), setup);
    APPTRC(L_DEBUG, "after calling JNIBridge.<init>()", SPL_JAVAOP_DBG);

    if (env->ExceptionCheck()) {
        env->DeleteGlobalRef(setup);
        JAVAOP_THROW(env, "Operator.initialize()");
    }

    env->DeleteGlobalRef(setup);
    jobject bridge = env->NewGlobalRef(localBridge);
    env->DeleteLocalRef(localBridge);

    // Register the operator metrics handler to allow access to the Java operator metrics from the C++ operator context.
    op->getOperatorImpl().getContextImpl().registerOperatorMetricsHandler(
      new JavaOpMetricsHandler(this, bridge));

    return bridge;
}

/**
   Call JNIBridge.allPortsReady() which will in turn call Operator.allPortsReady()
 */
void JNIBridgeInvoker::allPortsReady(jobject bridge)
{
    EnvUnion env;
    _jvm->AttachCurrentThread(&env.rawPtr, NULL);
    env.envPtr->CallVoidMethod(bridge, _allPortsReadyMethod);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "Operator.allPortsReady()");
    }
    _jvm->DetachCurrentThread();
}

/**
   Call JNIBridge.complete() to wait for asynchronous work
   started by allPortsReady() to complete.
 */
void JNIBridgeInvoker::complete(jobject bridge)
{
    EnvUnion env;
    _jvm->AttachCurrentThread(&env.rawPtr, NULL);
    env.envPtr->CallVoidMethod(bridge, _completeMethod);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "Operator.allPorts() asynchronous");
    }
    _jvm->DetachCurrentThread();
}

/**
   Call JNIBridge.shutdown() which will in turn call Operator.shutdown()
 */
void JNIBridgeInvoker::shutdown(jobject bridge)
{
    EnvUnion env;
    _jvm->AttachCurrentThread(&env.rawPtr, NULL);
    env.envPtr->CallVoidMethod(bridge, _shutdownMethod);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "Operator.shutdown()");
    }
    _jvm->DetachCurrentThread();
}

/**
   Call JNIBridge.process() which will in turn call Operator.processPunctuation()
   See processTuple for notes on attaching of threads to the JVM
 */
void JNIBridgeInvoker::processPunctuation(jobject bridge, Punctuation const& mark, uint32_t port)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _punctMethod, (jint)port, (jint)mark.getValue());
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "Operator.processPunctuation()");
    }
}

/**
   Call JNIBridge.processWatermark() which will in turn call Operator.processWatermark()
   See processTuple for notes on attaching of threads to the JVM
 */
void JNIBridgeInvoker::processWatermark(jobject bridge, Punctuation const& punct, uint32_t port)
{
    WatermarkPunctPayload* payload = WatermarkPunctPayload::getPayload(punct);
    uint32_t operatorId = payload->getOperatorInstanceId();
    uint32_t oportId = payload->getOperatorOPortIdx();
    timestamp wm = payload->getValue();

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _watermarkMethod, (jint)port, (jlong)wm.getSeconds(),
                               (jint)wm.getNanoseconds(), (jint)operatorId, (jint)oportId);
    if (env.envPtr->ExceptionCheck()) {
        // TODO real exception
        JAVAOP_THROW(env.envPtr, "Operator.processWatermark()");
    }
}

/**
 * Call JNIBridge.getWatermark to get the current watermark value, then
 * call getSeconds and getNanoseconds on the returned Java Timestamp object
 * and return a timestamp.
 */
SPL::timestamp JNIBridgeInvoker::watermark(jobject bridge)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    jobject ts = env.envPtr->CallObjectMethod(bridge, _getWatermarkMethod);
    if (env.envPtr->ExceptionCheck()) {
        // TODO real exception
        JAVAOP_THROW(env.envPtr, "Operator.watermark()");
    }

    int64_t seconds = 0;
    uint32_t nanoseconds = 0;

    jclass timestampClass = env.envPtr->GetObjectClass(ts);
    jmethodID secondsMethod = env.envPtr->GetMethodID(timestampClass, "getSeconds", "()J");

    seconds = env.envPtr->CallLongMethod(timestampClass, secondsMethod);
    if (env.envPtr->ExceptionCheck()) {
        // TODO real exception
        JAVAOP_THROW(env.envPtr, "Timestamp.getSeconds()");
    }

    jmethodID nanosMethod = env.envPtr->GetMethodID(timestampClass, "getNanoseconds", "()I");
    nanoseconds = env.envPtr->CallIntMethod(ts, nanosMethod);
    if (env.envPtr->ExceptionCheck()) {
        // TODO real exception
        JAVAOP_THROW(env.envPtr, "Timestamp.getNanoeconds()");
    }

    return SPL::timestamp(seconds, nanoseconds);
}

/**
   Call JNIBridge.process() which will in turn call Operator.process()
   through the private processTuple() function.
 */
void JNIBridgeInvoker::processTuple(jobject bridge, Tuple const& tuple, uint32_t port)
{
    void const* rawTupleData;
    unsigned int tupleSize = tuple.getSerializedSize();

    if (tuple.isFacade()) {
        rawTupleData = tuple.getSerializedDataPtr();
        processTuple(bridge, rawTupleData, tupleSize, port);
    } else {
        NativeByteBuffer buf(tupleSize);
        tuple.serialize(buf);
        processTuple(bridge, buf.getPtr(), tupleSize, port);
    }
}

void JNIBridgeInvoker::processTuple(jobject bridge, NativeByteBuffer& buf, uint32_t port)
{
    // Since we are receiving a native byte buffer, we dont need to do any processing and
    // can pass it directly to the JNI side
    processTuple(bridge, buf.getPtr(), buf.getSerializedDataSize(), port);
}

extern "C" void callProcessTupleWithNativeByteBuffer(JNIBridgeInvoker& bi,
                                                     jobject bridge,
                                                     NativeByteBuffer& buffer,
                                                     uint32_t port)
{
    bi.processTuple(bridge, buffer, port);
}

/**
  Pass a tuple to be processed off to the Java
  Operator runtime via JNIBridge. As required by JNI
  the JVM is attached to first, which is a no-op if the
  thread is already attached. This will be the case when
  the source of the tuple submission:
     - came from a java thread (in a Java operator)
     - came from a C++ thread that has previously submitted
       a tuple or punctuation mark (see below)

  The local reference to the byte buffer is destroyed
  explicitly because this method may be called directly from
  C++ and local references are only automatically destroyed
  when a native (C++) method returns to Java.

  The thread is left attached to the JVM as:
        1) Detaching it is the incorrect action if
           the thread was already attached upon entry
           (see above) as it leaves the JVM in a confused state.
           E.g. when there is a pipeline of C++ and Java operators
           with the source being a C++ operator then detaching
           every tuple submission would result in the last operator
           in the pipeline detaching from the JVM while the
           thread still has to travsere back through JVM code.

        2) For source C++ threads most likely the thread will
           be used to submit future tuples/punctuation to
           a Java operator so it's more performant to
           attach once. Such source C++ threads are attached
           as daemons to avoid the JVM waiting for them
           to complete at shutdown.

    Only processing tuples and punctuation into Java operators
    can result in nested Java/C++ stacks due to operator
    pipelining. All other calls from C++ into Java
    are driven by C++ source threads and thus are safe to use pairs of
    attach/detach calls as non-daemon status.
*/
void JNIBridgeInvoker::processTuple(jobject bridge,
                                    void const* rawTupleData,
                                    unsigned int tupleSize,
                                    uint32_t port)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    jobject raw = env.envPtr->NewDirectByteBuffer(const_cast<void*>(rawTupleData), tupleSize);

    if (raw == NULL) {
        if (env.envPtr->ExceptionCheck()) {
            env.envPtr->ExceptionDescribe();
            env.envPtr->ExceptionClear();
        }
        THROW(SPLRuntime, "Buffer cannot be allocated. Make sure that total tuple size of "
                            << tupleSize << " is less than " << std::numeric_limits<int>::max());
    }

    env.envPtr->CallVoidMethod(bridge, _processMethod, (jint)port, raw);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "Operator.process()");
    }
    env.envPtr->DeleteLocalRef(raw);
}

/**
   Consistent region handling.
 */
void JNIBridgeInvoker::drain(jobject bridge)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _drainMethod);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "StateHandler.drain()");
    }
}

void JNIBridgeInvoker::action(jobject bridge, Punctuation const& punct, uint32_t port)
{
    jint action;
    switch (punct) {
        case Punctuation::DrainMarker:
            action = 0;
            break;
        case Punctuation::ResetMarker:
            action = 1;
            break;
        case Punctuation::ResumeMarker:
            action = 2;
            break;
        case Punctuation::SwitchMarker:
            action = 6;
            break;
        case Punctuation::WatermarkMarker:
            processWatermark(bridge, punct, port);
            return;
        default:
            return;
    }

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _actionMethod, (jint)port, action);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "JNIBridge.action()");
    }
}

void JNIBridgeInvoker::checkpoint(jobject bridge, jlong ckptPtr, jlong key)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _checkpointMethod, ckptPtr, key);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "StateHandler.checkpoint()");
    }
}
void JNIBridgeInvoker::retireCheckpoint(jobject bridge, jlong key)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _retireCheckpointMethod, key);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "StateHandler.retireCheckpoint()");
    }
}
void JNIBridgeInvoker::reset(jobject bridge, jlong ckptPtr, jlong key)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _resetMethod, ckptPtr, key);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "StateHandler.reset()");
    }
}
void JNIBridgeInvoker::resetToInitialState(jobject bridge)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _resetToInitialStateMethod);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "StateHandler.resetToInitialState()");
    }
}
void JNIBridgeInvoker::prepareForNonBlockingCheckpoint(jobject bridge, jlong key)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _prepareForNonBlockingCheckpointMethod, key);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "StateHandler.prepareForNonBlockingCheckpoint()");
    }
}

void JNIBridgeInvoker::regionCheckpointed(jobject bridge, jlong key)
{

    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    env.envPtr->CallVoidMethod(bridge, _regionCheckpointedMethod, key);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "StateHandler.regionCheckpointed()");
    }
}

void JNIBridgeInvoker::blockConsistentRegionPermit(jobject bridge)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    APPTRC(L_DEBUG, "Calling void method ", SPL_JAVART_DBG);
    env.envPtr->CallVoidMethod(bridge, _blockConsistentRegionPermitMethod);
    APPTRC(L_DEBUG, "Called void method ", SPL_JAVART_DBG);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "ConsistentRegionContextImpl.blockPermit()");
    }
}

void JNIBridgeInvoker::releaseConsistentRegionPermit(jobject bridge)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);

    APPTRC(L_DEBUG, "Calling void method ", SPL_JAVART_DBG);
    env.envPtr->CallVoidMethod(bridge, _releaseConsistentRegionPermitMethod);
    APPTRC(L_DEBUG, "Called void method ", SPL_JAVART_DBG);
    if (env.envPtr->ExceptionCheck()) {
        JAVAOP_THROW(env.envPtr, "ConsistentRegionContextImpl.unblockPermit()");
    }
}

/**
   Access from C++ to JavaOp custom metrics.
   Custom metrics for a JavaOp operator are stored in Java.
 */
void JNIBridgeInvoker::createCustomMetric(jobject bridge,
                                          string const& name,
                                          string const& description,
                                          Metric::Kind kind,
                                          int64_t value)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);
    jstring jname = JVMControl::javaString(env.envPtr, name);
    jstring jdesc = JVMControl::javaString(env.envPtr, description);
    env.envPtr->CallVoidMethod(bridge, _createCustomMetricMethod, jname, jdesc, (jint)kind,
                               (jlong)value);
    env.envPtr->DeleteGlobalRef(jname);
    env.envPtr->DeleteGlobalRef(jdesc);
    if (env.envPtr->ExceptionCheck()) {
        if (hasCustomMetric(bridge, name)) {
            // Rethrow using C++ operator API.
            env.envPtr->ExceptionClear();
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidMetric, L_ERROR, SPL_RUNTIME_EXISTING_METRIC(name),
                                SPL_OPER_DBG);
        } else {
            // This should not happen.
            JAVAOP_THROW(env.envPtr, "Operator.createCustomMetric()");
        }
    }
}

int64_t JNIBridgeInvoker::getCustomMetricValue(jobject bridge, string const& name)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);
    jstring jname = JVMControl::javaString(env.envPtr, name);
    jlong jvalue = env.envPtr->CallLongMethod(bridge, _getCustomMetricValueMethod, jname);
    env.envPtr->DeleteGlobalRef(jname);
    if (env.envPtr->ExceptionCheck()) {
        if (!hasCustomMetric(bridge, name)) {
            // Rethrow using C++ operator API.
            env.envPtr->ExceptionClear();
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidMetric, L_ERROR, SPL_RUNTIME_UNKNOWN_METRIC(name),
                                SPL_OPER_DBG);
        } else {
            // This should not happen.
            JAVAOP_THROW(env.envPtr, "Operator.getCustomMetricValue()");
        }
    }
    return (int64_t)jvalue;
}

void JNIBridgeInvoker::setCustomMetricValue(jobject bridge, string const& name, int64_t value)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);
    jstring jname = JVMControl::javaString(env.envPtr, name);
    env.envPtr->CallVoidMethod(bridge, _setCustomMetricValueMethod, jname, (jlong)value);
    env.envPtr->DeleteGlobalRef(jname);
    if (env.envPtr->ExceptionCheck()) {
        if (!hasCustomMetric(bridge, name)) {
            // Rethrow using C++ operator API.
            env.envPtr->ExceptionClear();
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidMetric, L_ERROR, SPL_RUNTIME_UNKNOWN_METRIC(name),
                                SPL_OPER_DBG);
        } else {
            // This should not happen.
            JAVAOP_THROW(env.envPtr, "Operator.setCustomMetricValue()");
        }
    }
}

bool JNIBridgeInvoker::hasCustomMetric(jobject bridge, string const& name)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);
    jstring jname = JVMControl::javaString(env.envPtr, name);
    jboolean jres = env.envPtr->CallBooleanMethod(bridge, _hasCustomMetricMethod, jname);
    env.envPtr->DeleteGlobalRef(jname);
    if (env.envPtr->ExceptionCheck()) {
        // This should not happen.
        JAVAOP_THROW(env.envPtr, "Operator.hasCustomMetric()");
    }
    return (bool)jres;
}

vector<string> JNIBridgeInvoker::getCustomMetricNames(jobject bridge)
{
    EnvUnion env;
    _jvm->AttachCurrentThreadAsDaemon(&env.rawPtr, NULL);
    jobjectArray jnames =
      static_cast<jobjectArray>(env.envPtr->CallObjectMethod(bridge, _getCustomMetricNamesMethod));
    if (env.envPtr->ExceptionCheck()) {
        // This should not happen.
        JAVAOP_THROW(env.envPtr, "Operator.getCustomMetricNames()");
    }
    jsize size = env.envPtr->GetArrayLength(jnames);
    vector<string> names;
    for (jsize i = 0; i < size; i++) {
        jobject nb = env.envPtr->GetObjectArrayElement(jnames, i);
        string name((char*)(env.envPtr->GetDirectBufferAddress(nb)));
        names.push_back(name);
        env.envPtr->DeleteLocalRef(nb);
    }
    env.envPtr->DeleteLocalRef(jnames);
    return names;
}

/**
   Get a handle to the Java virtual machine.
   The current thread is not attached to the JVM when this returns.
 */
JVMControl* JVMControl::getJVM()
{
    // Find the JVM.
    JavaVM* jvm;
    EnvUnion env;
    jvm = (JavaVM*)SPL::ProcessingElement::pe().getJavaVM();
    if (NULL == jvm) {
        THROW(SPLRuntime, "pe().getJavaVM() returned NULL");
    }
    APPTRC(L_DEBUG, "Found PE Java VM", SPL_JAVART_DBG);
    jvm->AttachCurrentThread(&env.rawPtr, NULL);

    JVMControl* control = new JVMControl(jvm, env.envPtr);
    control->detach();
    return control;
}

/**
   Create the singleton classes used to invoke methods on the Java objects.

   Note: currently these are not singleton classes (probably they should be).
   There is one for each Java operator.
 */
JVMControl::JVMControl(JavaVM* jvm, JNIEnv* env)
{
    _jvm = jvm;
    _opSetup = new OpSetupInvoker(jvm, env);
    _bridge = new JNIBridgeInvoker(jvm, env);
}

/**
   Attach the current thread to the JVM.
 */
JNIEnv* JVMControl::attach()
{
    EnvUnion env;
    _jvm->AttachCurrentThread(&env.rawPtr, NULL);
    return env.envPtr;
}

/**
   Detach the current thread from the JVM.
 */
void JVMControl::detach()
{
    _jvm->DetachCurrentThread();
}

/*
 * State Handler interface
 */

JavaOpStateHandler::JavaOpStateHandler(JNIBridgeInvoker* bridge, jobject bridgeOp)
{
    _bridge = bridge;
    _bridgeOp = bridgeOp;
}

void JavaOpStateHandler::drain()
{
    _bridge->drain(_bridgeOp);
}
void JavaOpStateHandler::checkpoint(Checkpoint& ckpt)
{
    _bridge->checkpoint(_bridgeOp, (jlong)&ckpt, (jlong)ckpt.getSequenceId());
}
void JavaOpStateHandler::reset(Checkpoint& ckpt)
{
    _bridge->reset(_bridgeOp, (jlong)&ckpt, ckpt.getSequenceId());
}
void JavaOpStateHandler::resetToInitialState()
{
    _bridge->resetToInitialState(_bridgeOp);
}
void JavaOpStateHandler::retireCheckpoint(int64_t key)
{
    _bridge->retireCheckpoint(_bridgeOp, (jlong)key);
}
void JavaOpStateHandler::prepareForNonBlockingCheckpoint(int64_t key)
{
    _bridge->prepareForNonBlockingCheckpoint(_bridgeOp, (jlong)key);
}
void JavaOpStateHandler::regionCheckpointed(int64_t key)
{
    _bridge->regionCheckpointed(_bridgeOp, (jlong)key);
}
