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

#include <SPL/Core/PathSearch.h>

#include <SPL/FrontEnd/TypePromoter.h>

#include <SPL/FrontEnd/ExpressionTypeFinder.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/GenericsUnifier.h>
#include <SPL/FrontEnd/LexicalNameUseFinder.h>
#include <SPL/FrontEnd/MetaType.h>
#include <SPL/FrontEnd/NameDefFinder.h>
#include <SPL/FrontEnd/RelativeNameUseFinder.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeAnalyzer.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

#include <string>

using namespace std;

namespace SPL {

const RootTyp& keyType(const RootTyp& type)
{
    TypeFactory& tf = TypeFactory::instance();
    const RootTyp* typeToCheck = &type;

    // if it's an optional, check the underlying type instead
    if (type.isOptional()) {
        typeToCheck = &type.as<OptionalTyp>().getUnderlyingType();
    }

    switch (typeToCheck->getMetaType()) {
        case MetaType::MAP:
            return typeToCheck->as<MapTyp>().getKeyType();
        case MetaType::BMAP:
            return typeToCheck->as<BMapTyp>().getKeyType();
        default:
            cerr << "Expected type with a key type.  Bad type is: " << type.getName() << endl;
            assert(false);
    }
    return tf.unknownT();
}

const RootTyp& valueType(const RootTyp& type)
{
    TypeFactory& tf = TypeFactory::instance();

    const RootTyp* typeToCheck = &type;

    // if it's an optional, check the underlying type instead
    if (type.isOptional()) {
        typeToCheck = &type.as<OptionalTyp>().getUnderlyingType();
    }

    switch (typeToCheck->getMetaType()) {
        case MetaType::MAP:
            return typeToCheck->as<MapTyp>().getValueType();
        case MetaType::BMAP:
            return typeToCheck->as<BMapTyp>().getValueType();
        default:
            assert(false);
    }
    return tf.unknownT();
}

const RootTyp& subscriptType(const RootTyp& type)
{
    TypeFactory& tf = TypeFactory::instance();
    switch (type.getMetaType()) {
        case MetaType::LIST:
        case MetaType::BLIST:
            return tf.primitiveT(MetaType::UINT32);
        case MetaType::MAP:
            return type.as<MapTyp>().getKeyType();
        case MetaType::BMAP:
            return type.as<BMapTyp>().getKeyType();
        case MetaType::RSTRING:
        case MetaType::BSTRING:
        case MetaType::USTRING:
            return type;
        case MetaType::BLOB:
            return tf.primitiveT(MetaType::UINT8);
        default:
            assert(false);
    }
    return tf.unknownT();
}

namespace TypePromoterContexts {

TypePromoterContextBase::TypePromoterContextBase(TypePromoter& tp, Kind k, AstNode& ast)
  : _ast(ast)
  , _tp(tp)
  , _kind(k)
  , _previousContext(tp.setContext(this))
{}

TypePromoterContextBase::~TypePromoterContextBase()
{
    _tp.setContext(_previousContext);
}

bool TypePromoterContextBase::inLocalDeclContext() const
{
    if (NULL != _previousContext) {
        return _previousContext->inLocalDeclContext();
    }
    return false;
}

const TypePromoterContextBase& TypePromoterContextBase::currentContext() const
{
    return _tp.currentContext();
}

StartContext::StartContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::Start, ast)
{}

const RootTyp& StartContext::type() const
{
    assert(false);
    return TypeFactory::instance().unknownT();
}

ListLiteralContext::ListLiteralContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::ListLiteral, ast)
{}

const RootTyp& ListLiteralContext::type() const
{
    return astSymbol(_ast).type();
}

ListLiteralElementContext::ListLiteralElementContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::ListLiteralElement, ast)
{}

const RootTyp& ListLiteralElementContext::type() const
{
    return ExpressionTypeFinder::elementType(astSymbol(_ast).type());
}

SetLiteralElementContext::SetLiteralElementContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::SetLiteralElement, ast)
{}

const RootTyp& SetLiteralElementContext::type() const
{
    return ExpressionTypeFinder::elementType(astSymbol(_ast).type());
}

MapLiteralKeyContext::MapLiteralKeyContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::MapLiteralKey, ast)
{}

const RootTyp& MapLiteralKeyContext::type() const
{
    return keyType(astSymbol(_ast).type());
}

MapLiteralValueContext::MapLiteralValueContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::MapLiteralValue, ast)
{}

const RootTyp& MapLiteralValueContext::type() const
{
    return valueType(astSymbol(_ast).type());
}

LocalDeclContext::LocalDeclContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::LocalDecl, ast)
{}

const RootTyp& LocalDeclContext::type() const
{
    AstNode& t = AST::localDecl::type(_ast);
    return astSymbol(t).type();
}

bool LocalDeclContext::inLocalDeclContext() const
{
    return true;
}

LocalDeclItemContext::LocalDeclItemContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::LocalDeclItem, ast)
{}

const RootTyp& LocalDeclItemContext::type() const
{
    AstNode& id = AST::localDeclItem::id(_ast);
    return astSymbol(id).type();
}

SubscriptExprContext::SubscriptExprContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::SubscriptExpr, ast)
{}

const RootTyp& SubscriptExprContext::type() const
{
    AstNode& collAst = AST::subscriptExpr::expr(_ast);
    SymbolTableEntry& collSym = astSymbol(collAst);
    if (symIsStream(collSym)) {
        return TypeFactory::instance().primitiveT(MetaType::UINT32);
    }
    return subscriptType(astSymbol(AST::subscriptExpr::expr(_ast)).type());
}

CastExprContext::CastExprContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::CastExpr, ast)
{}

const RootTyp& CastExprContext::type() const
{
    return astSymbol(AST::castExpr::type(_ast)).type();
}

InfixExprContext::InfixExprContext(TypePromoter& tp, AstNode& ast, bool onLHS)
  : TypePromoterContextBase(tp, TypePromoterContextBase::InfixExpr, ast)
  , _onLHS(onLHS)
{}

const RootTyp& InfixExprContext::type() const
{
    assert(!_onLHS);
    return astSymbol(AST::infixExpr::lhs(_ast)).type();
}

OpInvokeOutputContext::OpInvokeOutputContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::OpInvokeOutput, ast)
{}

const RootTyp& OpInvokeOutputContext::type() const
{
    return astSymbol(AST::infixExpr::lhs(_ast)).type();
}

AttributeAssignContext::AttributeAssignContext(TypePromoter& tp, AstNode& ast)
  : TypePromoterContextBase(tp, TypePromoterContextBase::AttributeAssign, ast)
{}

const RootTyp& AttributeAssignContext::type() const
{
    return astSymbol(_ast).type();
}

ReturnStmtContext::ReturnStmtContext(TypePromoter& tp, AstNode& ast, const FunctionHeadSym& fun)
  : TypePromoterContextBase(tp, TypePromoterContextBase::ReturnStmt, ast)
  , _fun(fun)
{}

const RootTyp& ReturnStmtContext::type() const
{
    return _fun.returnType();
}

ConditionalExprContext::ConditionalExprContext(TypePromoter& tp, AstNode& ast, bool inThen)
  : TypePromoterContextBase(tp, TypePromoterContextBase::ConditionalExpr, ast)
  , _inThen(inThen)
{}

const RootTyp& ConditionalExprContext::type() const
{
    // If we are in the then branch, get the type from the else branch if possible
    if (_inThen) {
        const RootTyp& elseType = astSymbol(AST::conditionalExpr::elseExpr(_ast)).type();
        MetaType mt = elseType.getMetaType();
        if (mt != MetaType::NUL && mt != MetaType::UNKNOWN) {
            return elseType;
        }
    }
    // else, if we are in the else branch, get the type form the then branch if possible
    const RootTyp& thenType = astSymbol(AST::conditionalExpr::thenExpr(_ast)).type();
    MetaType mt = thenType.getMetaType();
    if (mt != MetaType::NUL && mt != MetaType::UNKNOWN) {
        return thenType;
    }

    // otherwise, if the type was not determinable, get if form the current context
    return currentContext().type();
}

CallExprParamContext::CallExprParamContext(TypePromoter& tp, AstNode& ast, const RootTyp& t)
  : TypePromoterContextBase(tp, TypePromoterContextBase::CallExprParam, ast)
  , _type(t)
{}

