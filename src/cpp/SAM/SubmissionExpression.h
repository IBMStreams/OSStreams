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

#ifndef SUBMISSION_EXPRESSION_H
#define SUBMISSION_EXPRESSION_H

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/SAMTypes.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <tr1/memory>

SAM_NAMESPACE_BEGIN

namespace LogicalToPhysical {
namespace SubmissionExpressionEval {

enum OrOp
{
    Or
};
enum AndOp
{
    And
};
enum RelOp
{
    LessThan,
    LessEqual,
    GreaterThan,
    GreaterEqual,
    Equal,
    NotEqual,
    In
};
enum AddOp
{
    Add,
    Sub
};
enum MulOp
{
    Mul,
    Div,
    Mod
};
enum Type
{
    Int32,
    Int64,
    Float64,
    RString
};

std::ostream& operator<<(std::ostream& ostr, OrOp v);
std::ostream& operator<<(std::ostream& ostr, AndOp v);
std::ostream& operator<<(std::ostream& ostr, RelOp v);
std::ostream& operator<<(std::ostream& ostr, AddOp v);
std::ostream& operator<<(std::ostream& ostr, MulOp v);
std::ostream& operator<<(std::ostream& ostr, Type v);

class Printable
{
  public:
    virtual ~Printable() {}
    virtual std::ostream& print(std::ostream& s) const = 0;
    virtual std::ostream& printTree(std::ostream& s, int level) const = 0;
};
std::ostream& operator<<(std::ostream& ostr, const Printable& v);

class Reducible
{
  public:
    virtual ~Reducible() {}
    virtual Type type() const = 0;
    virtual SPL::int32 int32Value() const = 0;
    virtual SPL::int64 int64Value() const = 0;
    virtual SPL::float64 float64Value() const = 0;
    virtual const SPL::rstring& rstringValue() const = 0;

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val) = 0;
};

class Literal : public Printable
{
  public:
    Literal(SPL::int64 value)
      : _type(Int64)
      , _int64(value)
    {}
    Literal(const SPL::rstring& value);
    Literal(SPL::float64 value)
      : _type(Float64)
      , _float64(value)
    {}
    virtual ~Literal();

    virtual std::ostream& print(std::ostream& s) const;
    virtual std::ostream& printTree(std::ostream& s, int level) const;

    Type type() const { return _type; }

    SPL::int32 int32Value() const;
    SPL::int64 int64Value() const;
    SPL::float64 float64Value() const;
    const SPL::rstring& rstringValue() const;

  private:
    inline void typeCheck(Type t, const char* msg) const;

    const Type _type;
    const union
    {
        SPL::int32 _int32;
        SPL::int64 _int64;
        SPL::float64 _float64;
        SPL::rstring* _rstring;
    };
};

class Intrinsic : public Printable
{
  public:
    Intrinsic(std::string f);
    virtual ~Intrinsic() {}

    virtual std::ostream& print(std::ostream& s) const;
    virtual std::ostream& printTree(std::ostream& s, int level) const;

    const std::string& function() const { return _function; }
    Type type() const { return _type; }

  private:
    Type _type;
    const std::string _function;
};

class Value
  : public Reducible
  , public Printable
{
  public:
    Value(Literal* l)
      : _literal(l)
    {}
    Value(Intrinsic* i)
      : _intrinsic(i)
    {}
    virtual ~Value() {}

    virtual Type type() const;
    virtual SPL::int32 int32Value() const;
    virtual SPL::int64 int64Value() const;
    virtual SPL::float64 float64Value() const;
    virtual const SPL::rstring& rstringValue() const;

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);

    virtual std::ostream& print(std::ostream& s) const;
    virtual std::ostream& printTree(std::ostream& s, int level) const;

  private:
    inline void literalCheck(const char* msg) const;

    std::auto_ptr<Literal> _literal;
    std::auto_ptr<Intrinsic> _intrinsic;
};

