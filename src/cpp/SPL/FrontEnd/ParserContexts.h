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

#ifndef SPL_PARSER_CONTEXTS_H
#define SPL_PARSER_CONTEXTS_H

#include <SPL/FrontEnd/SPLParser.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <iostream>
#include <vector>

namespace SPL {

class ParaphrasePusher
{
  public:
    ParaphrasePusher(SPL::ParserContext& parserState, int syntaxType)
      : _parserState(parserState)
    {
        parserState._paraphrase.push_back(syntaxType);
    }

    ~ParaphrasePusher() { _parserState._paraphrase.pop_back(); }

  private:
    SPL::ParserContext& _parserState;
};

/// The base class for the specific production context classes
class ContextBase
{
  public:
    virtual ~ContextBase();

    /// This is called at the end of a production to provide a single "return" value.
    virtual void addChild(AstNode& ast) { assert(false); }

    /// This is called at the end of a production to provide multiple "return" values.
    virtual void addChild(const std::vector<AstNode*>& v) { assert(false); }

    /// This is called at an arbitrary location to extract any SPL doc markup
    void grabSplDoc(pANTLR3_TOKEN_STREAM tokStream, AstToken& startTok);

    /// This is called to find the SPL doc associated with the compilation unit
    void grabSplDocForCompilationUnit(pANTLR3_TOKEN_STREAM tokStream, AstToken& startTok);

    /// Set the production's state
    void operator<<(unsigned val)
    {
        _state = val;
        stateChangeNotification();
    }

  protected:
    ContextBase(SPL::ParserContext& parserState, AstToken& startTok, unsigned returnValueCount);

    SPL::AstPool& pool() const { return _pool; }
    SPL::ParserContext& parseState() const { return _parserState; }
    SPL::ContextBase& previousContext() const
    {
        assert(NULL != _previousContext);
        return *_previousContext;
    }
    AstNode& errorNode();
    AstToken& startToken() const { return _startToken; }
    unsigned state() const { return _state; }
    AstNode& splDoc(pANTLR3_TOKEN_STREAM tokStream) const;
    AstNode& splDocForCompilationUnit(pANTLR3_TOKEN_STREAM tokStream) const;
    void setReturn(AstNode& ret, unsigned i = 0)
    {
        assert(_returnValueCount > i);
        _return[i] = &ret;
    }
    void setReturn(const std::vector<AstNode*>& ret)
    {
        assert(_returnValueCount > 1);
        _return = ret;
    }
    virtual void stateChangeNotification() {}

  private:
    SPL::ParserContext& _parserState;
    AstToken& _startToken;
    unsigned _returnValueCount;
    SPL::AstPool& _pool;
    ContextBase* _previousContext;
    unsigned _state;
    std::vector<AstNode*> _return;
};

class CompilationUnitContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    CompilationUnitContext(SPL::ParserContext& parserState,
                           pANTLR3_TOKEN_STREAM tokStream,
                           AstToken& startTok,
                           int syntaxType)
      : ContextBase(parserState, startTok, 0)
      , ParaphrasePusher(parserState, syntaxType)
      , _tokStream(tokStream)
      , _splNamespace(NULL)
    {}
    virtual ~CompilationUnitContext();
    virtual void addChild(AstNode& ast);

  private:
    pANTLR3_TOKEN_STREAM _tokStream;
    AstNode* _splNamespace;
    AstNode* _useDirectives;
    std::vector<SPL::AstNode*> _definitions;
};

class NamespaceContext : public ContextBase
{
  public:
    NamespaceContext(SPL::ParserContext& parserState,
                     pANTLR3_TOKEN_STREAM tokStream,
                     AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _tokStream(tokStream)
      , _namespaceName(NULL)
    {}
    virtual ~NamespaceContext();
    virtual void addChild(AstNode& ast);

  private:
    pANTLR3_TOKEN_STREAM _tokStream;
    AstNode* _namespaceName;
};

class NamespaceNameContext : public ContextBase
{
  public:
    NamespaceNameContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _id(NULL)
    {}
    virtual ~NamespaceNameContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _id;
};

class UseDirectivesContext : public ContextBase
{
  public:
    UseDirectivesContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~UseDirectivesContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<SPL::AstNode*> _useDirectives;
};

class UseDirectiveContext : public ContextBase
{
  public:
    UseDirectiveContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _namespaceName(NULL)
      , _useDirectiveTail(NULL)
    {}
    virtual ~UseDirectiveContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _namespaceName;
    AstNode* _useDirectiveTail;
};

// The following are not complete
class QualifiedNameContext : public ContextBase
{
  public:
    QualifiedNameContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _namespaceName(NULL)
      , _identifier(NULL)
    {}
    virtual ~QualifiedNameContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _namespaceName;
    AstNode* _identifier;
};

class CompositeDefContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    CompositeDefContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _head(NULL)
      , _body(NULL)
      , _rCurly(NULL)
    {}
    virtual ~CompositeDefContext();
    virtual void addChild(AstNode& ast);
    virtual void addChild(const std::vector<AstNode*>& v);

