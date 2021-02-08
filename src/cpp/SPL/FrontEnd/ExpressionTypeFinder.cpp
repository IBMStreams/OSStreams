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

#include <SPL/FrontEnd/Aliasing.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/GenericsUnifier.h>
#include <SPL/FrontEnd/SideEffectDetector.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
//#include <SPL/FrontEnd/TypePromoter.h>
#include <SPL/Runtime/Type/String.h>
#include <SPL/Runtime/Type/Xml.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

#include <sstream>

using namespace std;

namespace SPL {
void ExpressionTypeFinder::run(ParserContext& pCtx, AstNode& ast)
{
    ExpressionTypeFinder visitor(pCtx);
    MODEL("PhaseStart(ExpressionTypeFinder, " << (void*)&ast << ")\n");
    visitor.visit(ast);
    MODEL("PhaseEnd(ExpressionTypeFinder, " << (void*)&ast << ")\n");
}

// ======================== private member functions ========================
ExpressionTypeFinder::ExpressionTypeFinder(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceExpressionTypeFinder)
  , _symTab(SPL::SymbolTable::instance())
  , _outputPort(-1)
  , _inLogicOnClause(false)
  , _inViewAttributeList(false)
  , _inViewFilter(false)
{}

void ExpressionTypeFinder::ensureResolved(AstNode& ast)
{
    SymbolTableEntry& sym = astSymbol(ast);
    if (symIsType(sym) || symIsValue(sym)) {
        astSymbol(ast).type();
    }
}

SourceLocation const& ExpressionTypeFinder::newLoc(AstNode& ast)
{
    return SourceLocationFactory::instance().create(_pCtx, ast);
}

void ExpressionTypeFinder::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast
                               << ") stage is" << astStage(ast).toString(),
           Debug::TraceExpressionTypeFinder);
    if (astStage(ast) < FrontEndStage::EXPRESSION_TYPE_FINDER) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::EXPRESSION_TYPE_FINDER);
        AstVisitorPushPop::visit(ast);
    }
}

// ======================== file-local helper functions ========================

/// Ensure that 'custom output functions', such as 'Sum' in the 'Aggregate' operator,
/// are only called at the top-level, not nested in other expressions.
static void checkCustomOutputFunctionsAreNotNested(ParserContext const& pCtx,
                                                   AstNode& ast,
                                                   unsigned port,
                                                   bool nested)
{
    if (nested && AST::callExpr::is(ast) &&
        !SymbolTable::instance().currentOpInvoke().nestedCOFsAllowed(port)) {
        SymbolTableEntry* c = astSymbolPtr(ast);
        if (!c) {
            return;
        }
        CallExpressionSym& call = c->as<CallExpressionSym>();
        FunctionHeadSym* tgt = call.resolvedTgt();
        if (NULL != tgt && tgt->isOutputFunction()) {
            SysOut::errorln(SPL_FRONT_END_CUSTOM_OUTPUT_FUNCTION_NESTED(tgt->name()),
                            call.sourceLocation());
        }
    }
    for (int i = 0, n = astSize(ast); i < n; i++) {
        AstNode& child = astChild(ast, i);
        if (AST::castExpr::is(ast)) {
            checkCustomOutputFunctionsAreNotNested(pCtx, child, port, false);
        }

        else {
            checkCustomOutputFunctionsAreNotNested(pCtx, child, port, true);
        }
    }
}

class ExpressionSideEffectDetector : public SideEffectDetector
{

    /// Report an error involving use/mod
    /// @param v1 name of variable that modifies data used by v2
    /// @param loc1 location of v1
    /// @param v2 name of other variable using/modifying data
    /// @param location of v1
    virtual void useModError(const std::string& v1,
                             const SourceLocation& loc1,
                             const std::string& v2,
                             const SourceLocation& loc2)
    {
        SysOut::errorln(SPL_FRONT_END_USE_MOD_CONFLICT(v1), loc1);
        SysOut::detailsErrorln(SPL_FRONT_END_USE_MOD_CONFLICT2(v2), loc2);
    }

    /// Report an error with multiple stateful calls
    /// @param fcn1 name of first state function
    /// @param fcn2 name of other stateful function
    /// @param loc1 location of v1
    virtual void multipleStatefulCall(const std::string& fcn1,
                                      const std::string& fcn2,
                                      const SourceLocation& loc1)
    {
        SysOut::errorln(SPL_FRONT_END_TWO_STATEFUL_CALLS(fcn1, fcn2), loc1);
    }
};

static void checkStreamsStillVisibleAfterExpansion(ParserContext const& pCtx, AstNode& ast)
{
    if (AST::attributeExpr::is(ast)) {
        AstNode& baseAst = AST::attributeExpr::base(ast);
        if (AST::idT::is(baseAst)) {
            SymbolTableEntry* baseSym = astSymbolPtr(AST::attributeExpr::base(ast));
            if (NULL != baseSym && symIsStream(*baseSym) && !symIsValue(*baseSym)) {
                SourceLocation const& loc = SourceLocationFactory::instance().create(pCtx, baseAst);
                resetAstSymbol(AstPool::instance(), baseAst,
                               *new ErrorDummySym(loc, baseAst, baseSym->name(),
                                                  &SymbolTable::instance().currentScope()));
                SysOut::errorln(SPL_FRONT_END_VALUE_NAME_EXPECTED(baseSym->name()), loc);
            }
        }
    }
    for (int i = 0, n = astSize(ast); i < n; i++) {
        checkStreamsStillVisibleAfterExpansion(pCtx, astChild(ast, i));
    }
}

static void checkExpression(ParserContext& pCtx, AstNode& ast)
{
    ExpressionSideEffectDetector sfxd;
    sfxd.findSideEffects(pCtx, ast);
    checkStreamsStillVisibleAfterExpansion(pCtx, ast);
}

RootTyp const& ExpressionTypeFinder::collectionSubst(RootTyp const& collType,
                                                     RootTyp const& elemType)
{
    TypeFactory& tf = TypeFactory::instance();
    switch (collType.getMetaType()) {
        case MetaType::LIST:
            return tf.listT(elemType);
        case MetaType::BLIST:
            return tf.listT(elemType, collType.as<BListTyp>().getBound());
        case MetaType::MAP:
            return tf.mapT(collType.as<MapTyp>().getKeyType(), elemType);
        case MetaType::BMAP: {
            BMapTyp const& t = collType.as<BMapTyp>();
            return tf.mapT(t.getKeyType(), elemType, t.getBound());
        }
        case MetaType::SET:
            return tf.setT(elemType);
        case MetaType::BSET:
            return tf.setT(elemType, collType.as<BSetTyp>().getBound());
        default:
            assert(false);
    }
    return TypeFactory::instance().unknownT();
}

RootTyp const& ExpressionTypeFinder::elementType(RootTyp const& type)
{
    TypeFactory& tf = TypeFactory::instance();
    switch (type.getMetaType()) {
        case MetaType::RSTRING:
        case MetaType::BSTRING:
        case MetaType::USTRING:
            return type;
        case MetaType::BLOB:
            return tf.primitiveT(MetaType::UINT8);
        case MetaType::XML:
            return tf.primitiveT(MetaType::XML);
        case MetaType::LIST:
            return type.as<ListTyp>().getElementType();
        case MetaType::BLIST:
            return type.as<BListTyp>().getElementType();
        case MetaType::MAP:
            return type.as<MapTyp>().getValueType();
        case MetaType::BMAP:
            return type.as<BMapTyp>().getValueType();
        case MetaType::SET:
            return type.as<SetTyp>().getElementType();
        case MetaType::BSET:
            return type.as<BSetTyp>().getElementType();
        case MetaType::OPTIONAL:
            return elementType(type.as<OptionalTyp>().getUnderlyingType());
        default:
            return TypeFactory::instance().unknownT();
            //				cerr << "Unforeseen type in elementType.  Bad type is " <<
            // type.getMetaType().toString(); 				assert(false);
    }
    return TypeFactory::instance().unknownT();
}

static void ensureEndsWithReturn(ParserContext const& pCtx, AstNode& ast)
{
    if (AST::returnStmt::is(ast)) {
        return;
    }
    if (AST::blockStmt::is(ast) && 0 < AST::blockStmt::stmtCount(ast)) {
        ensureEndsWithReturn(pCtx, AST::blockStmt::stmt(ast, AST::blockStmt::stmtCount(ast) - 1));
        return;
    }
    if (AST::blockStmtNoScope::is(ast) && 0 < AST::blockStmtNoScope::stmtCount(ast)) {
        ensureEndsWithReturn(
          pCtx, AST::blockStmtNoScope::stmt(ast, AST::blockStmtNoScope::stmtCount(ast) - 1));
        return;
    }
    if (AST::ifStmt::is(ast) && AST::ifStmt::has_elseClause(ast)) {
        ensureEndsWithReturn(pCtx, AST::ifStmt::stmt(ast));
        ensureEndsWithReturn(pCtx, AST::elseClause::stmt(AST::ifStmt::elseClause(ast)));
        return;
    }
    SysOut::errorln(SPL_FRONT_END_MISSING_RETURN,
                    SourceLocationFactory::instance().create(pCtx, ast));
}