OpInvokeActualContext::OpInvokeActualContext(TypePromoter& tp,
                                             AstNode& ast,
                                             const RootTyp& t = TypeFactory::instance().unknownT())
  : TypePromoterContextBase(tp, TypePromoterContextBase::OpInvokeActual, ast)
  , _type(t)
{}

} // TypePromoterContexts

using namespace TypePromoterContexts;

TypePromoter::TypePromoter(ParserContext& pCtx, AstNode& ast)
  : AstVisitorPushPop(pCtx, Debug::TraceTypePromoter)
  , _currentContext(NULL)
  , _startContext(*this, ast)
  , _typeFactory(TypeFactory::instance())
{}

void TypePromoter::run(ParserContext& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);
    MODEL("PhaseStart(TypePromoter, " << (void*)&ast << ")\n");
    TypePromoter visitor(pCtx, ast);
    visitor.visit(ast);
    MODEL("PhaseEnd(TypePromoter, " << (void*)&ast << ")\n");
}

void TypePromoter::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast
                               << ") stage is " << astStage(ast).toString(),
           Debug::TraceTypePromoter);
    if (astStage(ast) < FrontEndStage::TYPE_PROMOTER) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::TYPE_PROMOTER);
        AstVisitorPushPop::visit(ast);
    }
}

bool TypePromoter::performPromotions(const RootTyp& toType,
                                     AstNode*& ast,
                                     bool allowImplicitPromotions,
                                     bool isParameter = false)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(*ast) << ", type is " << toType,
           Debug::TraceTypePromoter);

    // Ignore the type if it is unknown
    if (toType.isUnknown()) {
        return false;
    }

    const RootTyp& astType = astSymbol(*ast).type();
    if (astType.isUnknown()) {
        return false;
    }

    // Types are already the same; no promotion necessary
    if (toType == astType) {
        return true;
    }

    // Null literals are promoted to the lhs type
    if (AST::nullLiteralT::is(*ast)) {
        // The assignment is always done to eliminate a downstream error message that is somewhat
        // redundant.
        astSymbol(*ast).setType(toType);
        if (!toType.isOptional()) {
            SysOut::errorln(SPL_FRONT_END_CANT_PROMOTE_NULL, astSymbol(*ast).sourceLocation());
            return false;
        }
        return true;
    }

    // Until proven otherwise, we always allow an implicit promotion from a non-optional type to
    // its optional equivalent
    if (toType.isOptionalOfSameType(astType)) {
        ast = &createCastToOptional(*ast);
        return true;
    }

    // If we are in a context that allows implicit conversions, and an explicit conversion would be
    // allowed, create an implicit conversion
    if (allowImplicitPromotions && ExpressionTypeFinder::explicitCastAllowed(astType, toType)) {
        ast = &createCastToType(toType, *ast);
        return true;
    }

    if (AST::tupleLiteral::is(*ast)) {
        if (toType.isOptional()) {
            const RootTyp& underlyingType = toType.as<OptionalTyp>().getUnderlyingType();
            performTupleLiteralPromotions(underlyingType.as<TupleTyp>(), ast);
            ast = &createCastToOptional(*ast);
            return true;
        }
        // Meta-type of type could be TUPLE or TYPEFORMAL
        if (toType.is<TupleTyp>()) {
            return performTupleLiteralPromotions(toType.as<TupleTyp>(), ast);
        }
    } else if (AST::listLiteral::is(*ast)) {
        if (toType.isOptional()) {
            const RootTyp& underlyingType = toType.as<OptionalTyp>().getUnderlyingType();
            performListLiteralPromotions(underlyingType, ast);
            ast = &createCastToOptional(*ast);
            return true;
        } else {
            return performListLiteralPromotions(toType, ast);
        }
    } else if (AST::setLiteral::is(*ast)) {

        if (toType.isOptional()) {
            const RootTyp& underlyingType = toType.as<OptionalTyp>().getUnderlyingType();
            performSetLiteralPromotions(underlyingType, ast);
            ast = &createCastToOptional(*ast);
            return true;
        } else {
            return performSetLiteralPromotions(toType, ast);
        }
    } else if (AST::mapLiteral::is(*ast)) {
        if (toType.isOptional()) {
            const RootTyp& underlyingType = toType.as<OptionalTyp>().getUnderlyingType();
            performMapLiteralPromotions(underlyingType, ast);
            ast = &createCastToOptional(*ast);
            return true;
        } else {
            return performMapLiteralPromotions(toType, ast);
        }
    } else if (AST::intLiteralT::is(*ast) && allowImplicitPromotions) {
        if (toType.isOptional()) {
            const RootTyp& underlyingType = toType.as<OptionalTyp>().getUnderlyingType();
            if (underlyingType != astType && underlyingType.isNumericType()) {
                astSymbol(*ast).setType(underlyingType);
                return true;
            }

            else {
                if (underlyingType.isNumericType() && isParameter) {
                    // insert a cast to make the parameter checking happy
                    ast = &createCastToOptional(*ast);
                    return true;
                }
            }
        }
    } else if (AST::floatLiteralT::is(*ast) && allowImplicitPromotions) {
        if (toType.isOptional()) {
            assert(false);
            const RootTyp& underlyingType = toType.as<OptionalTyp>().getUnderlyingType();
            if (underlyingType != astType && underlyingType.isNumericType()) {
                astSymbol(*ast).setType(underlyingType);
                return true;
            } else {
                if (underlyingType.isNumericType() && isParameter) {
                    // insert a cast to make the parameter checking happy
                    ast = &createCastToOptional(*ast);
                    return true;
                }
            }
        }
    }

    return false;
}

bool TypePromoter::performTupleLiteralPromotions(const TupleTyp& lhsType, AstNode*& tupleLiteral)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    assert(AST::tupleLiteral::is(*tupleLiteral));

    uint32_t numAttributes = AST::tupleLiteral::attributeAssignCount(*tupleLiteral);
    if (numAttributes == lhsType.getNumberOfAttributes()) {
        TupleTyp::Attributes attrs;
        for (uint32_t i = 0; i < numAttributes; i++) {
            const string& lhsAttrName = lhsType.getAttributeName(i);
            const RootTyp& lhsAttrType = lhsType.getAttributeType(i);
            AstNode& rhsAttr = AST::tupleLiteral::attributeAssign(*tupleLiteral, i);
            string rhsAttrName = astText(AST::attributeAssign::id(rhsAttr));
            AstNode* expr = &AST::attributeAssign::expr(rhsAttr);
            const RootTyp& rhsAttrType = astSymbol(AST::attributeAssign::expr(rhsAttr)).type();
            if (lhsAttrName == rhsAttrName) {
                // If the attribute is itself a tuple type, perform recursive promotions
                if (lhsAttrType.is<TupleTyp>()) {
                    assert(rhsAttrType.is<TupleTyp>());
                    if (!performTupleLiteralPromotions(lhsAttrType.as<TupleTyp>(), expr)) {
                        return false;
                    }
                    // resetAstChild(AstPool::instance(), rhsAttr, 1, *expr);
                    AST::attributeAssign::setExpr(rhsAttr, *expr);
                    attrs.push_back(make_pair(rhsAttrName, &astSymbol(*expr).type()));
                } else if (lhsAttrType.isOptionalOfSameType(rhsAttrType)) {
                    expr = &createCastToOptional(*expr);
                    // resetAstChild(AstPool::instance(), rhsAttr, 1, *expr);
                    AST::attributeAssign::setExpr(rhsAttr, *expr);
                    attrs.push_back(make_pair(rhsAttrName, &astSymbol(*expr).type()));
                } else if (rhsAttrType.getMetaType() == MetaType::NUL) {
                    if (!lhsAttrType.isOptional()) {
                        SysOut::errorln(SPL_FRONT_END_CANT_PROMOTE_NULL,
                                        astSymbol(rhsAttr).sourceLocation());
                    }
                    // We do the assignment regardless of the error generated in order to prevent
                    // superfluous errors later
                    astSymbol(AST::attributeAssign::expr(rhsAttr)).setType(lhsAttrType);
                    attrs.push_back(make_pair(rhsAttrName, &lhsAttrType));
                } else {
                    if (!performPromotions(lhsAttrType, expr, false)) {
                        attrs.push_back(make_pair(rhsAttrName, &rhsAttrType));
                    } else {
                        AST::attributeAssign::setExpr(rhsAttr, *expr);
                        attrs.push_back(make_pair(rhsAttrName, &astSymbol(*expr).type()));
                    }
                }
            } else {
                // These are not the same tuple types...give up
                return false;
            }
        }
        // We got through the tuple with the correct attr count and names. Now create the new type.
        const RootTyp& newTupleType = typeFactory().tupleT(attrs);
        astSymbol(*tupleLiteral).setType(newTupleType);
    }

    return true;
}

