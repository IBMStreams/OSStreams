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

#include <SPL/FrontEnd/AstNode.h>
#include <SPL/FrontEnd/SPLLexer.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>

#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>

#include <boost/pool/object_pool.hpp>

using namespace std;

namespace SPL {

AstInformation* getAstInformation(AstNode& ast)
{
    return static_cast<AstInformation*>(ast.u);
}

pANTLR3_STRING AstPoolData::newStr(string const& cppString)
{
    return _stringFactory->newStr(_stringFactory, (pANTLR3_UINT8)cppString.c_str());
}

AstPoolData::AstPoolData()
  : _stringFactory(antlr3StringFactoryNew())
  , _treeAdaptor(ANTLR3_TREE_ADAPTORNew(_stringFactory))
{}

AstNode& AstPoolData::addChild(AstNode& parentAst, AstNode& childAst)
{
    _treeAdaptor->addChild(_treeAdaptor, &parentAst, &childAst);
    MODEL("AddChild(" << (void*)&parentAst << ", " << (void*)&childAst << ")\n");
    return parentAst;
}

AstNode& AstPoolData::createRawNode(AstToken& token)
{
    AstNode& result = *static_cast<AstNode*>(_treeAdaptor->create(_treeAdaptor, &token));
    result.u = NULL;
    MODEL("ASTNode(" << (void*)&result << ", " << (int)token.getType(&token) << ", "
                     << astText(result) << ", " << astLine(result) << ", " << astColumn(result)
                     << ")\n");
    return result;
}

AstNode& AstPoolData::createNode(AstToken& token)
{
    AstNode& result = *static_cast<AstNode*>(_treeAdaptor->create(_treeAdaptor, &token));
    (void)new AstGenericNode(result);
    assert(result.u != NULL);
    MODEL("ASTNode(" << (void*)&result << ", " << (int)token.getType(&token) << ", "
                     << astText(result) << ", " << astLine(result) << ", " << astColumn(result)
                     << ")\n");
    return result;
}

AstToken& AstPoolData::createToken(ANTLR3_UINT32 tokenType, string const& text)
{
    pANTLR3_COMMON_TOKEN result = _treeAdaptor->createToken(_treeAdaptor, tokenType, NULL);
    if (!text.empty()) {
        pANTLR3_STRING s = newStr(text);
        result->setText(result, s);
    }
    return *result;
}

AstNode& AstPoolData::createTypeText(ANTLR3_UINT32 tokenType, pANTLR3_UINT8 text)
{
    AstNode& result =
      *static_cast<AstNode*>(_treeAdaptor->createTypeText(_treeAdaptor, tokenType, text));
    (void)new AstGenericNode(result);
    assert(result.u != NULL);
    MODEL("ASTNode(" << (void*)&result << ", " << (int)tokenType << ", " << astText(result) << ", "
                     << astLine(result) << ", " << astColumn(result) << ")\n");
    return result;
}

AstInformation& AstPoolData::ensureAstInformation(AstNode& ast)
{
    assert(NULL != ast.u);
    return *getAstInformation(ast);
}

AstNode& AstPoolData::dupNode(AstNode& ast)
{
    AstNode& result = *static_cast<AstNode*>(ast.dupNode(&ast));
    (void)new AstGenericNode(result);
    assert(result.u != NULL);
    MODEL("ASTNode(" << (void*)&result << ", " << astType(ast) << ", " << astText(result) << ", "
                     << astLine(result) << ", " << astColumn(result) << ")\n");
    return result;
}

void AstPoolData::setChild(AstNode& parentAst, ANTLR3_UINT32 i, AstNode& childAst)
{
    _treeAdaptor->setChild(_treeAdaptor, &parentAst, i, &childAst);
    MODEL("ReplaceChild(" << (void*)&parentAst << ", " << (void*)&childAst << ", " << i << ")\n");
}

// Explicitly instatiate the singleton code here
template class Singleton<AstPool>;

AstPool::AstPool()
  : Singleton<AstPool>(*this)
  , _data(*new AstPoolData())
{}

void AstInformation::copy(AstInformation& other)
{
    assert(_stage == FrontEndStage::PARSER && NULL == _sym && NULL == _expn && NULL == _stmt);
    setStage(other.getStage());
    setSymbol(other.getSymbol());
    if (NULL != other.getExpression()) {
        setExpression(&other.getExpression()->clone());
    }
    if (NULL != other.getStatement()) {
        setStatement(&other.getStatement()->clone());
    }
    setSourceFileIndex(other.getSourceFileIndex());
}

AstInformation::AstInformation(const AstInformation& other)
  : _stage(FrontEndStage::PARSER)
  , _sym(NULL)
  , _expn(NULL)
  , _stmt(NULL)
  , _sourceFileIdx(-1)
{
    setStage(other.getStage());
    setSymbol(other.getSymbol());
    if (NULL != other.getExpression()) {
        setExpression(&other.getExpression()->clone());
    }
    if (NULL != other.getStatement()) {
        setStatement(&other.getStatement()->clone());
    }
    setSourceFileIndex(other.getSourceFileIndex());
}

string astText(AstNode& ast)
{
    pANTLR3_STRING text = ast.getText(&ast);
    if (NULL != text) {
        return (const char*)text->chars;
    }
    switch (ast.getType(&ast)) {
        case ACTUALCLAUSE:
            return "param";
        case AS:
            return "as";
        case ANTLR3_TOKEN_INVALID:
            return "<ERROR_NODE>";
        case ANNOTATION:
            return "ANNOTATION";
        case ANNOTATIONS:
            return "ANNOTATIONS";
            //        case ANNOTATIONTAIL: return "ANNOTATIONTAIL";
        case ATTRIBUTEASSIGN:
            return "ATTRIBUTEASSIGN";
        case ATTRIBUTEDECL:
            return "ATTRIBUTEDECL";
        case BLOCKSTMT:
            return "BLOCKSTMT";
        case BLOCKSTMTNOSCOPE:
            return "BLOCKSTMTNOSCOPE";
        case BOUNDSFORMALS:
            return "BOUNDSFORMALS";
        case CALLEXPR:
            return "CALLEXPR";
        case CASTEXPR:
            return "CASTEXPR";
        case COLON_COLON:
            return "::";
        case COMPILATIONUNIT:
            return "COMPILATIONUNIT";
        case COMPOSITE:
            return "composite";
        case COMPOSITEBODY:
            return "COMPOSITEBODY";
        case COMPOSITEDEF:
            return "COMPOSITEDEF";
        case COMPOSITEFORMAL:
            return "COMPOSITEFORMAL";
        case COMPOSITEFORMALINIT:
            return "COMPOSITEFORMALINIT";
        case COMPOSITEHEAD:
            return "COMPOSITEHEAD";
        case COMPOSITEIN:
            return "COMPOSITEIN";
        case COMPOSITEOUT:
            return "COMPOSITEOUT";
        case COMPOSITEPORT:
            return "COMPOSITEPORT";
        case CONFIG:
            return "config";
        case CONFIGITEM:
            return "CONFIGITEM";
        case CURLYLITERAL:
            return "CURLYLITERAL";
        case DEFTYPE:
            return "DEFTYPE";
        case DOT:
            return ".";
        case ENUMTYPE:
            return "enum";
        case EXPRESSIONMODE:
            return "EXPRESSIONMODE";
        case EXPRSTMT:
            return "EXPRSTMT";
        case FORINIT:
            return "FORINIT";
        case FORMALCLAUSE:
            return "param";
        case FUNCTIONDEF:
            return "FUNCTIONDEF";
        case FUNCTIONFORMAL:
            return "FUNCTIONFORMAL";
        case FUNCTIONHEAD:
            return "FUNCTIONHEAD";
        case FUNCTIONPROTOTYPE:
            return "FUNCTIONPROTOTYPE";
        case GEQ:
            return ">=";
        case GRAPH:
            return "graph";
        case KEYVALUEPAIR:
            return "KEYVALUEPAIR";
        case LISTLITERAL:
            return "LISTLITERAL";
        case LISTTYPE:
            return "list";
        case LOCALDECL:
            return "LOCALDECL";
        case LOCALDECLITEM:
            return "LOCALDECLITEM";
        case LOGIC:
            return "LOGIC";
        case MAPLITERAL:
            return "MAPLITERAL";
        case MAPTYPE:
            return "map";
        case MODIFIERS:
            return "MODIFIERS";
        case NAMESPACENAME:
            return "NAMESPACENAME";
        case NONE:
            return "NONE";
        case ONPROCESS:
            return "onProcess";
        case ONPUNCT:
            return "onPunct";
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        case ONWATERMARK: return "onWatermark";
#endif
        case ONTUPLE:
            return "onTuple";
        case OPACTUALEXPRS:
            return "OPACTUALEXPRS";
        case OPACTUALNAME:
            return "OPACTUALNAME";
        case OPACTUALTYPE:
            return "OPACTUALTYPE";
        case OPINPUTS:
            return "OPINPUTS";
        case OPINVOKE:
            return "OPINVOKE";
        case OPINVOKEACTUAL:
            return "OPINVOKEACTUAL";
        case OPINVOKEBODY:
            return "OPINVOKEBODY";
        case OPINVOKEHEAD:
            return "OPINVOKEHEAD";
        case OPINVOKEOUTPUT:
            return "OPINVOKEOUTPUT";
        case OPINVOKEWINDOW:
            return "OPINVOKEWINDOW";
        case OPOUTPUT:
            return "OPOUTPUT";
        case OPOUTPUTS:
            return "OPOUTPUTS";
        case OUTPUTCLAUSE:
            return "output";
        case PARAMSIGNATURE:
            return "PARAMSIGNATURE";
        case PARENTHESIZEDEXPR:
            return "PARENTHESIZEDEXPR";
        case PORTINPUTS:
            return "PORTINPUTS";
        case POSTFIXEXPR:
            return "POSTFIXEXPR";
        case PREFIXEXPR:
            return "PREFIXEXPR";
        case PRIMITIVETYPE:
            return "PRIMITIVETYPE";
        case PROTOFORMALS:
            return "PROTOFORMALS";
        case PROTOFORMAL:
            return "PROTOFORMAL";
        case RETURN:
            return "return";
        case RSHIFT:
            return ">>";
        case RSHIFT_EQ:
            return ">>=";
        case SETLITERAL:
            return "SETLITERAL";
        case SETTYPE:
            return "set";
        case SLICE:
            return ":";
        case SPLDOC:
            return "SPLDOC";
        case SPLNAMESPACE:
            return "SPLNAMESPACE";
        case STATE:
            return "state";
        case STREAM:
            return "STREAM";
        case STRING_LITERAL:
            return "STRING_LITERAL";
        case SUBSCRIPTEXPR:
            return "SUBSCRIPTEXPR";
        case STMT:
            return "STMT";
        case TUPLEATTRIB:
            return "TUPLEATTRIB";
        case TUPLEEXTEND:
            return "TUPLEEXTEND";
        case TUPLELITERAL:
            return "TUPLELITERAL";
        case TUPLETYPE:
            return "TUPLETYPE";
        case TYPEARGS:
            return "TYPEARGS";
        case TYPECLAUSE:
            return "type";
        case TYPEDIMS:
            return "TYPEDIMS";
        case TYPEFORMAL:
            return "TYPEFORMAL";
        case TYPEFORMALS:
            return "TYPEFORMALS";
        case TYPENAME:
            return "TYPENAME";
        case UNARY_MINUS:
            return "-";
        case USEDIRECTIVES:
            return "USEDIRECTIVES";
        case USEDIRECTIVETAIL:
            return "USEDIRECTIVETAIL";
        case USE_WILDCARD:
            return "*";
        case WINDOW:
            return "window";
        case XML:
            return "xml";
    }
    ostringstream os;
    os << "<" << ast.getType(&ast) << ">";
    return os.str();
}

int astColumn(AstNode& ast)
{
    return 1 + ast.getCharPositionInLine(&ast); // 1-based column numbers
}

int astLine(AstNode& ast)
{
    return ast.getLine(&ast);
}

int32_t astType(AstNode& ast)
{
    return (int32_t)ast.getType(&ast);
}

AstNodeBase& wrapper(AstNode& ast)
{
    pANTLR3_COMMON_TREE pSuper = static_cast<pANTLR3_COMMON_TREE>(ast.super);
    assert(NULL != pSuper->super);
    return *static_cast<AstNodeBase*>(pSuper->super);
}

AstToken& astToken(AstNode& ast)
{
    AstToken* tok = ast.getToken(&ast);
    assert(tok);
    return *tok;
}

AstNode& astChild(AstNode& ast, int index)
{
    assert(0 <= index && index < astSize(ast));
    AstNode* result = static_cast<AstNode*>(ast.getChild(&ast, index));
    assert(NULL != result);
    return *result;
}

int astSize(AstNode& ast)
{
    return ast.getChildCount(&ast);
}

static AstNode& setLoc(AstNode& ast, AstToken& start)
{
    AstToken& dstToken = astToken(ast);
    dstToken.setLine(&dstToken, start.getLine(&start));
    dstToken.setCharPositionInLine(&dstToken, start.getCharPositionInLine(&start));
    return ast;
}

AstNode& createAst(AstToken& start)
{
    AstPool& pool = AstPool::instance();
    AstNode& result = pool->createRawNode(start);
    assert(result.u == NULL);
    return setLoc(result, start);
}

AstNode& createAst(AstToken& start, int tokenType, const string& tokenText)
{
    AstPool& pool = AstPool::instance();
    AstToken& token = pool->createToken(tokenType, tokenText);
    AstNode& result = pool->createRawNode(token);
    assert(result.u == NULL);
    return setLoc(result, start);
}

AstNode& createAst(int tokenType, const string& tokenText)
{
    AstPool& pool = AstPool::instance();
    AstToken& token = pool->createToken(tokenType, tokenText);
    AstNode& result = pool->createRawNode(token);
    assert(result.u == NULL);
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   vector<AstNode*>& children)
{
    AstNode& result = createAst(start, tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    return result;
}

AstNode& createAst(int tokenType, const string& tokenText, vector<AstNode*>& children)
{
    AstNode& result = createAst(tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    return result;
}

AstNode& createAst(int tokenType, const string& tokenText, AstNode& child0)
{
    AstNode& result = createAst(tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child0);
    return result;
}

AstNode& createAst(int tokenType, const string& tokenText, AstNode& child0, AstNode& child1)
{
    AstNode& result = createAst(tokenType, tokenText, child0);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child1);
    return result;
}

AstNode& createAst(int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2)
{
    AstNode& result = createAst(tokenType, tokenText, child0, child1);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child2);
    return result;
}

AstNode& createAst(int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   vector<AstNode*>& children)
{
    AstNode& result = createAst(tokenType, tokenText, child0, child1, child2);
    AstPool& pool = AstPool::instance();
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    return result;
}

AstNode& createAst(int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   AstNode& child3)
{
    AstNode& result = createAst(tokenType, tokenText, child0, child1, child2);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child3);
    return result;
}

