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

#ifndef SPL_JAVAOP_H
#define SPL_JAVAOP_H

#include <jni.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/Metric.h>
#include <SPL/Runtime/Operator/State/StateHandler.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPort.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPort.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Type/String.h>

#define JAVAOP_THROW(env, where)                       \
    { env->ExceptionDescribe(); env->ExceptionClear(); \
      SPLTRACEMSGANDTHROW(SPLRuntimeJavaOperator, L_ERROR, SPL_APPLICATION_RUNTIME_JAVAOP_EXCEPTION((where)), SPL_FUNC_DBG); \
    }

namespace SPL
{
    namespace JNI
    {
        // Avoid type-punned pointer alias warnings
        union EnvUnion
        {
            JNIEnv * envPtr;
            void * rawPtr;
        };

        class LogTrace
        {
        public:
           static void trace(jint level, char * msg, char * aspects);
           static void log(jint level, char * msg, char * aspects);
        };

        class DLL_PUBLIC JNIBridgeInvoker
        {
        public:
            JNIBridgeInvoker(JavaVM * jvm, JNIEnv * env);

            /** Return an new JNIBridge to communicate with the Java operator */
            jobject newBridge(JNIEnv * env, SPL::Operator * op, jobject setup);

            /** Call Operator.allPortsReady() through the specific bridge */
            void allPortsReady(jobject bridge);

            /** Call OperatorAdapter.complete() through the specific bridge */
            void complete(jobject bridge);

            /** Call Operator.process() through the specific bridge */
            void processTuple(jobject bridge,
                 Tuple const & tuple, uint32_t port);

            void processTuple(jobject bridge,
                             NativeByteBuffer & buffer, uint32_t port);

            /** Call Operator.processPunctuation() through the specific bridge */
            void processPunctuation(jobject bridge,
                 Punctuation const & mark, uint32_t port);

            /** Call Operator.processWatermark() through the specified bridge */
            void processWatermark(jobject bridge,
                 Punctuation const & punct,
                 uint32_t port);

            /** Get the watermark for the operator */
            timestamp watermark(jobject bridge);

            /** Consistent region punctuation handling */
            void action(jobject bridge,
                 Punctuation const & mark, uint32_t port);

            /** Consistent region StateHandler support */
            void drain(jobject bridge);
            void checkpoint(jobject bridge, jlong ckptPtr, jlong key);
            void reset(jobject bridge, jlong ckptPtr, jlong key);
            void resetToInitialState(jobject bridge);
            void retireCheckpoint(jobject bridge, jlong key);
            void prepareForNonBlockingCheckpoint(jobject bridge, jlong key);
            void regionCheckpointed(jobject bridge, jlong key);

            /* ConsistentRegion semaphore support for non-blocking checkpointing */
            void blockConsistentRegionPermit(jobject bridge);

            /* ConsistentRegion semaphore support for resume processing */
            void releaseConsistentRegionPermit(jobject bridge);

            /** Call Operator.shutdown() through the specific bridge */
            void shutdown(jobject bridge);

            /** Custom metrics access */
            void createCustomMetric(jobject bridge,
                                    std::string const & name,
                                    std::string const & description,
                                    Metric::Kind kind,
                                    int64_t value);
            int64_t getCustomMetricValue(jobject bridge, std::string const & name);
            void setCustomMetricValue(jobject bridge, std::string const & name, int64_t value);
            bool hasCustomMetric(jobject bridge, std::string const & name);
            std::vector<std::string> getCustomMetricNames(jobject bridge);

        private:

            /* My JVM */
            JavaVM * _jvm;

            jclass _jnibridge;

            // The method id for JNIBridge constructor
            jmethodID _constructor;

            // The method id for JNIBridge.allPortsReady
            jmethodID _allPortsReadyMethod;

            // The method id for JNIBridge.complete
            jmethodID _completeMethod;

            // The method id for JNIBridge.process for tuples
            jmethodID _processMethod;

            // The method id for JNIBridge.process for punctuation
            jmethodID _punctMethod;

            // The method id for JNIBridge.processWatermark
            jmethodID _watermarkMethod;

            // The method id for JNIBridge.watermark
            jmethodID _getWatermarkMethod;

            // The method id for JNIBridge.shutdownOperator
            jmethodID _shutdownMethod;

            // Method for consistent region punctuation handling
            jmethodID _actionMethod;

            // Methods for consistent region StateHandler support
            jmethodID _drainMethod;
            jmethodID _checkpointMethod;
            jmethodID _resetMethod;
            jmethodID _resetToInitialStateMethod;
            jmethodID _retireCheckpointMethod;
            jmethodID _prepareForNonBlockingCheckpointMethod;
            jmethodID _regionCheckpointedMethod;

