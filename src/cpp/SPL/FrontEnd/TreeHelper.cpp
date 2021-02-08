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

#include <SPL/FrontEnd/TreeHelper.h>

#include <SPL/FrontEnd/SPLLexer.h>
#include <SPL/FrontEnd/SPLParser.h>
#include <antlr3.h>
#undef VERSION
#undef PACKAGE
#undef PACKAGE_VERSION
#undef PACKAGE_TARNAME
#undef PACKAGE_STRING
#undef PACKAGE_NAME
#undef PACKAGE_BUGREPORT

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/FrontEnd/AstPrinter.h>
#include <SPL/FrontEnd/Diagnostician.h>
#include <SPL/FrontEnd/EmptyContainerTypeFinder.h>
#include <SPL/FrontEnd/ExpressionTypeFinder.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/IRGenerator.h>
#include <SPL/FrontEnd/LexicalNameUseFinder.h>
#include <SPL/FrontEnd/LogicSubmitChecker.h>
#include <SPL/FrontEnd/NameDefFinder.h>
#include <SPL/FrontEnd/PromotionChecker.h>
#include <SPL/FrontEnd/RelativeNameUseFinder.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TypeAnalyzer.h>
#include <SPL/FrontEnd/TypePromoter.h>
#include <SPL/FrontEnd/VariableUseChecker.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SysOut.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <tr1/unordered_set>
#include <vector>

