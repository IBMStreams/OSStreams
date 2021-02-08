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

#ifndef Core_STATEMENT_H
#define Core_STATEMENT_H

#include <CodeGen/Config.h>
#include <CodeGen/Expression.h>
#include <UTILS/HashMapHelpers.h>

#include <cassert>

namespace SPL {
class RootTyp;
class SourceLocation;
class Expression;
class ExpressionEvaluator;
class FunctionHeadSym;
class LiteralReplacer;
class CompositeDefinition;
class Statement;

//! Indent depth for generatedCode
enum
{
    CodeIndent = 4
};

/// Hold Statements at compile time
/// Statement IR classes
/// This will ALWAYS be a tree (not a DAG), and we therefore use
/// the following rules to handle memory
/// 1 - All allocation of Statement classes shall be done with 'new'
/// 2 - A statement (when deleted) will delete all sub statements
class Statement
{
  public:
    /// Statement Kind
    enum Kind
    {
        Block,    //!< list of Statements
        Decl,     //!< [mutable] local type declaration [ = \<expn\> ]
        If,       //!< If \<cond\> then \<stmt\> [ else \<stmt\> ]
        For,      //!< for (\<type\> ID in \<expn\>) \<stmt\>
        While,    //!< while (\<expn\>) \<stmt\>
        Expr,     //!< \<expn\>  (includes assignment)
        Break,    //!< break
        Continue, //!< continue
        Return,   //!< return [\<expn\>]
        numStatementKind
    };

  protected:
    /// Constructor
    /// @param k Statement Kind
    /// @param location Source Location
    Statement(Kind k, const SourceLocation& inlocation)
      : _kind(k)
      , _location(inlocation)
    {}

  public:
    /// Destructor
    virtual ~Statement();

    /// Return the kind of a statement
    /// @returns statement type
    Kind getKind() const { return _kind; }

    /// Return Statement source location
    /// @return the coordinates for the statement
    virtual const SourceLocation& location() const { return _location; }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const = 0;

    /// Simplify the expressions in the statement
    virtual void simplifyExpression(ExpressionEvaluator& eval) = 0;

    /// Replace the literals in the expressions in the statement
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP) = 0;

    /// Generate C++ code for statement to output stream
    /// @param ostr   Output Stream
    void generate(std::ostream& ostr) const { generate(ostr, 0); }

    /// Generate C++ code for statement to output stream
    /// @param ostr   Output Stream
    /// @param depth  Statement nesting depth
    virtual void generate(std::ostream& ostr, uint32_t depth) const = 0;

    /// Clone a statement
    /// @return copy of *this
    virtual Statement& clone() const = 0;

