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

#include <SPL/Runtime/Operator/Port/StreamPropertyXML.h>
#include <SPL/Runtime/Type/Meta/Type.h>

#include <SAM/SAMInterfaceTypes.h>

using namespace std;
using namespace SPL;
using namespace xmlns::prod::streams::application;

auto_ptr<streamPropertyType> SPL::toXML(const StreamProperty& prop)
{
    auto_ptr<streamPropertyType> xml(new streamPropertyType());
    xml->name(prop.getName());
    xml->type(prop.getType().getName());
    ConstValueHandle vh = prop.getConstValueHandle();
    if (prop.getType().getMetaType() != Meta::Type::LIST) {
        xml->value().push_back(vh.toString());
    } else {
        List const& l = vh;
        for (ConstListIterator it = l.getBeginIterator(); it != l.getEndIterator(); ++it) {
            xml->value().push_back((*it).toString());
        }
    }
    return xml;
}

StreamProperty SPL::fromXML(streamPropertyType const& xml)
{
    const string& name = xml.name();
    const string& type = xml.type();
    int32 dummy;
    ValueHandle v(dummy);
    if (type == "int64") {
        v = ValueHandle(*new int64);
    } else if (type == "float64") {
        v = ValueHandle(*new float64);
    } else if (type == "rstring") {
        v = ValueHandle(*new rstring);
    } else if (type == "list<int64>") {
        v = ValueHandle(*new list<int64>());
    } else if (type == "list<float64>") {
        v = ValueHandle(*new list<float64>());
    } else if (type == "list<rstring>") {
        v = ValueHandle(*new list<rstring>());
    } else {
        assert(!"cannot happen");
    }
    if (v.getMetaType() != Meta::Type::LIST) {
        istringstream istr(*xml.value().begin());
        istr.imbue(locale::classic());
        v.deserialize(istr);
    } else {
        List& l = v;
        ValueHandle vh = l.createElement();
        streamPropertyType::value_const_iterator it;
        for (it = xml.value().begin(); it != xml.value().end(); ++it) {
            istringstream istr(*it);
            istr.imbue(locale::classic());
            vh.deserialize(istr);
            l.pushBack(vh);
        }
        vh.deleteValue();
    }
    return StreamProperty(name, v);
}
