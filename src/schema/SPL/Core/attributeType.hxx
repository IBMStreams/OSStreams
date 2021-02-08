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

#ifndef ANNOTATION_MODEL_ATTRIBUTE_TYPE_H
#define ANNOTATION_MODEL_ATTRIBUTE_TYPE_H
#include <SPL/Core/annotationModel.h> // xsd-generated
namespace xmlns { namespace prod { namespace streams { namespace spl { namespace annotation {

class attributeType : public attributeType_base {
    public:
        attributeType() : attributeType_base() {}
        attributeType(const name_type& n, const type_type& t) : attributeType_base(n, t) {}

        attributeType (const ::xercesc::DOMElement& e, ::xml_schema::flags f = 0, ::xml_schema::container* c = 0) : attributeType_base (e, f, c) {}

        attributeType (const attributeType& x, ::xml_schema::flags f = 0, ::xml_schema::container* c = 0) : attributeType_base (x, f, c), _value(x._value) {}

        virtual attributeType* _clone (::xml_schema::flags f = 0, ::xml_schema::container* c = 0) const;
        void setValue (const std::string& v) { _value = v; }
        const std::string& getValue() const { return _value;}

    private:
        std::string     _value;
};

void operator<<(xercesc::DOMElement& e, const attributeType& lit);

}}}}}
#endif // OPERATOR_INSTANCE_MODEL_LITERAL_CUSTOM_H
