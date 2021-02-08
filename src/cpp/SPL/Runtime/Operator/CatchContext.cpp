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

#include <SPL/Runtime/Common/RuntimeAnnotations.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/CatchContext.h>

using namespace SPL;

CatchContext::CatchContext(ADL::annotationType const& annot)
  : name_(CATCH_ANNOTATION)
{
    assert(annot.tag() == CATCH_ANNOTATION);

    tupleTrace_ = false;
    stackTrace_ = true;

    ADL::annotationType::tuple_type tuple = annot.tuple();
    ADL::annotationTupleValueType::attr_const_iterator attrIt;

    ADL::annotationTupleValueType::attr_sequence const& attributes = tuple.attr();

    for (attrIt = attributes.begin(); attrIt != attributes.end(); attrIt++) {
        ADL::annotationAttributeValueType const& attr = *attrIt;
        std::istringstream val(attr.value().get());
        if (attr.key() == CATCH_EXCEPTION) {
            std::string exceptionKind;
            val >> exceptionKind;
            if (exceptionKind == "streams") {
                kind_ = STREAMS;
            } else if (exceptionKind == "std") {
                kind_ = STD;
            } else if (exceptionKind == "all") {
                kind_ = ALL;
            } else if (exceptionKind == "none") {
                kind_ = NONE;
            }
        } else if (attr.key() == CATCH_TUPLE_TRACE) {
            val >> std::boolalpha >> tupleTrace_;
        } else if (attr.key() == CATCH_STACK_TRACE) {
            val >> std::boolalpha >> stackTrace_;
        } else {
            SPLAPPTRC(L_ERROR, "Error parsing AADL: " << attr.key(), SPL_OPER_DBG);
        }
    }
}

std::string const& CatchContext::getName() const
{
    return name_;
}

CatchExceptionKind CatchContext::getExceptionKind() const
{
    return kind_;
}

bool CatchContext::mustTraceTuples() const
{
    return tupleTrace_;
}

bool CatchContext::mustPrintStackTrace() const
{
    return stackTrace_;
}
