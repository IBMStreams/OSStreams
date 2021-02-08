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

#ifndef SUBSCRIPTION_EXPRESSION_H
#define SUBSCRIPTION_EXPRESSION_H

/*!
 * \file SubscriptionExpression.h \brief Definition of the SPL::SubscriptionExpression class.
 */

#include <SPL/Runtime/Type/SPLType.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <tr1/memory>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    class SubscriptionExpression;

    typedef std::tr1::shared_ptr<SubscriptionExpression> SubscriptionExpressionPtr;

    /// \brief Class that represents a subscription expression clause for import
    /// specifications.
    class SubscriptionExpression
    {
    public:

        /// Clause kind
        enum Kind
        {
            AndClause,  //!< 'sub1 ( && sub2 )*'
            OrClause,   //!< 'sub1 ( || sub2 )*'
            Predicate   //!< 'symbol \<op\> literal' or 'symbol[subscript] \<op\> literal' where \<op\> != In or 'literal In symbol'
        };

        /// Individual supported operations
        enum Op
        {
            LessThan,     //!< Less than
            LessEqual,    //!< Less than or equal
            GreaterThan,  //!< Greater than
            GreaterEqual, //!< Greater than or equal
            Equal,        //!< Equal
            NotEqual,     //!< Not equal
            In            //!< Contained
        };

        /// Class that represents subscription expression literals
        class Literal
        {
        public:
            /// Kind of literal represented
            enum Kind
            {
                RString, //!< value is an rstring
                Int64,   //!< value is an int64
                Float64  //!< value is an float64
            };

            /// Constructor for creating an empty literal
            Literal () : _kind (Int64), _int64(0) {}

            /// Create an integer literal
            /// @param value int64 value
            Literal (int64 value) : _kind (Int64), _int64(value) {}

            /// Create a string literal
            /// @param value rstring value
            Literal (const rstring & value);

            /// Create a float literal
            /// @param value float64 value
            Literal (float64 value) : _kind (Float64), _float64(value) {}

            /// Destructor
            ///
            ~Literal ();

            /// Copy constructor
            /// @param rhs right hand side
            Literal (const Literal& rhs);

            /// Assignment operator
            /// @param rhs right hand side
            Literal& operator= (const Literal& rhs);

            /// Kind of a literal
            /// @return literal kind
            Kind getKind() const { return _kind; }

            /// Integer value of a literal
            /// @pre getKind() == Int64
            /// @return integer value
            /// @throws SPLRuntimeSubscriptionInvalidLiteralException if the
            /// literal is of non-matching type
            int64 getInt64Value() const;

            /// Float value of a literal
            /// @pre getKind() == Float64
            /// @return float value
            /// @throws SPLRuntimeSubscriptionInvalidLiteralException if the
            /// literal is of non-matching type
            float64 getFloat64Value() const;

            /// String value of a literal
            /// @pre getKind() == RString
            /// @return string value
            /// @throws SPLRuntimeSubscriptionInvalidLiteralException if the
            /// literal is of non-matching type
            const rstring & getRStringValue() const;

            /// Compare two literals for equality
            /// @param rhs literal to compare to
            bool operator== (const Literal& rhs) const;

            /// Compare two literals for inequality
            /// @param rhs literal to compare to
            bool operator!= (const Literal& rhs) const
                { return !this->operator==(rhs); }

#ifndef DOXYGEN_SKIP_FOR_USERS
            friend std::ostream & operator<<(std::ostream &, const SubscriptionExpression::Literal&);

        private:
            std::ostream& print(std::ostream& s) const;

            Kind              _kind;
            union {
                int64         _int64;
                float64       _float64;
                rstring       *_rstring;
            };
#endif /*DOXYGEN_SKIP_FOR_USERS*/
        };

    public:
        /// Destructor
        ///
        ~SubscriptionExpression();

        /// Create a predicate clause ('symbol \<op\> literal' where \<op\> != In or 'literal In symbol')
        /// @param symbol symbol name
        /// @param op operation
        /// @param lit literal to compare to
        /// @return shared pointer to the predicate clause
        static SubscriptionExpressionPtr createPredicate (const std::string& symbol, Op op,
                                                          const Literal& lit);

        /// Create a predicate clause with a subscript on the symbol
        /// ('symbol[subscript] \<op\> literal' where \<op\> != In)
        /// @param symbol symbol name
        /// @param op operation
        /// @param lit literal to compare to
        /// @param subscript subscript value
        /// @return shared pointer to the predicate clause
        /// @throws SPLRuntimeSubscriptionNoSubscriptWithInException if the
        /// operator specified by \a op is equal to \c Op::In
        static SubscriptionExpressionPtr createPredicate (const std::string& symbol, Op op,
                                                          const Literal& lit, int64 subscript);

        /// Create a predicate clause ('symbol % int64Value \<op\> int64Value2' where \<op\> != In)
        /// @param symbol symbol name
        /// @param modDivisor divisor for modulus
        /// @param op operation
        /// @param lit value to compare to modulus
        /// @pre modDivisor != 0 && op != In
        /// @return shared pointer to the predicate clause
        /// @throws SPLRuntimeSubscriptionNoSubscriptWithInException if the
        /// operator specified by \a op is equal to \c Op::In
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if modDivisor == 0
        static SubscriptionExpressionPtr createModuloPredicate (const std::string& symbol,
                                                                int64 modDivisor, Op op,
                                                                const Literal& lit);

        /// Create a predicate clause with a subscript on the symbol
        /// Create a predicate clause ('symbol[subscript] % int64Value \<op\> int64Value2' where \<op\> != In)
        /// @param symbol symbol name
        /// @param modDivisor divisor for modulus
        /// @param op operation
        /// @param lit value to compare to modulus
        /// @param subscript subscript value
        /// @pre modDivisor != 0
        /// @return shared pointer to the predicate clause
        /// @throws SPLRuntimeSubscriptionNoSubscriptWithInException if the
        /// operator specified by \a op is equal to \c Op::In
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if modDivisor == 0
        static SubscriptionExpressionPtr createModuloPredicate (const std::string& symbol,
                                                                int64 modDivisor, Op op,
                                                                const Literal& lit, int64 subscript);

        /// Create an and-clause with one subclause
        /// @param subclause first subclause
        /// @return shared pointer to and-clause with one subclause
        static SubscriptionExpressionPtr createAndClause (SubscriptionExpressionPtr& subclause);

        /// Create an and-clause with two subclauses
        /// @param lhs first subclause
        /// @param rhs second subclause
        /// @return shared pointer to and-clause with two subclauses
        static SubscriptionExpressionPtr createAndClause (SubscriptionExpressionPtr & lhs,
                                                          SubscriptionExpressionPtr & rhs);

        /// Create an or-clause with one subclause
        /// @param subclause first subclause
        /// @return shared pointer to or-clause with one subclause
        static SubscriptionExpressionPtr createOrClause (SubscriptionExpressionPtr & subclause);

        /// Create an or-clause with two subclauses
        /// @param lhs first subclause
        /// @param rhs second subclause
        /// @return shared pointer to or-clause with two subclauses
        static SubscriptionExpressionPtr createOrClause (SubscriptionExpressionPtr & lhs,
                                                         SubscriptionExpressionPtr & rhs);

        /// Create a subscription expression from a string
        /// @param str string form of subscription expression
        /// @return pointer to created SubscriptionExpression or NULL if unable
        /// to form a valid expression
        static SubscriptionExpressionPtr fromString (const std::string& str);

        /// Get the string representation of the subscription expression
        /// @return string representation of the subscription expression
        std::string toString() const;

        /// Return the number of subclauses in an and/or-clause
        /// @pre getKind() != Predicate
        /// @return number of subclauses in the clause
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression represents a predicate clause
        uint32_t getSize () const;

        /// Return a subclause of an and/or-clause
        /// @pre getKind() != Predicate
        /// @pre index < getSize()
        /// @param index subclause index
        /// @return subclause at the specified index
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// number of subclauses are equal to or less than the index specified,
        /// or the subscription expression represents a predicate clause
        SubscriptionExpressionPtr getSubclause (uint32_t index);

        /// Return a subclause of an and/or-clause (const verstion)
        /// Note that this returns a const reference, not a shared pointer,
        /// which could cause problems if the original is removed while the
        /// returned value is being used.
        /// @pre getKind() != Predicate
        /// @pre index < getSize()
        /// @param index subclause index
        /// @return subclause at the specified index
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// number of subclauses are equal to or less than the index specified,
        /// or the subscription expression represents a predicate clause
        SubscriptionExpression const & getSubclause (uint32_t index) const;

        /// Append a subclause to an and/or-clause
        /// @pre getKind() != Predicate
        /// @param subclause clause to be appended
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression represents a predicate clause
        void addClause (SubscriptionExpressionPtr& subclause);

        /// Remove a subclause from an and/or clause
        /// @pre getKind() != Predicate
        /// @pre getSize() > 1
        /// @pre getSize() > index
        /// @param index clause to be removed
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression represents a predicate clause or if the
        /// index is invalid (greater than or equal to the number of clauses)
        void removeClause (uint32_t index);

        /// Get the kind of the subscription expression (and clause, or clause, or a predicate)
        /// @return kind of the subscription expression
        Kind getKind() const { return _kind;}

        /// Return the symbol in a predicate clause
        /// @pre getKind() == Predicate
        /// @return symbol
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression does not represent a predicate clause
        const std::string & getSymbol() const;

        /// Return the literal in a predicate
        /// @pre getKind() == Predicate
        /// @return literal
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression does not represent a predicate clause
        const Literal & getLiteral() const;

        /// Return the operator in an predicate clause
        /// @pre getKind() == Predicate
        /// @return operation
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression does not represent a predicate clause
        Op getOp() const;

        /// Return true if the symbol in a predicate clause has a subscript
        /// @pre getKind() == Predicate
        /// @return true if the item has a subscript
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression does not represent a predicate clause
        bool hasSubscript() const;

        /// Return subscript for the symbol in a predicate clause
        /// @pre getKind() == Predicate && hasSubscript()
        /// @return subscript value
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression does not represent a predicate clause or if
        /// it does not have a subscript
        int64 getSubscript() const;

        /// Return true if the symbol in a predicate clause has a modulo divisor
        /// @pre getKind() == Predicate
        /// @return true if the item has a modulo divisor
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression does not represent a predicate clause
        bool hasModuloDivisor() const;

        /// Return modulo divisor for the symbol in a predicate clause
        /// @pre getKind() == Predicate && hasModuloDivisor()
        /// @return modulo divisor value
        /// @throws SPLRuntimeSubscriptionInvalidClauseException if the
        /// subscription expression does not represent a predicate clause or if
        /// it does not have a modulo divisor
        int64 getModuloDivisor() const;

        /// Is this an empty SubscriptionExpression?
        /// @return true if the subscription expression represents an empty expression
        bool isEmpty() const;

        /// Compare two SubscriptionExpressions for equality (recursively)
        /// @param rhs SubscriptionExpression to compare to
        bool operator== (const SubscriptionExpression& rhs) const;

        /// Compare two SubscriptionExpressions for inequality (recursively)
        /// @param rhs SubscriptionExpression to compare to
        bool operator!= (const SubscriptionExpression& rhs) const
        {
            return !this->operator==(rhs);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        friend std::ostream & operator<<(std::ostream & ostr, const SubscriptionExpression& v);

    private:
        /// Constructor
        /// @param k clause kind
        SubscriptionExpression(Kind k);

        std::ostream& print(std::ostream& s) const;

        struct ItemInfo
        {
            Op _op;
            bool _hasSubscript;
            Literal _lit;
            std::string _symbol;
            int64 _subscript;
            int64 _moduloDivisor;
        };
        Kind _kind;
        union {
            std::vector<SubscriptionExpressionPtr> * _subClauses;
            ItemInfo * _item;
        };
#endif /*DOXYGEN_SKIP_FOR_USERS*/
    };

    // Simple printing functions
    std::ostream & operator<<(std::ostream & ostr, const SubscriptionExpression& v);
    std::ostream & operator<<(std::ostream & ostr, SubscriptionExpression::Kind v);
    std::ostream & operator<<(std::ostream & ostr, SubscriptionExpression::Op v);
    std::ostream & operator<<(std::ostream & ostr, const SubscriptionExpression::Literal& v);

};

#endif /* SUBSCRIPTION_EXPRESSION_H */
