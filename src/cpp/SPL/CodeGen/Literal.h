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

#ifndef LITERAL_H
#define LITERAL_H

#include <UTILS/HashMapHelpers.h>
#include <string>
#include <vector>

namespace SPL {
class Expression;
class ExpressionValue;
class RootTyp;
class FunctionHeadSym;
class LiteralReplacer;

//! Hold Literals at compile time
class Literal
{
  public:
    enum Kind
    {
        Invalid,   //!<   <not a valid kind>
        Primitive, //!<   int/float/decimal/rstring/etc.
        Expn,      //!<   non-constant int/float/decimal/rstring/etc.
        List,      //!< [ literal,* ]
        Set,       //!< { literal,* }
        Map,       //!< { (literal : literal),* }
        Tuple,     //!< { (id = literal),* }
        XML,       //!< "<doc><a/></doc>"x
        Nul        //!< null
    };

    struct MapLiteral
    {
        Literal* _key;
        Literal* _value;
    };

    struct TupleLiteral
    {
        std::string _id;
        Literal* _value;
    };

    /// Constructor (can't hold a value)
    Literal();

    /// Constructor
    /// @param type of literal primitive/list/set/map/tuple
    ///        _kind is set from type
    /// @param v Value of literal if primitive
    Literal(const RootTyp& type, const ExpressionValue* v);

    /// Constructor
    /// @param type of literal primitive/list/set/map/tuple
    ///        _kind is set from type
    /// @param varType Type if variable this literal would initialize
    /// @param v Value of literal if primitive
    Literal(const RootTyp& type, const RootTyp& varType, const ExpressionValue* v);

    /// Constructor
    /// @param type of literal primitive/list/set/map/tuple
    ///        _kind is set from type
    /// @param varType Type if variable this literal would initialize
    /// @param v Value of literal if primitive
    /// @param enumValueName For converted enum values, the string name of the enum value
    Literal(const RootTyp& type, const ExpressionValue* v, const std::string& enumValueName);

    /// Copy Constructor
    /// @param rhs Literal to copy
    Literal(const Literal& rhs);

    /// Constructor
    /// @param exp an expression
    Literal(Expression& exp);

    /// Destructor
    ~Literal();

    /// Assignment Operator
    /// @param rhs Literal to assign
    /// @return updated literal (this)
    Literal& operator=(const Literal& rhs);

    /// Return the kind of the literal
    /// @return kind
    Kind getKind() const { return _kind; }

    /// Return the type of the literal
    /// @return type
    const RootTyp& getType() const { return (NULL != _varType) ? *_varType : *_type; }

    /// Return the type of variable this literal is assiciated with
    /// Generally this would be the same as _type, T, unless the the type is optional<T>
    const RootTyp& getVarType() const { return *_varType; }

    /// Add a literal to a list
    /// @pre  getKind() == List
    /// @param l literal
    void addListElement(Literal& l);

    /// Add a literal to a set
    /// @pre  getKind() == Set
    /// @param l literal
    /// @return 'true' if literal already present in the set
    bool addSetElement(Literal& l);

    /// Add a literal pair to a map
    /// @pre  getKind() == Map
    /// @param k key
    /// @param v value
    /// @return 'true' if key already present in the set (Literal unchanged)
    bool addMapElement(Literal& k, Literal& v);

    /// Add a literal pair to a tuple
    /// @pre  getKind() == Tuple
    /// @param k id
    /// @param v value
    /// @return 'true' if key already present in the set (Literal unchanged)
    bool addTupleElement(const std::string& k, Literal& v);

    /// Return the number of elements in a composite literal
    /// @pre getKind() != Primitive
    /// @return number of elements
    uint32_t numElements() const;

    /// Return Value of a primitive
    /// @pre getKind() == Primitive
    /// @return value
    const ExpressionValue& primitive() const;

    /// Return Value of an expression
    /// @pre getKind() == Expn
    /// @return value
    const Expression& expression() const;

    /// Return Value of an expression
    /// @pre getKind() == Expn
    /// @return value
    Expression& expression();

    /// Return the i-th of element of a set/list
    /// @pre getKind() == List || getKind() == Set
    /// @pre i < numberOfElements()
    /// @param i element number
    /// @return The i-th literal in the list/set
    const Literal& listSetElement(uint32_t i) const;

    /// Return the i-th of element of a set/list
    /// @pre getKind() == List || getKind() == Set
    /// @pre i < numberOfElements()
    /// @param i element number
    /// @return The i-th literal in the list/set
    Literal& listSetElement(uint32_t i);

