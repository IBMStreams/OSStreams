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

#include <UTILS/ConsoleReader.h>
#include <UTILS/UTILSTypes.h>
#include <string>
#include <strings.h>

UTILS_NAMESPACE_USE
using namespace std;

int main()
{
    ConsoleReader console;
    while (1) {
        char* line = console.getCommandPTR("jme> ");
        if (line == NULL) {
            break;
        }
        if (strncasecmp(line, "quit", 5) == 0) {
            break;
        }
    }
    return 0;
}