bool ExpressionTypeFinder::explicitCastAllowed(RootTyp const& fromType, RootTyp const& toType)
{
    // We don't allow casting void to anything
    if (MetaType::VOID == fromType.getMetaType()) {
        return false;
    }

    // We can explicitly cast to the same type for any type
    if (fromType == toType) {
        return true;
    }

    // we can cast from any type T to optional<T>
    if (toType.isOptionalOfSameType(fromType)) {
        return true;
    }

    if (fromType.is<OptionalTyp>() && toType.is<OptionalTyp>()) {
        return explicitCastAllowed(fromType.as<OptionalTyp>().getUnderlyingType(),
                                   toType.as<OptionalTyp>().getUnderlyingType());
    }
    if (toType.is<OptionalTyp>()) {
        const OptionalTyp& oType = toType.as<OptionalTyp>();
        const RootTyp& uType = oType.getUnderlyingType();
        if (explicitCastAllowed(fromType, uType)) {
            return true;
        }
    }

    // we can cast null to any optional type
    if (fromType.getMetaType() == MetaType::NUL && toType.isOptional()) {
        return true;
    }

    // Any string type can be cast to any non-string type
    switch (fromType.getMetaType()) {
        case MetaType::UNKNOWN:
        case MetaType::RSTRING:
        case MetaType::BSTRING:
        case MetaType::USTRING:
            return true;
        default:
            break;
    }

    // conversly, we can cast any non-string type to a string
    switch (toType.getMetaType()) {
        case MetaType::UNKNOWN:
        case MetaType::RSTRING:
        case MetaType::BSTRING:
        case MetaType::USTRING:
            return true;
        default:
            break;
    }

    // Finally we have some additional casts that are allowed
    switch (fromType.getMetaType()) {
        case MetaType::BLOB:
            switch (toType.getMetaType()) {
                case MetaType::LIST:
                case MetaType::BLIST:
                    return MetaType::UINT8 == elementType(toType).getMetaType();
                default:
                    return false;
            }
        case MetaType::EMPTYCURLY:
            switch (toType.getMetaType()) {
                case MetaType::SET:
                case MetaType::BSET:
                case MetaType::MAP:
                case MetaType::BMAP:
                case MetaType::TUPLE:
                    return true;
                default:
                    return false;
            }
        case MetaType::ENUM:
            return satisfiesConstraint(toType, "integral");
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
            return (toType.is<EnumTyp>() || (satisfiesConstraint(toType, "numeric") &&
                                             !satisfiesConstraint(toType, "complex")));
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
            return (
              (satisfiesConstraint(toType, "numeric") && !satisfiesConstraint(toType, "complex")) ||
              ((fromType.getMetaType() == MetaType::FLOAT64) &&
               (toType.getMetaType() == MetaType::TIMESTAMP)));
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
            return satisfiesConstraint(toType, "complex");
        case MetaType::TIMESTAMP:
            return toType.getMetaType() == MetaType::FLOAT64;
        case MetaType::LIST:
        case MetaType::BLIST:
            switch (toType.getMetaType()) {
                case MetaType::LIST:
                case MetaType::BLIST: {
                    RootTyp const& fromElem = elementType(fromType);
                    RootTyp const& toElem = elementType(toType);

                    /*						if (fromElem.getMetaType() ==
                       MetaType::NUL) { return false; // we need to promote the actual null, not
                       just cast the whole literal
                                                                    }

                                                                    if(toElem.isOptional() &&
                       fromElem.getMetaType() == MetaType::UNKNOWN) { return false;
                                                                    }
                    */
                    if (unifies(fromElem, toElem)) {
                        return true;
                    }
                    if (satisfiesConstraint(fromElem, "primitive") &&
                        satisfiesConstraint(toElem, "primitive")) {
                        return explicitCastAllowed(fromElem, toElem);
                    }
                    return false;
                }
                case MetaType::BLOB: {
                    return MetaType::UINT8 == elementType(fromType).getMetaType() ||
                           MetaType::UNKNOWN == elementType(fromType).getMetaType();
                }
                case MetaType::COMPLEX32:
                case MetaType::COMPLEX64: {
                    RootTyp const& fromElem = elementType(fromType);
                    return satisfiesConstraint(fromElem, "numeric") &&
                           !satisfiesConstraint(fromElem, "complex");
                }
                default:
                    return false;
            }
        case MetaType::SET:
        case MetaType::BSET:
            if (satisfiesConstraint(toType, "set")) {
                RootTyp const& fromElem = elementType(fromType);
                RootTyp const& toElem = elementType(toType);

                /*					if (fromElem.getMetaType() == MetaType::NUL)
                   { return false; // we need to promote the actual null, not just cast the whole
                   literal
                                                        }

                                                        if (toElem.isOptional() &&
                   fromElem.getMetaType() == MetaType::UNKNOWN) { return false;
                                                        }
                */
                if (unifies(fromElem, toElem)) {
                    return true;
                }
                if (satisfiesConstraint(fromElem, "primitive") &&
                    satisfiesConstraint(toElem, "primitive")) {
                    return explicitCastAllowed(fromElem, toElem);
                }
            }
            return false;
        case MetaType::MAP:
        case MetaType::BMAP:
            if (satisfiesConstraint(toType, "map")) {
                bool fromIsBounded = fromType.is<BMapTyp>(), toIsBounded = toType.is<BMapTyp>();
                RootTyp const& fromKey = fromIsBounded ? fromType.as<BMapTyp>().getKeyType()
                                                       : fromType.as<MapTyp>().getKeyType();
                RootTyp const& fromValue = fromIsBounded ? fromType.as<BMapTyp>().getValueType()
                                                         : fromType.as<MapTyp>().getValueType();
                RootTyp const& toKey = toIsBounded ? toType.as<BMapTyp>().getKeyType()
                                                   : toType.as<MapTyp>().getKeyType();
                RootTyp const& toValue = toIsBounded ? toType.as<BMapTyp>().getValueType()
                                                     : toType.as<MapTyp>().getValueType();

                // if (fromKey.getMetaType() == MetaType::NUL || fromValue.getMetaType() ==
                // MetaType::NUL) { 	return false; // we need to promote the actual null, not
                // just
                // cast the whole literal
                //}

                if (unifies(fromKey, toKey) && unifies(fromValue, toValue)) {
                    return true;
                }
                if (satisfiesConstraint(fromKey, "primitive") &&
                    satisfiesConstraint(toKey, "primitive") &&
                    satisfiesConstraint(fromValue, "primitive") &&
                    satisfiesConstraint(toValue, "primitive")) {
                    return explicitCastAllowed(fromKey, toKey) &&
                           explicitCastAllowed(fromValue, toValue);
                }
            }
            return false;
        case MetaType::TUPLE:
            // We can cast from tuple to xml
            if (toType.is<XMLTyp>()) {
                return true;
            }
            if (toType.is<TupleTyp>()) {
                TupleTyp const& fromTuple = fromType.as<TupleTyp>();
                TupleTyp const& toTuple = toType.as<TupleTyp>();
                TupleTyp::Name2Index const& from_name2idx = fromTuple.getAttributeNames();
                for (int i = 0, n = toTuple.getNumberOfAttributes(); i < n; i++) {
                    std::string const& name = toTuple.getAttributeName(i);
                    TupleTyp::Name2Index::const_iterator it = from_name2idx.find(name);
                    if (it == from_name2idx.end()) {
                        return false;
                    }
                    RootTyp const& fromAttr = fromTuple.getAttributeType(name);
                    RootTyp const& toAttr = toTuple.getAttributeType(name);

                    /*if(fromAttr.getMetaType() == MetaType::NUL) {
                            return false; // we need to promote the actual null, not just cast the
                    whole literal
                    }*/

                    if (!unifies(fromAttr, toAttr)) {
                        return false;
                    }
                }
                return true;
            }
            return false;
        case MetaType::XML:
            // We can cast from xml to xml
            if (toType.is<XMLTyp>()) {
                return true;
            }
            // We can cast from xml to tuple.  The conversion can fail at runtime
            if (toType.is<TupleTyp>()) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

static std::string typeToString(RootTyp const& type)
{
    std::ostringstream os;
    os << type;
    return os.str();
}

static std::string removeNewLines(const std::string& st)
{
    size_t pos = st.find('\n');
    if (std::string::npos == pos) {
        return st;
    }

    std::string t;
    size_t start = 0;
    while (std::string::npos != pos) {
        t += st.substr(start, pos - start);
        start = pos + 1;
        pos = st.find('\n', start);
    }
    t += st.substr(start);
    return t;
}

static void validateXMLLiteral(const std::string& value,
                               const XMLTyp& type,
                               const SourceLocation& loc)
{
    try {
        // On entry the value has the content of the "literal", with quotes, but any type suffix
        // removed. That means attribute values, for example, potentially have escaped quotes.  We
        // need to convert the literal into its equivalent value in order to evaluate it.
        std::istringstream is(value);
        SPL::rstring v;
        is >> v;
        xml x(type.compileTimeSchema(), v.string());
    } catch (Distillery::DistilleryException const& e) {
        std::string v;
        if (value.size() > 25) {
            v = value.substr(1, 10);
            v += "...";
            v += value.substr(value.size() - 11, 10);
        } else {
            v = value.substr(1, value.size() - 2);
        }
        std::string exp(e.getExplanation());
        std::ostringstream os;
        os << type;
        // The message from the validator may have a prefix giving input file (which is bogus)
        // and coordinates which don't map.  Remove the prefix.
        // Prefix format is: '<input xml>:1:16: '.  If we see this prefix, remove it.
        size_t pos = exp.find("<input xml>:");
        if (std::string::npos != pos) {
            pos = exp.find(':', pos + 12);
            pos = exp.find(':', pos + 1);
            exp = exp.substr(pos + 2);
        }
        SysOut::errorln(SPL_FRONT_END_INVALID_XML_LITERAL(removeNewLines(v), os.str(), exp), loc);
    }
}

static bool getChildLiteralString(AstNode& ast, std::string& value, AstNode*& node)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    if (AST::stringLiteralT::is(ast) || AST::xmlLiteralT::is(ast)) {
        value = astText(ast);
        // Literals have an optional (for rstring) suffix.  Remove it.
        size_t len = value.size();
        if (!(value[len - 1] == '\"' || value[len - 1] == '\'')) {
            --len;
        }
        value = value.substr(0, len);
        node = &ast;
        return true;
    }
    if (AST::castExpr::is(ast)) {
        // Walk through an arbitrary number of casts
        return getChildLiteralString(AST::castExpr::expr(ast), value, node);
    }
    return false;
}

void ExpressionTypeFinder::visitXMLLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitXMLLiteral(ast);

    // Validate the xml literal for well-formedness
    std::string value = astText(ast);
    // Remove the trailing 'x'
    value = value.substr(0, value.size() - 1);
    assert(astSymbol(ast).type().is<XMLTyp>());
    validateXMLLiteral(value, astSymbol(ast).type().as<XMLTyp>(), newLoc(ast));
}

static void explicitCastErrorChecks(RootTyp const& toType, AstNode& ast, SourceLocation const& loc)
{
    AstNode& child = AST::castExpr::expr(ast);
    RootTyp const& fromType = astSymbol(child).type();
    if (!ExpressionTypeFinder::explicitCastAllowed(fromType, toType)) {
        SysOut::errorln(
          SPL_FRONT_END_TYPE_MISMATCH_CAST(typeToString(toType), typeToString(fromType)), loc);
    }
    // TODO: Move the following check to the diagnostician (at some point)
    // If this is a cast to xml, do a validation in the following cases:
    // - The child is a literal
    // - The child's type is not xml
    // - The cast-to type has a schema and it differs from the child's type
    if (toType.is<XMLTyp>() && toType != fromType && !toType.isOptionalOfSameType(fromType)) {
        std::string value;
        AstNode* node;
        if (getChildLiteralString(child, value, node)) {
            validateXMLLiteral(value, toType.as<XMLTyp>(), loc);
        }
    }
}

