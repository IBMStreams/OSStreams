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

#include <SPL/CodeGen/Param.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/SysOut.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

Param::~Param()
{
    for (uint32_t i = 0; i < _expnList->size(); i++) {
        delete (*_expnList)[i];
    }
    delete _expnList;
}

std::ostream& Param::print(std::ostream& s) const
{
    s << _id << " : ";
    for (uint32_t i = 0; i < _expnList->size(); i++) {
        if (i > 0) {
            s << ", ";
        }
        const Expression* e = (*_expnList)[i];
        e->print(s, *e);
    }
    s << ";";
    return s;
}

void Param::collectTypeAndFunctions(unordered_set<const RootTyp*>* types,
                                    unordered_set<const FunctionHeadSym*>* fcns,
                                    bool inFcns) const
{
    for (uint32_t i = 0; i < _expnList->size(); i++) {
        (*_expnList)[i]->collectTypeAndFunctions(types, fcns, inFcns);
    }
}

const Expression* Param::getSingleExpression(const std::string& paramName) const
{
    const std::vector<const Expression*>& expnList = getExpressionList();

    if (expnList.size() != 1) {
        return NULL;
    }

    return expnList[0];
}

bool Param::getSingleString(const std::string& paramName, std::string& singleString) const

{
    const Expression* e = getSingleExpression(paramName);

    if (!e) {
        return false;
    }

    if (e->getKind() != Expression::Lit || e->getType().getMetaType() != MetaType::RSTRING) {
        // let's try simplifying the expression, to see if THAT is a literal
        ExpressionEvaluator ee(CompilerConfiguration::instance());
        auto_ptr<Expression> se(Expression::simplify(&e->clone(), ee));
        if (se->getKind() == Expression::Lit && se->getType().getMetaType() == MetaType::RSTRING) {
            // We need to keep the string around
            const string& str = se->getLiteral()->primitive().get_rstring();
            _simplifiedStrings.insert(str);
            unordered_set<string>::const_iterator it = _simplifiedStrings.find(str);
            assert(it != _simplifiedStrings.end());
            singleString = *it;
            return true;
        }

        // simplified version wasn't a string either
        return false;
    }

    singleString = e->getLiteral()->primitive().get_rstring();
    return true;
}
