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

#include <SPL/Debugger/ValueDefinition.h>

#include <SPL/Debugger/ApplicationModel.h>
#include <SPL/Debugger/ProbePointServices.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPortImpl.h>

#include <string>
#include <vector>

UTILS_NAMESPACE_USE
using namespace std::tr1;
using namespace SPL;

// @return a string value of the field definition as "name : type"
const std::string ValueDefinition::toString() const
{
    std::stringstream result;
    result << _name << " : " << _type->getName();
    return result.str();
}

// @return a string value of the port definition as "opName i|o portIndex"
const std::string ValueDefinition::portString() const
{
    std::stringstream result;
    result << _opName << ' ' << (_portType == PEImpl::INPUT ? " i " : " o ") << _portIndex;
    return result.str();
}

uint32_t ValueDefinition::find(ValueDefinitionVector& result,
                               ProbePointServices& services,
                               const std::string& matchName,
                               const std::string& matchType,
                               const std::string& matchOpName,
                               const std::string& matchPortType,
                               const std::string& matchPortIndex)
{
    SPCDBG(L_INFO,
           "Finding fields, name matching '" << matchName << "', type matching '" << matchType
                                             << "', for operator name matching '" << matchOpName
                                             << "', port type matching '" << matchPortType
                                             << "' port index matching '" << matchPortIndex << "'",
           SPL_APP_DBG);
    const ApplicationModel& model = services.getApplicationModel();
    unordered_map<std::string, OperatorImpl*> operators;
    model.getOperators(operators);
    unordered_map<std::string, OperatorImpl*>::const_iterator iter_p;

    try {
        if (useRegex(matchName)) {
            // Checking regex compilation so we don't take multiple exceptions in the loop below
            SPL::Functions::String::regexMatch(SPL::rstring(), matchName);
        }
    } catch (SPLRuntimeInvalidArgumentException& e) {
        SPCDBG(L_ERROR, "Field name '" << matchName << "' is not a valid regular expression.",
               SPL_APP_DBG);
        return false;
    }
    try {
        if (useRegex(matchType)) {
            // Checking regex compilation so we don't take multiple exceptions in the loop below
            SPL::Functions::String::regexMatch(SPL::rstring(), matchType);
        }
    } catch (SPLRuntimeInvalidArgumentException& e) {
        SPCDBG(L_ERROR, "Field type '" << matchType << "' is not a valid regular expression.",
               SPL_APP_DBG);
        return false;
    }

    for (iter_p = operators.begin(); iter_p != operators.end(); iter_p++) {
        OperatorImpl* opProp = iter_p->second;
        const OperatorContext& context = opProp->getContext();
        SPCDBG(L_INFO, "  Processing operator " << context.getName(), SPL_APP_DBG);
        if (matchOpName.compare("*") == 0 || matchOpName.compare(context.getName()) == 0) {
            for (int i = 0, n = opProp->getNumberOfInputPorts(); i < n; i++) {
                OperatorInputPort& input = opProp->getInputPortAt(i);
                // Find value definitions that match query parms
                matchValueDefinition(result, matchName, matchType, context.getName(), PEImpl::INPUT,
                                     i, input.getTupleType());
            }
            for (int i = 0, n = opProp->getNumberOfOutputPorts(); i < n; i++) {
                OperatorOutputPort& output = opProp->getOutputPortAt(i);
                // Find value definitions that match query parms
                matchValueDefinition(result, matchName, matchType, context.getName(),
                                     PEImpl::OUTPUT, i, output.getTupleType());
            }
            SPCDBG(L_INFO, "    Result size: " << result.size(), SPL_APP_DBG);
        }
    }

    return result.size();
}

void ValueDefinition::matchValueDefinition(ValueDefinitionVector& result,
                                           const std::string& matchName,
                                           const std::string& matchType,
                                           const std::string& opName,
                                           PEImpl::PortType portType,
                                           uint32_t portIndex,
                                           const Meta::BaseType& baseType)
{
    SPCDBG(L_INFO,
           "    Searching type for name matching '" << matchName << "', type matching '"
                                                    << matchType << "'",
           SPL_APP_DBG);
    const Meta::TupleType& tupleType = baseType.as<Meta::TupleType>();
    for (size_t i = 0, iu = tupleType.getNumberOfAttributes(); i < iu; ++i) {
        const std::string& attrName = tupleType.getAttributeName(i);
        if (isMatchingString(attrName, matchName)) {
            Meta::BaseType const& attrbType = tupleType.getAttributeType(i);
            const std::string& typeName = attrbType.getName();
            if (isMatchingString(typeName, matchType)) {
                SPCDBG(L_INFO,
                       "      Match found for field '" << attrName << "', type '" << typeName
                                                       << "'",
                       SPL_APP_DBG);
                ValueDefinitionPtr v = ValueDefinitionPtr(
                  new ValueDefinition(attrName, &attrbType, opName, portType, portIndex));
                result.push_back(v);
                SPCDBG(L_INFO, "      Result size: " << result.size(), SPL_APP_DBG);
            } else {
                SPCDBG(L_INFO,
                       "      Field does not match type requested: '" << attrName << "', type '"
                                                                      << typeName << "'",
                       SPL_APP_DBG);
            }
        } else {
            SPCDBG(L_INFO, "      Field does not match name requested: '" << attrName << "'",
                   SPL_APP_DBG);
        }
    }
}

bool ValueDefinition::useRegex(const std::string& compareVal)
{
    return compareVal.compare("*") != 0;
}

bool ValueDefinition::isMatchingString(const std::string& stringVal, const std::string& compareVal)
{
    if (compareVal.compare("*") == 0) {
        return true;
    }
    if (useRegex(compareVal)) {
        try {
            SPL::list<SPL::rstring> matchList =
              SPL::Functions::String::regexMatch(SPL::rstring(stringVal), compareVal);
            return (matchList.size() > 0);
        } catch (SPLRuntimeInvalidArgumentException& e) {
            return false;
        }
    }

    return stringVal.compare(compareVal) == 0;
}
