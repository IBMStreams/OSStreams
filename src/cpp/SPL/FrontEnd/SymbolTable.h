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

#ifndef SPL_SYMBOL_TABLE_H
#define SPL_SYMBOL_TABLE_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>
#include <antlr3interfaces.h>

#include <cassert>
#include <vector>

namespace SPL {
typedef ANTLR3_BASE_TREE_struct AstNode;
class RootTyp;
class CallExpressionSym;
class CompilationUnitSym;
class CompositeDefSym;
class CompositeInstanceSym;
class FunctionFormalSym;
class FunctionHeadSym;
class NamespaceSym;
class OpInvokeSym;
class OtherScopeHolderSym;
class ParserContext;
class Scope;
class SymbolTableEntry;

/// Does this scope belong to param subscription of an Import operator, or
/// filter of an Export operator?
bool isInSubscriptionOrFilterExpression(Scope& scope);

/// Is the symbol something that can be used to denote an attribute?
bool symIsAttribute(SymbolTableEntry const& sym);

/// Is the symbol mutable?
bool symIsMutable(SymbolTableEntry const& sym, std::vector<AstNode*>& loopNestingContext);

/// Is the symbol something that can be used to denote a stream?
bool symIsStream(SymbolTableEntry const& sym);

/// Is the symbol something that can be used to denote a tuple type?
bool symIsTupleType(SymbolTableEntry const& sym);

/// Is the symbol something that can be used to denote a type?
bool symIsType(SymbolTableEntry const& sym);

/// Is the symbol something that can be used to denote a value?
bool symIsValue(SymbolTableEntry const& sym);

/// Print a type to an output stream.
std::ostream& operator<<(std::ostream&, RootTyp const&);

/// The symbol table maintains a tree of scopes, and each scope maps
/// identifiers to instances of subclasses of SymbolTableEntry.
class SymbolTable : public Singleton<SymbolTable>
{
  private:
    Scope* _commonCompositeDefScope;
    Scope* _commonWindowScope;
    Scope* _configLabelScope;
    Scope* _currentScope;
    std::vector<Scope*> _excursionStack;
    Scope* _logicScopeCurrentPunct;
#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        Scope * _logicScopeCurrentWatermark;
#endif
    Scope* _logicScopeSubmit;
    Scope* _topLevelScope;
    Scope* _intrinsicScope;
    void initCommonCompositeDefScope();
    void initCommonWindowScope();
    void initConfigLabelScope();
    void initLogicScopes();
    void initIntrinsicScope();
    void initScope(Scope* scope, const char** names);
    typedef std::vector<AstNode*> AstNodeList;
    typedef std::tr1::unordered_map<std::string, AstNodeList*> BundleMapper;
    BundleMapper _bundleMapper;
    FunctionHeadSym* _getToolkitDirectorySym;

    void checkVisibilityComposite(ParserContext const& pCtx, AstNode& ast, CompositeDefSym& oper);
    SymbolTableEntry* useInNamespace(std::string const& nameSpaceName,
                                     std::string const& simpleName);
    SymbolTableEntry* useInNamespaceFromSymbolTable(std::string const&, std::string const&);
    SymbolTableEntry* useInner(ParserContext const&, AstNode&, bool checkForShadow);

  public:
    /// Constructor, creates new empty symbol table.
    SymbolTable();

    /// Scope that contains definitions in common to all composite operators.
    Scope& commonCompositeDefScope() const { return *_commonCompositeDefScope; }

    /// Scope that contains definitions in common to all window clauses.
    Scope& commonWindowScope() const { return *_commonWindowScope; }

    /// Compilation unit enclosing current scope, if any.
    CompilationUnitSym const* compilationUnit() const;

    /// Scope of compilation unit enclosing current scope, if any.
    Scope* compilationUnitScope() const;

    /// Scope of intrinsic symbols available for given config label.
    Scope& configScope(std::string const&) const;

    /// Composite operator definition lexically enclosing current scope, error if none.
    CompositeDefSym const& currentCompositeDef() const;

