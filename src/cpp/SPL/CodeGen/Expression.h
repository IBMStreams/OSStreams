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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <SPL/Utility/ModelDumper.h>
#include <UTILS/HashMapHelpers.h>

#include <cassert>

namespace SPL {
class RootTyp;
class SourceLocation;
class ExpressionValue;
class Literal;
class ExpressionEvaluator;
class FunctionHeadSym;
class Expression;
class CompositeOperatorInstance;

//! Class to handle replacement of literals
class LiteralReplacer
{
  public:
    /// Constructor
    LiteralReplacer() { _nextId = 0; }

    /// Destructor
    ~LiteralReplacer();

    /// Reset the variable for a new routine
    void resetId() { _nextId = 0; }

    /// Put a new literal into the table.
    /// @param  lit   Literal to be added
    /// @param  loc   Source Location of the literal
    /// @param  index Set to index into literal table
    /// @return the variable number to be used for this replacement (i.e. 0 in lit$0)
    uint32_t addLiteral(const Literal& lit, const SourceLocation& loc, uint32_t& index);

    /// Put a new argument literal into the table.
    /// @param  lit   Literal of the correct type (rstring or list<rstring>)
    /// @param  loc   Source Location of the literal
    /// @param  kind  Desired kind of get argument call
    ///               kind must be one of none/named/namedList
    /// @param  compositeName location of the reference (if kind != none)
    /// @param  arg   Argument name
    /// @param  required   'true' if a value must be supplied at submission time
    /// @param  index Set to index into literal table
    /// @return the variable number to be used for this replacement (i.e. 0 in lit$0)
    uint32_t addArgument(const Literal& lit,
                         const SourceLocation& loc,
                         const std::string& kind,
                         const std::string& compositeName,
                         const std::string& arg,
                         bool required,
                         uint32_t& index);

    /// Return the table of saved literals
    /// @return the table
    const std::vector<const Literal*>& getLiterals() const { return _lits; }

    class ArgInfo
    {
      public:
        //
        // kind must be one of named/namedList
        ArgInfo(const std::string& kind,
                const std::string& compositeName,
                const std::string* name,
                bool required)
          : _kind(kind)
          , _compositeName(compositeName)
          , _name(name)
          , _required(required)
        {}
        ~ArgInfo() { delete _name; }
        ArgInfo(const ArgInfo& rhs);
        ArgInfo& operator=(const ArgInfo& rhs);
        bool operator==(const ArgInfo& rhs) const;
        const std::string& getKind() const { return _kind; }
        const std::string& getCompositeName() const { return _compositeName; }
        const std::string* getName() const { return _name; }
        bool getRequired() const { return _required; }

      private:
        std::string _kind;
        std::string _compositeName;
        const std::string* _name;
        bool _required;
    };

    /// Return the table of saved arguments
    /// @return the table
    const std::vector<ArgInfo>& getArguments() const { return _arguments; }

    /// Return the table of saved source locations
    /// @return the table
    const std::vector<const SourceLocation*>& getSourceLocations() const { return _sourceLocs; }

  private:
    uint32_t _nextId;
    std::vector<const Literal*> _lits;
    std::vector<ArgInfo> _arguments;
    std::vector<const SourceLocation*> _sourceLocs;
};

/// Expression IR classes
/// Hold Expressions at compile time
/// This will ALWAYS be a tree (not a DAG), and we therefore use
/// the following rules to handle memory
/// 1 - All allocation of Expression classes shall be done with 'new'
/// 2 - An expression (when deleted) will delete all sub expressions

class Expression
{
  public:
    /// Expression Kind
    enum Kind
    {
        Attribute,           //!< A . B
        Binary,              //!< A op B
        Call,                //!< A([args])
        Cast,                //!< T(A)
        Conditional,         //!< A ? B : C
        Enum,                //!< a SPL Enum Value
        Lit,                 //!< any SPL literal
        LiteralSymbol,       //!< Rewritten Literal
        Postfix,             //!< A--
        Prefix,              //!< --A
        StreamHistorySymbol, //!< A
        StreamSymbol,        //!< A
        Subscript,           //!< A [ B ]
        Symbol,              //!< A
        CustomLiteral,       //!< A (for operator models)
        Unary,               //!< op A
        Nary,                //!< op A B C ...
        Unwrap,
        UnwrapOrElse,
        IsPresent,
        numExpressionKind
    };