class Symbol : public Printable
{
  public:
    Symbol(const std::string& sym)
      : _symbol(sym)
      , _hasSubscript(false)
      , _subscript(0)
    {}
    Symbol(const std::string& sym, SPL::int64 sub)
      : _symbol(sym)
      , _hasSubscript(true)
      , _subscript(sub)
    {}
    virtual ~Symbol() {}

    virtual std::ostream& print(std::ostream& s) const;
    virtual std::ostream& printTree(std::ostream& s, int level) const;

  private:
    const std::string _symbol;
    const bool _hasSubscript;
    const SPL::int64 _subscript;
};

class OrExpression;

class PrimaryExpression
  : public Reducible
  , public Printable
{
  public:
    PrimaryExpression(Symbol* sym)
      : _symbol(sym)
    {}
    PrimaryExpression(Value* value)
      : _value(value)
    {}
    PrimaryExpression(OrExpression* orExp)
      : _or(orExp)
    {}
    virtual ~PrimaryExpression() {}

    virtual Type type() const;
    virtual SPL::int32 int32Value() const;
    virtual SPL::int64 int64Value() const;
    virtual SPL::float64 float64Value() const;
    virtual const SPL::rstring& rstringValue() const;

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);

    virtual std::ostream& print(std::ostream& s) const;
    virtual std::ostream& printTree(std::ostream& s, int level) const;

  private:
    inline void emptyCheck(const char* kind) const;
    std::auto_ptr<Symbol> _symbol;
    std::auto_ptr<Value> _value;
    std::auto_ptr<OrExpression> _or;
};

class CastExpression
  : public Reducible
  , public Printable
{
  public:
    CastExpression(PrimaryExpression* primary)
      : _primary(primary)
    {}
    CastExpression(Type t, CastExpression* cast)
      : _type(t)
      , _cast(cast)
    {}
    virtual ~CastExpression() {}

    virtual Type type() const;
    virtual SPL::int32 int32Value() const;
    virtual SPL::int64 int64Value() const;
    virtual SPL::float64 float64Value() const;
    virtual const SPL::rstring& rstringValue() const;

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);

    virtual std::ostream& print(std::ostream& s) const;
    virtual std::ostream& printTree(std::ostream& s, int level) const;

  private:
    inline void evaluatedCheck(const char* kind) const;

    std::auto_ptr<PrimaryExpression> _primary;
    Type _type;
    std::auto_ptr<CastExpression> _cast;
};

struct MakeIndent
{
    int level;
    MakeIndent(int l)
      : level(l)
    {}
};
std::ostream& operator<<(std::ostream& ostr, MakeIndent m);

template<class Exp, class Op>
class BinaryExpression
  : public Reducible
  , public Printable
{
  public:
    BinaryExpression(Exp* left)
      : _left(left)
    {}
    BinaryExpression(Exp* left, Op op, Exp* right)
      : _left(left)
      , _op(op)
      , _right(right)
    {}
    virtual ~BinaryExpression() {}

    virtual const char* name() const = 0;

    virtual Type type() const
    {
        if (_left.get() == NULL) {
            THROW(LogicalToPhysical, "Internal error: type() called on empty BinaryExpression");
        }
        return _left->type();
    }

    virtual SPL::int32 int32Value() const
    {
        evaluatedCheck("int32");
        return _left->int32Value();
    }

    virtual SPL::int64 int64Value() const
    {
        evaluatedCheck("int64");
        return _left->int64Value();
    }

    virtual SPL::float64 float64Value() const
    {
        evaluatedCheck("float64");
        return _left->float64Value();
    }

    virtual const SPL::rstring& rstringValue() const
    {
        evaluatedCheck("rstring");
        return _left->rstringValue();
    }

    virtual std::ostream& print(std::ostream& s) const
    {
        if (_right.get() && parenthesize()) {
            s << "(";
        }
        if (_left.get()) {
            s << *_left;
        }
        if (_right.get()) {
            if (parenthesize()) {
                s << ")";
            }
            s << " " << _op << " ";
            if (parenthesize()) {
                s << "(";
            }
            s << *_right;
            if (parenthesize()) {
                s << ")";
            }
        }
        return s;
    }

    virtual std::ostream& printTree(std::ostream& s, int level) const
    {
        s << MakeIndent(level) << name() << ": " << *this << std::endl;
        if (_left.get()) {
            _left->printTree(s, level + 1);
        }
        if (_right.get()) {
            _right->printTree(s, level + 1);
        }
        return s;
    }

  protected:
    virtual bool parenthesize() const = 0;

    void evaluatedCheck(const char* t) const
    {
        if (_left.get() == NULL) {
            THROW(LogicalToPhysical,
                  "Internal error: " << t << "Value() called on empty BinaryExpression");
        } else if (_right.get()) {
            THROW(LogicalToPhysical,
                  "Internal error: " << t << "Value() called on unevaluated BinaryExpression");
        }
    }

    std::auto_ptr<Exp> _left;
    Op _op;
    std::auto_ptr<Exp> _right;
};