    /// Composite operator instance lexically enclosing current scope, error if none.
    CompositeInstanceSym const& currentCompositeInstance() const;

    /// Namespace lexically enclosing current scope, error if none.
    NamespaceSym& currentNamespace() const;

    /// Operator invocation lexically enclosing current scope, error if none.
    OpInvokeSym const& currentOpInvoke() const;

    /// Current scope, based on pushing and popping scopes in tandem with traversing an AST with a
    /// tree grammar.
    Scope& currentScope() const
    {
        assert(NULL != _currentScope);
        return *_currentScope;
    }

    /// Create a new CompilationUnitSym and define it in the current scope.
    CompilationUnitSym& defCompilationUnit(ParserContext const&, AstNode*);

    /// Create a new DefTypeSym and define it in the current scope.
    void defDefType(ParserContext const&, AstNode&);

    /// Create a new FunctionFormalSym and define it in the current scope if it has a name.
    FunctionFormalSym& defFunctionFormal(ParserContext const&, AstNode&);

    /// Create a new NamespaceSym and define it in the current scope.
    NamespaceSym& defNamespace(std::string);

    /// Create a new PortAliasSym and define it in the current scope.
    void defPortAlias(ParserContext const&, AstNode&);

    /// Create a new StreamSym and define it in the current scope.
    void defStream(ParserContext const&, AstNode&);

    /// Create a new VariableSym and define it in the current scope.
    void defVariable(ParserContext const&, AstNode&, AstNode&);

    /// Instead of moving currentScope gradually with push/pop, jump abruptly
    /// to another scope, so future definitions happen there.
    Scope& excursionBegin(Scope&);

    /// Return to the scope active before the previous excursionBegin.
    /// Error if there is a mismatch.
    void excursionEnd(Scope&);

    /// Function enclosing current scope, if any.
    FunctionHeadSym* function() const;

    /// Scope of function enclosing current scope, if any.
    Scope* functionScope() const;

    /// Is the AST an identifier that resolves to a DefTypeSym in the current scope?
    bool isTypeId(AstNode&) const;

    /// Scope of intrinsic 'currentPunct()' available in 'logic onPunct' clause.
    Scope& logicScopeCurrentPunct() const
    {
        assert(NULL != _logicScopeCurrentPunct);
        return *_logicScopeCurrentPunct;
    }

#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        /// Scope of intrinsic 'currentWatermark()' available in 'logic onWatermark' clause.
			Scope & logicScopeCurrentWatermark() const {
				assert(NULL != _logicScopeCurrentWatermark);
				return *_logicScopeCurrentWatermark;
			}
#endif
    /// Scope of intrinsics 'submit()' available in 'logic' clause of 'Custom' operator.
    Scope& logicScopeSubmit() const
    {
        assert(NULL != _logicScopeSubmit);
        return *_logicScopeSubmit;
    }

    /// Scope of other intrinsics available in SPL
    Scope& intrinsicScope() const
    {
        assert(NULL != _intrinsicScope);
        return *_intrinsicScope;
    }

    /// Pop currentScope, error if mismatch.
    void pop(AstNode&);

    /// Pop currentScope, error if mismatch.
    void pop(SymbolTableEntry&);

    /// Push scope held by the symbol as current scope, and if the AST is non-null, decorate it with
    /// the symbol.
    void push(AstNode*, SymbolTableEntry&);

    /// Define new CompilationUnitSym and push its scope.
    CompilationUnitSym& pushCompilationUnit(ParserContext const&, AstNode&);

    /// Define new CompilationUnitSym and push its scope.
    CompilationUnitSym& pushCompilationUnit(ParserContext const&);

    /// Define new FunctionHeadSym and push its scope.
    FunctionHeadSym& pushFunction(ParserContext const&, AstNode&);

    /// Define new NamespaceSym and push its scope.
    NamespaceSym& pushNamespace(AstNode&);

    /// Define new NamespaceSym and push its scope.
    NamespaceSym& pushNamespace(std::string);

    /// Define new OtherScopeHolderSym and push its scope.
    OtherScopeHolderSym& pushNew(ParserContext const&, AstNode&);