    /// Source level operators
    /// If you change this, you have to fix opDescription in Expression.cpp
    enum Op
    {
        INVALID,
        // unary operators
        plusPlus,
        minusMinus,
        bang,
        tilde,
        unaryMinus,
        // simple infix operators
        star,
        slash,
        mod,
        plus,
        minus,
        amp,
        hat,
        bar,
        ampAmp,
        barBar,
        less,
        leq,
        greater,
        geq,
        neq,
        eqEq,
        lShift,
        rShift,
        in,
        // assignment operators
        eq,
        starEq,
        slashEq,
        modEq,
        plusEq,
        minusEq,
        ampEq,
        hatEq,
        barEq,
        lShiftEq,
        rShiftEq,
        // dotted infix operators
        dotStar,
        dotSlash,
        dotMod,
        dotPlus,
        dotMinus,
        dotAmp,
        dotHat,
        dotBar,
        dotLess,
        dotLeq,
        dotGreater,
        dotGeq,
        dotNeq,
        dotEqEq,
        dotLShift,
        dotRShift,
        // internal operators for expressions
        dot,
        subscript,
        call,
        cast,
        conditional,
        numOps
    };

    static const char* opDescription[numOps];

    /// Expression Generation Context
    enum GenerationContext
    {
        AssignmentLHS, //!< A = ...
        General        //!< A
    };

    /// Symbol Type (for SymbolExpression)
    enum SymbolType
    {
        State, //!< state variable (remap to state$id)
        Local, //!< local variable (remap to id$id)
        Attr,  //!< attribute name (leave alone)
        Other  //!< anything else (leave alone)
    };

  protected:
    /// Constructor
    /// @param k Expression Kind
    /// @param type Expression Type
    /// @param location Source Location
    Expression(Kind k, const RootTyp& type, const SourceLocation& location)
      : _kind(k)
      , _dontSimplify(false)
      , _type(type)
      , _location(location)
    {}

  public:
    /// Destructor
    virtual ~Expression();

    /// Clone an expression
    /// @return copy of *this
    virtual Expression& clone() const = 0;

    /// Return the kind of an expression
    /// @returns expression type
    Kind getKind() const { return _kind; }

    /// Return Expression source location
    /// @return the coordinates for the expression
    virtual const SourceLocation& getLocation() const { return _location; }

    /// Return Start location of Expression (i.e startLocation (4+4) is location(4)
    /// @return the coordinates for the start of the expression
    virtual const SourceLocation& getStartLocation() const { return _location; }

    /// Return an Expression as a string
    /// @return       displayable version of expression
    std::string toString() const;

    /// Print an Expression
    /// @param  s     stream to print to
    /// @param  root  top level expression
    /// @return       s
    virtual std::ostream& print(std::ostream& s, const Expression& root) const = 0;

    /// Compare two Expressions for equality (recursively)
    /// param rhs     Expression to compare to
    bool operator==(const Expression& rhs) const;

    /// Compare two Expressions for inequality (recursively)
    /// param rhs     Expression to compare to
    bool operator!=(const Expression& rhs) const { return !this->operator==(rhs); }

    /// Get the type of an expression
    /// @returns expression type
    virtual const RootTyp& getType() const { return _type; }

    /// Return the Literal value of an expression if available
    /// @return a Literal *, or NULL if not a LiteralExpression
    virtual const Literal* getLiteral() const { return NULL; }

    /// Return the Literal value of an expression if available
    /// @return a Literal *, or NULL if not a LiteralExpression
    virtual Literal* getLiteral() { return NULL; }

    /// Return the builtin number of a Symbol expression
    /// @return the number, or -1 if not valid
    virtual const int32_t getBuiltinNum() const { return -1; }

    /// Set an ExpressionValue to the value of a possibly foldable literal
    /// @param result  set to the value of the literal, if any
    /// @return 'true' if the value was set
    virtual bool getExpressionValue(ExpressionValue& result) const { return false; }

    /// Simplify (fold) an expression
    /// @param e      Expression to be simplified, which will be deleted if
    ///               the simplification succeeds
    /// @param eval   Expression Evaluator to be used
    /// @return the folded expression, or e if not foldable
    static Expression* simplify(Expression* e, ExpressionEvaluator& eval);

