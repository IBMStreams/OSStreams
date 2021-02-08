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

#ifndef USER_FUNCTION_TABLE_H
#define USER_FUNCTION_TABLE_H

#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <iostream>

namespace SPL {
class FunctionHeadSym;
class FunctionGenerator : Singleton<FunctionGenerator>
{
  public:
    FunctionGenerator()
      : Singleton<FunctionGenerator>(*this)
    {}

    /// Generate C++ code for all functions, if it has changed from the
    /// existing version on disk
    void generateCPPCode();

    // Optmize C++ code for all functions
    void optimize();
};
}

#endif
