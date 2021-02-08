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

/*
 * Override the literalType in operatorInstanceModel to get text nodes properly
 */
#ifndef OPERATOR_INSTANCE_MODEL_LITERAL_CUSTOM_H
#define OPERATOR_INSTANCE_MODEL_LITERAL_CUSTOM_H
#include <SPL/Core/operatorInstanceModel.h> // xsd-generated
namespace xmlns { namespace prod { namespace streams { namespace spl { namespace operator_ {
      namespace instance {

class literalType : public literalType_base {
    public:
        literalType() : literalType_base() {}
        literalType(const type_type& t) : literalType_base(t) {}

        literalType (const ::xercesc::DOMElement& e,
                    ::xml_schema::flags f = 0,
                    ::xml_schema::container* c = 0) : literalType_base (e, f, c) {}

        literalType (const literalType& x,
                    ::xml_schema::flags f = 0,
                    ::xml_schema::container* c = 0)
            : literalType_base (x, f, c), _value(x._value) {}

        virtual literalType* _clone (::xml_schema::flags f = 0,
                                     ::xml_schema::container* c = 0) const;
        void setValue (const std::string& v) { _value = v; }
        const std::string& getValue() const { return _value;}

    private:
        std::string     _value;
};

void operator<<(xercesc::DOMElement& e, const literalType& lit);

}}}}}}
#endif // OPERATOR_INSTANCE_MODEL_LITERAL_CUSTOM_H
