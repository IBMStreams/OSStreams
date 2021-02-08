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

#include <UTILS/MessageRecordingTypes.h>
#include <UTILS/SPCException.h>
#include <UTILS/UTILSMessages.h>
#include <UTILS/UTILSXmlHelper.h>
#include <cassert>
#include <sstream>

UTILS_NAMESPACE_USE
UTILS_RUNTIME_NS_USE
using namespace std;
using namespace UTILS_RUNTIME_NS::mr;

void UTILSXmlHelper::fromString(const string& xml, MessageRecordingConfigurationType& obj)
{
    try {
        stringstream ss(xml);
        auto_ptr<MessageRecordingConfigurationType> datavar(
          messageRecordingConfiguration(ss, UTILS_XSD_FLAGS));
        obj = *datavar;
    } catch (const xml_schema::exception& e) {
        THROW(XMLSerialization,
              "Failed to deserialize object of 'MessageRecordingConfigurationType'"
                << "from string : " << xml << " with failure: " << e);
    } catch (std::exception& e) {
        THROW(XMLSerialization,
              "Failed to deserialize object of 'MessageRecordingConfigurationType'"
                << "from string : " << xml << " with failure: " << e.what());
    } catch (...) {
        THROW(XMLSerialization,
              "Failed to deserialize object of 'MessageRecordingConfigurationType'"
                << "from string : " << xml << " with failure: unknown error.");
    }
}

string UTILSXmlHelper::toString(const MessageRecordingConfigurationType& obj)
{
    stringstream ss;
    try {
        xml_schema::namespace_infomap map;
        map["xs"].name = "http://www.w3.org/2001/XMLSchema";
        map["utils"].name = "http://www.ibm.com/xmlns/prod/streams/runtime/utils";
        messageRecordingConfiguration(ss, obj, map, "UTF-8", xml_schema::flags::dont_initialize);
    } catch (const xml_schema::exception& e) {
        THROW(XMLSerialization, "Failed to serialize object of "
                                  << "'MessageRecordingConfigurationType' with failure: " << e);
    } catch (std::exception& e) {
        THROW(XMLSerialization, "Failed to serialize object of "
                                  << "'MessageRecordingConfigurationType' with failure: "
                                  << e.what());
    } catch (...) {
        THROW(XMLSerialization,
              "Failed to serialize object of "
                << "'MessageRecordingConfigurationType' with failure: unknown error.");
    }
    return ss.str();
}

IMPL_EXCEPTION(UTILS_NAMESPACE, XMLSerialization, Utils);
