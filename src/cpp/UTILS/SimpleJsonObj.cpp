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

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SimpleJsonObj.h>
#include <sstream>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

SimpleJsonObj::SimpleJsonObj(const string& json)
{
    initJson(json);
}

void SimpleJsonObj::initJson(const string& json)
{
    // SPCDBG(L_DEBUG, "Enter with json string " << json, "SimpleJsonObj");
    // we can not split the string as content may include any char,
    // but need to walk through the string
    string key("");
    vector<string> strChildren;
    vector<SimpleJsonObj> jsonChildren;
    int start = 0;
    int startJson = 0;
    char preChar = ' ';
    bool inKey = false;
    bool inValue = false;
    bool inJson = false;
    bool inChildren = false;
    bool isValueChildren = false;
    int vectorTrack = 0;
    int jsonTrack = 0;
    for (unsigned i = 0; i < json.size(); ++i) {
        // keep history of the char ahead of current char.
        if (i > 0 && (json[i - 1] != ' ' || json[i - 1] != '\n' || json[i - 1] != '\t')) {
            preChar = json[i - 1];
        }

        if (json[i] == ' ' || (json[i] == '{' && key.length() == 0)) {
            // skip space and the beginning of '{'
            continue;
        }

        // for key or values
        if (!inJson) {
            if (json[i] == '"' && preChar != '\\') {
                // for key
                if (preChar != ':' && !inChildren && !inValue) {
                    if (!inKey) {
                        // Beginning of the key
                        start = i;
                        inKey = true;
                    } else {
                        // End of the key.
                        inKey = false;
                        key = json.substr(start + 1, i - start - 1);
                        // SPCDBG(L_TRACE, "Key is " << key, "SimpleJsonObj");
                    }
                } else {
                    // For value
                    if (!inValue) {
                        // Beginning of the value
                        start = i;
                        inValue = true;
                        if (inChildren) {
                            isValueChildren = true;
                        }
                    } else {
                        // End of the value
                        inValue = false;
                        string value = json.substr(start + 1, i - start - 1);
                        // SPCDBG(L_TRACE, "Value is " << value, "SimpleJsonObj");
                        if (inChildren) {
                            // If it belong to list of values, add to vector.
                            strChildren.push_back(value);
                        } else {
                            // If just key/value pair, directly set it.
                            _properties[key] = value;
                        }
                    }
                }
            } else if (json[i] == '[' && !inKey && !inValue) {
                if (!inChildren) {
                    // start of a list of values
                    inChildren = true;
                    vectorTrack = 1;
                } else {
                    // Already inside the list, no need to count it.
                    vectorTrack++;
                }
            } else if (json[i] == ']' && !inKey && !inValue) {
                if (inChildren) {
                    vectorTrack--;
                    if (vectorTrack == 0) {
                        // End of the list
                        if (isValueChildren) {
                            _childrenArrayStr[key] = strChildren;
                            strChildren.clear();
                            isValueChildren = false;
                        } else {
                            _childrenArrayJson[key] = jsonChildren;
                            jsonChildren.clear();
                        }
                        inChildren = false;
                    }
                }
            } else if (json[i] == '{' && !inKey && !inValue) {
                inJson = true;
                jsonTrack = 1;
                startJson = i;
            }
        } else {
            // Track for key and value to make sure "{" and "}" we track below
            // NOT inside a key or value.
            if (json[i] == '"' && preChar != '\\') {
                // for key
                if (preChar != ':' && !inValue) {
                    if (!inKey) {
                        inKey = true;
                    } else {
                        inKey = false;
                    }
                } else {
                    // For value
                    if (!inValue) {
                        inValue = true;
                    } else {
                        inValue = false;
                    }
                }
            }

            if (json[i] == '{' && !inKey && !inValue) {
                jsonTrack++;
            } else if (json[i] == '}' && !inKey && !inValue) {
                jsonTrack--;
                if (jsonTrack == 0) {
                    string subJsonStr = json.substr(startJson + 1, i - startJson - 1);
                    // SPCDBG(L_TRACE, "subJson is " << subJsonStr, "SimpleJsonObj");
                    SimpleJsonObj subJson(subJsonStr);
                    if (inChildren) {
                        jsonChildren.push_back(subJson);
                    } else {
                        _childrenJson[key] = subJson;
                    }
                }
            }
        }
    }
    // SPCDBG(L_DEBUG, "Exit with json " << writeJson(), "SimpleJsonObj");
}