AstNode& createAst(AstToken& start, int tokenType, const string& tokenText, AstNode& child0)
{
    AstNode& result = createAst(start, tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child0);
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   vector<AstNode*>& children)
{
    AstNode& result = createAst(start, tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child0);
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   vector<AstNode*>& children)
{
    AstNode& result = createAst(start, tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child0);
    pool->addChild(result, child1);
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   vector<AstNode*>& children)
{
    AstNode& result = createAst(start, tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child0);
    pool->addChild(result, child1);
    pool->addChild(result, child2);
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   AstNode& child3,
                   vector<AstNode*>& children)
{
    AstNode& result = createAst(start, tokenType, tokenText);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child0);
    pool->addChild(result, child1);
    pool->addChild(result, child2);
    pool->addChild(result, child3);
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1)
{
    AstNode& result = createAst(start, tokenType, tokenText, child0);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child1);
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2)
{
    AstNode& result = createAst(start, tokenType, tokenText, child0, child1);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child2);
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   AstNode& child3)
{
    AstNode& result = createAst(start, tokenType, tokenText, child0, child1, child2);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child3);
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   AstNode& child3,
                   AstNode& child4)
{
    AstNode& result = createAst(start, tokenType, tokenText, child0, child1, child2, child3);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child4);
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   AstNode& child3,
                   AstNode& child4,
                   AstNode& child5)
{
    AstNode& result =
      createAst(start, tokenType, tokenText, child0, child1, child2, child3, child4);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child5);
    return result;
}