namespace SPL {

static void fatalTreeParserError(pANTLR3_BASE_RECOGNIZER recognizer)
{
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << std::endl;
    SysOut::writeln(SPL_FRONT_END_FATAL_TREE_PARSER, std::cerr);
    pANTLR3_TREE_PARSER tparser = (pANTLR3_TREE_PARSER)recognizer->super;
    pANTLR3_VECTOR nodeVector = tparser->getTreeNodeStream(tparser)->nodes;
    int errorIndex = recognizer->state->exception->index;
    for (int i = 0; i < errorIndex; i++) {
        AstNode& curr = *static_cast<AstNode*>(nodeVector->get(nodeVector, i));
        AstNode& next = *static_cast<AstNode*>(nodeVector->get(nodeVector, i + 1));
        if (ANTLR3_TOKEN_DOWN == astType(next)) {
            std::cerr << "(";
        }
        if (ANTLR3_TOKEN_DOWN != astType(curr) && ANTLR3_TOKEN_UP != astType(curr)) {
            std::cerr << astText(curr);
        }
        if (ANTLR3_TOKEN_UP == astType(next)) {
            std::cerr << ")";
        } else {
            std::cerr << " ";
        }
    }
    std::cerr << "ERROR" << std::endl;
    AstNode& errorNode = *static_cast<AstNode*>(recognizer->state->exception->token);
    std::cerr << "Error node=" << errorNode << std::endl;
    assert(false);
}

static std::string tokenToString(int expected, pANTLR3_UINT8* tokenNames)
{
    switch (expected) {
        // tokens with direct string representations
        case ACTUALCLAUSE:
            return "'param'";
        case AMP:
            return "'&'";
        case AMP_AMP:
            return "'&&'";
        case AMP_EQ:
            return "'&='";
        case AS:
            return "'as'";
        case BANG:
            return "'!'";
        case BAR:
            return "'|'";
        case BAR_BAR:
            return "'||'";
        case BAR_EQ:
            return "'|='";
        case BLOB:
            return "'blob'";
        case BOOLEAN:
            return "'boolean'";
        case BREAK:
            return "'break'";
        case COLON:
            return "':'";
        case COLON_COLON:
            return "'::'";
        case COMMA:
            return "','";
        case COMPLEX128:
            return "'complex128'";
        case COMPLEX32:
            return "'complex32'";
        case COMPLEX64:
            return "'complex64'";
        case COMPOSITE:
            return "'composite'";
        case COMPOSITEFORMAL:
            return "'param'";
        case COMPOSITEIN:
            return "'input'";
        case COMPOSITEOUT:
            return "'output'";
        case CONFIG:
            return "'config'";
        case CONTINUE:
            return "'continue'";
        case CURLYLITERAL:
            return "'{}'";
        case DECIMAL128:
            return "'decimal128'";
        case DECIMAL32:
            return "'decimal32'";
        case DECIMAL64:
            return "'decimal64'";
        case DOLLAR:
            return "'$'";
        case DOT:
            return "'.'";
        case DOT_AMP:
            return "'.&'";
        case DOT_BAR:
            return "'.|'";
        case DOT_EQ_EQ:
            return "'.=='";
        case DOT_GEQ:
            return "'.>='";
        case DOT_GREATER:
            return "'.>'";
        case DOT_HAT:
            return "'.^'";
        case DOT_LEQ:
            return "'.<='";
        case DOT_LESS:
            return "'.<'";
        case DOT_LSHIFT:
            return "'.<<'";
        case DOT_MINUS:
            return "'.-'";
        case DOT_MOD:
            return "'.%'";
        case DOT_NEQ:
            return "'.!='";
        case DOT_PLUS:
            return "'.+'";
        case DOT_RSHIFT:
            return "'.>>'";
        case DOT_SLASH:
            return "'./'";
        case DOT_STAR:
            return "'.*'";
        case DQUOTE:
            return "'\"'";
        case ELSE:
            return "'else'";
        case ENUM:
            return "'enum'";
        case EQ:
            return "'='";
        case EQ_EQ:
            return "'=='";
        case FALSE_LITERAL:
            return "'false'";
        case FLOAT128:
            return "'float128'";
        case FLOAT32:
            return "'float32'";
        case FLOAT64:
            return "'float64'";
        case FOR:
            return "'for'";
        case FORMALCLAUSE:
            return "'param'";
        case GEQ:
            return "'>='";
        case GRAPH:
            return "'graph'";
        case GREATER:
            return "'>'";
        case HAT:
            return "'^'";
        case HAT_EQ:
            return "'^='";
        case IF:
            return "'if'";
        case IN:
            return "'in'";
        case INPUTLIT:
            return "'input'";
        case INT16:
            return "'int16'";
        case INT32:
            return "'int32'";
        case INT64:
            return "'int64'";
        case INT8:
            return "'int8'";
        case LBRACKET:
            return "'['";
        case LCURLY:
            return "'{'";
        case LEQ:
            return "'<='";
        case LESS:
            return "'<'";
        case LIST:
            return "'list'";
        case LOGIC:
            return "'logic'";
        case LPAREN:
            return "'('";
        case LSHIFT:
            return "'<<'";
        case LSHIFT_EQ:
            return "'<<='";
        case MAP:
            return "'map'";
        case MATRIX:
            return "'matrix'";
        case MINUS:
            return "'-'";
        case MINUS_EQ:
            return "'-='";
        case MINUS_MINUS:
            return "'--'";
        case ML_COMMENT:
            return "'/*...*/'";
        case MOD:
            return "'%'";
        case MOD_EQ:
            return "'%='";
        case MUTABLE:
            return "'mutable'";
        case NAMESPACE:
            return "'namespace'";
        case NEQ:
            return "'!='";
        case ONPROCESS:
            return "'onProcess'";
        case ONPUNCT:
            return "'onPunct'";
        case ONTUPLE:
            return "'onTuple'";
#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        case ONWATERMARK:
                                return "'onWatermark'";
#endif
        case OPINVOKEOUTPUT:
            return "'output'";
        case OPINVOKEWINDOW:
            return "'window'";
        case OPOUTPUTS:
            return "'output'";
        case OUTPUT:
            return "'output'";
        case OUTPUTCLAUSE:
            return "'output'";
        case PARAM:
            return "'param'";
        case PLUS:
            return "'+'";
        case PLUS_EQ:
            return "'+='";
        case PLUS_PLUS:
            return "'++'";
        case PUBLIC:
            return "'public'";
        case QMARK:
            return "'?'";
        case RBRACKET:
            return "']'";
        case RCURLY:
            return "'}'";
        case RETURN:
            return "'return'";
        case RPAREN:
            return "')'";
        case RSHIFT:
            return "'>>'";
        case RSHIFT_EQ:
            return "'>>='";
        case RSTRING:
            return "'rstring'";
        case SEMI:
            return "';'";
        case SET:
            return "'set'";
        case SLASH:
            return "'/'";
        case SLASH_EQ:
            return "'/='";
        case SLASH_SLASH:
            return "'//'";
        case SLASH_STAR:
            return "'/*'";
        case SL_COMMENT:
            return "'//...'";
        case SPLDOC:
            return "'splDoc'";
        case SPLNAMESPACE:
            return "'namespace'";
        case STAR:
            return "'*'";
        case STAR_EQ:
            return "'*='";
        case STAR_SLASH:
            return "'*/'";
        case STATE:
            return "'state'";
        case STATEFUL:
            return "'stateful'";
        case STATIC:
            return "'static'";
        case STREAM:
            return "'stream'";
        case STRING_LITERAL:
            return "'\"...\"'";
        case TILDE:
            return "'~'";
        case TIMESTAMP:
            return "'timestamp'";
        case TRUE_LITERAL:
            return "'true'";
        case TUPLE:
            return "'tuple'";
        case TYPE:
            return "'type'";
        case TYPECLAUSE:
            return "'type'";
        case UINT16:
            return "'uint16'";
        case UINT32:
            return "'uint32'";
        case UINT64:
            return "'uint64'";
        case UINT8:
            return "'uint8'";
        case UNARY_MINUS:
            return "'-'";
        case USE:
            return "'use'";
        case USE_WILDCARD:
            return "'*'";
        case USTRING:
            return "'ustring'";
        case VAR:
            return "'var'";
        case VOID:
            return "'void'";
        case WHILE:
            return "'while'";
        case WINDOW:
            return "'window'";
        case XML:
            return "'xml'";

            // syntactic entities with natural language descriptions
        case ATTRIBUTE:
            return SPL_FRONT_END_TOKEN_ATTRIBUTE;
        case ATTRIBUTEASSIGN:
            return SPL_FRONT_END_TOKEN_ATTRIBUTEASSIGN;
        case ATTRIBUTEDECL:
            return SPL_FRONT_END_TOKEN_ATTRIBUTEDECL;
        case BLOCKSTMT:
            return SPL_FRONT_END_TOKEN_BLOCKSTMT;
        case BLOCKSTMTNOSCOPE:
            return SPL_FRONT_END_TOKEN_BLOCKSTMTNOSCOPE;
        case BOUNDSFORMALS:
            return SPL_FRONT_END_TOKEN_BOUNDSFORMALS;
        case CALLEXPR:
            return SPL_FRONT_END_TOKEN_CALLEXPR;
        case CASTEXPR:
            return SPL_FRONT_END_TOKEN_CASTEXPR;
        case COMPILATIONUNIT:
            return SPL_FRONT_END_TOKEN_COMPILATIONUNIT;
        case COMPOSITEBODY:
            return SPL_FRONT_END_TOKEN_COMPOSITEBODY;
        case COMPOSITEDEF:
            return SPL_FRONT_END_TOKEN_COMPOSITEDEF;
        case COMPOSITEFORMALINIT:
            return SPL_FRONT_END_TOKEN_COMPOSITEFORMALINIT;
        case COMPOSITEHEAD:
            return SPL_FRONT_END_TOKEN_COMPOSITEHEAD;
        case COMPOSITEPORT:
            return SPL_FRONT_END_TOKEN_COMPOSITEPORT;
        case CONFIGITEM:
            return SPL_FRONT_END_TOKEN_CONFIGITEM;
        case DEFTYPE:
            return SPL_FRONT_END_TOKEN_DEFTYPE;
        case DIGIT:
            return SPL_FRONT_END_TOKEN_DIGIT;
        case ENUMTYPE:
            return SPL_FRONT_END_TOKEN_ENUMTYPE;
        case EXPONENT:
            return SPL_FRONT_END_TOKEN_EXPONENT;
        case EXPRESSION:
            return SPL_FRONT_END_TOKEN_EXPRESSION;
        case EXPRESSIONMODE:
            return SPL_FRONT_END_TOKEN_EXPRESSIONMODE;
        case EXPRSTMT:
            return SPL_FRONT_END_TOKEN_EXPRSTMT;
        case FLOAT_LITERAL:
            return SPL_FRONT_END_TOKEN_FLOAT_LITERAL;
        case FORINIT:
            return SPL_FRONT_END_TOKEN_FORINIT;
        case FUNCTION:
            return SPL_FRONT_END_TOKEN_FUNCTION;
        case FUNCTIONDEF:
            return SPL_FRONT_END_TOKEN_FUNCTIONDEF;
        case FUNCTIONFORMAL:
            return SPL_FRONT_END_TOKEN_FUNCTIONFORMAL;
        case FUNCTIONHEAD:
            return SPL_FRONT_END_TOKEN_FUNCTIONHEAD;
        case FUNCTIONPROTOTYPE:
            return SPL_FRONT_END_TOKEN_FUNCTIONPROTOTYPE;
        case HEXDIGIT:
            return SPL_FRONT_END_TOKEN_HEXDIGIT;
        case HEX_LITERAL:
            return SPL_FRONT_END_TOKEN_HEX_LITERAL;
        case ID:
            return SPL_FRONT_END_TOKEN_ID;
        case INT_LITERAL:
            return SPL_FRONT_END_TOKEN_INT_LITERAL;
        case LISTLITERAL:
            return SPL_FRONT_END_TOKEN_LISTLITERAL;
        case LISTTYPE:
            return SPL_FRONT_END_TOKEN_LISTTYPE;
        case LOCALDECL:
            return SPL_FRONT_END_TOKEN_LOCALDECL;
        case LOCALDECLITEM:
            return SPL_FRONT_END_TOKEN_LOCALDECLITEM;
        case MAPLITERAL:
            return SPL_FRONT_END_TOKEN_MAPLITERAL;
        case MAPTYPE:
            return SPL_FRONT_END_TOKEN_MAPTYPE;
        case MODIFIERS:
            return SPL_FRONT_END_TOKEN_MODIFIERS;
        case NAMESPACENAME:
            return SPL_FRONT_END_TOKEN_NAMESPACENAME;
        case NONE:
            return SPL_FRONT_END_TOKEN_NONE;
        case OPACTUALEXPRS:
            return SPL_FRONT_END_TOKEN_OPACTUALEXPRS;
        case OPACTUALNAME:
            return SPL_FRONT_END_TOKEN_OPACTUALNAME;
        case OPACTUALTYPE:
            return SPL_FRONT_END_TOKEN_OPACTUALTYPE;
        case OPERATOR:
            return SPL_FRONT_END_TOKEN_OPERATOR;
        case OPINPUTS:
            return SPL_FRONT_END_TOKEN_OPINPUTS;
        case OPINVOKE:
            return SPL_FRONT_END_TOKEN_OPINVOKE;
        case OPINVOKEACTUAL:
            return SPL_FRONT_END_TOKEN_OPINVOKEACTUAL;
        case OPINVOKEBODY:
            return SPL_FRONT_END_TOKEN_OPINVOKEBODY;
        case OPINVOKEHEAD:
            return SPL_FRONT_END_TOKEN_OPINVOKEHEAD;
        case OPOUTPUT:
            return SPL_FRONT_END_TOKEN_OPOUTPUT;
        case PARAMSIGNATURE:
            return SPL_FRONT_END_TOKEN_PARAMSIGNATURE;
        case PARENTHESIZEDEXPR:
            return SPL_FRONT_END_TOKEN_PARENTHESIZEDEXPR;
        case PORTINPUTS:
            return SPL_FRONT_END_TOKEN_PORTINPUTS;
        case POSTFIXEXPR:
            return SPL_FRONT_END_TOKEN_POSTFIXEXPR;
        case PREFIXEXPR:
            return SPL_FRONT_END_TOKEN_PREFIXEXPR;
        case PRIMITIVETYPE:
            return SPL_FRONT_END_TOKEN_PRIMITIVETYPE;
        case PROTOFORMALS:
            return SPL_FRONT_END_TOKEN_PROTOFORMALS;
        case SETLITERAL:
            return SPL_FRONT_END_TOKEN_SETLITERAL;
        case SETTYPE:
            return SPL_FRONT_END_TOKEN_SETTYPE;
        case SLICE:
            return SPL_FRONT_END_TOKEN_SLICE;
        case SUBSCRIPTEXPR:
            return SPL_FRONT_END_TOKEN_SUBSCRIPTEXPR;
        case STMT:
            return SPL_FRONT_END_TOKEN_STMT;
        case TUPLEATTRIB:
            return SPL_FRONT_END_TOKEN_TUPLEATTRIB;
        case TUPLEEXTEND:
            return SPL_FRONT_END_TOKEN_TUPLEEXTEND;
        case TUPLELITERAL:
            return SPL_FRONT_END_TOKEN_TUPLELITERAL;
        case TUPLETYPE:
            return SPL_FRONT_END_TOKEN_TUPLETYPE;
        case TYPEARGS:
            return SPL_FRONT_END_TOKEN_TYPEARGS;
        case TYPEDIMS:
            return SPL_FRONT_END_TOKEN_TYPEDIMS;
        case TYPEFORMAL:
            return SPL_FRONT_END_TOKEN_TYPEFORMAL;
        case TYPEFORMALS:
            return SPL_FRONT_END_TOKEN_TYPEFORMALS;
        case TYPENAME:
            return SPL_FRONT_END_TOKEN_TYPENAME;
        case USEDIRECTIVES:
            return SPL_FRONT_END_TOKEN_USEDIRECTIVES;
        case USEDIRECTIVETAIL:
            return SPL_FRONT_END_TOKEN_USEDIRECTIVETAIL;
        case WHITE_SPACE:
            return SPL_FRONT_END_TOKEN_WHITE_SPACE;

            // other
        case ANTLR3_TOKEN_EOF:
            return SPL_FRONT_END_TOKEN_EOF;
        default: {
            if (NULL == tokenNames) {
                return "" + expected;
            } else {
                return (const char*)tokenNames[expected];
            }
        }
    }
}

// -----------------------------------------------------------------
ParserContext::ParserContext(std::string const& infileName)
  : _lastSyntaxError(NULL)
  , _result(NULL)
  , _fileNameIdx(SourceLocation::fileName2idx(infileName))
  , _sourceStringIdx(StringSourceLocation::NOSTRING_IDX)
{}

ParserContext::ParserContext(std::string const& infileName, std::string const& insourceString)
  : _lastSyntaxError(NULL)
  , _result(NULL)
  , _fileNameIdx(SourceLocation::fileName2idx(infileName))
  , _sourceStringIdx(SourceLocationFactory::instance().sourceString2idx(insourceString))
{}

std::string const& ParserContext::fileName() const
{
    return SourceLocation::idx2fileName(_fileNameIdx);
}

std::string const& ParserContext::sourceString() const
{
    return SourceLocationFactory::instance().idx2sourceString(_sourceStringIdx);
}

// -----------------------------------------------------------------
PrinterContext::PrinterContext(std::string inputFileName, std::ostream& outputStream)
  : ParserContext(inputFileName)
  , _indentLevel(0)
  , _indentSize(2)
  , _isLineStart(true)
  , _os(outputStream)
{}

void PrinterContext::pln()
{
    _os << std::endl;
    _isLineStart = true;
}

void PrinterContext::print(std::string const& s)
{
    if (_isLineStart) {
        for (int i = 0, n = _indentLevel * _indentSize; i < n; i++) {
            _os << " ";
        }
    }
    _os << s;
    _isLineStart = false;
}

// -----------------------------------------------------------------

class SPLAst2 : public SPLAst
{
    pSPLLexer _lexer;
    pSPLParser _parser;
    SPLAst2(pANTLR3_INPUT_STREAM input, ParserContext& pCtx)
      : SPLAst(input)
    {
        _lexer = SPLLexerNew(input);
        _lexer->pLexer->rec->displayRecognitionError = displayRecognitionError;
        _lexer->pLexer->rec->state->userp = &pCtx;
        tokenize(TOKENSOURCE(_lexer));
        _parser = SPLParserNew(_tokens);
        _parser->pParser->rec->displayRecognitionError = displayRecognitionError;
        _parser->pParser->rec->state->userp = &pCtx;
    }

