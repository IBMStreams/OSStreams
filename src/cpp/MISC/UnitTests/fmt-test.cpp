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

#include <UTILS/Formatter.h>

using namespace std;
UTILS_NAMESPACE_USE;

int main()
{
    int i = 9;

    cout << Format("%2.2d", i) << endl;
    cout << Format("%2.2f", 2.4) << endl;
    cout << Format("%5.5s", "bob12345") << endl;
    string s("bob12345");
    cout << Format("%5.5s", s) << endl;

    return 0;
}
