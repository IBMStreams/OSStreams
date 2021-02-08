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

#ifndef SPL_RUNTIME_OPERATOR_STREAM_PROPERTY_H
#define SPL_RUNTIME_OPERATOR_STREAM_PROPERTY_H

/*!
 * \file StreamProperty.h \brief Definition of the SPL::StreamProperty class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/SPLType.h>

#define STREAM_PROPERTY_READ(type, mtype)                                            \
    operator type const &() const {                                                  \
        if(type_->getMetaType()!=Meta::Type::mtype) {                                \
            SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_STREAM_PROPERTY_TYPE_MISMATCH(type_->getName(), #type), SPL_OPER_DBG);                                     \
        }                                                                            \
        return static_cast<type const &>(value_);                                    \
    }                                                                                \

#define STREAM_PROPERTY_WRITE(type, mtype)                                           \
    operator type &() {                                                              \
        if(type_->getMetaType()!=Meta::Type::mtype) {                                \
            SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_STREAM_PROPERTY_TYPE_MISMATCH(type_->getName(), #type), SPL_OPER_DBG);                                     \
        }                                                                            \
        return static_cast<type &>(value_);                                          \
    }                                                                                \

#define STREAM_PROPERTY_LIST_READ(type, mtype)                                   \
    operator list<type > const & () const {                                      \
        if(type_->getMetaType()!=Meta::Type::LIST || type_->as<Meta::ListType>() \
           .getElementType().getMetaType()!=Meta::Type::mtype) {                 \
            SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_STREAM_PROPERTY_TYPE_MISMATCH(type_->getName(), "list<" #type ">"), SPL_OPER_DBG);                                 \
        }                                                                        \
        List const & llist = value_;                                             \
        return static_cast<list<type> const&>(llist);                            \
    }                                                                            \

#define STREAM_PROPERTY_LIST_WRITE(type, mtype)                                  \
    operator list<type > & () {                                                  \
        if(type_->getMetaType()!=Meta::Type::LIST || type_->as<Meta::ListType>() \
           .getElementType().getMetaType()!=Meta::Type::mtype) {                 \
            SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_STREAM_PROPERTY_TYPE_MISMATCH(type_->getName(), "list<" #type ">"), SPL_OPER_DBG);                                 \
        }                                                                        \
        List & llist = value_;                                                   \
        return static_cast<list<type> &>(llist);                                 \
    }

namespace SPL
{
    /*! \brief Class that represents a stream property.

    Example use:
    \code
      int64 myInt = ...
      StreamProperty intProp(myInt); // stores a copy of myInt
      int64 & iVal = intProp;
      iVal = 10; // modified intProp as well

      list<int64> myIntList = ...
      StreamProperty intListProp(myIntList); // stores a copy of myIntList
      list<int64> & ilVal = intListProp;
      ilVal.push_back(10); // modified intListProp as well

      try {
         int64 & iVal = intListProp; // will throw exception
      } catch(SPLRuntimeTypeMismatchException const & e) {}

      StreamProperty prop("test"); // of type rstring
      assert(prop.getType().getName() == "rstring");
      prop = intListProp; // ok to change the type via assignment
      assert(prop.getType().getName() == "list<unt64>");
    \endcode
    */
    class StreamProperty
    {
    public:
        /// Default constructor
        ///
        StreamProperty()
            : name_("unknown"), value_(*new int64(0)),
              type_(&Meta::BaseType::makeType(value_)) {}

        /// Constructor
        /// @param name property name
        /// @param val property value
        StreamProperty(const std::string & name, int64 const & val)
            : name_(name), value_(*new int64(val)),
              type_(&Meta::BaseType::makeType(val)) {}

        /// Constructor
        /// @param name property name
        /// @param val property value
        StreamProperty(const std::string & name, float64 const & val)
            : name_(name), value_(*new float64(val)),
              type_(&Meta::BaseType::makeType(val)) {}

        /// Constructor
        /// @param name property name
        /// @param val property value
        StreamProperty(const std::string & name, rstring const & val)
            : name_(name), value_(*new rstring(val)),
              type_(&Meta::BaseType::makeType(val)) {}

        /// Constructor
        /// @param name property name
        /// @param val property value
        StreamProperty(const std::string & name, list<int64> const & val)
            : name_(name), value_(*new list<int64>(val)),
              type_(&Meta::BaseType::makeType(val)) {}

        /// Constructor
        /// @param name property name
        /// @param val property value
        StreamProperty(const std::string & name, list<float64> const & val)
            : name_(name), value_(*new list<float64>(val)),
              type_(&Meta::BaseType::makeType(val)) {}

        /// Constructor
        /// @param name property name
        /// @param val property value
        StreamProperty(const std::string & name, list<rstring> const & val)
            : name_(name), value_(*new list<rstring>(val)),
              type_(&Meta::BaseType::makeType(val)) {}

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Constructor
        /// @param name property name
        /// @param val property value (memory will be owned by the StreamProperty)
        StreamProperty(const std::string & name, ValueHandle & val)
            : name_(name),
              value_(val.getValuePointer(), val.getMetaType(), &val.getTypeId()),
              type_(&Meta::BaseType::makeType((ConstValueHandle) val)) {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Copy constructor
        /// @param sp stream property to copy from
        StreamProperty(StreamProperty const & sp)
            : name_(sp.name_), value_(sp.value_.newValue()),
              type_(sp.type_)
        {
            value_.assignFrom(ConstValueHandle(sp.value_));
        }

        /// Destructor
        ~StreamProperty()
        {
            value_.deleteValue();
        }

        /// Assignment operator
        /// @param sp stream property to assign from
        StreamProperty & operator =(StreamProperty const & sp)
        {
            name_ = sp.name_;
            if(type_!=sp.type_) {
                value_.deleteValue();
                value_ = sp.value_.newValue();
            }
            value_.assignFrom(ConstValueHandle(sp.value_));
            type_ = sp.type_;
            return *this;
        }

        /// Get the property name
        /// @return property name
        const std::string getName() const
        {
            return name_;
        }

        /// Set the property name
        /// @param name name of the property
        void setName(const std::string & name)
        {
            name_ = name;
        }

        /// Get the property type
        /// @return property type
        const Meta::BaseType & getType() const
        {
            return *type_;
        }

        /// Get the value in string form
        /// @return value in string form
        std::string getValueString() const
        {
            return value_.toString();
        }

        /// Get the value handle
        /// @return value handle
        ValueHandle getValueHandle()
        {
            return value_;
        }

        /// Get the value handle
        /// @return value handle
        ConstValueHandle getConstValueHandle() const
        {
            return ConstValueHandle(value_);
        }

        /// Compare a StreamProperty to another for equality
        /// @param rhs StreamProperty to compare to
        bool operator== (const StreamProperty& rhs) const
        {
            return getConstValueHandle().equals(rhs.getConstValueHandle());
        }

        /// Compare a StreamProperty to another for inequality
        /// @param rhs StreamProperty to compare to
        bool operator!= (const StreamProperty& rhs) const
        {
            return !getConstValueHandle().equals(rhs.getConstValueHandle());
        }

        /// Get the int64 value (read only)
        /// @return int64 value as a constant reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not int64
        STREAM_PROPERTY_READ(int64,   INT64);

        /// Get the float64 value (read only)
        /// @return float64 value as a constant reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not float64
        STREAM_PROPERTY_READ(float64, FLOAT64);

        /// Get the rstring value (read only)
        /// @return rstring value as a constant reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not rstring
        STREAM_PROPERTY_READ(rstring, RSTRING);

        /// Get the list<int64> value (read only)
        /// @return list<int64> value as a constant reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not list<int64>
        STREAM_PROPERTY_LIST_READ(int64,   INT64);

        /// Get the list<float64> value (read only)
        /// @return list<float64> value as a constant reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not list<float64>
        STREAM_PROPERTY_LIST_READ(float64, FLOAT64);

        /// Get the list<rstring> value (read only)
        /// @return list<rstring> value as a constant reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not list<rstring>
        STREAM_PROPERTY_LIST_READ(rstring, RSTRING);

        /// Get the int64 value
        /// @return int64 value as a reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not int64
        STREAM_PROPERTY_WRITE(int64,   INT64);

        /// Get the float64 value
        /// @return float64 value as a reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not float64
        STREAM_PROPERTY_WRITE(float64, FLOAT64);

        /// Get the rstring value
        /// @return rstring value as a reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not rstring
        STREAM_PROPERTY_WRITE(rstring, RSTRING);

        /// Get the list<int64> value
        /// @return list<int64> value as a reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not list<int64>
        STREAM_PROPERTY_LIST_WRITE(int64,   INT64);

        /// Get the list<float64> value
        /// @return list<float64> value as a reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not list<float64>
        STREAM_PROPERTY_LIST_WRITE(float64, FLOAT64);

        /// Get the list<rstring> value
        /// @return list<rstring> value as a reference
        /// @throws SPLRuntimeTypeMismatchException if the property type is not list<rstring>
        STREAM_PROPERTY_LIST_WRITE(rstring, RSTRING);

    private:
        std::string name_;
        ValueHandle value_;
        Meta::BaseType const * type_;
    };

    inline std::ostream & operator<<(std::ostream & ostr, StreamProperty const & sp)
    {
        return ostr << "<" << sp.getName() << "," << sp.getValueString() << ">";
    }
};



#endif /* SPL_RUNTIME_OPERATOR_STREAM_PROPERTY_H */
