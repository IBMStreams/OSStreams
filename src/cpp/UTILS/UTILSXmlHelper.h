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
 * SAMXmlHelper.h
 *
 */

#ifndef UTILSXMLHELPER_H
#define UTILSXMLHELPER_H

#include <UTILS/DistilleryException.h>
#include <UTILS/MessageRecordingTypes.h>
#include <UTILS/UTILSTypes.h>
#include <string.h>

//#define UTILS_RUNTIME_NAMESPACE_BEGIN namespace xmlns{namespace prod{namespace streams{namespace
// runtime{namespace utils{ #define UTILS_RUNTIME_NAMESPACE_END } } } } } #define UTILS_RUNTIME_NS
//::xmlns::prod::streams::runtime::utils #define UTILS_RUNTIME_NS_USE using namespace
// UTILS_RUNTIME_NS;

#define UTILS_XSD_FLAGS                                                                            \
    static_cast<xml_schema::flags>(xml_schema::flags::dont_validate |                              \
                                   xml_schema::flags::dont_initialize)

UTILS_NAMESPACE_BEGIN

class UTILSXmlHelper
{

  public:
    /// Convert an MessageRecordingConfigurationType object to its corresponding XML string
    /// @param config the object
    static std::string toString(
      const UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& config);

    /// Create an MessageRecordingConfigurationType object from an XML representation string
    /// @param config the object
    /// @param s the XML string representation
    static void fromString(const std::string& s,
                           UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& config);
};

DECL_EXCEPTION(UTILS_NAMESPACE, XMLSerialization, Utils);

UTILS_NAMESPACE_END

#endif /* UTILSXMLHELPER_H */
