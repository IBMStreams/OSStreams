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
#include <UTILS/SupportFunctions.h>

using namespace std;
UTILS_NAMESPACE_USE

#define MAX_LINE_SIZE 4095

ConsoleReader::ConsoleReader(void)
  : _th(NULL)
  , _line_buffer(NULL)
{
    if (get_environment_variable("USE_TECLA", 1) != 0) {
        _th = new LibTeclaHelper;
    } else {
        _line_buffer = new char[MAX_LINE_SIZE + 1];
    }
}

ConsoleReader::~ConsoleReader(void)
{
    delete _th;
}

char* ConsoleReader::getCommandPTR(const string& promptMsg) const
{
    if (_th) {
        return _th->getCommandPTR(promptMsg);
    } else {
        char* ptr = _line_buffer;

        int c = EOF;

        printf("%s", promptMsg.c_str());
        fflush(stdout);

        while (1) {
            c = getchar();

            if (c == EOF || c == '\n') {
                break;
            }

            *ptr = c;
            ptr++;

            if (ptr > _line_buffer + MAX_LINE_SIZE) {
                break;
            }
        }

        if (c == EOF && ptr == _line_buffer) {
            return NULL;
        }

        *ptr = 0;

        return _line_buffer;
    }
}
