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

#include <SPL/Runtime/Type/Meta/Type.h>

#include <cassert>

using namespace SPL::Meta;

std::string Type::toString() const
{
    switch (value_) {
        case INVALID:
            return "<invalid>";
        case BOOLEAN:
            return "boolean";
        case ENUM:
            return "enum";
        case INT8:
            return "int8";
        case INT16:
            return "int16";
        case INT32:
            return "int32";
        case INT64:
            return "int64";
        case UINT8:
            return "uint8";
        case UINT16:
            return "uint16";
        case UINT32:
            return "uint32";
        case UINT64:
            return "uint64";
        case FLOAT32:
            return "float32";
        case FLOAT64:
            return "float64";
        case DECIMAL32:
            return "decimal32";
        case DECIMAL64:
            return "decimal64";
        case DECIMAL128:
            return "decimal128";
        case COMPLEX32:
            return "complex32";
        case COMPLEX64:
            return "complex64";
        case TIMESTAMP:
            return "timestamp";
        case RSTRING:
            return "rstring";
        case USTRING:
            return "ustring";
        case BSTRING:
            return "bstring";
        case BLOB:
            return "blob";
        case LIST:
            return "list";
        case BLIST:
            return "blist";
        case SET:
            return "set";
        case BSET:
            return "bset";
        case MAP:
            return "map";
        case BMAP:
            return "bmap";
        case TUPLE:
            return "tuple";
        case XML:
            return "xml";
        case OPTIONAL:
            return "optional";
        default:
            assert(!"cannot happen");
    }
    return "";
}
