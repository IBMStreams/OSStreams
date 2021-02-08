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

#include <SPL/Runtime/Operator/Port/FilterExpression.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#define BOOST_SPIRIT_THREADSAFE
#ifndef NDEBUG
// Define BOOST_SPRIT_DEBUG to get extra diagnostics from the parser.
#define BOOST_SPIRIT_DEBUG_OUT std::cerr
#endif
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/static_visitor.hpp>

#include <limits>

using namespace std;
using namespace SPL;

#ifndef APPTRCMSGANDTHROW
#define APPTRCMSGANDTHROW(type, lvl, msg, aspect)                                                  \
    do {                                                                                           \
        APPTRC(lvl, msg, aspect);                                                                  \
        THROW_STRING(type, msg);                                                                   \
    } while (false)
#else
#error Duplicate definition for APPTRCMSGANDTHROW
#endif

namespace {

boost::spirit::int_parser<int64_t> const int64_p = boost::spirit::int_parser<int64_t>();
boost::spirit::int_parser<uint64_t> const uint64_p = boost::spirit::int_parser<uint64_t>();

bool isNumeric(const Meta::Type type)
{
    switch (type) {
        case Meta::Type::INT8:
        case Meta::Type::INT16:
        case Meta::Type::INT32:
        case Meta::Type::INT64:
        case Meta::Type::UINT8:
        case Meta::Type::UINT16:
        case Meta::Type::UINT32:
        case Meta::Type::UINT64:
        case Meta::Type::FLOAT32:
        case Meta::Type::FLOAT64:
            return true;
        default:
            return false;
    }
}

template<typename T>
std::string typeName();
#ifdef DEFINE_TYPENAME
#error Duplicate definition for DEFINE_TYPENAME
#endif
#define DEFINE_TYPENAME(x)                                                                         \
    template<>                                                                                     \
    std::string typeName<x>()                                                                      \
    {                                                                                              \
        return #x;                                                                                 \
    }
DEFINE_TYPENAME(float64);
DEFINE_TYPENAME(float32);
DEFINE_TYPENAME(int64);
DEFINE_TYPENAME(int32);
DEFINE_TYPENAME(int16);
DEFINE_TYPENAME(int8);
DEFINE_TYPENAME(uint64);
DEFINE_TYPENAME(uint32);
DEFINE_TYPENAME(uint16);
DEFINE_TYPENAME(uint8);
DEFINE_TYPENAME(boolean);
DEFINE_TYPENAME(rstring);
#undef DEFINE_TYPENAME

template<typename T>
FilterExpression::Literal::Kind getLiteralKind();
template<>
FilterExpression::Literal::Kind getLiteralKind<rstring>()
{
    return FilterExpression::Literal::RString;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<int64>()
{
    return FilterExpression::Literal::Int64;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<int32>()
{
    return FilterExpression::Literal::Int32;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<int16>()
{
    return FilterExpression::Literal::Int16;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<int8>()
{
    return FilterExpression::Literal::Int8;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<uint64>()
{
    return FilterExpression::Literal::UInt64;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<uint32>()
{
    return FilterExpression::Literal::UInt32;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<uint16>()
{
    return FilterExpression::Literal::UInt16;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<uint8>()
{
    return FilterExpression::Literal::UInt8;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<float64>()
{
    return FilterExpression::Literal::Float64;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<float32>()
{
    return FilterExpression::Literal::Float32;
}
template<>
FilterExpression::Literal::Kind getLiteralKind<boolean>()
{
    return FilterExpression::Literal::Boolean;
}

// Cast a literal value from one type to another, checking to ensure the
// value can be represented in the target type without loss.
class LiteralCast : public boost::static_visitor<FilterExpression::Literal>
{
  public:
    // Determine whether a uint64 value can be cast to another type.
    template<typename TargetType>
    void checkRangeForCast(uint64 const& currentValue, TargetType const&) const
    {
        // The cast can be done unless the unsigned value is greater than
        // the maximum value of the target type.
        uint64 maxValue = static_cast<uint64>(std::numeric_limits<TargetType>::max());
        if (currentValue > maxValue) {
            APPTRCMSGANDTHROW(
              SPLRuntimeFilterValidationFailed, L_ERROR,
              SPL_RUNTIME_FILTER_VALUE_OUT_OF_RANGE(
                boost::lexical_cast<std::string>(currentValue),
                boost::lexical_cast<std::string>(std::numeric_limits<TargetType>::min()),
                boost::lexical_cast<std::string>(maxValue)),
              SPL_FUNC_DBG);
        }
    }

    // Determine whether a value can be cast to uint64.
    template<typename SourceType>
    void checkRangeForCast(SourceType const& currentValue, uint64 const&) const
    {
        // The cast can be done unless the value is negative
        if (currentValue < 0) {
            APPTRCMSGANDTHROW(
              SPLRuntimeFilterValidationFailed, L_ERROR,
              SPL_RUNTIME_FILTER_VALUE_OUT_OF_RANGE(
                boost::lexical_cast<std::string>(currentValue),
                boost::lexical_cast<std::string>(std::numeric_limits<uint64>::min()),
                boost::lexical_cast<std::string>(std::numeric_limits<uint64>::max())),
              SPL_FUNC_DBG);
        }
    }

    // trivial case for casting uint64 to uint64.
    void checkRangeForCast(uint64 const&, uint64 const&) {}

    // General range checking for casting numeric types, for cases in which
    // neither type is uint64.
    template<typename SourceType, typename TargetType>
    void checkRangeForCast(SourceType const& currentValue, TargetType const&) const
    {
        // Any non-negative value of any supported type can safely be cast to
        // int64, so we cast to int64 to do the comparison.

        int64 maxValue = static_cast<int64>(std::numeric_limits<TargetType>::max());
        int64 minValue = static_cast<int64>(std::numeric_limits<TargetType>::min());

        // special case for casting negative signed to unsigned values
        if (currentValue < 0 && minValue == 0) {
            APPTRCMSGANDTHROW(
              SPLRuntimeFilterValidationFailed, L_ERROR,
              SPL_RUNTIME_FILTER_VALUE_OUT_OF_RANGE(boost::lexical_cast<std::string>(currentValue),
                                                    boost::lexical_cast<std::string>(minValue),
                                                    boost::lexical_cast<std::string>(maxValue)),
              SPL_FUNC_DBG);
        }
        if (maxValue < static_cast<int64>(currentValue) ||
            minValue > static_cast<int64>(currentValue)) {
            APPTRCMSGANDTHROW(
              SPLRuntimeFilterValidationFailed, L_ERROR,
              SPL_RUNTIME_FILTER_VALUE_OUT_OF_RANGE(boost::lexical_cast<std::string>(currentValue),
                                                    boost::lexical_cast<std::string>(minValue),
                                                    boost::lexical_cast<std::string>(maxValue)),
              SPL_FUNC_DBG);
        }
    }

    // Cast any float type to any int type
    template<typename FloatType, typename TargetType>
    const FilterExpression::Literal castFloatToInt(FloatType const& value,
                                                   TargetType const& matchValue) const
    {
        // only allowed if value is an integer
        if (ceilf(value) == value) {

            // limits checking.
            TargetType maxValue = std::numeric_limits<TargetType>::max();
            TargetType minValue = std::numeric_limits<TargetType>::min();
            bool targetIsFloat = std::numeric_limits<TargetType>::is_iec559;
            if (targetIsFloat) {
                if (value != 0 && minValue > abs(value)) {
                    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                                      SPL_RUNTIME_INVALID_FLOAT_CAST_TO_FLOAT(
                                        value, ::typeName<TargetType>(), minValue),
                                      SPL_FUNC_DBG);
                }

                minValue = -maxValue;
            }
            if (maxValue < value || minValue > value) {
                APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                                  SPL_RUNTIME_FILTER_VALUE_OUT_OF_RANGE(value, minValue, maxValue),
                                  SPL_FUNC_DBG);
            }

            return FilterExpression::Literal(static_cast<TargetType>(value));
        }

        APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                          SPL_RUNTIME_INVALID_FLOAT_CAST_TO_INTEGER(value), SPL_FUNC_DBG);
    }

    // Source and target types are the same, so no cast needed.
    template<typename TargetType>
    const FilterExpression::Literal operator()(TargetType const& value,
                                               TargetType const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }

    // Cast from source type to target type.  This will be invoked for
    // all cases in which two different integral types are involved, since
    // there are more specific operators for all combinations not involving
    // to integers.
    template<typename TargetType, typename SourceType>
    const FilterExpression::Literal operator()(SourceType const& value,
                                               TargetType const& matchValue) const
    {

        // limits checking.
        checkRangeForCast(value, matchValue);

        return FilterExpression::Literal(static_cast<TargetType>(value));
    }

    // Casting any type to rstring is not allowed.
    template<typename SourceType>
    const FilterExpression::Literal operator()(SourceType const& value,
                                               rstring const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }

    // Casting rstring to any other type is not allowed.
    template<typename TargetType>
    const FilterExpression::Literal operator()(rstring const& value,
                                               TargetType const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }

    // Casting rstring to rstring succeeds.
    const FilterExpression::Literal operator()(rstring const& value,
                                               rstring const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }

    // Target type is float64.  Any float or integral type may be cast,
    // and no range check is needed.
    template<typename SourceType>
    const FilterExpression::Literal operator()(SourceType const& value,
                                               float64 const& matchValue) const
    {
        return FilterExpression::Literal(static_cast<float64>(value));
    }

    // Casting a float64 to an int.  This is permitted only if the float is
    // exactly integral, and if the value is in the range available to the
    // integral type.
    template<typename TargetType>
    const FilterExpression::Literal operator()(float64 const& value,
                                               TargetType const& matchValue) const
    {
        return castFloatToInt(value, matchValue);
    }

    // float64 to float64
    const FilterExpression::Literal operator()(float64 const& value,
                                               float64 const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }

    // string to float and float to string do not do any actual type conversions
    const FilterExpression::Literal operator()(rstring const& value,
                                               float64 const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(float64 const& value,
                                               rstring const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }

    // Target type is float32.  Any integral type may be cast.
    // Range check is required for float64
    template<typename SourceType>
    const FilterExpression::Literal operator()(SourceType const& value,
                                               float32 const& matchValue) const
    {
        return FilterExpression::Literal(static_cast<float32>(value));
    }

    const FilterExpression::Literal operator()(float64 const& value,
                                               float32 const& matchValue) const
    {
        return FilterExpression::Literal(static_cast<float32>(value));
    }

    const FilterExpression::Literal operator()(float32 const& value,
                                               float64 const& matchValue) const
    {
        return FilterExpression::Literal(static_cast<float64>(value));
    }

    // Casting a float32 to an int.  This is permitted only if the float is
    // exactly integral, and if the value is in the range available to the
    // integral type.
    template<typename TargetType>
    const FilterExpression::Literal operator()(float32 const& value,
                                               TargetType const& matchValue) const
    {
        return castFloatToInt(value, matchValue);
    }

    // float32 to float32
    const FilterExpression::Literal operator()(float32 const& value,
                                               float32 const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }

    // string to float and float to string do not do any actual type conversions
    const FilterExpression::Literal operator()(rstring const& value,
                                               float32 const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(float32 const& value,
                                               rstring const& matchValue) const
    {
        return FilterExpression::Literal(value);
    }
    // prevent boolean-to-int and int-to-boolean casts
    template<typename TargetType>
    const FilterExpression::Literal operator()(boolean value, TargetType const& matchType) const
    {
        return FilterExpression::Literal(value);
    }
    template<typename SourceType>
    const FilterExpression::Literal operator()(SourceType const& value, boolean matchType) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(boolean value, boolean type) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(boolean value, float64 const& type) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(float64 const& value, boolean type) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(boolean value, float32 const& type) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(float32 const& value, boolean type) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(boolean value, rstring const& type) const
    {
        return FilterExpression::Literal(value);
    }
    const FilterExpression::Literal operator()(rstring const& value, boolean type) const
    {
        return FilterExpression::Literal(value);
    }
};

// Test for equality of two variant values. Values are equal only if they
// have the same type and value.
class VariantEquals : public boost::static_visitor<bool>
{
  public:
    // handle comparing two values of the same type.
    template<typename T>
    bool operator()(T const& lhs, T const& rhs) const
    {
        return lhs == rhs;
    }

    // Other combinations return false
    template<typename T, typename U>
    bool operator()(T const& lhs, U const& rhs) const
    {
        return false;
    }
};

// Test whether a variant value is zero.
// If it is numeric, return is true if the value is zero.
// For other types, always false.
class VariantEqualsZero : public boost::static_visitor<bool>
{
  public:
    bool operator()(rstring const& v) const { return false; }

    bool operator()(boolean v) const { return false; }

    template<typename T>
    bool operator()(T const& v) const
    {
        return v == 0;
    }
};

class PrintVariantValue : public boost::static_visitor<>
{
  public:
    PrintVariantValue(ostream& s)
      : _s(s)
    {}

    // Special handling for float64
    void operator()(float64 const& v) const { SPL::serializeWithPrecision(_s, v); }

    // Special handling for float32
    void operator()(float32 const& v) const { SPL::serializeWithPrecision(_s, v); }

    // Default for other types
    template<class T>
    void operator()(T& v) const
    {
        _s << v;
    }

  private:
    ostream& _s;
};

} // end nameless namespace

FilterExpression::Op FilterExpression::opFromString(string const& opName)
{
    if (opName == "<") {
        return FilterExpression::LessThan;
    }
    if (opName == "<=") {
        return FilterExpression::LessEqual;
    }
    if (opName == ">") {
        return FilterExpression::GreaterThan;
    }
    if (opName == ">=") {
        return FilterExpression::GreaterEqual;
    }
    if (opName == "==") {
        return FilterExpression::Equal;
    }
    if (opName == "!=") {
        return FilterExpression::NotEqual;
    }
    if (opName == "In") {
        return FilterExpression::In;
    }

    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                      SPL_RUNTIME_IMPORT_FILTER_UNKNOWN_OPERATION(opName), SPL_FUNC_DBG);
}

FilterExpression::ArithmeticOp FilterExpression::arithmeticOpFromString(string const& opName)
{
    if (opName == "%") {
        return FilterExpression::Mod;
    }
    if (opName == "|") {
        return FilterExpression::BitOr;
    }
    if (opName == "&") {
        return FilterExpression::BitAnd;
    }
    if (opName == "^") {
        return FilterExpression::BitXor;
    }
    if (opName == "<<") {
        return FilterExpression::LeftShift;
    }
    if (opName == ">>") {
        return FilterExpression::RightShift;
    }
    if (opName == "+") {
        return FilterExpression::Add;
    }
    if (opName == "-") {
        return FilterExpression::Sub;
    }
    if (opName == "*") {
        return FilterExpression::Mul;
    }
    if (opName == "/") {
        return FilterExpression::Div;
    }
    if (opName == "~") {
        return FilterExpression::BNot;
    }

    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                      SPL_RUNTIME_IMPORT_FILTER_UNKNOWN_OPERATION(opName), SPL_FUNC_DBG);
}

string FilterExpression::opToString(Op op)
{
    switch (op) {
        case FilterExpression::LessThan:
            return "<";
        case FilterExpression::LessEqual:
            return "<=";
        case FilterExpression::GreaterThan:
            return ">";
        case FilterExpression::GreaterEqual:
            return ">=";
        case FilterExpression::Equal:
            return "==";
        case FilterExpression::NotEqual:
            return "!=";
        case FilterExpression::In:
            return "in";
        case FilterExpression::None:
            return "";
    }
    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                      SPL_RUNTIME_IMPORT_FILTER_UNKNOWN_OPERATION(boost::lexical_cast<string>(op)),
                      SPL_FUNC_DBG);
}

string FilterExpression::arithmeticOpToString(ArithmeticOp op)
{
    switch (op) {
        case FilterExpression::Mod:
            return "%";
        case FilterExpression::BitOr:
            return "|";
        case FilterExpression::BitAnd:
            return "&";
        case FilterExpression::BitXor:
            return "^";
        case FilterExpression::LeftShift:
            return "<<";
        case FilterExpression::RightShift:
            return ">>";
        case FilterExpression::NoArithmeticOp:
            return "";

        case FilterExpression::Add:
            return "+";
        case FilterExpression::Sub:
            return "-";
        case FilterExpression::Mul:
            return "*";
        case FilterExpression::Div:
            return "/";
        case FilterExpression::BNot:
            return "~";
    }
    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                      SPL_RUNTIME_IMPORT_FILTER_UNKNOWN_OPERATION(boost::lexical_cast<string>(op)),
                      SPL_FUNC_DBG);
}

bool FilterExpression::Literal::operator==(const Literal& rhs) const
{
    return boost::apply_visitor(VariantEquals(), _value, rhs._value);
}

FilterExpression::Literal& FilterExpression::Literal::operator=(const Literal& rhs)
{
    if (this != &rhs) {
        _value = rhs._value;
    }
    return *this;
}

bool FilterExpression::Literal::equalsZero() const
{
    return boost::apply_visitor(VariantEqualsZero(), _value);
}

ostream& FilterExpression::Literal::print(ostream& s) const
{
    boost::apply_visitor(PrintVariantValue(s), _value);
    return s;
}

template<typename T>
inline T const& FilterExpression::Literal::getValue() const
{
    FilterExpression::Literal::Kind k = ::getLiteralKind<T>();
    if (getKind() != k) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidLiteral, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_KIND_INVALID(::typeName<T>()), SPL_FUNC_DBG);
    }
    return boost::get<T>(_value);
}

int64 FilterExpression::Literal::getInt64Value() const
{
    return getValue<SPL::int64>();
}

int32 FilterExpression::Literal::getInt32Value() const
{
    return getValue<SPL::int32>();
}

int16 FilterExpression::Literal::getInt16Value() const
{
    return getValue<SPL::int16>();
}

int8 FilterExpression::Literal::getInt8Value() const
{
    return getValue<SPL::int8>();
}

uint64 FilterExpression::Literal::getUInt64Value() const
{
    return getValue<SPL::uint64>();
}

uint32 FilterExpression::Literal::getUInt32Value() const
{
    return getValue<SPL::uint32>();
}

uint16 FilterExpression::Literal::getUInt16Value() const
{
    return getValue<SPL::uint16>();
}

uint8 FilterExpression::Literal::getUInt8Value() const
{
    return getValue<SPL::uint8>();
}

float64 FilterExpression::Literal::getFloat64Value() const
{
    return getValue<SPL::float64>();
}

float32 FilterExpression::Literal::getFloat32Value() const
{
    return getValue<SPL::float32>();
}

const rstring& FilterExpression::Literal::getRStringValue() const
{
    return getValue<SPL::rstring>();
}

boolean FilterExpression::Literal::getBooleanValue() const
{
    return getValue<SPL::boolean>();
}

const FilterExpression::Literal FilterExpression::Literal::castToMatchingType(
  const FilterExpression::Literal& value,
  const FilterExpression::Literal& matchKind)
{
    FilterExpression::Literal::Kind valueKind = value.getKind();
    FilterExpression::Literal::Kind targetKind = matchKind.getKind();

    if (valueKind == targetKind) {
        return value;
    }
    switch (targetKind) {
        case FilterExpression::Literal::Int64:
        case FilterExpression::Literal::Int32:
        case FilterExpression::Literal::Int16:
        case FilterExpression::Literal::Int8:
        case FilterExpression::Literal::UInt64:
        case FilterExpression::Literal::UInt32:
        case FilterExpression::Literal::UInt16:
        case FilterExpression::Literal::UInt8:
        case FilterExpression::Literal::Float64:
        case FilterExpression::Literal::Float32: {
            FilterExpression::Literal result =
              boost::apply_visitor(LiteralCast(), value._value, matchKind._value);
            return result;
        }
        default:
            break;
    }

    // no cast supported
    return value;
}

FilterExpression::FilterExpression(Kind k)
  : _kind(k)
{
    if (k == Predicate) {
        _item = new ItemInfo;
    } else {
        _subClauses = new vector<FilterExpressionPtr>;
    }
}

FilterExpression::~FilterExpression()
{
    if (_kind == Predicate) {
        delete _item;
    } else {
        delete _subClauses;
    }
}

bool FilterExpression::isEmpty() const
{
    // We are true if we are an 'and' or 'or' with no clauses
    if (_kind == Predicate) {
        return false;
    }

    return _subClauses->size() == 0;
}

FilterExpressionPtr FilterExpression::createAndClause(FilterExpressionPtr& lhs,
                                                      FilterExpressionPtr& rhs)
{
    FilterExpressionPtr p(new FilterExpression(AndClause));
    p->_subClauses->push_back(lhs);
    p->_subClauses->push_back(rhs);
    return p;
}

FilterExpressionPtr FilterExpression::createAndClause(FilterExpressionPtr& subClause)
{
    FilterExpressionPtr p(new FilterExpression(AndClause));
    p->_subClauses->push_back(subClause);
    return p;
}

FilterExpressionPtr FilterExpression::createOrClause(FilterExpressionPtr& subClause)
{
    FilterExpressionPtr p(new FilterExpression(OrClause));
    p->_subClauses->push_back(subClause);
    return p;
}

FilterExpressionPtr FilterExpression::createOrClause(FilterExpressionPtr& lhs,
                                                     FilterExpressionPtr& rhs)
{
    FilterExpressionPtr p(new FilterExpression(OrClause));
    p->_subClauses->push_back(lhs);
    p->_subClauses->push_back(rhs);
    return p;
}

FilterExpressionPtr FilterExpression::createNotClause(FilterExpressionPtr& rhs)
{
    FilterExpressionPtr p(new FilterExpression(NotClause));
    p->_subClauses->push_back(rhs);
    return p;
}

FilterExpressionPtr FilterExpression::createPredicate(const string& symbol,
                                                      Op op,
                                                      const Literal& lit,
                                                      uint32_t index)
{
    FilterExpressionPtr p(new FilterExpression(Predicate));
    bool hasSubscript = false;
    int64_t dummySubscript = 0;
    ArithmeticOperationsVector noOperations;
    return createPredicate(symbol, hasSubscript, dummySubscript, op, lit, noOperations, index);
    return p;
}

namespace {
// Verify that the literal has the specified type.  This will either return,
// indicating success, or throw an exception.
void validate_lit(const Meta::Type& mt, const FilterExpression::Literal& lit)
{
    bool valid = false;
    if (mt == Meta::Type::INT64) {
        valid = lit.getKind() == FilterExpression::Literal::Int64;
    } else if (mt == Meta::Type::INT32) {
        valid = lit.getKind() == FilterExpression::Literal::Int32;
    } else if (mt == Meta::Type::INT16) {
        valid = lit.getKind() == FilterExpression::Literal::Int16;
    } else if (mt == Meta::Type::INT8) {
        valid = lit.getKind() == FilterExpression::Literal::Int8;
    } else if (mt == Meta::Type::UINT64) {
        valid = lit.getKind() == FilterExpression::Literal::UInt64;
    } else if (mt == Meta::Type::UINT32) {
        valid = lit.getKind() == FilterExpression::Literal::UInt32;
    } else if (mt == Meta::Type::UINT16) {
        valid = lit.getKind() == FilterExpression::Literal::UInt16;
    } else if (mt == Meta::Type::UINT8) {
        valid = lit.getKind() == FilterExpression::Literal::UInt8;
    } else if (mt == Meta::Type::RSTRING) {
        valid = lit.getKind() == FilterExpression::Literal::RString;
    } else if (mt == Meta::Type::FLOAT64) {
        valid = lit.getKind() == FilterExpression::Literal::Float64;
    } else if (mt == Meta::Type::FLOAT32) {
        valid = lit.getKind() == FilterExpression::Literal::Float32;
    } else if (mt == Meta::Type::BOOLEAN) {
        valid = lit.getKind() == FilterExpression::Literal::Boolean;
    }

    if (!valid) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterTypeMismatch, L_ERROR, SPL_RUNTIME_FILTER_TYPE_MISMATCH,
                          SPL_FUNC_DBG);
    }
}

Meta::Type validate_predicate(const Tuple& oTuple,
                              const string& symbol,
                              bool isList,
                              uint32_t& index)
{
    ConstTupleIterator it = oTuple.findAttribute(symbol);
    if (it == oTuple.getEndIterator()) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterNoAttribute, L_ERROR,
                          SPL_RUNTIME_FILTER_NO_ATTRIBUTE(symbol), SPL_FUNC_DBG);
    }
    index = it.index();
    ConstValueHandle val = oTuple.getAttributeValue(index);
    APPTRC(L_DEBUG, "Value of symbol " << symbol << " in tuple is " << val.toString(),
           SPL_FUNC_DBG);