  public:
    friend class SPLAst;
    virtual ~SPLAst2()
    {
        _lexer->free(_lexer);
        _parser->free(_parser);
    }
    ParserContext& parserContext()
    {
        return *static_cast<ParserContext*>(_parser->pParser->rec->state->userp);
    }
};

// -----------------------------------------------------------------
SPLAst::SPLAst(pANTLR3_INPUT_STREAM input)
  : _ast(NULL)
  , _input(input)
  , _tokens(NULL)
{}

SPLAst::~SPLAst()
{
    _ast = NULL;
    _input->close(_input);
    _tokens->free(_tokens);
}

AstNode& SPLAst::ast()
{
    assert(NULL != _ast);
    return *_ast;
}

AstNode* SPLAst::astPtr()
{
    return _ast;
}

void SPLAst::setAst(AstNode* inast)
{
    assert(NULL != inast);
    _ast = inast;
}

pANTLR3_INPUT_STREAM SPLAst::antlrStreamFromFile(std::string const& fileName)
{
    pANTLR3_UINT8 antlrFileName = (pANTLR3_UINT8)fileName.c_str();
    return antlr3AsciiFileStreamNew(antlrFileName);
}

pANTLR3_INPUT_STREAM SPLAst::antlrStreamFromString(std::string const& fileName,
                                                   std::string const& sourceString)
{
    pANTLR3_UINT8 antlrString = (pANTLR3_UINT8)sourceString.c_str();
    ANTLR3_UINT32 size = sourceString.size();
    pANTLR3_UINT8 name = (pANTLR3_UINT8)fileName.c_str();
    return antlr3NewAsciiStringCopyStream(antlrString, size, name);
}

SPLAst& SPLAst::compilationUnitFromFile(std::string const& fileName)
{
    ParserContext& pc = *new ParserContext(fileName, "");
    pANTLR3_INPUT_STREAM file = antlrStreamFromFile(fileName);
    if (!file) {
        SysOut::errorln(SPL_FRONT_END_UNABLE_TO_OPEN_READ(fileName));
        // No one will emit this string.  Just there to have something to throw
        throw "Unable to open SPL file";
    }
    SPLAst2& res = *new SPLAst2(file, pc);
    res._parser->compilationUnit(res._parser);
    AstNode* ast = pc._result;
    if (NULL == ast) {
        assert(false);
        std::vector<AstNode*> v;
        ast = &AST::compilationUnit::create(AST::splDoc::create(),
                                            AST::splNamespace::create(AST::splDoc::create()),
                                            AST::useDirectives::create(v), v);
    }
    res.setAst(ast);
    return res;
}

SPLAst& SPLAst::expressionFromString(std::string const& fileName, std::string const& sourceString)
{
    ParserContext& pc = *new ParserContext(fileName, sourceString);
    SPLAst2& res = *new SPLAst2(antlrStreamFromString(fileName, sourceString), pc);
    res._parser->splExpr(res._parser);
    AstNode* ast = pc._result;
    assert(NULL != ast);
    res.setAst(ast);
    return res;
}

SPLAst& SPLAst::functionPrototypeFromString(std::string const& fileName,
                                            uint32_t line,
                                            uint32_t column,
                                            std::string const& sourceString,
                                            bool isNativeFunction)
{
    pANTLR3_INPUT_STREAM input;
    std::string enhancedString;
    const std::string* p;
    if (column > 1) {
        enhancedString = std::string(column, ' ') + sourceString;
        p = &enhancedString;
    } else {
        p = &sourceString;
    }
    input = antlrStreamFromString(fileName, *p);
    // The following line seems to be needed in order to get the correct coordinates
    // on the nodes.  No idea why.
    input->setCharPositionInLine(input, 0);
    if (line) {
        input->setLine(input, line);
    }
    ParserContext& pCtx = *new ParserContext(fileName, *p);
    SPLAst2& res = *new SPLAst2(input, pCtx);
    res._parser->functionPrototype(res._parser);
    res.setAst(pCtx._result);
    if (indebug(Debug::ShowFunctionPrototypeAst)) {
        std::cerr << res.ast() << std::endl;
    }

    if (astHasErrorNodes(res.ast(), &pCtx)) {
        res.setAst(&AST::errorT::create());
        return res;
    }

    // Run a diagnostic pass on the function prototype if it's a native function
    if (isNativeFunction) {
        Diagnostician::run(pCtx, res.ast());
    }
    return res;
}

struct QuietParserContext : public ParserContext
{
    bool _hadError;
    QuietParserContext(std::string const& f, std::string const& s)
      : ParserContext(f, s)
      , _hadError(false)
    {}
};
static void quietDisplayRecognitionError(pANTLR3_BASE_RECOGNIZER recognizer,
                                         pANTLR3_UINT8* tokenNames)
{
    QuietParserContext* pCtx = static_cast<QuietParserContext*>(recognizer->state->userp);
    assert(NULL != pCtx);
    pCtx->_hadError = true;
}

SPLAst& SPLAst::SPLTypeFromString(std::string const& fileName,
                                  std::string const& sourceString,
                                  bool& error)
{
    QuietParserContext pCtx(fileName, sourceString);
    SPLAst2& res = *new SPLAst2(antlrStreamFromString(fileName, sourceString), pCtx);
    res._lexer->pLexer->rec->displayRecognitionError = quietDisplayRecognitionError;
    res._parser->pParser->rec->displayRecognitionError = quietDisplayRecognitionError;
    res._parser->splType(res._parser);
    error = pCtx._hadError;
    if (!error) {
        res.setAst(pCtx._result);
    }
    return res;
}

void SPLAst::tokenize(pANTLR3_TOKEN_SOURCE source)
{
    _tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, source);
}

