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

#ifndef SPL_SYMBOL_TABLE_ENTRIES_H
#define SPL_SYMBOL_TABLE_ENTRIES_H

#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>

#include <cassert>

#if NDEBUG
#define AST_SYM_ASSERT
#else
#define AST_SYM_ASSERT                                                                             \
    {                                                                                              \
        SymbolTableEntry* e = astSymbolPtr(_astNode);                                              \
        if (e && e != this) {                                                                      \
            std::cerr << "Sym/AST mismatch, ast: " << _astNode << "\n   sym: " << *e               \
                      << "\n   this: " << *this << std::endl;                                      \
            assert(false);                                                                         \
        }                                                                                          \
    }
#endif

namespace SPL {
class BoundsFormal;
class CompositeDefinition;
class CompositeInstanceSym;
class CompositeOperatorInstance;
class DeclStatement;
class EnumTyp;
class Expression;
class SPLAnnotation;
class Statement;
class SymbolTable;
class TypeFormal;

/// Symbol for an actual config, i.e., a concrete use of a FormalConfigSym in an operator.
class ActualConfigSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;
    bool _isExpanded;

  public:
    /// Constructor.
    ActualConfigSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Add all identifier of the corresponding FormalConfigSym to own scope.
    virtual void expand();

    /// Scope held by this symbol.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Name of the config label.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// No type.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
};
SYM_IS_AS_SPECIALIZATIONS(ActualConfigSym)

/// Symbol for an 'base.attrId' expression.
class AttributeAccessSym : public SymbolTableEntry
{
    AstNode& _astNode;

  public:
    /// Constructor.
    AttributeAccessSym(SourceLocation const&, AstNode&);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Symbol for the attribute being accessed.
    virtual SymbolTableEntry& attrib() const;

    /// Symbol for the base on which the attribute is defined.
    virtual SymbolTableEntry& base() const;

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Simple name (excluding qualifier) of this symbol.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const;

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(AttributeAccessSym)

/// Symbol for an 'attrId = expr' part of a tuple literal.
class AttributeAssignSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _parentScope;

  public:
    /// Constructor.
    AttributeAssignSym(SourceLocation const&, AstNode&, Scope*);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(AttributeAssignSym)

/// Symbol for an 'type attrId' part of a tuple type.
class AttributeDeclSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _parentScope;
    std::string _name;

  public:
    /// Constructor.
    AttributeDeclSym(SourceLocation const&, AstNode&, Scope*);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Simple name (excluding qualifier) of this symbol.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol.
    virtual RootTyp const& type() const;

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(AttributeDeclSym)

/// Symbol for an attribute derived not from an AST, but rather from a RootTyp.
class AttributeFromTypeSym : public SymbolTableEntry
{
    Scope* _heldScope;
    std::string const& _name;
    Scope& _parentScope;
    RootTyp const& _type;