    Meta::Type mt = val.getMetaType();
    if (isList) {
        if (mt != Meta::Type::LIST) {
            APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidAttributeType, L_ERROR,
                              (isList ? SPL_RUNTIME_FILTER_INVALID_ATTRIBUTE_LIST_TYPE
                                      : SPL_RUNTIME_FILTER_INVALID_ATTRIBUTE_TYPE),
                              SPL_FUNC_DBG);
        }
        const List& lt = (const List&)val;
        mt = lt.getElementMetaType();
    }
    switch (mt) {
        case Meta::Type::INT64:
        case Meta::Type::INT32:
        case Meta::Type::INT16:
        case Meta::Type::INT8:
        case Meta::Type::UINT64:
        case Meta::Type::UINT32:
        case Meta::Type::UINT16:
        case Meta::Type::UINT8:
        case Meta::Type::RSTRING:
        case Meta::Type::FLOAT64:
        case Meta::Type::FLOAT32:
        case Meta::Type::BOOLEAN:
            return mt;
        default:;
    }
    APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidAttributeType, L_ERROR,
                      (isList ? SPL_RUNTIME_FILTER_INVALID_ATTRIBUTE_LIST_TYPE
                              : SPL_RUNTIME_FILTER_INVALID_ATTRIBUTE_TYPE),
                      SPL_FUNC_DBG);
}