  private:
    AstNode* _head;
    AstNode* _body;
    AstNode* _rCurly;
};

class FunctionDefContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    FunctionDefContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _functionHead(NULL)
      , _blockStmtNoScope(NULL)
    {}
    virtual ~FunctionDefContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _functionHead;
    AstNode* _blockStmtNoScope;
};

class IdentifierContext : public ContextBase
{
  public:
    IdentifierContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~IdentifierContext();
};

class StarContext : public ContextBase
{
  public:
    StarContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~StarContext();

  private:
};

class CompositeHeadContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    CompositeHeadContext(SPL::ParserContext& parserState,
                         pANTLR3_TOKEN_STREAM tokStream,
                         AstToken& startTok,
                         int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _tokStream(tokStream)
      , _annotations(NULL)
      , _modifiers(NULL)
      , _compositeT(NULL)
      , _id(NULL)
      , _compositeOut(NULL)
      , _compositeIn(NULL)
    {}
    virtual ~CompositeHeadContext();
    virtual void addChild(AstNode& ast);
    virtual void addChild(const std::vector<AstNode*>& v);

  private:
    pANTLR3_TOKEN_STREAM _tokStream;
    AstNode* _annotations;
    AstNode* _modifiers;
    AstNode* _compositeT;
    AstNode* _id;
    AstNode* _compositeOut;
    AstNode* _compositeIn;
};

class CompositeInOutContext : public ContextBase
{
  public:
    CompositeInOutContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 2)
      , _compositeOut(NULL)
      , _compositeIn(NULL)
    {}
    virtual ~CompositeInOutContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _compositeOut;
    AstNode* _compositeIn;
};

class CompositeInContext : public ContextBase
{
  public:
    CompositeInContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~CompositeInContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _compositePorts;
};

class CompositeOutContext : public ContextBase
{
  public:
    CompositeOutContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~CompositeOutContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _compositePorts;
};

class CompositeModifiersContext : public ContextBase
{
  public:
    CompositeModifiersContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~CompositeModifiersContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _modifiers;
};

class PublicContext : public ContextBase
{
  public:
    PublicContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~PublicContext();
};

class EnumContext : public ContextBase
{
  public:
    EnumContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~EnumContext();
};

class ListContext : public ContextBase
{
  public:
    ListContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~ListContext();
};

class MapContext : public ContextBase
{
  public:
    MapContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~MapContext();
};

class SetContext : public ContextBase
{
  public:
    SetContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~SetContext();
};

class XMLContext : public ContextBase
{
  public:
    XMLContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~XMLContext();
};

class TupleContext : public ContextBase
{
  public:
    TupleContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~TupleContext();
};

class CompositeContext : public ContextBase
{
  public:
    CompositeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~CompositeContext();
};

class StaticContext : public ContextBase
{
  public:
    StaticContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~StaticContext();
};

class VoidContext : public ContextBase
{
  public:
    VoidContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~VoidContext();
};

class MutableContext : public ContextBase
{
  public:
    MutableContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~MutableContext();
};

class StatefulContext : public ContextBase
{
  public:
    StatefulContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~StatefulContext();
};

class CompositePortContext : public ContextBase
{
  public:
    CompositePortContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _streamType(NULL)
      , _id(NULL)
    {}
    virtual ~CompositePortContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _streamType;
    AstNode* _id;
};

class StreamTypeContext : public ContextBase
{
  public:
    StreamTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _tupleArgs(NULL)
    {}
    virtual ~StreamTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _tupleArgs;
};

class CompositeBodyContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    CompositeBodyContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 2)
      , ParaphrasePusher(parserState, syntaxType)
      , _rCurly(NULL)
    {}
    virtual ~CompositeBodyContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _compositeBodyClause;
    AstNode* _rCurly;
};

class FormalClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    FormalClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~FormalClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _compositeFormals;
};

class TypeClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    TypeClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~TypeClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _typeDefs;
};

class GraphClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    GraphClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~GraphClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _operatorInvocations;
};

class ConfigContext : public ContextBase
{
  public:
    ConfigContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _id(NULL)
    {}
    virtual ~ConfigContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _id;
    std::vector<AstNode*> _exprs;
};