// -----------------------------------------------------------------
struct astHash
{
    inline size_t operator()(AstNode* x) const { return reinterpret_cast<size_t>(x); }
};

struct astEquals
{
    inline bool operator()(AstNode* x, AstNode* y) const { return x == y; }
};

struct InverseAstState
{
    struct Val
    {
        int _indexInParent;
        AstNode* _parentAst;
        Val()
          : _indexInParent(-1)
          , _parentAst(NULL)
        {}
        Val(int indexInParent, AstNode& parentAst)
          : _indexInParent(indexInParent)
          , _parentAst(&parentAst)
        {}
    };
    typedef std::tr1::unordered_map<AstNode*, Val, astHash, astEquals> Map;
    Map _map;
    AstNode& _root;
    InverseAstState(AstNode& root)
      : _root(root)
    {}
    void lazyInit(AstNode& parentAst)
    {
        for (int i = 0, n = astSize(parentAst); i < n; i++) {
            AstNode& childAst = astChild(parentAst, i);
            Map::iterator it = _map.find(&childAst);
            if (it == _map.end()) {
                _map[&childAst] = Val(i, parentAst);
                lazyInit(childAst);
            }
        }
    }
};

InverseAst::InverseAst(AstNode& root)
  : _state(new InverseAstState(root))
{}

InverseAst::~InverseAst()
{
    delete _state;
    _state = NULL;
}

int InverseAst::indexInParent(AstNode& ast)
{
    _state->lazyInit(_state->_root);
    InverseAstState::Map::iterator it = _state->_map.find(&ast);
    return it == _state->_map.end() ? -1 : it->second._indexInParent;
}

AstNode* InverseAst::parent(AstNode& ast)
{
    if (&ast == &_state->_root) {
        return NULL;
    }
    _state->lazyInit(_state->_root);
    InverseAstState::Map::iterator it = _state->_map.find(&ast);
    assert(it != _state->_map.end());
    return it->second._parentAst;
}

// -----------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, AstNode* ast)
{
    if (NULL == ast) {
        return os << "NULL_AST";
    }
    return os << *ast;
}

std::ostream& operator<<(std::ostream& os, AstNode& ast)
{
    if (AST::idT::is(ast)) {
        return os << "(ID " << astText(ast) << ")";
    }
    if (NULL == ast.children || 0 == astSize(ast)) {
        return os << astText(ast);
    }
    assert(!ast.isNilNode(&ast));
    os << "(" << astText(ast);
    if (ast.children != NULL) {
        for (int i = 0, n = astSize(ast); i < n; i++) {
            os << " " << astChild(ast, i);
        }
    }
    os << ")";
    return os;
}

// -----------------------------------------------------------------
void analyzeExpressionAst(SPLAst& sa)
{
    NameDefFinder::run(sa.parserContext(), sa.ast());
    LexicalNameUseFinder::run(sa.parserContext(), sa.ast());
    RelativeNameUseFinder::run(sa.parserContext(), sa.ast());
    TypeAnalyzer::run(sa.parserContext(), sa.ast());
    TypePromoter::run(sa.parserContext(), sa.ast());
    PromotionChecker::run(sa.parserContext(), sa.ast());
    ExpressionTypeFinder::run(sa.parserContext(), sa.ast());
    EmptyContainerTypeFinder::run(sa.parserContext(), sa.ast());
    VariableUseChecker::run(sa.parserContext(), sa.ast());
    LogicSubmitChecker::run(sa.parserContext(), sa.ast());
    IRGenerator::run(sa.parserContext(), sa.ast());
}

int findAstChild(AstNode& parent, AstNode& child)
{
    int rc = -1;
    for (int i = 0, n = astSize(parent); i < n; ++i) {
        if (&child == &astChild(parent, i)) {
            return i;
        }
    }
    return rc;
}

AstNode* astNodeWithFirstLocation(AstNode& ast)
{
    if (0 != astLine(ast)) {
        return &ast;
    }
    if (NULL == ast.children || 0 == astSize(ast)) {
        return NULL;
    }
    for (int i = 0, n = astSize(ast); i < n; i++) {
        AstNode* result = astNodeWithFirstLocation(astChild(ast, i));
        if (NULL != result) {
            return result;
        }
    }
    return NULL;
}

AstNode* astChildWithLeftmostLocation(AstNode& ast)
{
    AstNode* leftmost = NULL;
    if (NULL != ast.children || 0 != astSize(ast)) {
        for (int i = 0, n = astSize(ast); i < n; i++) {
            AstNode* result = astNodeWithLeftmostLocation(astChild(ast, i));
            if (NULL != result) {
                if (NULL != leftmost && astLine(*leftmost) != 0) {
                    if ((astLine(*result) < astLine(*leftmost)) ||
                        ((astLine(*result) == astLine(*leftmost)) &&
                         (astColumn(*result) < astColumn(*leftmost)))) {
                        leftmost = result;
                    }
                } else {
                    leftmost = result;
                }
            }
        }
    }
    if (NULL == leftmost) {
        return NULL;
    }
    return (astLine(*leftmost) != 0) ? leftmost : NULL;
}

AstNode* astNodeWithLeftmostLocation(AstNode& ast)
{
    AstNode* leftmost = &ast;
    if (NULL != ast.children || 0 != astSize(ast)) {
        for (int i = 0, n = astSize(ast); i < n; i++) {
            AstNode* result = astNodeWithLeftmostLocation(astChild(ast, i));
            if (NULL != result) {
                if (astLine(*leftmost) != 0) {
                    if ((astLine(*result) < astLine(*leftmost)) ||
                        ((astLine(*result) == astLine(*leftmost)) &&
                         (astColumn(*result) < astColumn(*leftmost)))) {
                        leftmost = result;
                    }
                } else {
                    leftmost = result;
                }
            }
        }
    }
    return (astLine(*leftmost) != 0) ? leftmost : NULL;
}

AstNode* astNodeWithRightmostLocation(AstNode& ast)
{
    AstNode* rightmost = &ast;
    if (NULL != ast.children || 0 != astSize(ast)) {
        for (int i = 0, n = astSize(ast); i < n; i++) {
            AstNode* result = astNodeWithRightmostLocation(astChild(ast, i));
            if (NULL != result) {
                if (astLine(*rightmost) != 0) {
                    if ((astLine(*result) > astLine(*rightmost)) ||
                        ((astLine(*result) == astLine(*rightmost)) &&
                         (astColumn(*result) > astColumn(*rightmost)))) {
                        rightmost = result;
                    }
                } else {
                    rightmost = result;
                }
            }
        }
    }
    return (astLine(*rightmost) != 0) ? rightmost : NULL;
}

void astInitSourceFileIdx(AstPool& pool, AstNode& ast, int idx)
{
    setAstSourceFileIndex(pool, ast, idx);
    for (int i = 0, n = astSize(ast); i < n; i++) {
        astInitSourceFileIdx(pool, astChild(ast, i), idx);
    }
}