bool ExpressionTypeFinder::isElementOf(RootTyp const& elemType, RootTyp const& collType)
{
    return collType.is<CollectionTyp>() ? (elemType == ExpressionTypeFinder::elementType(collType))
                                        : false;
}

enum InfixExprExpected
{
    SAME,
    SAME_COLLECTION,
    T0_ELEMOF_T1,
    T1_ELEMOF_T0
};
enum InfixExprResult
{
    B,
    V,
    T0,
    T1,
    B0,
    B1,
    UNKNOWN
};

bool ExpressionTypeFinder::isMappableContainer(RootTyp const& type)
{
    switch (type.getMetaType()) {
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::MAP:
        case MetaType::BMAP:
            return true;
        default:
            return false;
    }
}

static bool isSubscriptable(RootTyp const& type, bool idxIsSlice)
{
    switch (type.getMetaType()) {
        case MetaType::RSTRING:
        case MetaType::BSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
        case MetaType::LIST:
        case MetaType::BLIST:
            return true;
        case MetaType::MAP:
        case MetaType::BMAP:
            return !idxIsSlice;
        default:
            return false;
    }
}

static RootTyp const& infixExprHelper(ParserContext const& pCtx,
                                      AstNode& ast,
                                      RootTyp const& type0,
                                      RootTyp const& type1,
                                      std::string const& constraint,
                                      bool useUnderlyingType,
                                      int n...)
{
    std::vector<InfixExprExpected> expected;
    va_list ap;
    va_start(ap, n);
    InfixExprResult result = UNKNOWN;
    RootTyp const* constrained = NULL;
    for (int i = 0; i < n; i++) {
        InfixExprExpected exp = (InfixExprExpected)va_arg(ap, int);
        expected.push_back(exp);
        InfixExprResult res = (InfixExprResult)va_arg(ap, int);
        switch (exp) {
            case SAME:
                if (type0 == type1 || type0.isOptionalOfSameType(type1) ||
                    type1.isOptionalOfSameType(type0)) {
                    result = res;
                    constrained = &type0;
                }
                break;
            case SAME_COLLECTION:
                if (type0 == type1 && ExpressionTypeFinder::isMappableContainer(type0)) {
                    result = res;
                    constrained = &ExpressionTypeFinder::elementType(type0);
                }
            case T0_ELEMOF_T1:
                if (ExpressionTypeFinder::isMappableContainer(type1) &&
                    ExpressionTypeFinder::isElementOf(type0, type1)) {
                    result = res;
                    constrained = &type0;
                }
                break;
            case T1_ELEMOF_T0:
                if (ExpressionTypeFinder::isMappableContainer(type0) &&
                    ExpressionTypeFinder::isElementOf(type1, type0)) {
                    result = res;
                    constrained = &type1;
                }
                break;
            default:
                assert(!"Unexpected case");
        }
    }
    va_end(ap);
    assert(1 <= expected.size() && expected.size() <= 4);
    if (result == UNKNOWN) {
        std::ostringstream os;
        for (int i = 0, N = expected.size(); i < N; i++) {
            if (0 < i) {
                os << SPL_FRONT_END_FRAGMENT_OR;
            }
            switch (expected.at(i)) {
                case SAME:
                    os << SPL_FRONT_END_FRAGMENT_SAME_TYPE;
                    break;
                case SAME_COLLECTION:
                    os << SPL_FRONT_END_FRAGMENT_SAME_COLLECTION;
                    break;
                case T0_ELEMOF_T1:
                    os << SPL_FRONT_END_FRAGMENT_ELEMENT_OF;
                    break;
                case T1_ELEMOF_T0:
                    os << SPL_FRONT_END_FRAGMENT_CONTAINER_FOR;
                    break;
            }
        }

        SPLDBG(__PRETTY_FUNCTION__ << ": unknown result", Debug::TraceExpressionTypeFinder);

        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_RELATIVE(
                          astText(ast), typeToString(type0), os.str(), typeToString(type1)),
                        SourceLocationFactory::instance().create(pCtx, AST::infixExpr::rhs(ast)));
    } else if (!satisfiesConstraint(*constrained, constraint, useUnderlyingType)) {
        std::string s;
        if (constraint == "addable") {
            s = SPL_FRONT_END_FRAGMENT_ADDABLE;
        } else if (constraint == "andorable") {
            s = SPL_FRONT_END_FRAGMENT_ANDORABLE;
        } else if (constraint == "boolean") {
            s = SPL_FRONT_END_FRAGMENT_BOOLEAN;
        } else if (constraint == "integral") {
            s = SPL_FRONT_END_FRAGMENT_INTEGRAL;
        } else if (constraint == "numeric") {
            s = SPL_FRONT_END_FRAGMENT_NUMERIC;
        } else if (constraint == "ordered") {
            s = SPL_FRONT_END_FRAGMENT_ORDERED;
        } else if (constraint == "subtractable") {
            s = SPL_FRONT_END_FRAGMENT_SUBTRACTABLE;
        } else if (constraint == "equatable") {
            s = SPL_FRONT_END_FRAGMENT_EQUATABLE;
        }

        SPLDBG(__PRETTY_FUNCTION__ << ": doesn't satisfy constraint",
               Debug::TraceExpressionTypeFinder);

        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_ABSOLUTE(
                          astText(ast), typeToString(type0), typeToString(type1), s),
                        SourceLocationFactory::instance().create(pCtx, AST::infixExpr::rhs(ast)));
        result = UNKNOWN;
    }
    switch (result) {
        case B:
            return TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
        case V:
            return TypeFactory::instance().voidT();
        case T0:
            return type0;
        case T1:
            return type1;
        case B0:
            return ExpressionTypeFinder::collectionSubst(
              type0, TypeFactory::instance().primitiveT(MetaType::BOOLEAN));
        case B1:
            return ExpressionTypeFinder::collectionSubst(
              type1, TypeFactory::instance().primitiveT(MetaType::BOOLEAN));
        case UNKNOWN:
            return TypeFactory::instance().unknownT();
        default:
            assert(!"Unexpected case");
    }
    SPLDBG(__PRETTY_FUNCTION__ << ": unknown type", Debug::TraceExpressionTypeFinder);
    return TypeFactory::instance().unknownT();
}

static void checkForFormalReferences(ParserContext const& pCtx,
                                     AstNode& ast,
                                     const SymbolTableEntry& formal)
{
    // If this node is an identifier, check to see if the name's first char is '$'
    if (AST::idT::is(ast)) {
        std::string name = astText(ast);
        if ('$' == name.at(0)) {
            SysOut::errorln(SPL_FRONT_END_COMPOSITE_FORMAL_REF(formal.name(), name),
                            SourceLocationFactory::instance().create(pCtx, ast));
        }
    } else {
        for (int i = 0, size = astSize(ast); i < size; ++i) {
            AstNode& child = astChild(ast, i);
            checkForFormalReferences(pCtx, child, formal);
        }
    }
}

static void opActualErrorChecks(ParserContext const& pCtx,
                                AstNode& ast,
                                std::string const& paramName,
                                SymbolTableEntry& formal)
{
    int expectedCardinality =
      formal.is<PrimitiveFormalSym>() ? formal.as<PrimitiveFormalSym>().cardinality() : 1;
    int actualCardinality = AST::opActualExprs::is(ast) ? AST::opActualExprs::exprCount(ast) : 1;
    if (-1 != expectedCardinality && expectedCardinality != actualCardinality) {
        SysOut::errorln(SPL_FRONT_END_OPERATOR_PARAMETER_CARDINALITY(paramName, expectedCardinality,
                                                                     actualCardinality),
                        SourceLocationFactory::instance().create(pCtx, ast));
    }
    RootTyp const& expectedType = formal.type();
    if (expectedType.getMetaType() != MetaType::UNKNOWN) {
        for (int i = 0; i < actualCardinality; i++) {
            AstNode& child = opActualChild(ast, i);
            SymbolTableEntry& actual = astSymbol(child);
            if (symIsValue(actual)) {
                RootTyp const& actualType = actual.type();
                if (!unifies(actualType, expectedType) &&
                    !expectedType.isOptionalOfSameType(actualType)) {
                    const SourceLocation& loc =
                      SourceLocationFactory::instance().create(pCtx, child);
                    SysOut::errorln(
                      SPL_FRONT_END_TYPE_MISMATCH_OPERATOR_PARAMETER(
                        paramName, typeToString(expectedType), typeToString(actualType)),
                      loc);
                }
            }
        }
    }
    if (formal.is<PrimitiveFormalSym>()) {
        // note: IRValidator.validateParameters() already checks the primitive expression-modes
        // (Attribute, AttributeFree, Constant, CustomLiteral, Expression, Nonexistent), so all we
        // need to check here is that it's a value.
        for (int i = 0; i < actualCardinality; i++) {
            AstNode& child = opActualChild(ast, i);
            SymbolTableEntry& actual = astSymbol(child);
            if (!symIsValue(actual)) {
                const SourceLocation& loc = SourceLocationFactory::instance().create(pCtx, child);
                SysOut::errorln(SPL_FRONT_END_PRIMITIVE_ACTUAL_MUST_DENOTE_VALUE, loc);
            }
        }
    } else if (formal.is<CompositeFormalSym>()) {
        for (int i = 0; i < actualCardinality; i++) {
            AstNode& child = opActualChild(ast, i);
            const SourceLocation& loc = SourceLocationFactory::instance().create(pCtx, child);
            SymbolTableEntry& actual = astSymbol(child);
            AstNode& modeAst =
              AST::expressionMode::name(AST::compositeFormal::expressionMode(formal.ast()));
            if (AST::attributeT::is(modeAst)) {
                bool err = false;
                if (actual.is<AttributeAccessSym>()) {
                    // If this is an attribute access sym we need to ensure that the base is a port
                    // sym
                    AstNode& base = AST::attributeExpr::base(child);
                    SymbolTableEntry& baseSym = astSymbol(base);
                    if (!symIsStream(baseSym)) {
                        err = true;
                    }
                } else if (!symIsAttribute(actual)) {
                    err = true;
                }
                if (err) {
                    SysOut::errorln(
                      SPL_FRONT_END_EXPRESSION_MODE_VIOLATION(formal.name(), "attribute"), loc);
                }
            } else if (AST::expressionT::is(modeAst)) {
                if (!symIsValue(actual)) {
                    SysOut::errorln(
                      SPL_FRONT_END_EXPRESSION_MODE_VIOLATION(formal.name(), "expression"), loc);
                }
            } else if (AST::functionT::is(modeAst)) {
                // no check here, function expression mode checking is delayed until the actual use,
                // because only then can the identifier be resolved to an actual function based on
                // the parameter types
            } else if (AST::operatorT::is(modeAst)) {
                if (!actual.is<OperatorSym>() && !actual.is<ErrorDummySym>()) {
                    SysOut::errorln(
                      SPL_FRONT_END_EXPRESSION_MODE_VIOLATION(formal.name(), "operator"), loc);
                }
            } else if (AST::typeT::is(modeAst)) {
                if (!symIsType(actual)) {
                    SysOut::errorln(SPL_FRONT_END_EXPRESSION_MODE_VIOLATION(formal.name(), "type"),
                                    loc);
                }
            }
            // ast is the initializers ast.  We need to walk it to see if there are any references
            // to other composite formal syms
            checkForFormalReferences(pCtx, child, formal);
        }
    }
}

