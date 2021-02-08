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

#include <SPL/Debugger/ProbePoint.h>

#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;
UTILS_NAMESPACE_USE

ProbePoint::ProbePoint(PEImpl& peHandle,
                       const std::string& opName,
                       PEImpl::PortType portType,
                       uint32_t portIndex)
  : _enabled(true)
  , _peHandle(peHandle)
  , _opName(opName)
  , _portType(portType)
  , _portIndex(portIndex)
{}

void ProbePoint::setPredicate(ProbePointPredicatePtr predicate)
{
    if (predicate.get() == NULL) {
        deleteCondition("predicate");
    } else {
        setCondition("predicate", predicate->toString());
    }
    _predicate = predicate;
}

ProbePoint::ProbePointConditionsType& ProbePoint::getConditions()
{
    return _conditions;
}

std::string ProbePoint::getCondition(const std::string& key)
{
    AutoMutex am(_mutex);
    ProbePointConditionsType::iterator iter = _conditions.find(key);
    if (iter == _conditions.end()) {
        return "";
    }
    return iter->second;
}

void ProbePoint::setCondition(const std::string& key, const std::string& val)
{
    AutoMutex am(_mutex);
    _conditions.erase(key);
    _conditions.insert(make_pair(key, val));
}

bool ProbePoint::isCondition(const std::string& key, const std::string& val)
{
    AutoMutex am(_mutex);
    ProbePointConditionsType::iterator iter = _conditions.find(key);
    if (iter == _conditions.end()) {
        return false;
    }
    return (iter->second.compare(val) == 0);
}

void ProbePoint::deleteCondition(const std::string& key)
{
    AutoMutex am(_mutex);
    ProbePointConditionsType::iterator it = _conditions.find(key);
    if (it != _conditions.end()) {
        _conditions.erase(it);
    }
}

bool ProbePoint::isType(const char* type)
{
    return (type != NULL && getProbePointType().compare(type) == 0);
}
