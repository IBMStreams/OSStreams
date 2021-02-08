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

#include <SPL/FrontEnd/ParserContexts.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <iostream>

using namespace SPL;
using namespace std;

ContextBase::ContextBase(SPL::ParserContext& parserState,
                         AstToken& startTok,
                         unsigned returnValueCount)
  : _parserState(parserState)
  , _startToken(startTok)
  , _returnValueCount(returnValueCount)
  , _pool(SPL::AstPool::instance())
  , _previousContext(parserState.pushContext(*this))
  , _state(0)
{
    for (unsigned i = 0; i < returnValueCount; ++i) {
        _return.push_back(&errorNode());
    }
}

AstNode& ContextBase::errorNode()
{
    static AstNode& error = SPL::AST::errorT::create();
    return error;
}

ContextBase::~ContextBase()
{
    // If a production is exiting, and the state is 0, then the parser was testing a syntactic
    // predicate so don't actually return anything.
    if (_state > 0) {
        assert(_returnValueCount == _return.size());
        if (1 == _returnValueCount) {
            previousContext().addChild(*_return[0]);
        } else if (1 < _returnValueCount) {
            previousContext().addChild(_return);
        }
    }
    _parserState.popContext();
}

AstNode& ContextBase::splDoc(pANTLR3_TOKEN_STREAM tokStream) const
{
    return SPL::grabSplDoc(pool(), tokStream, &_startToken);
}

AstNode& ContextBase::splDocForCompilationUnit(pANTLR3_TOKEN_STREAM tokStream) const
{
    return SPL::grabSplDocForCompilationUnit(pool(), tokStream, &_startToken);
}

CompilationUnitContext::~CompilationUnitContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4: {
            if (NULL == _splNamespace) {
                _splNamespace = &SPL::AST::splNamespace::create(
                  startToken(), SPL::AST::splDoc::create(startToken()));
            }
            AstNode& compilationUnit =
              SPL::AST::compilationUnit::create(startToken(), splDocForCompilationUnit(_tokStream),
                                                *_splNamespace, *_useDirectives, _definitions);
            parseState()._result = &compilationUnit;
            break;
        }
        default:
            assert(false);
    }
}

void CompilationUnitContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _splNamespace = &ast;
            break;
        case 2:
            _useDirectives = &ast;
            break;
        case 3:
            _definitions.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

void UseDirectivesContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _useDirectives.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

UseDirectivesContext::~UseDirectivesContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::useDirectives::create(startToken(), _useDirectives));
            break;
        default:
            assert(false);
    }
}

UseDirectiveContext::~UseDirectiveContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(
              SPL::AST::useDirective::create(startToken(), *_namespaceName, *_useDirectiveTail));
            break;
        default:
            assert(false);
    }
}

void UseDirectiveContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _namespaceName = &ast;
            break;
        case 2:
            _useDirectiveTail = &SPL::AST::useDirectiveTail::create(startToken(), ast);
            break;
        default:
            assert(false);
    }
}

void NamespaceContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _namespaceName = &ast;
            break;
        default:
            assert(false);
    }
}

NamespaceContext::~NamespaceContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(
              SPL::AST::splNamespace::create(startToken(), splDoc(_tokStream), *_namespaceName));
            break;
        default:
            assert(false);
    }
}

void NamespaceNameContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _id = &ast;
            break;
        case 2:
            _id = &SPL::AST::attributeExpr::create(startToken(), *_id, ast);
            break;
        default:
            assert(false);
    }
}

NamespaceNameContext::~NamespaceNameContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
        case 3:
            setReturn(SPL::AST::namespaceName::create(startToken(), *_id));
            break;
        default:
            assert(false);
    }
}

void CompositeDefContext::addChild(const std::vector<AstNode*>& v)
{
    switch (state()) {
        case 2:
            _body = v[0];
            _rCurly = v[1];
            break;
        default:
            assert(false);
    }
}

void CompositeDefContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _head = &ast;
            break;
        default:
            assert(false);
    }
}

CompositeDefContext::~CompositeDefContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::compositeDef::create(startToken(), *_head, *_body, *_rCurly));
            break;
        default:
            assert(false);
    }
}

FunctionDefContext::~FunctionDefContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(
              SPL::AST::functionDef::create(startToken(), *_functionHead, *_blockStmtNoScope));
            break;
        default:
            assert(false);
    }
}

void FunctionDefContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _functionHead = &ast;
            break;
        case 2:
            _blockStmtNoScope = &ast;
            break;
        default:
            assert(false);
    }
}

IdentifierContext::~IdentifierContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::idT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

StarContext::~StarContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::useWildcardT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void CompositeHeadContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _annotations = &ast;
            break;
        case 2:
            _modifiers = &ast;
            break;
        case 3:
            _compositeT = &ast;
            break;
        case 4:
            _id = &ast;
            break;
        default:
            assert(false);
    }
}

void CompositeHeadContext::addChild(const std::vector<AstNode*>& v)
{
    switch (state()) {
        case 5:
            _compositeOut = v[0];
            _compositeIn = v[1];
            break;
        default:
            assert(false);
    }
}

CompositeHeadContext::~CompositeHeadContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            break;
        case 6:
            setReturn(SPL::AST::compositeHead::create(startToken(), splDoc(_tokStream),
                                                      *_annotations, *_modifiers, *_compositeT,
                                                      *_id, *_compositeOut, *_compositeIn));
            break;
        default:
            assert(false);
    }
}

CompositeInOutContext::~CompositeInOutContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3: {
            vector<AstNode*> v;
            v.push_back((NULL == _compositeOut) ? &SPL::AST::noneT::create() : _compositeOut);
            v.push_back((NULL == _compositeIn) ? &SPL::AST::noneT::create() : _compositeIn);
            setReturn(v);
            break;
        }
        default:
            assert(false);
    }
}

void CompositeInOutContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _compositeOut = &ast;
            break;
        case 2:
            _compositeIn = &ast;
            break;
        default:
            assert(false);
    }
}

void CompositeInContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _compositePorts.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

CompositeInContext::~CompositeInContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::compositeIn::create(startToken(), _compositePorts));
            break;
        default:
            assert(false);
    }
}

void CompositeOutContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _compositePorts.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

CompositeOutContext::~CompositeOutContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::compositeOut::create(startToken(), _compositePorts));
            break;
        default:
            assert(false);
    }
}

void CompositeModifiersContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _modifiers.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

CompositeModifiersContext::~CompositeModifiersContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::modifiers::create(startToken(), _modifiers));
            break;
        default:
            assert(false);
    }
}

PublicContext::~PublicContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::publicT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

EnumContext::~EnumContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::enumT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

ListContext::~ListContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::listT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

MapContext::~MapContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::mapT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

SetContext::~SetContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::setT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

XMLContext::~XMLContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::xmlType::create(startToken()));
            break;
        default:
            assert(false);
    }
}

TupleContext::~TupleContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::tupleT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

CompositeContext::~CompositeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::compositeT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

StaticContext::~StaticContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::staticT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

VoidContext::~VoidContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::voidT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

