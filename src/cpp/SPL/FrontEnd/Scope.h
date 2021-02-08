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

#ifndef SPL_SYMBOL_TABLE_SCOPE_H
#define SPL_SYMBOL_TABLE_SCOPE_H

#include <UTILS/HashMapHelpers.h>
#include <assert.h>
#include <string>

class ANTLR3_BASE_TREE_struct;

namespace SPL {
typedef ANTLR3_BASE_TREE_struct AstNode;
class RootTyp;
class OtherSym;
class SourceLocation;
class SymbolTableEntry;
class TupleTyp;

/// Generic map from string keys to values of a type T that maintains
/// the order of insertion, such that values can also be accessed by an
/// index. This is useful to get deterministic iteration order for
/// messages that can be more easily diffed.
template<typename T>
class StringMap
{
    typedef std::vector<std::string> Idx2Key;
    typedef std::vector<T> Idx2Val;
    typedef std::tr1::unordered_map<std::string, int> Key2Idx;
    Idx2Key _idx2key;
    Idx2Val _idx2val;
    Key2Idx _key2idx;

  public:
    /// Constructor, creates new empty StringMap.
    StringMap() {}

    /// Retrieve value by integer index.
    T get(int idx) const { return _idx2val.at(idx); }

    /// Retrieve value by string key.
    T get(std::string const& key) const { return _idx2val.at(_key2idx.find(key)->second); }

    /// Retrieve key by integer index.
    std::string getKey(int idx) const { return _idx2key.at(idx); }

    /// Is the key valid?
    bool has(std::string const& key) const { return _key2idx.end() != _key2idx.find(key); }

    /// Insert key-value pair at next available index. Error if the key
    /// already exists.
    void put(std::string const& key, T val)
    {
        assert(!has(key));
        int idx = size();
        _idx2key.push_back(key);
        _idx2val.push_back(val);
        _key2idx[key] = idx;
    }

    /// Number of key-value entries in this StringMap.
    int size() const { return _idx2val.size(); }
};

/// Scope in the symbol table, containing symbol table entries.
class Scope
{
    typedef StringMap<SymbolTableEntry*> EntryMap;
    EntryMap _entries;
    SymbolTableEntry* _holder;
    Scope* _parentScope;

  public:
    /// Constructor, creates new scope with specified parent and holder.
    Scope(Scope* parentScope, SymbolTableEntry* holder);

    /// Constructor, creates new scope, and adds each attribute from the
    /// tuple type as an AttributeFromTypeSym.
    Scope(SourceLocation const&, TupleTyp const&, Scope* parentScope);

    /// Add all symbols from the other scope to this scope.
    void addAll(Scope const&);

    /// Define the symbol by adding it to this scope.
    SymbolTableEntry& def(SymbolTableEntry&);

    /// DEPRECATED. Create a new 'OtherSym' and define it in this scope.
    OtherSym& def(SourceLocation const&, AstNode* defNode, std::string name);

    /// Get a symbol by the index of its insertion.
    SymbolTableEntry& get(int) const;

    /// Get a symbol by its simple name.
    SymbolTableEntry& get(std::string const&) const;

    /// Does this scope contain a symbol by that simple name?
    bool has(std::string const&) const;

    /// The symbol table entry that holds this scope. For example, a
    /// tuple type is the holder for the scope with its attributes.
    SymbolTableEntry* holder() const { return _holder; }

    /// The parent scope (enclosing scope in lexical scoping).
    Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    std::ostream& print(std::ostream&) const;

    /// Number of symbols in this scope.
    int size() const { return _entries.size(); }
};

/// Print scope to output stream.
std::ostream& operator<<(std::ostream&, Scope const*);
std::ostream& operator<<(std::ostream&, Scope const&);

/// For each enum constant in the type (including member types in case
/// of containers), create a new OtherSym and add it to this scope. This
/// is used by the debugger to determine what's in scope at the REPL.
void addEnumConstantsToScope(Scope&, RootTyp const&);

/// Turn a tuple type into a scope. The caller may already have a scope
/// for the tuple type in question, which it can pass in to prevent
/// duplicate scope creation.
Scope* typeToScope(SourceLocation const&, RootTyp const&, Scope* scope);
};

#endif /*SPL_SYMBOL_TABLE_SCOPE_*/