bool TypePromoter::performMapLiteralPromotions(const RootTyp& type, AstNode*& mapLiteral)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": type is " << type, Debug::TraceTypePromoter);

    bool promotionsPerformed = false;
    const RootTyp* typeToPromoteTo = &type;

    // is the LHS type an optional map?  If so, we allow promoting map literals to the optional type
    if (type.isOptional()) {
        typeToPromoteTo = &(type.as<OptionalTyp>().getUnderlyingType());
    }

    // Make sure we are promoting to a map type
    if (!(typeToPromoteTo->is<MapTyp>() || typeToPromoteTo->is<BMapTyp>())) {
        return promotionsPerformed;
    }

    const RootTyp& keyType = (typeToPromoteTo->is<MapTyp>())
                               ? typeToPromoteTo->as<MapTyp>().getKeyType()
                               : typeToPromoteTo->as<BMapTyp>().getKeyType();
    const RootTyp& valueType = (typeToPromoteTo->is<MapTyp>())
                                 ? typeToPromoteTo->as<MapTyp>().getValueType()
                                 : typeToPromoteTo->as<BMapTyp>().getValueType();

    int count = AST::mapLiteral::mappingCount(*mapLiteral);
    for (int i = 0; i < count; ++i) {
        AstNode& mapping = AST::mapLiteral::mapping(*mapLiteral, i);
        AstNode* fromExpr = &AST::mapping::fromExpr(mapping);
        if (performPromotions(keyType, fromExpr, false)) {
            // resetAstChild(AstPool::instance(), mapping, 0, *fromExpr);
            AST::mapping::setFromExpr(mapping, *fromExpr);
            promotionsPerformed = true;
        }

        AstNode* toExpr = &AST::mapping::toExpr(mapping);
        if (performPromotions(valueType, toExpr, false)) {
            // resetAstChild(AstPool::instance(), mapping, 1, *toExpr);
            AST::mapping::setToExpr(mapping, *toExpr);
            promotionsPerformed = true;
        }
    }

    // Create a new map type
    const RootTyp* newMapType;
    if (typeToPromoteTo->is<MapTyp>()) {
        newMapType = &typeFactory().mapT(keyType, valueType);
    } else {
        assert(typeToPromoteTo->is<BMapTyp>());
        newMapType = &typeFactory().mapT(keyType, valueType, count);
    }
    astSymbol(*mapLiteral).setType(*newMapType);
    return promotionsPerformed;
}

bool TypePromoter::performSetLiteralPromotions(const RootTyp& type, AstNode*& setLiteral)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": type is " << type, Debug::TraceTypePromoter);

    // assert(!type.isOptional());

    // Make sure we are promoting to a list type or optional of list type
    if (!(type.is<SetTyp>() || type.is<BSetTyp>())) {
        return false;
    }

    const RootTyp& elementType = (type.is<SetTyp>()) ? type.as<SetTyp>().getElementType()
                                                     : type.as<BSetTyp>().getElementType();

    SPLDBG(__PRETTY_FUNCTION__ << ": elementType is " << elementType, Debug::TraceTypePromoter);

    int count = AST::setLiteral::exprCount(*setLiteral);
    for (int i = 0; i < count; ++i) {
        AstNode* element = &AST::setLiteral::expr(*setLiteral, i);
        if (!performPromotions(elementType, element, false)) {
            return false;
        }
        // resetAstChild(AstPool::instance(), *setLiteral, i, *element);
        AST::setLiteral::setExpr(*setLiteral, i, *element);
    }

    // Create a new set type
    const RootTyp* newSetType;
    if (type.is<SetTyp>()) {
        newSetType = &typeFactory().setT(elementType);
    } else {
        assert(type.is<BSetTyp>());
        newSetType = &typeFactory().setT(elementType, count);
    }
    astSymbol(*setLiteral).setType(*newSetType);
    return true;
}

bool TypePromoter::performListLiteralPromotions(const RootTyp& toType, AstNode*& listLiteral)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": type is " << toType, Debug::TraceTypePromoter);

    //		assert(!toType.isOptional());

    // Make sure we are promoting to a list type or optional of list toType
    if (!(toType.is<ListTyp>() || toType.is<BListTyp>())) {
        return false;
    }

    const RootTyp& elementType = (toType.is<ListTyp>()) ? toType.as<ListTyp>().getElementType()
                                                        : toType.as<BListTyp>().getElementType();

    SPLDBG(__PRETTY_FUNCTION__ << ": elementType is " << elementType, Debug::TraceTypePromoter);

    int count = AST::listLiteral::exprCount(*listLiteral);
    for (int i = 0; i < count; ++i) {
        AstNode* element = &AST::listLiteral::expr(*listLiteral, i);
        if (!performPromotions(elementType, element, false)) {
            return false;
        }
        // resetAstChild(AstPool::instance(), *listLiteral, i, *element);
        AST::listLiteral::setExpr(*listLiteral, i, *element);
    }

    // Create a new list toType
    const RootTyp* newListType;
    if (toType.is<ListTyp>()) {
        newListType = &typeFactory().listT(elementType);
    } else {
        assert(toType.is<BListTyp>());
        newListType = &typeFactory().listT(elementType, count);
    }
    astSymbol(*listLiteral).setType(*newListType);
    return true;
}

#define Case(type1, type2)                                                                         \
    case MetaType::type1:                                                                          \
        astTypeName = &AST::type2::create(start);                                                  \
        break
AstNode& TypePromoter::createPrimitiveTypeNode(AstToken& start,
                                               const SourceLocation& loc,
                                               const RootTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    AstNode* astTypeName = NULL;
    switch (type.getMetaType()) {
        Case(INT8, int8T);
        Case(UINT8, uint8T);
        Case(INT16, int16T);
        Case(UINT16, uint16T);
        Case(INT32, int32T);
        Case(UINT32, uint32T);
        Case(INT64, int64T);
        Case(UINT64, uint64T);
        Case(FLOAT32, float32T);
        Case(FLOAT64, float64T);
        Case(BOOLEAN, booleanT);
        Case(DECIMAL32, decimal32T);
        Case(DECIMAL64, decimal64T);
        Case(DECIMAL128, decimal128T);
        Case(COMPLEX32, complex32T);
        Case(COMPLEX64, complex64T);
        Case(TIMESTAMP, timestampT);
        Case(BLOB, blobT);
        Case(RSTRING, rstringType);
        Case(USTRING, ustringT);
        default:
            assert(false);
    }
    assert(astTypeName != NULL);
    AstNode& ast = AST::primitiveType::create(start, *astTypeName);
    return ast;
}

#undef Case

AstNode& createTypeDims(AstToken& start, int32_t bounds)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    ostringstream s;
    s << bounds;
    AstNode& boundsNode = AST::intLiteralT::create(s.str());
    AstNode& typeDims = AST::typeDims::create(start, boundsNode);
    return typeDims;
}

AstNode& TypePromoter::createListTypeNode(AstToken& start,
                                          const SourceLocation& loc,
                                          const ListTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const RootTyp& elementType = type.getElementType();
    AstNode& typeNode = createTypeNode(start, loc, elementType);
    std::vector<AstNode*> typeArgs;
    typeArgs.push_back(&typeNode);
    AstNode& typeArgsNode = AST::typeArgs::create(start, typeArgs);
    AstNode& listNode = AST::listType::create(start, typeArgsNode);
    return listNode;
}

AstNode& TypePromoter::createBListTypeNode(AstToken& start,
                                           const SourceLocation& loc,
                                           const BListTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const RootTyp& elementType = type.getElementType();
    int32_t bounds = type.getBoundSize();
    AstNode& typeNode = createTypeNode(start, loc, elementType);
    std::vector<AstNode*> typeArgs;
    typeArgs.push_back(&typeNode);
    AstNode& typeArgsNode = AST::typeArgs::create(start, typeArgs);
    AstNode& typeDims = createTypeDims(start, bounds);
    AstNode& listNode = AST::listType::create(start, typeArgsNode, typeDims);
    return listNode;
}

AstNode& TypePromoter::createSetTypeNode(AstToken& start,
                                         const SourceLocation& loc,
                                         const SetTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const RootTyp& elementType = type.getElementType();
    AstNode& typeNode = createTypeNode(start, loc, elementType);
    std::vector<AstNode*> typeArgs;
    typeArgs.push_back(&typeNode);
    AstNode& typeArgsNode = AST::typeArgs::create(start, typeArgs);
    AstNode& setNode = AST::setType::create(start, typeArgsNode);
    return setNode;
}

