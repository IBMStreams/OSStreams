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
#include "literalCustom.hxx"
namespace xmlns { namespace prod { namespace streams { namespace spl { namespace operator_ {
      namespace instance {

using namespace xercesc;

literalType* literalType::_clone(::xml_schema::flags f, ::xml_schema::container* c) const
{
    return new literalType (*this, f, c);
}

void operator<<(DOMElement& e, const literalType& lit)
{
    e << static_cast<const literalType_base&>(lit);
    const std::string& value = lit.getValue();
    if (!value.empty()) {
        DOMDocument* doc  = e.getOwnerDocument ();
        XMLCh *s = XMLString::transcode(value.c_str());
        DOMText *t = doc->createTextNode(s);
        e.appendChild (t);
        XMLString::release (&s);
    }
}

}}}}}}
