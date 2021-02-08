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

#ifndef SPL_FRONT_END_META_TYPE_H
#define SPL_FRONT_END_META_TYPE_H

#include <ostream>
#include <string>

namespace SPL {
/// Class represening top level spl meta types
class MetaType
{
  public:
    enum value // do not change the order (match Meta::Type)
    {
        INVALID = -1,
        BOOLEAN //! boolean in SPL
        ,
        ENUM //! enum{...} in SPL
        ,
        INT8 //! int8 in SPL
        ,
        INT16 //! int16 in SPL
        ,
        INT32 //! int32 in SPL
        ,
        INT64 //! int64 in SPL
        ,
        UINT8 //! uint8 in SPL
        ,
        UINT16 //! uint16 in SPL
        ,
        UINT32 //! uint32 in SPL
        ,
        UINT64 //! uint64 in SPL
        ,
        FLOAT32 //! float32 in SPL
        ,
        FLOAT64 //! float64 in SPL
        ,
        DECIMAL32 //! decimal32 in SPL
        ,
        DECIMAL64 //! decimal64 in SPL
        ,
        DECIMAL128 //! decimal128 in SPL
        ,
        COMPLEX32 //! complex32 in SPL
        ,
        COMPLEX64 //! complex64 in SPL
        ,
        TIMESTAMP //! timestamp in SPL
        ,
        RSTRING //! rstring in SPL
        ,
        BSTRING //! rstring[N] in SPL
        ,
        USTRING //! ustring in SPL
        ,
        BLOB //! blob in SPL
        ,
        LIST //! list<T> in SPL
        ,
        BLIST //! list<T>[N] in SPL
        ,
        SET //! set<T> in SPL
        ,
        BSET //! set<T>[N] in SPL
        ,
        MAP //! map<K,V> in SPL
        ,
        BMAP //! map<K,V>[N] in SPL
        ,
        TUPLE //! tuple<...> in SPL
        ,
        XML //! xml[<...>] in SPL
        ,
        UNKNOWN,
        EMPTYCURLY,
        TYPEFORMAL,
        BOUNDSFORMAL,
        VOID,
        OPTIONAL,
        NUL
    };

    /// Constructor
    ///
    MetaType() {}

    /// Constructor
    /// @param v enumeration value
    MetaType(value v)
      : value_(v)
    {}

    /// Get the enumeration value
    /// @return enumeration value
    MetaType::value getValue() const { return value_; }

    /// Get the string representation
    /// @return string representation
    std::string toString() const;

    /// '=' operator overload for assigning from an enumeration value
    /// @param v enumeration value to assign from
    /// @return a reference to this ibject
    MetaType& operator=(value v)
    {
        value_ = v;
        return *this;
    }

    /// Cast operator overload for casting to an enumeration value
    /// @return enumeration value
    operator MetaType::value() const { return value_; }

  private:
    value value_;
};

inline std::ostream& operator<<(std::ostream& ostr, const MetaType& type)
{
    ostr << type.toString();
    return ostr;
}

inline std::ostream& operator<<(std::ostream& ostr, const MetaType::value& type)
{
    ostr << MetaType(type);
    return ostr;
}
};

#endif /* SPL_FRONT_END_META_TYPE_H */
