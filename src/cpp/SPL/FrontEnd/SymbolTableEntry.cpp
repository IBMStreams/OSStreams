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

#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <iostream>

namespace SPL {
std::ostream& operator<<(std::ostream& os, SymbolTableEntry const* entry)
{
    if (NULL == entry) {
        return os << "NULL_ENTRY";
    }
    return entry->print(os);
}

std::ostream& operator<<(std::ostream& os, SymbolTableEntry const& entry)
{
    return entry.print(os);
}

std::string SymbolTableEntry::fullName() const
{
    return name();
}

std::string OperatorSym::fullName() const
{
    const Scope* pScope = parentScope();
    assert(pScope);
    const Scope* nsScope = pScope->parentScope();
    assert(nsScope);
    std::string nsName = nsScope->holder()->as<NamespaceSym>().name();
    if (isDefaultNameSpace(nsName)) {
        return name();
    }
    return nsName + "::" + name();
}
};