AstNode& TypePromoter::createBSetTypeNode(AstToken& start,
                                          const SourceLocation& loc,
                                          const BSetTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const RootTyp& elementType = type.getElementType();
    int32_t bounds = type.getBoundSize();
    AstNode& typeNode = createTypeNode(start, loc, elementType);
    std::vector<AstNode*> typeArgs;
    typeArgs.push_back(&typeNode);
    AstNode& typeArgsNode = AST::typeArgs::create(start, typeArgs);
    AstNode& typeDims = createTypeDims(start, bounds);
    AstNode& setNode = AST::setType::create(start, typeArgsNode, typeDims);
    return setNode;
}

AstNode& TypePromoter::createMapTypeNode(AstToken& start,
                                         const SourceLocation& loc,
                                         const MapTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const RootTyp& keyType = type.getKeyType();
    const RootTyp& valueType = type.getValueType();
    AstNode& keyTypeNode = createTypeNode(start, loc, keyType);
    AstNode& valueTypeNode = createTypeNode(start, loc, valueType);
    std::vector<AstNode*> typeArgs;
    typeArgs.push_back(&keyTypeNode);
    typeArgs.push_back(&valueTypeNode);
    AstNode& typeArgsNode = AST::typeArgs::create(start, typeArgs);
    AstNode& mapNode = AST::mapType::create(start, typeArgsNode);
    return mapNode;
}

AstNode& TypePromoter::createBMapTypeNode(AstToken& start,
                                          const SourceLocation& loc,
                                          const BMapTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const RootTyp& keyType = type.getKeyType();
    const RootTyp& valueType = type.getValueType();
    int32_t bounds = type.getBoundSize();
    AstNode& keyTypeNode = createTypeNode(start, loc, keyType);
    AstNode& valueTypeNode = createTypeNode(start, loc, valueType);
    std::vector<AstNode*> typeArgs;
    typeArgs.push_back(&keyTypeNode);
    typeArgs.push_back(&valueTypeNode);
    AstNode& typeArgsNode = AST::typeArgs::create(start, typeArgs);
    AstNode& typeDims = createTypeDims(start, bounds);
    AstNode& mapNode = AST::mapType::create(start, typeArgsNode, typeDims);
    return mapNode;
}

AstNode& TypePromoter::createEnumTypeNode(AstToken& start,
                                          const SourceLocation& loc,
                                          const EnumTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const EnumTyp::Values& values = type.getValidValues();
    vector<AstNode*> children;
    for (EnumTyp::Values::const_iterator it = values.begin(), itEnd = values.end(); it != itEnd;
         ++it) {
        AstNode& id = AST::idT::create(*it);
        children.push_back(&id);
    }
    AstNode& enumType = AST::enumType::create(start, children);
    return enumType;
}

AstNode& TypePromoter::createTupleTypeNode(AstToken& start,
                                           const SourceLocation& loc,
                                           const TupleTyp& tType)
{
    vector<AstNode*> attribs;
    for (uint32_t i = 0, count = tType.getNumberOfAttributes(); i < count; ++i) {
        const string& attrName = tType.getAttributeName(i);
        const RootTyp& attrType = tType.getAttributeType(i);
        AstNode& id = AST::idT::create(attrName);
        AstNode& typeNode = createTypeNode(start, loc, attrType);
        AstNode& attrDecl = AST::attributeDecl::create(start, typeNode, id);
        attribs.push_back(&attrDecl);
    }
    AstNode& tupleAttrib = AST::tupleAttrib::create(start, attribs);
    AstNode& tupleType = AST::tupleType::create(start, tupleAttrib);

    return tupleType;
}

AstNode& TypePromoter::createBStringTypeNode(AstToken& start,
                                             const SourceLocation& loc,
                                             const BStringTyp& type)
{
    int32_t bounds = type.getBoundSize();
    AstNode& typeDims = createTypeDims(start, bounds);
    return AST::rstringType::create(start, typeDims);
}

AstNode& TypePromoter::createXMLTypeNode(AstToken& start,
                                         const SourceLocation& loc,
                                         const XMLTyp& type)
{
    if (!type.schema().empty()) {
        AstNode& schema = AST::stringLiteralT::create("\"" + type.schema() + "\"");
        return AST::xmlType::create(start, schema);
    }
    return AST::xmlType::create(start);
}

AstNode& TypePromoter::createOptionalTypeNode(AstToken& start,
                                              const SourceLocation& loc,
                                              const OptionalTyp& type)
{
    AstNode& underlyingTypeNode = createTypeNode(start, loc, type.getUnderlyingType());
    AstNode& opType = AST::optionalType::create(start, underlyingTypeNode);
    return opType;
}

AstNode& TypePromoter::createTypeNode(AstToken& start,
                                      const SourceLocation& loc,
                                      const RootTyp& type)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    if (type.is<SimplePrimitiveTyp>()) {
        return createPrimitiveTypeNode(start, loc, type);
    } else if (type.is<TupleTyp>()) {
        return createTupleTypeNode(start, loc, type.as<TupleTyp>());
    } else if (type.is<EnumTyp>()) {
        return createEnumTypeNode(start, loc, type.as<EnumTyp>());
    } else if (type.is<ListTyp>()) {
        return createListTypeNode(start, loc, type.as<ListTyp>());
    } else if (type.is<BListTyp>()) {
        return createBListTypeNode(start, loc, type.as<BListTyp>());
    } else if (type.is<SetTyp>()) {
        return createSetTypeNode(start, loc, type.as<SetTyp>());
    } else if (type.is<BSetTyp>()) {
        return createBSetTypeNode(start, loc, type.as<BSetTyp>());
    } else if (type.is<MapTyp>()) {
        return createMapTypeNode(start, loc, type.as<MapTyp>());
    } else if (type.is<BMapTyp>()) {
        return createBMapTypeNode(start, loc, type.as<BMapTyp>());
    } else if (type.is<BStringTyp>()) {
        return createBStringTypeNode(start, loc, type.as<BStringTyp>());
    } else if (type.is<XMLTyp>()) {
        return createXMLTypeNode(start, loc, type.as<XMLTyp>());
    } else if (type.is<OptionalTyp>()) {
        return createOptionalTypeNode(start, loc, type.as<OptionalTyp>());
    }

    assert(false);
    return *(AstNode*)NULL;
}

AstNode& TypePromoter::createCastToType(const RootTyp& type, AstNode& rhs)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    AstNode* cast = NULL;
    AstNode* r = &rhs;
    if (type.isOptional()) {
        const RootTyp& underlyingType = type.as<OptionalTyp>().getUnderlyingType();
        const RootTyp& fromType = astSymbol(rhs).type();
        assert(!fromType.isOptional()); // We have to handle this at some point
        if (fromType != type && fromType.getMetaType() != MetaType::NUL) {
            r = &createCastToType(underlyingType, rhs);
        }
        AstNode& underType =
          createTypeNode(astToken(rhs), astSymbol(rhs).sourceLocation(), underlyingType);
        AstNode& optType = AST::optionalType::create(astToken(rhs), underType);
        cast = &AST::castExpr::create(astToken(rhs), optType, *r);
    } else {
        AstNode& toType = createTypeNode(astToken(rhs), astSymbol(rhs).sourceLocation(), type);
        cast = &AST::castExpr::create(astToken(rhs), toType, *r);
    }
    analyze(*cast);
    return *cast;
}

AstNode& TypePromoter::createCastToOptional(AstNode& rhs)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceTypePromoter);
    const RootTyp& type = astSymbol(rhs).type();
    AstNode& fromType = createTypeNode(astToken(rhs), astSymbol(rhs).sourceLocation(), type);
    AstNode& toType = AST::optionalType::create(astToken(rhs), fromType);
    AstNode& cast = AST::castExpr::create(astToken(rhs), toType, rhs);
    analyze(cast);
    return cast;
}

void TypePromoter::analyze(AstNode& ast)
{
    NameDefFinder::run(_pCtx, ast);
    LexicalNameUseFinder::run(_pCtx, ast);
    RelativeNameUseFinder::run(_pCtx, ast);
    TypeAnalyzer::run(_pCtx, ast);
}

