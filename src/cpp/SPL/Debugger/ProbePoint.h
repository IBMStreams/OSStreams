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

#ifndef SPL_DEBUGGER_ProbePoint_H
#define SPL_DEBUGGER_ProbePoint_H

#include <SPL/Debugger/ProbePointPredicate.h>
#include <SPL/Runtime/Operator/Port/DebugHook.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/Mutex.h>
#include <map>
#include <string>
#include <vector>

namespace SPL {
class PEImpl;
class Tuple;

/*!
 * Each instance of this class represents a probe point
 * on the SPL application's data path.  A probe point
 * is associated with an operator's port and is distinguished
 * by the operator's name, port type (input or output), and
 * port index.
 *
 * This class represents the base function shared by the
 * various probe point types - e.g. breakpoints, tracepoints, etc.
 */
class ProbePoint : public DebugHook
{
    bool _enabled;
    UTILS_NAMESPACE::Mutex _mutex;

  public:
    typedef std::map<std::string, std::string> ProbePointConditionsType;
    ProbePointConditionsType _conditions;

  protected:
    PEImpl& _peHandle;
    std::string _opName;
    PEImpl::PortType _portType;
    uint32_t _portIndex;
    ProbePointPredicatePtr _predicate;

    ProbePoint(PEImpl& peHandle,
               const std::string& opName,
               PEImpl::PortType portType,
               uint32_t portIndex);

  public:
    /// Return the Processing Element for this probe point
    /// @return Processing Element
    PEImpl& getPeHandle() const { return _peHandle; }

    /// Return the name of the operator associated with this probe point
    /// @return Operator name
    std::string getOperationName() const { return _opName; }

    /// Return the type of the port associated with this probe point
    /// @return input/output
    PEImpl::PortType getPortType() const { return _portType; }

    /// Return the number of the port associated with this probe point
    /// @return port number
    uint32_t getPortIndex() const { return _portIndex; }

    /// Is this probe point enabled?
    /// @return 'true' if it is currently enabled
    virtual bool isEnabled() const { return _enabled; }

    /// Enable or disable a probe point
    /// @param enabled 'true' to enable a probe point
    virtual void setEnabled(bool enabled) { _enabled = enabled; }

    /// Return the probe point type (i.e. Breakpoint/TracePoint/InjectPoint)
    /// @return probe point type
    virtual std::string getProbePointType() const = 0;

    /// Returns a map that describes the operating conditions of the probe point
    /// e.g. number of trace records captured.
    /// @return operating conditions
    virtual ProbePointConditionsType& getConditions();

    /// Return the condition matching the key
    /// @param key desired lookup key
    /// @return matching string, or ""
    std::string getCondition(const std::string& key);

    /// Set a condition to a value
    /// @param key condition key
    /// @param value condition value
    void setCondition(const std::string& key, const std::string& value);

    /// Delete a condition
    /// @param key condition to delete
    void deleteCondition(const std::string& key);

    /// Condition test - does the condition with the given key have the value?
    /// @param key condition to lookup
    /// @param value condition to test
    /// @return true if the key exists, and has value 'value'
    bool isCondition(const std::string& key, const std::string& value);

    /// Is this probe point the given type?
    /// @param type Type to match
    /// @return 'true' if the probe point type is 'type'
    bool isType(const char* type);

    /// Set a conditional predicate for the probe point
    /// @param predicate Predicate to set
    void setPredicate(ProbePointPredicatePtr predicate);

    /// Return the conditional predicate for the probe point
    /// @return Predicate associated with probe point
    ProbePointPredicatePtr getPredicate() { return _predicate; };

    /// Take action when a Probe Point triggers
    /// @param hookId Breakpoint number
    /// @param tuple Current Tuple, or NULL if punct != NULL
    /// @param punct Current punctuation or NULL if tuple != NULL
    /// @return true if tuple is to be preserved, false to drop tuple
    virtual bool process(uint32_t hookId, Tuple* tuple, Punctuation* punct) = 0;
};

typedef boost::shared_ptr<ProbePoint> ProbePointPtr;
}; // namespace SPL

#endif /* SPL_DEBUGGER_ProbePoint_H */
