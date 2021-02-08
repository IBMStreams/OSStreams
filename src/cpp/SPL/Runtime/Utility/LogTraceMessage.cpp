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

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

using namespace std;
using namespace SPL;

void SPL::splLogMessage(int ilvl,
                        const FormattableMessage& msg,
                        const string& aspect,
                        const string& function,
                        const string& file,
                        uint32_t line)
{
    // Generate ID : messages
    string m = msg.key();
    m += ": ";
    m += (string)msg;
    Distillery::debug::write_log(ilvl, SPL::splAppLogAspect(aspect), function, file, line, m);
}

void SPL::splTraceMessage(int ilvl,
                          const FormattableMessage& msg,
                          const string& aspect,
                          const string& function,
                          const string& file,
                          uint32_t line)
{
    // Generate ID : messages
    string m = msg.key();
    m += ": ";
    m += (string)msg;
    Distillery::debug::write_appmsg(ilvl, SPL::splAppTrcAspect(aspect), function, file, line, m);
}