class MulExpression : public BinaryExpression<CastExpression, MulOp>
{
  public:
    typedef BinaryExpression<CastExpression, MulOp> Base;

    MulExpression(CastExpression* left)
      : Base(left)
    {}
    MulExpression(CastExpression* left, MulOp op, CastExpression* right)
      : Base(left, op, right)
    {}
    virtual ~MulExpression() {}

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);
    virtual bool parenthesize() const { return false; }
    virtual const char* name() const { return "MulExpression"; }
};

class AddExpression : public BinaryExpression<MulExpression, AddOp>
{
  public:
    typedef BinaryExpression<MulExpression, AddOp> Base;

    AddExpression(MulExpression* left)
      : Base(left)
    {}
    AddExpression(MulExpression* left, AddOp op, MulExpression* right)
      : Base(left, op, right)
    {}
    virtual ~AddExpression() {}

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);
    virtual bool parenthesize() const { return false; }
    virtual const char* name() const { return "AddExpression"; }
};

class Predicate : public BinaryExpression<AddExpression, RelOp>
{
  public:
    typedef BinaryExpression<AddExpression, RelOp> Base;

    Predicate(AddExpression* left)
      : Base(left)
    {}
    Predicate(AddExpression* left, RelOp op, AddExpression* right)
      : Base(left, op, right)
    {}
    virtual ~Predicate() {}

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);
    virtual bool parenthesize() const { return false; }
    virtual const char* name() const { return "Predicate"; }
};

class AndExpression : public BinaryExpression<Predicate, AndOp>
{
  public:
    typedef BinaryExpression<Predicate, AndOp> Base;

    AndExpression(Predicate* left)
      : Base(left)
    {}
    AndExpression(Predicate* left, Predicate* right)
      : Base(left, And, right)
    {}
    virtual ~AndExpression() {}

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);
    virtual bool parenthesize() const { return true; }
    virtual const char* name() const { return "AndExpression"; }
};

class OrExpression : public BinaryExpression<AndExpression, OrOp>
{
  public:
    typedef BinaryExpression<AndExpression, OrOp> Base;

    OrExpression()
      : Base(NULL)
    {}
    OrExpression(AndExpression* left)
      : Base(left)
    {}
    OrExpression(AndExpression* left, AndExpression* right)
      : Base(left, Or, right)
    {}
    virtual ~OrExpression() {}

    virtual bool evaluate(const std::string& sym, const SPL::ConstValueHandle& val);
    virtual bool parenthesize() const { return true; }
    virtual const char* name() const { return "OrExpression"; }
    std::ostream& tree(std::ostream& s) const { return Base::printTree(s, 0); }

    std::string toString() const;
    static std::auto_ptr<OrExpression> fromString(const std::string& str);
};
} // namespace SubmissionExpressionEval
} // namespace LogicalToPhysical

SAM_NAMESPACE_END

#endif /* SUBMISSION_EXPRESSION_H */