StatefulContext::~StatefulContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::statefulT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

MutableContext::~MutableContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::mutableT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

CompositePortContext::~CompositePortContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::compositePort::create(startToken(), _streamType, *_id));
            break;
        default:
            assert(false);
    }
}

void CompositePortContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _streamType = &ast;
            break;
        case 2:
            _id = &ast;
            break;
        default:
            assert(false);
    }
}

void StreamTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _tupleArgs = &ast;
            break;
        default:
            assert(false);
    }
}

StreamTypeContext::~StreamTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::streamType::create(startToken(), *_tupleArgs));
            break;
        default:
            assert(false);
    }
}

void CompositeBodyContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _compositeBodyClause.push_back(&ast);
            break;
        case 2:
            _rCurly = &ast;
            break;
        default:
            assert(false);
    }
}

CompositeBodyContext::~CompositeBodyContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3: {
            vector<AstNode*> v;
            v.push_back(&SPL::AST::compositeBody::create(startToken(), _compositeBodyClause));
            v.push_back(_rCurly);
            setReturn(v);
            break;
        }
        default:
            assert(false);
    }
}

void FormalClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _compositeFormals.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

FormalClauseContext::~FormalClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::formalClause::create(startToken(), _compositeFormals));
            break;
        default:
            assert(false);
    }
}

void TypeClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeDefs.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

TypeClauseContext::~TypeClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::typeClause::create(startToken(), _typeDefs));
            break;
        default:
            assert(false);
    }
}

void GraphClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _operatorInvocations.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

GraphClauseContext::~GraphClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::graphClause::create(startToken(), _operatorInvocations));
            break;
        default:
            assert(false);
    }
}

ConfigContext::~ConfigContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::config::create(startToken(), *_id, _exprs));
            break;
        default:
            assert(false);
    }
}

void ConfigContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _id = &ast;
            break;
        case 2:
            _exprs.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

CompositeFormalContext::~CompositeFormalContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4:
            setReturn(SPL::AST::compositeFormal::create(startToken(), *_expressionMode, *_id,
                                                        *_compositeFormalInit));
            break;
        default:
            assert(false);
    }
}

void CompositeFormalContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expressionMode = &ast;
            break;
        case 2:
            _id = &ast;
            break;
        case 3:
            _compositeFormalInit = &ast;
            break;
        default:
            assert(false);
    }
}

OpInvokeContext::~OpInvokeContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(
              SPL::AST::opInvoke::create(startToken(), *_opInvokeHead, *_opInvokeBody, *_rCurly));
            break;
        default:
            assert(false);
    }
}

void OpInvokeContext::addChild(const std::vector<AstNode*>& v)
{
    switch (state()) {
        case 2:
            _opInvokeBody = v[0];
            _rCurly = v[1];
            break;
        default:
            assert(false);
    }
}

void OpInvokeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opInvokeHead = &ast;
            break;
        default:
            assert(false);
    }
}

RCurlyContext::~RCurlyContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::rCurlyT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

SemiContext::~SemiContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::semiT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

ExpressionModeContext::~ExpressionModeContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3: {
            AstNode& expressionMode =
              (NULL == _typeArgs)
                ? SPL::AST::expressionMode::create(startToken(), *_type)
                : SPL::AST::expressionMode::create(startToken(), *_type, *_typeArgs);
            setReturn(expressionMode);
            break;
        }
        default:
            assert(false);
    }
}

void ExpressionModeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _type = &ast;
            break;
        case 2:
            _typeArgs = &ast;
            break;
        default:
            assert(false);
    }
}

AttributeTContext::~AttributeTContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::attributeT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

ExpressionTContext::~ExpressionTContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::expressionT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

FunctionTContext::~FunctionTContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::functionT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

OperatorTContext::~OperatorTContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::operatorT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

TypeTContext::~TypeTContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::typeT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void CompositeFormalInitContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opActual = &ast;
            break;
        default:
            assert(false);
    }
}

CompositeFormalInitContext::~CompositeFormalInitContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2: {
            AstNode& compositeFormalInit =
              (NULL == _opActual) ? SPL::AST::compositeFormalInit::create(startToken())
                                  : SPL::AST::compositeFormalInit::create(startToken(), *_opActual);
            setReturn(compositeFormalInit);
            break;
        }
        default:
            assert(false);
    }
}

OpInvokeActualContext::~OpInvokeActualContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::opInvokeActual::create(startToken(), *_id, *_opActual));
            break;
        default:
            assert(false);
    }
}

void OpInvokeActualContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _id = &ast;
            break;
        case 2:
            _opActual = &ast;
            break;
        default:
            assert(false);
    }
}

void ActualClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opInvokeActual.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

ActualClauseContext::~ActualClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::actualClause::create(startToken(), _opInvokeActual));
            break;
        default:
            assert(false);
    }
}

OpInvokeBodyContext::~OpInvokeBodyContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3: {
            vector<AstNode*> v;
            v.push_back(&SPL::AST::opInvokeBody::create(startToken(), _opInvokeBodyClause));
            v.push_back(_rCurly);
            setReturn(v);
            break;
        }
        default:
            assert(false);
    }
}

void OpInvokeBodyContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opInvokeBodyClause.push_back(&ast);
            break;
        case 2:
            _rCurly = &ast;
            break;
        default:
            assert(false);
    }
}

void LogicClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opInvokeLogic.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

LogicClauseContext::~LogicClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::logic::create(startToken(), _opInvokeLogic));
            break;
        default:
            assert(false);
    }
}

void WindowClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opInvokeWindow.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

WindowClauseContext::~WindowClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::window::create(startToken(), _opInvokeWindow));
            break;
        default:
            assert(false);
    }
}

void OutputClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opInvokeOutput.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

OutputClauseContext::~OutputClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::outputClause::create(startToken(), _opInvokeOutput));
            break;
        default:
            assert(false);
    }
}

void ConfigClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _config.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

ConfigClauseContext::~ConfigClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::configClause::create(startToken(), _config));
            break;
        default:
            assert(false);
    }
}

TypeDefContext::~TypeDefContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            break;
        case 5:
            setReturn(SPL::AST::typeDef::create(startToken(), splDoc(_tokStream), *_typeModifiers,
                                                *_identifier, *_typeDefTail, *_semi));
            break;
        default:
            assert(false);
    }
}

void TypeDefContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeModifiers = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        case 3:
            _typeDefTail = &ast;
            break;
        case 4:
            _semi = &ast;
            break;
        default:
            assert(false);
    }
}

StandaloneTypeDefContext::~StandaloneTypeDefContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4: {
            vector<AstNode*> modifiers;
            setReturn(
              SPL::AST::typeDef::create(startToken(), splDoc(_tokStream),
                                        SPL::AST::modifiers::create(startToken(), modifiers),
                                        *_identifier, *_typeDefTail, *_semi));
            break;
        }
        default:
            assert(false);
    }
}

void StandaloneTypeDefContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _typeDefTail = &ast;
            break;
        case 3:
            _semi = &ast;
            break;
        default:
            assert(false);
    }
}

