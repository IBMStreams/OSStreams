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

#ifndef SPL_TYPE_MANGLER_H
#define SPL_TYPE_MANGLER_H

#include <SPL/Core/BaseMangler.h>
#include <SPL/FrontEnd/Typ.h>

namespace SPL {
//! Mangle Types to no more than MaxMangledNameSize characters
class TypeMangler : public BaseMangler
{
  public:
    /// Possible type modifiers
    enum Modifier
    {
        Regular, //! Normal, unmodifed type
        Facade   //! Facade Tuple
    };

    /// Compute the signature of a Tuple
    /// @param tuple Tuple Type
    /// @param modifier Tuple modifier
    /// @return signature
    static std::string getTupleSignature(const TupleTyp& tuple, Modifier modifier)
    {
        return mangle(tuple, modifier);
    }

    /// Compute the signature of an enum type
    /// @param enum_ Enum Type
    /// @param modifier Enum modifier
    /// @return signature
    static std::string getEnumSignature(const EnumTyp& enum_, Modifier modifier)
    {
        return mangle(enum_, modifier);
    }

    /// Compute the signature of an xml type
    /// @param xml XML type
    /// @param modifier XML modifier
    /// @return signature
    static std::string getXMLSignature(const XMLTyp& xml, Modifier modifier)
    {
        return mangle(xml, modifier);
    }

    /// Mangle a TupleTyp into a string for C++ or filesystem
    /// This is the signature, if < MaxMangledNameSize
    /// else the signature will be forced to be in that size
    /// @param tuple Tuple to be mangled
    /// @param unique Unique index value, if needed
    /// @param modifier Tuple modifier
    /// @return mangled name
    static std::string mangle(const TupleTyp& tuple, uint64_t unique, Modifier modifier);

    /// Mangle an EnumTyp into a string for C++ or filesystem
    /// This is the signature, if < MaxMangledNameSize
    /// else the signature will be forced to be in that size
    /// @param enum_ Enum to be mangled
    /// @param unique Unique index value, if needed
    /// @param modifier Enum modifier
    /// @return mangled name
    static std::string mangle(const EnumTyp& enum_, uint64_t unique, Modifier modifier);

    /// Mangle a XMLTyp into a string for C++ or filesystem
    /// This is the signature, if < MaxMangledNameSize
    /// else the signature will be forced to be in that size
    /// @param xml XML to be mangled
    /// @param unique Unique index value, if needed
    /// @param modifier XML modifier
    /// @return mangled name
    static std::string mangle(const XMLTyp& xml, uint64_t unique, Modifier modifier);

  private:
    static std::string mangle(const TupleTyp& tuple, Modifier modifier);
    static std::string mangle(const EnumTyp& e, Modifier modifier);
    static std::string mangle(const XMLTyp& x, Modifier modifier);
    static std::string mangle(const RootTyp& type);
    static void uniqifyIfLong(std::string& result, uint64_t unique);
};
};

#endif /* SPL_TYPE_MANGLER_H */
