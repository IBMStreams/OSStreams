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

#include <SPL/Core/CompilerConfiguration.h> // Include the class header first
#include <SPL/Runtime/Utility/Singleton.t>

#include <memory.h>
#include <stdlib.h>

template class SPL::Singleton<
  SPL::CompilerConfiguration>; // Explicitly instantiate the singleton code here

SPL::CompilerConfiguration::CompilerConfiguration()
  : SPL::Singleton<SPL::CompilerConfiguration>(*this)
{
    memset(_boolOptions, 0, sizeof _boolOptions);
    memset(_intOptions, 0, sizeof _intOptions);
    memset(_floatOptions, 0, sizeof _floatOptions);
    memset(_isSetBool, 0, sizeof _isSetBool);
    memset(_isSetString, 0, sizeof _isSetString);
    memset(_isSetInt, 0, sizeof _isSetInt);
    memset(_isSetFloat, 0, sizeof _isSetFloat);
    memset(_isSetStringVector, 0, sizeof _isSetStringVector);
    _fusionMode = FusionDefault;
    _aryMode = ARYAuto;
    _cxxLangLevel = LangLevelCxx98;

    const char* modelFile = getenv("STREAMS_SPL_MODEL_FILE");
    if (modelFile) {
        setStringValue(CompilerConfiguration::ModelFile, modelFile);
    }
}
