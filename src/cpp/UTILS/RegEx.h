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
//       RegEx.h
//
// Purpose-
//       Put a simplier interface on regexec(3)
//
//----------------------------------------------------------------------------

#ifndef _REGEX_H_
#define _REGEX_H_

#include <regex.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include <UTILS/DistilleryException.h>
#include <UTILS/Mutex.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class RegEx
{
  protected:
    Mutex _mutex;
    std::string _pattern;
    int _flags;
    regex_t _re;
    bool _compiled;

    void compile(void);
    int match(const char* str, int nmatch, regmatch_t* pmatch, int flags) const;

  public:
    enum
    {

        SR_GLOBAL_REPLACE = 0x1,
        SR_CASE_INSENSITIVE = 0x2,
        SR_NEWLINE = 0x4,
        SR_FIXED = 0x8,

    };

    RegEx(const std::string& pattern = "", int flags = REG_EXTENDED);
    RegEx(const char* pattern, int flags = REG_EXTENDED);
    RegEx(const RegEx& other);
    ~RegEx(void);
    RegEx& operator=(const RegEx& other);

    int match(const std::string& str,
              std::vector<std::string>& result,
              int flags = 0,
              int match_size = 1024) const;
    int match(const std::string& str, int flags = 0) const;

    static std::string glob2regex(const std::string& glob_pattern,
                                  char match_many = '*',
                                  char match_one1 = '?',
                                  char match_one2 = '+',
                                  bool filename_glob = true);

    friend std::ostream& operator<<(std::ostream& strm, const RegEx& re);

    static std::string search_replace(const std::string& pattern, const std::string& str);
    static std::string search_replace(const std::string& search,
                                      const std::string& replace,
                                      const std::string& options,
                                      const std::string& str);
    static bool search_replace_one(RegEx& search,
                                   const std::string& replace,
                                   unsigned int options,
                                   const std::string& str,
                                   std::string& res,
                                   std::string& remainder);
    static std::string search_replace_recur(RegEx& search,
                                            const std::string& replace,
                                            unsigned int options,
                                            const std::string& str);
    static std::string search_replace(RegEx& search,
                                      const std::string& replace,
                                      unsigned int options,
                                      const std::string& str);
};

std::ostream& operator<<(std::ostream& strm, const RegEx& re);

DECL_EXCEPTION(UTILS_NAMESPACE, RegEx, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, BadRegEx, RegEx);
DECL_EXCEPTION(UTILS_NAMESPACE, BadGlobRegEx, RegEx);
DECL_EXCEPTION(UTILS_NAMESPACE, BadSearchReplace, RegEx);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