void TypePromoter::visitConditionalExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);

    _parentNodeStack.push(ast);

    {
        ConditionalExprContext context(*this, ast, true);
        visit(AST::conditionalExpr::thenExpr(ast));
    }
    {
        ConditionalExprContext context(*this, ast, false);
        visit(AST::conditionalExpr::elseExpr(ast));
    }

    AstNode* thenAst = &AST::conditionalExpr::thenExpr(ast);
    AstNode* elseAst = &AST::conditionalExpr::elseExpr(ast);

    const RootTyp& thenType = astSymbol(*thenAst).type();
    const RootTyp& elseType = astSymbol(*elseAst).type();

    // If both sides have the same type, no promotions are needed
    if (thenType == elseType) {
        return;
    }

    const RootTyp& parentType = currentContext().type();

    performPromotions(parentType, thenAst, false);
    // resetAstChild(AstPool::instance(), ast, 1, *thenAst);
    AST::conditionalExpr::setThenExpr(ast, *thenAst);

    performPromotions(parentType, elseAst, false);
    // resetAstChild(AstPool::instance(), ast, 2, *elseAst);
    AST::conditionalExpr::setElseExpr(ast, *elseAst);

    _parentNodeStack.pop();
}

void TypePromoter::visitLocalDecl(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);
    _parentNodeStack.push(ast);
    LocalDeclContext context(*this, ast);
    AstVisitorPushPop::visitLocalDecl(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitLocalDeclItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);
    _parentNodeStack.push(ast);

    if (AST::noneT::is(AST::localDeclItem::expr_q(ast))) {
        return;
    }

    LocalDeclItemContext context(*this, ast);
    AstVisitorPushPop::visitLocalDeclItem(ast);

    const RootTyp& lhsType = currentContext().type();

    AstNode* rhs = &AST::localDeclItem::expr_q(ast);
    performPromotions(lhsType, rhs, true);
    // resetAstChild(AstPool::instance(), ast, 1, *rhs);
    AST::localDeclItem::setExpr_q(ast, *rhs);

    _parentNodeStack.pop();
}

void TypePromoter::visitNullLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);
    // We don't actually do any direct promotions to a NULL literal. These are handled in the parent
    // node
}

void TypePromoter::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        AstNode& lhs = AST::infixExpr::lhs(exprAst);
        AstNode* rhs = &AST::infixExpr::rhs(exprAst);
        OpInvokeOutputContext context(*this, exprAst);
        visit(*rhs);
        performPromotions(astSymbol(lhs).type(), rhs, false);
        // resetAstChild(AstPool::instance(), exprAst, 1, *rhs);
        AST::infixExpr::setRhs(exprAst, *rhs);
    }
}

bool TypePromoter::containsAnyNulls(const RootTyp& type)
{
    switch (type.getMetaType()) {
        case MetaType::NUL:
            return true;
            break;

        case MetaType::TUPLE: {
            const TupleTyp& tupleType = type.as<TupleTyp>();
            for (uint32_t k = 0; k < tupleType.getNumberOfAttributes(); k++) {
                if (containsAnyNulls(tupleType.getAttributeType(k))) {
                    return true;
                }
            }
            return false;
            break;
        }

        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::SET:
        case MetaType::BSET:
        case MetaType::MAP:
        case MetaType::BMAP:
        case MetaType::OPTIONAL:
            return containsAnyNulls(ExpressionTypeFinder::elementType(type));
            break;

        default:
            return false;
            break;
    }
}

void TypePromoter::visitCallExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);

    _parentNodeStack.push(ast);

    SymbolTableEntry& sym = astSymbol(ast);
    CallExpressionSym& call = sym.as<CallExpressionSym>();
    string functionName = call.name();

    SymbolTableEntry* tgt = call.resolvedTgt();
    if (NULL != tgt) {
        FunctionHeadSym& fun = tgt->as<FunctionHeadSym>();
        auto_ptr<GenericsUnifier const> gu(&GenericsUnifier::unify(call, fun));
        assert(!gu->isFailure());
        for (int i = 0, n = call.actualTypes().size(); i < n; i++) {
            FunctionFormalSym* formal = fun.formals().at(i);
            const RootTyp& formalType = formal->type();
            const RootTyp& actual = *(call.actualTypes().at(i));

            if (i == 1 && functionName == "submit" && actual.getMetaType() != MetaType::UINT32) {
                continue; // do not do any promotions on the second param of submit(T, T)
            }

            if (formalType.getMetaType() == MetaType::TYPEFORMAL &&
                actual.getMetaType() == MetaType::TUPLE) { // templated type
                string typeFormalID = formalType.as<TypeFormal>().getIdentifier();

                // look for other parameters that use the same type formal
                bool foundOtherTypeFormal = false;
                for (int j = 0; j < n; j++) {
                    if (j == i) {
                        continue; // no point checking ourselves!
                    }

                    if (fun.formals().at(j)->type().getMetaType() == MetaType::TYPEFORMAL &&
                        fun.formals().at(j)->type().as<TypeFormal>().getIdentifier() ==
                          typeFormalID) {
                        // this parameter references the same formal type
                        // if the actual that's passed for this type doesn't contain any nulls, use
                        // its type
                        if (!containsAnyNulls(*(call.actualTypes().at(j)))) {
                            const RootTyp& actualType = *(call.actualTypes().at(j));
                            AstNode* rhs = &AST::callExpr::actual(ast, i);
                            CallExprParamContext context(*this, ast, actualType);
                            visit(*rhs);
                            performPromotions(actualType, rhs, false);
                            // resetAstChild(AstPool::instance(), ast, i+1, *rhs);
                            AST::callExpr::setActual(ast, i, *rhs);
                            foundOtherTypeFormal = true;
                        }
                        break;
                    }
                }

                if (!foundOtherTypeFormal) {
                    // didn't find anything to help... do the usual thing
                    const RootTyp& actualType = gu->substitute(formalType);
                    AstNode* rhs = &AST::callExpr::actual(ast, i);
                    CallExprParamContext context(*this, ast, actualType);
                    visit(*rhs);
                    performPromotions(actualType, rhs, false);
                    // resetAstChild(AstPool::instance(), ast, i+1, *rhs);
                    AST::callExpr::setActual(ast, i, *rhs);
                }
            }

            else {
                const RootTyp& actualType = gu->substitute(formalType);
                AstNode* rhs = &AST::callExpr::actual(ast, i);
                CallExprParamContext context(*this, ast, actualType);
                visit(*rhs);
                performPromotions(actualType, rhs, false);
                // resetAstChild(AstPool::instance(), ast, i+1, *rhs);
                AST::callExpr::setActual(ast, i, *rhs);
            }
        }
    }

    call.initActualTypes(
      ast); // reset types on actuals now that we've done promotions (potentially)
    _parentNodeStack.pop();
}

void TypePromoter::visitCastExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);

    _parentNodeStack.push(ast);

    CastExprContext context(*this, ast);
    AstNode* child = &AST::castExpr::expr(ast);
    visit(*child);

    const RootTyp& fromType = astSymbol(*child).type();
    const RootTyp& toType = astSymbol(AST::castExpr::type(ast)).type();

    // The only time we perform promotions is if the from type is null
    if (fromType.getMetaType() != MetaType::NUL) {
        return;
    }

    performPromotions(toType, child, false);
    // resetAstChild(AstPool::instance(), ast, 1, *child);
    AST::castExpr::setExpr(ast, *child);

    _parentNodeStack.pop();
}