            // Method for consistent region non-blocking checkpointing support
            jmethodID _blockConsistentRegionPermitMethod;

            // Method for consistent region resume processing support
            jmethodID _releaseConsistentRegionPermitMethod;

            // Methods for custom metrics access
            jmethodID _createCustomMetricMethod;
            jmethodID _getCustomMetricValueMethod;
            jmethodID _setCustomMetricValueMethod;
            jmethodID _hasCustomMetricMethod;
            jmethodID _getCustomMetricNamesMethod;

            /** Call Operator.process() through the specific bridge */
            void processTuple(jobject bridge,
                 void const * rawData, unsigned int rawSize, uint32_t port);
        };

extern "C" {
   void callProcessTupleWithNativeByteBuffer(JNIBridgeInvoker & bi, jobject bridge, NativeByteBuffer & buffer, uint32_t port);
}
        /**
           Class that interfaces with the Java class OperatorSetup used
           during setup of the JavaOp. OperatorSetup contains all the
           static information about the environment the Java Operator
           implementation will run in. Once complete a JNIBridge is
           created using the OperatorSetup instance.
           This class may be shared across multiple C++ operators,
           it is used with a specific OperatorSetup for each instance,
           created by newSetup().
         */
        class DLL_PUBLIC OpSetupInvoker
        {
        public:
            /** Construct the class for use within the given JVM */
            OpSetupInvoker(JavaVM * jvm, JNIEnv * env);

            /** Return an instance of OperatorSetup for use by a specific C++ operator */
            jobject newSetup(JNIEnv * env, SPL::Operator * op);

            /** Set a JavaOp parameter */
            void setParameter(JNIEnv * env, jobject setup, const char * name, const std::string & value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, const SPL::ustring & value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, const char * value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, bool value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, int32_t value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, uint64_t value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, int64_t value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, double value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, const SPL::decimal32 & value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, const SPL::decimal64 & value);
            void setParameter(JNIEnv * env, jobject setup, const char * name, const SPL::decimal128 & value);


        private:
            /* My JVM */
            JavaVM * _jvm;

            /** Global reference to the OperatorSetup class object */
            jclass _opSetup;

            /** The method id for OperatorSetup constructor */
            jmethodID _constructor;

            /** The method id for OperatorSetuup.setParameter */
            jmethodID _setParameterMethod;

            /** Method identifiers to set up input ports */
            jmethodID _setNumberOfInputPortsMethod;
            jmethodID _setInputPortMethod;
            jmethodID _setInputPortAttributeMethod;

            /** Method identifiers to set up output ports */
            jmethodID _setNumberOfOutputPortsMethod;
            jmethodID _setOutputPortMethod;
            jmethodID _setOutputPortAttributeMethod;

            /** Set the number of input or output ports */
            void setPortCount(JNIEnv * env, jobject setup, bool output, jint count);

            /** Set the port's meta-data */
            void setPort(JNIEnv * env, jobject setup, bool output,
                  OperatorPort & port, jint typeOrdinal);

        };

        class DLL_PUBLIC JVMControl
        {
        public:
            static JVMControl * getJVM();

            /** Attach and detach the current thread from the VM */
            JNIEnv * attach();
            void detach();

            OpSetupInvoker * getOpSetupInvoker()
            {
                return _opSetup;
            }
            JNIBridgeInvoker * getBridgeInvoker()
            {
                return _bridge;
            }

            /** Create a Java String from a std::string */
            static jstring javaString(JNIEnv * env, const std::string & value);

            /** Create a Java String from a UTF-8 char * value */
            static jstring javaString(JNIEnv * env, const char * value);

        private:
            JVMControl(JavaVM * jvm, JNIEnv * env);
            JavaVM * _jvm;
            OpSetupInvoker * _opSetup;
            JNIBridgeInvoker * _bridge;


        };

        class DLL_PUBLIC JavaOpStateHandler : public SPL::StateHandler
        {
            public:
                 JavaOpStateHandler(JNIBridgeInvoker * bridge, jobject bridgeOp);
                 void drain();
                 void checkpoint(Checkpoint & ckpt);
                 void reset(Checkpoint & ckpt);
                 void retireCheckpoint(int64_t id);
                 void resetToInitialState();
                 void prepareForNonBlockingCheckpoint(int64_t id);
                 void regionCheckpointed(int64_t id);

            private:
                jobject _bridgeOp;
                JNIBridgeInvoker *_bridge;
        };
    };
};

#endif /* SPL_JAVAOP_H */
