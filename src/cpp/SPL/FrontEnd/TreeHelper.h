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

#ifndef SPL_TREE_HELPER_H
#define SPL_TREE_HELPER_H

#include <SPL/FrontEnd/AstNode.h>
#include <SPL/FrontEnd/FrontEndStage.h>
//#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

//#include <antlr3interfaces.h>
#include <cassert>
#include <string>

/** Definitions that mirror Antlr-generated token "#define"s, to avoid
 polluting the namespace by "#include"ing the grammar headers. */
#include "SPL/CodeGen/Expression.h"

namespace SPL {
class AstPoolData;
class SymbolTableEntry;
class Statement;
class ContextBase;

/// Target of a "void * u" pointer reachable from an Antlr-generated
/// tree parser, used to remember the file name and/or source string
/// from which the AST was originally parsed.
class DLL_PUBLIC ParserContext
{
  public:
    /// Constructor.
    ParserContext(std::string const& fileName);

    /// Constructor.
    ParserContext(std::string const& fileName, std::string const& sourceString);

    /// Destructor.
    virtual ~ParserContext() {}

    /// File name from which the AST was originally parsed.
    virtual std::string const& fileName() const;
    /// Index of file name from which the AST was originally parsed in
    /// the unique table used by source locations.
    virtual int fileNameIdx() const { return _fileNameIdx; }
    /// Source location of last syntax error, or NULL if no syntax errors.
    SourceLocation const* _lastSyntaxError;
    /// Stack of syntactic context types, tracked during parsing, used
    /// for error reporting.
    std::vector<int> _paraphrase;
    /// Source string from which the AST was originally parsed.
    virtual std::string const& sourceString() const;
    /// Index of source string from which the AST was originally parsed
    /// in the unique table used by source locations.
    virtual int sourceStringIdx() const { return _sourceStringIdx; }

    /// Takes the given file and line and remaps that to a different file and line
    /// Used when processing temporary files during translation
    /// The default is to do nothing
    virtual void mapFileAndLine(int& file, int& line) const {}
    SPL::ContextBase& currentContext()
    {
        assert(!_contextStack.empty());
        return *_contextStack.back();
    }
    SPL::ContextBase* pushContext(SPL::ContextBase& context)
    {
        SPL::ContextBase* previousContext = _contextStack.empty() ? NULL : &currentContext();
        _contextStack.push_back(&context);
        return previousContext;
    }
    void popContext() { _contextStack.pop_back(); }
    AstNode* _result;

  private:
    int _fileNameIdx;
    int _sourceStringIdx;
    std::vector<SPL::ContextBase*> _contextStack;
};

/// Subclass of ParserContext for use by the tree grammar that does
/// pretty-printing, to keep track of state used for correct
/// indentation, as well as keeping a handle to the output stream.
class PrinterContext : public ParserContext
{
    int _indentLevel;
    int _indentSize;
    bool _isLineStart;

  protected:
    std::ostream& _os;

  public:
    /// Constructor.
    PrinterContext(std::string inputFileName, std::ostream& outputStream);
    /// Increase indentation level.
    virtual void indent() { _indentLevel++; }
    /// Decrease indentation level.
    virtual void outdent()
    {
        assert(_indentLevel > 0);
        _indentLevel--;
    }
    /// Print a newline.
    virtual void pln();
    /// Print a string; if this is the start of a line, precede that
    /// string by the appropriate amount of indentation.
    virtual void print(std::string const&);

    /// Propagate any comments that are within the token range
    /// specified on the given node.  The default
    /// behaviour for this is to do nothing
    virtual void printComments(AstNode& ast) {}
};

/// Bundles up the actual AstNode that is the root of the abstract
/// syntax tree (AST) with other Antlr-internal stuff that ought to be
/// released when we are done with the AST.
class SPLAst
{
  protected:
    AstNode* _ast;
    pANTLR3_INPUT_STREAM _input;
    pANTLR3_COMMON_TOKEN_STREAM _tokens;
    SPLAst(pANTLR3_INPUT_STREAM);
    static pANTLR3_INPUT_STREAM antlrStreamFromString(std::string const& fileName,
                                                      std::string const& sourceString);
    virtual void tokenize(pANTLR3_TOKEN_SOURCE);

  public:
    /// Create an antlr input stream given a file
    static pANTLR3_INPUT_STREAM antlrStreamFromFile(std::string const& fileName);

    /// Parse using the non-terminal 'compilationUnit' as the start symbol from a file.
    static SPLAst& compilationUnitFromFile(std::string const& fileName);
    /// Parse using the non-terminal 'expression' as the start symbol from a string.
    static SPLAst& expressionFromString(std::string const& fileName,
                                        std::string const& sourceString);

    /// Parse using the non-terminal 'function prototype' as the start symbol from a string.
    static SPLAst& functionPrototypeFromString(std::string const& fileName,
                                               uint32_t line,
                                               uint32_t column,
                                               std::string const& sourceString,
                                               bool isNativeFunction) DLL_PUBLIC;

