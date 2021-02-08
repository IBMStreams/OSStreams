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

#include <SAM/SAMInterfaceTypes.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyXML.h>

using namespace std;
using namespace SPL;
using namespace xmlns::prod::streams::application;

auto_ptr<streamPropertiesType> SPL::toXML(const StreamPropertyCollection& prop)
{
    auto_ptr<streamPropertiesType> xml(new streamPropertiesType());
    for (StreamPropertyCollection::const_iterator it = prop.begin(); it != prop.end(); ++it) {
        xml->property().push_back(toXML(*it));
    }
    return xml;
}

void SPL::fromXML(streamPropertiesType const& xml, StreamPropertyCollection& propCol)
{
    propCol.clear();
    for (streamPropertiesType::property_const_iterator it = xml.property().begin();
         it != xml.property().end(); ++it) {
        propCol.push_back(fromXML(*it));
    }
}
