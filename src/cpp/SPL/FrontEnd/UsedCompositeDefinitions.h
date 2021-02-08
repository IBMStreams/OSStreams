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

#ifndef SPL_USED_COMPOSITE_DEFINITIONS_H
#define SPL_USED_COMPOSITE_DEFINITIONS_H

#include <iostream>
#include <string>
#include <vector>

#include <SPL/CodeGen/Config.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>

namespace SPL {
class SourceLocation;
class CompositeDefinition;

/// List of composite operator definitions that are used. Used means it is
/// instantiated in this application
class UsedCompositeDefinitions : public Singleton<UsedCompositeDefinitions>
{
  public:
    /// Constructor
    UsedCompositeDefinitions();

    /// Destructor
    ~UsedCompositeDefinitions();

    /// Add a composite operator
    /// @param kind name (name.space::oper)
    CompositeDefinition& add(const std::string& kind);

    /// Return the number of composties
    /// @return size of table
    uint32_t numComposites() const { return _composites.size(); }

    /// Return all the composites
    /// @return vector of composites
    const std::vector<const CompositeDefinition*>& getComposites() const { return _composites; }

    /// Return the composite matching a name
    /// @param kind composite being searched for
    /// @return pointer to composite or NULL if not found
    const CompositeDefinition* find(const std::string& kind) const;

    /// Return the composite of given index
    /// @param index desired composite
    /// @return pointer to composite or NULL if not found
    const CompositeDefinition& operator[](uint32_t index) const { return *_composites[index]; }

    /// Print the table
    /// @param s output stream
    /// @return s
    std::ostream& print(std::ostream& s) const;

  private:
    std::vector<const CompositeDefinition*> _composites;
    std::tr1::unordered_map<std::string, const CompositeDefinition*> _index;
};
};

#endif /* SPL_USED_COMPOSITE_DEFINITIOS_H */
