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

#include "attributeType.hxx"
namespace xmlns { namespace prod { namespace streams { namespace spl { namespace annotation {

using namespace xercesc;

attributeType* attributeType::_clone(::xml_schema::flags f, ::xml_schema::container* c) const
{
    return new attributeType (*this, f, c);
}

void operator<<(DOMElement& e, const attributeType& attr)
{
    e << static_cast<const attributeType_base&>(attr);
    const std::string& value = attr.getValue();
    if (!value.empty()) {
        DOMDocument* doc  = e.getOwnerDocument ();
        XMLCh *s = XMLString::transcode(value.c_str());
        DOMText *t = doc->createTextNode(s);
        e.appendChild (t);
        XMLString::release (&s);
    }
}

}}}}}
