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
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace std;
using namespace SPL;

string SPL::spllogAspect(const string& aspect)
{
    string ret = "#spllog,#splapptrc,";
    if (PEImpl::hasInstance()) {
        static PEImpl& pe = PEImpl::instance();
        ret += pe.extraAspectInfo();
        ret += OperatorTracker::getCurrentOperatorName();
    }
    if (!aspect.empty()) {
        ret += ',';
        ret += aspect;
    }
    return ret;
}

string SPL::splAppTrcAspect(const string& aspect)
{
    string ret = "#splapptrc,";
    if (PEImpl::hasInstance()) {
        static PEImpl& pe = PEImpl::instance();
        ret += pe.extraAspectInfo();
        ret += OperatorTracker::getCurrentOperatorName();
    }
    if (!aspect.empty()) {
        ret += ',';
        ret += aspect;
    }
    return ret;
}

string SPL::splAppLogAspect(const string& aspect)
{
    // Jingdong: log notification process parses log aspects.
    // Please talk with Platform team if this updated.
    string ret = "#splapplog,";
    if (PEImpl::hasInstance()) {
        static PEImpl& pe = PEImpl::instance();
        ret += pe.extraAspectInfo();
        ret += OperatorTracker::getCurrentOperatorName();
    }
    if (!aspect.empty()) {
        ret += ',';
        ret += aspect;
    }
    return ret;
}