// grand unified validate_predicate.  All other valid_predicates
// should be removed or simply forward to GUVP.
Meta::Type validate_predicate(Tuple const& oTuple,
                              string const& symbol,
                              bool hasSubscript,
                              int64_t subscript,
                              FilterExpression::Op op,
                              FilterExpression::Literal const& literal,
                              FilterExpression::ArithmeticOperationsVector const& operations,
                              uint32_t& index)
{
    bool isList = hasSubscript || op == FilterExpression::In;
    if (hasSubscript && op == FilterExpression::In) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterNoSubscriptWithIn, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_IN_INVALID, SPL_FUNC_DBG);
    }

    Meta::Type mt = validate_predicate(oTuple, symbol, isList, index);

    // Arithmetic operations currently require an integer literal
    if (!operations.empty()) {
        if (mt != Meta::Type::INT64 && mt != Meta::Type::INT32 && mt != Meta::Type::INT16 &&
            mt != Meta::Type::INT8 && mt != Meta::Type::UINT64 && mt != Meta::Type::UINT32 &&
            mt != Meta::Type::UINT16 && mt != Meta::Type::UINT8) {
            APPTRCMSGANDTHROW(SPLRuntimeFilterModNotInt64, L_ERROR,
                              SPL_RUNTIME_FILTER_MOD_NOT_INT64, SPL_FUNC_DBG);
        }
    }

    // Check for division by zero
    for (FilterExpression::ArithmeticOperationsVector::const_iterator opIt = operations.begin();
         opIt != operations.end(); ++opIt) {

        FilterExpression::ArithmeticOp arithmeticOperator = opIt->getOperator();
        if (arithmeticOperator == FilterExpression::Mod ||
            arithmeticOperator == FilterExpression::Div) {
            if (opIt->getOperand().equalsZero()) {
                APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                                  SPL_RUNTIME_SUBSCRIPTION_MODULUS_DIVISOR_ZERO, SPL_FUNC_DBG);
            }
        }
    }

    if (hasSubscript && subscript < 0) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterNegativeSubscript, L_ERROR,
                          SPL_RUNTIME_FILTER_NEGATIVE_SUBSCRIPT, SPL_FUNC_DBG);
    }

    if (op != FilterExpression::None) {
        validate_lit(mt, literal);
    }

    return mt;
}