static bool symCanExtendStaticDef(const SymbolTableEntry& sym)
{
    if (sym.is<CompositeInputPortSym>() || sym.is<StreamSym>() || sym.is<CompositeFormalSym>()) {
        return false;
    }

    if (sym.is<DefTypeSym>()) {
        const DefTypeSym& typeDef = sym.as<DefTypeSym>();
        if (typeDef.isStatic()) {
            return true;
        }
        if (typeDef.parentScope()) {
            SymbolTableEntry* holder = typeDef.parentScope()->holder();
            if (holder && holder->is<CompilationUnitSym>()) {
                return true;
            }
        }

        return false;
    }

    if (sym.is<VariableSym>() && !sym.as<VariableSym>().isStatic()) {
        return false;
    }

    return true;
}

static bool staticDefErrorChecks(ParserContext const& pCtx,
                                 AstNode& ast,
                                 std::string const& defName)
{
    bool result = false;
    if (AST::idT::is(ast)) {
        SymbolTableEntry* sym = astSymbolPtr(ast);
        if (NULL != sym && !symCanExtendStaticDef(*sym)) {
            SysOut::errorln(SPL_FRONT_END_NONSTATIC_DEFINING_STATIC(sym->name(), defName),
                            SourceLocationFactory::instance().create(pCtx, ast));
            result = true;
        }
    } else if (AST::callExpr::is(ast)) {
        FunctionHeadSym* callee = astSymbol(ast).as<CallExpressionSym>().resolvedTgt();
        if (NULL != callee && callee->isIntrinsic() &&
            callee->name() == "getThisCompositeInstanceName") {
            SysOut::errorln(SPL_FRONT_END_THIS_COMPOSITE_INSTANCE_STATIC,
                            astSymbol(ast).sourceLocation());
        }
    }
    for (int i = 0, n = astSize(ast); i < n; i++) {
        result |= staticDefErrorChecks(pCtx, astChild(ast, i), defName);
    }
    return result;
}

// ======================== overridden visitor methods ========================

// ----- The compilation unit is the start symbol of the SPL grammar.
void ExpressionTypeFinder::visitNamespaceName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    // skip
}

// ----- Composite operators are defined at the top-level in a namespace.
void ExpressionTypeFinder::visitCompositeBody(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitCompositeBody(ast);
    if (AST::compositeBody::has_types(ast)) {
        AstNode& clauseAst = AST::compositeBody::types(ast);
        for (int i = 0, n = AST::typeClause::typeDefCount(clauseAst); i < n; i++) {
            AstNode& typeDefAst = AST::typeClause::typeDef(clauseAst, i);
            DefTypeSym& typeDefSym = astSymbol(typeDefAst).as<DefTypeSym>();
            if (typeDefSym.isStatic()) {
                staticDefErrorChecks(_pCtx, AST::typeDef::typeDefTail(typeDefAst),
                                     typeDefSym.name());
            }
        }
    }
    if (AST::compositeBody::has_config(ast)) {
        AstNode& clauseAst = AST::compositeBody::config(ast);
        for (int iItem = 0, nItems = AST::configClause::configCount(clauseAst); iItem < nItems;
             iItem++) {
            AstNode& configItemAst = AST::configClause::config(clauseAst, iItem);
            std::string name = astText(AST::config::id(configItemAst));
            for (int iExpr = 0, nExprs = AST::config::exprCount(configItemAst); iExpr < nExprs;
                 iExpr++) {
                staticDefErrorChecks(_pCtx, AST::config::expr(configItemAst, iExpr), name);
            }
        }
    }
}

void ExpressionTypeFinder::visitCompositeFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitCompositeFormal(ast);
    SymbolTableEntry& formal = astSymbol(ast);
    AstNode& initAst = AST::compositeFormal::compositeFormalInit(ast);
    if (AST::compositeFormalInit::has_opActual(initAst)) {
        AstNode& actualsAst = AST::compositeFormalInit::opActual(initAst);
        opActualErrorChecks(_pCtx, actualsAst, formal.name(), formal);
    }
}

void ExpressionTypeFinder::visitOpActualExprs(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitOpActualExprs(ast);
    checkExpression(_pCtx, ast);
}

void ExpressionTypeFinder::visitOpActualName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstNode& nameAst = AST::opActualName::name(ast);
    if (!AST::idT::is(nameAst)) {
        ensureResolved(nameAst);
    }
    for (Scope* s = &_symTab.currentScope(); NULL != s; s = s->parentScope()) {
        SymbolTableEntry const* sym = s->holder();
        if (NULL != sym && sym->is<OpInvokeSym>()) {
            OperatorSym* op = sym->as<OpInvokeSym>().op();
            if (NULL != op && op->is<PrimitiveOperatorSym>()) {
                checkExpression(_pCtx, ast);
            }
            break;
        }
    }
}

void ExpressionTypeFinder::visitConfigItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitConfigItem(ast);
    std::string label = astText(AST::config::id(ast));
    if (label == "defaultPoolSize") {
        _symTab.configScope("defaultPoolSize");
        if (1 == AST::config::exprCount(ast)) {
            AstNode& sizeAst = AST::config::expr(ast, 0);
            if (!satisfiesConstraint(astSymbol(sizeAst).type(), "integral")) {
                SysOut::errorln(SPL_FRONT_END_CONFIG_DEFAULTPOOL_SIZE, newLoc(sizeAst));
            }
        } else {
            SysOut::errorln(SPL_FRONT_END_CONFIG_DEFAULTPOOL_ARITY, newLoc(ast));
        }
    } else if (label == "threadedPort") {
        OpInvokeSym const* oi = NULL;
        for (Scope* s = &_symTab.currentScope(); NULL != s; s = s->parentScope()) {
            SymbolTableEntry const* sym = s->holder();
            if (NULL != sym && sym->is<OpInvokeSym>()) {
                oi = &sym->as<OpInvokeSym>();
                break;
            }
        }
        OperatorSym* op = (NULL == oi) ? NULL : oi->op();
        if (NULL == oi || (NULL != op && op->is<CompositeDefSym>())) {
            SysOut::errorln(SPL_FRONT_END_THREADED_PORT_NOT_PRIMITIVE, newLoc(ast));
        } else {
            for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
                AstNode& exprAst = AST::config::expr(ast, i);
                if (AST::callExpr::is(exprAst) && 0 < AST::callExpr::actualCount(exprAst)) {
                    AstNode& targetAst = AST::callExpr::target(exprAst);
                    AstNode& actualAst = AST::callExpr::actual(exprAst, 0);
                    if (AST::idT::is(targetAst) && AST::idT::is(actualAst) &&
                        astText(targetAst) == "queue") {
                        bool isInput = false;
                        int found = findStreamInOpInvoke(astText(actualAst), oi->ast(), NULL,
                                                         &isInput, NULL, NULL);
                        if (0 == found || !isInput) {
                            SysOut::errorln(
                              SPL_FRONT_END_THREADED_PORT_BAD_QUEUE(astText(actualAst)),
                              astSymbol(actualAst).sourceLocation());
                        }
                    }
                }
            }
        }
    }
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        checkExpression(_pCtx, AST::config::expr(ast, i));
    }
}

// ----- Streams are defined in a composite operator's graph clause.
void ExpressionTypeFinder::visitOpInvoke(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitOpInvoke(ast);
    OpInvokeSym& sym = astSymbol(ast).as<OpInvokeSym>();
    if (NULL != sym.op() && sym.op()->is<CompositeDefSym>()) {
        AstNode& bodyAst = AST::opInvoke::body(ast);
        if (AST::opInvokeBody::has_logic(bodyAst)) {
            SysOut::errorln(SPL_FRONT_END_UNEXPECTED_COMPOSITE_CLAUSE("logic"),
                            newLoc(AST::opInvokeBody::logic(bodyAst)));
        }
        if (AST::opInvokeBody::has_window(bodyAst)) {
            SysOut::errorln(SPL_FRONT_END_UNEXPECTED_COMPOSITE_CLAUSE("window"),
                            newLoc(AST::opInvokeBody::window(bodyAst)));
        }
        if (AST::opInvokeBody::has_output(bodyAst)) {
            SysOut::errorln(SPL_FRONT_END_UNEXPECTED_COMPOSITE_CLAUSE("output"),
                            newLoc(AST::opInvokeBody::output(bodyAst)));
        }
    }
}

