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

#ifndef SIMPLE_JSON_OBJ_H
#define SIMPLE_JSON_OBJ_H

#include <UTILS/UTILSTypes.h>
#include <map>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN

/// This class stores some properties and print out as json format
class SimpleJsonObj
{
  public:
    /// Default Constructor
    SimpleJsonObj() {}

    /// Constructor with json formatted content
    /// @param json json formatted content
    SimpleJsonObj(const std::string& json);

    /// Adding a property
    /// @param key property key
    /// @param value property value
    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
    const std::map<std::string, std::string>& getAll() const;

    /// Adding a child object
    /// @param key child key
    /// @param child child object
    void addChild(const std::string& key, const SimpleJsonObj& child);
    bool hasJsonChild(const std::string& key) const;
    const SimpleJsonObj& getJsonChild(const std::string& key) const;
    void addChild(const std::string& key, const std::vector<std::string>& child);
    bool hasStringChild(const std::string& key) const;
    const std::vector<std::string>& getStringChild(const std::string& key) const;
    void getStringChild(const std::string& key, std::vector<std::string>& vals);
    void addChild(const std::string& key, const std::vector<SimpleJsonObj>& child);
    bool hasJsonListChild(const std::string& key) const;
    const std::vector<SimpleJsonObj>& getJsonListChild(const std::string& key) const;

    /// Write content to a json formatted string
    std::string writeJson(const std::string& initSpace = "", bool pretty = true) const;

    /// Doing following update for the string content:
    /// 1. escaping ", and /, since json parsing will fail with them in the value content.
    /// 2. removing newline "\n" as it will invalid Java side parsing.
    /// @param str string to be added escaping chars
    /// @return the updated string
    static std::string jsonSanitizeMsg(const std::string& str);

    static std::string removeEscapingChars(const std::string& str);

  private:
    void initJson(const std::string& json);

    std::map<std::string, std::string> _properties;
    std::map<std::string, SimpleJsonObj> _childrenJson;
    std::map<std::string, std::vector<std::string> > _childrenArrayStr;
    std::map<std::string, std::vector<SimpleJsonObj> > _childrenArrayJson;
};

UTILS_NAMESPACE_END

#endif