// The type of the left operand of an arithmetic operation must match
// the type of the right operand.  Here we cast the left operand, which
// is a literal, to the type of the right operand, which is the type of
// a symbol within a tuple and thus cannot be deduced until a tuple has
// been received.
class FixOperandType
{
  public:
    FixOperandType(FilterExpression::ArithmeticOperationsVector& fixedOperations,
                   FilterExpression::Literal const& typeToMatch)
      : _fixedOperations(fixedOperations)
      , _typeToMatch(typeToMatch)
    {}
    void operator()(FilterExpression::ArithmeticOperation const& op)
    {
        FilterExpression::ArithmeticOp arithmeticOperator = op.getOperator();
        FilterExpression::Literal operand = op.getOperand();
        _fixedOperations.push_back(FilterExpression::ArithmeticOperation(
          arithmeticOperator,
          FilterExpression::Literal::castToMatchingType(operand, _typeToMatch)));
    }

  private:
    FilterExpression::ArithmeticOperationsVector& _fixedOperations;
    FilterExpression::Literal const& _typeToMatch;
};

} // end nameless namespace

FilterExpressionPtr FilterExpression::createPredicate(const Tuple& oTuple,
                                                      const string& symbol,
                                                      Op op,
                                                      const Literal& lit)
{
    uint32_t index = 0;
    bool hasSubscript = false;
    int64 dummySubscript = 0;
    ArithmeticOperationsVector noOperations;
    validate_predicate(oTuple, symbol, hasSubscript, dummySubscript, op, lit, noOperations, index);
    return createPredicate(symbol, hasSubscript, dummySubscript, op, lit, noOperations, index);
}

FilterExpressionPtr FilterExpression::createPredicate(const Tuple& oTuple,
                                                      const string& symbol,
                                                      Op op,
                                                      const Literal& lit,
                                                      int64 subscript)
{
    if (op == In) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterNoSubscriptWithIn, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_IN_INVALID, SPL_FUNC_DBG);
    }
    uint32_t index;
    bool hasSubscript = true;
    ArithmeticOperationsVector noArithmeticOps;
    validate_predicate(oTuple, symbol, hasSubscript, subscript, op, lit, noArithmeticOps, index);
    return createPredicate(symbol, hasSubscript, subscript, op, lit, noArithmeticOps, index);
}

FilterExpressionPtr FilterExpression::createPredicate(std::string const& symbol,
                                                      bool hasSubscript,
                                                      int64_t subscript,
                                                      Op op,
                                                      Literal const& operand,
                                                      ArithmeticOperationsVector& operations,
                                                      uint32_t index)
{

    FilterExpressionPtr p(new FilterExpression(Predicate));

    for (ArithmeticOperationsVector::iterator operation = operations.begin();
         operation != operations.end(); ++operation) {
        ArithmeticOp const& arithmeticOperator = operation->getOperator();
        Literal const& arithmeticOperand = operation->getOperand();

        if (((arithmeticOperator == Mod) || (arithmeticOperator == Div)) &&
            arithmeticOperand.equalsZero()) {
            APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                              SPL_RUNTIME_SUBSCRIPTION_MODULUS_DIVISOR_ZERO, SPL_FUNC_DBG);
        }
    }

    if (op == In && hasSubscript) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterNoSubscriptWithIn, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_IN_INVALID, SPL_FUNC_DBG);
    }

    p->_item->_op = op;
    p->_item->_hasSubscript = hasSubscript;
    p->_item->_index = index;
    p->_item->_lit = operand;
    p->_item->_symbol = symbol;
    p->_item->_subscript = subscript;

    ArithmeticOperationsVector fixedOperations;
    for_each(operations.begin(), operations.end(), FixOperandType(fixedOperations, operand));
    p->_item->_arithmeticOperations = fixedOperations;

    p->_item->_hasNot = false;

    return p;
}

FilterExpressionPtr FilterExpression::createPredicate(Tuple const& oTuple,
                                                      std::string const& symbol,
                                                      bool hasSubscript,
                                                      int64_t subscript,
                                                      Op op,
                                                      Literal const& operand,
                                                      ArithmeticOperationsVector& operations)
{

    uint32_t index = 0;
    validate_predicate(oTuple, symbol, hasSubscript, subscript, op, operand, operations, index);
    return createPredicate(symbol, hasSubscript, subscript, op, operand, operations, index);
}

FilterExpressionPtr FilterExpression::createModuloPredicate(const Tuple& oTuple,
                                                            const string& symbol,
                                                            Literal const& modDivisor,
                                                            Op op,
                                                            const Literal& lit)
{
    bool hasSubscript = false;
    int64 dummySubscript = 0;
    ArithmeticOperationsVector operations;
    operations.push_back(ArithmeticOperation(Mod, modDivisor));
    return createPredicate(oTuple, symbol, hasSubscript, dummySubscript, op, lit, operations);
}

FilterExpressionPtr FilterExpression::createModuloPredicate(const Tuple& oTuple,
                                                            const string& symbol,
                                                            const Literal& modDivisor,
                                                            Op op,
                                                            const Literal& lit,
                                                            int64 subscript)
{
    bool hasSubscript = true;
    ArithmeticOperationsVector operations;
    operations.push_back(ArithmeticOperation(Mod, modDivisor));
    FilterExpressionPtr p =
      createPredicate(oTuple, symbol, hasSubscript, subscript, op, lit, operations);

    return p;
}

void FilterExpression::addClause(FilterExpressionPtr& subClause)
{
    if (_kind == Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    _subClauses->push_back(subClause);
}

void FilterExpression::removeClause(uint32_t index)
{
    if (_kind == Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    uint32_t n = _subClauses->size();
    if (n < 2) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_REMOVE_LAST(toString()), SPL_FUNC_DBG);
    }
    if (index >= n) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_INVALID_REMOVE(index), SPL_FUNC_DBG);
    }

    // Okay, do it
    _subClauses->erase(_subClauses->begin() + index);
}

const string& FilterExpression::getSymbol() const
{
    if (_kind != Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_symbol;
}

uint32_t FilterExpression::getIndex() const
{
    if (_kind != Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_index;
}

const FilterExpression::Literal& FilterExpression::getLiteral() const
{
    if (_kind != Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_lit;
}

FilterExpression::Op FilterExpression::getOp() const
{
    if (_kind != Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_op;
}

bool FilterExpression::hasSubscript() const
{
    if (_kind != Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    return _item->_hasSubscript;
}

int64 FilterExpression::getSubscript() const
{
    if (_kind != Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
    }

    if (!_item->_hasSubscript) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_INVALID_SUBSCRIPT(toString()), SPL_FUNC_DBG);
    }

    return _item->_subscript;
}

bool FilterExpression::hasModuloDivisor() const
{
    if (_kind == Predicate) {
        if (!_item->_arithmeticOperations.empty()) {
            ArithmeticOp arithmeticOperator = _item->_arithmeticOperations.back().getOperator();
            return arithmeticOperator == Mod;
        }
        return false;
    }

    APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                      SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
}

FilterExpression::Literal const& FilterExpression::getModuloDivisor() const
{
    if (_kind == Predicate) {

        if (!_item->_arithmeticOperations.empty()) {
            ArithmeticOperation operation = _item->_arithmeticOperations.back();
            Literal const& operand = operation.getOperand();

            if (operation.getOperator() != Mod || operand.equalsZero()) {
                APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                                  SPL_RUNTIME_SUBSCRIPTION_INVALID_MODULO_DIVISOR(toString()),
                                  SPL_FUNC_DBG);
            }
            return operand;
        } else {
            APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                              SPL_RUNTIME_SUBSCRIPTION_INVALID_MODULO_DIVISOR(toString()),
                              SPL_FUNC_DBG);
        }
    }
    APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                      SPL_RUNTIME_SUBSCRIPTION_NEED_PREDICATE(toString()), SPL_FUNC_DBG);
}