    /// Return the i-th of element of a map
    /// @pre getKind() == Map
    /// @pre i < numberOfElements()
    /// @param i element number
    /// @return The i-th literal in the list/set
    const MapLiteral& mapElement(uint32_t i) const;

    /// Return the i-th of element of a map
    /// @pre getKind() == Map
    /// @pre i < numberOfElements()
    /// @param i element number
    /// @return The i-th literal in the list/set
    MapLiteral& mapElement(uint32_t i);

    /// Return the i-th of element of a tuple
    /// @pre getKind() == Tuple
    /// @pre i < numberOfElements()
    /// @param i element number
    /// @return The i-th literal in the tuple
    const TupleLiteral& tupleElement(uint32_t i) const;

    /// Return the i-th of element of a tuple
    /// @pre getKind() == Tuple
    /// @pre i < numberOfElements()
    /// @param i element number
    /// @return The i-th literal in the tuple
    TupleLiteral& tupleElement(uint32_t i);

    /// Compare two Literals for Equality
    /// @param rhs Literal to compare to
    /// @return true if identical
    bool operator==(const Literal& rhs) const;

    /// Compare two Literals for Inequality
    /// @param rhs Literal to compare to
    /// @return true if not identical
    bool operator!=(const Literal& rhs) const { return !((*this) == rhs); }

    /// Compare two Literals for Greater Than
    /// @param rhs Literal to compare to
    /// @return true *this > rhs
    bool operator>(const Literal& rhs) const;

    /// Clone a literal
    /// @return a copy of the literal
    Literal& clone() const;

    /// Generate C++ for a primitive value
    /// @param ostr   stream to generate code on
    /// @param justInitializer 'true' if just the initializer expression is to be generated
    /// @param genTemplate Generate a template for C++ code for SPL expression tree
    void generate(std::ostream& ostr, bool justInitializer, bool genTemplate = false) const;

    /// Does this literal contain only compile time evaluatable values?
    /// @return 'false' if it contains a runtime literal, otherwise 'true'
    bool compileTimeEvaluatable() const;

    /// Does this literal contain expressions with side effects?
    /// @return 'true' if it contains an expression with side effects,
    /// otherwise 'false'
    bool hasSideEffects(void) const;

    /// Does this literal contain expressions that read state
    /// @return 'true' if it contains an expression that read state, otherwise false
    bool readsState() const;

    /// Does this literal contain composite literals with no members?
    /// @return 'true' if it contains composite literals with no members?
    /// otherwise 'false'
    bool hasEmptyCompositeLiterals(void) const;

    /// Return the name of an enum value (if any)
    /// @return string name of the enum value, or NULL if not derived from an enum
    const std::string* enumValueName() const;

    /// Collect Type and Function Information
    /// @param types   if non-NULL, named types will be added
    /// @param fcns    if non-NULL, referenced functions will be added
    /// @param inFcns  if true, walk into referenced functions
    void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                 std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                 bool inFcns) const;

    /// Replace Literals in an expression literal
    /// @param lit    Literal Replacer to be used
    /// @return existing literal, or new literal with new expression
    Literal* replaceLits(LiteralReplacer& lit);

    /// Is this an empty tuple literal?
    /// @return 'true' if this is tuple literal with no fields
    bool emptyTupleLiteral() const { return _kind == Tuple && _tuple->empty(); }

    // @return true if this literal is allowed to have its code moved for optimization
    bool shouldAllowBackwardsCodeMotion() const;

  private:
    Literal(const RootTyp& type, const RootTyp* varType, Expression& exp);

    Kind _kind;              // What we represent - derived from _type
    const RootTyp* _type;    // SPL type of the literal
    const RootTyp* _varType; // SPL type of the variable for this literal

    union
    {
        Expression* _expn;
        const ExpressionValue* _primitives;
        std::vector<Literal*>* _listSet;
        std::vector<MapLiteral>* _map;
        std::vector<TupleLiteral>* _tuple;
    };
    void initialize(const ExpressionValue* v);
    static std::tr1::unordered_map<const Literal*, std::string> _enumMap;
    inline void destroy();
    inline void copy(const Literal& rhs);
};

std::ostream& operator<<(std::ostream& ostr, const Literal& v);
std::ostream& operator<<(std::ostream& ostr, Literal::Kind v);
};

#endif /* LITERAL_H */
