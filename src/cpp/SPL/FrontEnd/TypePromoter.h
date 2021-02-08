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

#ifndef SPL_TYPE_PROMOTER_H
#define SPL_TYPE_PROMOTER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <stack>

namespace SPL {

class ParserContext;
class RootTyp;
class ListTyp;
class BListTyp;
class SetTyp;
class BSetTyp;
class MapTyp;
class BMapTyp;
class BStringTyp;
class TupleTyp;
class XMLTyp;
class OptionalTyp;
class EnumTyp;
class SourceLocation;
class FunctionHeadSym;

class TypeFactory;
class TypePromoter;

namespace TypePromoterContexts {

class TypePromoterContextBase
{
  public:
    enum Kind
    {
        Start,
        ListLiteral,
        ListLiteralElement,
        SetLiteralElement,
        MapLiteralKey,
        MapLiteralValue,
        LocalDecl,
        LocalDeclItem,
        SubscriptExpr,
        CastExpr,
        InfixExpr,
        OpInvokeOutput,
        AttributeAssign,
        ReturnStmt,
        ConditionalExpr,
        CallExprParam,
        OpInvokeActual,
        numContextKind
    };

    Kind getKind() const { return _kind; }

    /// Declares the primary template for a method to query if the object is one of a specific kind.
    /// @return Returns true if the object is of the specific kind, otherwise false.
    template<class T>
    bool is() const;

    /// Declares the primary template for a const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    const T& as() const;

    /// Declares the primary template for a non-const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    T& as();

    const TypePromoterContextBase& previousContext() const
    {
        assert(NULL != _previousContext);
        return *_previousContext;
    }

    const TypePromoterContextBase& currentContext() const;

    virtual void print() const = 0;

    virtual bool inLocalDeclContext() const;

    virtual const RootTyp& type() const = 0;

  protected:
    TypePromoterContextBase(TypePromoter& tp, Kind k, AstNode& ast);
    virtual ~TypePromoterContextBase();

    AstNode& _ast;

