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

#include <SPL/Runtime/Operator/Port/SubsExpressionEvalJNI.h>
#include <SPL/Runtime/Operator/Port/SubsExpressionEvaluator.h>
#include <UTILS/DistilleryException.h>
#include <xercesc/util/PlatformUtils.hpp>

using namespace std;
UTILS_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE

JNIEXPORT void JNICALL
Java_com_ibm_streams_spl_internal_api_port_SubsExpressionEvalJNI_initCpp(JNIEnv*, jobject)
{
    XMLPlatformUtils::Initialize();
}

JNIEXPORT void JNICALL
Java_com_ibm_streams_spl_internal_api_port_SubsExpressionEvalJNI_closeCpp(JNIEnv*, jobject)
{
    // skip the Terminate call as it may interfere with other active XML uses in the process
    // XMLPlatformUtils::Terminate();
}

JNIEXPORT jstring JNICALL
Java_com_ibm_streams_spl_internal_api_port_SubsExpressionEvalJNI_evaluateCpp(JNIEnv* env,
                                                                             jobject jobj,
                                                                             jstring jexpr,
                                                                             jstring jprops)
{
    jstring retS;
    jboolean isCopy;
    const char* exprString = env->GetStringUTFChars(jexpr, &isCopy);
    const char* propString = env->GetStringUTFChars(jprops, &isCopy);
    try {
        bool ret = SPL::SubsExpressionEvaluator::evaluate(string(exprString), string(propString));
        retS = env->NewStringUTF((ret ? "true" : "false"));
    } catch (const DistilleryException& e) {
        retS = env->NewStringUTF(e.toXMLString().c_str());
    } catch (...) {
        retS = env->NewStringUTF("UNKNOWN_EXCEPTION");
    }

    // release the chars
    env->ReleaseStringUTFChars(jexpr, exprString);
    env->ReleaseStringUTFChars(jprops, propString);
    return retS;
}