void TypePromoter::visitInfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);

    _parentNodeStack.push(ast);

    AstNode* lhs = &AST::infixExpr::lhs(ast);
    {
        InfixExprContext context(*this, ast, true);
        visit(*lhs);
    }
    AstNode* rhs = &AST::infixExpr::rhs(ast);
    {
        InfixExprContext context(*this, ast, false);
        visit(*rhs);
    }

    RootTyp const& lhsType = astSymbol(*lhs).type();
    RootTyp const& rhsType = astSymbol(*rhs).type();
    Expression::Op op = astNodeOp(ast);
    switch (op) {
        case Expression::eq:
            performPromotions(lhsType, rhs, false);
            // resetAstChild(AstPool::instance(), ast, 1, *rhs);
            AST::infixExpr::setRhs(ast, *rhs);
            break;

        case Expression::in: {
            switch (rhsType.getMetaType()) {
                case MetaType::UNKNOWN:
                    break;

                case MetaType::RSTRING:
                case MetaType::BSTRING:
                case MetaType::USTRING:
                case MetaType::BLOB:
                case MetaType::LIST:
                case MetaType::BLIST:
                case MetaType::SET:
                case MetaType::BSET:
                    performPromotions(ExpressionTypeFinder::elementType(rhsType), lhs, false);
                    break;
                case MetaType::MAP:
                    performPromotions(rhsType.as<MapTyp>().getKeyType(), lhs, false);
                    break;
                case MetaType::BMAP:
                    performPromotions(rhsType.as<BMapTyp>().getKeyType(), lhs, false);
                    break;
                default:
                    break;
            }
            // resetAstChild(AstPool::instance(), ast, 0, *lhs);
            AST::infixExpr::setLhs(ast, *lhs);
        } break;

        case Expression::eqEq:
        case Expression::neq: {
            // Null literals are promoted to the type of the other operand
            // if (AST::nullLiteralT::is(*rhs) && lhsType.isOptional()) {
            //    astSymbol(*rhs).setType(lhsType);
            //}
            // else if (AST::nullLiteralT::is(*lhs) && rhsType.isOptional()) {
            //    astSymbol(*lhs).setType(rhsType);
            //}
            if (performPromotions(lhsType, rhs, false)) {
                AST::infixExpr::setRhs(ast, *rhs);
            } else if (performPromotions(rhsType, lhs, false)) {
                AST::infixExpr::setLhs(ast, *lhs);
            }
        } break;
        default:
            break;
    }

    _parentNodeStack.pop();
}

void TypePromoter::visitSetLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());

    _parentNodeStack.push(ast);

    // Visit all the entries in the set to see if any are null and to find a common type
    // for the elements
    const RootTyp* elementType = NULL;
    bool nullSeen = false;
    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        AstNode& element = AST::setLiteral::expr(ast, i);
        RootTyp const& et = astSymbol(element).type();
        if (et.getMetaType() == MetaType::NUL) {
            nullSeen = true;
        } else if (NULL == elementType) {
            elementType = (et.is<OptionalTyp>()) ? &et.as<OptionalTyp>().getUnderlyingType() : &et;
        } else {
            // If the types are not homogenous (or null) give up
            if ((*elementType) != (const RootTyp&)((et.is<OptionalTyp>())
                                                     ? et.as<OptionalTyp>().getUnderlyingType()
                                                     : et)) {
                _parentNodeStack.pop();
                return;
            }
        }
    }

    // At this point we have an element type which is either NULL (the entry in the list is null) or
    // non NULL (but one entry may be null)
    if (NULL != elementType) {
        if (nullSeen) {
            // We found an element type and there is a one null. The element type must then be
            // optional<whatever>
            if (!elementType->isOptional()) {
                // The common type is not optional, so promote it to optional
                elementType = &typeFactory().optionalT(*elementType);
            }
        }
    } else {
        // elementType is NULL, so all we saw was a null entry in the set
        // In this case we need to figure out the type from the context stack
        const RootTyp& setType = currentContext().type();
        elementType = &ExpressionTypeFinder::elementType(setType);

        if (elementType->getMetaType() == MetaType::UNKNOWN) {
            return;
        }
    }
    astSymbol(ast).setType(typeFactory().setT(*elementType));

    // Perform any promotions that might be necesary
    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        SetLiteralElementContext elementContext(*this, ast);
        AstNode* element = &AST::setLiteral::expr(ast, i);
        visit(*element);
        // Promote all entries to the common type
        performPromotions(*elementType, element, false);
        // resetAstChild(AstPool::instance(), ast, i, *element);
        AST::setLiteral::setExpr(ast, i, *element);
    }

    _parentNodeStack.pop();
}

void TypePromoter::visitMapLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());

    _parentNodeStack.push(ast);

    const RootTyp* keyType = NULL;
    bool nullKeySeen = false;
    const RootTyp* valueType = NULL;
    bool nullValueSeen = false;

    for (int i = 0, n = AST::mapLiteral::mappingCount(ast); i < n; i++) {
        AstNode& child = AST::mapLiteral::mapping(ast, i);
        RootTyp const& literalKeyType = astSymbol(AST::mapping::fromExpr(child)).type();

        if (literalKeyType.getMetaType() == MetaType::NUL) {
            nullKeySeen = true;
        } else if (NULL == keyType) {
            keyType = (literalKeyType.is<OptionalTyp>())
                        ? &literalKeyType.as<OptionalTyp>().getUnderlyingType()
                        : &literalKeyType;
        } else {
            // If the types are not homogenous (or null) give up
            if ((*keyType) !=
                (const RootTyp&)((literalKeyType.is<OptionalTyp>())
                                   ? literalKeyType.as<OptionalTyp>().getUnderlyingType()
                                   : literalKeyType)) {
                _parentNodeStack.pop();
                return;
            }
        }

        RootTyp const& literalValueType = astSymbol(AST::mapping::toExpr(child)).type();
        if (literalValueType.getMetaType() == MetaType::NUL) {
            nullValueSeen = true;
        } else if (NULL == valueType) {
            valueType = (literalValueType.is<OptionalTyp>())
                          ? &literalValueType.as<OptionalTyp>().getUnderlyingType()
                          : &literalValueType;
        } else {
            // If the types are not homogenous (or null) give up
            if ((*valueType) !=
                (const RootTyp&)((literalValueType.is<OptionalTyp>())
                                   ? literalValueType.as<OptionalTyp>().getUnderlyingType()
                                   : literalValueType)) {
                _parentNodeStack.pop();
                return;
            }
        }
    }

    // At this point we have an key type which is either NULL (a key entry in the map is null) or
    // non NULL (but one entry may be null)
    if (NULL != keyType) {
        if (nullKeySeen) {
            // We found an element type and there is a one null. The element type must then be
            // optional<whatever>
            if (!keyType->isOptional()) {
                // The common type is not optional, so promote it to optional
                keyType = &typeFactory().optionalT(*keyType);
            }
        }
    } else {
        // keyType is NULL, so all we saw was a null entry in the map
        // In this case we need to figure out the type from the context stack
        const RootTyp& mapType = currentContext().type();
        keyType = &SPL::keyType(mapType);

        if (keyType->getMetaType() == MetaType::UNKNOWN) {
            _parentNodeStack.pop();
            return;
        }
    }

    // At this point we have a value type which is either NULL (a value entry in the map is null) or
    // non NULL (but one entry may be null)
    if (NULL != valueType) {
        if (nullValueSeen) {
            // We found an element type and there is a one null. The element type must then be
            // optional<whatever>
            if (!valueType->isOptional()) {
                // The common type is not optional, so promote it to optional
                valueType = &typeFactory().optionalT(*valueType);
            }
        }
    } else {
        // vType is NULL, so all we saw was a null entry in the map
        // In this case we need to figure out the type from the context stack
        const RootTyp& mapType = currentContext().type();
        valueType = &SPL::valueType(mapType);

        if (valueType->getMetaType() == MetaType::UNKNOWN) {
            _parentNodeStack.pop();
            return;
        }
    }
    astSymbol(ast).setType(typeFactory().mapT(*keyType, *valueType));
    bool inLocalDeclContext = currentContext().is<LocalDeclItemContext>();

    // Perform any promotions that might be necesary
    for (int i = 0, n = AST::mapLiteral::mappingCount(ast); i < n; i++) {
        AstNode& child = AST::mapLiteral::mapping(ast, i);

        AstNode* key = &AST::mapping::fromExpr(child);
        {
            MapLiteralKeyContext keyContext(*this, ast);
            visit(*key);
        }
        // Promote all entry keys to the common type
        performPromotions(*keyType, key, inLocalDeclContext);
        // resetAstChild(AstPool::instance(), child, 0, *key);
        AST::mapping::setFromExpr(child, *key);

        AstNode* value = &AST::mapping::toExpr(child);
        {
            MapLiteralValueContext valueContext(*this, ast);
            visit(*value);
        }
        // Promote all entry values to the common type
        performPromotions(*valueType, value, inLocalDeclContext);
        // resetAstChild(AstPool::instance(), child, 1, *value);
        AST::mapping::setToExpr(child, *value);
    }

    _parentNodeStack.pop();
}

