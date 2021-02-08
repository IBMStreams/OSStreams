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

#include <SPL/FrontEnd/ExpressionTypeFinder.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/TypeAnalyzer.h>
#include <SPL/FrontEnd/TypeFactory.h>

#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

using namespace std;
using namespace SPL;

SourceLocation const& TypeAnalyzer::newLoc(AstNode& ast)
{
    return SourceLocationFactory::instance().create(_pCtx, ast);
}

TypeAnalyzer::TypeAnalyzer(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceTypeAnalyzer)
  , _typeFactory(TypeFactory::instance())
{}

void TypeAnalyzer::setValueExprType(AstNode& ast, RootTyp const& type)
{
    setAstSymbol(AstPool::instance(), ast, *new ExpressionSym(newLoc(ast), ast, type));
}

void TypeAnalyzer::run(ParserContext& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           Debug::TraceTypeAnalyzer);
    TypeAnalyzer visitor(pCtx);
    visitor.visit(ast);
}

void TypeAnalyzer::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast
                               << ") stage is " << astStage(ast).toString(),
           debugFlag());
    if (astStage(ast) < FrontEndStage::TYPE_ANALYZER) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::TYPE_ANALYZER);
        AstVisitorPushPop::visit(ast);
    }
}

void TypeAnalyzer::visitNullLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    const SimplePrimitiveTyp& type = typeFactory().nullT();
    setValueExprType(ast, type);
}

void TypeAnalyzer::visitXMLLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitXMLLiteral(ast);
    const XMLTyp& type = TypeFactory::instance().xmlT();
    setValueExprType(ast, type);
}

void TypeAnalyzer::visitCastExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitCastExpr(ast);

    RootTyp const& toType = astSymbol(AST::castExpr::type(ast)).type();
    setValueExprType(ast, toType);
}

void TypeAnalyzer::visitConditionalExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitConditionalExpr(ast);

    TypeFactory& tf = TypeFactory::instance();
    RootTyp const& thenType = astSymbol(AST::conditionalExpr::thenExpr(ast)).type();
    RootTyp const& elseType = astSymbol(AST::conditionalExpr::elseExpr(ast)).type();
    RootTyp const& unknown = tf.unknownT();

    setValueExprType(ast, thenType == unknown ? elseType : thenType);
}

void TypeAnalyzer::visitParenthesizedExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitParenthesizedExpr(ast);
    setValueExprType(ast, astSymbol(AST::parenthesizedExpr::expr(ast)).type());
}

void TypeAnalyzer::visitPostfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitPostfixExpr(ast);
    RootTyp const& type = astSymbol(AST::postfixExpr::expr(ast)).type();
    setValueExprType(ast, type);
}

void TypeAnalyzer::visitPrefixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitPrefixExpr(ast);
    RootTyp const& type = astSymbol(AST::prefixExpr::expr(ast)).type();
    setValueExprType(ast, type);
}

void TypeAnalyzer::visitCurlyLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitCurlyLiteral(ast);
    assert(AST::emptyCurlyLiteralT::is(ast) && 0 == astSize(ast));
    setValueExprType(ast, TypeFactory::instance().emptyCurlyT());
}

void TypeAnalyzer::visitBooleanLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitBooleanLiteral(ast);
    setValueExprType(ast, TypeFactory::instance().primitiveT(MetaType::BOOLEAN));
}

void TypeAnalyzer::visitCallExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    assert(AST::callExpr::is(ast));

    // Visit each argument
    for (int j = 0, cnt = AST::callExpr::actualCount(ast); j < cnt; ++j) {
        visit(AST::callExpr::actual(ast, j));
    }

    Scope* scope = &_symTab.currentScope();
    (void)new CallExpressionSym(newLoc(ast), ast, scope);
}

void TypeAnalyzer::visitStringLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitStringLiteral(ast);
    std::string token = astText(ast);
    int bitsSuffix = 8;
    for (int i = token.size() - 1; i >= 0; i--) {
        switch (token[i]) {
            case 'r':
                bitsSuffix = 8;
                break;
            case 'u':
                bitsSuffix = 16;
                break;
            case '"':
                goto doneParsingSuffix;
            case '\'':
                goto doneParsingSuffix;
            default:
                assert(!"should never reach here");
        }
    }
