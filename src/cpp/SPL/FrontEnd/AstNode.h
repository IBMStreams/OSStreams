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

#ifndef SPL_AST_NODE_H
#define SPL_AST_NODE_H

#undef VERSION
#undef PACKAGE
#undef PACKAGE_VERSION
#undef PACKAGE_TARNAME
#undef PACKAGE_STRING
#undef PACKAGE_NAME
#undef PACKAGE_BUGREPORT
#include <antlr3.h>
#include <antlr3interfaces.h>
#undef VERSION
#undef PACKAGE
#undef PACKAGE_VERSION
#undef PACKAGE_TARNAME
#undef PACKAGE_STRING
#undef PACKAGE_NAME
#undef PACKAGE_BUGREPORT

#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/Runtime/Utility/Singleton.h>

#include <SPL/Runtime/Utility/Visibility.h>

#include <boost/pool/object_pool.hpp>
#include <cassert>
#include <string>
#include <vector>

namespace SPL {

typedef ANTLR3_BASE_TREE_struct AstNode;
typedef ANTLR3_COMMON_TOKEN_struct AstToken;

class AstNodeBase;
class SymbolTableEntry;
class Expression;
class Statement;

class AstInformation
{
  public:
    AstInformation()
      : _stage(FrontEndStage::PARSER)
      , _sym(NULL)
      , _expn(NULL)
      , _stmt(NULL)
      , _sourceFileIdx(-1)
    {}
    AstInformation(const AstInformation& other);

    Expression* getExpression() const { return _expn; }
    void setExpression(Expression* e) { _expn = e; }

    Statement* getStatement() const { return _stmt; }
    void setStatement(Statement* s) { _stmt = s; }

    FrontEndStage getStage() const { return _stage; }
    void setStage(FrontEndStage s) { _stage = s; }

    SymbolTableEntry* getSymbol() const { return _sym; }
    void setSymbol(SymbolTableEntry* s) { _sym = s; }

    int getSourceFileIndex() const { return _sourceFileIdx; }
    void setSourceFileIndex(int idx) { _sourceFileIdx = idx; }

    void copy(AstInformation& other);

  private:
    FrontEndStage _stage;
    SymbolTableEntry* _sym;
    Expression* _expn;
    Statement* _stmt;
    int _sourceFileIdx;
};

class AstPoolData
{
    boost::object_pool<AstInformation> _infoPool;
    pANTLR3_STRING_FACTORY _stringFactory;
    pANTLR3_BASE_TREE_ADAPTOR _treeAdaptor;
    pANTLR3_STRING newStr(std::string const& cppString);

  public:
    AstPoolData();
    AstNode& addChild(AstNode& parentAst, AstNode& childAst);
    AstNode& createNode(AstToken& token);
    AstNode& createRawNode(AstToken& token);
    AstToken& createToken(ANTLR3_UINT32 tokenType, std::string const& text);
    AstNode& createTypeText(ANTLR3_UINT32 tokenType, pANTLR3_UINT8 text);
    AstInformation& ensureAstInformation(AstNode& ast);
    void setChild(AstNode& parentAst, ANTLR3_UINT32 i, AstNode& childAst);
    AstNode& dupNode(AstNode& ast);
};

class AstPool : public Singleton<AstPool>
{
    AstPoolData& _data;

  public:
    AstPool();
    AstPoolData* operator->() { return &_data; }
};

/// String representation of the root node's token.
std::string astText(AstNode&);

/// Line number of AST node for source-level error reporting.
int astLine(AstNode&) DLL_PUBLIC;

/// Column number of AST node for source-level error reporting.
int astColumn(AstNode&) DLL_PUBLIC;

AstInformation* getAstInformation(AstNode& ast);

/// The integral 'type' of the node
int32_t astType(AstNode& ast);

/// Given an AstNode, get the C++ object that wraps it.
AstNodeBase& wrapper(AstNode& ast);

/// Retrieve the token for the given AST node.
AstToken& astToken(AstNode& ast) DLL_PUBLIC;

/// Retrieve an AST child by index; you should try to avoid calling this directly when possible,
/// by using accessor functions of the form AST::parentNonterminal::childNonterminal(parentAst).
AstNode& astChild(AstNode&, int) DLL_PUBLIC;

/// Number of immediate children of AST node.
int astSize(AstNode&);

} // namespace SPL

#include <SPL/FrontEnd/AstNodeGen.h>

namespace SPL {

namespace AST {

namespace compositePort {
PUSH_DLL_PUBLIC
AstNode& create(AstToken& start, AstNode* streamType, AstNode& id);
bool is(AstNode& ast);
bool has_type(AstNode& ast);
AstNode& type(AstNode& ast);
AstNode& id(AstNode& ast);
POP_DLL_PUBLIC
} // namespace compositePort

namespace compositeBody {
AstNode& create(AstToken& start, std::vector<AstNode*>& children);
AstNode& create(AstNode* formal, AstNode* types, AstNode* graph, AstNode* config);
bool is(AstNode&);
bool has_formal(AstNode&);
AstNode& formal(AstNode&);
bool has_types(AstNode&);
AstNode& types(AstNode&);
bool has_graph(AstNode&);
AstNode& graph(AstNode&);
bool has_config(AstNode&);
AstNode& config(AstNode&);
} // namespace compositeBody

namespace portInputs {
AstNode& create(AstToken& start, AstNode* type, std::vector<AstNode*>& children, AstNode* alias);
bool is(AstNode& ast);
bool has_type(AstNode& ast);
AstNode& type(AstNode& ast);
int idCount(AstNode& ast);
int idStart(AstNode& ast);
AstNode& id(AstNode& ast, int iRel);
bool has_alias(AstNode& ast);
AstNode& alias(AstNode& ast);
} // namespace portInputs

namespace opInvokeBody {
AstNode& create(AstToken& start, std::vector<AstNode*>& children);
bool is(AstNode& ast);
bool has_logic(AstNode& ast);
AstNode& logic(AstNode& ast);
bool has_window(AstNode& ast);
AstNode& window(AstNode& ast);
bool has_actual(AstNode& ast);
AstNode& actual(AstNode& ast);
bool has_output(AstNode& ast);
AstNode& output(AstNode& ast);
bool has_config(AstNode& ast);
AstNode& config(AstNode& ast);
} // namespace opInvokeBody

namespace infixExpr {
AstNode& create(AstToken& start, AstNode& opAst, AstNode& lhsAst, AstNode& rhsAst);
bool is(AstNode& ast);
AstNode& lhs(AstNode& ast);
AstNode& rhs(AstNode& ast);
void setLhs(AstNode& ast, AstNode& newChild);
void setRhs(AstNode& ast, AstNode& newChild);
} // namespace infixExpr

} // namespace AST
} // namespace SPL
#endif // SPL_AST_NODE_H