uint32_t FilterExpression::getSize() const
{
    if (_kind == Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    return _subClauses->size();
}

FilterExpressionPtr FilterExpression::getSubclause(uint32_t index)
{
    if (_kind == Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    uint32_t n = _subClauses->size();
    if (index >= n) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_INVALID_GETSUBCLAUSE(index), SPL_FUNC_DBG);
    }

    // Okay, do it
    return (*_subClauses)[index];
}

FilterExpression const& FilterExpression::getSubclause(uint32_t index) const
{
    if (_kind == Predicate) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_NEED_AND_OR(toString()), SPL_FUNC_DBG);
    }

    uint32_t n = _subClauses->size();
    if (index >= n) {
        APPTRCMSGANDTHROW(SPLRuntimeFilterInvalidClause, L_ERROR,
                          SPL_RUNTIME_SUBSCRIPTION_INVALID_GETSUBCLAUSE(index), SPL_FUNC_DBG);
    }

    // Okay, do it
    return *((*_subClauses)[index]);
}

ostream& FilterExpression::print(ostream& s) const
{
    switch (_kind) {
        case Predicate:
            if (_item->_op == In) {
                s << _item->_lit << " in " << _item->_symbol;
                if (_item->_hasSubscript) {
                    s << '[' << _item->_subscript << ']';
                }
            } else {
                for (ArithmeticOperationsVector::reverse_iterator operation =
                       _item->_arithmeticOperations.rbegin();
                     operation != _item->_arithmeticOperations.rend(); ++operation) {
                    ArithmeticOp op = operation->getOperator();
                    if (op == BNot) {
                        // Special case for bit not: prefix unary operator
                        s << op;
                    } else {
                        // Everything else is infix binary
                        s << "(";
                    }
                }
                s << _item->_symbol;
                if (_item->_hasSubscript) {
                    s << '[' << _item->_subscript << ']';
                }
                for (ArithmeticOperationsVector::iterator operation =
                       _item->_arithmeticOperations.begin();
                     operation != _item->_arithmeticOperations.end(); ++operation) {
                    ArithmeticOp op = operation->getOperator();
                    if (op == BNot) {
                        // Special case for bit not: prefix unary operator .
                        // Nothing needed here.
                    } else {
                        // Everything else is infix binary
                        s << " " << operation->getOperator() << " " << operation->getOperand()
                          << ")";
                    }
                }
                if (_item->_op != FilterExpression::None) {
                    s << ' ' << _item->_op << ' ' << _item->_lit;
                }
            }
            break;
        case NotClause:
            s << '!' << *((*_subClauses)[0]);
            break;
        default: {
            string op = (_kind == AndClause) ? " && " : " || ";
            for (uint32_t i = 0, n = _subClauses->size(); i < n; i++) {
                if (i > 0) {
                    s << op;
                }
                s << '(' << *((*_subClauses)[i]) << ')';
            }
        }
    }
    return s;
}

bool FilterExpression::operator==(const FilterExpression& rhs) const
{
    if (_kind != rhs._kind) {
        // check for some simple equivalences
        if (_kind == Predicate && rhs._subClauses->size() == 1) {
            return *this == *((*rhs._subClauses)[0]);
        }
        if (rhs._kind == Predicate && _subClauses->size() == 1) {
            return *((*_subClauses)[0]) == rhs;
        }

        // no way we could match
        return false;
    }

    switch (_kind) {
        case Predicate:
            if (_item->_op != rhs._item->_op) {
                return false;
            }
            if (_item->_symbol != rhs._item->_symbol) {
                return false;
            }
            if (_item->_hasSubscript != rhs._item->_hasSubscript) {
                return false;
            }
            if (_item->_lit != rhs._item->_lit) {
                return false;
            }
            if (_item->_hasSubscript && _item->_subscript != rhs._item->_subscript) {
                return false;
            }

            if (_item->_arithmeticOperations.size() != rhs._item->_arithmeticOperations.size()) {
                return false;
            }
            for (ArithmeticOperationsVector::size_type index = 0;
                 index < _item->_arithmeticOperations.size(); ++index) {
                ArithmeticOperation const& leftOp = _item->_arithmeticOperations[index];
                ArithmeticOperation const& rightOp = rhs._item->_arithmeticOperations[index];
                if (leftOp.getOperator() != rightOp.getOperator()) {
                    return false;
                }
                if (leftOp.getOperand() != rightOp.getOperand()) {
                    return false;
                }
            }
            break;

        case AndClause:
        case OrClause:
        case NotClause:
            if (_subClauses->size() != rhs._subClauses->size()) {
                return false;
            }
            for (uint32_t i = 0, n = _subClauses->size(); i < n; i++) {
                if (*((*_subClauses)[i]) != *((*rhs._subClauses)[i])) {
                    return false;
                }
            }
            break;
    }
    return true;
}

string FilterExpression::toString() const
{
    ostringstream s;
    s.imbue(locale::classic());
    print(s);
    return s.str();
}

ostream& SPL::operator<<(ostream& ostr, const FilterExpression& v)
{
    return v.print(ostr);
}

ostream& SPL::operator<<(ostream& ostr, const FilterExpression::Literal& v)
{
    return v.print(ostr);
}

ostream& SPL::operator<<(ostream& ostr, FilterExpression::Kind v)
{
    switch (v) {
        case FilterExpression::AndClause:
            return ostr << "AndClause";
        case FilterExpression::OrClause:
            return ostr << "OrClause";
        case FilterExpression::Predicate:
            return ostr << "Predicate";
        case FilterExpression::NotClause:
            return ostr << "NotClause";
    }

    return ostr;
}

ostream& SPL::operator<<(ostream& ostr, FilterExpression::Op v)
{
    return ostr << FilterExpression::opToString(v);
}

ostream& SPL::operator<<(ostream& ostr, FilterExpression::ArithmeticOp v)
{
    return ostr << FilterExpression::arithmeticOpToString(v);
}

ostream& SPL::operator<<(ostream& ostr, FilterExpression::Literal::Kind v)
{
    switch (v) {
        case FilterExpression::Literal::RString:
            ostr << "rstring";
            break;
        case FilterExpression::Literal::Int64:
            ostr << "int64";
            break;
        case FilterExpression::Literal::Int32:
            ostr << "int32";
            break;
        case FilterExpression::Literal::Int16:
            ostr << "int16";
            break;
        case FilterExpression::Literal::Int8:
            ostr << "int8";
            break;
        case FilterExpression::Literal::UInt64:
            ostr << "uint64";
            break;
        case FilterExpression::Literal::UInt32:
            ostr << "uint32";
            break;
        case FilterExpression::Literal::UInt16:
            ostr << "uint16";
            break;
        case FilterExpression::Literal::UInt8:
            ostr << "uint8";
            break;
        case FilterExpression::Literal::Float64:
            ostr << "float64";
            break;
        case FilterExpression::Literal::Float32:
            ostr << "float32";
            break;
        case FilterExpression::Literal::Boolean:
            ostr << "boolean";
            break;
    }
    return ostr;
}

using namespace boost::spirit;
using namespace std::tr1;

struct TupleValidator : FilterExpression::NameValidator
{
    TupleValidator(const Tuple& tuple)
      : _tuple(tuple)
    {}

    virtual Meta::Type validate(const string& name, bool isList, uint32_t& index) const;

  private:
    const Tuple& _tuple;
};

Meta::Type TupleValidator::validate(const string& name, bool isList, uint32_t& index) const
{
    return validate_predicate(_tuple, name, isList, index);
}

namespace {
struct FilterParserState
{
    FilterParserState()
      : expressions()
      , currentLiteral()
      , currentSymbol()
      , currentSubscript()
      , hasSubscript(false)
      , currentArithmeticOperator(FilterExpression::NoArithmeticOp)
      , arithmeticOperations()
      , currentInt()
      , currentOperator(FilterExpression::None)
      , currentOperand()
      , _validator(NULL)
      , hasNot(false)
      , bitNotStack()
    {}