AstNode& cloneTree(AstPool& pool, AstNode& ast)
{
    AstNode& result = wrapper(ast).clone().astNode();

    for (int i = 0, n = astSize(ast); i < n; ++i) {
        AstNode& child = cloneTree(pool, astChild(ast, i));
        pool->addChild(result, child);
    }
    return result;
}

AstNode& astClone(AstPool& pool, AstNode& ast)
{
    AstNode& result = cloneTree(pool, ast);
    MODEL("ASTClone(" << (void*)&ast << ", " << (void*)&result << ")\n");
    return result;
}

bool astHasErrorNodes(AstNode& ast, ParserContext const* pCtx)
{
    if (ANTLR3_TOKEN_INVALID == ast.getType(&ast)) {
        return true;
    }
    if (NULL == ast.children || ast.isNilNode(&ast)) {
        return false;
    }
    bool subtreeHasErrors = false;
    for (int i = 0, n = astSize(ast); i < n; i++) {
        subtreeHasErrors |= astHasErrorNodes(astChild(ast, i), pCtx);
    }
    if (subtreeHasErrors) {
        return true;
    }
    if (NULL == pCtx) {
        return false;
    }
    if (AST::namespaceName::is(ast)) {
        AstNode* a = &AST::namespaceName::body(ast);
        while (AST::attributeExpr::is(*a)) {
            a = &AST::attributeExpr::base(*a);
        }
        if (!AST::idT::is(*a)) {
            SysOut::errorln(SPL_FRONT_END_INVALID_NAMESPACE_NAME,
                            SourceLocationFactory::instance().create(*pCtx, ast));
            return true;
        }
    }
    if (AST::opInvokeBody::is(ast) && 0 < astSize(ast)) {
        for (int i = 1, n = astSize(ast); i < n; i++) {
            AstNode &prev = astChild(ast, i - 1), &curr = astChild(ast, i);
            bool isCorrect;
            if (AST::logic::is(prev)) {
                isCorrect = AST::configClause::is(curr) || AST::outputClause::is(curr) ||
                            AST::actualClause::is(curr) || AST::window::is(curr);
            } else if (AST::window::is(prev)) {
                isCorrect = AST::configClause::is(curr) || AST::outputClause::is(curr) ||
                            AST::actualClause::is(curr);
            } else if (AST::actualClause::is(prev)) {
                isCorrect = AST::configClause::is(curr) || AST::outputClause::is(curr);
            } else if (AST::outputClause::is(prev)) {
                isCorrect = AST::configClause::is(curr);
            } else {
                isCorrect = false;
            }
            if (!isCorrect) {
                SysOut::errorln(
                  SPL_FRONT_END_INVALID_CLAUSE_ORDER(astText(prev), astText(curr),
                                                     "logic, window, param, output, config"),
                  SourceLocationFactory::instance().create(*pCtx, curr));
                return true;
            }
        }
    }
    if (AST::compositeBody::is(ast) && 0 < astSize(ast)) {
        for (int i = 1, n = astSize(ast); i < n; i++) {
            AstNode &prev = astChild(ast, i - 1), &curr = astChild(ast, i);
            bool isCorrect;
            if (AST::formalClause::is(prev)) {
                isCorrect = AST::configClause::is(curr) || AST::graphClause::is(curr) ||
                            AST::typeClause::is(curr);
            } else if (AST::typeClause::is(prev)) {
                isCorrect = AST::configClause::is(curr) || AST::graphClause::is(curr);
            } else if (AST::graphClause::is(prev)) {
                isCorrect = AST::configClause::is(curr);
            } else {
                isCorrect = false;
            }
            if (!isCorrect) {
                SysOut::errorln(SPL_FRONT_END_INVALID_CLAUSE_ORDER(astText(prev), astText(curr),
                                                                   "param, type, graph, config"),
                                SourceLocationFactory::instance().create(*pCtx, curr));
                return true;
            }
        }
    }
    return false;
}

bool astValidNames(AstNode& ast)
{
    if (NULL == ast.children || 0 == astSize(ast)) {
        return true;
    }
    for (int i = 0, n = astSize(ast); i < n; i++) {
        if (!astValidNames(astChild(ast, i))) {
            return false;
        }
    }
    return true;
}

Expression::Op astNodeOp(AstNode& ast)
{
    switch (ast.getType(&ast)) {
        // prefix operators
        case PLUS_PLUS:
            return Expression::plusPlus;
        case MINUS_MINUS:
            return Expression::minusMinus;
        case BANG:
            return Expression::bang;
        case TILDE:
            return Expression::tilde;
        case UNARY_MINUS:
            return Expression::unaryMinus;
            // simple infix operators
        case STAR:
            return Expression::star;
        case SLASH:
            return Expression::slash;
        case MOD:
            return Expression::mod;
        case PLUS:
            return Expression::plus;
        case MINUS:
            return Expression::minus;
        case AMP:
            return Expression::amp;
        case HAT:
            return Expression::hat;
        case BAR:
            return Expression::bar;
        case AMP_AMP:
            return Expression::ampAmp;
        case BAR_BAR:
            return Expression::barBar;
        case LESS:
            return Expression::less;
        case LEQ:
            return Expression::leq;
        case GREATER:
            return Expression::greater;
        case GEQ:
            return Expression::geq;
        case NEQ:
            return Expression::neq;
        case EQ_EQ:
            return Expression::eqEq;
        case LSHIFT:
            return Expression::lShift;
        case RSHIFT:
            return Expression::rShift;
        case IN:
            return Expression::in;
            // assignment operators
        case EQ:
            return Expression::eq;
        case STAR_EQ:
            return Expression::starEq;
        case SLASH_EQ:
            return Expression::slashEq;
        case MOD_EQ:
            return Expression::modEq;
        case PLUS_EQ:
            return Expression::plusEq;
        case MINUS_EQ:
            return Expression::minusEq;
        case AMP_EQ:
            return Expression::ampEq;
        case HAT_EQ:
            return Expression::hatEq;
        case BAR_EQ:
            return Expression::barEq;
        case LSHIFT_EQ:
            return Expression::lShiftEq;
        case RSHIFT_EQ:
            return Expression::rShiftEq;
            // dotted infix operators
        case DOT_STAR:
            return Expression::dotStar;
        case DOT_SLASH:
            return Expression::dotSlash;
        case DOT_HAT:
            return Expression::dotHat;
        case DOT_PLUS:
            return Expression::dotPlus;
        case DOT_MINUS:
            return Expression::dotMinus;
        case DOT_AMP:
            return Expression::dotAmp;
        case DOT_MOD:
            return Expression::dotMod;
        case DOT_BAR:
            return Expression::dotBar;
        case DOT_LESS:
            return Expression::dotLess;
        case DOT_LEQ:
            return Expression::dotLeq;
        case DOT_GREATER:
            return Expression::dotGreater;
        case DOT_GEQ:
            return Expression::dotGeq;
        case DOT_NEQ:
            return Expression::dotNeq;
        case DOT_EQ_EQ:
            return Expression::dotEqEq;
        case DOT_LSHIFT:
            return Expression::dotLShift;
        case DOT_RSHIFT:
            return Expression::dotRShift;
        default:
            return Expression::INVALID;
    }
}

pANTLR3_COMMON_TREE_NODE_STREAM astNodeStream(AstNode& ast)
{
    return antlr3CommonTreeNodeStreamNewTree(&ast, ANTLR3_SIZE_HINT);
}

int astSourceFileIdx(AstNode& ast)
{
    AstInformation* a = getAstInformation(ast);
    return NULL == a ? -1 : a->getSourceFileIndex();
}

FrontEndStage astStage(AstNode& ast)
{
    AstInformation* a = getAstInformation(ast);
    return NULL == a ? FrontEndStage::PARSER : a->getStage();
}

SymbolTableEntry& astSymbol(AstNode& ast)
{
    SymbolTableEntry* result = astSymbolPtr(ast);
    assert(NULL != result);
    return *result;
}

SymbolTableEntry* astSymbolPtr(AstNode& ast)
{
    AstInformation* a = getAstInformation(ast);
    return NULL == a ? NULL : a->getSymbol();
}

void copyAstPosition(AstPool& pool, AstNode& dst, AstNode& src)
{
    AstToken &dstToken = astToken(dst), &srcToken = astToken(src);
    dstToken.setLine(&dstToken, srcToken.getLine(&srcToken));
    dstToken.setCharPositionInLine(&dstToken, srcToken.getCharPositionInLine(&srcToken));
    setAstSourceFileIndex(pool, dst, astSourceFileIdx(src));
}