OpInvokeWindowContext::~OpInvokeWindowContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::opInvokeWindow::create(startToken(), *_identifier, _exprs));
            break;
        default:
            assert(false);
    }
}

void OpInvokeWindowContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _exprs.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

OpOutputContext::~OpOutputContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4: {
            AstNode& opOutput =
              (NULL == _asAlias)
                ? SPL::AST::opOutput::create(startToken(), *_streamType, *_identifier)
                : SPL::AST::opOutput::create(startToken(), *_streamType, *_identifier, *_asAlias);
            setReturn(opOutput);
            break;
        }
        default:
            assert(false);
    }
}

void OpOutputContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _streamType = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        case 3:
            _asAlias = &ast;
            break;
        default:
            assert(false);
    }
}

OpOutputsContext::~OpOutputsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::opOutputs::create(startToken(), _opOutput_ps));
            break;
        default:
            assert(false);
    }
}

void OpOutputsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _opOutput_ps.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

void OpActualTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _type = &ast;
            break;
        default:
            assert(false);
    }
}

OpActualTypeContext::~OpActualTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::opActualType::create(startToken(), *_type));
            break;
        default:
            assert(false);
    }
}

void OpActualNameContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeNameContents = &ast;
            break;
        default:
            assert(false);
    }
}

OpActualNameContext::~OpActualNameContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::opActualName::create(startToken(), *_typeNameContents));
            break;
        default:
            assert(false);
    }
}

void OpActualExprsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _exprs.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

OpActualExprsContext::~OpActualExprsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::opActualExprs::create(startToken(), _exprs));
            break;
        default:
            assert(false);
    }
}

OpInvokeHeadContext::~OpInvokeHeadContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            break;
        case 8: {
            setReturn(SPL::AST::opInvokeHead::create(
              startToken(), splDoc(_tokStream), *_annotations, *_opOutputs,
              (NULL == _asAlias) ? SPL::AST::noneT::create() : *_asAlias, *_opName, *_opInputs));
            break;
        }
        default:
            assert(false);
    }
}

void OpInvokeHeadContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _annotations = &ast;
            break;
        case 2: {
            vector<AstNode*> v;
            _opOutputs = &SPL::AST::opOutputs::create(startToken(), v);
            _asAlias = &ast;
        } break;
        case 3: {
            vector<AstNode*> v;
            v.push_back(&ast);
            _opOutputs = &SPL::AST::opOutputs::create(startToken(), v);
        } break;
        case 4:
            _opOutputs = &ast;
            break;
        case 5:
            _asAlias = &ast;
            break;
        case 6:
            _opName = &ast;
            break;
        case 7:
            _opInputs = &ast;
            break;
        default:
            assert(false);
    }
}

void OpInputsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _portInputs_ps.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

OpInputsContext::~OpInputsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::opInputs::create(startToken(), _portInputs_ps));
            break;
        default:
            assert(false);
    }
}

PortInputsContext::~PortInputsContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4:
            setReturn(SPL::AST::portInputs::create(startToken(), _streamType, _id_pc, _asAlias));
            break;
        default:
            assert(false);
    }
}

void PortInputsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _streamType = &ast;
            break;
        case 2:
            _id_pc.push_back(&ast);
            break;
        case 3:
            _asAlias = &ast;
            break;
        default:
            assert(false);
    }
}

void AsAliasContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

AsAliasContext::~AsAliasContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::as::create(startToken(), *_identifier));
            break;
        default:
            assert(false);
    }
}

QualifiedNameContext::~QualifiedNameContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
            //        case 3: setReturn(SPL::AST::qualifiedName::create(pool(), &startToken(),
            //        _namespaceName, _identifier)); break;
        case 3:
            setReturn(SPL::AST::qualifierExpr::create(startToken(), *_namespaceName, *_identifier));
            break;
        default:
            assert(false);
    }
}

void QualifiedNameContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _namespaceName = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

OpInvokeOutputContext::~OpInvokeOutputContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(
              SPL::AST::opInvokeOutput::create(startToken(), *_identifier, _outputAssign_pc));
            break;
        default:
            assert(false);
    }
}

void OpInvokeOutputContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _outputAssign_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

OutputAssignContext::~OutputAssignContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4:
            setReturn(SPL::AST::infixExpr::create(startToken(), *_eq, *_identifier, *_expr));
            break;
        default:
            assert(false);
    }
}

void OutputAssignContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _eq = &ast;
            break;
        case 3:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

OpEqualContext::~OpEqualContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::eqT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

OpInvokeStateContext::~OpInvokeStateContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::opInvokeLogicState::create(startToken(), *_decl));
            break;
        default:
            assert(false);
    }
}

void OpInvokeStateContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            assert(SPL::AST::idT::is(ast)); // ignore this return
        case 2:
            _decl = &ast;
            break;
        default:
            assert(false);
    }
}

KeySTATEContext::~KeySTATEContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _identifier);
            setReturn(*_identifier);
            break;
        default:
            assert(false);
    }
}

void KeySTATEContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

OpInvokeOnProcessContext::~OpInvokeOnProcessContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::opInvokeLogicOnProcess::create(startToken(), *_stmt));
            break;
        default:
            assert(false);
    }
}

void OpInvokeOnProcessContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            assert(SPL::AST::idT::is(ast)); // ignore this return
        case 2:
            _stmt = &ast;
            break;
        default:
            assert(false);
    }
}

KeyONPROCESSContext::~KeyONPROCESSContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _identifier);
            setReturn(*_identifier);
            break;
        default:
            assert(false);
    }
}

void KeyONPROCESSContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

OpInvokeOnTuple::~OpInvokeOnTuple()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::opInvokeLogicOnTuple::create(startToken(), *_identifier, *_stmt));
            break;
        default:
            assert(false);
    }
}

void OpInvokeOnTuple::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _stmt = &ast;
            break;
        default:
            assert(false);
    }
}

OpInvokeOnPunct::~OpInvokeOnPunct()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::opInvokeLogicOnPunct::create(startToken(), *_identifier, *_stmt));
            break;
        default:
            assert(false);
    }
}

void OpInvokeOnPunct::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _stmt = &ast;
            break;
        default:
            assert(false);
    }
}

#if 0
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
OpInvokeOnWatermark::~OpInvokeOnWatermark()
{
    switch(state()) {
        case 0:
        case 1:
        case 2: break;
        case 3: setReturn(SPL::AST::opInvokeLogicOnWatermark::create(startToken(), *_identifier, *_stmt)); break;
        default: assert(false);
    }
}

void OpInvokeOnWatermark::addChild(AstNode & ast)
{
    switch(state()) {
        case 1: _identifier = &ast; break;
        case 2: _stmt = &ast; break;
        default: assert(false);
    }
}
#endif

FunctionHeadContext::~FunctionHeadContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            break;
        case 5:
            setReturn(SPL::AST::functionHead::create(startToken(), splDoc(_tokStream),
                                                     *_functionModifiers, *_returnType,
                                                     *_identifier, _functionFormal_pc));
            break;
        default:
            assert(false);
    }
}

void FunctionHeadContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _functionModifiers = &ast;
            break;
        case 2:
            _returnType = &ast;
            break;
        case 3:
            _identifier = &ast;
            break;
        case 4:
            _functionFormal_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

FunctionPrototypeContext::~FunctionPrototypeContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            break;
        case 7:
            parseState()._result = &SPL::AST::functionPrototype::create(
              startToken(), *_typeFormals, *_boundsFormals, *_functionModifiers, *_returnType,
              *_identifier, *_protoFormals);
            break;
        default:
            assert(false);
    }
}

void FunctionPrototypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeFormals = &ast;
            break;
        case 2:
            _boundsFormals = &ast;
            break;
        case 3:
            _functionModifiers = &ast;
            break;
        case 4:
            _returnType = &ast;
            break;
        case 5:
            _identifier = &ast;
            break;
        case 6:
            _protoFormals = &ast;
            break;
        default:
            assert(false);
    }
}

void TypeFormalsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeFormal_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

TypeFormalsContext::~TypeFormalsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::typeFormals::create(startToken(), _typeFormal_pc));
            break;
        default:
            assert(false);
    }
}

TypeFormalContext::~TypeFormalContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(
              SPL::AST::typeFormal::create(startToken(), *_typeFormalConstraint, *_identifier));
            break;
        default:
            assert(false);
    }
}

void TypeFormalContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeFormalConstraint = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

void BoundsFormalsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _id_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

BoundsFormalsContext::~BoundsFormalsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::boundsFormals::create(startToken(), _id_pc));
            break;
        default:
            assert(false);
    }
}

void FunctionModifiersContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _functionModifier.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

FunctionModifiersContext::~FunctionModifiersContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::modifiers::create(startToken(), _functionModifier));
            break;
        default:
            assert(false);
    }
}

FunctionFormalContext::~FunctionFormalContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4:
            setReturn(SPL::AST::functionFormal::create(startToken(), *_formalModifiers, *_type,
                                                       *_identifier));
            break;
        default:
            assert(false);
    }
}

void FunctionFormalContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _formalModifiers = &ast;
            break;
        case 2:
            _type = &ast;
            break;
        case 3:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

void ProtoFormalsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _protoFormal_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

ProtoFormalsContext::~ProtoFormalsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::protoFormals::create(startToken(), _protoFormal_pc));
            break;
        default:
            assert(false);
    }
}

ProtoFormalContext::~ProtoFormalContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4: {
            AstNode& protoFormal =
              (NULL == _identifier)
                ? SPL::AST::protoFormal::create(startToken(), *_formalModifiers, *_type)
                : SPL::AST::protoFormal::create(startToken(), *_formalModifiers, *_type,
                                                *_identifier);
            setReturn(protoFormal);
            break;
        }
        default:
            assert(false);
    }
}

void ProtoFormalContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _formalModifiers = &ast;
            break;
        case 2:
            _type = &ast;
            break;
        case 3:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

void FormalModifiersContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _formalModifier.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

FormalModifiersContext::~FormalModifiersContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::modifiers::create(startToken(), _formalModifier));
            break;
        default:
            assert(false);
    }
}

void LocalDeclsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _localDecl.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

LocalDeclsContext::~LocalDeclsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::blockStmtNoScope::create(startToken(), _localDecl));
            break;
        default:
            assert(false);
    }
}

LocalDeclContext::~LocalDeclContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
            //        case 4: setReturn(SPL::AST::localDecl::create(pool(), &startToken(),
            //        _localModifiers, _type, _localDeclItem_pc)); break;
        case 4:
            setReturn(SPL::AST::localDecl::create(startToken(), *_localModifiers, *_type,
                                                  _localDeclItem_pc));
            break;
        default:
            assert(false);
    }
}

void LocalDeclContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _localModifiers = &ast;
            break;
        case 2:
            _type = &ast;
            break;
        case 3:
            _localDeclItem_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

void LocalModifiersContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _localModifier.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

LocalModifiersContext::~LocalModifiersContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::modifiers::create(startToken(), _localModifier));
            break;
        default:
            assert(false);
    }
}

LocalDeclItemContext::~LocalDeclItemContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::localDeclItem::create(startToken(), *_identifier, *_eqExpr_q));
            break;
        default:
            assert(false);
    }
}

void LocalDeclItemContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _eqExpr_q = &ast;
            break;
        default:
            assert(false);
    }
}

void StmtContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _child = &ast;
            break;
        default:
            assert(false);
    }
}

StmtContext::~StmtContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _child);
            setReturn(*_child);
            break;
        default:
            assert(false);
    }
}

void ReturnStmtContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

ReturnStmtContext::~ReturnStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3: {
            AstNode& returnStmt = (NULL == _expr)
                                    ? SPL::AST::returnStmt::create(startToken())
                                    : SPL::AST::returnStmt::create(startToken(), *_expr);
            setReturn(returnStmt);
            break;
        }
        default:
            assert(false);
    }
}

ContinueStmtContext::~ContinueStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::continueT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

BreakStmtContext::~BreakStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::breakT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void WhileStmtContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        case 2:
            _stmt = &ast;
            break;
        default:
            assert(false);
    }
}

WhileStmtContext::~WhileStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::whileStmt::create(startToken(), *_expr, *_stmt));
            break;
        default:
            assert(false);
    }
}

void ForStmtContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _forInit = &ast;
            break;
        case 2:
            _stmtNoScope = &ast;
            break;
        default:
            assert(false);
    }
}

ForStmtContext::~ForStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            assert(NULL != _forInit);
            assert(NULL != _stmtNoScope);
            setReturn(SPL::AST::forStmt::create(startToken(), *_forInit, *_stmtNoScope));
            break;
        default:
            assert(false);
    }
}

void IfStmtContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        case 2:
            _stmt = &ast;
            break;
        case 3:
            _elseClause = &ast;
            break;
        default:
            assert(false);
    }
}

IfStmtContext::~IfStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4: {
            AstNode& ifStmt =
              (NULL == _elseClause)
                ? SPL::AST::ifStmt::create(startToken(), *_expr, *_stmt)
                : SPL::AST::ifStmt::create(startToken(), *_expr, *_stmt, *_elseClause);
            setReturn(ifStmt);
            break;
        }
        default:
            assert(false);
    }
}

void ExprStmtContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

ExprStmtContext::~ExprStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::exprStmt::create(startToken(), *_expr));
            break;
        default:
            assert(false);
    }
}

void ElseClauseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _stmt = &ast;
            break;
        default:
            assert(false);
    }
}

ElseClauseContext::~ElseClauseContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::elseClause::create(startToken(), *_stmt));
            break;
        default:
            assert(false);
    }
}

void ForInitContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _type = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        case 3:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

ForInitContext::~ForInitContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            break;
        case 5: {
            vector<AstNode*> modifiers;
            assert(NULL != _type);
            assert(NULL != _identifier);
            assert(NULL != _expr);
            setReturn(SPL::AST::forInit::create(
              startToken(), SPL::AST::modifiers::create(startToken(), modifiers), *_type,
              *_identifier, *_expr));
            break;
        }
        default:
            assert(false);
    }
}

void ExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _assignExpr = &ast;
            break;
        default:
            assert(false);
    }
}

ExprContext::~ExprContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _assignExpr);
            setReturn(*_assignExpr);
            break;
        default:
            assert(false);
    }
}

void AssignExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _condExpr = &ast;
            break;
        case 3:
            _assignOp = &ast;
            break;
        case 4:
            _condExpr = &SPL::AST::infixExpr::create(startToken(), *_assignOp, *_condExpr, ast);
            break;
        default:
            assert(false);
    }
}

AssignExprContext::~AssignExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _condExpr);
            setReturn(*_condExpr);
            break;
        default:
            assert(false);
    }
}

void CondExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _logicOrExpr = &ast;
            break;
        case 3:
            _condExpr = &ast;
            break;
        case 4:
            _logicOrExpr =
              &SPL::AST::conditionalExpr::create(startToken(), *_logicOrExpr, *_condExpr, ast);
            break;
        default:
            assert(false);
    }
}

CondExprContext::~CondExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _logicOrExpr);
            setReturn(*_logicOrExpr);
            break;
        default:
            assert(false);
    }
}

void LogicOrExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _logicAndExpr = &ast;
            break;
        case 3:
            _barBar = &ast;
            break;
        case 4:
            _logicAndExpr =
              &SPL::AST::infixExpr::create(startToken(), *_barBar, *_logicAndExpr, ast);
            break;
        default:
            assert(false);
    }
}

LogicOrExprContext::~LogicOrExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _logicAndExpr);
            setReturn(*_logicAndExpr);
            break;
        default:
            assert(false);
    }
}

BarBarContext::~BarBarContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::barBarT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void LogicAndExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _memberExpr = &ast;
            break;
        case 3:
            _ampAmp = &ast;
            break;
        case 4:
            _memberExpr = &SPL::AST::infixExpr::create(startToken(), *_ampAmp, *_memberExpr, ast);
            break;
        default:
            assert(false);
    }
}

LogicAndExprContext::~LogicAndExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _memberExpr);
            setReturn(*_memberExpr);
            break;
        default:
            assert(false);
    }
}

AmpAmpContext::~AmpAmpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::ampAmpT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void MemberExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _bitOrExpr = &ast;
            break;
        case 3:
            _in = &ast;
            break;
        case 4:
            _bitOrExpr = &SPL::AST::infixExpr::create(startToken(), *_in, *_bitOrExpr, ast);
            break;
        default:
            assert(false);
    }
}

MemberExprContext::~MemberExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _bitOrExpr);
            setReturn(*_bitOrExpr);
            break;
        default:
            assert(false);
    }
}

InContext::~InContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::inT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void BitOrExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _bitXorExpr = &ast;
            break;
        case 3:
            _bitOrOp = &ast;
            break;
        case 4:
            _bitXorExpr = &SPL::AST::infixExpr::create(startToken(), *_bitOrOp, *_bitXorExpr, ast);
            break;
        default:
            assert(false);
    }
}

BitOrExprContext::~BitOrExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _bitXorExpr);
            setReturn(*_bitXorExpr);
            break;
        default:
            assert(false);
    }
}

void BitXorExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _bitAndExpr = &ast;
            break;
        case 3:
            _bitXorOp = &ast;
            break;
        case 4:
            _bitAndExpr = &SPL::AST::infixExpr::create(startToken(), *_bitXorOp, *_bitAndExpr, ast);
            break;
        default:
            assert(false);
    }
}

BitXorExprContext::~BitXorExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _bitAndExpr);
            setReturn(*_bitAndExpr);
            break;
        default:
            assert(false);
    }
}

void BitAndExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _compareExpr = &ast;
            break;
        case 3:
            _bitAndOp = &ast;
            break;
        case 4:
            _compareExpr =
              &SPL::AST::infixExpr::create(startToken(), *_bitAndOp, *_compareExpr, ast);
            break;
        default:
            assert(false);
    }
}

BitAndExprContext::~BitAndExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _compareExpr);
            setReturn(*_compareExpr);
            break;
        default:
            assert(false);
    }
}

void CompareExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _shiftExpr = &ast;
            break;
        case 3:
            _compareOp = &ast;
            break;
        case 4:
            _shiftExpr = &SPL::AST::infixExpr::create(startToken(), *_compareOp, *_shiftExpr, ast);
            break;
        default:
            assert(false);
    }
}

CompareExprContext::~CompareExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _shiftExpr);
            setReturn(*_shiftExpr);
            break;
        default:
            assert(false);
    }
}

void ShiftExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _addExpr = &ast;
            break;
        case 3:
            _shiftOp = &ast;
            break;
        case 4:
            _addExpr = &SPL::AST::infixExpr::create(startToken(), *_shiftOp, *_addExpr, ast);
            break;
        default:
            assert(false);
    }
}

ShiftExprContext::~ShiftExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _addExpr);
            setReturn(*_addExpr);
            break;
        default:
            assert(false);
    }
}

void AddExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _multExpr = &ast;
            break;
        case 3:
            _addOp = &ast;
            break;
        case 4:
            _multExpr = &SPL::AST::infixExpr::create(startToken(), *_addOp, *_multExpr, ast);
            break;
        default:
            assert(false);
    }
}

AddExprContext::~AddExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _multExpr);
            setReturn(*_multExpr);
            break;
        default:
            assert(false);
    }
}

void MultExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _prefixExpr = &ast;
            break;
        case 3:
            _multOp = &ast;
            break;
        case 4:
            _prefixExpr = &SPL::AST::infixExpr::create(startToken(), *_multOp, *_prefixExpr, ast);
            break;
        default:
            assert(false);
    }
}

MultExprContext::~MultExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
        case 5:
            assert(NULL != _prefixExpr);
            setReturn(*_prefixExpr);
            break;
        default:
            assert(false);
    }
}

void PrefixOrUnaryMinusContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _prefixOp = &ast;
            break;
        case 2:
            _prefixExpr = &ast;
            break;
        default:
            assert(false);
    }
}

PrefixOrUnaryMinusContext::~PrefixOrUnaryMinusContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::prefixOrPrependMinus(pool(), startToken(), *_prefixOp, *_prefixExpr));
            break;
        default:
            assert(false);
    }
}

void CastExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _type = &ast;
            break;
        case 2:
            _prefixExpr = &ast;
            break;
        default:
            assert(false);
    }
}

CastExprContext::~CastExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            assert(NULL != _type);
            assert(NULL != _prefixExpr);
            setReturn(SPL::AST::castExpr::create(startToken(), *_type, *_prefixExpr));
            break;
        default:
            assert(false);
    }
}

PassThroughContext::~PassThroughContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _child);
            setReturn(*_child);
            break;
        default:
            assert(false);
    }
}

void PassThroughContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _child = &ast;
            break;
        default:
            assert(false);
    }
}