    void setValidator(const FilterExpression::NameValidator& val) { _validator = &val; }

    void clear()
    {
        expressions.clear();
        currentOperator = FilterExpression::None;
        currentArithmeticOperator = FilterExpression::NoArithmeticOp;
        arithmeticOperations.clear();
        hasSubscript = false;
        hasNot = false;
        bitNotStack.clear();
        _validator = NULL;
    }

    void do_symbol(const char* f, const char* l) { currentSymbol.assign(f, l - f); }

    void do_sub(const char* f)
    {
        hasSubscript = true;
        currentSubscript = currentInt.getInt64Value();
    }

    void do_arithmetic_op(const char* b, const char* e)
    {
        string arithmeticOperatorName = string(b, e);
        currentArithmeticOperator =
          FilterExpression::arithmeticOpFromString(arithmeticOperatorName);
    }

    void do_arithmetic(const char* b, const char* e)
    {
        FilterExpression::Literal currentArithmeticOperand = currentInt.getInt64Value();

        FilterExpression::ArithmeticOperation operation = FilterExpression::ArithmeticOperation(
          currentArithmeticOperator, currentArithmeticOperand);
        arithmeticOperations.push_back(operation);
        currentArithmeticOperator = FilterExpression::NoArithmeticOp;
    }

    void do_int64(int64_t v)
    {
        currentInt = v;
        currentLiteral = v;
    }

    void do_uint64(uint64_t v)
    {
        currentInt = v;
        currentLiteral = v;
    }

    void do_real(double v)
    {
        APPTRC(L_DEBUG, "do_real(" << v << ")", SPL_FUNC_DBG);
        currentLiteral = FilterExpression::Literal(v);
        APPTRC(L_DEBUG, "currentLiteral: " << currentLiteral, SPL_FUNC_DBG);
    }

    void do_string(const char* f, const char* l)
    {
        APPTRC(L_DEBUG, "do_string(\"" << std::string(f, l) << "\")", SPL_FUNC_DBG);
        currentLiteral = FilterExpression::Literal(interpretRStringLiteral(string(f, l)));
        APPTRC(L_DEBUG, "currentLiteral: " << currentLiteral, SPL_FUNC_DBG);
    }

    void do_bool(const char* f, const char* l)
    {
        APPTRC(L_DEBUG, "do_bool(" << std::string(f, l) << ")", SPL_FUNC_DBG);
        size_t length = l - f;
        boolean v = strncmp(f, "false", length);
        currentLiteral = FilterExpression::Literal(v);
        APPTRC(L_DEBUG, "currentLiteral: " << currentLiteral, SPL_FUNC_DBG);
    }

    void do_bool_true(const char* f, const char* l)
    {
        currentLiteral = FilterExpression::Literal(boolean(true));
    }

    void do_bool_false(const char* f, const char* l)
    {
        currentLiteral = FilterExpression::Literal(boolean(false));
    }

    void do_op(const char* f, const char* l)
    {
        APPTRC(L_DEBUG, "do_op(\"" << std::string(f, l) << "\")", SPL_FUNC_DBG);
        currentOperator = FilterExpression::opFromString(std::string(f, l));

        APPTRC(L_DEBUG, "currentOperator: " << currentOperator, SPL_FUNC_DBG);
    }

    void do_predicate(const char* f, const char* l)
    {
        // If we find a symbol with no operators, it is a boolean
        // symbol being used as a predicate.
        if (!currentSymbol.empty() && arithmeticOperations.empty()) {
            do_symbol_predicate(f, l);
        }
    }

    void do_symbol_predicate(const char* f, const char* l)
    {
        APPTRC(L_DEBUG, "do_symbol_predicate(\"" << std::string(f, l) << "\")", SPL_FUNC_DBG);

        // A symbol not followed by any arithmetic operators may be either:
        // a symbol representing a boolean value, or a symbol representing
        // an integer value.  If there is a bit not operator preceeding the
        // symbol, then the symbol must be an integer, and is to be operated
        // on by the bit not operator. Otherwise, it is a boolean symbol
        // to be evaluated on its own.
        if (!bitNotStack.empty() && bitNotStack.back()) {
            handle_bit_not();
        } else {
            currentLiteral = FilterExpression::Literal(static_cast<boolean>(true));
            currentOperator = FilterExpression::None;
            do_operation(f, l);
        }
    }

    void do_operation(const char* f, const char* l)
    {
        APPTRC(L_DEBUG, "do_operation(\"" << std::string(f, l) << "\")", SPL_FUNC_DBG);

        if (currentOperator == FilterExpression::In) {
            return;
        }
        if (currentOperator == FilterExpression::None) {
            // If there is no operator, we are either handling a bitwise
            // not operator, or a symbol without operation to be treated
            // as a boolean literal.

            // If the operation at the back of the list is bitwise not,
            // create a fake integer literal.  Otherwise, create a fake
            // boolean literal.

            if (!arithmeticOperations.empty()) {
                FilterExpression::ArithmeticOperation const& topOperation =
                  arithmeticOperations.back();
                if (topOperation.getOperator() == FilterExpression::BNot) {
                    currentArithmeticOperator = FilterExpression::BNot;
                }
                currentLiteral = FilterExpression::Literal(static_cast<int64>(0));
            } else {
                currentLiteral = FilterExpression::Literal(static_cast<boolean>(true));
            }
        }
        uint32_t index = validateOperation();

        FilterExpressionPtr expression = FilterExpression::createPredicate(
          currentSymbol, hasSubscript, currentSubscript, currentOperator, currentLiteral,
          arithmeticOperations, index);
        expressions.push_back(expression);
        if (hasNot) {
            FilterExpressionPtr rhs = expressions.back();
            expressions.pop_back();
            expressions.push_back(FilterExpression::createNotClause(rhs));
        }

        currentSymbol.clear();
        hasSubscript = false;
        hasNot = false;
        bitNotStack.clear();
        currentOperator = FilterExpression::None;
        currentArithmeticOperator = FilterExpression::NoArithmeticOp;
        arithmeticOperations.clear();
    }

    void do_in(const char* f, const char* l)
    {
        currentOperator = FilterExpression::In;
        uint32_t index = validateOperation();
        expressions.push_back(FilterExpression::createPredicate(currentSymbol, FilterExpression::In,
                                                                currentLiteral, index));
        hasSubscript = false;
        currentSymbol.clear();
    }

    void do_and(const char* b, const char* e)
    {
        assert(expressions.size() >= 2);
        FilterExpressionPtr r = expressions.back();
        expressions.pop_back();
        FilterExpressionPtr l = expressions.back();
        expressions.pop_back();
        if (l->getKind() == FilterExpression::AndClause) {
            // We can just add it in to that
            l->addClause(r);
            expressions.push_back(l);
        } else {
            expressions.push_back(FilterExpression::createAndClause(l, r));
        }

        currentSymbol.clear();
    }

    void do_or(const char* b, const char* e)
    {
        assert(expressions.size() >= 2);
        FilterExpressionPtr r = expressions.back();
        expressions.pop_back();
        FilterExpressionPtr l = expressions.back();
        expressions.pop_back();
        if (l->getKind() == FilterExpression::OrClause) {
            // We can just add it in to that
            l->addClause(r);
            expressions.push_back(l);
        } else {
            expressions.push_back(FilterExpression::createOrClause(l, r));
        }
        currentSymbol.clear();
    }

    void do_not(const char) { hasNot = true; }

    // Each primary expression either starts with a bit not operator,
    // or does not.  We need to apply the operator only after the primary
    // expression has been evaluated, but the operator appears, optionally,
    // before the expression.  Primary expressions may be nested.  Thus
    // we keep a stack of booleans, indicating whether each primary
    // expression was or was not preceeded by a bit not operator.
    // This is the case where there is a bit not operator
    void do_bit_not(const char) { bitNotStack.push_back(true); }

    // This is the case where there is not a bit not operator
    void do_no_bit_not() { bitNotStack.push_back(false); }

    void do_handle_bit_not(const char* b, const char* e) { handle_bit_not(); }

    void handle_bit_not()
    {
        bool hasBitNot = bitNotStack.back();
        bitNotStack.pop_back();
        if (hasBitNot) {
            // This is a unary operation.  We put it into the
            // operations vector with a dummy operand.
            FilterExpression::ArithmeticOperation operation = FilterExpression::ArithmeticOperation(
              FilterExpression::BNot, FilterExpression::Literal());
            arithmeticOperations.push_back(operation);
        }
    }

    void do_recurse(const char)
    {
        APPTRC(L_DEBUG, "recursing into sub expression", SPL_FUNC_DBG);
        currentSymbol.clear();
    }

    void do_operator(const char* b, const char* e)
    {
        currentOperator = FilterExpression::opFromString(std::string(b, e));
    }

    void do_operand(const char* b, const char* e) { currentOperand = currentLiteral; }