    /// Parse using the non-terminal 'SPLtype' as the start symbol from a string.
    static SPLAst& SPLTypeFromString(std::string const& fileName,
                                     std::string const& sourceString,
                                     bool& hadError);
    /// Destructor: null the AST, close the input, free the tokens.
    virtual ~SPLAst();
    /// The root node of the abstract syntax tree.
    virtual AstNode& ast();
    /// The root node of the abstract syntax tree, or NULL if none.
    virtual AstNode* astPtr();
    /// The parser context, i.e., what to stick into a 'void * u' of an Antlr parser.
    virtual ParserContext& parserContext() = 0;
    /// Replace the AST.
    virtual void setAst(AstNode*);

    /// Access the token stream
    pANTLR3_COMMON_TOKEN_STREAM tokens() const { return _tokens; }
};

class InverseAstState;

/// Mapping from AST nodes to their parents.
class InverseAst
{
    InverseAstState* _state;

  public:
    /// Constructor.
    InverseAst(AstNode& root);
    /// Destructor.
    virtual ~InverseAst();
    /// Child index in parent, or -1 if root or not descendant of root.
    int indexInParent(AstNode& ast);
    /// Parent AST, or NULL if root, or error if not descendant of root.
    AstNode* parent(AstNode& ast);
};

/// Print an AST in a Lisp-like notation for debugging purposes.
std::ostream& operator<<(std::ostream&, AstNode*) DLL_PUBLIC;
std::ostream& operator<<(std::ostream&, AstNode&) DLL_PUBLIC;

/// Add new child to parent AST.
AstNode& addAstChild(AstPool& pool, AstNode& parent, AstNode& child);

/// Run all front-end stages on an expression AST.
void analyzeExpressionAst(SPLAst&) DLL_PUBLIC;

/// Find the given child in the given parent
/// Returns -1 if not found
int findAstChild(AstNode& parent, AstNode& child);

/// Left-most tree node with non-zero line number, if any.
AstNode* astNodeWithFirstLocation(AstNode& ast);

/// Return the subtree node with the leftmost location
/// Note that this checks the children only, not the given node
AstNode* astChildWithLeftmostLocation(AstNode& ast);

/// Return the tree node with the leftmost location
AstNode* astNodeWithLeftmostLocation(AstNode& ast);

/// Return the subtree node with the rightmost location
AstNode* astNodeWithRightmostLocation(AstNode& ast);

/// Clear all the 'void * u' pointers from AST nodes.
void astClearUPointers(AstNode&) DLL_PUBLIC;

/// Initialize the source file index in all the AST nodes in the tree.
void astInitSourceFileIdx(AstPool& pool, AstNode& ast, int idx) DLL_PUBLIC;

/// Deep-clone an AST, and copy over the symbol table entries too, if any.
AstNode& astClone(AstPool&, AstNode&);

/// Is any node in this AST an error node that resulted from an Antlr parser error?
bool astHasErrorNodes(AstNode& ast, ParserContext const* pCtx);

/// Assuming this AST represents an expression with a (prefix or infix) expression
/// operator, return that operator's enum value.
Expression::Op astNodeOp(AstNode&);

/// Get the AST nodes as a stream that can be fed to an antlr tree parser.
pANTLR3_COMMON_TREE_NODE_STREAM astNodeStream(AstNode&) DLL_PUBLIC;

/// Source file index of AST node for source-level error reporting.
int astSourceFileIdx(AstNode& ast) DLL_PUBLIC;

/// Last front-end stage that has been completed on this AST node.
FrontEndStage astStage(AstNode&);

/// Symbol table entry of AST node.
SymbolTableEntry& astSymbol(AstNode&);

/// Symbol table entry of AST node, if any; only use if result may be NULL.
SymbolTableEntry* astSymbolPtr(AstNode&);

/// Copy the file index, line and column number from the source node to the destination node.
void copyAstPosition(AstPool& pool, AstNode& dst, AstNode& src);

/// Copy the symbol from the source node to the destination node.
void copyAstSymbol(AstPool& pool, AstNode& dst, AstNode& src);

/// Grab the preceding SPL-doc comments, if any.
AstNode& grabSplDoc(AstPool& pool, pANTLR3_TOKEN_STREAM tokStream, pANTLR3_COMMON_TOKEN tok);

/// Grab the SPL-doc comments for the compilation unit, if any
AstNode& grabSplDocForCompilationUnit(AstPool& pool,
                                      pANTLR3_TOKEN_STREAM tokStream,
                                      pANTLR3_COMMON_TOKEN tok);

/// Create a curly literal given a vector of expressions or mappings
AstNode& createCurlyLiteral(AstPool& pool,
                            AstToken& start,
                            std::vector<AstNode*>& v,
                            ParserContext const& ctx);

/// Create a tuple literal given a vector of expressions
AstNode& createTupleLiteral(AstPool& pool,
                            AstToken& start,
                            std::vector<AstNode*>& v,
                            ParserContext const& ctx);

/// Create an annotation  literal given a vector of expressions
AstNode& createAnnotationLiteral(AstPool& pool,
                                 AstToken& start,
                                 std::vector<AstNode*>& v,
                                 ParserContext const& ctx);

// Create either a prefixExpr or convert the literal to a negative
AstNode& prefixOrPrependMinus(AstPool& pool,
                              AstToken& start,
                              AstNode& prefixOp,
                              AstNode& prefixExpr);

/// Statement IR of AST node, if any.
Statement* getAstStatement(AstNode& ast);

/// Set the statement IR for the AST node; error if already set.
void setAstStatement(AstPool& pool, AstNode& ast, Statement& s);

/// Expression IR of AST node.
Expression& astExpression(AstNode& ast) DLL_PUBLIC;

/// Expression IR of AST node, if any; only use if result may be NULL.
Expression* astExpressionPtr(AstNode& ast) DLL_PUBLIC;

/// Set the expression IR for the AST node; error if already set.
void setAstExpression(AstPool& pool, AstNode& ast, Expression& e);

/// clear out any expression that was associated with this AST node
void clearAstExpression(AstPool& pool, AstNode& ast);

/// Set the expression IR for the AST node; overwrite previous if already set.
void resetAstExpression(AstPool& pool, AstNode& ast, Expression& e);

/// The integral 'type' of the node

/// Overrides default behavior of Antlr-generated parsers upon detecting an error.
void displayRecognitionError(pANTLR3_BASE_RECOGNIZER, uint8_t**) DLL_PUBLIC;

/// Populate the result with the ASTs of all calls under the root ast.
void findAllCalls(AstNode& ast, std::vector<AstNode*>& result);

/// Populate the result with the ASTs of all variable occurrences under the root ast.
void findAllVariables(AstNode& ast, std::vector<AstNode*>& result);

/// Given an identifier, find the input or output stream or port connected to the given
/// opInvoke that has that identifier.
int findStreamInOpInvoke(std::string const& streamName,
                         AstNode& opInvokeAst,
                         AstNode** foundAst,
                         bool* isInput,
                         int* portIndex,
                         bool* isPortAlias);

/// True for function formals, local variable declarations, for-loop iterator variables
bool isVarDefAst(AstNode&);

/// Return child number i from opActualExprs or opActualName or opActualType ast.
AstNode& opActualChild(AstNode& ast, int i);

/// Print AST back out in SPL source syntax, properly indented.
std::ostream& prettyPrint(std::ostream&, AstNode&) DLL_PUBLIC;

/// Print expresion AST back out in SPL source syntax.
std::ostream& printExpr(std::ostream&, AstNode&);

/// Decorate AST node with the source file index.
void setAstSourceFileIndex(AstPool& pool, AstNode& ast, int idx);

/// Decorate AST node with the front end stage.
void setAstStage(AstPool& pool, AstNode&, FrontEndStage);

/// Decorate AST node with symbol table entry; error if already set.
void setAstSymbol(AstPool& pool, AstNode&, SymbolTableEntry&);

/// Decorate AST node with symbol table entry; overwrite if already set.
void resetAstSymbol(AstPool& pool, AstNode&, SymbolTableEntry&);

/// Remove any symbol associated with this AST node
void clearAstSymbol(AstPool& pool, AstNode& ast);

/// Replace a child AST.
void resetAstChild(AstPool& pool, AstNode& parent, int idx, AstNode& newChild);

/// Replace all child ASTs.
void resetAstChildren(AstPool& pool, AstNode& parent, std::vector<AstNode*>& newChildren);
}

#define runAntlrAstParser(prsName, ruleName, astRoot, context)                                     \
    {                                                                                              \
        p##prsName treeParser = prsName##New(SPL::astNodeStream(*astRoot));                        \
        assert(NULL != treeParser);                                                                \
        treeParser->pTreeParser->rec->state->userp = &context;                                     \
        treeParser->pTreeParser->rec->displayRecognitionError = SPL::displayRecognitionError;      \
        treeParser->ruleName(treeParser);                                                          \
        treeParser->free(treeParser);                                                              \
    }

#define runAntlrAstTransformer(prsName, ruleName, astRoot, context)                                \
    {                                                                                              \
        p##prsName treeParser = prsName##New(SPL::astNodeStream(*astRoot));                        \
        assert(NULL != treeParser);                                                                \
        treeParser->pTreeParser->rec->state->userp = &context;                                     \
        treeParser->pTreeParser->rec->displayRecognitionError = SPL::displayRecognitionError;      \
        astRoot = treeParser->ruleName(treeParser).tree;                                           \
        astClearUPointers(*astRoot);                                                               \
        treeParser->free(treeParser);                                                              \
    }

#endif /*SPL_SYMBOL_TABLE_H*/

//  LocalWords:  opActual
