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

#include <UTILS/PathSerializer.h>
#include <iostream>
#include <sstream>

using namespace std;
UTILS_NAMESPACE_USE

string PathSerializer::serialize(map<string, string>& in)
{
    stringstream ss;

    for (map<string, string>::iterator itr = in.begin(); itr != in.end(); itr++) {
        string key = itr->first;
        string value = itr->second;
        string item = key;
        if (0 != key.compare(value)) {
            item = item + "=" + value;
        }
        ss << item << endl;
    }

    return ss.str();
}

map<string, string> PathSerializer::unserialize(const string& in)
{
    map<string, string> out;
    istringstream is(in);
    string line;

    while (getline(is, line, '\n')) {
        size_t eq = line.find('=');
        if (eq > 0) {
            string key = line.substr(0, eq);
            string value = line.substr(eq + 1);
            out[key] = value;
        } else if (eq == string::npos) {
            out[line] = line;
        }
    }

    return out;
}