string SimpleJsonObj::jsonSanitizeMsg(const string& str)
{
    string sanitized;
    for (unsigned i = 0; i < str.size(); ++i) {
        if ((str[i] == '\\' && (i > 0 && str[i - 1] != '\\') &&
             ((i + 1) < str.size() && str[i + 1] != '\\' && str[i + 1] != '"' &&
              str[i + 1] != '/')) ||
            ((str[i] == '"' || str[i] == '/') && (i == 0 || (i > 0 && str[i - 1] != '\\')))) {
            sanitized += "\\";
        }
        if (str[i] == '\n') {
            // If newline at the middle of content, replacing it with space.
            // Otherwise, ignore.
            if (i < (str.size() - 1)) {
                sanitized += ' ';
            }
        } else {
            sanitized += str[i];
        }
    }
    SPCDBG(L_DEBUG, "Exit with string " << sanitized, "SimpleJsonObj");
    return sanitized;
}

string SimpleJsonObj::removeEscapingChars(const string& str)
{
    string sanitized;
    // removing escaping '\' and '\n' that not within a value
    bool insideValue = false;
    for (unsigned i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && (i + 1) < str.size()) {
            // SPCDBG(L_DEBUG, "Current char:" << str[i] << " i+1:" << str[i+1], "SimpleJsonObj");
            // if(i > 0) {
            //  SPCDBG(L_DEBUG, "pre-char:" << str[i-1], "SimpleJsonObj");
            //}
            if ((str[i + 1] == '\\' || str[i + 1] == '"' || str[i + 1] == '/')) {
                // skip this '\' as it is escaping char.
                i = i + 1;
            } else if (str[i + 1] == 'n' && !insideValue) {
                i = i + 2; // skip "\n" that not from an internal value
            }
        }
        if (str[i] == '"') {
            if (insideValue) {
                insideValue = false;
            } else {
                insideValue = true;
            }
        }
        sanitized += str[i];
    }
    SPCDBG(L_DEBUG, "Exit with string " << sanitized, "SimpleJsonObj");
    return sanitized;
}

void SimpleJsonObj::put(const string& key, const string& value)
{
    _properties[key] = value;
}

string SimpleJsonObj::get(const string& key) const
{
    if (_properties.find(key) != _properties.end()) {
        return _properties.at(key);
    } else {
        return "";
    }
}

const map<string, string>& SimpleJsonObj::getAll() const
{
    return _properties;
}

void SimpleJsonObj::addChild(const string& key, const SimpleJsonObj& child)
{
    _childrenJson[key] = child;
}

bool SimpleJsonObj::hasJsonChild(const string& key) const
{
    return _childrenJson.find(key) != _childrenJson.end();
}

const SimpleJsonObj& SimpleJsonObj::getJsonChild(const string& key) const
{
    return _childrenJson.at(key);
}

void SimpleJsonObj::addChild(const string& key, const vector<string>& child)
{
    _childrenArrayStr[key] = child;
}

bool SimpleJsonObj::hasStringChild(const string& key) const
{
    return _childrenArrayStr.find(key) != _childrenArrayStr.end();
}

const vector<string>& SimpleJsonObj::getStringChild(const string& key) const
{
    return _childrenArrayStr.at(key);
}

void SimpleJsonObj::getStringChild(const string& key, vector<string>& vals)
{
    vals.insert(vals.begin(), _childrenArrayStr.at(key).begin(), _childrenArrayStr.at(key).end());
}

void SimpleJsonObj::addChild(const string& key, const vector<SimpleJsonObj>& child)
{
    _childrenArrayJson[key] = child;
}