    /// Collect Type and Function Information
    /// @param types   if non-NULL, named types will be added
    /// @param fcns    if non-NULL, referenced functions will be added
    /// @param inFcns  if true, walk into references functions
    void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                 std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                 bool inFcns) const;

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

  protected:
    Kind _kind;
    const SourceLocation& _location;
};
#define STMT_IS_AS_SPECIALIZATIONS(isEnum, asType)                                                 \
    template<>                                                                                     \
    inline bool Statement::is<asType>() const                                                      \
    {                                                                                              \
        return getKind() == Statement::isEnum;                                                     \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& Statement::as<asType>()                                                         \
    {                                                                                              \
        assert(dynamic_cast<asType*>(this) != 0);                                                  \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline const asType& Statement::as<asType>() const                                             \
    {                                                                                              \
        assert(dynamic_cast<const asType*>(this) != 0);                                            \
        return *static_cast<const asType*>(this);                                                  \
    }

std::ostream& operator<<(std::ostream& ostr, const Statement& v);
std::ostream& operator<<(std::ostream& ostr, Statement::Kind v);

class BlockStatement : public Statement
{
  public:
    /// Constructor
    /// @param l Source Location
    /// @param stmts Statements contained in the block
    BlockStatement(const SourceLocation& l, const std::vector<Statement*>* stmts);

    /// Destructor
    ~BlockStatement();

    /// Access the list of statements in the block
    /// @returns vector of statements
    std::vector<Statement*>& getStmts() { return _stmts; }
    const std::vector<Statement*>& getStmts() const { return _stmts; }

    /// Append a new statement to the block
    /// @param stmt Statement to be appended
    void append(Statement& stmt) { _stmts.push_back(&stmt); }

    /// Mark a block to be unnested in FE
    void setUnnest() { _unnest = true; }

    /// Should a block be unnested?
    /// @return 'true' if block was marked as unnested
    bool getUnnest() const { return _unnest; }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP);
    virtual void simplifyExpression(ExpressionEvaluator& eval);
    virtual BlockStatement& clone() const;

    std::vector<Statement*> _stmts;
    bool _unnest;
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(Block, BlockStatement)

class DeclStatement : public Statement
{
  public:
    /// Constructor
    /// @param l Source Location
    /// @param isMutable 'true' if the id can be assigned to
    /// @param type Variable type
    /// @param id Variable name
    /// @param expn Optional initializer expression
    /// @param isStateVar 'true' if this is a state variable declaration
    DeclStatement(const SourceLocation& l,
                  bool isMutable,
                  const RootTyp& type,
                  const std::string& id,
                  Expression* expn,
                  bool isStateVar = false);
    /// Destructor
    ~DeclStatement();

    /// Return declaration id
    /// @return id
    const std::string& getId() const { return _id; }

    /// Set the declaration id
    /// @param id Symbol name
    void setId(const std::string& id) { _id = id; }

    /// make declaration a state var
    void setStateVar() { _isStateVar = true; }

    /// Return 'mutability' of declaration
    /// @return 'true' if declaration is mutable
    bool getMutable() const { return _mutable; }

    /// Return declaration type
    /// @return type of declaration
    const RootTyp& getType() const { return _type; }

    /// Return declaration initialization expression
    /// @return init expression (if present) or NULL
    const Expression* getExpression() const { return _expn; }
    Expression* getExpression() { return _expn; }

    // Set declaration initialization expression
    /// @param expn initialization expression
    void setExpression(Expression* expn) { _expn = expn; }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  protected:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    virtual void simplifyExpression(ExpressionEvaluator& eval);
    virtual DeclStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP);

    std::string _id;
    const RootTyp& _type;
    bool _mutable;
    Expression* _expn;
    bool _isStateVar;
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(Decl, DeclStatement)

class IfStatement : public Statement
{
  public:
    /// Constructor
    /// @param l Source Location
    /// @param condition 'if' condition expression
    /// @param thenClause 'then' statement
    /// @param elseClause Optional 'else' statement
    IfStatement(const SourceLocation& l,
                Expression& condition,
                Statement& thenClause,
                Statement* elseClause);

    /// Destructor
    ~IfStatement();

    /// Get the condition expression
    /// @returns condition
    const Expression& getCondition() const { return *_cond; }
    Expression& getCondition() { return *_cond; }

    /// Get the then statement
    /// @returns statement for then
    const Statement& getThenClause() const { return *_then; }
    Statement& getThenClause() { return *_then; }
    void setThenClause(Statement* thenClause) { _then = thenClause; }

    /// Get the else statement (if present)
    /// @returns statement for else clause (or NULL)
    const Statement* getElseClause() const { return _else; }
    Statement* getElseClause() { return _else; }
    void setElseClause(Statement* elseClause) { _else = elseClause; }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void simplifyExpression(ExpressionEvaluator& eval);
    virtual IfStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& ostr, uint32_t depth) const;

  protected:
    Expression* _cond;
    Statement* _then;
    Statement* _else;
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(If, IfStatement)

class ForStatement : public Statement
{
  public:
    /// Constructor
    /// @param l Source Location
    /// @param type Variable type
    /// @param id Variable name
    /// @param expn Expression to iterated over
    /// @param stmt Statement to be executed
    ForStatement(const SourceLocation& l,
                 const RootTyp& type,
                 const std::string& id,
                 Expression& expn,
                 Statement& stmt);

    /// Destructor
    ~ForStatement();

    /// Return identifier in 'for' statement
    /// @return id
    const std::string& getId() const { return _id; }

    /// Return id type
    /// @return type of declaration
    const RootTyp& getType() const { return _type; }

    /// Return expression that will be iterated over
    /// @return expression
    const Expression& getExpression() const { return *_expn; }
    Expression& getExpression() { return *_expn; }

    /// Return for statement 'body'
    /// @return statement to be iterated
    const Statement& getStatement() const { return *_stmt; }
    Statement& getStatement() { return *_stmt; }
    void setStatement(Statement* stmt) { _stmt = stmt; }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    void generateOptimizedForRange(std::ostream& ostr, uint32_t depth, CallExpression& call) const;
    virtual void simplifyExpression(ExpressionEvaluator& eval);
    virtual ForStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP);

    const RootTyp& _type;
    std::string _id;
    Expression* _expn;
    Statement* _stmt;
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(For, ForStatement)

class WhileStatement : public Statement
{
  public:
    /// Create a While statement
    /// @param l         source location
    /// @param expn      while expression
    /// @param stmt      statement to be executed
    WhileStatement(const SourceLocation& l, Expression& expn, Statement& stmt);

    /// Destructor
    ~WhileStatement();

    /// Get the expression
    /// @returns expn
    const Expression& getExpression() const { return *_expn; }
    Expression& getExpression() { return *_expn; }

    /// Get the statement
    /// @returns statement
    const Statement& getStatement() const { return *_stmt; }
    Statement& getStatement() { return *_stmt; }
    void setStatement(Statement* stmt) { _stmt = stmt; }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    virtual void simplifyExpression(ExpressionEvaluator& eval);
    virtual WhileStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP);

    Expression* _expn;
    Statement* _stmt;
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(While, WhileStatement)

class ExpressionStatement : public Statement
{
  public:
    /// @param l Source Location
    /// @param expn Expression to be executed
    ExpressionStatement(const SourceLocation& l, Expression& expn);

    /// Destructor
    ~ExpressionStatement();

    /// Get the expression
    /// @returns condition
    const Expression& getExpression() const { return *_expn; }
    Expression& getExpression() { return *_expn; }

    /// Set the expression
    /// @param expn new Expression
    void setExpression(Expression& expn)
    {
        if (_expn) {
            delete _expn;
        }
        _expn = &expn;
    }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    virtual void simplifyExpression(ExpressionEvaluator& eval);
    virtual ExpressionStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP);

    Expression* _expn;
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(Expr, ExpressionStatement)

class BreakStatement : public Statement
{
  public:
    /// Constructor
    /// @param l Source Location
    BreakStatement(const SourceLocation& l);

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    virtual void simplifyExpression(ExpressionEvaluator& eval) {}
    virtual BreakStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP) {}
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(Break, BreakStatement)

class ContinueStatement : public Statement
{
  public:
    /// Constructor
    /// @param l Source Location
    ContinueStatement(const SourceLocation& l);

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    virtual void simplifyExpression(ExpressionEvaluator& eval) {}
    virtual ContinueStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP) {}
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(Continue, ContinueStatement)

class ReturnStatement : public Statement
{
  public:
    /// Constructor
    /// @param l Source Location
    /// @param expn Optional expression containing return value
    ReturnStatement(const SourceLocation& l, Expression* expn);

    /// Destructor
    ~ReturnStatement();

    /// Get the expression
    /// @pre hasExpression()
    /// @returns expn
    const Expression& getExpression() const
    {
        assert(_expn != NULL);
        return *_expn;
    }
    Expression& getExpression()
    {
        assert(_expn != NULL);
        return *_expn;
    }

    /// Check if there is an expression
    /// @return true if expression exist
    bool hasExpression() const { return _expn != NULL; }

    /// Print a Statement
    /// @param  s     stream to print to
    /// @param depth The number of CodeIndent space indents to start indenting from
    /// @return       s
    virtual std::ostream& print(std::ostream& s, uint32_t depth) const;

  private:
    virtual void generate(std::ostream& ostr, uint32_t depth) const;
    virtual void simplifyExpression(ExpressionEvaluator& eval);
    virtual ReturnStatement& clone() const;
    virtual void replaceLiterals(LiteralReplacer& lit, bool onlySTP);

    Expression* _expn;
};
// is/as specializations
STMT_IS_AS_SPECIALIZATIONS(Return, ReturnStatement)
};

#endif /* Core_STATEMENT_H */