class CompositeFormalContext : public ContextBase
{
  public:
    CompositeFormalContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expressionMode(NULL)
      , _id(NULL)
      , _compositeFormalInit(NULL)
    {}
    virtual ~CompositeFormalContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expressionMode;
    AstNode* _id;
    AstNode* _compositeFormalInit;
};

class OpInvokeContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    OpInvokeContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _opInvokeHead(NULL)
      , _opInvokeBody(NULL)
      , _rCurly(NULL)
    {}
    virtual ~OpInvokeContext();
    virtual void addChild(AstNode& ast);
    virtual void addChild(const std::vector<AstNode*>& v);

  private:
    AstNode* _opInvokeHead;
    AstNode* _opInvokeBody;
    AstNode* _rCurly;
};

class RCurlyContext : public ContextBase
{
  public:
    RCurlyContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~RCurlyContext();
};

class SemiContext : public ContextBase
{
  public:
    SemiContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~SemiContext();
};

class ExpressionModeContext : public ContextBase
{
  public:
    ExpressionModeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _type(NULL)
      , _typeArgs(NULL)
    {}
    virtual ~ExpressionModeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _type;
    AstNode* _typeArgs;
};

class AttributeTContext : public ContextBase
{
  public:
    AttributeTContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~AttributeTContext();
};

class ExpressionTContext : public ContextBase
{
  public:
    ExpressionTContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~ExpressionTContext();
};

class FunctionTContext : public ContextBase
{
  public:
    FunctionTContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~FunctionTContext();
};

class OperatorTContext : public ContextBase
{
  public:
    OperatorTContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~OperatorTContext();
};

class TypeTContext : public ContextBase
{
  public:
    TypeTContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~TypeTContext();
};

class CompositeFormalInitContext : public ContextBase
{
  public:
    CompositeFormalInitContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _opActual(NULL)
    {}
    virtual ~CompositeFormalInitContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _opActual;
};

class OpInvokeActualContext : public ContextBase
{
  public:
    OpInvokeActualContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _id(NULL)
      , _opActual(NULL)
    {}
    virtual ~OpInvokeActualContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _id;
    AstNode* _opActual;
};

class ActualClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    ActualClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~ActualClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _opInvokeActual;
};

class OpInvokeBodyContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    OpInvokeBodyContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 2)
      , ParaphrasePusher(parserState, syntaxType)
      , _rCurly(NULL)
    {}
    virtual ~OpInvokeBodyContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _opInvokeBodyClause;
    AstNode* _rCurly;
};

class LogicClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    LogicClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~LogicClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _opInvokeLogic;
};

class WindowClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    WindowClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~WindowClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _opInvokeWindow;
};

class OutputClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    OutputClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~OutputClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _opInvokeOutput;
};

class ConfigClauseContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    ConfigClauseContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
    {}
    virtual ~ConfigClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _config;
};