    /// Re-push the scope of the symbol decorating the given AST node.
    void pushOld(AstNode&);

    /// Define new OpInvokeSym and push its scope.
    void pushOpInvoke(ParserContext const&, AstNode&);

    /// Define new TupleAttribSym and push its scope.
    void pushTupleAttrib(ParserContext const&, AstNode&);

    /// Return the top-level scope (there is only one, and it contains the namespaces).
    Scope& topLevelScope() const;

    /// Based on the identifier in the AST, look up a FormalConfigSym in the configLabelScope;
    /// if success, decorate the AST, otherwise, print error message and place ErrorDummySym.
    void useConfig(ParserContext const&, AstNode&);

    /// Look up a name on the rhs of an opInvokeActual. If the formal has expression mode
    /// 'function', decorate the AST with that formal for later resolution after parameter
    /// expansion, else resolve it now.
    void useOpActualName(ParserContext const&, AstNode&);

    /// Look up an identifier on the lhs of an opInvokeActual.
    void useParam(ParserContext const&, AstNode& paramNode);

    /// Look up a qualified identifier (i.e., namespace::simpleId);
    /// if success, decorate the AST, otherwise, print error message and place ErrorDummySym.
    void useQualifiedId(ParserContext const&, AstNode&);

    /// Look up a qualified identifier (i.e., namespace::simpleId) and ensure it refers to an
    /// operator; if success, decorate the AST, otherwise, print error message and place
    /// ErrorDummySym.
    void useQualifiedOperator(ParserContext const&, AstNode&);

    /// Look up a qualified identifier (i.e., namespace::simpleId) and ensure it refers to an
    /// operator or an enum; if success, decorate the AST, otherwise, print error message and place
    /// ErrorDummySym.
    void useQualifiedOperatorOrEnum(ParserContext const&, AstNode&);

    /// Look up a qualified identifier (i.e., namespace::simpleId) and ensure it refers to an type;
    /// if success, decorate the AST, otherwise, print error message and place ErrorDummySym.
    void useQualifiedType(ParserContext const&, AstNode&);

    /// Look up a simple identifier (i.e., not qualified) from the current scope;
    /// if success, decorate the AST, otherwise, print error message and place ErrorDummySym.
    void useSimpleId(ParserContext const&, AstNode&, bool checkForShadow);

    /// If the simple identifier starts with a '$', look it up from the current scope;
    /// if success, decorate the AST, otherwise, print error message and place ErrorDummySym.
    void useSimpleIdIfDollar(ParserContext const&, AstNode&);

    /// Look up a simple identifier (i.e., not qualified) from the current scope and ensure it
    /// refers to an operator; if success, decorate the AST, otherwise, print error message and
    /// place ErrorDummySym.
    void useSimpleOperatorId(ParserContext const&, AstNode&);

    /// Look up a simple identifier (i.e., not qualified) from the current scope and ensure it
    /// refers to a stream; if success, decorate the AST, otherwise, print error message and place
    /// ErrorDummySym.
    void useSimpleStreamId(ParserContext const&, AstNode&);

    /// Look up a simple identifier (i.e., not qualified) from the current scope and ensure it
    /// refers to a type; if success, decorate the AST, otherwise, print error message and place
    /// ErrorDummySym.
    void useSimpleTypeId(ParserContext const&, AstNode&);

    /// Look up a simple identifier (i.e., not qualified) from the current scope and ensure it
    /// refers to a value; if success, decorate the AST, otherwise, print error message and place
    /// ErrorDummySym.
    void useSimpleValueId(ParserContext const&, AstNode&);

    /// Issue detail messages about unknown symbol
    /// @pre sym.is<ErrorDummySym>()
    void checkForMissingSymbol(ParserContext const&, AstNode&, SymbolTableEntry& sym);

    // Get a sym for the getToolkitDirectory function
    const FunctionHeadSym& getToolkitDirectorySym() const
    {
        assert(NULL != _getToolkitDirectorySym);
        return *_getToolkitDirectorySym;
    }
};
};

#endif /*SPL_SYMBOL_TABLE_H*/
