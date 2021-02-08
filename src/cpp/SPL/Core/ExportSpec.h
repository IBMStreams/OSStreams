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

#ifndef SPL_CORE_EXPORTSPEC_H
#define SPL_CORE_EXPORTSPEC_H

#include <SPL/CodeGen/Literal.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

namespace SPL {
class PrimitiveOperatorInstance;

/// Define the Export Specification for an Export port
/// Example:
///   param properties : ticker = ["a","b"], value = 5.16
class ExportSpec
{
  public:
    /// Data type for string/expn pairs
    typedef std::pair<std::string, Literal> Property;

    /// Destructor
    ~ExportSpec();

    /// Get the operator IR used to create this export spec
    /// @return operator IR used to create this export spec
    PrimitiveOperatorInstance const& getOperatorIR() const { return _operatorIR; }

    /// Is this a named export stream?
    /// @return 'true' if the stream is named
    bool isNameBased() const { return _isNameBased; }

    /// Return the stream id
    /// @pre isNameBased()
    /// @return stream id
    const std::string& getStreamId() const { return _streamId; }

    /// Return the list of exported expressions
    /// @pre !isStreamName()
    /// @return list
    const std::vector<Property>& getStreamProperties() const { return *_propList; }

    /// Construct an export spec from the parameters to an Export operator
    /// @param op Operator Invocation (contains loc and params)
    /// @return constructed ExportSpec (or NULL if params not valid)
    static const ExportSpec* createFromExportOper(const PrimitiveOperatorInstance& op);

    /// Return whether filters are allowed on an export
    /// @return 'true' if filters are allowed
    bool getAllowFilter() const { return _allowFilter; }

    /// Returns the congestionPolicy for the Export operator
    /// @return Returns either `wait` or `dropConnection`.
    const std::string& getCongestionPolicy() const { return _congestionPolicy; }

    /// Print an Export Spec
    std::ostream& print(std::ostream&) const;

  private:
    ExportSpec(const std::string& streamName,
               bool allowFilter,
               const std::string& congestionPolicy,
               PrimitiveOperatorInstance const& operatorIR);

    ExportSpec(const std::vector<Property>& propList,
               bool allowFilter,
               const std::string& congestionPolicy,
               PrimitiveOperatorInstance const& operatorIR);

    bool _isNameBased;
    bool _allowFilter;
    std::string _streamId;
    std::string _congestionPolicy;
    const std::vector<Property>* _propList;

    PrimitiveOperatorInstance const& _operatorIR;
};
}

#endif // SPL_CORE_EXPORTSPEC_H