class TypeDefContext : public ContextBase
{
  public:
    TypeDefContext(SPL::ParserContext& parserState,
                   pANTLR3_TOKEN_STREAM tokStream,
                   AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _tokStream(tokStream)
      , _typeModifiers(NULL)
      , _identifier(NULL)
      , _typeDefTail(NULL)
      , _semi(NULL)
    {}
    virtual ~TypeDefContext();
    virtual void addChild(AstNode& ast);

  private:
    pANTLR3_TOKEN_STREAM _tokStream;
    AstNode* _typeModifiers;
    AstNode* _identifier;
    AstNode* _typeDefTail;
    AstNode* _semi;
};

class StandaloneTypeDefContext : public ContextBase
{
  public:
    StandaloneTypeDefContext(SPL::ParserContext& parserState,
                             pANTLR3_TOKEN_STREAM tokStream,
                             AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _tokStream(tokStream)
      , _identifier(NULL)
      , _typeDefTail(NULL)
      , _semi(NULL)
    {}
    virtual ~StandaloneTypeDefContext();
    virtual void addChild(AstNode& ast);

  private:
    pANTLR3_TOKEN_STREAM _tokStream;
    AstNode* _identifier;
    AstNode* _typeDefTail;
    AstNode* _semi;
};

class OpInvokeWindowContext : public ContextBase
{
  public:
    OpInvokeWindowContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
    {}
    virtual ~OpInvokeWindowContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
    std::vector<AstNode*> _exprs;
};

class OpOutputContext : public ContextBase
{
  public:
    OpOutputContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _streamType(NULL)
      , _identifier(NULL)
      , _asAlias(NULL)
    {}
    virtual ~OpOutputContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _streamType;
    AstNode* _identifier;
    AstNode* _asAlias;
};

class OpOutputsContext : public ContextBase
{
  public:
    OpOutputsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~OpOutputsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _opOutput_ps;
};

class OpActualTypeContext : public ContextBase
{
  public:
    OpActualTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _type(NULL)
    {}
    virtual ~OpActualTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _type;
};

class OpActualNameContext : public ContextBase
{
  public:
    OpActualNameContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeNameContents(NULL)
    {}
    virtual ~OpActualNameContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeNameContents;
};

class OpActualExprsContext : public ContextBase
{
  public:
    OpActualExprsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~OpActualExprsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _exprs;
};

class OpInvokeHeadContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    OpInvokeHeadContext(SPL::ParserContext& parserState,
                        pANTLR3_TOKEN_STREAM tokStream,
                        AstToken& startTok,
                        int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _tokStream(tokStream)
      , _annotations(NULL)
      , _opOutputs(NULL)
      , _asAlias(NULL)
      , _opName(NULL)
      , _opInputs(NULL)
    {}
    virtual ~OpInvokeHeadContext();
    virtual void addChild(AstNode& ast);

  private:
    pANTLR3_TOKEN_STREAM _tokStream;
    AstNode* _annotations;
    AstNode* _opOutputs;
    AstNode* _asAlias;
    AstNode* _opName;
    AstNode* _opInputs;
};

class OpInputsContext : public ContextBase
{
  public:
    OpInputsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~OpInputsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _portInputs_ps;
};

class PortInputsContext : public ContextBase
{
  public:
    PortInputsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _streamType(NULL)
      , _asAlias(NULL)
    {}
    virtual ~PortInputsContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _streamType;
    std::vector<AstNode*> _id_pc;
    AstNode* _asAlias;
};

class AsAliasContext : public ContextBase
{
  public:
    AsAliasContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
    {}
    virtual ~AsAliasContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
};

class OpInvokeOutputContext : public ContextBase
{
  public:
    OpInvokeOutputContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
    {}
    virtual ~OpInvokeOutputContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
    std::vector<AstNode*> _outputAssign_pc;
};

class OutputAssignContext : public ContextBase
{
  public:
    OutputAssignContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
      , _eq(NULL)
      , _expr(NULL)
    {}
    virtual ~OutputAssignContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
    AstNode* _eq;
    AstNode* _expr;
};

class OpEqualContext : public ContextBase
{
  public:
    OpEqualContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~OpEqualContext();
};

class OpInvokeStateContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    OpInvokeStateContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _decl(NULL)
    {}
    virtual ~OpInvokeStateContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _decl;
};

class KeySTATEContext : public ContextBase
{
  public:
    KeySTATEContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
    {}
    virtual ~KeySTATEContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
};

class OpInvokeOnProcessContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    OpInvokeOnProcessContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _stmt(NULL)
    {}
    virtual ~OpInvokeOnProcessContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _stmt;
};

class KeyONPROCESSContext : public ContextBase
{
  public:
    KeyONPROCESSContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
    {}
    virtual ~KeyONPROCESSContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
};

