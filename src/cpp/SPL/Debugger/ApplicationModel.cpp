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

#include <SPL/Debugger/ApplicationModel.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

ApplicationModel::ApplicationModel(PEImpl& peHandle)
  : _peHandle(peHandle)
{}

void ApplicationModel::getOperatorNames(unordered_set<std::string>& names) const
{
    const unordered_map<std::string, OperatorImpl*>& ops = _peHandle.getOperatorMapping();
    unordered_map<std::string, OperatorImpl*>::const_iterator it;

    for (it = ops.begin(); it != ops.end(); it++) {
        names.insert(it->first);
    }
}

void ApplicationModel::getOperators(unordered_map<std::string, OperatorImpl*>& opMap) const
{
    opMap = _peHandle.getOperatorMapping();
}

OperatorImpl* ApplicationModel::getOperator(const std::string& opName) const
{
    const unordered_map<std::string, OperatorImpl*>& ops = _peHandle.getOperatorMapping();
    unordered_map<std::string, OperatorImpl*>::const_iterator it = ops.find(opName);
    if (it == ops.end()) {
        return NULL;
    }
    return it->second;
}