AssignOpContext::~AssignOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::eqT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::plusEqT::create(startToken()));
            break;
        case 4:
            setReturn(SPL::AST::minusEqT::create(startToken()));
            break;
        case 5:
            setReturn(SPL::AST::starEqT::create(startToken()));
            break;
        case 6:
            setReturn(SPL::AST::slashEqT::create(startToken()));
            break;
        case 7:
            setReturn(SPL::AST::modEqT::create(startToken()));
            break;
        case 8:
            setReturn(SPL::AST::lShiftEqT::create(startToken()));
            break;
        case 9:
            assert(NULL != _rShiftEq);
            setReturn(*_rShiftEq);
            break;
        case 10:
            setReturn(SPL::AST::ampEqT::create(startToken()));
            break;
        case 11:
            setReturn(SPL::AST::hatEqT::create(startToken()));
            break;
        case 12:
            setReturn(SPL::AST::barEqT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void AssignOpContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _rShiftEq = &ast;
            break;
        default:
            assert(false);
    }
}

MultOpContext::~MultOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::starT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::slashT::create(startToken()));
            break;
        case 4:
            setReturn(SPL::AST::modT::create(startToken()));
            break;
        case 5:
            setReturn(SPL::AST::dotStarT::create(startToken()));
            break;
        case 6:
            setReturn(SPL::AST::dotSlashT::create(startToken()));
            break;
        case 7:
            setReturn(SPL::AST::dotModT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

AddOpContext::~AddOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::plusT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::minusT::create(startToken()));
            break;
        case 4:
            setReturn(SPL::AST::dotPlusT::create(startToken()));
            break;
        case 5:
            setReturn(SPL::AST::dotMinusT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

ShiftOpContext::~ShiftOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::lShiftT::create(startToken()));
            break;
        case 3:
            assert(NULL != _rShiftEq);
            setReturn(*_rShiftEq);
            break;
        case 4:
            setReturn(SPL::AST::dotLShiftT::create(startToken()));
            break;
        case 5:
            setReturn(SPL::AST::dotRShiftT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void ShiftOpContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _rShiftEq = &ast;
            break;
        default:
            assert(false);
    }
}

CompareOpContext::~CompareOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::lessT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::leqT::create(startToken()));
            break;
        case 4:
            setReturn(SPL::AST::greaterT::create(startToken()));
            break;
        case 5:
            assert(NULL != _gEq);
            setReturn(*_gEq);
            break;
        case 6:
            setReturn(SPL::AST::neqT::create(startToken()));
            break;
        case 7:
            setReturn(SPL::AST::eqEqT::create(startToken()));
            break;
        case 8:
            setReturn(SPL::AST::dotLessT::create(startToken()));
            break;
        case 9:
            setReturn(SPL::AST::dotLeqT::create(startToken()));
            break;
        case 10:
            setReturn(SPL::AST::dotGreaterT::create(startToken()));
            break;
        case 11:
            setReturn(SPL::AST::dotGeqT::create(startToken()));
            break;
        case 12:
            setReturn(SPL::AST::dotNeqT::create(startToken()));
            break;
        case 13:
            setReturn(SPL::AST::dotEqEqT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void CompareOpContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _gEq = &ast;
            break;
        default:
            assert(false);
    }
}

BitOrOpContext::~BitOrOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::barT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::dotBarT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

BitXorOpContext::~BitXorOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::hatT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::dotHatT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

BitAndOpContext::~BitAndOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::ampT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::dotAmpT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

PrefixOpContext::~PrefixOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::bangT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::unaryMinusT::create(startToken()));
            break;
        case 4:
            setReturn(SPL::AST::tildeT::create(startToken()));
            break;
        case 5:
            setReturn(SPL::AST::plusPlusT::create(startToken()));
            break;
        case 6:
            setReturn(SPL::AST::minusMinusT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

PostfixExprContext::~PostfixExprContext()
{
    switch (state()) {
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        case 1:
        case 10:
            assert(NULL != _primaryExpr);
            setReturn(*_primaryExpr);
            break;
        default:
            assert(false);
    }
}

void PostfixExprContext::stateChangeNotification()
{
    switch (state()) {
        case 2:
            _expr_pc.clear();
            break;
            //        case 3: assert(NULL != _primaryExpr); _primaryExpr =
            //        &SPL::AST::callExpr::create(pool(), &startToken(), _primaryExpr, _expr_pc);
            //        break;
        case 3:
            assert(NULL != _primaryExpr);
            _primaryExpr = &SPL::AST::callExpr::create(startToken(), *_primaryExpr, _expr_pc);
            break;
        case 8:
            assert(NULL != _primaryExpr);
            _primaryExpr = &SPL::AST::unwrapExpr::create(startToken(), *_primaryExpr);
            break;
        case 9:
            assert(NULL != _primaryExpr);
            _primaryExpr = &SPL::AST::isPresentExpr::create(startToken(), *_primaryExpr);
            break;
        default:
            break;
    }
}

void PostfixExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _primaryExpr = &ast;
            break;
        case 2:
            _expr_pc.push_back(&ast);
            break;
            //        case 4: assert(NULL != _primaryExpr); _primaryExpr =
            //        &SPL::AST::qualifierExpr::create(pool(), &startToken(),
            //        &SPL::AST::namespaceName::create(startToken(), *_primaryExpr), &ast); break;
        case 4:
            assert(NULL != _primaryExpr);
            _primaryExpr = &SPL::AST::qualifierExpr::create(
              startToken(), SPL::AST::namespaceName::create(startToken(), *_primaryExpr), ast);
            break;
        case 5:
            assert(NULL != _primaryExpr);
            _primaryExpr = &SPL::AST::attributeExpr::create(startToken(), *_primaryExpr, ast);
            break;
        case 6:
            assert(NULL != _primaryExpr);
            _primaryExpr = &SPL::AST::subscriptExpr::create(startToken(), *_primaryExpr, ast);
            break;
        case 7:
            assert(NULL != _primaryExpr);
            _primaryExpr = &SPL::AST::postfixExpr::create(startToken(), *_primaryExpr, ast);
            break;
        default:
            assert(false);
    }
}

GEqContext::~GEqContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::geqT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

RShiftContext::~RShiftContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::rShiftT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

RShiftEqContext::~RShiftEqContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::rShiftEqT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

UnwrapOrElseContext::~UnwrapOrElseContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
            break;
        case 6:
            if (_foundUnwrap) {
                if (_lhs && _rhs) { // unwrapOrElse
                    setReturn(SPL::AST::unwrapOrElseExpr::create(startToken(), *_lhs, *_rhs));
                }

                else {
                    // should not happen
                    assert(false);
                }
            }

            else {
                if (_lhs) {
                    // prefix Expression... just use the child expression
                    setReturn(*_lhs);
                }

                else {
                    // should not happen
                    assert(false);
                }
            }
            break;
        default:
            cerr << "bad state: " << state() << "\n";
            assert(false);
    }
}

void UnwrapOrElseContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _lhs = &ast;
            break;
        case 4:
            _rhs = &ast;
            break;
        default:
            assert(false);
    }
}

