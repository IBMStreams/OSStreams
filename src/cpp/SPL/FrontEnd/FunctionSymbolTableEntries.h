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

#ifndef SPL_FUNCTION_SYMBOL_TABLE_ENTRIES_H
#define SPL_FUNCTION_SYMBOL_TABLE_ENTRIES_H

#include <SPL/Core/FunctionModelImpl.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <vector>

namespace SPL {
class BlockStatement;
class BoundsFormal;
class Expression;
class FunctionFormalSym;
class FunctionHeadSym;
class TypeFormal;

/// Symbol table entry for a call.
class CallExpressionSym : public SymbolTableEntry
{
  public:
    typedef std::vector<RootTyp const*> ActualTypes;
    bool initActualTypes(AstNode& ast);

  private:
    ActualTypes _actualTypes;
    std::string _name;
    Scope* _parentScope;
    FunctionHeadSym* _resolvedTgt;
    RootTyp const* _returnType;
    bool _submitNeedingCopy;
    bool match(AstNode&, SymbolTableEntry&, std::vector<FunctionHeadSym*>&);
    void useFunction(std::string const& namespaceName,
                     AstNode&,
                     std::vector<FunctionHeadSym*>& candidates);
    bool promotionRequiredToGetFrom(const RootTyp& fromType, const RootTyp& toType);

  public:
    /// Constructor, triggers resolution by lookup from the current symbol table scope.
    /// May lead to pulling in more compilation units; may lead to error messages.
    CallExpressionSym(SourceLocation const&, AstNode&, Scope*);

    /// Types of the actual parameters in this call.
    virtual ActualTypes const& actualTypes() const { return _actualTypes; }

    /// We refrain from storing a pointer from this symbol table entry to its AST,
    /// to simplify memory management by allowing the AST to be reclaimed first.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// If return value is a tuple, scope with the attributes, else NULL.
    virtual Scope* heldScope() const;

    /// Simple name of the target function or type.
    virtual std::string name() const { return _name; }

    /// Parent scope.
    virtual Scope* parentScope() const { return _parentScope; }

    /// For example, 'foo(int32, rstring)'.
    virtual std::ostream& printBare(std::ostream&) const;

    /// For example, 'CallExpressionSym(void foo(int32, rstring))'.
    virtual std::ostream& print(std::ostream&) const;

    /// Reset the target function.
    virtual void resetResolvedTgt(FunctionHeadSym& tgt) { _resolvedTgt = &tgt; }

    /// Target function.
    virtual FunctionHeadSym* resolvedTgt() const { return _resolvedTgt; }

    /// Return type.
    virtual RootTyp const& type() const
    {
        assert(NULL != _returnType);
        return *_returnType;
    }

    /// Is this an intrinsic submit function that needs a copy?
    bool getSubmitNeedingCopy() const { return _submitNeedingCopy; }

    /// Make this an intrinsic submit function that needs a copy
    void setSubmitNeedingCopy() { _submitNeedingCopy = true; }

    virtual void expand() {}

    friend class SymbolTable;
};
SYM_IS_AS_SPECIALIZATIONS(CallExpressionSym)

/// Symbol table entry for a function definition (with body),
/// declaration (without body), or intrinsic (special for
/// compiler). This is never looked up directly; instead, lookup always
/// goes through an OverloadedFunctionsSym.
class DLL_PUBLIC FunctionHeadSym : public SymbolTableEntry
{
  public:
    friend class SymbolTable;
    typedef std::vector<BoundsFormal*> BoundsFormals;
    typedef std::vector<Function::LibraryPtr> Dependencies;
    typedef std::vector<FunctionFormalSym*> Formals;
    typedef std::vector<TypeFormal const*> TypeFormals;

  private:
    SPLAst* _splAst;                   // LEAK only non-NULL for unresolved protos
    BoundsFormals _boundsFormals;      // empty unless native and generic
    int16_t _builtinNumber;            //-1 unless native and builtin
    Dependencies const* _dependencies; // native functions only
    Formals _formals;                  // e.g., (int32 x, int32 y)
    Scope* _heldScope;                 // scope for formals and top-level locals
    BlockStatement* _ir;               // body
    bool _isDef : 1;                   // SPL def, as opposed to (maybe native) proto
    bool _isIntrinsic : 1; // not a real function, but an output function or window or config value
                           // or various others
    bool _isPublic : 1;    // has public modifier
    bool _isStateful : 1;  // has stateful modifier
    bool _isUsed : 1;      // in a reachable file and used by a call expression
    bool _includeIsBuiltin : 1;   // standard builtin
    bool _isNativeFunction : 1;   // 'true' -> native ; false 'SPL function'
    bool _isOutputFunction : 1;   // 'true' -> Output function name ; false 'anything else'
    bool _makesStatefulCall : 1;  // 'true' -> makes at least one call to a stateful function
    bool _isPseudoFunction : 1;   // 'true' -> Not really a function; different alias check
    bool _isJavaFunction : 1;     // 'false' -> Not a Java function by default
    std::string _name;            // simple name (without namespace qualifier)
    std::string _includeFileName; // include file name for native functions
    std::string _modelBase;       // base directory for model
    RootTyp const* _returnType;
    std::string _splFileName;     // SPL file name for non-native functions
    TypeFormals _typeFormals;     // empty unless native and generic
    uint32_t _toolkitIndex;       // which toolkit is this from?
    std::string _cppNamespace;    // for native functions, what is the C++ namespace?
    std::string _cppFunctionName; // for native functions, what is the C++ function name?
    FunctionHeadSym(SourceLocation const&, AstNode&, Scope*);
    void initFormalType(AstNode&);
    void joinGroup();

