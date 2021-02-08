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

#ifndef SPL_RUNTIME_OPERATOR_PORT_STREAM_PROPERTY_XML_H
#define SPL_RUNTIME_OPERATOR_PORT_STREAM_PROPERTY_XML_H

#include <SPL/Runtime/Operator/Port/StreamProperty.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyCollection.h>

namespace xmlns {
namespace prod {
namespace streams {
namespace application {
class streamPropertyType;
class streamPropertiesType;
}
}
}
}

namespace SPL {
/// Convert a StreamProperty to the corresponding XML
/// @param prop  Input StreamProperty
/// @return auto_ptr to converted XML
std::auto_ptr<xmlns::prod::streams::application::streamPropertyType> toXML(
  const StreamProperty& prop);
/// Convert a streamPropertyType XML to a StreamProperty
/// @param xml  Input xml for StreamProperty
/// @return xml converted to StreamProperty
StreamProperty fromXML(xmlns::prod::streams::application::streamPropertyType const& xml);

/// Convert a StreamPropertyCollection to the corresponding XML
/// @param prop  Input StreamPropertyCollection
/// @return auto_ptr to converted XML
std::auto_ptr<xmlns::prod::streams::application::streamPropertiesType> toXML(
  const StreamPropertyCollection& prop);
/// Convert a streamPropertyType XML to a StreamPropertyCollection
/// @param xml  Input xml for StreamPropertyCollection
/// @param propCol StreamProperty collection set to XML input
void fromXML(xmlns::prod::streams::application::streamPropertiesType const& xml,
             StreamPropertyCollection& propCol);

};

#endif /* SPL_RUNTIME_OPERATOR_PORT_STREAM_PROPERTY_XML_H */