PostfixOpContext::~PostfixOpContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::plusPlusT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::minusMinusT::create(startToken()));
            break;
        case 4:
            setReturn(SPL::AST::bangT::create(startToken()));
            break;
        case 5:
            setReturn(SPL::AST::qmarkT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

ParenExprContext::~ParenExprContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::parenthesizedExpr::create(startToken(), *_expr));
            break;
        default:
            assert(false);
    }
}

void ParenExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

ListLiteralContext::~ListLiteralContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::listLiteral::create(startToken(), _expr_pc));
            break;
        default:
            assert(false);
    }
}

void ListLiteralContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

CurlyLiteralContext::~CurlyLiteralContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(createCurlyLiteral(pool(), startToken(), _exprOrMapping_pc, parseState()));
            break;
        default:
            assert(false);
    }
}

void CurlyLiteralContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _exprOrMapping_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

PrimitiveLiteralContext::~PrimitiveLiteralContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::trueLiteralT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::falseLiteralT::create(startToken()));
            break;
        case 4:
        case 5:
            assert(NULL != _child);
            setReturn(*_child);
            break;
        case 6:
            setReturn(SPL::AST::floatLiteralT::create(startToken()));
            break;
        case 7:
            setReturn(SPL::AST::intLiteralT::create(startToken()));
            break;
        case 8:
            setReturn(SPL::AST::hexLiteralT::create(startToken()));
            break;
        case 9:
            setReturn(SPL::AST::nullLiteralT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void PrimitiveLiteralContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _child = &ast;
            break;
        default:
            assert(false);
    }
}

NullLiteralContext::~NullLiteralContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::nullLiteralT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

StringLiteralContext::~StringLiteralContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::stringLiteralT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

XMLLiteralContext::~XMLLiteralContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::xmlLiteralT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

XMLTypeContext::~XMLTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::xmlType::create(startToken()));
            break;
        case 3:
            assert(NULL != _xmlSchema);
            setReturn(SPL::AST::xmlType::create(startToken(), *_xmlSchema));
            break;
        default:
            assert(false);
    }
}

void XMLTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 2:
            _xmlSchema = &ast;
            break;
        default:
            assert(false);
    }
}

TypeModifiersContext::~TypeModifiersContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            setReturn(SPL::AST::modifiers::create(startToken(), _typeModifier));
            break;
        default:
            assert(false);
    }
}

void TypeModifiersContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeModifier.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

TypeContext::~TypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _child);
            setReturn(*_child);
            break;
        default:
            assert(false);
    }
}

void TypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _child = &ast;
            break;
        default:
            assert(false);
    }
}

SubscriptContext::~SubscriptContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _expr_q);
            setReturn(
              SPL::AST::subscriptSlice::create(startToken(), SPL::AST::noneT::create(), *_expr_q));
            break;
        case 3:
            break;
        case 4:
            assert(NULL != _expr);
            setReturn(*_expr);
            break;
        case 5:
            setReturn(SPL::AST::subscriptSlice::create(startToken(), *_expr, *_expr_q));
            break;
        default:
            assert(false);
    }
}

void SubscriptContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr_q = &ast;
            break;
        case 3:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

ExprQContext::~ExprQContext()
{
    switch (state()) {
        case 0:
            break;
        case 1:
            setReturn(SPL::AST::noneT::create());
            break;
        case 2:
            assert(NULL != _expr);
            setReturn(*_expr);
            break;
        default:
            assert(false);
    }
}

void ExprQContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

SPLTypeContext::~SPLTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            parseState()._result = &errorNode();
            break;
        case 2:
            assert(NULL != _typeLiteral);
            parseState()._result = _typeLiteral;
            break;
        default:
            assert(false);
    }
}

void SPLTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeLiteral = &ast;
            break;
        default:
            assert(false);
    }
}

SPLExprContext::~SPLExprContext()
{
    switch (state()) {
        case 0:
        case 1:
            parseState()._result = &errorNode();
            break;
        case 2:
            assert(NULL != _expr);
            parseState()._result = _expr;
            break;
        default:
            assert(false);
    }
}

void SPLExprContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

ListTypeContext::~ListTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL == _typeDims);
            setReturn(SPL::AST::listType::create(startToken(), *_typeArgs1));
            break;
        case 3:
            assert(NULL != _typeDims);
            setReturn(SPL::AST::listType::create(startToken(), *_typeArgs1, *_typeDims));
            break;
        default:
            assert(false);
    }
}

void ListTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeArgs1 = &ast;
            break;
        case 2:
            _typeDims = &ast;
            break;
        default:
            assert(false);
    }
}

SetTypeContext::~SetTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL == _typeDims);
            setReturn(SPL::AST::setType::create(startToken(), *_typeArgs1));
            break;
        case 3:
            assert(NULL != _typeDims);
            setReturn(SPL::AST::setType::create(startToken(), *_typeArgs1, *_typeDims));
            break;
        default:
            assert(false);
    }
}

void SetTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeArgs1 = &ast;
            break;
        case 2:
            _typeDims = &ast;
            break;
        default:
            assert(false);
    }
}

MapTypeContext::~MapTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL == _typeDims);
            setReturn(SPL::AST::mapType::create(startToken(), *_typeArgs2));
            break;
        case 3:
            assert(NULL != _typeDims);
            setReturn(SPL::AST::mapType::create(startToken(), *_typeArgs2, *_typeDims));
            break;
        default:
            assert(false);
    }
}

void MapTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeArgs2 = &ast;
            break;
        case 2:
            _typeDims = &ast;
            break;
        default:
            assert(false);
    }
}

TypeArgs1Context::~TypeArgs1Context()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(_types.size() == 1);
            setReturn(SPL::AST::typeArgs::create(startToken(), _types));
            break;
        default:
            assert(false);
    }
}

void TypeArgs1Context::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _types.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

TypeArgs2Context::~TypeArgs2Context()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            assert(_types.size() == 2);
            setReturn(SPL::AST::typeArgs::create(startToken(), _types));
            break;
        default:
            assert(false);
    }
}

void TypeArgs2Context::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
        case 2:
            _types.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

TupleTypeContext::~TupleTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _tupleArgs);
            setReturn(SPL::AST::tupleType::create(startToken(), *_tupleArgs));
            break;
        default:
            assert(false);
    }
}

void TupleTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _tupleArgs = &ast;
            break;
        default:
            assert(false);
    }
}

TupleBodyContext::~TupleBodyContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::tupleAttrib::create(startToken(), _v));
            break;
        case 3:
            setReturn(SPL::AST::tupleExtend::create(startToken(), _v));
            break;
        default:
            assert(false);
    }
}

void TupleBodyContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _v.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

OptionalContext::~OptionalContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::optionalT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

OptionalTypeContext::~OptionalTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::optionalType::create(startToken(), *_type));
            break;
        default:
            assert(false);
    }
}

void OptionalTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _type = &ast;
            break;
        default:
            assert(false);
    }
}

AttributeDeclContext::~AttributeDeclContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            setReturn(SPL::AST::attributeDecl::create(startToken(), *_type, *_identifier));
            break;
        default:
            assert(false);
    }
}

void AttributeDeclContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _type = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