    /// Replace Literals in an expression
    /// Literals that are simple (currently primitives and list<primitive>) are replaced
    /// by a LiteralSymbol that contains a number and an index and the original literal is
    /// cloned into the LiteralReplacer
    /// @param e      Expression to be rewritten, which will be deleted if
    ///               the replacement succeeds
    /// @param lit    Literal Replacer to be used
    /// @param onlySTP if 'true', only replace getSubmissionTime* calls
    /// @return the folded expression, or e if not rewritable
    static Expression* replaceLits(Expression* e, LiteralReplacer& lit, bool onlySTP);

    /// Generate an Expression
    /// @param  s     stream to generate to
    /// @param  depth statement/expression nesting depth
    /// @param  context Context in which the expression is being generated
    /// @param  genTemplate Generate a template for C++ code for SPL expression tree
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate = false) const = 0;

    /// Collect Type and Function Information
    /// @param types   if non-NULL, named types will be added
    /// @param fcns    if non-NULL, referenced functions will be added
    /// @param inFcns  if true, walk into references functions
    void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                 std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                 bool inFcns) const;

    /// Check if expression is evaluatable (types and functions supported by ExpressionEvaluator)
    /// @return Returns true if the expression is evaluatable
    bool isEvaluatable(void) const;

    /// Check if expression has side effects (assign, post/pre inc, calls sidefx)
    /// @return Returns true if the expression has side effects
    virtual bool hasSideEffects(void) const { return false; }

    /// Check if this expression reads operator state
    /// @return Returns true if the expression reads state
    virtual bool readsState() const { return false; }

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

    /// Can we simplify further?
    bool dontSimplify() const { return _dontSimplify; }

    /// No further simplification of this expression allowed
    void setDontSimplify() { _dontSimplify = true; }

    /// Return a printable representation of the given operator
    /// @param op The operator
    /// @return Returns a string containing a prinable version of the operator.
    static std::string opAsString(Op op) { return opDescription[op]; }

    /// Map identifier
    /// @param type Symbol type
    /// @param id identifier to remap
    /// @return remapped identifier
    static std::string remapId(SymbolType type, const std::string& id);

  protected:
    /// Simplify (fold) an expression
    /// @return a folded Expression, or 'this' if not foldable
    virtual Expression* simplify(ExpressionEvaluator& eval) { return this; }

    /// Replace Literals in an expression
    /// @return a rewritten Expression, or 'this' if not rewritable
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP) { return this; }

  protected:
    /// Compare two Expressions for equality (recursively)
    /// Used to implement ==
    /// param rhs     Expression to compare to
    virtual bool equal(const Expression& rhs) const = 0;

    Kind _kind : 8;
    bool _dontSimplify;
    const RootTyp& _type;
    const SourceLocation& _location;
};

#define EXPR_IS_AS_SPECIALIZATIONS(isEnum, asType)                                                 \
    template<>                                                                                     \
    inline bool Expression::is<asType>() const                                                     \
    {                                                                                              \
        return getKind() == Expression::isEnum;                                                    \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& Expression::as<asType>()                                                        \
    {                                                                                              \
        assert(dynamic_cast<asType*>(this) != 0);                                                  \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline const asType& Expression::as<asType>() const                                            \
    {                                                                                              \
        assert(dynamic_cast<const asType*>(this) != 0);                                            \
        return *static_cast<const asType*>(this);                                                  \
    }

std::ostream& operator<<(std::ostream& ostr, const Expression& v);
std::ostream& operator<<(std::ostream& ostr, Expression::Kind v);

class LiteralExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param lit Literal Value
    LiteralExpression(const RootTyp& type, const SourceLocation& l, Literal& lit);

    /// Destructor
    ~LiteralExpression();

    /// Get the value of a literal
    /// @returns expression value
    const Literal& getValue() const;

    /// Get the value of a literal
    /// @returns expression value
    Literal& getValue();

    /// Get the value of a literal (overrides base class)
    /// @returns expression value
    virtual const Literal* getLiteral() const { return _value; }

    /// Get the value of a literal (overrides base class)
    /// @returns expression value
    virtual Literal* getLiteral() { return _value; }

    /// Set an ExpressionValue to the value of a possibly foldable literal
    /// @param result  set to the value of the literal, if any
    /// @return 'true' if the value was set
    virtual bool getExpressionValue(ExpressionValue& result) const;

    /// Set the replacement function name for a Literal.   Used to replace literals
    /// in functions with calls to functions returning statically computed literals
    /// @pre literal->iscompileTimeEvaluatable() is 'true'
    /// @param fcnName Name to be used to replace the literal value
    void setReplacementFunctionName(const std::string& fcnName) { _replFcnName = fcnName; }

    /// Return the replacement function name for this literal
    /// @return The saved replacement function name (empty if not valid)
    const std::string& getReplacementFunctionName() const { return _replFcnName; };

    virtual bool readsState() const;

  private:
    virtual bool hasSideEffects(void) const;
    virtual LiteralExpression& clone() const;
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;

    Literal* _value;
    std::string _replFcnName;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Lit, LiteralExpression)

class EnumExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param value Enumeration Value (name)
    EnumExpression(const RootTyp& type, const SourceLocation& l, const std::string& value);

    /// Destructor
    ~EnumExpression();

    /// Get the name of the enum
    /// @returns enum name
    const std::string& getId() const { return _id; }

    /// Get the value of a literal
    /// @returns expression value
    virtual const Literal* getLiteral() const { return _value; }

    /// Get the value of a literal (overrides base class)
    /// @returns expression value
    virtual Literal* getLiteral() { return _value; }

    /// Set an ExpressionValue to the value of a possibly foldable literal
    /// @param result  set to th value of the literal, if any
    /// @return 'true' if the value was set
    virtual bool getExpressionValue(ExpressionValue& result) const;

  private:
    virtual EnumExpression& clone() const;
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;

    std::string _id;
    Literal* _value;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Enum, EnumExpression)

class SymbolExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param id symbol name
    /// @param symbolType symbol type
    /// @param builtinNum For predefined procedures, the number of the builtin (for evaluation)
    SymbolExpression(const RootTyp& type,
                     const SourceLocation& l,
                     const std::string& id,
                     SymbolType symbolType,
                     int32_t builtinNum = -1);

    /// Get the symbol name
    /// @return Symbol name
    const std::string& getId() const { return _id; }

    /// Set the symbol name
    /// @param id Symbol name
    void setId(const std::string& id) { _id = id; }

    /// Set the symbol type
    /// @param symbolType Symbol type
    void setSymbolType(SymbolType symbolType) { _symbolType = symbolType; }

    /// Return the builtin number (for evaluation purposes)
    /// @return builtin number
    virtual const int32_t getBuiltinNum() const { return _builtinNum; }

    virtual bool readsState() const { return _symbolType == State; }

  private:
    virtual SymbolExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  protected:
    SymbolExpression(Kind k,
                     const RootTyp& type,
                     const SourceLocation& l,
                     const std::string& id,
                     int32_t builtinNum = -1);
    virtual bool equal(const Expression& rhs) const;
    std::string _id;
    int32_t _builtinNum;
    SymbolType _symbolType;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Symbol, SymbolExpression)

class CustomLiteralExpression : public SymbolExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param id symbol name
    /// @param enumName Name of containing Enum
    CustomLiteralExpression(const RootTyp& type,
                            const SourceLocation& l,
                            const std::string& id,
                            const std::string& enumName);

    /// Get the enum name
    /// @return containing enum name
    const std::string& getEnumName() const { return _enumName; }

  private:
    virtual CustomLiteralExpression& clone() const;
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  protected:
    virtual bool equal(const Expression& rhs) const;
    std::string _enumName;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(CustomLiteral, CustomLiteralExpression)

class StreamSymbolExpression : public SymbolExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param id Stream name
    /// @param port Port number
    StreamSymbolExpression(const RootTyp& type,
                           const SourceLocation& l,
                           const std::string& id,
                           int32_t port);
    /// Access the port for this symbol
    /// @return Returns the port that this symbol represents
    int32_t getPort() const { return _port; }

  private:
    virtual StreamSymbolExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  protected:
    StreamSymbolExpression(Kind k,
                           const RootTyp& type,
                           const SourceLocation& l,
                           const std::string& id,
                           int32_t port);
    virtual bool equal(const Expression& rhs) const;
    int32_t _port;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(StreamSymbol, StreamSymbolExpression)

