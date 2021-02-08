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
#include <SPL/Runtime/Operator/ThreadingModelContext.h>

using namespace SPL;

ThreadingModelContext::ThreadingModelContext(ADL::annotationType const& annot)
  : name_(THREADING_ANNOTATION)
  , threads_(1)
  , elastic_(true)
{
    assert(annot.tag() == THREADING_ANNOTATION);

    ADL::annotationType::tuple_type tuple = annot.tuple();
    ADL::annotationTupleValueType::attr_const_iterator attrIt;

    ADL::annotationTupleValueType::attr_sequence const& attributes = tuple.attr();

    for (attrIt = attributes.begin(); attrIt != attributes.end(); attrIt++) {
        ADL::annotationAttributeValueType const& attr = *attrIt;
        std::istringstream val(attr.value().get());
        if (attr.key() == THREADING_MODEL) {
            std::string modelKind;
            val >> modelKind;
            if (modelKind == "manual") {
                kind_ = MANUAL;
            } else if (modelKind == "automatic") {
                kind_ = AUTOMATIC;
            } else if (modelKind == "dynamic") {
                kind_ = DYNAMIC;
            } else if (modelKind == "dedicated") {
                kind_ = DEDICATED;
            } else {
                SPLAPPTRC(L_ERROR, "Error parsing AADL, unknown model kind: " << modelKind,
                          SPL_OPER_DBG);
            }
        } else if (attr.key() == THREADING_THREADS) {
            val >> threads_;
        } else if (attr.key() == THREADING_ELASTIC) {
            val >> std::boolalpha >> elastic_;
        } else {
            SPLAPPTRC(L_ERROR, "Error parsing AADL: " << attr.key(), SPL_OPER_DBG);
        }
    }
}

std::string const& ThreadingModelContext::getName() const
{
    return name_;
}

ThreadingModelKind ThreadingModelContext::getKind() const
{
    return kind_;
}

uint32_t ThreadingModelContext::getThreads() const
{
    return threads_;
}

bool ThreadingModelContext::getElastic() const
{
    return elastic_;
}