doneParsingSuffix:
    setValueExprType(ast, TypeFactory::instance().primitiveT(8 == bitsSuffix ? MetaType::RSTRING
                                                                             : MetaType::USTRING));
}

void TypeAnalyzer::visitNumericLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitNumericLiteral(ast);
    RootTyp const* result = NULL;
    if (AST::hexLiteralT::is(ast)) {
        char typeSuffix = 's';
        int bitsSuffix = -1;
        std::string token = astText(ast);
        int i = token.size() - 1;
        for (; i >= 0; i--) {
            switch (token[i]) {
                case 's':
                    typeSuffix = 's';
                    break;
                case 'u':
                    typeSuffix = 'u';
                    break;
                default:
                    goto doneParsingHexSuffix;
            }
        }
    doneParsingHexSuffix:
        int negSign = (token[0] == '-' ? 1 : 0);
        // at this point i is the length of the token without the suffix but
        // may include a negative size
        if (i > 17 + negSign) { // > 16 hex digits + '0x'
            bitsSuffix = 128;
        } else if (i > 9 + negSign) { // > 8 hex digits  + '0x'
            bitsSuffix = 64;
        } else if (i > 5 + negSign) { // > 4 hex digits  + '0x'
            bitsSuffix = 32;
        } else if (i > 3 + negSign) { // > 2 hex digits  + '0x'
            bitsSuffix = 16;
        } else {
            bitsSuffix = 8;
        }
        result = &TypeFactory::instance().numberT(typeSuffix, bitsSuffix);
        // hex literals greater than 16 hex digits are not yet supported
        if (bitsSuffix == 128) {
            switch (typeSuffix) {
                case 's':
                    SysOut::errorln(SPL_FRONT_END_TYPE_INVALID_HEXADECIMAL_LITERAL("int"),
                                    newLoc(ast));
                    result = &TypeFactory::instance().unknownT();
                    break;
                case 'u':
                    SysOut::errorln(SPL_FRONT_END_TYPE_INVALID_HEXADECIMAL_LITERAL("uint"),
                                    newLoc(ast));
                    result = &TypeFactory::instance().unknownT();
                    break;
            }
        }
    } else {
        char typeSuffix = AST::intLiteralT::is(ast) ? 's' : 'f';
        int bitsSuffix = -1;
        std::string token = astText(ast);
        for (int i = token.size() - 1; i >= 0; i--) {
            switch (token[i]) {
                case 's':
                    typeSuffix = 's';
                    break;
                case 'u':
                    typeSuffix = 'u';
                    break;
                case 'f':
                    typeSuffix = 'f';
                    break;
                case 'd':
                    typeSuffix = 'd';
                    break;
                case 'b':
                    bitsSuffix = 8;
                    break;
                case 'h':
                    bitsSuffix = 16;
                    break;
                case 'w':
                    bitsSuffix = 32;
                    break;
                case 'l':
                    bitsSuffix = 64;
                    break;
                case 'q':
                    bitsSuffix = 128;
                    break;
                default:
                    goto doneParsingDecSuffix;
            }
        }
    doneParsingDecSuffix:
        switch (typeSuffix) {
            case 's':
                if (128 == bitsSuffix) {
                    SysOut::errorln(SPL_FRONT_END_TYPE_INVALID_NUMERIC_LITERAL("int", bitsSuffix),
                                    newLoc(ast));
                    result = &TypeFactory::instance().unknownT();
                }
                break;
            case 'u':
                if (128 == bitsSuffix) {
                    SysOut::errorln(SPL_FRONT_END_TYPE_INVALID_NUMERIC_LITERAL("uint", bitsSuffix),
                                    newLoc(ast));
                    result = &TypeFactory::instance().unknownT();
                }
                break;
            case 'f':
                if (8 == bitsSuffix || 16 == bitsSuffix || 128 == bitsSuffix) {
                    SysOut::errorln(SPL_FRONT_END_TYPE_INVALID_NUMERIC_LITERAL("float", bitsSuffix),
                                    newLoc(ast));
                    result = &TypeFactory::instance().unknownT();
                }
                break;
            case 'd':
                if (8 == bitsSuffix || 16 == bitsSuffix) {
                    SysOut::errorln(
                      SPL_FRONT_END_TYPE_INVALID_NUMERIC_LITERAL("decimal", bitsSuffix),
                      newLoc(ast));
                    result = &TypeFactory::instance().unknownT();
                }
                break;
            default:
                assert(!"can never reach this statement");
        }
        if (NULL == result) {
            result = &TypeFactory::instance().numberT(typeSuffix, bitsSuffix);
        }
    }
    assert(NULL != result);
    setValueExprType(ast, *result);
}