void TypePromoter::visitListLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypePromoter);

    _parentNodeStack.push(ast);

    // Visit all the entries of the list to see if any entries are the literal null and
    // to ensure that all non-null entries have a consistent type
    const RootTyp* elementType = NULL;
    bool nullSeen = false;
    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        AstNode& element = AST::listLiteral::expr(ast, i);
        RootTyp const& et = astSymbol(element).type();
        if (et.getMetaType() == MetaType::NUL) {
            nullSeen = true;
        } else if (NULL == elementType) {
            elementType = (et.is<OptionalTyp>()) ? &et.as<OptionalTyp>().getUnderlyingType() : &et;
        } else {
            // If the types are not homogenous (or null) give up
            if ((*elementType) != (const RootTyp&)((et.is<OptionalTyp>())
                                                     ? et.as<OptionalTyp>().getUnderlyingType()
                                                     : et)) {
                _parentNodeStack.pop();
                return;
            }
        }
    }
    // At this point we have an element type which is either NULL (all entries in the list are null)
    // or non NULL (but some entries may be null)
    if (NULL != elementType) {
        // We found a common type other than NULL

        if (nullSeen) {
            // We found an element type and there is at least one null. The element type must then
            // be optional<whatever>
            if (!elementType->isOptional()) {
                // The common type is not optional, so promote it to optional
                elementType = &typeFactory().optionalT(*elementType);
            }
        }
    } else {
        // elementType is NULL, so all we saw were null entries in the list
        // In this case we need to figure out the type from the context stack
        const RootTyp& listType = currentContext().type();
        elementType = &ExpressionTypeFinder::elementType(listType);

        if (elementType->getMetaType() == MetaType::UNKNOWN) {
            elementType = &(typeFactory().nullT());
            _parentNodeStack.pop();
            return;
        }
    }
    astSymbol(ast).setType(typeFactory().listT(*elementType));

    ListLiteralContext context(*this, ast);

    // Perform any promotions that might be necesary
    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        ListLiteralElementContext elementContext(*this, ast);
        AstNode* element = &AST::listLiteral::expr(ast, i);
        visit(*element);
        // Promote all entries to the common type
        performPromotions(*elementType, element, false);
        // resetAstChild(AstPool::instance(), ast, i, *element);
        AST::listLiteral::setExpr(ast, i, *element);
    }

    _parentNodeStack.pop();
}

void TypePromoter::visitSubscriptExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());

    _parentNodeStack.push(ast);

    AstNode& collAst = AST::subscriptExpr::expr(ast);
    SymbolTableEntry& collSym = astSymbol(collAst);
    AstNode* subscriptExpr = &AST::subscriptExpr::subscript(ast);
    bool subIsSplice = AST::subscriptSlice::is(*subscriptExpr);
    SubscriptExprContext context(*this, ast);
    visit(*subscriptExpr);
    if (symIsStream(collSym)) {
        if (!subIsSplice) {
            performPromotions(typeFactory().primitiveT(MetaType::UINT32), subscriptExpr, false);
            // resetAstChild(AstPool::instance(), ast, 1, *subscriptExpr);
            AST::subscriptExpr::setSubscript(ast, *subscriptExpr);
        }
    } else {
        const RootTyp& collType = collSym.type();
        if (collType.getMetaType() == MetaType::UNKNOWN) {
            _parentNodeStack.pop();
            return;
        }
        const RootTyp& indexType = subscriptType(collType);
        if (!subIsSplice) {
            performPromotions(indexType, subscriptExpr, false);
            // resetAstChild(AstPool::instance(), ast, 1, *subscriptExpr);
            AST::subscriptExpr::setSubscript(ast, *subscriptExpr);
        }
    }

    _parentNodeStack.pop();
}

void TypePromoter::visitAttributeAssign(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());
    _parentNodeStack.push(ast);
    AstNode& id = AST::attributeAssign::id(ast);
    AstNode* expr = &AST::attributeAssign::expr(ast);
    AttributeAssignContext context(*this, id);
    visit(*expr);
    const RootTyp* expectedType = &(astSymbol(id).type());

    if (expectedType->getMetaType().getValue() == MetaType::NUL) {
        // might always be the case?  Not sure.
        // Walk the parent contexts and see if we can get a type from there.
        const TypePromoterContextBase* parentContextBase =
          &(this->_currentContext->previousContext());
        while (!parentContextBase->is<TypePromoterContexts::StartContext>() &&
               parentContextBase->type().getMetaType().getValue() == MetaType::NUL) {
            parentContextBase = &(parentContextBase->previousContext());
        }

        if (!parentContextBase->is<TypePromoterContexts::StartContext>()) {
            const RootTyp& parentType = parentContextBase->type();
            if (parentType.is<TupleTyp>()) {
                // if there's an attribute of the same name as our ID, use its type
                const TupleTyp::Name2Index& attributeMap =
                  parentType.as<TupleTyp>().getAttributeNames();
                TupleTyp::Name2Index::const_iterator it =
                  attributeMap.find((char*)(id.getText(&id)->chars));
                if (it != attributeMap.end()) {
                    expectedType = &(parentType.as<TupleTyp>().getAttributeType(it->second));
                }
            }
        }
    }

    performPromotions(*expectedType, expr, false);
    // resetAstChild(AstPool::instance(), ast, 1, *expr);
    AST::attributeAssign::setExpr(ast, *expr);
    _parentNodeStack.pop();
}

void TypePromoter::visitReturnStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());

    _parentNodeStack.push(ast);

    FunctionHeadSym const* fun = SymbolTable::instance().function();
    if (NULL != fun) {
        if (AST::returnStmt::has_expr(ast)) {
            AstNode* expr = &AST::returnStmt::expr(ast);
            ReturnStmtContext context(*this, ast, *fun);
            visit(*expr);
            RootTyp const& expectedType = fun->returnType();
            performPromotions(expectedType, expr, false);
            // resetAstChild(AstPool::instance(), ast, 0, *expr);
            AST::returnStmt::setExpr(ast, *expr);
        }
    }

    _parentNodeStack.pop();
}

void TypePromoter::visitOpInvokeActual(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());

    _parentNodeStack.push(ast);
    OpInvokeActualContext context(*this, ast);
    AstVisitorPushPop::visitOpInvokeActual(ast);

    // get our symbol
    if (astSymbol(ast).is<OpInvokeActualSym>()) {
        OpInvokeActualSym& parameterActualSym = astSymbol(ast).as<OpInvokeActualSym>();

        // get the parent scope (it's the sym for the operator invocation)
        if (parameterActualSym.parentScope()->holder()->is<OpInvokeSym>()) {
            OpInvokeSym& opInvokeSym =
              parameterActualSym.parentScope()->holder()->as<OpInvokeSym>();
            OperatorSym* pOpSym = opInvokeSym.op();

            // it's possible at this point that the symbol isn't found (which is an error
            // on the part of the user).  If so, we don't do anything here, there are other checks
            // for that.
            if (pOpSym != NULL) {

                // find out the type of the parameter from the operator model
                const Operator::OperatorModel* pOpModel =
                  PathSearch::instance().findOperatorModel(pOpSym->fullName(), NULL);

                if (pOpModel != NULL) { // non-primitive operators don't have an operator model
                    const Operator::Parameters::NamedParameters& params =
                      pOpModel->getParameters().getNamedParameters();

                    AstNode& idNode = AST::opInvokeActual::id(ast);
                    std::string nodeText((char*)idNode.getText(&idNode)->chars);
                    SPL::Operator::Parameters::NamedParameters::const_iterator it =
                      params.find(nodeText);

                    if (it != params.end()) {
                        // get parameter's type
                        SPL::Operator::ParameterPtr pParam = (*it).second;

                        assert(pParam != NULL);

                        if (pParam->hasType()) {
                            const std::string& parameterTypeString = pParam->getType();

                            const RootTyp* parameterType = Utility::splType(parameterTypeString);
                            if (parameterType != NULL &&
                                (*parameterType) != typeFactory().instance().unknownT()) {

                                AstNode& expressions = AST::opInvokeActual::opActual(ast);
                                if (AST::opActualExprs::is(expressions)) {
                                    for (int k = 0; k < AST::opActualExprs::exprCount(expressions);
                                         k++) {
                                        AstNode* expr = &(AST::opActualExprs::expr(expressions, k));
                                        performPromotions(*parameterType, expr, false);
                                    }
                                }
                            }
                        }
                    }

                }

                else { // composite operator with no operator model

                    Scope* paramScope = pOpSym->paramsScope();

                    AstNode& idNode = AST::opInvokeActual::id(ast);

                    // composite operator parameters are expressions prepended by $
                    std::string nodeText("$");
                    nodeText.append(((char*)idNode.getText(&idNode)->chars));

                    SymbolTableEntry& paramSymbol = paramScope->get(nodeText);

                    AstNode& expressions = AST::opInvokeActual::opActual(ast);

                    if (AST::opActualExprs::is(expressions)) {
                        // go through all the expressions and promote using the type of the
                        // appropriate parameter
                        for (int j = 0; j < AST::opActualExprs::exprCount(expressions); j++) {
                            AstNode* expr = &(AST::opActualExprs::expr(expressions, j));

                            performPromotions(paramSymbol.type(), expr, false);
                        }
                    }
                }
            }
        } else {
            // bad
            assert(false);
        }
    }

    _parentNodeStack.pop();
}