bool SimpleJsonObj::hasJsonListChild(const string& key) const
{
    return _childrenArrayJson.find(key) != _childrenArrayJson.end();
}

const vector<SimpleJsonObj>& SimpleJsonObj::getJsonListChild(const string& key) const
{
    return _childrenArrayJson.at(key);
}

string SimpleJsonObj::writeJson(const std::string& iniSpace, bool pretty) const
{
    string initSpace = iniSpace;
    string curSpace = initSpace + "    ";
    string nextSpace = curSpace + "    ";
    if (!pretty) {
        initSpace = "";
        curSpace = "";
        nextSpace = "";
    }
    int propSize = _properties.size();
    int childArrayStrSize = _childrenArrayStr.size();
    int childJsonSize = _childrenJson.size();
    int childArrayJsonSize = _childrenArrayJson.size();
    int track = 0;

    stringstream ss;
    // Initial
    ss << initSpace << "{";
    if (pretty) {
        ss << "\n";
    }

    // All properties
    for (map<string, string>::const_iterator it1 = _properties.begin(); it1 != _properties.end();
         it1++) {
        track++;
        ss << curSpace << "\"" << it1->first << "\": "
           << "\"" << it1->second << "\"";
        if (track < propSize) {
            ss << ",";
            if (pretty) {
                ss << "\n";
            }
        }
    }
    if (propSize > 0 && (childArrayStrSize + childJsonSize + childArrayJsonSize) > 0) {
        ss << ",";
        if (pretty) {
            ss << "\n";
        }
    }

    // All string arrays
    track = 0;
    for (map<string, vector<string> >::const_iterator it2 = _childrenArrayStr.begin();
         it2 != _childrenArrayStr.end(); it2++) {
        track++;
        ss << curSpace << "\"" << it2->first << "\": "
           << "[";
        if (pretty) {
            ss << "\n";
        }
        vector<string> values = it2->second;
        int valSize = values.size();
        for (int i = 0; i < valSize; i++) {
            ss << nextSpace << "\"" << values[i] << "\"";
            if ((i + 1) < valSize) {
                ss << ",";
            }
            if (pretty) {
                ss << "\n";
            }
        }
        ss << curSpace << "]";
        if (track < childArrayStrSize) {
            ss << ",";
            if (pretty) {
                ss << "\n";
            }
        }
    }
    if (childArrayStrSize > 0 && (childJsonSize + childArrayJsonSize) > 0) {
        ss << ",";
        if (pretty) {
            ss << "\n";
        }
    }

    // all child json object
    track = 0;
    for (map<string, SimpleJsonObj>::const_iterator it3 = _childrenJson.begin();
         it3 != _childrenJson.end(); it3++) {
        track++;
        ss << curSpace << "\"" << it3->first << "\": " << it3->second.writeJson(curSpace, pretty);
        if (track < childJsonSize) {
            ss << ",";
            if (pretty) {
                ss << "\n";
            }
        }
    }
    if (childJsonSize > 0 && childArrayJsonSize > 0) {
        ss << ",";
        if (pretty) {
            ss << "\n";
        }
    }

    // All json arrays
    track = 0;
    for (map<string, vector<SimpleJsonObj> >::const_iterator it4 = _childrenArrayJson.begin();
         it4 != _childrenArrayJson.end(); it4++) {
        track++;
        ss << curSpace << "\"" << it4->first << "\": "
           << "[";
        if (pretty) {
            ss << "\n";
        }
        vector<SimpleJsonObj> values = it4->second;
        int valSize = values.size();
        for (int i = 0; i < valSize; i++) {
            ss << values[i].writeJson(nextSpace, pretty);
            if ((i + 1) < valSize) {
                ss << ",";
            }
            if (pretty) {
                ss << "\n";
            }
        }
        ss << curSpace << "]";
        if (track < childArrayJsonSize) {
            ss << ",";
            if (pretty) {
                ss << "\n";
            }
        }
    }
    if (pretty) {
        ss << "\n";
    }

    // end
    ss << initSpace << "}";
    return ss.str();
}