void TypeAnalyzer::visitTupleLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitTupleLiteral(ast);
}

void TypeAnalyzer::visitSubscriptExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitSubscriptExpr(ast);
    AstNode& collAst = AST::subscriptExpr::expr(ast);
    SymbolTableEntry& collSym = astSymbol(collAst);
    RootTyp const& collType = collSym.type();

    // In some cases we don't know the type. IE: Test spl_codeGen_d13310
    if (collType.getMetaType() == MetaType::UNKNOWN) {
        setValueExprType(ast, typeFactory().unknownT());
        return;
    }
    if (symIsStream(collSym)) {
        setValueExprType(ast, collType);
    } else {
        bool idxIsSlice = AST::subscriptSlice::is(AST::subscriptExpr::subscript(ast));
        RootTyp const& result = idxIsSlice ? collType : ExpressionTypeFinder::elementType(collType);
        setValueExprType(ast, result);
    }
}

void TypeAnalyzer::visitListLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());
    AstVisitorPushPop::visitListLiteral(ast);

    // For a first pass, find a non-null common type. If the elements are not homogenous it will be
    // diagnosed later
    const RootTyp* commonType = &typeFactory().unknownT();

    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        AstNode& element = AST::listLiteral::expr(ast, i);
        RootTyp const& et = astSymbol(element).type();
        if (et.getMetaType() != MetaType::NUL) {
            commonType = &et;
            break;
        }
    }

    setValueExprType(ast, typeFactory().listT(*commonType));
}

void TypeAnalyzer::visitMapLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());
    AstVisitorPushPop::visitMapLiteral(ast);
    RootTyp const& unknown = typeFactory().unknownT();
    const RootTyp* commonKeyType = NULL;
    const RootTyp* commonValueType = NULL;

    for (int i = 0, n = AST::mapLiteral::mappingCount(ast); i < n; i++) {
        AstNode& child = AST::mapLiteral::mapping(ast, i);
        RootTyp const& kt = astSymbol(AST::mapping::fromExpr(child)).type();
        RootTyp const& vt = astSymbol(AST::mapping::toExpr(child)).type();
        if (commonKeyType == NULL && kt.getMetaType() != MetaType::NUL) {
            commonKeyType = &kt;
        }
        if (commonValueType == NULL && vt.getMetaType() != MetaType::NUL) {
            commonValueType = &vt;
        }
    }

    setValueExprType(ast, typeFactory().mapT(commonKeyType != NULL ? *commonKeyType : unknown,
                                             commonValueType != NULL ? *commonValueType : unknown));
}