class OpInvokeOnTuple : public ContextBase
{
  public:
    OpInvokeOnTuple(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
      , _stmt(NULL)
    {}
    virtual ~OpInvokeOnTuple();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
    AstNode* _stmt;
};

class OpInvokeOnPunct : public ContextBase
{
  public:
    OpInvokeOnPunct(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
      , _stmt(NULL)
    {}
    virtual ~OpInvokeOnPunct();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
    AstNode* _stmt;
};

#if 0
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
class OpInvokeOnWatermark : public ContextBase
{
public:
    OpInvokeOnWatermark(SPL::ParserContext & parserState, AstToken & startTok)
        : ContextBase(parserState, startTok, 1), _identifier(NULL), _stmt(NULL) { }
    virtual ~OpInvokeOnWatermark();
    virtual void addChild(AstNode & ast);
private:
    AstNode * _identifier;
    AstNode * _stmt;
};
#endif

class FunctionHeadContext : public ContextBase
{
  public:
    FunctionHeadContext(SPL::ParserContext& parserState,
                        pANTLR3_TOKEN_STREAM tokStream,
                        AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _tokStream(tokStream)
      , _functionModifiers(NULL)
      , _returnType(NULL)
      , _identifier(NULL)
    {}
    virtual ~FunctionHeadContext();
    virtual void addChild(AstNode& ast);

  private:
    pANTLR3_TOKEN_STREAM _tokStream;
    AstNode* _functionModifiers;
    AstNode* _returnType;
    AstNode* _identifier;
    std::vector<AstNode*> _functionFormal_pc;
};

class FunctionPrototypeContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    FunctionPrototypeContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 0)
      , ParaphrasePusher(parserState, syntaxType)
      , _typeFormals(NULL)
      , _boundsFormals(NULL)
      , _functionModifiers(NULL)
      , _returnType(NULL)
      , _identifier(NULL)
      , _protoFormals(NULL)
    {}
    virtual ~FunctionPrototypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeFormals;
    AstNode* _boundsFormals;
    AstNode* _functionModifiers;
    AstNode* _returnType;
    AstNode* _identifier;
    AstNode* _protoFormals;
};

class TypeFormalsContext : public ContextBase
{
  public:
    TypeFormalsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~TypeFormalsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _typeFormal_pc;
};

class TypeFormalContext : public ContextBase
{
  public:
    TypeFormalContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeFormalConstraint(NULL)
      , _identifier(NULL)
    {}
    virtual ~TypeFormalContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeFormalConstraint;
    AstNode* _identifier;
};

class BoundsFormalsContext : public ContextBase
{
  public:
    BoundsFormalsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BoundsFormalsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _id_pc;
};

class FunctionModifiersContext : public ContextBase
{
  public:
    FunctionModifiersContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~FunctionModifiersContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _functionModifier;
};

class FunctionFormalContext : public ContextBase
{
  public:
    FunctionFormalContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _formalModifiers(NULL)
      , _type(NULL)
      , _identifier(NULL)
    {}
    virtual ~FunctionFormalContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _formalModifiers;
    AstNode* _type;
    AstNode* _identifier;
};

class ProtoFormalsContext : public ContextBase
{
  public:
    ProtoFormalsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~ProtoFormalsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _protoFormal_pc;
};

class ProtoFormalContext : public ContextBase
{
  public:
    ProtoFormalContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _formalModifiers(NULL)
      , _type(NULL)
      , _identifier(NULL)
    {}
    virtual ~ProtoFormalContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _formalModifiers;
    AstNode* _type;
    AstNode* _identifier;
};

class FormalModifiersContext : public ContextBase
{
  public:
    FormalModifiersContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~FormalModifiersContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _formalModifier;
};

class LocalDeclsContext : public ContextBase
{
  public:
    LocalDeclsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~LocalDeclsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _localDecl;
};

class LocalDeclContext : public ContextBase
{
  public:
    LocalDeclContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _localModifiers(NULL)
      , _type(NULL)
    {}
    virtual ~LocalDeclContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _localModifiers;
    AstNode* _type;
    std::vector<AstNode*> _localDeclItem_pc;
};

class LocalModifiersContext : public ContextBase
{
  public:
    LocalModifiersContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~LocalModifiersContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _localModifier;
};

class LocalDeclItemContext : public ContextBase
{
  public:
    LocalDeclItemContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
      , _eqExpr_q(NULL)
    {}
    virtual ~LocalDeclItemContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
    AstNode* _eqExpr_q;
};

class StmtContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    StmtContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _child(NULL)
    {}
    virtual ~StmtContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _child;
};

class ReturnStmtContext : public ContextBase
{
  public:
    ReturnStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
    {}
    virtual ~ReturnStmtContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
};

class ContinueStmtContext : public ContextBase
{
  public:
    ContinueStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~ContinueStmtContext();
};

class BreakStmtContext : public ContextBase
{
  public:
    BreakStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BreakStmtContext();
};

class WhileStmtContext : public ContextBase
{
  public:
    WhileStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
      , _stmt(NULL)
    {}
    virtual ~WhileStmtContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
    AstNode* _stmt;
};

class ForStmtContext : public ContextBase
{
  public:
    ForStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _forInit(NULL)
      , _stmtNoScope(NULL)
    {}
    virtual ~ForStmtContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _forInit;
    AstNode* _stmtNoScope;
};

class IfStmtContext : public ContextBase
{
  public:
    IfStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
      , _stmt(NULL)
      , _elseClause(NULL)
    {}
    virtual ~IfStmtContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
    AstNode* _stmt;
    AstNode* _elseClause;
};

class ExprStmtContext : public ContextBase
{
  public:
    ExprStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
    {}
    virtual ~ExprStmtContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
};

class ElseClauseContext : public ContextBase
{
  public:
    ElseClauseContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _stmt(NULL)
    {}
    virtual ~ElseClauseContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _stmt;
};