void ExpressionTypeFinder::visitOpInvokeHead(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitOpInvokeHead(ast);
    SymbolTableEntry& op = astSymbol(AST::opInvokeHead::opName(ast));
    {
        AstNode& opInputsAst = AST::opInvokeHead::inputs(ast);
        for (int iPort = 0, nPorts = AST::opInputs::portInputsCount(opInputsAst); iPort < nPorts;
             iPort++) {
            AstNode& portInputsAst = AST::opInputs::portInputs(opInputsAst, iPort);
            SymbolTableEntry* firstStream = NULL;
            for (int iChild = 0, nChildren = AST::portInputs::idCount(portInputsAst);
                 iChild < nChildren; iChild++) {
                AstNode& childAst = AST::portInputs::id(portInputsAst, iChild);
                SymbolTableEntry& child = astSymbol(childAst);
                bool isStream = child.is<CompositeInputPortSym>() || child.is<PortAliasSym>() ||
                                child.is<StreamSym>();
                if (isStream) {
                    if (NULL == firstStream) {
                        firstStream = &child;
                        if (AST::portInputs::has_type(portInputsAst)) {
                            RootTyp const& expectedType =
                              astSymbol(
                                AST::streamType::tupleBody(AST::portInputs::type(portInputsAst)))
                                .type();
                            if (!unifies(child.type(), expectedType)) {
                                SysOut::errorln(SPL_FRONT_END_MISMATCHED_TYPES_ON_PORT(
                                                  child.name(), typeToString(child.type()),
                                                  typeToString(expectedType)),
                                                newLoc(childAst));
                            }
                        }
                    } else if (!unifies(child.type(), firstStream->type())) {
                        SysOut::errorln(SPL_FRONT_END_HETEROGENEOUS_TYPES_ON_PORT(
                                          child.name(), firstStream->name()),
                                        newLoc(childAst));
                    }
                }
            }
        }
    }
    if (op.is<CompositeDefSym>()) {
        AstNode& compositeHead = AST::compositeDef::compositeHead(op.ast());
        int actualOutputCount = AST::opOutputs::outputCount(AST::opInvokeHead::outputs(ast));
        AstNode& compositeOut = AST::compositeHead::compositeOut(compositeHead);
        int expectedOutputCount =
          AST::noneT::is(compositeOut) ? 0 : AST::compositeOut::compositePortCount(compositeOut);
        if (expectedOutputCount != actualOutputCount) {
            SysOut::errorln(SPL_FRONT_END_INCORRECT_NUMBER_OF_OUTPUT_PORTS(
                              op.name(), expectedOutputCount, actualOutputCount),
                            newLoc(ast));
        }
        int actualInputCount = AST::opInputs::portInputsCount(AST::opInvokeHead::inputs(ast));
        AstNode& compositeIn = AST::compositeHead::compositeIn(compositeHead);
        int expectedInputCount =
          AST::noneT::is(compositeIn) ? 0 : AST::compositeIn::compositePortCount(compositeIn);
        if (expectedInputCount != actualInputCount) {
            SysOut::errorln(SPL_FRONT_END_INCORRECT_NUMBER_OF_INPUT_PORTS(
                              op.name(), expectedInputCount, actualInputCount),
                            newLoc(ast));
        }
    }
}

void ExpressionTypeFinder::visitOpInvokeWindow(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitOpInvokeWindow(ast);
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        checkExpression(_pCtx, AST::opInvokeWindow::expr(ast, i));
    }
    _symTab.pop(ast);
}

void ExpressionTypeFinder::visitOpInvokeActual(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitOpInvokeActual(ast);
    OpInvokeActualSym& actual = astSymbol(ast).as<OpInvokeActualSym>();
    SymbolTableEntry* formal = actual.formal();
    if (NULL != formal) {
        AstNode& valuesAst = AST::opInvokeActual::opActual(ast);
        opActualErrorChecks(_pCtx, valuesAst, actual.name(), *formal);
    }
}

void ExpressionTypeFinder::visitOutputClause(AstNode& ast)
{
    assert(-1 == _outputPort);
    _outputPort = 0;
    for (int n = astSize(ast); _outputPort < n; ++_outputPort) {
        visit(astChild(ast, _outputPort));
    }
    _outputPort = -1;
}

void ExpressionTypeFinder::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitOpInvokeOutput(ast);

    _symTab.pushOld(ast);
    OpInvokeOutputSym& sym = astSymbol(ast).as<OpInvokeOutputSym>();
    AstNode& labelAst = AST::opInvokeOutput::id(ast);
    RootTyp const& labelTyp = astSymbol(labelAst).type();
    std::tr1::unordered_set<std::string> attributeNames;
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        AstNode& lhsAst = AST::infixExpr::lhs(exprAst);
        RootTyp const& lhsTyp = astSymbol(lhsAst).type();
        AstNode& rhsAst = AST::infixExpr::rhs(exprAst);
        RootTyp const& rhsTyp = astSymbol(rhsAst).type();
        if (!astSymbol(lhsAst).is<ErrorDummySym>() && labelTyp.is<TupleTyp>()) {
            TupleTyp::Name2Index n2i = labelTyp.as<TupleTyp>().getAttributeNames();
            std::string attributeName = astText(lhsAst);
            if (n2i.end() == n2i.find(attributeName)) {
                SysOut::errorln(SPL_FRONT_END_NOT_AN_ATTRIBUTE_OF(attributeName, astText(labelAst)),
                                newLoc(lhsAst));
            }
            if (attributeNames.end() == attributeNames.find(attributeName)) {
                attributeNames.insert(attributeName);
            } else {
                SysOut::errorln(SPL_FRONT_END_DUPLICATE_OUTPUT_ASSIGNMENT(attributeName),
                                newLoc(lhsAst));
            }
        }
        if (!unifies(lhsTyp, rhsTyp)) {
            SysOut::errorln(
              SPL_FRONT_END_TYPE_MISMATCH_INFIX_RELATIVE(
                "=", typeToString(lhsTyp), SPL_FRONT_END_FRAGMENT_SAME_TYPE, typeToString(rhsTyp)),
              newLoc(rhsAst));
        }
        sym.switchRight();
        checkExpression(_pCtx, rhsAst);
        assert(-1 < _outputPort);
        checkCustomOutputFunctionsAreNotNested(_pCtx, rhsAst, _outputPort, false);
        sym.switchLeft();
    }
    _symTab.pop(ast);
}

// ----- Functions are defined at the top-level in a namespace.
void ExpressionTypeFinder::visitFunctionDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitFunctionDef(ast);
    FunctionHeadSym& fh = astSymbol(ast).as<FunctionHeadSym>();
    assert(!fh.isNativeFunction() && fh.isDef());
    if (fh.isStateful() && !fh.makesStatefulCall()) {
        SysOut::warnln(SPL_FRONT_END_FUNCTION_NEED_NOT_BE_STATEFUL(fh.name()), fh.sourceLocation());
    }
    RootTyp const& rt = fh.returnType();
    if (rt.getMetaType() != MetaType::UNKNOWN && rt.getMetaType() != MetaType::VOID) {
        ensureEndsWithReturn(_pCtx, AST::functionDef::body(ast));
    }
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void ExpressionTypeFinder::visitLocalDecl(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitLocalDecl(ast);

    AstNode& lhs = AST::localDecl::type(ast);
    RootTyp const& lhsType = astSymbol(lhs).type();
    for (int i = 0, n = AST::localDecl::localDeclItemCount(ast); i < n; i++) {
        AstNode& item = AST::localDecl::localDeclItem(ast, i);
        AstNode& rhs = AST::localDeclItem::expr_q(item);
        if (!AST::noneT::is(rhs)) {
            RootTyp const& rhsType = astSymbol(rhs).type();
            if (!explicitCastAllowed(rhsType, lhsType)) {
                SysOut::errorln(
                  SPL_FRONT_END_TYPE_MISMATCH_VARINIT(typeToString(rhsType), typeToString(lhsType)),
                  newLoc(item));
            }
        }
    }
}

void ExpressionTypeFinder::visitLocalDeclItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitLocalDeclItem(ast);
    checkExpression(_pCtx, ast);
}

void ExpressionTypeFinder::visitExprStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitExprStmt(ast);
    AstNode& exprAst = AST::exprStmt::expr(ast);
    if (AST::infixExpr::is(exprAst) // allow top-level expressions of the form "x = f(x)"
        && (AST::ampEqT::is(exprAst) || AST::barEqT::is(exprAst) || AST::eqT::is(exprAst) ||
            AST::hatEqT::is(exprAst) || AST::lShiftEqT::is(exprAst) || AST::minusEqT::is(exprAst) ||
            AST::modEqT::is(exprAst) || AST::plusEqT::is(exprAst) || AST::rShiftEqT::is(exprAst) ||
            AST::slashEqT::is(exprAst) || AST::starEqT::is(exprAst))) {
        checkExpression(_pCtx, AST::infixExpr::lhs(exprAst));
        checkExpression(_pCtx, AST::infixExpr::rhs(exprAst));
    } else {
        checkExpression(_pCtx, exprAst);
    }
}

void ExpressionTypeFinder::visitIfStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitIfStmt(ast);
    AstNode& condAst = AST::ifStmt::expr(ast);
    RootTyp const& condType = astSymbol(condAst).type();
    RootTyp const& boolType = TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
    if (!unifies(condType, boolType)) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COND_EXPR(typeToString(condType)),
                        newLoc(condAst));
    }
    checkExpression(_pCtx, AST::ifStmt::expr(ast));
}

void ExpressionTypeFinder::visitForStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    _loopNestingContext.push_back(&ast);
    AstVisitorPushPop::visitForStmt(ast);
    checkExpression(_pCtx, AST::forInit::expr(AST::forStmt::forInit(ast)));
    assert(&ast == _loopNestingContext.back());
    _loopNestingContext.pop_back();
    AstNode& forInitAst = AST::forStmt::forInit(ast);
    RootTyp const& itemType = astSymbol(AST::forInit::id(forInitAst)).type();
    RootTyp const& collType = astSymbol(AST::forInit::expr(forInitAst)).type();
    bool correct = true;
    switch (collType.getMetaType()) {
        case MetaType::UNKNOWN:
            correct = true;
            break;
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::SET:
        case MetaType::BSET:
            correct = isElementOf(itemType, collType);
            break;
        case MetaType::MAP: {
            correct = unifies(itemType, collType.as<MapTyp>().getKeyType());
            break;
        }
        case MetaType::BMAP: {
            correct = unifies(itemType, collType.as<BMapTyp>().getKeyType());
            break;
        }
        default:
            correct = false;
            break;
    }
    if (!correct) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_RELATIVE(
                          "in", typeToString(itemType), SPL_FRONT_END_FRAGMENT_ELEMENT_OF,
                          typeToString(collType)),
                        newLoc(forInitAst));
    }
}

void ExpressionTypeFinder::visitWhileStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    _loopNestingContext.push_back(&ast);
    AstVisitorPushPop::visitWhileStmt(ast);
    AstNode& condAst = AST::whileStmt::expr(ast);
    RootTyp const& condType = astSymbol(condAst).type();
    RootTyp const& boolType = TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
    if (!unifies(condType, boolType)) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COND_EXPR(typeToString(condType)),
                        newLoc(condAst));
    }
    checkExpression(_pCtx, condAst);
    assert(&ast == _loopNestingContext.back());
    _loopNestingContext.pop_back();
}