void TypeAnalyzer::visitInfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());
    AstVisitorPushPop::visitInfixExpr(ast);

    AstNode& lhs = AST::infixExpr::lhs(ast);
    AstNode& rhs = AST::infixExpr::rhs(ast);
    RootTyp const& type0 = astSymbol(lhs).type();
    RootTyp const& type1 = astSymbol(rhs).type();

    RootTyp const& unknown = typeFactory().unknownT();
    if ((type0) == unknown || (type1) == unknown) {
        setValueExprType(ast, unknown);
        return;
    }

    Expression::Op op = astNodeOp(ast);
    const RootTyp* result = &unknown;
    switch (op) {

        case Expression::plus:
        case Expression::minus:
        case Expression::star:
        case Expression::slash:
        case Expression::mod:
        case Expression::hat:
        case Expression::amp:
        case Expression::bar:
        case Expression::ampAmp:
        case Expression::barBar:
            if (ExpressionTypeFinder::isMappableContainer(type1) &&
                ExpressionTypeFinder::isElementOf(type0, type1)) {
                result = &type1;
            } else {
                result = &type0;
            }
            break;

        case Expression::dotPlus:
        case Expression::dotMinus:
        case Expression::dotStar:
        case Expression::dotSlash:
        case Expression::dotMod:
        case Expression::dotHat:
        case Expression::dotAmp:
        case Expression::dotBar:
        case Expression::lShift:
        case Expression::rShift:
        case Expression::dotLShift:
        case Expression::dotRShift:
            result = &type0;
            break;

        case Expression::in:
            result = &typeFactory().primitiveT(MetaType::BOOLEAN);
            break;

        case Expression::less:
        case Expression::leq:
        case Expression::greater:
        case Expression::geq:
        case Expression::neq:
        case Expression::eqEq:
            if (ExpressionTypeFinder::isMappableContainer(type0) &&
                ExpressionTypeFinder::isElementOf(type1, type0)) {
                result = &ExpressionTypeFinder::collectionSubst(
                  type0, typeFactory().primitiveT(MetaType::BOOLEAN));
            } else if (ExpressionTypeFinder::isMappableContainer(type1) &&
                       ExpressionTypeFinder::isElementOf(type0, type1)) {
                result = &ExpressionTypeFinder::collectionSubst(
                  type1, typeFactory().primitiveT(MetaType::BOOLEAN));
            } else {
                result = &typeFactory().primitiveT(MetaType::BOOLEAN);
            }
            break;

        case Expression::dotLess:
        case Expression::dotLeq:
        case Expression::dotGreater:
        case Expression::dotGeq:
        case Expression::dotNeq:
        case Expression::dotEqEq:
            result = &ExpressionTypeFinder::collectionSubst(
              type0, typeFactory().primitiveT(MetaType::BOOLEAN));
            break;

        case Expression::eq:
        case Expression::plusEq:
        case Expression::minusEq:
        case Expression::starEq:
        case Expression::slashEq:
        case Expression::modEq:
        case Expression::hatEq:
        case Expression::ampEq:
        case Expression::barEq:
        case Expression::lShiftEq:
        case Expression::rShiftEq:
            result = &typeFactory().voidT();
            break;

        default:
            assert(false);
    }

    setValueExprType(ast, *result);
}

void TypeAnalyzer::visitSetLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), debugFlag());
    AstVisitorPushPop::visitSetLiteral(ast);
    RootTyp const& unknown = TypeFactory::instance().unknownT();
    const RootTyp* setType = &unknown;

    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        const RootTyp& et = astSymbol(AST::setLiteral::expr(ast, i)).type();
        if (et.getMetaType() != MetaType::NUL) {
            setType = &et;
            break;
        }
    }

    setValueExprType(ast, TypeFactory::instance().setT(*setType));
}

void TypeAnalyzer::visitOptionalType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceExpressionTypeFinder);
    AstNode& child = AST::optionalType::type(ast);
    visit(child);
    RootTyp const& childType = astSymbol(child).type();
    if (childType.isOptional()) {
        SysOut::errorln(SPL_FRONT_END_NESTED_OPTIONAL_NOT_ALLOWED,
                        SourceLocationFactory::instance().create(_pCtx, child));
    }
}