AstNode& createAst(AstToken& start,
                   int tokenType,
                   const string& tokenText,
                   AstNode& child0,
                   AstNode& child1,
                   AstNode& child2,
                   AstNode& child3,
                   AstNode& child4,
                   AstNode& child5,
                   AstNode& child6)
{
    AstNode& result =
      createAst(start, tokenType, tokenText, child0, child1, child2, child3, child4, child5);
    AstPool& pool = AstPool::instance();
    pool->addChild(result, child6);
    return result;
}

namespace AST {

namespace compositePort {

AstNode& create(AstToken& start, AstNode* streamType, AstNode& id)
{
    AstNode& result = createAst(start, COMPOSITEPORT, "COMPOSITEPORT");
    AstPool& pool = AstPool::instance();
    if (NULL != streamType) {
        pool->addChild(result, *streamType);
    }
    pool->addChild(result, id);
    assert(result.u == NULL);
    (void)new AstCompositePort(result);
    assert(result.u != NULL);
    return result;
}

bool is(AstNode& ast)
{
    return AstNodeBase::CompositePort == wrapper(ast).getKind();
}

bool has_type(AstNode& ast)
{
    assert(is(ast));
    return 2 == astSize(ast);
}

AstNode& type(AstNode& ast)
{
    assert(is(ast) && has_type(ast));
    return astChild(ast, 0);
}

AstNode& id(AstNode& ast)
{
    assert(is(ast));
    return astChild(ast, astSize(ast) - 1);
}

} // namespace compositePort

namespace compositeBody {

AstNode& create(AstToken& start, std::vector<AstNode*>& children)
{
    AstNode& result = createAst(start, COMPOSITEBODY, "COMPOSITEBODY", children);
    assert(result.u == NULL);
    (void)new AstCompositeBody(result);
    assert(result.u != NULL);
    return result;
}

AstNode& create(AstNode* formal, AstNode* types, AstNode* graph, AstNode* config)
{
    AstNode& result = createAst(COMPOSITEBODY, "COMPOSITEBODY");
    AstPool& pool = AstPool::instance();
    if (NULL != formal) {
        pool->addChild(result, *formal);
    }
    if (NULL != types) {
        pool->addChild(result, *types);
    }
    if (NULL != graph) {
        pool->addChild(result, *graph);
    }
    if (NULL != config) {
        pool->addChild(result, *config);
    }
    assert(result.u == NULL);
    (void)new AstCompositeBody(result);
    assert(result.u != NULL);
    return result;
}

bool is(AstNode& ast)
{
    return AstNodeBase::CompositeBody == wrapper(ast).getKind();
}

bool has_formal(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::formalClause::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& formal(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::formalClause::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

bool has_types(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::typeClause::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& types(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::typeClause::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

bool has_graph(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::graphClause::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& graph(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::graphClause::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

bool has_config(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::configClause::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& config(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::configClause::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

} // namespace compositeBody

namespace portInputs {

AstNode& create(AstToken& start, AstNode* type, std::vector<AstNode*>& children, AstNode* alias)
{
    AstNode& result = createAst(start, PORTINPUTS, "PORTINPUTS");
    AstPool& pool = AstPool::instance();
    if (NULL != type) {
        pool->addChild(result, *type);
    }
    for (vector<AstNode*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        pool->addChild(result, **it);
    }
    if (NULL != alias) {
        pool->addChild(result, *alias);
    }
    assert(result.u == NULL);
    (void)new AstPortInputs(result);
    assert(result.u != NULL);
    return result;
}

bool is(AstNode& ast)
{
    return AstNodeBase::PortInputs == wrapper(ast).getKind();
}

bool has_type(AstNode& ast)
{
    assert(is(ast));
    return 0 < astSize(ast) && AST::streamType::is(astChild(ast, 0));
}

AstNode& type(AstNode& ast)
{
    assert(has_type(ast));
    return astChild(ast, 0);
}

int idCount(AstNode& ast)
{
    assert(is(ast));
    return astSize(ast) + (has_type(ast) ? -1 : 0) + (has_alias(ast) ? -1 : 0);
}

int idStart(AstNode& ast)
{
    assert(is(ast));
    return 0 + (has_type(ast) ? 1 : 0);
}

AstNode& id(AstNode& ast, int iRel)
{
    assert(is(ast) && 0 <= iRel && iRel < idCount(ast));
    return astChild(ast, iRel + (has_type(ast) ? 1 : 0));
}

bool has_alias(AstNode& ast)
{
    assert(is(ast));
    return 0 < astSize(ast) && AST::as::is(astChild(ast, astSize(ast) - 1));
}

AstNode& alias(AstNode& ast)
{
    assert(has_alias(ast));
    return astChild(ast, astSize(ast) - 1);
}

} // namespace portInputs

namespace opInvokeBody {

AstNode& create(AstToken& start, std::vector<AstNode*>& children)
{
    AstNode& result = createAst(start, OPINVOKEBODY, "OPINVOKEBODY", children);
    assert(result.u == NULL);
    (void)new AstOpInvokeBody(result);
    assert(result.u != NULL);
    return result;
}

bool is(AstNode& ast)
{
    return AstNodeBase::OpInvokeBody == wrapper(ast).getKind();
}

bool has_logic(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::logic::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& logic(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::logic::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

bool has_window(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::window::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& window(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::window::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

bool has_actual(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::actualClause::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& actual(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::actualClause::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

bool has_output(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::outputClause::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& output(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::outputClause::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}

bool has_config(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::configClause::is(astChild(ast, i))) {
            return true;
        }
    }
    return false;
}

AstNode& config(AstNode& ast)
{
    assert(is(ast));
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (AST::configClause::is(astChild(ast, i))) {
            return astChild(ast, i);
        }
    }
    assert(false);
    return ast;
}
} // namespace opInvokeBody

namespace infixExpr {

AstNode& create(AstToken& start, AstNode& opAst, AstNode& lhsAst, AstNode& rhsAst)
{
    assert(0 == astSize(opAst));
    AstPool& pool = AstPool::instance();
    if (!AST::errorT::is(opAst)) {
        pool->addChild(opAst, lhsAst);
        pool->addChild(opAst, rhsAst);
    }
    setLoc(opAst, start);
    return opAst;
}

bool is(AstNode& ast)
{
    switch (ast.getType(&ast)) {
        case AMP:
        case AMP_AMP:
        case AMP_EQ:
        case BAR:
        case BAR_BAR:
        case BAR_EQ:
        case DOT_AMP:
        case DOT_BAR:
        case DOT_EQ_EQ:
        case DOT_GEQ:
        case DOT_GREATER:
        case DOT_HAT:
        case DOT_LEQ:
        case DOT_LESS:
        case DOT_LSHIFT:
        case DOT_MINUS:
        case DOT_MOD:
        case DOT_NEQ:
        case DOT_PLUS:
        case DOT_RSHIFT:
        case DOT_SLASH:
        case DOT_STAR:
        case EQ:
        case EQ_EQ:
        case GEQ:
        case GREATER:
        case HAT:
        case HAT_EQ:
        case IN:
        case LEQ:
        case LESS:
        case LSHIFT:
        case LSHIFT_EQ:
        case MINUS:
        case MINUS_EQ:
        case MOD:
        case MOD_EQ:
        case NEQ:
        case PLUS:
        case PLUS_EQ:
        case RSHIFT:
        case RSHIFT_EQ:
        case SLASH:
        case SLASH_EQ:
        case STAR:
        case STAR_EQ:
            return 2 == astSize(ast);
        default:
            return false;
    }
    return false;
}

AstNode& lhs(AstNode& ast)
{
    assert(is(ast));
    return astChild(ast, 0);
}

AstNode& rhs(AstNode& ast)
{
    assert(is(ast));
    return astChild(ast, 1);
}

void setLhs(AstNode& ast, AstNode& newChild)
{
    assert(is(ast));
    assert(0 < astSize(ast));
    AstPool::instance()->setChild(ast, 0, newChild);
    assert(&newChild == &astChild(ast, 0));
}

void setRhs(AstNode& ast, AstNode& newChild)
{
    assert(is(ast));
    assert(1 < astSize(ast));
    AstPool::instance()->setChild(ast, 1, newChild);
    assert(&newChild == &astChild(ast, 1));
}
} // namespace infixExpr
} // namespace AST
} // namespace SPL

#include <SPL/FrontEnd/AstNodeGen.cpp>