  public:
    /// Constructor.
    AttributeFromTypeSym(SourceLocation const&, Scope&, std::string const&, RootTyp const&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return &_parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol.
    virtual RootTyp const& type() const { return _type; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(AttributeFromTypeSym)

/// Symbol for a '[N]' formal generic parameter denoting a bound for a
/// bounded string or collection.
class BoundsFormalSym : public SymbolTableEntry
{
    BoundsFormal& _bound;

  public:
    /// Constructor.
    BoundsFormalSym(SourceLocation const&, Scope&, AstNode&);
    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// The actual bound.
    /// TODO: Change return type to a reference
    virtual BoundsFormal* bound() const { return &_bound; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const;

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(BoundsFormalSym)

/// Symbol for a compilation unit, i.e., an entire SPL file.
class CompilationUnitSym : public SymbolTableEntry
{
  public:
    typedef std::tr1::unordered_set<std::string> StringSet;
    typedef std::tr1::unordered_map<std::string, std::string> String2String;

  private:
    AstNode* _astNode;
    Scope* _heldScope;
    String2String _namedUses;
    StringSet _wildcardUses;

  public:
    /// Set of namespaces implicitly in scope in every compilation unit
    /// even when there is no explicit 'use some.name.space::*'.
    static StringSet const& implicitUses();

    /// Constructor.
    CompilationUnitSym(SourceLocation const&, Scope*, AstNode* astNode);
    /// Add a use directive.
    virtual void addUseDirective(AstNode& ast);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Map from simple identifiers to qualified identifiers brought in scope by
    /// declarations of the form 'use some.name.space::simpleId'.
    virtual String2String const& namedUses() const { return _namedUses; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    /// Set of namespaces brought in scope by declarations of the form
    /// 'use some.name.space::*'.
    virtual StringSet const& wildcardUses() const { return _wildcardUses; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(CompilationUnitSym)

/// Symbol for a composite operator definition.
class CompositeDefSym : public OperatorSym
{
    AstNode& _astNode;
    Scope* _heldScope;
    AstNode& _instanceBodyAst;
    CompositeDefinition mutable* _ir;
    Scope* _paramsScope;
    void ensureNoDuplicateClauses();
    void ensureNoDuplicatePorts();

  public:
    /// Constructor.
    CompositeDefSym(SourceLocation const&,
                    AstNode& ast,
                    Scope& parentScope,
                    AstNode& instanceBodyAst);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Composite operator definition IR for the compiler back-end.
    virtual CompositeDefinition& compositeDefIR() const { return *_ir; }

    /// Put each identifier from heldScope that starts with a '$' into paramsScope.
    virtual void expandParamScope();

    /// Qualified name (including qualifier) of this symbol.
    virtual std::string fullName() const;

    /// Scope held by this symbol.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Composite body AST with the instance (i.e., non-static) parts.
    virtual AstNode& instanceBodyAst() const { return _instanceBodyAst; }

    /// True if public, false if instance.
    virtual bool isPublic() const;

    /// Does the composite operator definition in the AST have an explicit 'public' modifier?
    static bool isPublic(AstNode&);

    /// Simple name (excluding qualifier) of this symbol.
    virtual std::string name() const;

    /// Scope containing the formal parameters of this operator.
    virtual Scope* paramsScope() const { return _paramsScope; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Set composite operator definition IR for the compiler back-end; error if already set.
    virtual CompositeDefinition& setCompositeDefIR(CompositeDefinition&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(CompositeDefSym)

/// Symbol for a formal parameter to a composite operator, e.g.,
/// 'param expression<int32> $formalId : 123;'.
class CompositeFormalSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;

  public:
    /// Constructor.
    CompositeFormalSym(SourceLocation const&, AstNode&, Scope*);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// One of attribute, expression, function, operator, type.
    AstNode& expressionMode() const;

    /// Determine if the expression mode for this formal is 'attribute'
    /// @return Returns true if the expression mode is attribute, otherwise false.
    bool expressionModeIsAttribute() const;

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(CompositeFormalSym)

/// Symbol for an input port to a composite operator.
class CompositeInputPortSym : public SymbolTableEntry
{
    AstNode& _actualAst;
    AstNode& _formalAst;
    CompositeInstanceSym& _instanceSym;

  public:
    /// Constructor.
    CompositeInputPortSym(SourceLocation const& loc,
                          AstNode& actualAst,
                          AstNode& formalAst,
                          CompositeInstanceSym& instanceSym);

    /// Abstract syntax tree of actual(s).
    virtual AstNode& actualAst() const { return _actualAst; }

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const { return _formalAst; }

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const;

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Find the full names of all the actual input streams, and push them into the destination
    /// buffer.
    virtual std::vector<std::string>& pushBundle(std::vector<std::string>& destBuffer) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// The instance
    const CompositeInstanceSym& instance() const { return _instanceSym; }
    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(CompositeInputPortSym)

/// Symbol for a composite operator instance.
class CompositeInstanceSym : public SymbolTableEntry
{
    StringMap<AstNode*> _actuals;
    CompositeDefSym& _def;
    std::string const _fullName;
    Scope& _heldScope;
    CompositeOperatorInstance mutable* _ir;
    StringMap<SymbolTableEntry*> _outputPorts;
    CompositeInstanceSym* _parent;
    void fixUpActual(AstNode& ast, AstNode& useAst) const;

  public:
    /// Constructor.
    CompositeInstanceSym(SourceLocation const& loc,
                         CompositeDefSym& def,
                         std::string const& fullName,
                         CompositeInstanceSym* parent);

    /// AstNode of actual parameter.
    virtual AstNode& actual(std::string const& formalName, AstNode& useAst) const;

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const { return _def.ast(); }

    /// Composite operator instance IR for the compiler back-end.
    virtual CompositeOperatorInstance& compositeInstanceIR() const
    {
        assert(NULL != _ir);
        return *_ir;
    }

    /// Define a mapping from formal parameter name to actual AST.
    virtual void defActual(std::string const& formalName, AstNode& actualAst);

    /// Define a mapping from formal output port name to actual stream.
    virtual void defOutputPort(std::string const& formalName, SymbolTableEntry& actualSym);

    /// Outside-in name of this composite instance.
    virtual std::string fullName() const { return _fullName; }

    /// Outside-in name of a stream in this composite instance.
    virtual std::string fullStreamName(std::string const& localStreamName) const;

    /// True if this instance has a value for the parameter, false otherwise.
    virtual bool hasActual(std::string const& formalName) const;

    /// Scope held by this symbol.
    virtual Scope* heldScope() const { return &_heldScope; }

    /// Simple name (excluding qualifier) of this symbol.
    virtual std::string name() const { return _def.name(); }

    /// Composite operator definition for which this is an instantiation.
    virtual CompositeDefSym& op() const { return _def; }

    /// Parent composite operator instance, from which this one was expanded.
    virtual CompositeInstanceSym* parent() const { return _parent; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Set composite operator instance IR for the compiler back-end; error if already set.
    virtual CompositeOperatorInstance& setCompositeInstanceIR(CompositeOperatorInstance&) const;

    /// No type.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    virtual void expand() {}

    // Helper functions to handle error cases
    void pushInputPortActualStreamList();
    void addInputPortActualStream(const std::string& stream);
    void addOutputPortActualStream(const std::string& stream);
    void addInputPortFormalStream(const std::string& stream);
    void addOutputPortFormalStream(const std::string& stream);
    void addOutputPortAlias(const std::string& alias);
    void addInputPortAlias(const std::string& alias);
};
SYM_IS_AS_SPECIALIZATIONS(CompositeInstanceSym)

/// Symbol for a type definition, e.g., 'type typeId = list<int32>'.
class DefTypeSym : public SymbolTableEntry
{
    typedef enum
    {
        notYet,
        inProgress,
        failure
    } ExpansionStatus;
    AstNode& _astNode;
    Scope* _parentScope;
    mutable ExpansionStatus _expansionStatus;
    SymbolTableEntry& expandedTail() const;

  public:
    /// Constructor.
    DefTypeSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Does this typedef have an explicit 'static' modifier?
    virtual bool isStatic() const;

    /// Does the typedef in the AST have an explicit 'static' modifier?
    static bool isStatic(AstNode&);

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(DefTypeSym)

/// Symbol for an enumeration type, e.g., 'enum { a, b, c }'.
class EnumTypeSym : public SymbolTableEntry
{
    Scope* _heldScope;
    std::string const* _name;
    RootTyp const& _type;

  public:
    /// Constructor.
    EnumTypeSym(SourceLocation const& loc,
                std::string const* name,
                Scope* parentScope,
                EnumTyp const& type);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Is this the type of a custom literal in a primitive operator?
    virtual bool isCustomLiteral() const { return NULL != _name; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const
    {
        assert(NULL != _name);
        return *_name;
    }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol.
    virtual RootTyp const& type() const { return _type; }

    /// Type of this AST.
    static EnumTyp const& type(AstNode&);

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(EnumTypeSym)

/// Symbol for one of the values in an enum type, e.g., symbol for 'a' in 'enum {a, b, c}'.
class EnumValueSym : public SymbolTableEntry
{
    std::string _name;
    Scope& _parentScope;
    EnumTypeSym& _owningEnum;

  public:
    /// Constructor.
    EnumValueSym(SourceLocation const& loc,
                 std::string const& name,
                 Scope& parentScope,
                 EnumTypeSym& owningEnum);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Is this the value of a custom literal in a primitive operator?
    virtual bool isCustomLiteral() const;

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return &_parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    virtual void expand() {}

    /// Access the owning EnumTypeSym
    EnumTypeSym& owningEnumTypeSym() const { return _owningEnum; }
};
SYM_IS_AS_SPECIALIZATIONS(EnumValueSym)

/// Symbol to suppress further error messages when the front-end continues after
/// some name look-up failed.
class ErrorDummySym : public SymbolTableEntry
{
    AstNode& _astNode;
    std::string _name;
    Scope* _parentScope;

  public:
    /// Constructor.
    ErrorDummySym(SourceLocation const&, AstNode&, std::string, Scope*);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(ErrorDummySym)

/// Symbol for an eventTime annotation.
class EventTimeAnnotationSym : public SymbolTableEntry
{
    AstNode& _astNode;
    AstNode& attribute_;
    AstNode* resolution_;
    AstNode* minimumGap_;
    AstNode* lag_;
    Scope* heldScope_;
    Scope* attributeScope_;
    Scope* resolutionScope_;
    enum ScopeMode
    {
        Held,
        Attribute,
        Resolution
    };
    ScopeMode mode_;

  public:
    EventTimeAnnotationSym(SourceLocation const& loc,
                           AstNode& ast,
                           AstNode& attributeAst,
                           AstNode* resolutionAst,
                           AstNode* minimumGapAst,
                           AstNode* lagAst,
                           Scope& parentScope);

    virtual ~EventTimeAnnotationSym();

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Name of the annotation
    virtual std::string name() const { return "eventTime"; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const
    {
        return mode_ == Attribute ? attributeScope_
                                  : mode_ == Resolution ? resolutionScope_ : heldScope_;
    }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return 0; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream& s) const;

    /// Has no type
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
    virtual void expand() {}

    AstNode& eventTimeAttribute() const { return attribute_; }
    AstNode* resolution() const { return resolution_; }
    AstNode* minimumGap() const { return minimumGap_; }
    AstNode* lag() const { return lag_; }

    Scope& attributeScope() { return *attributeScope_; }
    void setAttributeScopeMode() { mode_ = Attribute; }
    Scope& resolutionScope() { return *resolutionScope_; }
    void setResolutionScopeMode() { mode_ = Resolution; }
    void resetScopeMode() { mode_ = Held; }
};
SYM_IS_AS_SPECIALIZATIONS(EventTimeAnnotationSym)

/// Symbol for any other expression or literal that doesn't have its own subclass of
/// SymbolTableEntry.
class ExpressionSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;
    RootTyp const* _type;

  public:
    /// Constructor.
    ExpressionSym(SourceLocation const&, AstNode&, RootTyp const&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const
    {
        assert(false);
        return *((std::string*)NULL);
    }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const { return *_type; }

    // used to promote types during semantic analysis expression types
    virtual void setType(RootTyp const& t) { _type = &t; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(ExpressionSym)

/// Symbol for a formal config, i.e., what any ActualConfigSym should eventually resolve to.
class FormalConfigSym : public SymbolTableEntry
{
    Scope* _heldScope;
    std::string _name;

  public:
    /// Constructor.
    FormalConfigSym(Scope* parentScope, const std::string& name);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(FormalConfigSym)

/// Symbol for a host-pool. E.g., in 'config hostPool : SourcePool = ["localhost", "10.8.5.6"],
/// ComputingPool = createPool(15);', each of 'SourcePool' and 'ComputingPool' gives rise to a
/// HostPoolSym.
class HostPoolSym : public SymbolTableEntry
{
    AstNode& _astNode;

  public:
    /// Constructor.
    HostPoolSym(SourceLocation const&, AstNode&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const;

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(HostPoolSym)

/// Place-holder for a symbol from another scope that is visible from an operator invocation. For
/// example, the tuple attributes of streams are visible in the scope of an operator invocation that
/// is connected to them.
class IndirectSym : public SymbolTableEntry
{
    typedef std::tr1::unordered_map<SymbolTableEntry*, SymbolTableEntry*> MultiDefMap;
    Scope mutable* _heldScope;
    MultiDefMap _multiDefMap;
    std::string _name;

  public:
    /// Constructor.
    IndirectSym(const std::string& name);

    /// Add a symbol that this symbol is indirecting to.
    virtual void addDef(SymbolTableEntry& stream, SymbolTableEntry& attr);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Where does the indirected-to symbol come from originally?
    virtual SymbolTableEntry* base() const;

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Does this symbol indirect to more than one symbol?
    virtual bool isAmbiguous() const { return 1 != _multiDefMap.size(); }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// E.g., 'base1.id1, base2.id2'.
    virtual std::ostream& printBare(std::ostream&) const;

    /// Result of printBare() as a string.
    virtual std::string toString() const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    /// First symbol that this symbol indirects to.
    virtual SymbolTableEntry& firstSym() const;

    /// AST of the first symbol that this symbol indirects to.
    virtual AstNode& getFirstAst() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(IndirectSym)

/// Symbol for a list type, e.g., 'list<int32>'.
class ListTypeSym : public SymbolTableEntry
{
    AstNode& _astNode;
    RootTyp mutable const* _type;

  public:
    /// Constructor.
    ListTypeSym(SourceLocation const&, AstNode&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(ListTypeSym)

/// Symbol for a map type, e.g., 'map<rstring, int32>'.
class MapTypeSym : public SymbolTableEntry
{
    AstNode& _astNode;
    RootTyp mutable const* _type;

  public:
    /// Constructor.
    MapTypeSym(SourceLocation const&, AstNode&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(MapTypeSym)

/// Symbol for an SPL namespace. The scope of a namespace contains
/// compilation units and overloaded function groups.
class NamespaceSym : public SymbolTableEntry
{
    Scope* _heldScope;
    std::string _name;

  public:
    /// Constructor.
    NamespaceSym(const std::string&, Scope*);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Namespace name of this symbol.
    virtual std::string name() const { return _name; }

    /// Namespace name of this AST.
    static std::string name(AstNode&);

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(NamespaceSym)

/// Symbol for a 'logic onPunct streamId : stmt' clause.
class OnPunctLogicSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;
    bool _isExpanded;

  public:
    /// Constructor.
    OnPunctLogicSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Add the intrinsics for currentPunct() and, in case of Custom, submit().
    virtual void expand();

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
};
SYM_IS_AS_SPECIALIZATIONS(OnPunctLogicSym)

#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        /// Symbol for a 'logic onWatermark streamId : stmt' clause.
	class OnWatermarkLogicSym: public SymbolTableEntry {
			AstNode & _astNode;
			Scope * _heldScope;
			bool _isExpanded;
		public:
			/// Constructor.
			OnWatermarkLogicSym(SourceLocation const &, AstNode &, Scope * parentScope);

			/// Abstract syntax tree of definition of this symbol, if any.
			virtual AstNode & ast() const {
				AST_SYM_ASSERT
				;
				return _astNode;
			}

			/// Add the intrinsics for currentWatermark() and, in case of Custom, submit().
			virtual void expand();

			/// Scope held by this symbol, if any.
			virtual Scope * heldScope() const {
				return _heldScope;
			}

			/// Simple name (excluding qualifier) of this symbol, if any.
			virtual std::string name() const;

			/// Parent scope in which this symbol is defined.
			virtual Scope * parentScope() const {
				return heldScope()->parentScope();
			}

			/// Used internally by compiler developers for diagnostic messages.
			virtual std::ostream & print(std::ostream &) const;

			/// Type of this symbol, if any.
			virtual RootTyp const & type() const {
				assert(false);
				return *((RootTyp *) NULL);
			}
	};
	SYM_IS_AS_SPECIALIZATIONS(OnWatermarkLogicSym)
#endif
/// Symbol for a 'logic onTuple streamId : stmt' clause.
class OnTupleLogicSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;
    bool _isExpanded;

  public:
    /// Constructor.
    OnTupleLogicSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Add the attributes of the tuple and, in case of Custom, intrinsic submit().
    virtual void expand();

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
};
SYM_IS_AS_SPECIALIZATIONS(OnTupleLogicSym)

/// Symbol for a 'logic onProcess : stmt' clause.
class OnProcessLogicSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;
    bool _isExpanded;

  public:
    /// Constructor.
    OnProcessLogicSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Add the attributes of the tuple and, in case of Custom, intrinsic submit().
    virtual void expand();

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
};
SYM_IS_AS_SPECIALIZATIONS(OnProcessLogicSym)

/// Symbol for an actual parameter in an operator invocation, e.g., 'param filter : true'.
class OpInvokeActualSym : public SymbolTableEntry
{
    AstNode& _astNode;
    SymbolTableEntry* _formal;
    Scope* _heldScope;
    bool _isExpanded;

  public:
    /// Constructor.
    OpInvokeActualSym(SourceLocation const&, AstNode&, Scope*);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Add all the indirect symbols for attributes in input ports, and enumerations.
    virtual void expand(AstNode& opInvoke);
    virtual void expand() {}

    /// Formal operator parameter for this actual.
    SymbolTableEntry* formal() const { return _formal; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Set the formal operator parameter for this actual.
    virtual void setFormal(SymbolTableEntry* formal);

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;
};
SYM_IS_AS_SPECIALIZATIONS(OpInvokeActualSym)

/// Symbol for an output clause in an operator invocation,
/// e.g., 'output streamId : attrId1 = expr1, attrId2 = expr2'.
class OpInvokeOutputSym : public SymbolTableEntry
{
    AstNode& _astNode;
    bool _isExpanded;
    bool _isOnLeft;
    Scope* _lhsScope;
    Scope* _rhsScope;

  public:
    /// Constructor.
    OpInvokeOutputSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Add all the attributes in output port to lhsScope, and all the attribuets in input ports to
    /// rhsScope.
    virtual void expand(AstNode& opInvoke);
    virtual void expand() {}

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _isOnLeft ? _lhsScope : _rhsScope; }

    /// Name of the output port.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Activate the lhsScope for the 'attrId = ' part.
    virtual void switchLeft();

    /// Activate the rhsScope for the '= expr' part.
    virtual void switchRight();

    /// No type.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
};
SYM_IS_AS_SPECIALIZATIONS(OpInvokeOutputSym)

/// Symbol for an operator invocation.
class OpInvokeSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _exprScope;
    Scope* _heldScope;
    bool _isExpanded;
    std::vector<const SPLAnnotation*> _annotations;
    void ensureNoDuplicateClauses();
    void expandAttributes();
    void expandEnumerations();

  public:
    /// Constructor.
    OpInvokeSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Figure out the attributes of input ports, and the enumerations of parameters in case of a
    /// primitive operators.
    virtual void expand();

    /// Scope for any expression occuring in the various clauses.
    virtual Scope& exprScope() const { return *_exprScope; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// The primitive or composite operator being invoked.
    virtual OperatorSym* op() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Determine if nested Custom Output Functions are allowed to nest
    /// @param port The port we are interested in
    /// @return Returns true if the given port supports nested output functions, otherwise false.
    bool nestedCOFsAllowed(unsigned port) const;

    /// Hang an annotations off this symbol that are specified for composite invocations
    void addAnnotation(const SPLAnnotation& annot) { _annotations.push_back(&annot); }

    /// Access any annotations that have been associated with this composite
    const std::vector<const SPLAnnotation*>& annotations() const { return _annotations; }
};
SYM_IS_AS_SPECIALIZATIONS(OpInvokeSym)

/// Symbol for a window clause in an operator invocation, e.g., 'window Bid : sliding, time(30)'.
class OpInvokeWindowSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;
    bool _isExpanded;

  public:
    /// Constructor.
    OpInvokeWindowSym(SourceLocation const&, AstNode&, Scope* parentScope, Scope& windowScope);
    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Bring into scope the various intrinsics for windows, like 'size()'.
    virtual void expand();

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Name of the input port.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
};
SYM_IS_AS_SPECIALIZATIONS(OpInvokeWindowSym)

/// Symbol for a type declared optional<T>
class OptionalTypeSym : public SymbolTableEntry
{
  public:
    OptionalTypeSym(SourceLocation const& loc, AstNode& childType);

    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }
    virtual bool hasAst() const { return false; }
    virtual Scope* heldScope() const { return NULL; }
    virtual std::string name() const;
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }
    virtual std::ostream& print(std::ostream&) const;
    virtual RootTyp const& type() const;
    virtual void expand() {}

  private:
    AstNode& _childType;
    mutable const RootTyp* _type;
};

/// Symbol for anything that has a scope but doesn't have its own dedicated subclass of
/// SymbolTableEntry, i.e.: block, for loop, tuple literal, or scope common to all windows.
class OtherScopeHolderSym : public SymbolTableEntry
{
    AstNode* _astNode;
    Scope* _heldScope;

  public:
    /// Constructor.
    OtherScopeHolderSym(SourceLocation const&, AstNode*, Scope*);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(NULL != _astNode);
        return *_astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const
    {
        assert(false);
        return *((std::string*)NULL);
    }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(OtherScopeHolderSym)

/// Symbol for anything else that has a name but doesn't have its own dedicated subclass of
/// SymbolTableEntry, e.g., a bare identifier in a config.
class OtherSym : public SymbolTableEntry
{
    AstNode* _astNode;
    std::string _name;
    Scope* _parentScope;
    RootTyp const& _type;

  public:
    /// Constructor.
    OtherSym(SourceLocation const&,
             const std::string&,
             Scope*,
             AstNode* astNode,
             RootTyp const* type);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(NULL != _astNode);
        return *_astNode;
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const { return _type; }

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(OtherSym)

class UnwrapReferenceSym : public SymbolTableEntry
{
    AstNode& _astNode;
    AstNode& _lhs;
    std::string _name;
    Scope* _parentScope;
    RootTyp const& _type;

  public:
    /// Constructor.
    UnwrapReferenceSym(SourceLocation const&,
                       Scope*,
                       AstNode& astNode,
                       AstNode& lhs,
                       RootTyp const& type);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        // assert(NULL != _astNode);
        return _astNode;
    }

    virtual bool hasAst() const { return true; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const { return _type; }

    virtual void expand() {}

    SymbolTableEntry& getLhsSymbol() const;
};
SYM_IS_AS_SPECIALIZATIONS(UnwrapReferenceSym)

class IsPresentReferenceSym : public SymbolTableEntry
{
    AstNode& _astNode;
    AstNode& _lhs;

    std::string _name;
    Scope* _parentScope;
    RootTyp const& _type;

  public:
    /// Constructor.
    IsPresentReferenceSym(SourceLocation const&,
                          Scope*,
                          AstNode& astNode,
                          AstNode& lhs,
                          RootTyp const& type);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        // assert(NULL != _astNode);
        return _astNode;
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const { return _type; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(IsPresentReferenceSym)

class ParallelAnnotationSym : public SymbolTableEntry
{
  public:
    class Partition
    {
      public:
        Partition(AstNode& portIn, AstNode* attributesIn)
          : _port(&portIn)
          , _attributes(attributesIn)
        {}
        AstNode& port() const { return *_port; }
        AstNode& attributes() const { return *_attributes; }

      private:
        AstNode* _port;
        AstNode* _attributes;
    };
    typedef std::vector<Partition> PartitionList;
    typedef std::vector<std::string> BroadcastList;

    /// Constructor
    ParallelAnnotationSym(SourceLocation const& loc,
                          AstNode& inast,
                          AstNode& widthIn,
                          AstNode* hostTagsIn,
                          const PartitionList& partitionsIn,
                          AstNode* broadcastsIn,
                          Scope& parentScopeIn);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const { return _astNode; }

    /// Name of the annotation
    virtual std::string name() const { return "parallel"; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream& s) const;

    /// Has no type
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    /// Expand the scope
    virtual void expand();

    /// Access the ast for width
    AstNode& width() const { return _width; }

    /// Access the ast for host tags
    AstNode* hostTags() const { return _hostTags; }

    /// Access the set of partition values
    const PartitionList& partitions() const { return _partitions; }

    AstNode* broadcasts() const { return _broadcasts; }

  private:
    AstNode& _astNode;
    AstNode& _width;
    AstNode* _hostTags;
    Scope* _heldScope;
    PartitionList _partitions;
    AstNode* _broadcasts;
};
SYM_IS_AS_SPECIALIZATIONS(ParallelAnnotationSym)

/// Symbol for a port alias, as declared by an 'as' clause in the input or output of an operator
/// invocation.
class PortAliasSym : public SymbolTableEntry
{
    AstNode& _portAst;
    SymbolTableEntry& firstStreamSym() const;

  public:
    /// Constructor.
    PortAliasSym(SourceLocation const&, AstNode&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const;

    /// Is the call to ast() allowed?
    virtual bool hasAst() const { return true; }

    /// Turn this expression into the IR for the compiler back-end.
    Expression* genExpression(SourceLocation const&) const;

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(PortAliasSym)

/// Symbol for a primitive operator.
class PrimitiveOperatorSym : public OperatorSym
{
    typedef SPL::Operator::OperatorModel Model;
    typedef SPL::Operator::OutputPort Port;
    typedef std::tr1::unordered_map<std::string, Scope*> ScopeMap;
    typedef std::vector<Scope*> ScopeVector;
    ScopeVector _ofScopes; // scopes for output clauses
    ScopeMap _pfScopes;    // scope for parameters
    Scope* _enumsScope;
    Scope* _heldScope;
    Model const& _model;
    std::string _name;
    Scope* _paramsScope;
    uint32_t _toolkitIndex;
    void initOutputFunctions(ScopeMap&);
    void initOutputFunctionsPort(ScopeMap&, int idx, const Port&);
    void initOutputFunctionsScopes(ScopeMap&);
    void initEnumerations();
    void initParameters(const ScopeMap&);

  public:
    /// Constructor.
    PrimitiveOperatorSym(Model const&,
                         const std::string& name,
                         Scope* parentScope,
                         uint32_t toolkitIndex);

    /// Scope with the output functions (e.g., Avg, Max), if any.
    virtual Scope* ofsScope(int portId) const;

    /// Scope with the output functions for parameters (e.g., Avg, Max), if any.
    virtual Scope* ofsScope(const std::string& paramName) const;

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope for the enumerations (e.g., csv), if any.
    virtual Scope* enumsScope() const { return _enumsScope; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Operator model straight out of the XML file.
    virtual Model const& model() const { return _model; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Scope containing the formal parameters of this operator.
    virtual Scope* paramsScope() const { return _paramsScope; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}

    /// Return the toolkit index where the Composite is defined.  0 is the 'main SPL file'
    uint32_t toolkitIndex() const { return _toolkitIndex; }

    /// Determine if nested Custom Output Functions are allowed to nest
    /// @param port The port we are interested in
    /// @return Returns true if the given port supports nested output functions, otherwise false.
    bool nestedCOFsAllowed(unsigned port) const;
};
SYM_IS_AS_SPECIALIZATIONS(PrimitiveOperatorSym)

/// Symbol for a formal parameter to a primitive operator, as declared
/// in the XML file with the operator model.
class PrimitiveFormalSym : public SymbolTableEntry
{
    Scope mutable* _heldScope;
    PrimitiveOperatorSym& _op;
    SPL::Operator::Parameter const& _param;

  public:
    /// Constructor.
    PrimitiveFormalSym(SourceLocation const&,
                       PrimitiveOperatorSym&,
                       SPL::Operator::Parameter const&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Number of actuals that can be passed to this formal.
    virtual int cardinality() const;

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// See schema/SPL/operatorModel.xsd, for example, Attribute or CustomLiteral.
    virtual SPL::Operator::ExpressionMode const& mode() const { return _param.getExpressionMode(); }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// The primitive operator of which this is a formal.
    virtual PrimitiveOperatorSym& op() const { return _op; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _op.paramsScope(); }
    /// Used internally by compiler developers for diagnostic messages.

    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(PrimitiveFormalSym)

/// Symbol for a set type, e.g., 'set<int32>'.
class SetTypeSym : public SymbolTableEntry
{
    AstNode& _astNode;
    RootTyp mutable const* _type;

  public:
    /// Constructor.
    SetTypeSym(SourceLocation const&, AstNode&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(SetTypeSym)

/// This class represents an annotation that has a set of 0 or more key/value pairs,
/// where each value is an rstring or ustring
class SimpleAnnotationSym : public SymbolTableEntry
{
    AstNode& _astNode;
    std::string _name;
    std::vector<std::pair<AstNode*, AstNode*> > _keyValuePairs;
    Scope* _heldScope;

  public:
    /// Constructor
    SimpleAnnotationSym(SourceLocation const& loc,
                        AstNode& inAstNode,
                        const std::string& inname,
                        Scope& inParentScope);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const { return _astNode; }

    /// Name of the annotation
    virtual std::string name() const { return _name; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Parent scope in which this symbol is defined.  NULL in this case.
    virtual Scope* parentScope() const { return NULL; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream& s) const;

    /// Has no type
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    /// Expand the scope
    virtual void expand() {}

    /// Add a key/value pair
    void addKeyValuePair(const std::pair<AstNode*, AstNode*>& p) { _keyValuePairs.push_back(p); }

    /// Access the key value ast pairs
    const std::vector<std::pair<AstNode*, AstNode*> >& keyValuePairs() const
    {
        return _keyValuePairs;
    }
};
SYM_IS_AS_SPECIALIZATIONS(SimpleAnnotationSym)

/// Symbol for a type that is denoted just by its name, i.e., all
/// primitive types except bounded strings, and void.
class SimpleTypeSym : public SymbolTableEntry
{
    RootTyp const& _type;

  public:
    /// Constructor.
    SimpleTypeSym(SourceLocation const&, RootTyp const&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const { return _type; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(SimpleTypeSym)

/// Symbol for a stream as defined by the output of an operator invocation.
class StreamSym : public SymbolTableEntry
{
    AstNode& _astNode;
    std::string _fullName;
    std::string _localName;
    SymbolTableEntry& tupleBodySym() const;

  public:
    /// Constructor.
    StreamSym(SourceLocation const& loc,
              AstNode& ast,
              std::string const& fullName,
              std::string const& localName);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Outside-in qualified name of the form 'opInstance.stream'. In case of a stream connected
    /// to an output port of a composite, name of the actual stream, not the formal port.
    virtual std::string fullName() const { return _fullName; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return heldScope()->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    virtual void expand();

    /// Local name for the stream (in composites)
    std::string localName() const { return _localName; }
};

SYM_IS_AS_SPECIALIZATIONS(StreamSym)

/// Symbol for an rstring type, which may or may not be bounded.
class RStringTypeSym : public SymbolTableEntry
{
    AstNode& _astNode;
    RootTyp mutable const* _type;

  public:
    /// Constructor.
    RStringTypeSym(SourceLocation const&, AstNode&);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const
    {
        assert(false);
        return NULL;
    }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(RStringTypeSym)

class ThreadingAnnotationSym : public SymbolTableEntry
{
    AstNode& _astNode;
    AstNode& _model;
    AstNode* _threads;
    AstNode* _elastic;
    Scope* _heldScope;

  public:
    ThreadingAnnotationSym(SourceLocation const& loc,
                           AstNode& inast,
                           AstNode& modelin,
                           AstNode* threadsin,
                           AstNode* elasticin,
                           Scope& parentScopeIn);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Name of the annotation
    virtual std::string name() const { return "threading"; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream& s) const;

    /// Has no type
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }
    virtual void expand() {}

    AstNode& model() const { return _model; }
    AstNode* threads() const { return _threads; }
    AstNode* elastic() const { return _elastic; }
};
SYM_IS_AS_SPECIALIZATIONS(ThreadingAnnotationSym)

/// Symbol for a tuple type defined by a list of attributes, e.g., 'tuple<int32 x, int32 y>'.
class TupleAttribSym : public SymbolTableEntry
{
    typedef enum
    {
        notYet,
        inProgress,
        success,
        failure
    } ExpansionStatus;
    AstNode& _astNode;
    ExpansionStatus _expansionStatus;
    Scope* _heldScope;
    RootTyp const* _type;

  public:
    /// Constructor.
    TupleAttribSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Expand any types of attributes.
    virtual void expand();

    /// Scope with the attributes defined by this tuple type.
    virtual Scope* heldScope() const { return _heldScope; }

    /// No name.
    virtual std::string name() const
    {
        assert(false);
        return *((std::string*)NULL);
    }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    virtual void setType(const RootTyp& t) { _type = &t; }
};
SYM_IS_AS_SPECIALIZATIONS(TupleAttribSym)

/// Symbol for a tuple type defined by a list of other tuple types that are being extended,
/// e.g., 'tuple<otherTypeId, tuple<int32 x, int32 y>>'.
class TupleExtendSym : public SymbolTableEntry
{
    typedef enum
    {
        notYet,
        inProgress,
        success,
        failure
    } ExpansionStatus;
    AstNode& _astNode;
    ExpansionStatus _expansionStatus;
    Scope* _heldScope;
    RootTyp const* _type;

  public:
    /// Constructor.
    TupleExtendSym(SourceLocation const&, AstNode&, Scope* parentScope);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Expand all the extendees, then assimilate their attributes.
    virtual void expand();

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const
    {
        assert(false);
        return *((std::string*)NULL);
    }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Update the type of the sym
    virtual void setType(const RootTyp& typ) { _type = &typ; }
};
SYM_IS_AS_SPECIALIZATIONS(TupleExtendSym)

/// Symbol for a literal tuple value, e.g., '{ x = 1, y = 2 }'.
class TupleLiteralSym : public SymbolTableEntry
{
    AstNode& _astNode;
    Scope* _heldScope;
    RootTyp mutable const* _type;

  public:
    /// Constructor for regular tuple literal whose type is determined by its contents..
    TupleLiteralSym(SourceLocation const& loc, AstNode& ast, Scope* parentScope);

    /// Constructor for empty tuple literal whose type is determined by its context.
    TupleLiteralSym(SourceLocation const& loc,
                    AstNode& ast,
                    Scope& parentScope,
                    TupleTyp const& type);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return _heldScope; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const
    {
        assert(false);
        return *((std::string*)NULL);
    }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return _heldScope->parentScope(); }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Update the type of the sym
    virtual void setType(const RootTyp& typ) { _type = &typ; }

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(TupleLiteralSym)

/// Symbol for a '<constraint formalId>' formal generic parameter
/// denoting a type that parameterizes a native function.
class TypeFormalSym : public SymbolTableEntry
{
    TypeFormal const& _type;

  public:
    /// Constructor.
    TypeFormalSym(SourceLocation const&, Scope&, AstNode&);
    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        assert(false);
        return *((AstNode*)NULL);
    }

    /// False.
    virtual bool hasAst() const { return false; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const { return NULL; }

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const;

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const;

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Type of this AST, if any.
    static TypeFormal const& type(Scope&, AstNode&);

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(TypeFormalSym)

/// Symbol for a local variable (including for loop initializer) or a stream-logic variable
class VariableSym : public SymbolTableEntry
{
    AstNode& _astNode;
    AstNode& _idAst;
    bool _isMutable;
    bool _isPublic;
    bool _isStatic;
    std::string _name;
    Scope& _parentScope;

  public:
    /// Constructor.
    VariableSym(SourceLocation const&, AstNode& defAst, AstNode& idAst, Scope& parentScope);

    /// Abstract syntax tree of definition of this symbol, if any.
    virtual AstNode& ast() const
    {
        AST_SYM_ASSERT;
        return _astNode;
    }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Abstract syntax tree of defining ID of this symbol, if any.
    virtual AstNode& idAst() const { return _idAst; }

    /// True if mutable, false if immutable.
    virtual bool isMutable() const { return _isMutable; }

    /// True if public, false if private.
    virtual bool isPublic() const { return _isPublic; }

    /// True if static, false if instance.
    virtual bool isStatic() const { return _isStatic; }

    /// Does the variable definition in the AST have an explicit 'static' modifier?
    static bool isStatic(AstNode&);

    /// Simple name (excluding qualifier) of this symbol, if any.
    virtual std::string name() const { return _name; }

    /// Parent scope in which this symbol is defined.
    virtual Scope* parentScope() const { return &_parentScope; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream&) const;

    /// Type of this symbol, if any.
    virtual RootTyp const& type() const;

    /// Turn this expression into the IR for the compiler back-end.
    virtual Expression* genExpression(const SourceLocation& loc) const;

    virtual void expand() {}
};
SYM_IS_AS_SPECIALIZATIONS(VariableSym)

class ViewAnnotationSym : public SymbolTableEntry
{
    enum ScopeMode
    {
        Held,
        Attribute,
        Port
    };

  public:
    ViewAnnotationSym(SourceLocation const& loc,
                      AstNode& inast,
                      AstNode& nameIn,
                      AstNode& portIn,
                      AstNode& sampleSizeIn,
                      AstNode* bufferSizeIn,
                      AstNode* bufferTimeIn,
                      AstNode* descriptionIn,
                      AstNode* attributesIn,
                      AstNode* filterIn,
                      AstNode* activateOptionIn,
                      Scope& inParentScope);

    virtual ~ViewAnnotationSym();

    /// Abstract syntax tree of definition of this symbol.
    virtual AstNode& ast() const { return _astNode; }

    /// Name of the annotation
    virtual std::string name() const { return "view"; }

    /// Scope held by this symbol, if any.
    virtual Scope* heldScope() const;

    /// Parent scope in which this symbol is defined.  NULL in this case.
    virtual Scope* parentScope() const { return NULL; }

    /// Used internally by compiler developers for diagnostic messages.
    virtual std::ostream& print(std::ostream& s) const;

    /// Has no type
    virtual RootTyp const& type() const
    {
        assert(false);
        return *((RootTyp*)NULL);
    }

    virtual void expand() {}

    AstNode& viewName() const { return _name; }
    AstNode& port() const { return _port; }
    AstNode& sampleSize() const { return _sampleSize; }
    AstNode* bufferSize() const { return _bufferSize; }
    AstNode* bufferTime() const { return _bufferTime; }
    AstNode* description() const { return _description; }
    AstNode* attributes() const { return _attributes; }
    AstNode* filter() const { return _filter; }
    AstNode* activateOption() const { return _activateOption; }

    Scope& portScope() { return *_portScope; }
    Scope& attributesScope() { return *_attributesScope; }

    void setPortScopeMode() { _mode = Port; }
    void setAttributesScopeMode() { _mode = Attribute; }
    void resetScopeMode() { _mode = Held; }

  private:
    AstNode& _astNode;
    AstNode& _name;
    AstNode& _port;
    AstNode& _sampleSize;
    AstNode* _bufferSize;
    AstNode* _bufferTime;
    AstNode* _description;
    AstNode* _attributes;
    AstNode* _filter;
    AstNode* _activateOption;
    Scope* _heldScope;
    Scope* _portScope;
    Scope* _attributesScope;
    ScopeMode _mode;
};
SYM_IS_AS_SPECIALIZATIONS(ViewAnnotationSym)

} // namespace SPL

#undef AST_SYM_ASSERT
#endif /*SPL_SYMBOL_TABLE_ENTRIES_H*/
