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

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Core/Collector.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

void Collector::collectType(unordered_set<const RootTyp*>& result, const RootTyp& b)
{
    switch (b.getMetaType()) {
        case MetaType::LIST:
            collectType(result, static_cast<const ListTyp&>(b).getElementType());
            break;
        case MetaType::BLIST:
            collectType(result, static_cast<const BListTyp&>(b).getElementType());
            break;
        case MetaType::MAP:
            collectType(result, static_cast<const MapTyp&>(b).getKeyType());
            collectType(result, static_cast<const MapTyp&>(b).getValueType());
            break;
        case MetaType::BMAP:
            collectType(result, static_cast<const BMapTyp&>(b).getKeyType());
            collectType(result, static_cast<const BMapTyp&>(b).getValueType());
            break;
        case MetaType::SET:
            collectType(result, static_cast<const SetTyp&>(b).getElementType());
            break;
        case MetaType::BSET:
            collectType(result, static_cast<const BSetTyp&>(b).getElementType());
            break;
        case MetaType::OPTIONAL:
            collectType(result, static_cast<const OptionalTyp&>(b).getUnderlyingType());
            break;
        case MetaType::TUPLE: {
            const TupleTyp& tt = static_cast<const TupleTyp&>(b);
            for (uint32_t i = 0, n = tt.getNumberOfAttributes(); i < n; i++)
                collectType(result, tt.getAttributeType(i));
        }
        case MetaType::XML:
        case MetaType::ENUM:
            // This is as far down as we need to go.  Add the RootTyp
            result.insert(&b);
            break;

        default:
            break;
    }
}

void Collector::collectTypes(unordered_set<const RootTyp*>& result, const FunctionHeadSym& fcn)
{
    collectType(result, fcn.returnType());
    const FunctionHeadSym::Formals& formals = fcn.formals();
    for (uint32_t i = 0; i < formals.size(); i++)
        collectType(result, formals[i]->type());
}

void Collector::collectTypesAndFunctions(unordered_set<const RootTyp*>* types,
                                         unordered_set<const FunctionHeadSym*>* fcns,
                                         const Expression& expn,
                                         bool inFcns)
{
    expn.collectTypeAndFunctions(types, fcns, inFcns);
}

void Collector::collectTypesAndFunctions(unordered_set<const RootTyp*>* types,
                                         unordered_set<const FunctionHeadSym*>* fcns,
                                         const Statement& stmt,
                                         bool inFcns)
{
    stmt.collectTypeAndFunctions(types, fcns, inFcns);
}