SimplePrimitiveTypeContext::~SimplePrimitiveTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _simplePrimitiveTypeName);
            setReturn(SPL::AST::primitiveType::create(startToken(), *_simplePrimitiveTypeName));
            break;
        default:
            assert(false);
    }
}

void SimplePrimitiveTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _simplePrimitiveTypeName = &ast;
            break;
        default:
            assert(false);
    }
}

StringTypeContext::~StringTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::rstringType::create(startToken()));
            break;
        case 3:
            assert(NULL != _typeDims);
            setReturn(SPL::AST::rstringType::create(startToken(), *_typeDims));
            break;
        case 4:
            setReturn(SPL::AST::ustringT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

void StringTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 2:
            _typeDims = &ast;
            break;
        default:
            assert(false);
    }
}

EnumTypeContext::~EnumTypeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::enumType::create(startToken(), _id_pc));
            break;
        default:
            assert(false);
    }
}

void EnumTypeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _id_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

SimplePrimitiveTypeNameContext::~SimplePrimitiveTypeNameContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::booleanT::create(startToken()));
            break;
        case 3:
            setReturn(SPL::AST::int8T::create(startToken()));
            break;
        case 4:
            setReturn(SPL::AST::int16T::create(startToken()));
            break;
        case 5:
            setReturn(SPL::AST::int32T::create(startToken()));
            break;
        case 6:
            setReturn(SPL::AST::int64T::create(startToken()));
            break;
        case 7:
            setReturn(SPL::AST::uint8T::create(startToken()));
            break;
        case 8:
            setReturn(SPL::AST::uint16T::create(startToken()));
            break;
        case 9:
            setReturn(SPL::AST::uint32T::create(startToken()));
            break;
        case 10:
            setReturn(SPL::AST::uint64T::create(startToken()));
            break;
        case 11:
            setReturn(SPL::AST::float32T::create(startToken()));
            break;
        case 12:
            setReturn(SPL::AST::float64T::create(startToken()));
            break;
        case 13:
            setReturn(SPL::AST::decimal32T::create(startToken()));
            break;
        case 14:
            setReturn(SPL::AST::decimal64T::create(startToken()));
            break;
        case 15:
            setReturn(SPL::AST::decimal128T::create(startToken()));
            break;
        case 16:
            setReturn(SPL::AST::complex32T::create(startToken()));
            break;
        case 17:
            setReturn(SPL::AST::complex64T::create(startToken()));
            break;
        case 18:
            setReturn(SPL::AST::timestampT::create(startToken()));
            break;
        case 19:
            setReturn(SPL::AST::blobT::create(startToken()));
            break;
        default:
            assert(false);
    }
}

TypeDimsContext::~TypeDimsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _expr);
            setReturn(SPL::AST::typeDims::create(startToken(), *_expr));
            break;
        default:
            assert(false);
    }
}

void TypeDimsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

TypeNameContext::~TypeNameContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _typeNameContents);
            setReturn(SPL::AST::typeName::create(startToken(), *_typeNameContents));
            break;
        default:
            assert(false);
    }
}

void TypeNameContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeNameContents = &ast;
            break;
        default:
            assert(false);
    }
}

TypeNameContentsContext::~TypeNameContentsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _typeNameStart);
            setReturn(*_typeNameStart);
            break;
        case 3:
            assert(NULL != _typeNameStart && NULL != _identifier);
            setReturn(SPL::AST::attributeExpr::create(startToken(), *_typeNameStart, *_identifier));
            break;
        default:
            assert(false);
    }
}

void TypeNameContentsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _typeNameStart = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

TypeNameStartContext::~TypeNameStartContext()
{
    switch (state()) {
        case 0:
        case 1:
        case 2:
            break;
            //        case 3: assert(NULL != _namespaceName && NULL != _identifier);
            //        setReturn(SPL::AST::qualifiedName::create(pool(), &startToken(),
            //        _namespaceName, _identifier)); break;
        case 3:
            assert(NULL != _namespaceName && NULL != _identifier);
            setReturn(SPL::AST::qualifierExpr::create(startToken(), *_namespaceName, *_identifier));
            break;
        case 4:
            assert(NULL != _identifier);
            setReturn(*_identifier);
            break;
        default:
            assert(false);
    }
}

void TypeNameStartContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _namespaceName = &ast;
            break;
        case 2:
            _identifier = &ast;
            break;
        default:
            assert(false);
    }
}

BlockStmtNoScopeContext::~BlockStmtNoScopeContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::blockStmtNoScope::create(startToken(), _stmtOrType));
            break;
        default:
            assert(false);
    }
}

void BlockStmtNoScopeContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _stmtOrType.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

BlockStmtContext::~BlockStmtContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::blockStmt::create(startToken(), _stmtOrType));
            break;
        default:
            assert(false);
    }
}

void BlockStmtContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _stmtOrType.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

ExprOrMappingContext::~ExprOrMappingContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            assert(NULL != _expr1);
            setReturn(*_expr1);
            break;
        case 3:
            assert(NULL != _expr1 && NULL != _expr2);
            setReturn(SPL::AST::mapping::create(startToken(), *_expr1, *_expr2));
            break;
        default:
            assert(false);
    }
}

void ExprOrMappingContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr1 = &ast;
            break;
        case 2:
            _expr2 = &ast;
            break;
        default:
            assert(false);
    }
}

void EqExprQContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _expr = &ast;
            break;
        default:
            assert(false);
    }
}

EqExprQContext::~EqExprQContext()
{
    switch (state()) {
        case 0:
            break;
        case 1:
            (NULL == _expr);
            setReturn(SPL::AST::noneT::create());
            break;
        case 2:
            assert(NULL != _expr);
            setReturn(*_expr);
            break;
        default:
            assert(false);
    }
}

AnnotationsContext::~AnnotationsContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::annotations::create(startToken(), _annotation));
            break;
        default:
            assert(false);
    }
}

void AnnotationsContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _annotation.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

AnnotationContext::~AnnotationContext()
{
    switch (state()) {
        case 0:
        case 1:
            break;
        case 2:
            setReturn(SPL::AST::annotation::create(startToken(), *_identifier));
            break;
        case 3:
            setReturn(SPL::AST::annotation::create(
              startToken(), *_identifier,
              createAnnotationLiteral(pool(), startToken(), _exprOrMapping_pc, parseState())));
            break;
        default:
            assert(false);
    }
}

void AnnotationContext::addChild(AstNode& ast)
{
    switch (state()) {
        case 1:
            _identifier = &ast;
            break;
        case 2:
            _exprOrMapping_pc.push_back(&ast);
            break;
        default:
            assert(false);
    }
}

#if 0
AnnotationTailContext::~AnnotationTailContext()
{
    switch(state()) {
        case 0:
        case 1: break;
        case 2: setReturn(createTupleLiteral(pool(), startToken(), _exprOrMapping_pc, parseState())); break;
        default: assert(false);
    }
}

void AnnotationTailContext::addChild(AstNode & ast)
{
    switch(state()) {
        case 1: _exprOrMapping_pc.push_back(&ast); break;
        default: assert(false);
    }
}
#endif