class StreamHistorySymbolExpression : public StreamSymbolExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param id Stream Name
    /// @param port Port number
    /// @param past_depth How far back are we looking?
    StreamHistorySymbolExpression(const RootTyp& type,
                                  const SourceLocation& l,
                                  const std::string& id,
                                  int32_t port,
                                  int32_t past_depth);

    /// Access the index into the history
    /// @return Returns the index into the history
    int32_t getPastDepth() const { return _past_depth; }

  private:
    virtual StreamHistorySymbolExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

    int32_t _past_depth;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(StreamHistorySymbol, StreamHistorySymbolExpression)

class LiteralSymbolExpression : public SymbolExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param idIndex index into the identifier table
    /// @param litIndex index into the literal table
    LiteralSymbolExpression(const RootTyp& type,
                            const SourceLocation& l,
                            uint32_t idIndex,
                            uint32_t litIndex);

    /// Get the identifier index
    /// @return identifier index
    uint32_t getIndex() const { return _idIndex; }

    /// Get the literal index
    /// @return literal index
    uint32_t getLitIndex() const { return _litIndex; }

  private:
    virtual LiteralSymbolExpression& clone() const;

  private:
    uint32_t _idIndex;
    uint32_t _litIndex;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(LiteralSymbol, LiteralSymbolExpression)

class BinaryExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param op Operator
    /// @param lhs Left hand expression
    /// @param rhs Right hand expression
    BinaryExpression(const RootTyp& type,
                     const SourceLocation& l,
                     Op op,
                     Expression& lhs,
                     Expression& rhs);

    /// Destructor
    ~BinaryExpression();

    /// Get the Operation for the binary op
    /// @returns operation
    const Op getOp() const { return _op; }

    /// Get the LHS expression
    /// @returns lhs
    const Expression& getLhs() const { return *_lhs; }

    Expression& getLhs() { return *_lhs; }

    /// Get the RHS expression
    /// @returns rhs
    const Expression& getRhs() const { return *_rhs; }

    Expression& getRhs() { return *_rhs; }

    /// Set the LHS expression
    /// @param lhs new left hand expression
    void setLhs(Expression& lhs)
    {
        if (_lhs) {
            delete _lhs;
        }
        _lhs = &lhs;
    }

    /// Set the RHS expression
    /// @param rhs new rhs hand expression
    void setRhs(Expression& rhs)
    {
        if (_rhs) {
            delete _rhs;
        }
        _rhs = &rhs;
    }

    virtual bool readsState() const;

  private:
    virtual BinaryExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    static std::string dotOperator(Op op);
    static std::string opEqualOperator(Op op);
    Op mapOperator() const;

  protected:
    BinaryExpression(Kind k,
                     const RootTyp& type,
                     const SourceLocation& l,
                     Op op,
                     Expression& lhs,
                     Expression& rhs);
    virtual bool hasSideEffects(void) const;
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return _lhs->getStartLocation(); }

    Expression *_lhs, *_rhs;
    Op _op;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Binary, BinaryExpression)

class AttributeExpression : public BinaryExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param lhs Left hand expression
    /// @param rhs Symbol Expression for attribute
    AttributeExpression(const RootTyp& type,
                        const SourceLocation& l,
                        Expression& lhs,
                        Expression& rhs)
      : BinaryExpression(Attribute, type, l, dot, lhs, rhs)
    {
        MODEL("AttributeExpression(" << (void*)this << ", " << (void*)&lhs << ", " << (void*)&rhs
                                     << ")\n");
    }

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return _location; }

  private:
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual AttributeExpression& clone() const;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Attribute, AttributeExpression)

class SubscriptExpression : public Expression
{
  public:
    /// Create a Subscript expression
    /// @param type         type of the expression
    /// @param l            location
    /// @param lhs          expression being subscripted
    /// @param isSlice      'true' if this is an array slice
    /// @param lower        lower bound (or only expn if not slice)
    /// @pre isSlice || lower != NULL
    /// @param upper        upper bound of slice (if present)
    SubscriptExpression(const RootTyp& type,
                        const SourceLocation& l,
                        Expression& lhs,
                        bool isSlice,
                        Expression* lower,
                        Expression* upper);

    /// Destructor
    ~SubscriptExpression();

    /// Get the LHS expression
    /// @returns lhs
    const Expression& getLhs() const { return *_lhs; }

    Expression& getLhs() { return *_lhs; }

    /// Set the LHS expression
    /// @param lhs new left hand expression
    void setLhs(Expression& lhs)
    {
        if (_lhs) {
            delete _lhs;
        }
        _lhs = &lhs;
    }

