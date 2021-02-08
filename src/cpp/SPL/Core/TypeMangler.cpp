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

#include "boost/lexical_cast.hpp"
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/TypeMangler.h>

using namespace SPL;

// Mangle a TupleTyp into a string for C++ or filesystem
std::string TypeMangler::mangle(const TupleTyp& tuple, uint64_t unique, Modifier modifier)
{
    std::string result = mangle(tuple, modifier);
    return compress(result, unique, true, true);
}

// Mangle a EnumTyp into a string for C++ or filesystem
std::string TypeMangler::mangle(const EnumTyp& enum_, uint64_t unique, Modifier modifier)
{
    std::string result = mangle(enum_, modifier);
    return compress(result, unique, true, true);
}

// Mangle a XMLTyp into a string for C++ or filesystem
std::string TypeMangler::mangle(const XMLTyp& xml, uint64_t unique, Modifier modifier)
{
    std::string result = mangle(xml, modifier);
    return compress(result, unique, true, true);
}

std::string TypeMangler::mangle(const TupleTyp& tuple, Modifier modifier)
{
    // To mangle a tuple, we do 't' ['f'|'s'] field_count { type <nameLen> name}*
    uint32_t numAttrs = tuple.getNumberOfAttributes();
    std::string result;
    switch (modifier) {
        case Regular:
            result = "t";
            break;
        case Facade:
            result = "tf";
            break;
    }
    result += boost::lexical_cast<std::string>(numAttrs);
    for (uint32_t i = 0; i < numAttrs; i++) {
        const RootTyp& b = tuple.getAttributeType(i);
        const std::string& fieldName = tuple.getAttributeName(i);
        result += mangle(b);
        result += boost::lexical_cast<std::string>(fieldName.size());
        result += fieldName;
    }
    return result;
}

std::string TypeMangler::mangle(const EnumTyp& e, Modifier modifier)
{
    // To mangle an enum tuple, we do 'y' ['s'] field_count '_' { <nameLen> name}*
    const std::vector<std::string>& v = e.getValidValues();
    uint32_t numFields = v.size();
    std::string result = "y";
    result += boost::lexical_cast<std::string>(numFields) + '_';
    for (uint32_t i = 0; i < numFields; i++) {
        const std::string& fname = v[i];
        result += boost::lexical_cast<std::string>(fname.size());
        result += fname;
    }
    return result;
}

std::string TypeMangler::mangle(const XMLTyp& x, Modifier modifier)
{
    // To mangle an XML tuple, we do 'x' ['s'] {'_' <schemaNameLen> schemaName '_' toolkitIndex}?
    std::string result = "x";
    const std::string& schemaName = x.schema();
    if (!schemaName.empty()) {
        result += '_';
        result += boost::lexical_cast<std::string>(schemaName.size());
        result += schemaName;
        result += '_';
        result += boost::lexical_cast<std::string>(x.toolkitIndex());
    }
    return result;
}

std::string TypeMangler::mangle(const RootTyp& type)
{
    MetaType::value mt = type.getMetaType();
    switch (mt) {
        case MetaType::VOID:
            return "v";
        case MetaType::BOOLEAN:
            return "b";
        case MetaType::ENUM:
            // more
            return mangle((EnumTyp&)type, Regular);
        case MetaType::INT8:
            return "j";
        case MetaType::INT16:
            return "k";
        case MetaType::INT32:
            return "i";
        case MetaType::INT64:
            return "I";
        case MetaType::UINT8:
            return "J";
        case MetaType::UINT16:
            return "K";
        case MetaType::UINT32:
            return "u";
        case MetaType::UINT64:
            return "U";
        case MetaType::FLOAT32:
            return "f";
        case MetaType::FLOAT64:
            return "F";
        case MetaType::DECIMAL32:
            return "d";
        case MetaType::DECIMAL64:
            return "D";
        case MetaType::DECIMAL128:
            return "X";
        case MetaType::COMPLEX32:
            return "c";
        case MetaType::COMPLEX64:
            return "C";
        case MetaType::TIMESTAMP:
            return "T";
        case MetaType::RSTRING:
            return "s";
        case MetaType::BSTRING:
            return "S" + boost::lexical_cast<std::string>(((BStringTyp&)type).getBoundSize());
        case MetaType::USTRING:
            return "r";
        case MetaType::BLOB:
            return "B";
        case MetaType::XML:
            return mangle(type.as<XMLTyp>(), Regular);
        case MetaType::LIST:
            return "l" + mangle(((ListTyp&)type).getElementType());
        case MetaType::BLIST:
            return "L" + boost::lexical_cast<std::string>(((BListTyp&)type).getBoundSize()) +
                   mangle(((BListTyp&)type).getElementType());
        case MetaType::SET:
            return "e" + mangle(((SetTyp&)type).getElementType());
        case MetaType::BSET:
            return "E" + boost::lexical_cast<std::string>(((BSetTyp&)type).getBoundSize()) +
                   mangle(((BSetTyp&)type).getElementType());
        case MetaType::MAP:
            return "m" + mangle(((MapTyp&)type).getKeyType()) +
                   mangle(((MapTyp&)type).getValueType());
        case MetaType::BMAP:
            return "M" + boost::lexical_cast<std::string>(((BMapTyp&)type).getBoundSize()) +
                   mangle(((BMapTyp&)type).getKeyType()) + mangle(((BMapTyp&)type).getValueType());
        case MetaType::TUPLE:
            return mangle((TupleTyp&)type, Regular);
        case MetaType::OPTIONAL:
            return "o" + mangle(type.as<OptionalTyp>().getUnderlyingType());
        default:
            THROW(SPLCompilerInvalidArgument, "Unknown meta type: " << mt);
    }
}