  public:
    /// Called during compiler start-up to register a declaration (without body).
    static FunctionHeadSym& analyzeFunctionFromToolkitModel(
      std::string const& nameSpace,   // e.g. "spl.relational"
      std::string const& source,      // e.g. "<any T> stateful void f(list<T> x, mutable int32 y)"
      std::string const& xmlFileName, // e.g. "foo/bar/function.xml"
      std::string const& splFileName, // e.g. "foo/bar/util.spl"
      uint32_t line,                  // e.g. 5
      uint32_t column,                // e.g. 1
      std::string const& includeFileName, // e.g. SPLFunctions.h (or empty)
      std::string const& modelBase,       // e.g. toolkit/spl/sample/native.function
      bool isNativeFunction,              // Is this a native function, or a spl one?
      Dependencies const* deps,           // from function model xml
      int32_t builtinNumber,              // e.g. 99
      uint32_t toolkitNumber,             // e.g. 1
      std::string const& cppNamespace,    // e.g. "SPL::Math" or empty (use spl one)
      std::string const& cppFunctionName  // e.g. "fred" or empty (use spl one)
    );

    /// Intrinsic functions pretend to be normal functions, but the
    /// compiler treats them specially, such as the size() in a window,
    /// the host() in a config, the submit() in operator logic, or the
    /// Avg() output function in an aggregate operator.
    static FunctionHeadSym& analyzeIntrinsicFunction(Scope* scope,
                                                     std::string const& source,
                                                     bool isNativeFunction);

    /// In this case we have hidden functions which the compiler is aware of but no-one outside of
    /// the compiler can call them.
    /// Unlike intrinsic functions these functions are hidden real functions.
    static FunctionHeadSym& analyzeHiddenFunction(Scope* scope,
                                                  std::string const& source,
                                                  bool isNativeFunction,
                                                  std::string const& cppNamespace);

    /// Pull out types from the AST and record them in this symbol table
    /// entry (including the return type, generic formals, and regular formals).
    void initTypes(AstNode&);

    // methods to be called from PathSearch

    /// If in builtin toolkit (i.e. namespace 'spl::'), unique number in there, else -1.
    int16_t builtinNumber() const { return _builtinNumber; }

    /// Vector of FunctionFormalSym.
    Formals const& formals() const { return _formals; }

    /// True if definition, false if mere prototype.
    bool isDef() const { return _isDef; }

    /// Is this a prototype declaration implemented by the other function definition?
    bool isProtoFor(FunctionHeadSym const&) const;

    /// Can any calls be ambiguous between this and the other FunctionHeadSym?
    bool overlapsWith(FunctionHeadSym const&) const;

    /// Is this an intrinsic (see analyzeIntrinsicFunction)?
    bool isIntrinsic() const { return _isIntrinsic; }

    /// Is this a pseudo output function, e.g., Custom(), for Aggregate?
    bool isPseudoFunction() const { return _isPseudoFunction; }

    /// Is this an output function, e.g., Avg(), for an operator?
    bool isOutputFunction() const { return _isOutputFunction; }

    /// Mark this FunctionHeadSym as an output function.
    void setIsOutputFunction(bool v) { _isOutputFunction = v; }

    /// Mark this FunctionHeadSym as an pseudo output function.
    void setIsPseudoFunction(bool v) { _isPseudoFunction = v; }

    /// Is this a native function, i.e., the body is not defined in SPL itself?
    bool isNativeFunction() const { return _isNativeFunction; }

    /// Is this a Java native function
    bool isJavaFunction() const { return _isJavaFunction; }

    /// Did we encounter any call to this function?
    virtual bool isUsed() const { return _isUsed; }

    /// Mark this FunctionHeadSym as used.
    virtual void markUsed();

    /// Run the remaining stages on this prototype.
    void resolvePrototype();

    /// Set IR encapsulating the body for the compiler back-end.
    virtual BlockStatement* setFunctionBodyIR(BlockStatement*);

    /// Result of printBare().
    virtual std::string toString() const;

    /// In case of a native function, native libraries it depends on.
    const std::vector<Function::LibraryPtr>* dependencies() const { return _dependencies; }

    /// In case of a native function, include file with the native declaration.
    const std::string& includeFileName() const { return _includeFileName; }

    /// Return the includeIsBuiltin parameter passed to the constructor.
    bool includeIsBuiltin() const { return _includeIsBuiltin; }

    /// Return the modelBase parameter passed to the constructor.
    const std::string& modelBase() const { return _modelBase; }

    // methods to pull out stuff from the AST

    /// Full name (i.e., including namespace qualifier).
    std::string fullName() const;

