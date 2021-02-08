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

#ifndef SPL_SYMBOL_TABLE_ENTRY_H
#define SPL_SYMBOL_TABLE_ENTRY_H

#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <string>
#include <vector>

namespace SPL {
class RootTyp;
class Scope;
class SourceLocation;
class Expression;

/// Abstract class for entry in a scope in the symbol table and/or
/// decoration for an AST node. Has many subclasses for different
/// compile-time SPL entities.
class SymbolTableEntry
{
    SourceLocation const& _loc;

  protected:
    SymbolTableEntry(SourceLocation const& loc)
      : _loc(loc)
    {}

  public:
    /// Is 'name' the internal dummy name for the default namespace?
    static bool isDefaultNameSpace(const std::string& name);

    /// Assert that this symbol is an instance of T, and do type conversion.
    template<class T>
    T& as();
    template<class T>
    const T& as() const;

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const = 0;

    /// If this is a symbol for an expression, after the compiler front-end
    /// is done with it, turn it into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const { return NULL; }

    /// Will a call to the method 'ast()' succeed?
    virtual bool hasAst() const { return true; }

    /// Scope held by this symbol: for example, a tuple type holds a
    /// scope for its attributes. May trigger recursive lazy type
    /// resolution: for example, a tuple type may depend on other types.
    virtual Scope* heldScope() const = 0;

    /// Is this symbol an instance of T?
    template<class T>
    bool is() const;

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const = 0;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const = 0;

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const = 0;

    /// Source location where this symbol is defined.
    virtual SourceLocation const& sourceLocation() const { return _loc; }

    /// Type of this symbol, if any. May trigger recursive lazy type
    /// resolution: for example, a tuple type may depend on other types.
    virtual RootTyp const& type() const = 0;

    virtual void setType(const RootTyp& typ) { assert(false); }

    /// Full name (including qualifier) of this symbol, if any.
    virtual std::string fullName() const;

    /// Expands the held scope if necessary
    virtual void expand() = 0;

    /// Destructor.
    virtual ~SymbolTableEntry() {}
};

#define SYM_IS_AS_SPECIALIZATIONS(asType)                                                          \
    template<>                                                                                     \
    inline bool SymbolTableEntry::is<asType>() const                                               \
    {                                                                                              \
        assert(NULL != this);                                                                      \
        return typeid(asType) == typeid(*this);                                                    \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& SymbolTableEntry::as<asType>()                                                  \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType*>(this));                                               \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline asType const& SymbolTableEntry::as<asType>() const                                      \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType const*>(this));                                         \
        return *static_cast<asType const*>(this);                                                  \
    }

#define SYM_IS_AS_SPECIALIZATIONS_DC(asType)                                                       \
    template<>                                                                                     \
    inline bool SymbolTableEntry::is<asType>() const                                               \
    {                                                                                              \
        return NULL != dynamic_cast<asType const*>(this);                                          \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& SymbolTableEntry::as<asType>()                                                  \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType*>(this));                                               \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline asType const& SymbolTableEntry::as<asType>() const                                      \
    {                                                                                              \
        assert(NULL != dynamic_cast<asType const*>(this));                                         \
        return *static_cast<asType const*>(this);                                                  \
    }

/// Print the symbol to the output stream.
std::ostream& operator<<(std::ostream&, SymbolTableEntry const*) DLL_PUBLIC;
std::ostream& operator<<(std::ostream&, SymbolTableEntry const&) DLL_PUBLIC;

/// Abstract class for operator symbol, which can be either a primitive
/// operator or a composite operator.
class OperatorSym : public SymbolTableEntry
{
  protected:
    OperatorSym(SourceLocation const& loc)
      : SymbolTableEntry(loc)
    {}

  public:
    /// Full name (including qualifier) of this symbol, if any.
    virtual std::string fullName() const;

    /// Scope containing the symbols of formal parameters of this operator.
    virtual Scope* paramsScope() const = 0;
};
SYM_IS_AS_SPECIALIZATIONS_DC(OperatorSym)
// must use dynamic_cast variant, because "typeid==" only makes sense at a leaf in the inheritance
// hierarchy
};

#endif /*SPL_SYMBOL_TABLE_ENTRY_H*/
