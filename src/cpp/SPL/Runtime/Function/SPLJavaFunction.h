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

#ifndef SPL_RUNTIME_FUNCTION_SPL_JAVA_FUNCTION_H
#define SPL_RUNTIME_FUNCTION_SPL_JAVA_FUNCTION_H

/*
 * Support calling Java native functions from SPL.
 */

#include <jni.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL {
class DLL_PUBLIC SPLJavaFunction
{
  public:
    static void* loadFunction(const char* toolkitName,
                              const char* toolkitVers,
                              const char* adlFileName,
                              const char* funcClassLibRelPath,
                              const char* funcClassName,
                              const char* funcMethodName,
                              const char* funcSignature,
                              const char* SPLNamespace,
                              const char* prototype,
                              jclass* classGlobalRefPtr,
                              jmethodID* midPtr);
    static JNIEnv* getJNIEnv(void* func);
    static void throwCallError(void* func);
    static void throwCreateError(void* func);
};
}

#endif /* SPL_RUNTIME_FUNCTION_SPL_JAVA_FUNCTION_H */
