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

#include "LogicalToPhysicalJNI.h"
#include "LogicalToPhysical.h"
#include "SAMHelperFunctions.h"
#include <UTILS/DistilleryException.h>
#include <exception>
#include <string>

UTILS_NAMESPACE_USE

/*
 * Class:     com_ibm_streams_spl_internal_api_port_LogicalToPhysicalJNI
 * Method:    submissionTimeExpressionEvalCpp
 * Signature: (Ljava/lang/String;JJ)Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL
Java_com_ibm_streams_spl_internal_api_port_LogicalToPhysicalJNI_submissionTimeExpressionEvalCpp(
  JNIEnv* env,
  jobject jobj,
  jstring jExpression,
  jlong maxChannels,
  jlong channelIndex,
  jobject errorMessageBuffer)
{
    jstring ret;
    std::string errorMessage;
    char const* expression = env->GetStringUTFChars(jExpression, NULL);
    std::string result;
    try {
        result = SAM_NAMESPACE::LogicalToPhysical::Transformer::performIntrinsicFunctionEval(
          std::string(expression), maxChannels, channelIndex);
    } catch (const DistilleryException& e) {
        errorMessage = e.toXMLString();
    } catch (...) {
        errorMessage = "UNKNOWN_EXCEPTION";
    }

    if (!errorMessage.empty() && errorMessageBuffer != 0) {
        jclass clazz = env->GetObjectClass(errorMessageBuffer);
        jmethodID mid =
          env->GetMethodID(clazz, "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
        jstring value = env->NewStringUTF(errorMessage.c_str());
        env->CallObjectMethod(errorMessageBuffer, mid, value);
    }

    ret = env->NewStringUTF(result.c_str());
    env->ReleaseStringUTFChars(jExpression, expression);

    return ret;
}

/*
 * Class:     com_ibm_streams_spl_internal_api_port_LogicalToPhysicalJNI
 * Method:    submissionTimePropertiesEvalCpp
 * Signature: (Ljava/lang/String;JJ)Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL
Java_com_ibm_streams_spl_internal_api_port_LogicalToPhysicalJNI_submissionTimePropertiesEvalCpp(
  JNIEnv* env,
  jobject jobj,
  jstring jExpression,
  jlong maxChannels,
  jlong channelIndex,
  jobject errorMessageBuffer)
{
    jstring ret;
    std::string errorMessage;
    char const* expression = env->GetStringUTFChars(jExpression, NULL);
    std::string result;
    try {
        result = SAM_NAMESPACE::LogicalToPhysical::Transformer::submissionTimePropertiesEval(
          std::string(expression), maxChannels, channelIndex);
    } catch (const DistilleryException& e) {
        errorMessage = e.toXMLString();
    } catch (...) {
        errorMessage = "UNKNOWN_EXCEPTION";
    }

    if (!errorMessage.empty() && errorMessageBuffer != 0) {
        jclass clazz = env->GetObjectClass(errorMessageBuffer);
        jmethodID mid =
          env->GetMethodID(clazz, "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
        jstring value = env->NewStringUTF(errorMessage.c_str());
        env->CallObjectMethod(errorMessageBuffer, mid, value);
    }

    ret = env->NewStringUTF(result.c_str());
    env->ReleaseStringUTFChars(jExpression, expression);

    return ret;
}

/*
 * Class:     com_ibm_streams_spl_internal_api_port_LogicalToPhysicalJNI
 * Method:    transformCpp
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_ibm_streams_spl_internal_api_port_LogicalToPhysicalJNI_transformCpp(
  JNIEnv* env,
  jobject jobj,
  jstring jadlStr,
  jobject errorMessageBuffer)
{

    jstring ret;
    std::string errorMessage;
    char const* adlStr = env->GetStringUTFChars(jadlStr, NULL);
    std::string result;

    try {
        ADL::applicationType logicalApp;
        SAM_NAMESPACE::fromString(logicalApp, std::string(adlStr));
        std::auto_ptr<ADL::applicationType> physApp =
          SAM_NAMESPACE::LogicalToPhysical::transform(logicalApp);
        result = SAM_NAMESPACE::toString(*physApp);
    } catch (const DistilleryException& e) {
        errorMessage = e.toXMLString();
    } catch (...) {
        errorMessage = "UNKNOWN_EXCEPTION";
    }

    if (!errorMessage.empty() && errorMessageBuffer != 0) {
        jclass clazz = env->GetObjectClass(errorMessageBuffer);
        jmethodID mid =
          env->GetMethodID(clazz, "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
        jstring value = env->NewStringUTF(errorMessage.c_str());
        env->CallObjectMethod(errorMessageBuffer, mid, value);
    }

    ret = env->NewStringUTF(result.c_str());
    env->ReleaseStringUTFChars(jadlStr, adlStr);
    return ret;
}
