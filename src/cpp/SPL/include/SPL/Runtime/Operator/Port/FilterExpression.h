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

#ifndef FILTER_EXPRESSION_H
#define FILTER_EXPRESSION_H

/*!
 * \file FilterExpression.h \brief Definition of the SPL::FilterExpression class.
 */

#include <SPL/Runtime/Type/SPLType.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <boost/variant/variant.hpp>
#include <tr1/memory>
#include <SPL/Runtime/Type/Meta/Type.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    class FilterExpression;

    typedef std::tr1::shared_ptr<FilterExpression> FilterExpressionPtr;
#ifndef DOXYGEN_SKIP_FOR_USERS
    class TupleTyp;
#endif

    /// \brief Class that represents a filter expression clause for import
    /// specifications.
    class FilterExpression
    {
    public:

        /// Clause kind
        enum Kind
        {
            AndClause,   //!< 'sub1 ( && sub2 )*'
            OrClause,    //!< 'sub1 ( || sub2 )*'
            Predicate,   //!< 'symbol \<op\> literal' or 'symbol[subscript] \<op\> literal' where \<op\> != In or 'literal In symbol'
            NotClause,   //!< '! sub1'
        };

        /// Individual supported relative operations
        enum Op
        {
            LessThan,     //!< Less than
            LessEqual,    //!< Less than or equal
            GreaterThan,  //!< Greater than
            GreaterEqual, //!< Greater than or equal
            Equal,        //!< Equal
            NotEqual,     //!< Not equal
            In,           //!< Contained
            None,         //!< No operator
        };

        // Supported arithmetic operations
        enum ArithmeticOp
        {
            NoArithmeticOp, //!< No arithmetic operator
            Mod,            //!< Modulus
            BitOr,          //!< Bitwise or
            BitAnd,         //!< Bitwise and
            BitXor,         //!< Bitwise exclusive or
            LeftShift,      //!< Left shift
            RightShift,     //!< Right shift

            Add,            //!< Addition
            Sub,            //!< Subtraction
            Mul,            //!< Multiplication
            Div,            //!< Division

            BNot,           //!< Bitwise not
        };

        /// Get the Op from the opName
        static Op opFromString(std::string const& opName);
        /// Get the opName from the Op
        static std::string opToString(Op op);
        /// Get the ArithmeticOp from the opName
        static ArithmeticOp arithmeticOpFromString(std::string const& opName);
        /// Get the opName from the ArithmeticOp
        static std::string arithmeticOpToString(ArithmeticOp op);

        /// Class that represents filter expression literals
        class Literal
        {
        public:
            /// Kind of literal represented
            enum Kind
            {
                RString, //!< value is an rstring
                Int64,   //!< value is an int64
                Int32,   //!< value is an int32
                Int16,   //!< value is an int16
                Int8,    //!< value is an int8
                UInt64,  //!< value is a uint64
                UInt32,  //!< value is a uint32
                UInt16,  //!< value is a uint16
                UInt8,   //!< value is a uint8
                Float64, //!< value is a float64
                Float32, //!< value is a float32
                Boolean, //!< value is a boolean
            };

            /// Constructor for creating a default literal
            /// The default value is 0 of type Int64
            Literal () : _value((int64)0) {}

            /// Create an integer literal
            /// @param value int64 value
            Literal (int64 value) : _value(value) {}

            /// Create an integer literal
            /// @param value int32 value
            Literal (int32 value) : _value(value) {}

            /// Create an integer literal
            /// @param value int16 value
            Literal (int16 value) :  _value(value) {}

            /// Create an integer literal
            /// @param value int8 value
            Literal (int8 value) :  _value(value) {}

            /// Create an integer literal
            /// @param value uint64 value
            Literal (uint64 value) : _value(value) {}

            /// Create an integer literal
            /// @param value uint32 value
            Literal (uint32 value) : _value(value) {}

            /// Create an integer literal
            /// @param value uint16 value
            Literal (uint16 value) : _value(value) {}

            /// Create an integer literal
            /// @param value uint8 value
            Literal (uint8 value) : _value(value) {}

            /// Create a string literal
            /// @param value rstring value
            Literal (const rstring & value) : _value(value) {}

            /// Create a string literal
            /// @param value rstring value
            Literal (const char* value) : _value((rstring) value) {}

            /// Create a float literal
            /// @param value float64 value
            Literal (float64 value) : _value(value) {}

            /// Create a float literal
            /// @param value float64 value
            Literal (float32 value) : _value(value) {}

            /// Create a boolean literal
            /// @param value boolean value
            Literal (boolean value) : _value(value) {}
            Literal (bool value) : _value(SPL::boolean(value)) {}

            /// Copy constructor
            /// @param rhs right hand side
            Literal (const Literal& rhs) : _value(rhs._value) {}

            /// Assignment operator
            /// @param rhs right hand side
            Literal& operator= (const Literal& rhs);

            /// Kind of a literal
            /// @return literal kind
            Kind getKind() const { return static_cast<Kind>(_value.which()); }

            /// Integer value of a literal
            /// @pre getKind() == Int64
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            int64 getInt64Value() const;

            /// Integer value of a literal
            /// @pre getKind() == Int32
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            int32 getInt32Value() const;

            /// Integer value of a literal
            /// @pre getKind() == Int16
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            int16 getInt16Value() const;

            /// Integer value of a literal
            /// @pre getKind() == Int8
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            int8 getInt8Value() const;

            /// Integer value of a literal
            /// @pre getKind() == UInt64
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            uint64 getUInt64Value() const;

            /// Integer value of a literal
            /// @pre getKind() == UInt32
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            uint32 getUInt32Value() const;

            /// Integer value of a literal
            /// @pre getKind() == UInt16
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            uint16 getUInt16Value() const;

            /// Integer value of a literal
            /// @pre getKind() == UInt8
            /// @return integer value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            uint8 getUInt8Value() const;

            /// Float value of a literal
            /// @pre getKind() == Float64
            /// @return float value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            float64 getFloat64Value() const;

            /// Float value of a literal
            /// @pre getKind() == Float32
            /// @return float value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            float32 getFloat32Value() const;

            /// String value of a literal
            /// @pre getKind() == RString
            /// @return string value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            const rstring & getRStringValue() const;

            /// Boolean value of a literal
            /// @pre getKind() == Boolean
            /// @return boolean value
            /// @throws SPLRuntimeFilterInvalidLiteralException if the
            /// literal is of non-matching type
            boolean getBooleanValue() const;

            /// Returns true of the value of this literal is an integer
            /// of any type.
            bool isInteger() const {
                Kind kind = getKind();
                return kind == Int64 || kind == Int32 || kind == Int16 ||
                       kind == Int8 || kind == UInt64 || kind == UInt32 ||
                       kind == UInt16 || kind == UInt8;
            }

            /// Compare two literals for equality
            /// @param rhs literal to compare to
            bool operator== (const Literal& rhs) const;

            /// Compare two literals for inequality
            /// @param rhs literal to compare to
            bool operator!= (const Literal& rhs) const
                { return !this->operator==(rhs); }

            /// If the literal is a numeric value
            /// test whether is is zero.
            bool equalsZero() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
            friend std::ostream & operator<<(std::ostream &, const FilterExpression::Literal&);
            friend std::ostream & operator<<(std::ostream &, const FilterExpression::Literal::Kind);

            static const Literal castToMatchingType(const Literal& value, const Literal& matchKind);

            template<typename T>
            T const& getValue() const;

        private:
            std::ostream& print(std::ostream& s) const;

            // Note these must be in the same order as enum Kind above.
            boost::variant<rstring, int64, int32, int16, int8, uint64, uint32, uint16, uint8, float64, float32, boolean> _value;
#endif /*DOXYGEN_SKIP_FOR_USERS*/
        };

        /// An ArithmeticOperation is a pair of an operator and an operand.
        /// The operation may be binary or unary.  If the operation is binary
        /// Applying an ArithmeticOperation to a value will treat the
        /// value as the left operand of the operator, and the operand
        /// as the right operand.  If the operation is unary, the operand
        /// is ignored, and the operation is applied to the value.
        class ArithmeticOperation
        {
        public:
          /// Construct a default ArithmeticOperation.  This will have
          /// no operator, and a default operand.
          ArithmeticOperation() : _operator(NoArithmeticOp), _operand() {}

          /// Construct an ArithmeticOperation with the specified operator and
          /// operand
          ArithmeticOperation(ArithmeticOp op, Literal operand) : _operator(op), _operand(operand) {}

          /// Get the operator
          ArithmeticOp getOperator() const { return _operator; }
          /// Get the operand
          Literal const & getOperand() const { return _operand; }

          /// Test for equality between this and another ArithmeticOperation
          bool operator == (ArithmeticOperation const& other) const {
            return _operator == other._operator && _operand == other._operand;
          }

          /// Test for inequality between this and another ArithmeticOperation
          bool operator != (ArithmeticOperation const& other) const {
            return !operator ==(other);
          }
        private:
          ArithmeticOp _operator;
          Literal _operand;
        };
        typedef std::vector<ArithmeticOperation> ArithmeticOperationsVector;

    public:
        /// Destructor
        ///
        ~FilterExpression();

        /// Create a predicate clause ('symbol \<op\> literal' where \<op\> != In or 'literal In symbol')
        /// @param symbol symbol name
        /// @param oTuple Output Tuple for Import operator
        /// @param op operation
        /// @param lit literal to compare to
        /// @return shared pointer to the predicate clause
        /// @throws SPLRuntimeSubscriptionNoAttributeException if attribute is not in tuple
        /// @throws SPLRuntimeSubscriptionInvalidAttributeTypeException if attribute type is incorrect
        /// @throws SPLRuntimeSubscriptionTypeMismatchException if attribute typed doesn't match lit
        static FilterExpressionPtr createPredicate (const Tuple& oTuple, const std::string& symbol,
                                                    Op op, const Literal& lit);

        /// Create a predicate clause with a subscript on the symbol
        /// ('symbol[subscript] \<op\> literal' where \<op\> != In)
        /// @param oTuple Output Tuple for Import operator
        /// @param symbol symbol name
        /// @param op operation
        /// @param lit literal to compare to
        /// @param subscript subscript value
        /// @return shared pointer to the predicate clause
        /// @throws SPLRuntimeFilterNoSubscriptWithInException if the
        /// operator specified by \a op is equal to \c Op::In
        /// @throws SPLRuntimeSubscriptionNoAttributeException if attribute is not in tuple
        /// @throws SPLRuntimeSubscriptionInvalidAttributeTypeException if attribute type is incorrect
        /// @throws SPLRuntimeSubscriptionTypeMismatchException if attribute typed doesn't match lit
        static FilterExpressionPtr createPredicate (const Tuple& oTuple, const std::string& symbol,
                                                    Op op, const Literal& lit, int64 subscript);

        /// Create a predicate clause ('symbol % int64Value \<op\> int64Value2' where \<op\> != In)
        /// @param oTuple Output Tuple for Import operator
        /// @param symbol symbol name
        /// @param modDivisor divisor for modulus
        /// @param op operation
        /// @param lit value to compare to modulus
        /// @pre modDivisor != 0 && op != In
        /// @return shared pointer to the predicate clause
        /// @throws SPLRuntimeFilterNoSubscriptWithInException if the
        /// operator specified by \a op is equal to \c Op::In
        /// @throws SPLRuntimeFilterInvalidClauseException if modDivisor == 0
        /// @throws SPLRuntimeSubscriptionNoAttributeException if attribute is not in tuple
        /// @throws SPLRuntimeSubscriptionInvalidAttributeTypeException if attribute type is incorrect
        /// @throws SPLRuntimeSubscriptionTypeMismatchException if attribute typed doesn't match lit
        /// @throws SPLRuntimeSubscriptionModNotInt64Exception if attribute type is not int64
        static FilterExpressionPtr createModuloPredicate (const Tuple& oTuple,
                                                          const std::string& symbol,
                                                          const Literal& modDivisor, Op op,
                                                          const Literal& lit);

        /// Create a predicate clause with a subscript on the symbol
        /// Create a predicate clause ('symbol[subscript] % int64Value \<op\> int64Value2' where \<op\> != In)
        /// @param oTuple Output Tuple for Import operator
        /// @param symbol symbol name
        /// @param modDivisor divisor for modulus
        /// @param op operation
        /// @param lit value to compare to modulus
        /// @param subscript subscript value
        /// @pre modDivisor != 0
        /// @return shared pointer to the predicate clause
        /// @throws SPLRuntimeFilterNoSubscriptWithInException if the
        /// operator specified by \a op is equal to \c Op::In
        /// @throws SPLRuntimeFilterInvalidClauseException if modDivisor == 0
        /// @throws SPLRuntimeSubscriptionNoAttributeException if attribute is not in tuple
        /// @throws SPLRuntimeSubscriptionInvalidAttributeTypeException if attribute type is incorrect
        /// @throws SPLRuntimeSubscriptionTypeMismatchException if attribute typed doesn't match lit
        /// @throws SPLRuntimeSubscriptionModNotInt64Exception if attribute type is not int64
        static FilterExpressionPtr createModuloPredicate (const Tuple& oTuple,
                                                          const std::string& symbol,
                                                          const Literal& modDivisor, Op op,
                                                          const Literal& lit, int64 subscript);

        /// Create an and-clause with one subclause
        /// @param subclause first subclause
        /// @return shared pointer to and-clause with one subclause
        static FilterExpressionPtr createAndClause (FilterExpressionPtr& subclause);

        /// Create an and-clause with two subclauses
        /// @param lhs first subclause
        /// @param rhs second subclause
        /// @return shared pointer to and-clause with two subclauses
        static FilterExpressionPtr createAndClause (FilterExpressionPtr & lhs,
                                                    FilterExpressionPtr & rhs);

        /// Create an or-clause with one subclause
        /// @param subclause first subclause
        /// @return shared pointer to or-clause with one subclause
        static FilterExpressionPtr createOrClause (FilterExpressionPtr & subclause);

        /// Create an or-clause with two subclauses
        /// @param lhs first subclause
        /// @param rhs second subclause
        /// @return shared pointer to or-clause with two subclauses
        static FilterExpressionPtr createOrClause (FilterExpressionPtr & lhs,
                                                   FilterExpressionPtr & rhs);

        /// Create a not-clause with one subclause
        /// @param rhs the subclause
        /// @return shared pointer to not-clause with subclause
        static FilterExpressionPtr createNotClause(FilterExpressionPtr & subclause);

        /// Create a filter expression from a string
        /// @param oTuple output tuple from the Import operator (to check names/types)
        /// @param str string form of filter expression
        /// @return pointer to created FilterExpression or NULL if unable
        /// to form a valid expression
        static FilterExpressionPtr fromString (const Tuple& oTuple, const std::string& str);

        /// Get the string representation of the filter expression
        /// @return string representation of the filter expression
        std::string toString() const;

        /// Return the number of subclauses in an and/or-clause
        /// @pre getKind() != Predicate
        /// @return number of subclauses in the clause
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression represents a predicate clause
        uint32_t getSize () const;

        /// Return a subclause of an and/or-clause
        /// @pre getKind() != Predicate
        /// @pre index < getSize()
        /// @param index subclause index
        /// @return subclause at the specified index
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// number of subclauses are equal to or less than the index specified,
        /// or the filter expression represents a predicate clause
        FilterExpressionPtr getSubclause (uint32_t index);

        /// Return a subclause of an and/or-clause (const verstion)
        /// Note that this returns a const reference, not a shared pointer,
        /// which could cause problems if the original is removed while the
        /// returned value is being used.
        /// @pre getKind() != Predicate
        /// @pre index < getSize()
        /// @param index subclause index
        /// @return subclause at the specified index
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// number of subclauses are equal to or less than the index specified,
        /// or the filter expression represents a predicate clause
        FilterExpression const & getSubclause (uint32_t index) const;

        /// Append a subclause to an and/or-clause
        /// @pre getKind() != Predicate
        /// @param subclause clause to be appended
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression represents a predicate clause
        void addClause (FilterExpressionPtr& subclause);

        /// Remove a subclause from an and/or clause
        /// @pre getKind() != Predicate
        /// @pre getSize() > 1
        /// @pre getSize() > index
        /// @param index clause to be removed
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression represents a predicate clause or if the
        /// index is invalid (greater than or equal to the number of clauses)
        void removeClause (uint32_t index);

        /// Get the kind of the filter expression (and clause, or clause, or a predicate)
        /// @return kind of the filter expression
        Kind getKind() const { return _kind;}

        /// Return the symbol in a predicate clause
        /// @pre getKind() == Predicate
        /// @return symbol
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression does not represent a predicate clause
        const std::string & getSymbol() const;

        /// Return the literal in a predicate
        /// @pre getKind() == Predicate
        /// @return literal
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression does not represent a predicate clause
        const Literal & getLiteral() const;

        /// Return the operator in an predicate clause
        /// @pre getKind() == Predicate
        /// @return operation
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression does not represent a predicate clause
        Op getOp() const;

        /// Return true if the symbol in a predicate clause has a subscript
        /// @pre getKind() == Predicate
        /// @return true if the item has a subscript
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression does not represent a predicate clause
        bool hasSubscript() const;

        /// Return subscript for the symbol in a predicate clause
        /// @pre getKind() == Predicate && hasSubscript()
        /// @return subscript value
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression does not represent a predicate clause or if
        /// it does not have a subscript
        int64 getSubscript() const;

        /// Return true if the symbol in a predicate clause has a modulo divisor
        /// If there are multiple
        /// arithmetic operations, the result is undefined.
        /// @pre getKind() == Predicate
        /// @return true if the item has a modulo divisor
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression does not represent a predicate clause
        /// \deprecated{use getArithmeticOperand instead}
        bool hasModuloDivisor() const;

        /// Return modulo divisor for the symbol in a predicate clause
        /// If there are multiple
        /// arithmetic operations, the result is undefined.
        /// @pre getKind() == Predicate && hasModuloDivisor()
        /// @return modulo divisor value
        /// @throws SPLRuntimeFilterInvalidClauseException if the
        /// filter expression does not represent a predicate clause or if
        /// it does not have a modulo divisor.
        /// \deprecated{use getArithmeticOperand instead}
        const Literal & getModuloDivisor() const;

        /// Is this an empty FilterExpression?
        /// @return true if the filter expression represents an empty expression
        bool isEmpty() const;

        /// Compare two FilterExpressions for equality (recursively)
        /// @param rhs FilterExpression to compare to
        bool operator== (const FilterExpression& rhs) const;

        /// Compare two FilterExpressions for inequality (recursively)
        /// @param rhs FilterExpression to compare to
        bool operator!= (const FilterExpression& rhs) const
        {
            return !this->operator==(rhs);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        struct NameValidator {
            virtual ~NameValidator() {}
            virtual Meta::Type validate (const std::string& name, bool isList,
                                         uint32_t& index) const = 0;
        };

        static FilterExpressionPtr fromString (const NameValidator&, const std::string& str);

        friend std::ostream & operator<<(std::ostream & ostr, const FilterExpression& v);

        // Only for internal Streams use
        static FilterExpressionPtr createPredicate (const std::string& symbol,
                                                    Op op, const Literal& lit, uint32_t index);
#endif /*DOXYGEN_SKIP_FOR_USERS*/

    private:
        /// Constructor
        /// @param k clause kind
        FilterExpression(Kind k);

        std::ostream& print(std::ostream& s) const;

        struct ItemInfo
        {
            Op _op;
            bool _hasSubscript;
            uint32_t _index;
            Literal _lit;
            std::string _symbol;
            int64 _subscript;
            ArithmeticOperationsVector _arithmeticOperations;
            bool _hasNot;

            ItemInfo(): _op(None), _hasSubscript(), _index(), _lit(), _symbol(), _subscript(), _arithmeticOperations(), _hasNot(){}
        };

        Kind _kind;
        std::vector<FilterExpressionPtr> * _subClauses;
        ItemInfo * _item;

    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        uint32_t getIndex() const;

        /// For internal use only
        static FilterExpressionPtr createPredicate(std::string const& symbol, bool hasSubscript, int64_t subscript, Op op, Literal const& operand, ArithmeticOperationsVector & operations, uint32_t index);
        /// For internal use only
        static FilterExpressionPtr createPredicate(Tuple const& oTuple, std::string const& symbol, bool hasSubscript, int64_t subscript, Op op, Literal const& operand, ArithmeticOperationsVector & operations);
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Get the number of arithmetic operations in this expression.
        ArithmeticOperationsVector::size_type getArithmeticOperationsCount() const;
        /// Get an arithmetic operation in this expression.  The operations
        /// are in the order they are to be applied.
        ArithmeticOperation const& getArithmeticOperation(ArithmeticOperationsVector::size_type operationIndex) const;

    };

    // Simple printing functions
    std::ostream & operator<<(std::ostream & ostr, const FilterExpression& v);
    std::ostream & operator<<(std::ostream & ostr, FilterExpression::Kind v);
    std::ostream & operator<<(std::ostream & ostr, FilterExpression::Op v);
    std::ostream & operator<<(std::ostream & ostr, const FilterExpression::Literal& v);
    std::ostream & operator<<(std::ostream & ostr, FilterExpression::ArithmeticOp v);
};

#endif /* FILTER_EXPRESSION_H */