    template<typename TargetType>
    bool castCurrentLiteralTo()
    {
        FilterExpression::Literal targetLiteralType = FilterExpression::Literal(TargetType());

        // If they are not the same type, attempt to cast
        if (currentLiteral.getKind() != targetLiteralType.getKind()) {
            currentLiteral =
              FilterExpression::Literal::castToMatchingType(currentLiteral, targetLiteralType);
        }

        // We succeed if they were the same type, or the cast succeeded.
        if (currentLiteral.getKind() == targetLiteralType.getKind()) {
            return true;
        }

        return false;
    }

    uint32_t validateOperation()
    {
        bool valid = true;
        uint32_t index;
        Meta::Type mt = _validator->validate(
          currentSymbol, hasSubscript || currentOperator == FilterExpression::In, index);
        if (mt == Meta::Type::INVALID) {
            // This is unreachable.  _validator->validate either returns
            // a valid meta type, or throws an exception.
            valid = false;
        } else {
            if (hasSubscript && currentSubscript < 0) {
                APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                                  SPL_RUNTIME_FILTER_NEGATIVE_SUBSCRIPT, SPL_FUNC_DBG);
            }
            // Must be integer if arithmetic (TODO float)
            if (currentArithmeticOperator != FilterExpression::NoArithmeticOp) {
                if (!isNumeric(mt)) {
                    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                                      SPL_RUNTIME_FILTER_MOD_NOT_INT64, SPL_FUNC_DBG);
                }
            }

            if (currentOperator == FilterExpression::None &&
                currentArithmeticOperator != FilterExpression::BNot) {
                if (mt != Meta::Type::BOOLEAN) {
                    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                                      SPL_RUNTIME_FILTER_NOT_BOOLEAN(currentSymbol), SPL_FUNC_DBG);
                } else {
                    valid = true;
                }
            }

            else {
                FilterExpression::Literal::Kind litKind = currentLiteral.getKind();

                // mt must match currentLiteral type
                if (valid) {
                    switch (mt) {
                        case Meta::Type::INT64:
                            valid = castCurrentLiteralTo<int64>();
                            break;
                        case Meta::Type::INT32:
                            valid = castCurrentLiteralTo<int32>();
                            break;
                        case Meta::Type::INT16:
                            valid = castCurrentLiteralTo<int16>();
                            break;
                        case Meta::Type::INT8:
                            valid = castCurrentLiteralTo<int8>();
                            break;
                        case Meta::Type::UINT64:
                            valid = castCurrentLiteralTo<uint64>();
                            break;
                        case Meta::Type::UINT32:
                            valid = castCurrentLiteralTo<uint32>();
                            break;
                        case Meta::Type::UINT16:
                            valid = castCurrentLiteralTo<uint16>();
                            break;
                        case Meta::Type::UINT8:
                            valid = castCurrentLiteralTo<uint8>();
                            break;
                        case Meta::Type::FLOAT64:
                            valid = castCurrentLiteralTo<float64>();
                            break;
                        case Meta::Type::FLOAT32:
                            valid = castCurrentLiteralTo<float32>();
                            break;
                        case Meta::Type::RSTRING:
                            if (litKind != FilterExpression::Literal::RString) {
                                valid = false;
                            }
                            break;
                        case Meta::Type::BOOLEAN:
                            if (litKind == FilterExpression::Literal::Boolean) {
                                // good if the operation is == or !=
                                if (currentOperator != FilterExpression::Equal &&
                                    currentOperator != FilterExpression::NotEqual &&
                                    currentOperator != FilterExpression::In &&
                                    currentOperator != FilterExpression::None) {
                                    std::stringstream operation;
                                    std::stringstream type;
                                    operation << currentOperator;
                                    type << litKind;
                                    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                                                      SPL_RUNTIME_FILTER_INVALID_OPERATION(
                                                        operation.str(), type.str()),
                                                      SPL_FUNC_DBG);
                                }
                            } else if (currentOperator == FilterExpression::None) {
                                // The literal kind is not relevant in this case
                            } else {
                                valid = false;
                            }
                            break;
                        default:
                            valid = false;
                    }
                }
            }
            // this should only happen if types mismatch and cannot be
            // coerced.
            if (!valid) {
                std::stringstream literalString;
                literalString << currentLiteral;
                std::stringstream mtString;
                mtString << mt;
                APPTRCMSGANDTHROW(
                  SPLRuntimeFilterValidationFailed, L_ERROR,
                  SPL_RUNTIME_FILTER_CAST_FAILED(literalString.str(), mtString.str()),
                  SPL_FUNC_DBG);
            }
        }

        if (!valid) {
            // This is probably unreachable.  Ideally, we don't even get here
            // but instead throw a more specific error above.
            THROW(SPLRuntimeFilterValidationFailed, "Filter validation failed");
        }
        return index;
    }

    vector<FilterExpressionPtr> expressions;
    FilterExpression::Literal currentLiteral;
    string currentSymbol;
    int64 currentSubscript;
    bool hasSubscript;
    FilterExpression::ArithmeticOp currentArithmeticOperator;
    FilterExpression::ArithmeticOperationsVector arithmeticOperations;
    FilterExpression::Literal currentInt;
    FilterExpression::Op currentOperator;
    FilterExpression::Literal currentOperand;
    const FilterExpression::NameValidator* _validator;
    bool hasNot;
    // Each primary expression either starts with a bit not operator,
    // or does not.  We need to apply the operator only after the primary
    // expression has been evaluated, but the operator appears, optionally,
    // before the expression.  Primary expressions may be nested.  Thus
    // we keep a stack of booleans, indicating whether each primary
    // expression was or was not preceeded by a bit not operator.
    std::vector<bool> bitNotStack;
};

#define BIND0(x) boost::bind(&FilterParserState::x, &const_cast<filterExpn&>(self).state)
#define BIND1(x) boost::bind(&FilterParserState::x, &const_cast<filterExpn&>(self).state, _1)
#define BIND2(x) boost::bind(&FilterParserState::x, &const_cast<filterExpn&>(self).state, _1, _2)

