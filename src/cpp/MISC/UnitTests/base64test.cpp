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

#include <UTILS/Base64.h>
#include <UTILS/SBuffer.h>
#include <cassert>
#include <iostream>
#include <string>

UTILS_NAMESPACE_USE
using namespace std;

int main()
{
    SBuffer in;
    in.addSTLString("IBM Rocks");
    in.addInt32(2006);
    in.addFloat(3.14);
    string outstr;
    Base64::encode(in, outstr);
    cout << "Encoded value [" << outstr << "]" << endl;
    SBuffer decoded;
    Base64::decode(outstr, decoded);
    string str(decoded.getSTLString());
    int year = decoded.getInt32();
    float pi = decoded.getFloat();
    cout << "String: [" << str << "] Year: [" << year << "] PI: [" << pi << "]" << endl;
    assert(str == "IBM Rocks" && year == 2006 && pi > 3.1 && pi < 3.2);
    return EXIT_SUCCESS;
}