void copyAstSymbol(AstPool& pool, AstNode& dst, AstNode& src)
{
    if ((NULL == astSymbolPtr(dst) || astSymbol(dst).is<CompositeFormalSym>()) &&
        NULL != astSymbolPtr(src)) {
        resetAstSymbol(pool, dst, astSymbol(src));
    }
    if (astSymbolPtr(dst) != astSymbolPtr(src)) {
        std::cerr << "copyAstSymbol(" << dst << ", " << src << ") failed, " << astSymbolPtr(dst)
                  << " != " << astSymbolPtr(src) << std::endl;
    }
    assert(astSymbolPtr(dst) == astSymbolPtr(src));
}

void displayRecognitionError(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8* tokenNames)
{
    static bool anyParserErrors = false;
    if (ANTLR3_TYPE_TREE_PARSER == recognizer->type) {
        if (!anyParserErrors) {
            fatalTreeParserError(recognizer);
        }
    } else {
        assert(ANTLR3_TYPE_LEXER == recognizer->type || ANTLR3_TYPE_PARSER == recognizer->type);
        anyParserErrors = true;
        pANTLR3_EXCEPTION ex = recognizer->state->exception;
        ParserContext* parserCtx = static_cast<ParserContext*>(recognizer->state->userp);
        assert(NULL != parserCtx);
        int fileIdx = parserCtx->fileNameIdx();
        int line = ex->line;
        parserCtx->mapFileAndLine(fileIdx, line);
        SourceLocation const& loc = SourceLocationFactory::instance().create(
          fileIdx, parserCtx->sourceStringIdx(), line,
          1 + ex->charPositionInLine); // 1-based column numbers
        if (parserCtx->_lastSyntaxError == &loc) {
            return; // suppress cascading syntax errors for the same source location
        }
        parserCtx->_lastSyntaxError = &loc;
        if (ANTLR3_TYPE_LEXER == recognizer->type) {
            if (-1 == (int)ex->c) {
                SysOut::errorln(SPL_FRONT_END_LEXER_EOF, loc);
            } else {
                char s[2];
                s[0] = ex->c;
                s[1] = '\0';
                SysOut::errorln(SPL_FRONT_END_LEXER_ERROR(ex->c, s), loc);
            }
        } else {
            assert(ANTLR3_TYPE_PARSER == recognizer->type);
            pANTLR3_PARSER parser = (pANTLR3_PARSER)recognizer->super;
            AstToken* currentToken = parser->tstream->get(parser->tstream, ex->index);
            if (currentToken &&
                (FLOAT128 == currentToken->type || COMPLEX128 == currentToken->type ||
                 MATRIX == currentToken->type)) {
                SysOut::errorln(
                  SPL_FRONT_END_RESERVED_WORD(tokenToString(currentToken->type, tokenNames)), loc);
            } else {
                std::string tokenString =
                  tokenToString(currentToken ? currentToken->type : ANTLR3_TOKEN_EOF, tokenNames);
                switch (ex->type) {
                    case ANTLR3_UNWANTED_TOKEN_EXCEPTION:
                        SysOut::errorln(SPL_FRONT_END_UNWANTED_TOKEN(
                                          tokenToString(parserCtx->_paraphrase.back(), tokenNames),
                                          tokenString, tokenToString(ex->expecting, tokenNames)),
                                        loc);
                        break;
                    case ANTLR3_MISSING_TOKEN_EXCEPTION:
                        SysOut::errorln(SPL_FRONT_END_MISSING_TOKEN(
                                          tokenToString(parserCtx->_paraphrase.back(), tokenNames),
                                          tokenString, tokenToString(ex->expecting, tokenNames)),
                                        loc);
                        break;
                    case ANTLR3_RECOGNITION_EXCEPTION:
                    case ANTLR3_EARLY_EXIT_EXCEPTION:
                    case ANTLR3_MISMATCHED_TOKEN_EXCEPTION:
                    case ANTLR3_MISMATCHED_SET_EXCEPTION:
                    default:
                        SysOut::errorln(
                          SPL_FRONT_END_SYNTAX_ERROR(
                            tokenString, tokenToString(parserCtx->_paraphrase.back(), tokenNames)),
                          loc);
                        break;
                    case ANTLR3_NO_VIABLE_ALT_EXCEPTION:
                        SysOut::errorln(
                          SPL_FRONT_END_NO_VIABLE_ALTERNATIVE(
                            tokenString, tokenToString(parserCtx->_paraphrase.back(), tokenNames)),
                          loc);
                        break;
                }
            }
        }
    }
}

void findAllCalls(AstNode& ast, std::vector<AstNode*>& result)
{
    if (AST::callExpr::is(ast)) {
        result.push_back(&ast);
    } else if (NULL != ast.children) {
        for (int i = 0, n = astSize(ast); i < n; i++) {
            findAllCalls(astChild(ast, i), result);
        }
    }
}

void findAllVariables(AstNode& ast, std::vector<AstNode*>& result)
{
    if (AST::idT::is(ast) || AST::qualifierExpr::is(ast)) {
        result.push_back(&ast);
    } else if (AST::attributeExpr::is(ast)) {
        findAllVariables(AST::attributeExpr::base(ast), result);
    } else if (AST::castExpr::is(ast)) {
        findAllVariables(AST::castExpr::expr(ast), result);
    } else if (AST::callExpr::is(ast)) {
        // Special case pseudo-functions - ignore their arguments in this case
        SymbolTableEntry* sym = astSymbolPtr(AST::callExpr::target(ast));
        if (sym && sym->is<FunctionHeadSym>()) {
            FunctionHeadSym& func = sym->as<FunctionHeadSym>();
            if (func.isPseudoFunction()) {
                return;
            }
        }
        for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
            findAllVariables(AST::callExpr::actual(ast, i), result);
        }
    } else if (NULL != ast.children) {
        for (int i = 0, n = astSize(ast); i < n; i++) {
            findAllVariables(astChild(ast, i), result);
        }
    }
}

static void foundStreamInOpInvoke(int& countFound,
                                  AstNode** foundAst,
                                  AstNode& ast,
                                  bool* isInput,
                                  bool in,
                                  int* portIndex,
                                  int iPort,
                                  bool* isPortAlias,
                                  bool alias)
{
    if (0 == countFound) {
        if (NULL != foundAst) {
            *foundAst = &ast;
        }
        if (NULL != isInput) {
            *isInput = in;
        }
        if (NULL != portIndex) {
            *portIndex = iPort;
        }
        if (NULL != isPortAlias) {
            *isPortAlias = alias;
        }
    }
    ++countFound;
}

int findStreamInOpInvoke(std::string const& streamName,
                         AstNode& opInvokeAst,
                         AstNode** foundAst,
                         bool* isInput,
                         int* portIndex,
                         bool* isPortAlias)
{
    int countFound = 0;
    AstNode& headAst = AST::opInvoke::head(opInvokeAst);
    AstNode& outputsAst = AST::opInvokeHead::outputs(headAst);
    for (int iPort = 0, nPorts = AST::opOutputs::outputCount(outputsAst); iPort < nPorts; iPort++) {
        AstNode& outputAst = AST::opOutputs::output(outputsAst, iPort);
        if (astText(AST::opOutput::id(outputAst)) == streamName) {
            foundStreamInOpInvoke(countFound, foundAst, outputAst, isInput, false, portIndex, iPort,
                                  isPortAlias, false);
        }
        if (AST::opOutput::has_alias(outputAst) &&
            astText(AST::as::id(AST::opOutput::alias(outputAst))) == streamName) {
            foundStreamInOpInvoke(countFound, foundAst, outputAst, isInput, false, portIndex, iPort,
                                  isPortAlias, true);
        }
    }
    AstNode& inputsAst = AST::opInvokeHead::inputs(headAst);
    for (int iPort = 0, nPorts = AST::opInputs::portInputsCount(inputsAst); iPort < nPorts;
         iPort++) {
        AstNode& portInputsAst = AST::opInputs::portInputs(inputsAst, iPort);
        for (int iStream = 0, nStreams = AST::portInputs::idCount(portInputsAst);
             iStream < nStreams; iStream++) {
            AstNode& streamAst = AST::portInputs::id(portInputsAst, iStream);
            if (astText(streamAst) == streamName) {
                foundStreamInOpInvoke(countFound, foundAst, streamAst, isInput, true, portIndex,
                                      iPort, isPortAlias, false);
            }
        }
        if (AST::portInputs::has_alias(portInputsAst)) {
            AstNode& aliasAst = AST::portInputs::alias(portInputsAst);
            if (astText(AST::as::id(aliasAst)) == streamName) {
                foundStreamInOpInvoke(countFound, foundAst, aliasAst, isInput, true, portIndex,
                                      iPort, isPortAlias, true);
            }
        }
    }
    if (0 == countFound && NULL != foundAst) {
        *foundAst = NULL;
    }
    return countFound;
}

