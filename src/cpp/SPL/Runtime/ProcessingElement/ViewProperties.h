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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_VIEW_PROPERTIES_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_VIEW_PROPERTIES_H

#include <UTILS/HashMapHelpers.h>
#include <UTILS/Mutex.h>
#include <UTILS/SPCTypes.h>
#include <string>

namespace SPL {
class OperatorImpl;

class ViewProperties
{
  public:
    /// Constructor
    /// @param viewId view id for this set of properties
    /// @param value initial properties for the view
    /// @param op operator for this view
    /// @param port output port for this view
    ViewProperties(uint64_t viewId,
                   OperatorImpl& op,
                   uint32_t port,
                   const std::vector<Streams::nameValuePair_t>& value);

    /// Update the properties for this view
    /// @param value properties to update
    void update(const std::vector<Streams::nameValuePair_t>& value);

    /// Return the view id for the properties
    /// @return the view id
    uint64_t getViewId() const { return _viewId; }

    /// Return the operator for this view id
    /// @return the operator
    OperatorImpl& getOperator() const { return _op; }

    /// Return the port for this view id
    /// @return the output port number
    uint32_t getPort() const { return _port; }

    /// Return the tuple rate for the view
    uint32_t getRate() const { return _rate; }

  private:
    static const uint32_t MaxRate = 100;

    uint64_t _viewId;
    OperatorImpl& _op;
    uint32_t _port;
    uint32_t _rate;
};
};
#endif // SPL_RUNTIME_PROCESSING_ELEMENT_VIEW_PROPERTIES_H