void ExpressionTypeFinder::visitBreakStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitBreakStmt(ast);
    if (_loopNestingContext.size() <= 0) {
        SysOut::errorln(SPL_FRONT_END_BREAK_OUTSIDE_LOOP, newLoc(ast));
    }
}

void ExpressionTypeFinder::visitContinueStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitContinueStmt(ast);
    if (_loopNestingContext.size() <= 0) {
        SysOut::errorln(SPL_FRONT_END_CONTINUE_OUTSIDE_LOOP, newLoc(ast));
    }
}

void ExpressionTypeFinder::visitReturnStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitReturnStmt(ast);

    FunctionHeadSym const* fun = SymbolTable::instance().function();
    if (_inLogicOnClause) {
        // This is legal without a return type
        if (AST::returnStmt::has_expr(ast)) {
            SysOut::errorln(SPL_FRONT_END_VOID_VALUE_RETURN_LOGIC_CLAUSE, newLoc(ast));
        }
    } else if (NULL == fun) {
        SysOut::errorln(SPL_FRONT_END_RETURN_OUTSIDE_FUNCTION, newLoc(ast));
    } else {
        RootTyp const& expectedType = fun->returnType();
        if (MetaType::VOID == expectedType.getMetaType()) {
            if (AST::returnStmt::has_expr(ast)) {
                SysOut::errorln(SPL_FRONT_END_VOID_VALUE_RETURN, newLoc(ast));
            }
        } else {
            RootTyp const& actualType = AST::returnStmt::has_expr(ast)
                                          ? astSymbol(AST::returnStmt::expr(ast)).type()
                                          : TypeFactory::instance().voidT();
            if (!unifies(expectedType, actualType)) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_RETURN(typeToString(actualType),
                                                                   fun->name(),
                                                                   typeToString(expectedType)),
                                newLoc(ast));
            }
        }
    }
    checkExpression(_pCtx, ast);
}

// ----- Expressions can appear in many places in the grammar.
void ExpressionTypeFinder::visitAttributeExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitAttributeExpr(ast);
    SymbolTableEntry& attribSym =
      astSymbol(ast).as<AttributeAccessSym>().attrib(); // side effect: ensures initialization
    AstNode& baseAst = AST::attributeExpr::base(ast);
    if (AST::idT::is(baseAst)) {
        SymbolTableEntry& baseSym = astSymbol(baseAst);
        bool isEnum =
          (baseSym.is<DefTypeSym>() && baseSym.type().getMetaType().getValue() == MetaType::ENUM);
        if (!(isEnum || (baseSym.is<CompositeDefSym>() || symIsValue(baseSym)))) {
            SysOut::errorln(SPL_FRONT_END_VALUE_NAME_EXPECTED(baseSym.name()), newLoc(baseAst));
        }
    }
    if (!symIsValue(attribSym)) {
        SysOut::errorln(SPL_FRONT_END_VALUE_NAME_EXPECTED(attribSym.name()), newLoc(ast));
    }
}

void ExpressionTypeFinder::visitCastExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitCastExpr(ast);
    RootTyp const& toType = astSymbol(AST::castExpr::type(ast)).type();
    explicitCastErrorChecks(toType, ast, astSymbol(ast).sourceLocation());
}

void ExpressionTypeFinder::visitCallExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    assert(AST::callExpr::is(ast));

    // Visit each argument
    for (int j = 0, cnt = AST::callExpr::actualCount(ast); j < cnt; ++j) {
        visit(AST::callExpr::actual(ast, j));
    }

    SymbolTableEntry& sym = astSymbol(ast);
    assert(sym.is<CallExpressionSym>());
    CallExpressionSym& call = sym.as<CallExpressionSym>();
    string functionName = call.name();

    SymbolTableEntry* tgt = call.resolvedTgt();
    if (NULL != tgt) {
        FunctionHeadSym& fun = tgt->as<FunctionHeadSym>();

        // loop through formal function parameters
        for (int i = 0, n = fun.formals().size(); i < n; i++) {
            FunctionFormalSym* formal = fun.formals().at(i);

            // mutability must agree between the parameter declaration and what we pass in
            // to the call
            if (NULL != formal && formal->isMutable()) {
                AstNode& actual = AST::callExpr::actual(ast, i);
                if (!symIsMutable(astSymbol(actual), _loopNestingContext)) {
                    std::ostringstream os;
                    if (formal->hasName()) {
                        os << formal->name();
                    } else {
                        os << i;
                    }
                    SysOut::errorln(SPL_FRONT_END_EXPECTED_MUTABLE_ACTUAL(os.str()),
                                    newLoc(actual));
                }
            }
        }
        if (fun.isStateful()) {
            FunctionHeadSym* caller = _symTab.function();
            if (NULL != caller) {
                caller->setMakesStatefulCall();
                if (!caller->isStateful()) {
                    SysOut::errorln(
                      SPL_FRONT_END_CALLER_MUST_BE_STATEFUL(caller->name(), fun.name()),
                      call.sourceLocation());
                }
            }
        }

        if (functionName == "submit" && call.actualTypes().size() == 2 &&
            call.actualTypes().at(1)->getMetaType() != MetaType::UINT32) {
            // check types of first and second params
            // node that we need to actual check the types of the symbols for the expressions
            // because the actualTypes() on the sym don't get updated after promotion, only the
            // symbols for the expressions do
            if (astSymbol(AST::callExpr::actual(ast, 0)).type().getMetaType() == MetaType::TUPLE &&
                astSymbol(AST::callExpr::actual(ast, 0)).type() !=
                  astSymbol(AST::callExpr::actual(ast, 1)).type()) {
                SysOut::errorln(
                  SPL_FRONT_END_PORT_NAME_TYPE_MISMATCH(call.actualTypes().at(0)->getName(),
                                                        call.actualTypes().at(1)->getName()),
                  sym.sourceLocation());
            }
        }
    }
}

void ExpressionTypeFinder::visitConditionalExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitConditionalExpr(ast);

    TypeFactory& tf = TypeFactory::instance();
    RootTyp const& condType = astSymbol(AST::conditionalExpr::condExpr(ast)).type();
    RootTyp const& thenType = astSymbol(AST::conditionalExpr::thenExpr(ast)).type();
    RootTyp const& elseType = astSymbol(AST::conditionalExpr::elseExpr(ast)).type();
    RootTyp const& unknown = TypeFactory::instance().unknownT();
    RootTyp const& boolType = tf.primitiveT(MetaType::BOOLEAN);

    if (condType != unknown && condType != boolType) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COND_EXPR(typeToString(condType)), newLoc(ast));
    }
    if (thenType != unknown && elseType != unknown && thenType != elseType) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COND_EXPR_ARGS(typeToString(thenType),
                                                                   typeToString(elseType)),
                        newLoc(ast));
    }
}