class ForInitContext : public ContextBase
{
  public:
    ForInitContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _type(NULL)
      , _identifier(NULL)
      , _expr(NULL)
    {}
    virtual ~ForInitContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _type;
    AstNode* _identifier;
    AstNode* _expr;
};

class ExprContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    ExprContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _assignExpr(NULL)
    {}
    virtual ~ExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _assignExpr;
};

class AssignExprContext : public ContextBase
{
  public:
    AssignExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _condExpr(NULL)
      , _assignOp(NULL)
    {}
    virtual ~AssignExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _condExpr;
    AstNode* _assignOp;
};

class CondExprContext : public ContextBase
{
  public:
    CondExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _logicOrExpr(NULL)
      , _condExpr(NULL)
    {}
    virtual ~CondExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _logicOrExpr;
    AstNode* _condExpr;
};

class LogicOrExprContext : public ContextBase
{
  public:
    LogicOrExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _logicAndExpr(NULL)
      , _barBar(NULL)
    {}
    virtual ~LogicOrExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _logicAndExpr;
    AstNode* _barBar;
};

class BarBarContext : public ContextBase
{
  public:
    BarBarContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BarBarContext();
};

class LogicAndExprContext : public ContextBase
{
  public:
    LogicAndExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _memberExpr(NULL)
      , _ampAmp(NULL)
    {}
    virtual ~LogicAndExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _memberExpr;
    AstNode* _ampAmp;
};

class AmpAmpContext : public ContextBase
{
  public:
    AmpAmpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~AmpAmpContext();
};

class MemberExprContext : public ContextBase
{
  public:
    MemberExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _bitOrExpr(NULL)
      , _in(NULL)
    {}
    virtual ~MemberExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _bitOrExpr;
    AstNode* _in;
};

class InContext : public ContextBase
{
  public:
    InContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~InContext();
};

class BitOrExprContext : public ContextBase
{
  public:
    BitOrExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _bitXorExpr(NULL)
      , _bitOrOp(NULL)
    {}
    virtual ~BitOrExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _bitXorExpr;
    AstNode* _bitOrOp;
};

class BitXorExprContext : public ContextBase
{
  public:
    BitXorExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _bitAndExpr(NULL)
      , _bitXorOp(NULL)
    {}
    virtual ~BitXorExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _bitAndExpr;
    AstNode* _bitXorOp;
};

class BitAndExprContext : public ContextBase
{
  public:
    BitAndExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _compareExpr(NULL)
      , _bitAndOp(NULL)
    {}
    virtual ~BitAndExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _compareExpr;
    AstNode* _bitAndOp;
};

class CompareExprContext : public ContextBase
{
  public:
    CompareExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _shiftExpr(NULL)
      , _compareOp(NULL)
    {}
    virtual ~CompareExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _shiftExpr;
    AstNode* _compareOp;
};

class ShiftExprContext : public ContextBase
{
  public:
    ShiftExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _addExpr(NULL)
      , _shiftOp(NULL)
    {}
    virtual ~ShiftExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _addExpr;
    AstNode* _shiftOp;
};

class AddExprContext : public ContextBase
{
  public:
    AddExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _multExpr(NULL)
      , _addOp(NULL)
    {}
    virtual ~AddExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _multExpr;
    AstNode* _addOp;
};

class MultExprContext : public ContextBase
{
  public:
    MultExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _prefixExpr(NULL)
      , _multOp(NULL)
    {}
    virtual ~MultExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _prefixExpr;
    AstNode* _multOp;
};

class PrefixOrUnaryMinusContext : public ContextBase
{
  public:
    PrefixOrUnaryMinusContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _prefixOp(NULL)
      , _prefixExpr(NULL)
    {}
    virtual ~PrefixOrUnaryMinusContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _prefixOp;
    AstNode* _prefixExpr;
};

class CastExprContext : public ContextBase
{
  public:
    CastExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _type(NULL)
      , _prefixExpr(NULL)
    {}
    virtual ~CastExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _type;
    AstNode* _prefixExpr;
};

class PassThroughContext : public ContextBase
{
  public:
    PassThroughContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _child(NULL)
    {}
    virtual ~PassThroughContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _child;
};

class AssignOpContext : public ContextBase
{
  public:
    AssignOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _rShiftEq(NULL)
    {}
    virtual ~AssignOpContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _rShiftEq;
};

class MultOpContext : public ContextBase
{
  public:
    MultOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~MultOpContext();
};

class AddOpContext : public ContextBase
{
  public:
    AddOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~AddOpContext();
};

class ShiftOpContext : public ContextBase
{
  public:
    ShiftOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _rShiftEq(NULL)
    {}
    virtual ~ShiftOpContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _rShiftEq;
};