    /// True if public, false if private.
    bool isPublic() const { return _isPublic; }

    /// True if stateful, false if stateless.
    bool isStateful() const { return _isStateful; }

    /// True if the function calls at least one other function that is stateful.
    bool makesStatefulCall() const { return _makesStatefulCall; }

    /// Set the flag for calling a stateful function to true.
    void setMakesStatefulCall() { _makesStatefulCall = true; }

    /// IR encapsulating the body for the compiler back-end.
    virtual BlockStatement* functionBodyIR() const { return _ir; }

    /// The namespace in which this FunctionHeadSym is defined.
    virtual std::string nameSpace() const;

    /// Return type.
    RootTyp const& returnType() const
    {
        assert(NULL != _returnType);
        return *_returnType;
    }

    /// Return type.
    RootTyp const* returnTypePtr() const { return _returnType; }

    /// If this function is not native, the name of the SPL file in which it is defined.
    std::string const& splFileName() const { return _splFileName; }

    // methods inherited from SymbolTableEntry

    /// Abstract syntax tree, if prototype.
    virtual AstNode& ast() const;

    /// Is the call to ast() allowed?
    virtual bool hasAst() const { return NULL != _splAst; }

    /// Scope of the generic and regular formals, as well as the local
    /// variables in the top-level body block for non-native functions.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (i.e., without the namespace qualifier).
    virtual std::string name() const { return _name; }

    /// Scope of the compilation unit in which this function is defined).
    virtual Scope* parentScope() const;

    /// E.g., 'FunctionHeadSym(foo :: <any T> stateful void f(list<T> x, mutable int32 y))'.
    virtual std::ostream& print(std::ostream&) const;

    /// E.g., '<any T> stateful void f(list<T> x, mutable int32 y)'.
    virtual std::ostream& printBare(std::ostream&) const;

    /// Do not call this.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp const*)NULL);
    }

    /// toolkit index
    const uint32_t getToolkitIndex() const { return _toolkitIndex; }

    /// Is there an overridden C++ namespace for a native function?
    /// @return true if this native function has it's own C++ namespace (not using SPL one)
    bool hasCppNamespace() const { return !_cppNamespace.empty(); }

    /// Return the overridden C++ namespace for a native function
    /// @return C++ namespace for native function call
    const std::string& getCppNamespace() const { return _cppNamespace; }

    /// Is there an overridden C++ function name for a native function?
    /// @return true if this native function has a different C++ function name (not using SPL one)
    bool hasCppFunctionName() const { return !_cppFunctionName.empty(); }

    /// Return the overridden C++ function name for a native function
    /// @return C++ function name for native function call
    const std::string& getCppFunctionName() const { return _cppFunctionName; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(FunctionHeadSym)

/// Symbol table entry for a formal parameter.
class FunctionFormalSym : public SymbolTableEntry
{
    mutable Scope* _heldScope;
    bool _isMutable;
    std::string _name;
    Scope* _parentScope;
    RootTyp const* _type;

  public:
    friend class FunctionHeadSym;
    /// Constructor.
    FunctionFormalSym(SourceLocation const&, Scope*, AstNode&);

    /// We refrain from storing a pointer from this symbol table entry to its AST,
    /// to simplify memory management by allowing the AST to be reclaimed first.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// The name is mandatory for SPL function definitions, but optional for native function
    /// prototypes (declarations).
    virtual bool hasName() const { return "" != _name; }

    /// If parameter type is a tuple, scope with the attributes, else NULL.
    virtual Scope* heldScope() const;

    /// True if mutable, false if immutable.
    virtual bool isMutable() const { return _isMutable; }

    /// Name, if any.
    virtual std::string name() const;

    /// Scope of the FunctionHeadSym that contains this formal.
    virtual Scope* parentScope() const { return _parentScope; }

    /// E.g. 'FunctionFormalSym(<anonymous>, int32)'.
    virtual std::ostream& print(std::ostream&) const;

    /// Declared type.
    virtual RootTyp const& type() const { return *_type; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(FunctionFormalSym)

/// Symbol table entry for a group of FunctionHeadSym in the same
/// namespace with the same name but different parameter types.
class DLL_PUBLIC OverloadedFunctionsSym : public SymbolTableEntry
{
  public:
    typedef std::vector<FunctionHeadSym*> Functions;

  private:
    Functions _functions;
    std::string _name;
    Scope* _parentScope;

  public:
    /// Constructor.
    OverloadedFunctionsSym(std::string const&, Scope*);

    /// Add, or replace declaration by definition, or error if conflict.
    virtual void addFunction(FunctionHeadSym&);

    /// An OverloadedFunctionsSym does not correspond to any single AST.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Functions in this group of overloaded functions.
    Functions const& functions();

    /// An OverloadedFunctionsSym does not have a scope.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple common name of all the functions in this OverloadedFunctionsSym.
    virtual std::string name() const { return _name; }

    /// Scope of the namespace that contains this symbol.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Do not call this.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp const*)NULL);
    }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(OverloadedFunctionsSym)
};

#endif /*SPL_FUNCTION_SYMBOL_TABLE_ENTRIES_H*/