void ExpressionTypeFinder::visitInfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitInfixExpr(ast);

    AstNode& lhs = AST::infixExpr::lhs(ast);
    AstNode& rhs = AST::infixExpr::rhs(ast);
    RootTyp const* type0 = &astSymbol(lhs).type();
    RootTyp const* type1 = &astSymbol(rhs).type();
    RootTyp const& unknown = TypeFactory::instance().unknownT();
    if ((*type0) == unknown || (*type1) == unknown) {
        return;
    }
    RootTyp const& nullTyp = TypeFactory::instance().nullT();
    if ((*type0) == nullTyp && (*type1) == nullTyp) {
        SysOut::errorln(SPL_FRONT_END_INFIX_OPERANDS_BOTH_NULL, newLoc(ast));
        return;
    }

    RootTyp const* result = &TypeFactory::instance().unknownT();
    Expression::Op op = astNodeOp(ast);

    // now check legality of the expression
    switch (op) {
        case Expression::plus:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "addable", false, 3, SAME, T0,
                                      T0_ELEMOF_T1, T1, T1_ELEMOF_T0, T0);
            break;
        case Expression::minus:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "subtractable", false, 3, SAME,
                                      T0, T0_ELEMOF_T1, T1, T1_ELEMOF_T0, T0);
            break;
        case Expression::star:
        case Expression::slash:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "numeric", false, 3, SAME, T0,
                                      T0_ELEMOF_T1, T1, T1_ELEMOF_T0, T0);
            break;
        case Expression::mod:
        case Expression::hat:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "integral", false, 3, SAME, T0,
                                      T0_ELEMOF_T1, T1, T1_ELEMOF_T0, T0);
            break;
        case Expression::amp:
        case Expression::bar:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "andorable", false, 3, SAME, T0,
                                      T0_ELEMOF_T1, T1, T1_ELEMOF_T0, T0);
            break;
        case Expression::ampAmp:
        case Expression::barBar:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "boolean", false, 3, SAME, T0,
                                      T0_ELEMOF_T1, T1, T1_ELEMOF_T0, T0);
            break;
        case Expression::less:
        case Expression::leq:
        case Expression::greater:
        case Expression::geq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "ordered", false, 3, SAME, B,
                                      T0_ELEMOF_T1, B1, T1_ELEMOF_T0, B0);
            break;
        case Expression::neq:
        case Expression::eqEq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "equatable", false, 3, SAME, B,
                                      T0_ELEMOF_T1, B1, T1_ELEMOF_T0, B0);
            break;
        case Expression::lShift:
        case Expression::rShift: {
            bool correct = true;
            bool isCollection0 = type0->is<CollectionTyp>();
            bool isCollection1 = type1->is<CollectionTyp>();
            if (isCollection0) {
                correct = correct && satisfiesConstraint(elementType(*type0), "integral", false);
                correct = correct && satisfiesConstraint(*type1, "integral", false);
                result = correct ? type0 : &unknown;
            } else if (isCollection1) {
                correct = correct && satisfiesConstraint(*type0, "integral", false);
                correct = correct && satisfiesConstraint(elementType(*type1), "integral", false);
                result = correct ? &collectionSubst(*type1, *type0) : &unknown;
            } else {
                correct = correct && satisfiesConstraint(*type0, "integral", false);
                correct = correct && satisfiesConstraint(*type1, "integral", false);
                result = correct ? type0 : &unknown;
            }
            if (!correct) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_SHIFT(
                                  typeToString(*type0), astText(ast), typeToString(*type1)),
                                newLoc(AST::infixExpr::rhs(ast)));
            }
            break;
        }
        case Expression::in: {
            bool correct = true;
            switch (type1->getMetaType()) {
                case MetaType::UNKNOWN:
                    correct = true;
                    break;
                case MetaType::RSTRING:
                case MetaType::BSTRING:
                case MetaType::USTRING:
                case MetaType::BLOB:
                case MetaType::LIST:
                case MetaType::BLIST:
                case MetaType::SET:
                case MetaType::BSET:
                    correct = isElementOf(*type0, *type1);
                    break;
                case MetaType::MAP:
                    correct = unifies(*type0, type1->as<MapTyp>().getKeyType());
                    break;
                case MetaType::BMAP:
                    correct = unifies(*type0, type1->as<BMapTyp>().getKeyType());
                    break;
                default:
                    correct = false;
                    break;
            }
            if (!correct) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_RELATIVE(
                                  "in", typeToString(*type0), SPL_FRONT_END_FRAGMENT_ELEMENT_OF,
                                  typeToString(*type1)),
                                newLoc(AST::infixExpr::rhs(ast)));
            }
            result = &TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
            break;
            break;
        }
        case Expression::eq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "any", true, 1, SAME, V);
            break;
        case Expression::plusEq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "addable", false, 2, SAME, V,
                                      T1_ELEMOF_T0, V);
            break;
        case Expression::minusEq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "subtractable", false, 2, SAME, V,
                                      T1_ELEMOF_T0, V);
            break;
        case Expression::starEq:
        case Expression::slashEq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "numeric", false, 2, SAME, V,
                                      T1_ELEMOF_T0, V);
            break;
        case Expression::modEq:
        case Expression::hatEq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "integral", false, 2, SAME, V,
                                      T1_ELEMOF_T0, V);
            break;
        case Expression::ampEq:
        case Expression::barEq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "andorable", false, 2, SAME, V,
                                      T1_ELEMOF_T0, V);
            break;
        case Expression::lShiftEq:
        case Expression::rShiftEq: {
            bool correct = true;
            if (type0->is<CollectionTyp>()) {
                correct = correct && satisfiesConstraint(elementType(*type0), "integral", false);
            } else {
                correct = correct && satisfiesConstraint(*type0, "integral", false);
            }
            correct = correct && satisfiesConstraint(*type1, "integral", false);
            if (!correct) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_SHIFT(
                                  astText(ast), typeToString(*type0), typeToString(*type1)),
                                newLoc(AST::infixExpr::rhs(ast)));
            }
            result = &TypeFactory::instance().voidT();
            break;
        }
        case Expression::dotPlus:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "addable", false, 1,
                                      SAME_COLLECTION, T0);
            break;
        case Expression::dotMinus:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "subtractable", false, 1,
                                      SAME_COLLECTION, T0);
            break;
        case Expression::dotStar:
        case Expression::dotSlash:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "numeric", false, 1,
                                      SAME_COLLECTION, T0);
            break;
        case Expression::dotMod:
        case Expression::dotHat:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "integral", false, 1,
                                      SAME_COLLECTION, T0);
            break;
        case Expression::dotAmp:
        case Expression::dotBar:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "andorable", false, 1,
                                      SAME_COLLECTION, T0);
            break;
        case Expression::dotLess:
        case Expression::dotLeq:
        case Expression::dotGreater:
        case Expression::dotGeq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "ordered", false, 1,
                                      SAME_COLLECTION, B0);
            break;
        case Expression::dotNeq:
        case Expression::dotEqEq:
            result = &infixExprHelper(_pCtx, ast, *type0, *type1, "equatable", false, 1,
                                      SAME_COLLECTION, B0);
            break;
        case Expression::dotLShift:
        case Expression::dotRShift: {
            bool correct = true;
            correct = correct && type0->getMetaType() == type1->getMetaType();
            correct = correct && type0->is<CollectionTyp>();
            correct = correct && satisfiesConstraint(elementType(*type0), "integral", false);
            correct = correct && satisfiesConstraint(elementType(*type1), "integral", false);
            if (type0->is<BListTyp>()) {
                correct =
                  correct && type0->as<BListTyp>().getBound() == type1->as<BListTyp>().getBound();
            }
            if (type0->is<BMapTyp>()) {
                BMapTyp const& sub0 = type0->as<BMapTyp>();
                BMapTyp const& sub1 = type1->as<BMapTyp>();
                correct = correct && sub0.getBound() == sub1.getBound();
                correct = correct && (sub0.getKeyType() == sub1.getKeyType() ||
                                      sub0.getKeyType() == unknown || sub1.getKeyType() == unknown);
            }
            if (!correct) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_SHIFT(
                                  astText(ast), typeToString(*type0), typeToString(*type1)),
                                newLoc(ast));
            }
            result = correct ? type0 : &unknown;
            break;
        }
        default:
            assert(!"should never reach this code");
    }
    switch (op) {
        case Expression::eq:
        case Expression::plusEq:
        case Expression::minusEq:
        case Expression::starEq:
        case Expression::slashEq:
        case Expression::modEq:
        case Expression::ampEq:
        case Expression::hatEq:
        case Expression::barEq:
        case Expression::lShiftEq:
        case Expression::rShiftEq:
            if (!symIsMutable(astSymbol(lhs), _loopNestingContext)) {
                if (AST::subscriptExpr::is(lhs)) {
                    std::ostringstream ss;
                    printExpr(ss, lhs);
                    AstNode& subscript = AST::subscriptExpr::subscript(lhs);
                    if (AST::subscriptSlice::is(subscript)) {
                        // assignment to slice
                        SysOut::errorln(SPL_FRONT_END_ASSIGN_TO_SLICE(ss.str()), newLoc(ast));
                        break;
                    } else {
                        AstNode& expr = AST::subscriptExpr::expr(lhs);
                        MetaType mt = astSymbol(expr).type().getMetaType();
                        if (mt == MetaType::RSTRING || mt == MetaType::BSTRING ||
                            mt == MetaType::USTRING) {
                            // assignment to string subscript
                            SysOut::errorln(SPL_FRONT_END_ASSIGN_TO_STRING_SUBSCRIPT(ss.str()),
                                            newLoc(ast));
                            break;
                        }
                    }
                }
                // Generic error
                SysOut::errorln(SPL_FRONT_END_EXPECTED_MUTABLE_OPERAND(astText(ast)), newLoc(ast));
            }
            break;
        default:
            break;
    }
    assert(NULL != result);
    astSymbol(ast).setType(*result);
}

void ExpressionTypeFinder::visitPostfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitPostfixExpr(ast);
    RootTyp const& type = astSymbol(AST::postfixExpr::expr(ast)).type();
    if (type != TypeFactory::instance().unknownT() && !satisfiesConstraint(type, "numeric")) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_UNARY(
                          typeToString(type), astText(AST::postfixExpr::postfixOp(ast))),
                        newLoc(ast));
    }
    if (!symIsMutable(astSymbol(AST::postfixExpr::expr(ast)), _loopNestingContext)) {
        SysOut::errorln(
          SPL_FRONT_END_EXPECTED_MUTABLE_OPERAND(astText(AST::postfixExpr::postfixOp(ast))),
          newLoc(ast));
    }
}

void ExpressionTypeFinder::visitPrefixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitPrefixExpr(ast);
    RootTyp const& type = astSymbol(AST::prefixExpr::expr(ast)).type();
    bool isCorrectType = true;
    Expression::Op op = astNodeOp(AST::prefixExpr::prefixOp(ast));
    if (type != TypeFactory::instance().unknownT()) {
        switch (op) {
            case Expression::bang:
                isCorrectType = satisfiesConstraint(type, "boolean");
                break;
            case Expression::tilde:
                isCorrectType = satisfiesConstraint(type, "integral");
                break;
            case Expression::unaryMinus:
            case Expression::plusPlus:
            case Expression::minusMinus:
                isCorrectType = satisfiesConstraint(type, "numeric");
                break;
            default:
                assert(!"should never reach this code");
        }
    }
    if (!isCorrectType) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_UNARY(astText(AST::prefixExpr::prefixOp(ast)),
                                                          typeToString(type)),
                        newLoc(ast));
    }
    switch (op) {
        case Expression::plusPlus:
        case Expression::minusMinus:
            if (!symIsMutable(astSymbol(AST::prefixExpr::expr(ast)), _loopNestingContext)) {
                SysOut::errorln(
                  SPL_FRONT_END_EXPECTED_MUTABLE_OPERAND(astText(AST::prefixExpr::prefixOp(ast))),
                  newLoc(ast));
            }
            break;
        default:
            break;
    }
}

void ExpressionTypeFinder::visitSubscriptExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitSubscriptExpr(ast);
    AstNode& collAst = AST::subscriptExpr::expr(ast);
    SymbolTableEntry& collSym = astSymbol(collAst);
    RootTyp const& collType = collSym.type();
    if (collType == TypeFactory::instance().unknownT()) {
        return;
    }
    bool idxIsSlice = AST::subscriptSlice::is(AST::subscriptExpr::subscript(ast));
    if (symIsStream(collSym)) { // history access
        if (idxIsSlice) {
            SysOut::errorln(SPL_FRONT_END_SUBSCRIPT_HISTORY_SLICE(collSym.name()), newLoc(ast));
        } else {
            RootTyp const& subscriptType = astSymbol(AST::subscriptExpr::subscript(ast)).type();
            if (!satisfiesConstraint(subscriptType, "integral")) {
                SysOut::errorln(
                  SPL_FRONT_END_SUBSCRIPT_HISTORY_TYPE(collSym.name(), typeToString(subscriptType)),
                  newLoc(ast));
            }
        }
    } else {
        bool correct = isSubscriptable(collType, idxIsSlice);
        if (idxIsSlice) {
            AstNode& lowerAst = AST::subscriptSlice::lower(AST::subscriptExpr::subscript(ast));
            RootTyp const& lowerType = AST::noneT::is(lowerAst) ? TypeFactory::instance().unknownT()
                                                                : astSymbol(lowerAst).type();
            correct = correct && satisfiesConstraint(lowerType, "integral");
            AstNode& upperAst = AST::subscriptSlice::upper(AST::subscriptExpr::subscript(ast));
            RootTyp const& upperType = AST::noneT::is(upperAst) ? TypeFactory::instance().unknownT()
                                                                : astSymbol(upperAst).type();
            correct = correct && satisfiesConstraint(upperType, "integral");
            correct = correct && unifies(lowerType, upperType);
            if (!correct) {
                SysOut::errorln(SPL_FRONT_END_SUBSCRIPT_SLICE(typeToString(collType),
                                                              typeToString(lowerType),
                                                              typeToString(upperType)),
                                newLoc(ast));
            }
        } else {
            RootTyp const& idxType = astSymbol(AST::subscriptExpr::subscript(ast)).type();
            if (collType.is<MapTyp>()) {
                correct = correct && unifies(idxType, collType.as<MapTyp>().getKeyType());
            } else if (collType.is<BMapTyp>()) {
                correct = correct && unifies(idxType, collType.as<BMapTyp>().getKeyType());
            } else {
                correct = correct && satisfiesConstraint(idxType, "integral");
            }
            if (!correct) {
                SysOut::errorln(
                  SPL_FRONT_END_SUBSCRIPT_INDEX(typeToString(collType), typeToString(idxType)),
                  newLoc(ast));
            }
        }
        if (!correct) {
            astSymbol(ast).setType(TypeFactory::instance().unknownT());
        }
    }
}