    /// Is this subscript a slice?
    /// @returns 'true' if this is a slice
    const bool getSlice() const { return _isSlice; }

    /// Get the slice lower bound (or only subscript)
    /// @returns slice lower bound expression
    const Expression* lower() const { return _lower; }

    Expression* lower() { return _lower; }

    /// Set the slice lower bound (or only subscript)
    /// @param l new lower bound expression
    void setLower(Expression& l)
    {
        if (_lower) {
            delete _lower;
        }
        _lower = &l;
    }

    /// Get the slice upper bound
    /// @returns slice upper bound expression
    /// @pre isSlice
    const Expression* upper() const { return _upper; }

    Expression* upper() { return _upper; }

    /// Set the slice upper bound
    /// @param u new upper bound expression
    void setUpper(Expression& u)
    {
        if (_upper) {
            delete _upper;
        }
        _upper = &u;
    }

    virtual bool readsState() const;

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return _lhs->getStartLocation(); }

    Expression* _lhs;
    Expression* _lower;
    Expression* _upper;
    bool _isSlice;

  private:
    virtual bool hasSideEffects(void) const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    virtual SubscriptExpression& clone() const;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Subscript, SubscriptExpression)

class UnwrapExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param lhs Expression to be unwrapped, if optional present.  If a non-optional is given,
    /// then the unwrap operation acts as an identity function.
    UnwrapExpression(const RootTyp& type, const SourceLocation& l, Expression& lhs);

    /// Destructor
    ~UnwrapExpression();

    /// Get the LHS expression
    /// @returns lhs
    const Expression& getLHS() const { return *_lhs; }

    Expression& getLHS() { return *_lhs; }

    /// Set the LHS expression
    /// @param lhs new LHS expression
    void setLHS(Expression& lhs)
    {
        if (_lhs) {
            delete _lhs;
        }
        _lhs = &lhs;
    }

    virtual bool readsState() const;

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return _lhs->getStartLocation(); }

  private:
    virtual bool hasSideEffects(void) const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    virtual UnwrapExpression& clone() const;

    Expression* _lhs;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Unwrap, UnwrapExpression)

class UnwrapOrElseExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param condition Condition Expression
    /// @param lhs Expression to be unwrapped, if optional present.  If a non-optional is given,
    /// then the unwrap operation acts as an identity function.
    /// @param rhs Expression to be used if the lhs is optional and is not present
    UnwrapOrElseExpression(const RootTyp& type,
                           const SourceLocation& l,
                           Expression& lhs,
                           Expression& rhs);

    /// Destructor
    ~UnwrapOrElseExpression();

    /// Get the LHS expression
    /// @returns LHS
    const Expression& getLHS() const { return *_lhs; }

    Expression& getLHS() { return *_lhs; }

    /// Set the LHS expression
    /// @param lhs new LHS expression
    void setLHS(Expression& lhs)
    {
        if (_lhs) {
            delete _lhs;
        }
        _lhs = &lhs;
    }

    /// Get the RHS expression
    /// @returns RHS
    const Expression& getRHS() const { return *_rhs; }

    Expression& getRHS() { return *_rhs; }

    /// Set the RHS expression
    /// @param rhs new RHS expression
    void setRHS(Expression& rhs)
    {
        if (_rhs) {
            delete _rhs;
        }
        _rhs = &rhs;
    }

    virtual bool readsState() const;

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return _lhs->getStartLocation(); }

  private:
    virtual bool hasSideEffects(void) const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    virtual UnwrapOrElseExpression& clone() const;

    Expression* _lhs;
    Expression* _rhs;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(UnwrapOrElse, UnwrapOrElseExpression)

class IsPresentExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param lhs Expression to be evaluated
    IsPresentExpression(const RootTyp& type, const SourceLocation& l, Expression& lhs);

    /// Destructor
    ~IsPresentExpression();

    /// Get the LHS expression
    /// @returns LHS
    const Expression& getLHS() const { return *_lhs; }

    Expression& getLHS() { return *_lhs; }

    /// Set the LHS expression
    /// @param lhs new LHS expression
    void setLHS(Expression& lhs)
    {
        if (_lhs) {
            delete _lhs;
        }
        _lhs = &lhs;
    }

    virtual bool readsState() const;

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return _lhs->getStartLocation(); }

  private:
    virtual bool hasSideEffects(void) const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    virtual IsPresentExpression& clone() const;

    Expression* _lhs;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(IsPresent, IsPresentExpression)