class CompareOpContext : public ContextBase
{
  public:
    CompareOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _gEq(NULL)
    {}
    virtual ~CompareOpContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _gEq;
};

class BitOrOpContext : public ContextBase
{
  public:
    BitOrOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BitOrOpContext();
};

class BitXorOpContext : public ContextBase
{
  public:
    BitXorOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BitXorOpContext();
};

class BitAndOpContext : public ContextBase
{
  public:
    BitAndOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BitAndOpContext();
};

class PrefixOpContext : public ContextBase
{
  public:
    PrefixOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~PrefixOpContext();
};

class PostfixExprContext : public ContextBase
{
  public:
    PostfixExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _primaryExpr(NULL)
    {}
    virtual ~PostfixExprContext();
    virtual void addChild(AstNode& ast);
    virtual void stateChangeNotification();

  private:
    std::vector<AstNode*> _expr_pc;
    AstNode* _primaryExpr;
};

class GEqContext : public ContextBase
{
  public:
    GEqContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~GEqContext();
};

class RShiftContext : public ContextBase
{
  public:
    RShiftContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~RShiftContext();
};

class RShiftEqContext : public ContextBase
{
  public:
    RShiftEqContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~RShiftEqContext();
};

class UnwrapOrElseContext : public ContextBase
{
  public:
    UnwrapOrElseContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _foundUnwrap(false)
      , _lhs(NULL)
      , _rhs(NULL)
    {}
    virtual ~UnwrapOrElseContext();
    virtual void addChild(AstNode& ast);
    bool _foundUnwrap;

  private:
    AstNode* _lhs;
    AstNode* _rhs;
};

class PostfixOpContext : public ContextBase
{
  public:
    PostfixOpContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~PostfixOpContext();
};

class ParenExprContext : public ContextBase
{
  public:
    ParenExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
    {}
    virtual ~ParenExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
};

class ListLiteralContext : public ContextBase
{
  public:
    ListLiteralContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~ListLiteralContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _expr_pc;
};

class CurlyLiteralContext : public ContextBase
{
  public:
    CurlyLiteralContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~CurlyLiteralContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _exprOrMapping_pc;
};

class PrimitiveLiteralContext : public ContextBase
{
  public:
    PrimitiveLiteralContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _child(NULL)
    {}
    virtual ~PrimitiveLiteralContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _child;
};

class NullLiteralContext : public ContextBase
{
  public:
    NullLiteralContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~NullLiteralContext();
};

class StringLiteralContext : public ContextBase
{
  public:
    StringLiteralContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~StringLiteralContext();
};

class XMLLiteralContext : public ContextBase
{
  public:
    XMLLiteralContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~XMLLiteralContext();
};

class XMLTypeContext : public ContextBase
{
  public:
    XMLTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _xmlSchema(NULL)
    {}
    virtual ~XMLTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _xmlSchema;
};

class TypeModifiersContext : public ContextBase
{
  public:
    TypeModifiersContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~TypeModifiersContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _typeModifier;
};

class TypeContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    TypeContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 1)
      , ParaphrasePusher(parserState, syntaxType)
      , _child(NULL)
    {}
    virtual ~TypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _child;
};

class SubscriptContext : public ContextBase
{
  public:
    SubscriptContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr_q(NULL)
      , _expr(NULL)
    {}
    virtual ~SubscriptContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr_q;
    AstNode* _expr;
};

class ExprQContext : public ContextBase
{
  public:
    ExprQContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
    {}
    virtual ~ExprQContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
};

class SPLTypeContext
  : public ContextBase
  , public ParaphrasePusher
{
  public:
    SPLTypeContext(SPL::ParserContext& parserState, AstToken& startTok, int syntaxType)
      : ContextBase(parserState, startTok, 0)
      , ParaphrasePusher(parserState, syntaxType)
      , _typeLiteral(NULL)
    {}
    virtual ~SPLTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeLiteral;
};

class SPLExprContext : public ContextBase
{
  public:
    SPLExprContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 0)
      , _expr(NULL)
    {}
    virtual ~SPLExprContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
};

class ListTypeContext : public ContextBase
{
  public:
    ListTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeArgs1(NULL)
      , _typeDims(NULL)
    {}
    virtual ~ListTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeArgs1;
    AstNode* _typeDims;
};

class SetTypeContext : public ContextBase
{
  public:
    SetTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeArgs1(NULL)
      , _typeDims(NULL)
    {}
    virtual ~SetTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeArgs1;
    AstNode* _typeDims;
};