// ----- Literals are the highest-precedence expressions denoting values.
void ExpressionTypeFinder::visitListLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": enter: visiting " << astText(ast), debugFlag());
    AstVisitorPushPop::visitListLiteral(ast);

    TypeFactory& tf = TypeFactory::instance();
    RootTyp const& unknown = tf.unknownT();
    RootTyp const* elemType = &unknown;
    bool correct = true;
    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        AstNode& element = AST::listLiteral::expr(ast, i);
        RootTyp const& et = astSymbol(element).type();
        if (_inViewAttributeList) {
            if (et.getMetaType() == MetaType::TUPLE) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("attributes", "view"),
                  newLoc(element));
            }
        } else {
            correct = correct && unifies(*elemType, et);
            if (&unknown == elemType) {
                elemType = &et;
            }
        }
    }
    if (!correct) {
        SysOut::errorln(SPL_FRONT_END_TYPE_HETEROGENEOUS_LITERAL("list"), newLoc(ast));
        elemType = &unknown;
    }

    // Type promotions have been done so we can now set the type correctly
    astSymbol(ast).setType(TypeFactory::instance().listT(*elemType));
    SPLDBG(__PRETTY_FUNCTION__ << ": exit: astType is " << astSymbol(ast).type(), debugFlag());
}

void ExpressionTypeFinder::visitTupleLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitTupleLiteral(ast);
    ensureResolved(ast);
}

void ExpressionTypeFinder::visitAttributeAssign(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitAttributeAssign(ast);
    AstNode& id = AST::attributeAssign::id(ast);
    visit(id);
    if (_inViewFilter) {
        if (astText(id) == "attr") {
            AstNode& expr = AST::attributeAssign::expr(ast);
            RootTyp const& et = astSymbol(expr).type();
            if (et.getMetaType() == MetaType::TUPLE) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("filter", "view"),
                                newLoc(expr));
            }
        }
    }
}

void ExpressionTypeFinder::visitMapLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitMapLiteral(ast);
    RootTyp const& unknown = TypeFactory::instance().unknownT();
    RootTyp const* keyType = &unknown;
    RootTyp const* valType = &unknown;
    bool correct = true;
    for (int i = 0, n = AST::mapLiteral::mappingCount(ast); i < n; i++) {
        AstNode& child = AST::mapLiteral::mapping(ast, i);
        RootTyp const& kt = astSymbol(AST::mapping::fromExpr(child)).type();
        RootTyp const& vt = astSymbol(AST::mapping::toExpr(child)).type();
        correct = correct && unifies(*keyType, kt);
        correct = correct && unifies(*valType, vt);
        if (&unknown == keyType) {
            keyType = &kt;
        }
        if (&unknown == valType) {
            valType = &vt;
        }
    }
    if (!correct) {
        SysOut::errorln(SPL_FRONT_END_TYPE_HETEROGENEOUS_LITERAL("map"), newLoc(ast));
        keyType = &unknown;
        valType = &unknown;
    }

    // Type promotions have been done so we can now set the type
    astSymbol(ast).setType(TypeFactory::instance().mapT(*keyType, *valType));
}

void ExpressionTypeFinder::visitSetLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitSetLiteral(ast);
    RootTyp const& unknown = TypeFactory::instance().unknownT();
    RootTyp const* elemType = &unknown;
    bool correct = true;
    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        RootTyp const& et = astSymbol(AST::setLiteral::expr(ast, i)).type();
        correct = correct && unifies(*elemType, et);
        if (&unknown == elemType) {
            elemType = &et;
        }
    }
    if (!correct) {
        SysOut::errorln(SPL_FRONT_END_TYPE_HETEROGENEOUS_LITERAL("set"), newLoc(ast));
        elemType = &unknown;
    }
    astSymbol(ast).setType(TypeFactory::instance().setT(*elemType));
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void ExpressionTypeFinder::visitTypeName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstNode& contentsAst = AST::typeName::contents(ast);
    ensureResolved(contentsAst);
    copyAstSymbol(AstPool::instance(), ast, contentsAst);
}

void ExpressionTypeFinder::visitTypeDims(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitTypeDims(ast);
    checkExpression(_pCtx, ast);
}

#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        // logic onProcess/onTuple/onPunct/onWatermark can appear in a logic clause
#else
// logic onProcess/onTuple/onPunct can appear in a logic clause
#endif
void ExpressionTypeFinder::visitOnProcessLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    _inLogicOnClause = true;
    AstVisitorPushPop::visitOnProcessLogic(ast);
    _inLogicOnClause = false;
}

void ExpressionTypeFinder::visitOnTupleLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    _inLogicOnClause = true;
    AstVisitorPushPop::visitOnTupleLogic(ast);
    _inLogicOnClause = false;
}

void ExpressionTypeFinder::visitOnPunctLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    _inLogicOnClause = true;
    AstVisitorPushPop::visitOnPunctLogic(ast);
    _inLogicOnClause = false;
}

#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        void ExpressionTypeFinder::visitOnWatermarkLogic(AstNode &ast) {
		SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
		_inLogicOnClause = true;
		AstVisitorPushPop::visitOnWatermarkLogic(ast);
		_inLogicOnClause = false;
	}
#endif

void ExpressionTypeFinder::visitUnwrapOrElseExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitUnwrapOrElseExpr(ast);
    const RootTyp& exprType = astSymbol(ast).type();
    AstNode& rhs = AST::unwrapOrElseExpr::rhs(ast);
    const RootTyp& elseType = astSymbol(rhs).type();
    const RootTyp& nullTyp = TypeFactory::instance().nullT();
    if (exprType == nullTyp) {
        SysOut::errorln(SPL_FRONT_END_OPTIONAL_OPERAND_NULL, newLoc(ast));
    } else if (exprType != elseType) {
        SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_INFIX_RELATIVE(
                          "unwrapOrElse", typeToString(exprType), SPL_FRONT_END_FRAGMENT_SAME_TYPE,
                          typeToString(elseType)),
                        newLoc(ast));
    }
}

void ExpressionTypeFinder::visitUnwrapExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitUnwrapExpr(ast);
    const RootTyp& type = astSymbol(ast).type();
    const RootTyp& nullTyp = TypeFactory::instance().nullT();
    if (type == nullTyp) {
        SysOut::errorln(SPL_FRONT_END_OPTIONAL_OPERAND_NULL, newLoc(ast));
    }
}

void ExpressionTypeFinder::visitIsPresentExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceExpressionTypeFinder);
    AstVisitorPushPop::visitIsPresentExpr(ast);
    AstNode& lhs = AST::isPresentExpr::lhs(ast);
    const RootTyp& type = astSymbol(lhs).type();
    const RootTyp& nullTyp = TypeFactory::instance().nullT();
    if (type == nullTyp) {
        SysOut::errorln(SPL_FRONT_END_OPTIONAL_OPERAND_NULL, newLoc(ast));
    }
}

void ExpressionTypeFinder::visitAnnotationT(AstNode& ast)
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
            _inViewAttributeList = true;
            visit(*attributes);
            _inViewAttributeList = false;
        }
        AstNode* filter = vSym.filter();
        if (NULL != filter) {
            _inViewFilter = true;
            visit(*filter);
            _inViewFilter = false;
        }
        AstNode* activateOption = vSym.activateOption();
        if (NULL != activateOption) {
            visit(*activateOption);
        }
    } else if (sym.is<ParallelAnnotationSym>()) {
        ParallelAnnotationSym& pSym = sym.as<ParallelAnnotationSym>();
        AstNode& width = pSym.width();
        visit(width);
        SymbolTableEntry* wSym = astSymbolPtr(width);
        const RootTyp& type = (NULL != wSym) ? wSym->type() : TypeFactory::instance().unknownT();
        if (type.getMetaType() != MetaType::INT32) {
            SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE("width", "parallel", "int32"),
                            newLoc(width));
        }
        AstNode* hostTags = pSym.hostTags();
        bool hostTagError = true;
        if (NULL != hostTags) {
            visit(*hostTags);
            SymbolTableEntry* hSym = astSymbolPtr(*hostTags);
            const RootTyp& hType =
              (NULL != hSym) ? hSym->type() : TypeFactory::instance().unknownT();
            // Type should be a list of rstrings
            if (hType.getMetaType() == MetaType::LIST) {
                const RootTyp& et = hType.as<ListTyp>().getElementType();
                if (et.getMetaType() == MetaType::RSTRING) {
                    hostTagError = false;
                }
            }
            if (hostTagError) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(
                                  "replicateHostTags", "parallel", "list<rstring>"),
                                newLoc(*hostTags));
            }
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
            SymbolTableEntry* hSym = astSymbolPtr(*threads);
            const RootTyp& hType =
              (NULL != hSym) ? hSym->type() : TypeFactory::instance().unknownT();
            if (hType.getMetaType() != MetaType::INT32) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE("threads", "threading", "int32"),
                  newLoc(*threads));
            }
        }
        AstNode* elastic = pSym.elastic();
        if (NULL != elastic) {
            visit(*elastic);
            SymbolTableEntry* hSym = astSymbolPtr(*elastic);
            const RootTyp& hType =
              (NULL != hSym) ? hSym->type() : TypeFactory::instance().unknownT();
            if (hType.getMetaType() != MetaType::BOOLEAN) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE("elastic", "threading", "boolean"),
                  newLoc(*elastic));
            }
        }
    }
}
}