class ConditionalExpression : public BinaryExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param condition Condition Expression
    /// @param lhs Expression to be evaluated if cond evaluates to 'true'
    /// @param rhs Expression to be evaluated if cond evaluates to 'false'
    ConditionalExpression(const RootTyp& type,
                          const SourceLocation& l,
                          Expression& condition,
                          Expression& lhs,
                          Expression& rhs);

    /// Destructor
    ~ConditionalExpression();

    /// Get the condition expression
    /// @returns condition
    const Expression& getCondition() const { return *_cond; }

    Expression& getCondition() { return *_cond; }

    /// Set the condition expression
    /// @param cond new condition expression
    void setCondition(Expression& cond)
    {
        if (_cond) {
            delete _cond;
        }
        _cond = &cond;
    }

    virtual bool readsState() const;

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return _cond->getStartLocation(); }

  private:
    virtual bool hasSideEffects(void) const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    virtual ConditionalExpression& clone() const;

    Expression* _cond;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Conditional, ConditionalExpression)

class UnaryExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param op Operation
    /// @param expn Sub-expression
    UnaryExpression(const RootTyp& type, const SourceLocation& l, Op op, Expression& expn);

    /// Destructor
    ~UnaryExpression();

    /// Get the Operation for the unary op
    /// @returns operation
    const Op getOp() const { return _op; }

    /// Get the expression
    /// @returns expn
    const Expression& getExpn() const { return *_expn; }

    Expression& getExpn() { return *_expn; }

    /// Set the expression
    /// @param expn new expression
    void setExpn(Expression& expn)
    {
        if (_expn) {
            delete _expn;
        }
        _expn = &expn;
    }

    virtual bool readsState() const;

  protected:
    UnaryExpression(Kind k, const RootTyp& type, const SourceLocation& l, Op op, Expression& expn);
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  private:
    virtual bool hasSideEffects(void) const;
    virtual UnaryExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;

  protected:
    Expression* _expn;
    Op _op;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Unary, UnaryExpression)

class PrefixExpression : public UnaryExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param op Operation
    /// @param expn Sub-expression
    PrefixExpression(const RootTyp& type, const SourceLocation& l, Op op, Expression& expn);

  protected:
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  private:
    virtual bool hasSideEffects(void) const { return _op != bang; }
    virtual PrefixExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Prefix, PrefixExpression)

class PostfixExpression : public UnaryExpression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param op Operation
    /// @param expn Sub-expression
    PostfixExpression(const RootTyp& type, const SourceLocation& l, Op op, Expression& expn);

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  private:
    virtual bool hasSideEffects(void) const { return true; }
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual PostfixExpression& clone() const;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Postfix, PostfixExpression)

class CastExpression : public UnaryExpression
{
  public:
    /// Constructor
    /// @param type Type expression is casted too
    /// @param l Source Location
    /// @param expn Sub-expression
    CastExpression(const RootTyp& type, const SourceLocation& l, Expression& expn);

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  private:
    virtual CastExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Cast, CastExpression)

class CallExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param fcn Function to be called
    /// @param args Arguments to the function
    /// @param compositeName current composite name (only for getSubmission*Value)
    CallExpression(const RootTyp& type,
                   const SourceLocation& l,
                   const FunctionHeadSym& fcn,
                   const std::vector<Expression*>& args,
                   const std::string& compositeName);

    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param functionName Name of function to be called
    /// @param args Arguments to the function
    /// @param compositeName current composite name (only for getSubmission*Value)
    CallExpression(const RootTyp& type,
                   const SourceLocation& l,
                   const std::string& functionName,
                   const std::vector<Expression*>& args,
                   const std::string& compositeName);

    /// Destructor
    ~CallExpression();

    /// Get the fcn to be called
    /// @return fcn
    const FunctionHeadSym& getFcn() const
    {
        assert(NULL != _fcn);
        return *_fcn;
    }

    /// Access the arguments to the function
    /// @return arguments
    const std::vector<Expression*> getArgs() const { return _args; }
    std::vector<Expression*>& getArgs() { return _args; }

    /// Access the composite name where the submission time value is referenced
    /// @return composite namew
    const std::string& getCompositeName() const { return _compositeName; }

    /// Mark this as a submit that needs a copy
    void setSubmitNeedingCopy() { _submitNeedingCopy = true; }

    virtual bool readsState() const;

  protected:
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;

  private:
    virtual bool hasSideEffects(void) const;
    virtual CallExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;

    const std::string _functionName;
    const FunctionHeadSym* _fcn;
    std::vector<Expression*> _args;
    const std::string _compositeName;
    bool _submitNeedingCopy;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Call, CallExpression)