static bool isSplDocSeparated(pANTLR3_TOKEN_STREAM tokStream, int tokenIdx)
{
    pANTLR3_COMMON_TOKEN t = tokStream->get(tokStream, tokenIdx + 1);
    if (WHITE_SPACE == t->getType(t)) {
        char* text = (char*)t->getText(t)->chars;
        text = strstr(text, "\n");
        if (NULL != text && NULL != strstr(text + 1, "\n")) {
            return true;
        }
    }
    return false;
}

static bool isSplDocTargetTheFirstNonCommentArtifactInCompilationUnit(
  pANTLR3_TOKEN_STREAM tokStream,
  int tok)
{
    int tokenIdx = tok - 1;
    while (0 <= tokenIdx) {
        pANTLR3_COMMON_TOKEN t = tokStream->get(tokStream, tokenIdx);
        if (HIDDEN != t->channel) {
            return false;
        }
        tokenIdx--;
    }
    return true;
}

static bool hasPrecedingSplDoc(pANTLR3_TOKEN_STREAM tokStream, int tok)
{
    int tokenIdx = tok - 1;
    while (0 <= tokenIdx) {
        pANTLR3_COMMON_TOKEN t = tokStream->get(tokStream, tokenIdx);
        if (ML_COMMENT == t->getType(t)) {
            char* text = (char*)t->getText(t)->chars;
            if (0 == strncmp(text, "/**", 3)) {
                return true;
            }
        }
        tokenIdx--;
    }
    return false;
}

AstNode& grabSplDoc(AstPool& pool, pANTLR3_TOKEN_STREAM tokStream, pANTLR3_COMMON_TOKEN tok)
{
    int tokenIdx = tok->getTokenIndex(tok) - 1;
    while (0 <= tokenIdx) {
        pANTLR3_COMMON_TOKEN t = tokStream->get(tokStream, tokenIdx);
        if (ML_COMMENT == t->getType(t)) {
            char* text = (char*)t->getText(t)->chars;
            if (0 == strncmp(text, "/**", 3)) {
                bool separated = isSplDocSeparated(tokStream, tokenIdx);
                if (!separated) {
                    return AST::splDoc::create(AST::mlCommentT::create(*t));
                } else {
                    bool first = isSplDocTargetTheFirstNonCommentArtifactInCompilationUnit(
                      tokStream, tok->getTokenIndex(tok));
                    if (!first) {
                        return AST::splDoc::create(AST::mlCommentT::create(*t));
                    } else {
                        bool exists = hasPrecedingSplDoc(tokStream, tokenIdx);
                        if (exists) {
                            return AST::splDoc::create(AST::mlCommentT::create(*t));
                        }
                        break;
                    }
                }
            }
        }
        if (HIDDEN != t->channel) {
            switch (t->getType(t)) {
                case MUTABLE:
                case PUBLIC:
                case STATEFUL:
                case STATIC:
                    break;
                default:
                    return AST::splDoc::create();
            }
        }
        tokenIdx--;
    }
    return AST::splDoc::create();
}

AstNode& grabSplDocForCompilationUnit(AstPool& pool,
                                      pANTLR3_TOKEN_STREAM tokStream,
                                      pANTLR3_COMMON_TOKEN tok)
{
    int foundTokenIdx = -1;
    pANTLR3_COMMON_TOKEN foundToken = NULL;
    int tokenIdx = tok->getTokenIndex(tok) - 1;
    while (0 <= tokenIdx) {
        pANTLR3_COMMON_TOKEN t = tokStream->get(tokStream, tokenIdx);
        if (ML_COMMENT == t->getType(t)) {
            char* text = (char*)t->getText(t)->chars;
            if (0 == strncmp(text, "/**", 3)) {
                foundToken = t;
                foundTokenIdx = tokenIdx;
            }
        }
        if (HIDDEN != t->channel) {
            break;
        }
        tokenIdx--;
    }
    if (foundToken != NULL) {
        if (isSplDocSeparated(tokStream, foundTokenIdx)) {
            return AST::splDoc::create(AST::mlCommentT::create(*foundToken));
        }
        // there is an extra line between the comment and the code,
        // so this commend belongs to the compilation unit
    }
    return AST::splDoc::create();
}

bool isVarDefAst(AstNode& ast)
{
    switch (ast.getType(&ast)) {
        case FUNCTIONFORMAL:
        case LOCALDECL:
        case FORINIT:
            break;
        default:
            return false;
    }
    assert(AST::modifiers::is(astChild(ast, 0)));
    return true;
}

AstNode& opActualChild(AstNode& ast, int i)
{
    if (AST::opActualExprs::is(ast)) {
        return AST::opActualExprs::expr(ast, i);
    }
    assert(0 == i);
    return AST::opActualName::is(ast) ? AST::opActualName::name(ast) : AST::opActualType::type(ast);
}

std::ostream& prettyPrint(std::ostream& os, AstNode& ast)
{
    PrinterContext printerContext("", os);
    AstPrinter::print(printerContext, ast);
    return os;
}

std::ostream& printExpr(std::ostream& os, AstNode& ast)
{
    PrinterContext printerContext("", os);
    AstPrinter::print(printerContext, ast);
    return os;
}

void setAstSourceFileIndex(AstPool& pool, AstNode& ast, int idx)
{
    pool->ensureAstInformation(ast).setSourceFileIndex(idx);
}

void setAstStage(AstPool& pool, AstNode& ast, FrontEndStage stage)
{
    pool->ensureAstInformation(ast).setStage(stage);
    SPLDBG(__PRETTY_FUNCTION__ << ": setting " << (void*)&ast << " to " << astStage(ast).toString(),
           Debug::TraceAstStageChanges);
}

void clearAstSymbol(AstPool& pool, AstNode& ast)
{
    AstInformation& a = pool->ensureAstInformation(ast);
    a.setSymbol(NULL);
    MODEL("ClearSymbol(" << (void*)&ast << ")\n");
}

void setAstSymbol(AstPool& pool, AstNode& ast, SymbolTableEntry& sym)
{
    AstInformation& a = pool->ensureAstInformation(ast);
    SymbolTableEntry* oldSym = a.getSymbol();
    if (NULL != oldSym && oldSym != &sym) {
        std::cerr << "setAstSymbol non-null, ast " << ast << ", oldSym " << *oldSym << ", newSym "
                  << sym << std::endl;
        assert(false);
    }
    a.setSymbol(&sym);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateSymbol: ast: " << (void*)&ast
                               << " sym: " << (void*)&sym,
           Debug::ShowSymbols);
    MODEL("AssociateSymbol(" << (void*)&ast << ", " << (void*)&sym << ")\n");
}

void resetAstSymbol(AstPool& pool, AstNode& ast, SymbolTableEntry& sym)
{
    if (sym.is<ErrorDummySym>()) {
        SPLDBG(__PRETTY_FUNCTION__ << ": AssociateErrorSymbol: ast: " << (void*)&ast
                                   << " sym: " << (void*)&sym,
               Debug::ShowSymbols);
        MODEL("AssociateErrorSymbol(" << (void*)&ast << ", " << (void*)&sym << ")\n");
    } else {
        SPLDBG(__PRETTY_FUNCTION__ << ": ReassociateSymbol: ast: " << (void*)&ast
                                   << " sym: " << (void*)&sym,
               Debug::ShowSymbols);
        MODEL("ReassociateSymbol(" << (void*)&ast << ", " << (void*)&sym << ")\n");
    }
    pool->ensureAstInformation(ast).setSymbol(&sym);
}