void TypeAnalyzer::visitAnnotationT(AstNode& ast)
{
    // We need to do analysis on the ast fragments we have collected
    SymbolTableEntry& sym = astSymbol(ast);
    if (sym.is<SimpleAnnotationSym>()) {
        SimpleAnnotationSym& simpleSym = sym.as<SimpleAnnotationSym>();
        const std::vector<std::pair<AstNode*, AstNode*> >& kvPairs = simpleSym.keyValuePairs();
        for (std::vector<std::pair<AstNode*, AstNode*> >::const_iterator it = kvPairs.begin();
             it != kvPairs.end(); ++it) {
            const std::pair<AstNode*, AstNode*> p = *it;
            AstNode& key = *p.first;
            AstNode& value = *p.second;
            visit(key);
            visit(value);
        }
    } else if (sym.is<EventTimeAnnotationSym>()) {
        EventTimeAnnotationSym& etSym = sym.as<EventTimeAnnotationSym>();
        AstNode& attribute = etSym.eventTimeAttribute();
        visit(attribute);
        AstNode* resolution = etSym.resolution();
        if (resolution) {
            visit(*resolution);
        }
        AstNode* minimumGap = etSym.minimumGap();
        if (minimumGap) {
            visit(*minimumGap);
        }
        AstNode* lag = etSym.lag();
        if (lag) {
            visit(*lag);
        }
    } else if (sym.is<ViewAnnotationSym>()) {
        ViewAnnotationSym& vSym = sym.as<ViewAnnotationSym>();
        AstNode& name = vSym.viewName();
        visit(name);
        AstNode& port = vSym.port();
        visit(port);
        AstNode& sampleSize = vSym.sampleSize();
        visit(sampleSize);
        AstNode* bufferSize = vSym.bufferSize();
        if (NULL != bufferSize) {
            visit(*bufferSize);
        }
        AstNode* bufferTime = vSym.bufferTime();
        if (NULL != bufferTime) {
            visit(*bufferTime);
        }
        AstNode* description = vSym.description();
        if (NULL != description) {
            visit(*description);
        }
        AstNode* attributes = vSym.attributes();
        if (NULL != attributes) {
            visit(*attributes);
        }
        AstNode* filter = vSym.filter();
        if (NULL != filter) {
            visit(*filter);
        }
        AstNode* activateOption = vSym.activateOption();
        if (NULL != activateOption) {
            visit(*activateOption);
        }
    } else if (sym.is<ParallelAnnotationSym>()) {
        ParallelAnnotationSym& pSym = sym.as<ParallelAnnotationSym>();
        AstNode& width = pSym.width();
        visit(width);
        AstNode* hostTags = pSym.hostTags();
        if (NULL != hostTags) {
            visit(*hostTags);
        }
        const ParallelAnnotationSym::PartitionList& partitions = pSym.partitions();
        for (ParallelAnnotationSym::PartitionList::const_iterator it = partitions.begin();
             it != partitions.end(); ++it) {
            const ParallelAnnotationSym::Partition& partition = *it;
            visit(partition.port());
            AstNode& attributes = partition.attributes();
            if (AST::listLiteral::is(attributes)) {
                for (int32_t i = 0, iEnd = AST::listLiteral::exprCount(attributes); i < iEnd; ++i) {
                    AstNode& child = AST::listLiteral::expr(attributes, i);
                    visit(child);
                }
            }
        }
    } else if (sym.is<ThreadingAnnotationSym>()) {
        ThreadingAnnotationSym& pSym = sym.as<ThreadingAnnotationSym>();
        AstNode& model = pSym.model();
        visit(model);
        AstNode* threads = pSym.threads();
        if (NULL != threads) {
            visit(*threads);
        }
        AstNode* elastic = pSym.elastic();
        if (NULL != elastic) {
            visit(*elastic);
        }
    }
}

void TypeAnalyzer::visitUnwrapExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitUnwrapExpr(ast);
    AstNode& lhs = AST::unwrapExpr::lhs(ast);

    RootTyp const& lhsType = astSymbol(lhs).type();
    RootTyp const* result = &(TypeFactory::instance().unknownT());

    if (lhsType.isOptional()) {
        // use the type wrapped in the optional
        result = &(lhsType.as<OptionalTyp>().getUnderlyingType());

        // if we unwrapped an optional, then it's mutable, so we need our own symbol type for this
        setAstSymbol(
          AstPool::instance(), ast,
          *new UnwrapReferenceSym(newLoc(ast), &(_symTab.currentScope()), ast, lhs, *result));
    } else {
        // otherwise the mutability depends on whetever expression we passed through with identity
        // semantics
        setAstSymbol(
          AstPool::instance(), ast,
          *new UnwrapReferenceSym(newLoc(ast), &(_symTab.currentScope()), ast, lhs, lhsType));
    }
}

void TypeAnalyzer::visitIsPresentExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitIsPresentExpr(ast);
    RootTyp const& result = TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
    setValueExprType(ast, result);
}

void TypeAnalyzer::visitUnwrapOrElseExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceTypeAnalyzer);
    AstVisitorPushPop::visitUnwrapOrElseExpr(ast);
    AstNode& lhs = AST::unwrapOrElseExpr::lhs(ast);
    RootTyp const& lhsType = astSymbol(lhs).type();
    RootTyp const* result = NULL;

    if (lhsType.isOptional()) {
        // use the type wrapped in the optional
        result = &(lhsType.as<OptionalTyp>().getUnderlyingType());
    } else {
        // just use the type of whatever the expression itself is
        result = &lhsType;
    }
    setValueExprType(ast, *result);
}