  private:
    TypePromoter& _tp;
    Kind _kind;
    TypePromoterContextBase* _previousContext;
};

#define TP_IS_AS_SPECIALIZATIONS(isEnum, asType)                                                   \
    template<>                                                                                     \
    inline bool TypePromoterContextBase::is<asType>() const                                        \
    {                                                                                              \
        return getKind() == TypePromoterContextBase::isEnum;                                       \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& TypePromoterContextBase::as<asType>()                                           \
    {                                                                                              \
        assert(dynamic_cast<asType*>(this) != 0);                                                  \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline const asType& TypePromoterContextBase::as<asType>() const                               \
    {                                                                                              \
        assert(dynamic_cast<const asType*>(this) != 0);                                            \
        return *static_cast<const asType*>(this);                                                  \
    }

class StartContext : public TypePromoterContextBase
{
  public:
    StartContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "Start" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(Start, StartContext)

class ListLiteralContext : public TypePromoterContextBase
{
  public:
    ListLiteralContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "ListLiteral" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(ListLiteral, ListLiteralContext)

class ListLiteralElementContext : public TypePromoterContextBase
{
  public:
    ListLiteralElementContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "ListLiteralElement" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(ListLiteralElement, ListLiteralElementContext)

class SetLiteralElementContext : public TypePromoterContextBase
{
  public:
    SetLiteralElementContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "SetLiteralElement" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(SetLiteralElement, SetLiteralElementContext)

class MapLiteralKeyContext : public TypePromoterContextBase
{
  public:
    MapLiteralKeyContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "MapLiteralKey" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(MapLiteralKey, MapLiteralKeyContext)

class MapLiteralValueContext : public TypePromoterContextBase
{
  public:
    MapLiteralValueContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "MapLiteralValue" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(MapLiteralValue, MapLiteralValueContext)

class LocalDeclContext : public TypePromoterContextBase
{
  public:
    LocalDeclContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "LocalDecl" << std::endl; }
    virtual const RootTyp& type() const;
    virtual bool inLocalDeclContext() const;
};
TP_IS_AS_SPECIALIZATIONS(LocalDecl, LocalDeclContext)

class LocalDeclItemContext : public TypePromoterContextBase
{
  public:
    LocalDeclItemContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "LocalDeclItem" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(LocalDeclItem, LocalDeclItemContext)

class SubscriptExprContext : public TypePromoterContextBase
{
  public:
    SubscriptExprContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "SubscriptExpr" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(SubscriptExpr, SubscriptExprContext)

class CastExprContext : public TypePromoterContextBase
{
  public:
    CastExprContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "CastExpr" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(CastExpr, CastExprContext)

class InfixExprContext : public TypePromoterContextBase
{
  public:
    InfixExprContext(TypePromoter& tp, AstNode& ast, bool onLHS);
    virtual void print() const { std::cerr << "InfixExpr" << std::endl; }
    virtual const RootTyp& type() const;

  private:
    bool _onLHS;
};
TP_IS_AS_SPECIALIZATIONS(InfixExpr, InfixExprContext)

class OpInvokeOutputContext : public TypePromoterContextBase
{
  public:
    OpInvokeOutputContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "OpInvokeOutput" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(OpInvokeOutput, OpInvokeOutputContext)

class AttributeAssignContext : public TypePromoterContextBase
{
  public:
    AttributeAssignContext(TypePromoter& tp, AstNode& ast);
    virtual void print() const { std::cerr << "AttributeAssign" << std::endl; }
    virtual const RootTyp& type() const;
};
TP_IS_AS_SPECIALIZATIONS(AttributeAssign, AttributeAssignContext)

class ReturnStmtContext : public TypePromoterContextBase
{
  public:
    ReturnStmtContext(TypePromoter& tp, AstNode& ast, const FunctionHeadSym& fun);
    virtual void print() const { std::cerr << "ReturnStmt" << std::endl; }
    virtual const RootTyp& type() const;

  private:
    const FunctionHeadSym& _fun;
};
TP_IS_AS_SPECIALIZATIONS(ReturnStmt, ReturnStmtContext)

class ConditionalExprContext : public TypePromoterContextBase
{
  public:
    ConditionalExprContext(TypePromoter& tp, AstNode& ast, bool inThen);
    virtual void print() const { std::cerr << "ConditionalExpr" << std::endl; }
    virtual const RootTyp& type() const;

  private:
    bool _inThen;
};
TP_IS_AS_SPECIALIZATIONS(ConditionalExpr, ConditionalExprContext)

class CallExprParamContext : public TypePromoterContextBase
{
  public:
    CallExprParamContext(TypePromoter& tp, AstNode& ast, const RootTyp& t);
    virtual void print() const { std::cerr << "CallExprParam" << std::endl; }
    virtual const RootTyp& type() const { return _type; }

  private:
    const RootTyp& _type;
};
TP_IS_AS_SPECIALIZATIONS(CallExprParam, CallExprParamContext)

class OpInvokeActualContext : public TypePromoterContextBase
{
  public:
    OpInvokeActualContext(TypePromoter& tp, AstNode& ast, const RootTyp& t);

    virtual void print() const { std::cerr << "OpInvokeActual" << std::endl; }

    virtual const RootTyp& type() const { return _type; }

  private:
    const RootTyp& _type;
};
TP_IS_AS_SPECIALIZATIONS(OpInvokeActual, OpInvokeActualContext)

} // TypePromoterContexts

class TypePromoter : public AstVisitorPushPop
{
  public:
    static void run(ParserContext& pCtx, AstNode& ast);

    TypePromoterContexts::TypePromoterContextBase* setContext(
      TypePromoterContexts::TypePromoterContextBase* context)
    {
        TypePromoterContexts::TypePromoterContextBase* prev = _currentContext;
        _currentContext = context;
        return prev;
    }

    friend class TypePromoterContexts::TypePromoterContextBase;

  protected:
    TypePromoter(ParserContext& pCtx, AstNode& ast);

    const TypePromoterContexts::TypePromoterContextBase& currentContext() const
    {
        assert(NULL != _currentContext);
        return *_currentContext;
    }
    TypePromoterContexts::TypePromoterContextBase& currentContext()
    {
        assert(NULL != _currentContext);
        return *_currentContext;
    }

    virtual void visit(AstNode& ast);
    virtual void visitAttributeT(AstNode&);
    virtual void visitBangT(AstNode&);
    virtual void visitOptionalT(AstNode&);
    virtual void visitCompositeT(AstNode&);
    virtual void visitEnumT(AstNode&);
    virtual void visitExpressionT(AstNode&);
    virtual void visitFunctionT(AstNode&);
    virtual void visitListT(AstNode&);
    virtual void visitMapT(AstNode&);
    virtual void visitMinusMinusT(AstNode&);
    virtual void visitMutableT(AstNode&);
    virtual void visitNoneT(AstNode&);
    virtual void visitOperatorT(AstNode&);
    virtual void visitPlusPlusT(AstNode&);
    virtual void visitQMarkT(AstNode&);
    // virtual void visitQMarkQMarkT(AstNode &);
    virtual void visitPublicT(AstNode&);
    virtual void visitSetT(AstNode&);
    virtual void visitStatefulT(AstNode&);
    virtual void visitStaticT(AstNode&);
    virtual void visitTildeT(AstNode&);
    virtual void visitTupleT(AstNode&);
    virtual void visitTypeT(AstNode&);
    virtual void visitUnaryMinusT(AstNode&);

    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitCompilationUnit(AstNode&);
    virtual void visitNamespace(AstNode&);
    virtual void visitUseDirectives(AstNode&);
    virtual void visitUseDirective(AstNode&);
    virtual void visitNamespaceName(AstNode&);
    virtual void visitUseDirectiveTail(AstNode&);
    virtual void visitUseWildcard(AstNode&);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitCompositeDef(AstNode&);
    virtual void visitCompositeHead(AstNode&);
    virtual void visitSplDoc(AstNode&);
    virtual void visitMlComment(AstNode&);
    virtual void visitCompositeIn(AstNode&);
    virtual void visitCompositeOut(AstNode&);
    virtual void visitCompositePort(AstNode&);
    virtual void visitCompositeBody(AstNode&);
    virtual void visitFormalClause(AstNode&);
    virtual void visitTypeClause(AstNode&);
    virtual void visitVarClause(AstNode&);
    virtual void visitGraphClause(AstNode&);
    virtual void visitConfigClause(AstNode&);
    virtual void visitCompositeFormal(AstNode&);
    virtual void visitCompositeFormalInit(AstNode&);
    virtual void visitOpActualExprs(AstNode&);
    virtual void visitOpActualName(AstNode&);
    virtual void visitOpActualType(AstNode&);
    virtual void visitConfigItem(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOpInvoke(AstNode&);
    virtual void visitOpInvokeHead(AstNode&);
    virtual void visitOpOutputs(AstNode&);
    virtual void visitOpOutput(AstNode&);
    virtual void visitStreamType(AstNode&);
    virtual void visitAsAlias(AstNode&);
    virtual void visitOpInputs(AstNode&);
    virtual void visitPortInputs(AstNode&);
    virtual void visitOpInvokeBody(AstNode&);
    virtual void visitLogicClause(AstNode&);
    virtual void visitWindowClause(AstNode&);
    virtual void visitActualClause(AstNode&);
    virtual void visitOutputClause(AstNode&);
    virtual void visitLogicState(AstNode&);
    virtual void visitOnTupleLogic(AstNode&);
    virtual void visitOnPunctLogic(AstNode&);
#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        virtual void visitOnWatermarkLogic(AstNode &);
#endif
    virtual void visitOnProcessLogic(AstNode&);
    virtual void visitOpInvokeWindow(AstNode&);
    virtual void visitOpInvokeActual(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);
    virtual void visitFunctionHead(AstNode&);
    virtual void visitFunctionFormal(AstNode&);

    // ----- Function Prototype declarations come from XML files, not regular SPL files.
    virtual void visitFunctionPrototype(AstNode&);
    virtual void visitTypeFormals(AstNode&);
    virtual void visitTypeFormal(AstNode&);
    virtual void visitBoundsFormals(AstNode&);
    virtual void visitProtoFormals(AstNode&);
    virtual void visitProtoFormal(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDecl(AstNode&);
    virtual void visitLocalDeclItem(AstNode&);
    virtual void visitBlockStmt(AstNode&);
    virtual void visitBlockStmtNoScope(AstNode&);
    virtual void visitExprStmt(AstNode&);
    virtual void visitIfStmt(AstNode&);
    virtual void visitElseClause(AstNode&);
    virtual void visitForInit(AstNode&);
    virtual void visitForStmt(AstNode&);
    virtual void visitWhileStmt(AstNode&);
    virtual void visitBreakStmt(AstNode&);
    virtual void visitContinueStmt(AstNode&);
    virtual void visitReturnStmt(AstNode&);

    // ----- Expressions can appear in many places in the grammar.
    virtual void visitMapping(AstNode&);
    virtual void visitAttributeExpr(AstNode&);
    virtual void visitCastExpr(AstNode&);
    virtual void visitCallExpr(AstNode&);
    virtual void visitConditionalExpr(AstNode&);
    virtual void visitInfixExpr(AstNode&);
    virtual void visitParenthesizedExpr(AstNode&);
    virtual void visitPostfixExpr(AstNode&);
    virtual void visitPrefixExpr(AstNode&);
    virtual void visitQualifierExpr(AstNode&);
    virtual void visitSubscriptExpr(AstNode&);
    virtual void visitSlice(AstNode&);
    virtual void visitId(AstNode&);
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);
    virtual void visitIsPresentExpr(AstNode& ast);

    // ----- Literals are the highest-precedence expressions denoting values.
    virtual void visitListLiteral(AstNode&);
    virtual void visitCurlyLiteral(AstNode&);
    virtual void visitTupleLiteral(AstNode&);
    virtual void visitAttributeAssign(AstNode&);
    virtual void visitMapLiteral(AstNode&);
    virtual void visitSetLiteral(AstNode&);
    virtual void visitBooleanLiteral(AstNode&);
    virtual void visitStringLiteral(AstNode&);
    virtual void visitXMLLiteral(AstNode&);
    virtual void visitNumericLiteral(AstNode&);
    virtual void visitNullLiteral(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitDefType(AstNode&);
    virtual void visitExpressionMode(AstNode&);
    virtual void visitTypeName(AstNode&);
    virtual void visitEnumType(AstNode&);
    virtual void visitXMLType(AstNode&);
    virtual void visitListType(AstNode&);
    virtual void visitMapType(AstNode&);
    virtual void visitPrimitiveType(AstNode&);
    virtual void visitPrimitiveTypeName(AstNode&);
    virtual void visitRStringType(AstNode&);
    virtual void visitSetType(AstNode&);
    virtual void visitTupleType(AstNode&);
    virtual void visitUStringType(AstNode&);
    virtual void visitVoidType(AstNode&);
    virtual void visitTupleAttrib(AstNode&);
    virtual void visitTupleExtend(AstNode&);
    virtual void visitOptionalType(AstNode&);
    virtual void visitAttributeDecl(AstNode&);
    virtual void visitTypeArgs(AstNode&);
    virtual void visitTypeDims(AstNode&);

    // ----- Modifiers prefix function, variable, and type definitions.
    virtual void visitModifiers(AstNode&);

    // ---- Annotations
    virtual void visitAnnotations(AstNode&);
    virtual void visitAnnotation(AstNode&);
    virtual void visitAnnotationT(AstNode&);

    TypeFactory& typeFactory() const { return _typeFactory; }

    std::stack<AstNode> _parentNodeStack;

  private:
    void analyze(AstNode& ast);
    AstNode& createCastToOptional(AstNode& rhs);
    AstNode& createCastToType(const RootTyp& type, AstNode& rhs);
    AstNode& createOptionalTypeNode(AstToken& start,
                                    const SourceLocation& loc,
                                    const OptionalTyp& type);
    AstNode& createSetTypeNode(AstToken& start, const SourceLocation& loc, const SetTyp& type);
    AstNode& createBSetTypeNode(AstToken& start, const SourceLocation& loc, const BSetTyp& type);
    AstNode& createMapTypeNode(AstToken& start, const SourceLocation& loc, const MapTyp& type);
    AstNode& createBMapTypeNode(AstToken& start, const SourceLocation& loc, const BMapTyp& type);
    AstNode& createListTypeNode(AstToken& start, const SourceLocation& loc, const ListTyp& type);
    AstNode& createBListTypeNode(AstToken& start, const SourceLocation& loc, const BListTyp& type);
    AstNode& createBStringTypeNode(AstToken& start,
                                   const SourceLocation& loc,
                                   const BStringTyp& type);
    AstNode& createXMLTypeNode(AstToken& start, const SourceLocation& loc, const XMLTyp& type);
    AstNode& createTypeNode(AstToken& start, const SourceLocation& loc, const RootTyp& type);
    AstNode& createPrimitiveTypeNode(AstToken& start,
                                     const SourceLocation& loc,
                                     const RootTyp& type);
    AstNode& createEnumTypeNode(AstToken& start, const SourceLocation& loc, const EnumTyp& type);
    AstNode& createTupleTypeNode(AstToken& start, const SourceLocation& loc, const TupleTyp& tType);
    bool performTupleLiteralPromotions(const TupleTyp& lhsType, AstNode*& tupleLiteral);
    bool performPromotions(const RootTyp& type,
                           AstNode*& ast,
                           bool allowImplicitPromotions,
                           bool isParameter);
    bool performListLiteralPromotions(const RootTyp& lhsType, AstNode*& listLiteral);
    bool performMapLiteralPromotions(const RootTyp& lhsType, AstNode*& mapLiteral);
    bool performSetLiteralPromotions(const RootTyp& lhsType, AstNode*& setLiteral);
    bool containsAnyNulls(const RootTyp& type);

    TypePromoterContexts::TypePromoterContextBase* _currentContext;
    TypePromoterContexts::StartContext _startContext;

    TypeFactory& _typeFactory;
};

} // namespace SPL
#endif // SPL_TYPE_PROMOTER_H