Statement* getAstStatement(AstNode& ast)
{
    AstInformation* a = getAstInformation(ast);
    return NULL == a ? NULL : a->getStatement();
}

void setAstStatement(AstPool& pool, AstNode& ast, Statement& s)
{
    pool->ensureAstInformation(ast).setStatement(&s);
}

Expression& astExpression(AstNode& ast)
{
    Expression* result = astExpressionPtr(ast);
    assert(NULL != result);
    return *result;
}

Expression* astExpressionPtr(AstNode& ast)
{
    AstInformation* a = getAstInformation(ast);
    return NULL == a ? NULL : a->getExpression();
}

void clearAstExpression(AstPool& pool, AstNode& ast)
{
    AstInformation& a = pool->ensureAstInformation(ast);
    MODEL("ClearExpression(" << (void*)&ast << ")\n");
    a.setExpression(NULL);
}

void setAstExpression(AstPool& pool, AstNode& ast, Expression& e)
{
    AstInformation& a = pool->ensureAstInformation(ast);
    assert(a.getExpression() == NULL);
    a.setExpression(&e);
}

void resetAstExpression(AstPool& pool, AstNode& ast, Expression& e)
{
    AstInformation& a = pool->ensureAstInformation(ast);
    assert(a.getExpression() != NULL);
    a.setExpression(&e);
}

AstNode& createTupleLiteral(AstPool& pool,
                            AstToken& start,
                            std::vector<SPL::AstNode*>& v,
                            ParserContext const& ctx)
{
    std::vector<SPL::AstNode*> assigns;
    std::tr1::unordered_set<std::string> attrNames;
    // Make sure all the sub-expressions are assignments
    // While we're at it, make attributeAssign from them
    for (int i = 0, size = v.size(); i < size; ++i) {
        AstNode* node = v[i];
        if (!SPL::AST::eqT::is(*node)) {
            SysOut::errorln(SPL_FRONT_END_INVALID_TUPLE_LITERAL_ELEMENT,
                            SourceLocationFactory::instance().create(ctx, *node));
            return SPL::AST::errorT::create();
        }
        AstNode& id = SPL::AST::infixExpr::lhs(*node);
        // Check to make sure the attribute id is of type ID
        if (id.getType(&id) != ID) {
            SysOut::errorln(SPL_FRONT_END_INVALID_TUPLE_ATTRIBUTE_NAME(astText(id)),
                            SourceLocationFactory::instance().create(ctx, *node));
            return SPL::AST::errorT::create();
        }
        // Check for repeated attribute names
        if (attrNames.insert(astText(id)).second == false) {
            SysOut::errorln(SPL_FRONT_END_REPEATED_TUPLE_ATTRIBUTE_NAME(astText(id)),
                            SourceLocationFactory::instance().create(ctx, *node));
            return SPL::AST::errorT::create();
        }
        AstNode& expr = SPL::AST::infixExpr::rhs(*node);
        AstToken& token = astToken(*node);
        AstNode& assign = SPL::AST::attributeAssign::create(token, id, expr);
        assigns.push_back(&assign);
        MODEL("RemoveASTNode(" << (void*)node << ")\n");
    }
    return SPL::AST::tupleLiteral::create(start, assigns);
}

AstNode& createAnnotationLiteral(AstPool& pool,
                                 AstToken& start,
                                 std::vector<SPL::AstNode*>& v,
                                 ParserContext const& ctx)
{
    std::vector<SPL::AstNode*> assigns;
    // Make sure all the sub-expressions are assignments
    // While we're at it, make attributeAssign from them
    for (int i = 0, size = v.size(); i < size; ++i) {
        AstNode* node = v[i];
        if (!SPL::AST::eqT::is(*node)) {
            SysOut::errorln(SPL_FRONT_END_INVALID_ANNOTATION_ELEMENT,
                            SourceLocationFactory::instance().create(ctx, *node));
            return SPL::AST::errorT::create();
        }
        AstNode& id = SPL::AST::infixExpr::lhs(*node);
        AstNode& expr = SPL::AST::infixExpr::rhs(*node);
        AstToken& token = astToken(*node);
        AstNode& assign = SPL::AST::attributeAssign::create(token, id, expr);
        assigns.push_back(&assign);
        MODEL("RemoveASTNode(" << (void*)node << ")\n");
    }
    return SPL::AST::tupleLiteral::create(start, assigns);
}

AstNode& createMapLiteral(AstPool& pool,
                          AstToken& start,
                          std::vector<SPL::AstNode*>& v,
                          ParserContext const& ctx)
{
    for (int i = 0, size = v.size(); i < size; ++i) {
        AstNode* node = v[i];
        if (!SPL::AST::mapping::is(*node)) {
            SysOut::errorln(SPL_FRONT_END_INVALID_MAP_LITERAL_ELEMENT,
                            SourceLocationFactory::instance().create(ctx, *node));
            return SPL::AST::errorT::create();
        }
    }
    return SPL::AST::mapLiteral::create(start, v);
}

AstNode& createCurlyLiteral(AstPool& pool,
                            AstToken& start,
                            std::vector<SPL::AstNode*>& v,
                            ParserContext const& ctx)
{
    if (v.empty()) {
        return SPL::AST::emptyCurlyLiteralT::create(start);
    }

    // We need to distinguish if the literal is tuple, set or map.  The
    // sub-expressions need to be homogenous also or it is an error,
    // so the first child can determine the type
    AstNode* node = v[0];
    if (SPL::AST::infixExpr::is(*node)) {
        return createTupleLiteral(pool, start, v, ctx);
    }
    if (SPL::AST::mapping::is(*node)) {
        return createMapLiteral(pool, start, v, ctx);
    }
    return SPL::AST::setLiteral::create(start, v);
}

AstNode& prefixOrPrependMinus(AstPool& pool,
                              AstToken& start,
                              AstNode& prefixOp,
                              AstNode& prefixExpr)
{
    if (AST::unaryMinusT::is(prefixOp)) {
        bool needsPrependMinus = false;
        if (AST::floatLiteralT::is(prefixExpr)) {
            needsPrependMinus = true;
        } else if (AST::intLiteralT::is(prefixExpr) || AST::hexLiteralT::is(prefixExpr)) {
            std::string spelling = astText(prefixExpr);
            if (std::string::npos == spelling.find_first_of("uU")) {
                needsPrependMinus = true;
            }
        }
        if (needsPrependMinus) {
            MODEL("RemoveASTNode(" << (void*)&prefixOp << ")\n");
            MODEL("RemoveASTNode(" << (void*)&prefixExpr << ")\n");
            std::ostringstream os;
            os << "-" << astText(prefixExpr);
            AstNode* node = NULL;
            if (AST::hexLiteralT::is(prefixExpr)) {
                node = &AST::hexLiteralT::create(os.str());
            } else if (AST::floatLiteralT::is(prefixExpr)) {
                node = &AST::floatLiteralT::create(os.str());
            } else if (AST::intLiteralT::is(prefixExpr)) {
                node = &AST::intLiteralT::create(os.str());
            }
            assert(NULL != node);
            AstToken* toToken = node->getToken(node);
            toToken->setLine(toToken, prefixOp.getLine(&prefixOp));
            toToken->setCharPositionInLine(toToken, prefixOp.getCharPositionInLine(&prefixOp));
            return *node;
        }
    }
    return AST::prefixExpr::create(start, prefixOp, prefixExpr);
}

AstNode& addAstChild(AstPool& pool, AstNode& parent, AstNode& child)
{
    return pool->addChild(parent, child);
}

void resetAstChild(AstPool& pool, AstNode& parent, int idx, AstNode& newChild)
{
    assert(0 <= idx && idx < astSize(parent));
    pool->setChild(parent, idx, newChild);
    assert(&newChild == &astChild(parent, idx));
}

void resetAstChildren(AstPool& pool, AstNode& parent, std::vector<AstNode*>& newChildren)
{
    assert(astSize(parent) <= (int)newChildren.size());
    for (int i = 0, n = astSize(parent); i < n; i++) {
        resetAstChild(pool, parent, i, *newChildren[i]);
    }
    for (int i = astSize(parent); i < (int)newChildren.size(); i++) {
        pool->addChild(parent, *newChildren[i]);
    }
    assert(astSize(parent) == (int)newChildren.size());
}
}
