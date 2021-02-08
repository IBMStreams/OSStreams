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

#ifndef SIMPLE_JSON_SERIALIZER_H
#define SIMPLE_JSON_SERIALIZER_H

#include <UTILS/SimpleJsonObj.h>
#include <UTILS/UTILSTypes.h>
#include <boost/property_tree/ptree.hpp>

UTILS_NAMESPACE_BEGIN

/// This class stores some properties and print out as json format
class SimpleJsonSerializer
{
  public:
    // Create a Json object of this object
    // @param pt property_tree object
    virtual void toJson(boost::property_tree::ptree& pt) const;
    virtual void toSimpleJson(SimpleJsonObj& pt) const;

    // Serialize Json object to a string
    // @param jsonStr string to hold the serialized Json object
    // @param pretty flag to see if serialized string preserve newline, space, etc.
    virtual void serializeJson(std::string& jsonStr, bool pretty = false) const;
    virtual void serializeSimpleJson(std::string& jsonStr, bool pretty = false) const;

    // Deserialize string to Json object
    // @param jsonStr input string object to deserialize from.
    virtual void deserializeJson(const std::string& jsonStr);
};

UTILS_NAMESPACE_END

#endif
