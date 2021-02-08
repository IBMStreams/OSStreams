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

//----------------------------------------------------------------------------
//
// Title-
//       BacktraceDumper.h
//
// Purpose-
//       Dump the current calling stack of the process -- Might only work on
//       linux
//
// Change date-
//       2004/11/15
//
//----------------------------------------------------------------------------
#ifndef BACKTRACEDUMPER_H_INCLUDED
#define BACKTRACEDUMPER_H_INCLUDED

#ifdef __i386__
// seems like libiberty is not compiled with fPIC, which make PowerPC and x86-64 unhappy
// comment out bfd.h inclusion
// #include <bfd.h>
#endif

#include <UTILS/UTILSTypes.h>
#include <iostream>

UTILS_NAMESPACE_BEGIN

//----------------------------------------------------------------------------
//
// Class-
//       BacktraceDumper
//
// Purpose-
//       Define the Stream Data Object header
//
//----------------------------------------------------------------------------

#define STACK_TRACE_SIZE 1024
#define MAX_STACK_TRACE_SIZE (16 * 1024)

class BacktraceDumper
{
  protected:
#ifdef DUMP_LINE_NUMBER

    typedef struct _find_context_t
    {
        asymbol** syms;
        int found;
        unsigned long pc;
        unsigned int line;
        const char* filename;
    } find_context_t;

    static void findAddress(bfd* abfd, asection* section, void* data);

#endif

  public:
    static int dump(std::ostream& s, bool add_lines = false, int skip = 1);
    static int dump(char* buffer,
                    int max_size,
                    int& final_size,
                    bool add_lines = false,
                    int skip = 1);
    static int dump(int fd, bool add_lines, int skip);
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