void TypePromoter::visitNamespace(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitUseDirectives(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitUseDirective(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitNamespaceName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitUseDirectiveTail(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitUseWildcard(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Composite operators are defined at the top-level in a namespace.
void TypePromoter::visitCompositeHead(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitSplDoc(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitMlComment(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void TypePromoter::visitCompositeIn(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitCompositeOut(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitCompositePort(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitCompositeBody(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitFormalClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTypeClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitVarClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitGraphClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitConfigClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitCompositeFormalInit(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOpActualExprs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOpActualName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOpActualType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Streams are defined in a composite operator's graph clause.

void TypePromoter::visitOpInvokeHead(AstNode& ast)
{
    _parentNodeStack.push(ast);
    visit(AST::opInvokeHead::outputs(ast));
    visit(AST::opInvokeHead::alias(ast));
    visit(AST::opInvokeHead::opName(ast));
    visit(AST::opInvokeHead::inputs(ast));
    visit(AST::opInvokeHead::annotations(ast));
    _parentNodeStack.pop();
}

void TypePromoter::visitOpOutputs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOpOutput(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitStreamType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitAsAlias(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOpInputs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitPortInputs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOpInvokeBody(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitLogicClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitWindowClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitActualClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOutputClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitLogicState(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Functions are defined at the top-level in a namespace.
void TypePromoter::visitFunctionHead(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitFunctionFormal(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Function Prototype declarations come from XML files, not regular SPL files.
void TypePromoter::visitTypeFormals(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTypeFormal(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitBoundsFormals(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitProtoFormals(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitProtoFormal(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void TypePromoter::visitBlockStmtNoScope(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitExprStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitIfStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitElseClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitForInit(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitWhileStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitBreakStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitContinueStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Expressions can appear in many places in the grammar.
void TypePromoter::visitMapping(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitAttributeExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitParenthesizedExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitPostfixExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitPrefixExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitQualifierExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitSlice(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitId(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitUnwrapExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitUnwrapOrElseExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitIsPresentExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Literals are the highest-precedence expressions denoting values.
void TypePromoter::visitCurlyLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitBooleanLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitStringLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitXMLLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitNumericLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void TypePromoter::visitDefType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitExpressionMode(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTypeName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitXMLType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitListType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitMapType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitPrimitiveType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitPrimitiveTypeName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitRStringType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitSetType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTupleType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitUStringType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitVoidType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOptionalType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitAttributeDecl(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTypeArgs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTypeDims(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Modifiers prefix function, variable, and type definitions.
void TypePromoter::visitModifiers(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ---- Annotations
void TypePromoter::visitAnnotations(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitAnnotation(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitAnnotationT(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitCompilationUnit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    AstNode& nsAst = AST::compilationUnit::splNamespace(ast);
    _parentNodeStack.push(ast);
    visit(nsAst);
    _symTab.pushOld(nsAst);
    _symTab.pushOld(ast);
    visit(AST::compilationUnit::useDirectives(ast));
    for (int i = 0, n = AST::compilationUnit::definitionCount(ast); i < n; i++) {
        visit(AST::compilationUnit::definition(ast, i));
    }
    _symTab.pop(ast);
    _symTab.pop(nsAst);
    _parentNodeStack.pop();
}

// ----- Composite operators are defined at the top-level in a namespace.
void TypePromoter::visitCompositeDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _symTab.pushOld(ast);
    _parentNodeStack.push(ast);
    AstVisitor::visitCompositeDef(ast);
    _parentNodeStack.pop();
    _symTab.pop(ast);
}

void TypePromoter::visitCompositeFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    visit(AST::compositeFormal::expressionMode(ast));
    visit(AST::compositeFormal::id(ast));
    _symTab.pushOld(ast);
    visit(AST::compositeFormal::compositeFormalInit(ast));
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitConfigItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    visit(AST::config::id(ast));
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        visit(AST::config::expr(ast, i));
    }
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

// ----- Streams are defined in a composite operator's graph clause.
void TypePromoter::visitOpInvoke(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitOpInvoke(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOnProcessLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    visit(AST::opInvokeLogicOnProcess::stmt(ast));
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOnTupleLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    visit(AST::opInvokeLogicOnTuple::id(ast));
    _symTab.pushOld(ast);
    visit(AST::opInvokeLogicOnTuple::stmt(ast));
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitOnPunctLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    visit(AST::opInvokeLogicOnPunct::id(ast));
    _symTab.pushOld(ast);
    visit(AST::opInvokeLogicOnPunct::stmt(ast));
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        void TypePromoter::visitOnWatermarkLogic(AstNode & ast) {
		SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")", debugFlag());
		_parentNodeStack.push(ast);
		visit(AST::opInvokeLogicOnWatermark::id(ast));
		_symTab.pushOld(ast);
		visit(AST::opInvokeLogicOnWatermark::stmt(ast));
		_symTab.pop(ast);
		_parentNodeStack.pop();
	}
#endif

void TypePromoter::visitOpInvokeWindow(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    visit(AST::opInvokeWindow::id(ast));
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        visit(AST::opInvokeWindow::expr(ast, i));
    }
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

// ----- Functions are defined at the top-level in a namespace.
void TypePromoter::visitFunctionDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitFunctionDef(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

// ----- Function Prototype declarations come from XML files, not regular SPL files.
void TypePromoter::visitFunctionPrototype(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitFunctionPrototype(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void TypePromoter::visitBlockStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitBlockStmt(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitForStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstNode& initAst = AST::forStmt::forInit(ast);
    visit(AST::forInit::modifiers(initAst));
    visit(AST::forInit::type(initAst));
    visit(AST::forInit::id(initAst));
    _symTab.pop(ast);
    visit(AST::forInit::expr(initAst));
    _symTab.pushOld(ast);
    visit(AST::forStmt::stmt(ast));
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

// ----- Literals are the highest-precedence expressions denoting values.
void TypePromoter::visitTupleLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitTupleLiteral(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void TypePromoter::visitEnumType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitEnumType(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTupleAttrib(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitTupleAttrib(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

void TypePromoter::visitTupleExtend(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    _parentNodeStack.push(ast);
    _symTab.pushOld(ast);
    AstVisitor::visitTupleExtend(ast);
    _symTab.pop(ast);
    _parentNodeStack.pop();
}

#define TYPE_PROMOTER_DEFAULT(x)                                                                   \
    void TypePromoter::x(AstNode& ast) { AstVisitorPushPop::x(ast); }

TYPE_PROMOTER_DEFAULT(visitTupleT)
TYPE_PROMOTER_DEFAULT(visitPublicT)
TYPE_PROMOTER_DEFAULT(visitUnaryMinusT)
TYPE_PROMOTER_DEFAULT(visitMutableT)
TYPE_PROMOTER_DEFAULT(visitTypeT)
TYPE_PROMOTER_DEFAULT(visitStatefulT)
TYPE_PROMOTER_DEFAULT(visitNoneT)
TYPE_PROMOTER_DEFAULT(visitQMarkT)
TYPE_PROMOTER_DEFAULT(visitEnumT)
TYPE_PROMOTER_DEFAULT(visitBangT)
TYPE_PROMOTER_DEFAULT(visitStaticT)
TYPE_PROMOTER_DEFAULT(visitOptionalT)
TYPE_PROMOTER_DEFAULT(visitCompositeT)
TYPE_PROMOTER_DEFAULT(visitSetT)
TYPE_PROMOTER_DEFAULT(visitExpressionT)
TYPE_PROMOTER_DEFAULT(visitOperatorT)
TYPE_PROMOTER_DEFAULT(visitPlusPlusT)
TYPE_PROMOTER_DEFAULT(visitTildeT)
TYPE_PROMOTER_DEFAULT(visitMapT)
TYPE_PROMOTER_DEFAULT(visitAttributeT)
TYPE_PROMOTER_DEFAULT(visitFunctionT)
TYPE_PROMOTER_DEFAULT(visitMinusMinusT)
TYPE_PROMOTER_DEFAULT(visitListT)

} // namespace SPL
