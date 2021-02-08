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

/*
 * Implementation of splCkptLogAspect(), splCkptTrcAspect(), splCkptLogMessage(),
 * and splCkptTraceMessage() functions
 */

#include <SPL/Runtime/Operator/State/CheckpointLogTrace.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace std;
using namespace Distillery;
using namespace SPL;

std::string SPL::splCkptLogAspect(const std::string& opName)
{
    std::string ret = "#splapplog,";
    static PEImpl& pe = PEImpl::instance();
    ret += pe.extraAspectInfo();
    if (!opName.empty()) {
        return ret + opName + "," SPL_CKPT;
    } else {
        return ret + SPL_CKPT;
    }
}

std::string SPL::splCkptTrcAspect(const std::string& opName)
{
    std::string ret = "#splapptrc,";
    static PEImpl& pe = PEImpl::instance();
    ret += pe.extraAspectInfo();
    if (!opName.empty()) {
        return ret + opName + "," SPL_CKPT;
    } else {
        return ret + SPL_CKPT;
    }
}

void SPL::splCkptLogMessage(int ilvl,
                            const FormattableMessage& msg,
                            const std::string& aspect,
                            const std::string& function,
                            const std::string& file,
                            uint32_t line)
{
    std::string msgStr = msg.key();
    msgStr += ": ";
    msgStr += (std::string)msg;
    Distillery::debug::write_log(ilvl, aspect, function, file, line, msgStr);
}

void SPL::splCkptTraceMessage(int ilvl,
                              const FormattableMessage& msg,
                              const std::string& aspect,
                              const std::string& function,
                              const std::string& file,
                              uint32_t line)
{
    std::string msgStr = msg.key();
    msgStr += ": ";
    msgStr += (std::string)msg;
    Distillery::debug::write_appmsg(ilvl, aspect, function, file, line, msgStr);
}