struct filterExpn : public grammar<filterExpn>
{
    template<typename ScannerT>
    struct definition
    {
        definition(filterExpn const& self)
        {
#ifdef BOOST_SPIRIT_DEBUG
            static int debugFlags =
              (0 != getenv("STREAMS_FILTER_EXPRESSION_DEBUG")) ? BOOST_SPIRIT_DEBUG_FLAGS : 0;
#endif

            BOOST_SPIRIT_DEBUG_TRACE_NODE(orExpression, debugFlags);
            orExpression = andExpression >> *(("||" >> andExpression)[BIND2(do_or)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(andExpression, debugFlags);
            andExpression = predicate >> *(("&&" >> predicate)[BIND2(do_and)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(predicate, debugFlags);
            predicate = bitOrExpression[BIND2(do_predicate)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(bitOrExpression, debugFlags);
            bitOrExpression = bitXorExpression >> *(((str_p("|") - "||")[BIND2(do_arithmetic_op)] >>
                                                     bitXorExpression)[BIND2(do_arithmetic)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(bitXorExpression, debugFlags);
            bitXorExpression =
              bitAndExpression >>
              *((str_p("^")[BIND2(do_arithmetic_op)] >> bitAndExpression[BIND2(do_arithmetic)]));

            BOOST_SPIRIT_DEBUG_TRACE_NODE(bitAndExpression, debugFlags);
            bitAndExpression = inExpression >> *(((str_p("&") - "&&")[BIND2(do_arithmetic_op)] >>
                                                  inExpression)[BIND2(do_arithmetic)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(inExpression, debugFlags);
            inExpression = literal >> ("in" >> relExpression)[BIND2(do_in)] | relExpression;

            BOOST_SPIRIT_DEBUG_TRACE_NODE(relExpression, debugFlags);
            relExpression =
              shiftExpression >> !(relOp[BIND2(do_op)] >> literal)[BIND2(do_operation)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(shiftExpression, debugFlags);
            shiftExpression =
              addExpression >>
              *((shiftOp[BIND2(do_arithmetic_op)] >> integer)[BIND2(do_arithmetic)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(addExpression, debugFlags);
            addExpression =
              mulExpression >>
              *((addOp[BIND2(do_arithmetic_op)] >> numberLiteral)[BIND2(do_arithmetic)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(mulExpression, debugFlags);
            mulExpression =
              castExpression >>
              *((mulOp[BIND2(do_arithmetic_op)] >> numberLiteral)[BIND2(do_arithmetic)]);

            BOOST_SPIRIT_DEBUG_TRACE_NODE(castExpression, 0);
            castExpression = postfixExpression
              // | '(' type ')' castExpression // not supported
              ;

            BOOST_SPIRIT_DEBUG_TRACE_NODE(postfixExpression, debugFlags);
            postfixExpression = prefixExpression >> !('[' >> integer[BIND1(do_sub)] >> ']');

            BOOST_SPIRIT_DEBUG_TRACE_NODE(prefixExpression, debugFlags);
            prefixExpression = notExpression | bitNotExpression[BIND2(do_handle_bit_not)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(notExpression, debugFlags);
            notExpression = ch_p('!')[BIND1(do_not)] >> primaryExpression;

            BOOST_SPIRIT_DEBUG_TRACE_NODE(bitNotExpression, debugFlags);
            bitNotExpression =
              (ch_p('~')[BIND1(do_bit_not)] | eps_p[BIND0(do_no_bit_not)]) >> primaryExpression;

            BOOST_SPIRIT_DEBUG_TRACE_NODE(primaryExpression, debugFlags);
            primaryExpression = intrinsic | functionCall | identifier | literal |
                                ch_p('(')[BIND1(do_recurse)] >> orExpression >> ch_p(')');

            BOOST_SPIRIT_DEBUG_TRACE_NODE(literal, 0);
            literal = numberLiteral | stringLiteral | booleanLiteral;

            BOOST_SPIRIT_DEBUG_TRACE_NODE(numberLiteral, 0);
            numberLiteral = floating | integer;

            BOOST_SPIRIT_DEBUG_TRACE_NODE(relOp, 0);
            relOp = str_p("<=") | "<" | ">=" | ">" | "==" | "!=";

            BOOST_SPIRIT_DEBUG_TRACE_NODE(shiftOp, 0);
            shiftOp = str_p("<<") | ">>";

            BOOST_SPIRIT_DEBUG_TRACE_NODE(mulOp, 0);
            mulOp = ch_p('%') | '/' | '*';

            BOOST_SPIRIT_DEBUG_TRACE_NODE(addOp, 0);
            addOp = ch_p('+') | '-';

            BOOST_SPIRIT_DEBUG_TRACE_NODE(type, 0);
            type = str_p("boolean") | "int64" | "float64" | "rstring";

            BOOST_SPIRIT_DEBUG_TRACE_NODE(intrinsic, 0);
            intrinsic = nothing_p; // not supported

            BOOST_SPIRIT_DEBUG_TRACE_NODE(functionCall, 0);
            functionCall = function >> '(' >> orExpression >> ')';

            BOOST_SPIRIT_DEBUG_TRACE_NODE(function, 0);
            function = nothing_p; // not supported

            BOOST_SPIRIT_DEBUG_TRACE_NODE(identifier, debugFlags);
            identifier = lexeme_d[(alphaUnderscore_p) >> *(alnumUnderscore_p)][BIND2(do_symbol)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(integer, 0);
            integer = lexeme_d[int64_p[BIND1(do_int64)] >> !ch_p('l')] |
                      lexeme_d[uint64_p[BIND1(do_uint64)]];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(floating, 0);
            floating = strict_real_p[BIND1(do_real)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(stringLiteral, 0);
            stringLiteral = confix_p('"', *c_escape_ch_p, '"')[BIND2(do_string)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(booleanLiteral, 0);
            booleanLiteral =
              str_p("true")[BIND2(do_bool_true)] | str_p("false")[BIND2(do_bool_false)];

            BOOST_SPIRIT_DEBUG_TRACE_NODE(alphaUnderscore_p, 0);
            alphaUnderscore_p = alpha_p | ch_p('_');

            BOOST_SPIRIT_DEBUG_TRACE_NODE(alnumUnderscore_p, 0);
            alnumUnderscore_p = alnum_p | ch_p('_');
        }
        rule<ScannerT> const& start() const { return orExpression; }
        rule<ScannerT> orExpression, andExpression, predicate, bitOrExpression, bitXorExpression,
          bitAndExpression, inExpression, relExpression, shiftExpression, addExpression,
          mulExpression, castExpression, postfixExpression, prefixExpression, notExpression,
          bitNotExpression, primaryExpression, relOp, shiftOp, mulOp, addOp, type, intrinsic,
          functionCall, function, identifier, integer, floating, literal, stringLiteral,
          booleanLiteral, numberLiteral, alphaUnderscore_p, alnumUnderscore_p;
    };
    FilterParserState state;
};

static Distillery::Mutex parserMutex_;
static __thread filterExpn* parser_ = NULL;
static vector<boost::shared_ptr<filterExpn> > parsers_;
static void allocateParser()
{
#ifdef BOOST_SPIRIT_DEBUG
    static int debugFlags =
      (0 != getenv("STREAMS_FILTER_EXPRESSION_DEBUG")) ? BOOST_SPIRIT_DEBUG_FLAGS : 0;
#endif

    parser_ = new filterExpn();
    BOOST_SPIRIT_DEBUG_TRACE_NODE_NAME(*parser_, "filterExpn", debugFlags);
    Distillery::AutoMutex am(parserMutex_);
    parsers_.push_back(boost::shared_ptr<filterExpn>(parser_));
}
} // end nameless namespace

FilterExpressionPtr FilterExpression::fromString(const Tuple& oTuple, const string& str)
{
    string s = boost::trim_copy(str);
    APPTRC(L_DEBUG, "Parsing filter " << s << " with tuple", SPL_FUNC_DBG);
#if defined(BOOST_SPIRIT_DEBUG)
    BOOST_SPIRIT_DEBUG_OUT << "Parsing filter " << s << " with tuple" << std::endl;
#endif
    if (s.empty()) {
        // This is the empty string; just return an empty 'or' clause
        FilterExpressionPtr p(new FilterExpression(OrClause));
        return p;
    }

    if (!parser_) {
        allocateParser();
    }

    TupleValidator validator(oTuple);
    parser_->state.setValidator(validator);
    FilterExpressionPtr result;
    parse_info<> info;
    try {
        info = parse(s.c_str(), *parser_, space_p);
    } catch (const SPLRuntimeFilterException& rfe) {
        APPTRC(L_ERROR, rfe.what(), SPL_FUNC_DBG);
        parser_->state.clear();
        return result; // empty == failure
    }

    if (info.full) { // worked
        assert(parser_->state.expressions.size() == 1);
        result = parser_->state.expressions[0];
    } else {
        if (info.hit) {
            APPTRCMSGANDTHROW(SPLRuntimeFilterParseFailed, L_ERROR,
                              SPL_RUNTIME_FILTER_PARSE_FAILED_AT(s, info.stop), SPL_FUNC_DBG);
        } else {
            APPTRCMSGANDTHROW(SPLRuntimeFilterParseFailed, L_ERROR,
                              SPL_RUNTIME_FILTER_PARSE_FAILED(s), SPL_FUNC_DBG);
        }
    }

    parser_->state.clear();
    return result;
}

FilterExpressionPtr FilterExpression::fromString(const NameValidator& validator, const string& str)
{
    string s = boost::trim_copy(str);

    APPTRC(L_DEBUG, "Parsing filter " << s << " with name validator", SPL_FUNC_DBG);

    if (s.empty()) {
        // This is the empty string; just return an empty 'or' clause
        FilterExpressionPtr p(new FilterExpression(OrClause));
        return p;
    }

    if (!parser_) {
        allocateParser();
    }
    FilterExpressionPtr result;
    parser_->state.setValidator(validator);
    parse_info<> info;
    try {
        info = parse(s.c_str(), *parser_, space_p);
    } catch (const SPLRuntimeFilterException& rfe) {
        APPTRC(L_ERROR, rfe.what(), SPL_FUNC_DBG);
        parser_->state.clear();
        return result; // empty == failure
    }
    if (info.full) { // worked
        assert(parser_->state.expressions.size() == 1);
        result = parser_->state.expressions[0];
    } else {
        if (info.hit) {
            APPTRCMSGANDTHROW(SPLRuntimeFilterParseFailed, L_ERROR,
                              SPL_RUNTIME_FILTER_PARSE_FAILED_AT(s, info.stop), SPL_FUNC_DBG);
        } else {
            APPTRCMSGANDTHROW(SPLRuntimeFilterParseFailed, L_ERROR,
                              SPL_RUNTIME_FILTER_PARSE_FAILED(s), SPL_FUNC_DBG);
        }
    }
    parser_->state.clear();
    return result;
}

FilterExpression::ArithmeticOperationsVector::size_type
FilterExpression::getArithmeticOperationsCount() const
{
    if (_item) {
        return _item->_arithmeticOperations.size();
    }
    return 0;
}

FilterExpression::ArithmeticOperation const& FilterExpression::getArithmeticOperation(
  ArithmeticOperationsVector::size_type operationIndex) const
{

    ArithmeticOperationsVector::size_type maxIndex = 0;
    if (_item) {
        maxIndex = _item->_arithmeticOperations.size();
        if (operationIndex < maxIndex) {
            return _item->_arithmeticOperations[operationIndex];
        }
    }
    APPTRCMSGANDTHROW(SPLRuntimeFilterValidationFailed, L_ERROR,
                      SPL_APPLICATION_RUNTIME_INVALID_INDEX(operationIndex, maxIndex),
                      SPL_FUNC_DBG);
}