class MapTypeContext : public ContextBase
{
  public:
    MapTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeArgs2(NULL)
      , _typeDims(NULL)
    {}
    virtual ~MapTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeArgs2;
    AstNode* _typeDims;
};

class TypeArgs1Context : public ContextBase
{
  public:
    TypeArgs1Context(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~TypeArgs1Context();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _types;
};

class TypeArgs2Context : public ContextBase
{
  public:
    TypeArgs2Context(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~TypeArgs2Context();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _types;
};

class TupleTypeContext : public ContextBase
{
  public:
    TupleTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _tupleArgs(NULL)
    {}
    virtual ~TupleTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _tupleArgs;
};

class TupleBodyContext : public ContextBase
{
  public:
    TupleBodyContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~TupleBodyContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _v;
};

class OptionalContext : public ContextBase
{
  public:
    OptionalContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~OptionalContext();
};

class OptionalTypeContext : public ContextBase
{
  public:
    OptionalTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _type(NULL)
    {}
    virtual ~OptionalTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _type;
};

class AttributeDeclContext : public ContextBase
{
  public:
    AttributeDeclContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _type(NULL)
      , _identifier(NULL)
    {}
    virtual ~AttributeDeclContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _type;
    AstNode* _identifier;
};

class SimplePrimitiveTypeContext : public ContextBase
{
  public:
    SimplePrimitiveTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _simplePrimitiveTypeName(NULL)
    {}
    virtual ~SimplePrimitiveTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _simplePrimitiveTypeName;
};

class StringTypeContext : public ContextBase
{
  public:
    StringTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeDims(NULL)
    {}
    virtual ~StringTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeDims;
};

class EnumTypeContext : public ContextBase
{
  public:
    EnumTypeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~EnumTypeContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _id_pc;
};

class SimplePrimitiveTypeNameContext : public ContextBase
{
  public:
    SimplePrimitiveTypeNameContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~SimplePrimitiveTypeNameContext();
};

class TypeDimsContext : public ContextBase
{
  public:
    TypeDimsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
    {}
    virtual ~TypeDimsContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
};

class TypeNameContext : public ContextBase
{
  public:
    TypeNameContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeNameContents(NULL)
    {}
    virtual ~TypeNameContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeNameContents;
};

class TypeNameContentsContext : public ContextBase
{
  public:
    TypeNameContentsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _typeNameStart(NULL)
      , _identifier(NULL)
    {}
    virtual ~TypeNameContentsContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _typeNameStart;
    AstNode* _identifier;
};

class TypeNameStartContext : public ContextBase
{
  public:
    TypeNameStartContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _namespaceName(NULL)
      , _identifier(NULL)
    {}
    virtual ~TypeNameStartContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _namespaceName;
    AstNode* _identifier;
};

class BlockStmtNoScopeContext : public ContextBase
{
  public:
    BlockStmtNoScopeContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BlockStmtNoScopeContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _stmtOrType;
};

class BlockStmtContext : public ContextBase
{
  public:
    BlockStmtContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~BlockStmtContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<AstNode*> _stmtOrType;
};

class ExprOrMappingContext : public ContextBase
{
  public:
    ExprOrMappingContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr1(NULL)
      , _expr2(NULL)
    {}
    virtual ~ExprOrMappingContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr1;
    AstNode* _expr2;
};

class EqExprQContext : public ContextBase
{
  public:
    EqExprQContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _expr(NULL)
    {}
    virtual ~EqExprQContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _expr;
};

class AnnotationsContext : public ContextBase
{
  public:
    AnnotationsContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
    {}
    virtual ~AnnotationsContext();
    virtual void addChild(AstNode& ast);

  private:
    std::vector<SPL::AstNode*> _annotation;
};

class AnnotationContext : public ContextBase
{
  public:
    AnnotationContext(SPL::ParserContext& parserState, AstToken& startTok)
      : ContextBase(parserState, startTok, 1)
      , _identifier(NULL)
    {}
    virtual ~AnnotationContext();
    virtual void addChild(AstNode& ast);

  private:
    AstNode* _identifier;
    std::vector<SPL::AstNode*> _exprOrMapping_pc;
};

#if 0
class AnnotationTailContext : public ContextBase
{
public:
    AnnotationTailContext(SPL::ParserContext & parserState, AstToken & startTok)
        : ContextBase(parserState, startTok, 1) {}
    virtual ~AnnotationTailContext();
    virtual void addChild(AstNode & ast);
private:
    std::vector<SPL::AstNode*> _exprOrMapping_pc;
};
#endif

} // namespace SPL
#endif // SPL_PARSER_CONTEXTS_H
