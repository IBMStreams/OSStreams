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

// Class Description:
//
// Implements a wrapper around different console readers

#ifndef __CONSOLE_READER_H__
#define __CONSOLE_READER_H__

#include <UTILS/LibTeclaHelper.h>
#include <UTILS/UTILSTypes.h>
#include <string>

UTILS_NAMESPACE_BEGIN

class ConsoleReader
{
  public:
    ConsoleReader();
    virtual ~ConsoleReader(void);
    virtual char* getCommandPTR(const std::string& promptMsg) const;

  private:
    LibTeclaHelper* _th;
    char* _line_buffer;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