class NaryExpression : public Expression
{
  public:
    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param op Operator
    /// @param opds operands of the nary operator
    NaryExpression(const RootTyp& type,
                   const SourceLocation& l,
                   Op op,
                   const std::vector<Expression*>& opds);

    /// Constructor
    /// @param type Expression Type
    /// @param l Source Location
    /// @param op Operator
    /// @param lhs Left hand expression
    /// @param rhs Right hand expression
    /// @param res Result expression
    NaryExpression(const RootTyp& type,
                   const SourceLocation& l,
                   Op op,
                   Expression& lhs,
                   Expression& rhs,
                   Expression& res);

    /// Destructor
    ~NaryExpression();

    /// Get the Operation for the binary op
    /// @returns operation
    const Op getOp() const { return _op; }

    /// Get the LHS expression
    /// @returns lhs
    const Expression& getLhs() const { return *_opds[0]; }

    Expression& getLhs() { return *_opds[0]; }

    /// Get the RHS expression
    /// @returns rhs
    const Expression& getRhs() const { return *_opds[1]; }

    Expression& getRhs() { return *_opds[1]; }

    /// Get the RES expression
    /// @returns res
    const Expression& getRes() const { return *_opds[2]; }

    Expression& getRes() { return *_opds[2]; }

    /// Set the LHS expression
    /// @param lhs new left hand expression
    void setLhs(Expression& lhs)
    {
        if (_opds[0]) {
            delete _opds[0];
        }
        _opds[0] = &lhs;
    }

    /// Set the RHS expression
    /// @param rhs new rhs hand expression
    void setRhs(Expression& rhs)
    {
        if (_opds[1]) {
            delete _opds[1];
        }
        _opds[1] = &rhs;
    }

    /// Set the RES expression
    /// @param res new res expression
    void setRes(Expression& res)
    {
        if (_opds[2]) {
            delete _opds[2];
        }
        _opds[2] = &res;
    }

    /// Access the operands of the operator
    /// @return operands
    const std::vector<Expression*> getOpds() const { return _opds; }
    std::vector<Expression*>& getOpds() { return _opds; }

  private:
    virtual NaryExpression& clone() const;
    virtual std::ostream& print(std::ostream& s, const Expression& root) const;
    virtual bool equal(const Expression& rhs) const;
    static std::string dotOperator(Op op);
    Op mapOperator() const;

  protected:
#if 0 // FIX_ME
        NaryExpression(Kind k, const RootTyp& type, const SourceLocation& l,
                       Op op, Expression& lhs, Expression& rhs, Expression& res);
#endif
    virtual bool hasSideEffects(void) const;
    virtual Expression* simplify(ExpressionEvaluator& eval);
    virtual Expression* replaceLits(LiteralReplacer& lit, bool onlySTP);
    virtual void generate(std::ostream& s,
                          uint32_t depth,
                          GenerationContext context,
                          bool genTemplate) const;
    virtual const SourceLocation& getStartLocation() const { return getLhs().getStartLocation(); }

    std::vector<Expression*> _opds;
    Op _op;
};

// is/as specializations
EXPR_IS_AS_SPECIALIZATIONS(Nary, NaryExpression)

std::ostream& operator<<(std::ostream& ostr, Expression::Op op);

};

namespace std {
namespace tr1 {
template<>
struct hash<SPL::LiteralReplacer::ArgInfo>
{
    size_t operator()(const SPL::LiteralReplacer::ArgInfo& a) const
    {
        hash<std::string> myhash;
        const std::string* name = a.getName();
        if (name) {
            return myhash(*name + a.getKind()) + static_cast<size_t>(a.getRequired());
        }
        return myhash(a.getKind()) + static_cast<size_t>(a.getRequired());
    }
};
}
}

#endif /* EXPRESSION_H */
